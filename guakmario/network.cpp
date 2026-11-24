#include "network.h"
#include "config.h"
#include "player.h"
#include "game.h"
#include "item.h"
#include "map.h"
#include "sound.h"
#include <queue>

// ============================================================
//  전역 변수 정의
// ============================================================

NetContext g_net       = {};
int        g_sharedLives = 3;


// ============================================================
//  수신 큐 / 크리티컬 섹션 (A-4, A-5에서 사용)
// ============================================================

struct ReceivedPacket {
    std::vector<uint8_t> bytes;
    int senderPlayerId;
    bool disconnected;
};

static std::queue<ReceivedPacket> g_recvQueue;
static CRITICAL_SECTION          g_recvCS;
static bool                      g_recvCSInit = false;
static std::vector<HANDLE>       g_recvThreads;  // 메인 스레드에서만 접근

struct RecvThreadParam {
    SOCKET sock;
    int    playerId;  // 이 소켓 상대의 ID — 호스트 쪽: 1 또는 2, 클라이언트 쪽: 0 (호스트)
};

static bool RecvFull(SOCKET sock, std::vector<uint8_t>& out);
static DWORD WINAPI RecvThread(LPVOID param);
static void SendFull(SOCKET sock, const void* data, int size);
static void RecvQueuePush(ReceivedPacket packet);
static bool RecvQueuePop(ReceivedPacket& out);

// 소켓 하나를 맡을 RecvThread 생성 (메인 스레드에서만 호출)
static bool NetStartRecvThread(SOCKET sock, int playerId)
{
    auto* tp = new RecvThreadParam{ sock, playerId };
    HANDLE thread = CreateThread(nullptr, 0, RecvThread, tp, 0, nullptr);
    if (thread == nullptr)
    {
        delete tp;
        return false;
    }
    g_recvThreads.push_back(thread);
    return true;
}

// 소켓을 닫아 blocking recv를 깨운 뒤 RecvThread 종료를 기다리고 수신 큐를 비움
static void NetCloseResources()
{
    for (int i = 0; i < 2; i++)
    {
        if (g_net.clientSockets[i] != INVALID_SOCKET)
        {
            closesocket(g_net.clientSockets[i]);
            g_net.clientSockets[i] = INVALID_SOCKET;
        }
    }
    if (g_net.hostSocket != INVALID_SOCKET)
    {
        closesocket(g_net.hostSocket);
        g_net.hostSocket = INVALID_SOCKET;
    }
    if (g_net.listenSock != INVALID_SOCKET)
    {
        closesocket(g_net.listenSock);
        g_net.listenSock = INVALID_SOCKET;
    }

    for (HANDLE thread : g_recvThreads)
    {
        WaitForSingleObject(thread, INFINITE);  // RecvThread 종료 대기
        CloseHandle(thread);
    }
    g_recvThreads.clear();

    // 스레드가 모두 끝난 뒤라 잠금 불필요 — 이전 방의 패킷 제거
    std::queue<ReceivedPacket>().swap(g_recvQueue);
}


// ============================================================
//  10/29 — WSAStartup + listen 소켓 생성 + bind + listen
// ============================================================

void NetFetchLocalIp()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    strcpy_s(g_net.localIp, "127.0.0.1");
    char hostname[256] = {};
    if (gethostname(hostname, sizeof(hostname)) == 0)
    {
        addrinfo hints = {};
        hints.ai_family   = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        addrinfo* res = nullptr;
        if (getaddrinfo(hostname, nullptr, &hints, &res) == 0)
        {
            for (addrinfo* p = res; p != nullptr; p = p->ai_next)
            {
                sockaddr_in* sin = (sockaddr_in*)p->ai_addr;
                inet_ntop(AF_INET, &sin->sin_addr, g_net.localIp, sizeof(g_net.localIp));
                break;
            }
            freeaddrinfo(res);
        }
    }
}

bool NetInit(bool isHost, const char* hostIp)
{
    // 이전 방의 소켓과 스레드가 남아 있으면 먼저 정리 (재진입 대비)
    if (g_recvCSInit)
        NetCloseResources();

    g_net.isHost          = isHost;
    g_net.myPlayerId      = isHost ? 0 : -1;   // 클라이언트는 ACK 받은 후 설정
    g_net.connectedCount  = isHost ? 1 : 0;    // 호스트 자신 포함
    g_net.lobbyMask       = isHost ? 1 : 0;    // 클라이언트는 EVT_LOBBY_STATUS를 받으면 설정
    g_net.waitingForItemPickup = false;
    g_net.listenSock      = INVALID_SOCKET;
    g_net.clientSockets[0] = INVALID_SOCKET;
    g_net.clientSockets[1] = INVALID_SOCKET;
    g_net.hostSocket      = INVALID_SOCKET;

    // 수신 큐 크리티컬 섹션 초기화 (최초 1회)
    if (!g_recvCSInit)
    {
        InitializeCriticalSection(&g_recvCS);
        g_recvCSInit = true;
    }

    // localIp가 아직 없으면 조회
    if (g_net.localIp[0] == '\0')
        NetFetchLocalIp();

    if (isHost)
    {
        g_net.listenSock = socket(AF_INET, SOCK_STREAM, 0);
        if (g_net.listenSock == INVALID_SOCKET) return false;

        int opt = 1;
        setsockopt(g_net.listenSock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

        sockaddr_in addr = {};
        addr.sin_family      = AF_INET;
        addr.sin_port        = htons(NET_PORT);
        addr.sin_addr.s_addr = INADDR_ANY;

        // 포트 사용 중 등으로 실패하면 방을 만들 수 없음
        if (bind(g_net.listenSock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR ||
            listen(g_net.listenSock, 2) == SOCKET_ERROR)    // backlog 2 (클라이언트 최대 2명)
        {
            closesocket(g_net.listenSock);
            g_net.listenSock = INVALID_SOCKET;
            return false;
        }

        // listen 소켓만 non-blocking — NetPollAccept에서 매 프레임 폴링
        u_long mode = 1;
        ioctlsocket(g_net.listenSock, FIONBIO, &mode);
        return true;
    }

    // ---- 클라이언트: 호스트에 connect ----
    if (hostIp == nullptr || hostIp[0] == '\0') return false;

    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(NET_PORT);
    if (inet_pton(AF_INET, hostIp, &addr.sin_addr) != 1)
        return false;  // IP 형식 오류 (예: "192.168.0")

    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET) return false;

    // blocking connect — 연결되거나 실패할 때까지 메인 스레드가 대기
    // (방이 없으면 바로 실패, 응답 없는 IP면 OS 기본 대기 시간 동안 창이 멈춤)
    if (connect(s, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
    {
        closesocket(s);
        return false;  // 호스트가 없거나 방이 안 열림
    }

    g_net.hostSocket = s;

    // 호스트에서 오는 패킷 수신 — 끊기면 EVT_DISCONNECT(param 0)가 큐에 들어옴
    if (!NetStartRecvThread(s, 0))
    {
        closesocket(s);
        g_net.hostSocket = INVALID_SOCKET;
        return false;
    }
    // myPlayerId는 PKT_CONNECT_ACK을 받으면 설정됨 (NetPollPackets)
    return true;
}


// ============================================================
//  PKT_CONNECT_ACK 전송 헬퍼 (내부용)
// ============================================================

// 호스트: 현재 접속 현황을 비트로 만들어 모든 클라이언트에게 알린다 (접속 · 끊김 때마다)
static void BroadcastLobbyStatus()
{
    uint8_t mask = 1;  // bit0 = 호스트
    for (int i = 0; i < 2; i++)
        if (g_net.clientSockets[i] != INVALID_SOCKET) mask |= (uint8_t)(1 << (i + 1));
    g_net.lobbyMask = mask;

    uint8_t buf[sizeof(PacketHeader) + sizeof(GameEventData)];
    auto* hdr     = (PacketHeader*)buf;
    hdr->size     = (uint16_t)sizeof(buf);
    hdr->type     = PKT_GAME_EVENT;
    hdr->playerId = 0;

    auto* evt      = (GameEventData*)(buf + sizeof(PacketHeader));
    evt->eventType = EVT_LOBBY_STATUS;
    evt->param     = mask;

    SendToAll(buf, (int)sizeof(buf));
}

bool NetIsPlayerConnected(int playerId)
{
    if (playerId < 0 || playerId > 2) return false;
    return (g_net.lobbyMask & (1 << playerId)) != 0;
}

static void SendConnectAck(int assignedId)
{
    uint8_t buf[sizeof(PacketHeader) + sizeof(ConnectAckData)];

    auto* hdr  = (PacketHeader*)buf;
    hdr->size     = (uint16_t)sizeof(buf);
    hdr->type     = PKT_CONNECT_ACK;
    hdr->playerId = 0;  // 호스트 송신

    auto* ack     = (ConnectAckData*)(buf + sizeof(PacketHeader));
    ack->assignedId  = (uint8_t)assignedId;
    ack->sharedLives = g_sharedLives;

    SendToOne(assignedId, buf, (int)sizeof(buf));
}


// ============================================================
//  스텁 — 이후 단계에서 구현 예정
// ============================================================

void NetShutdown()
{
    if (!g_recvCSInit) return;  // NetInit 전이거나 이미 종료됨

    NetCloseResources();

    DeleteCriticalSection(&g_recvCS);
    g_recvCSInit = false;

    WSACleanup();
    g_net.localIp[0] = '\0';  // 다음 NetInit에서 IP 재조회 + WSAStartup 호출
}

// ============================================================
//  10/30 — accept 폴링 (호스트 전용, 로비 루프에서 매 프레임 호출)
// ============================================================

void NetPollAccept()
{
    if (!g_net.isHost) return;
    if (g_net.connectedCount >= 3) return;
    if (g_net.listenSock == INVALID_SOCKET) return;

    sockaddr_in clientAddr = {};
    int addrLen = sizeof(clientAddr);
    SOCKET s = accept(g_net.listenSock, (sockaddr*)&clientAddr, &addrLen);
    if (s == INVALID_SOCKET)
    {
        if (WSAGetLastError() != WSAEWOULDBLOCK)
            OutputDebugStringA("[NetPollAccept] accept 오류\n");
        return;  // WSAEWOULDBLOCK → 접속 없음, 정상
    }

    // accept 소켓은 listen 소켓의 non-blocking 속성을 상속 → RecvThread용으로 blocking 전환
    u_long blockingMode = 0;
    ioctlsocket(s, FIONBIO, &blockingMode);

    // 빈 슬롯 찾기 (0 또는 1)
    int slot = -1;
    for (int i = 0; i < 2; i++)
    {
        if (g_net.clientSockets[i] == INVALID_SOCKET)
        {
            slot = i;
            break;
        }
    }
    if (slot == -1)
    {
        closesocket(s);  // 슬롯 없음 (예외 상황)
        return;
    }

    g_net.clientSockets[slot] = s;
    g_net.connectedCount++;

    SendConnectAck(slot + 1);  // playerId 1 또는 2 부여

    NetStartRecvThread(s, slot + 1);

    // ACK 뒤에 보내므로 새 클라이언트도 자기 ID를 먼저 알고 현황을 받는다
    BroadcastLobbyStatus();
}

// ============================================================
//  10/31 — connectedCount == 3 감지 시 GAME_START 브로드캐스트
// ============================================================

void NetBroadcastGameStart()
{
    uint8_t buf[sizeof(PacketHeader) + sizeof(GameEventData)];

    auto* hdr     = (PacketHeader*)buf;
    hdr->size     = (uint16_t)sizeof(buf);
    hdr->type     = PKT_GAME_EVENT;
    hdr->playerId = 0;

    auto* evt        = (GameEventData*)(buf + sizeof(PacketHeader));
    evt->eventType   = EVT_GAME_START;
    evt->param       = 0;

    SendToAll(buf, (int)sizeof(buf));

    // 더 이상 accept 받지 않음 — listen 소켓 닫기
    closesocket(g_net.listenSock);
    g_net.listenSock = INVALID_SOCKET;
}

void NetPollPackets()
{
    ReceivedPacket received;
    while (RecvQueuePop(received))
    {
        const auto& pkt = received.bytes;
        if (pkt.size() < sizeof(PacketHeader)) continue;
        const auto* hdr = (const PacketHeader*)pkt.data();
        const int senderPlayerId = received.senderPlayerId;
        const bool fromHost = !g_net.isHost && senderPlayerId == 0 && hdr->playerId == 0;

        switch (hdr->type)
        {
        case PKT_PLAYER_STATE:
        {
            if (pkt.size() < sizeof(PacketHeader) + sizeof(PlayerStateData)) break;
            int id = hdr->playerId;
            if (id < 0 || id > 2 || id == g_net.myPlayerId) break;
            if (g_net.isHost && id != senderPlayerId) break;

            const auto* ps = (const PlayerStateData*)(pkt.data() + sizeof(PacketHeader));
            if (!ApplyRemotePlayerState(id, *ps)) break;

            // 호스트: 보낸 사람을 제외한 나머지 클라이언트에게 중계 (헤더의 playerId 유지)
            if (g_net.isHost)
            {
                for (int target = 1; target <= 2; target++)
                    if (target != id) SendToOne(target, pkt.data(), (int)pkt.size());
            }

            break;
        }
        case PKT_ITEM_PICKUP:
        {
            if (pkt.size() != sizeof(ItemPickupPacket) || hdr->size != pkt.size()) break;
            ItemPickupData pickup;
            memcpy(&pickup, pkt.data() + sizeof(PacketHeader), sizeof(pickup));
            if (g_net.isHost)
            {
                if (senderPlayerId < 1 || senderPlayerId > 2 || hdr->playerId != senderPlayerId ||
                    pickup.pickerId != senderPlayerId || pickup.approved != 0) break;
                if (!ConfirmItemPickup(senderPlayerId, pickup))
                {
                    const ItemPickupPacket rejected = {
                        { sizeof(ItemPickupPacket), PKT_ITEM_PICKUP, 0 }, pickup
                    };
                    SendToOne(senderPlayerId, &rejected, sizeof(rejected));
                }
            }
            else if (fromHost)
            {
                ApplyItemPickupResult(pickup);
            }
            break;
        }
        case PKT_SHOT_SPAWN:
        {
            if (!g_game.gameStart || !g_game.isMulti || g_game.isRenderTest ||
                pkt.size() != sizeof(PacketHeader) + sizeof(ShotSpawnData) ||
                hdr->size != pkt.size()) break;
            if (!g_net.isHost || senderPlayerId < 1 || senderPlayerId > 2 ||
                hdr->playerId != senderPlayerId) break;
            ShotSpawnData spawn;
            memcpy(&spawn, pkt.data() + sizeof(PacketHeader), sizeof(spawn));
            if (spawn.shotType > SHOT_TINOFIRE || spawn.slot >= MAX_SHOT ||
                spawn.direction > DIR_RIGHT || !spawn.width || !spawn.height) break;
            for (int target = 1; target <= 2; ++target)
                if (target != senderPlayerId) SendToOne(target, pkt.data(), (int)pkt.size());
            break;
        }
        case PKT_BLOCK_HIT:
        {
            if (!g_net.isHost || !g_game.gameStart || !g_game.isMulti ||
                senderPlayerId < 1 || senderPlayerId > 2 || hdr->playerId != senderPlayerId ||
                pkt.size() != sizeof(PacketHeader) + sizeof(BlockHitData) || hdr->size != pkt.size()) break;
            BlockHitData hit;
            memcpy(&hit, pkt.data() + sizeof(PacketHeader), sizeof(hit));
            if (hit.row >= MAP_HEIGHT || hit.col >= MAP_WIDTH) break;
            if (currentMap[hit.row][hit.col] == TILE_COIN)
                CollectMapCoin(hit.row, hit.col, senderPlayerId);
            else
                HandleBlockHit(hit.row, hit.col, senderPlayerId);
            break;
        }
        case PKT_TILE_CHANGE:
        {
            // 호스트 연결만 신뢰하고, 헤더 ID는 호스트가 승인한 타격자를 나타낸다.
            if (g_net.isHost || senderPlayerId != 0 || hdr->playerId > 2 ||
                !g_game.gameStart || !g_game.isMulti ||
                pkt.size() != sizeof(PacketHeader) + sizeof(TileChangeData) ||
                hdr->size != pkt.size()) break;
            TileChangeData change;
            memcpy(&change, pkt.data() + sizeof(PacketHeader), sizeof(change));
            if (change.row >= MAP_HEIGHT || change.col >= MAP_WIDTH) break;
            if (change.tile != TILE_USED_BLOCK && change.tile != TILE_EMPTY) break;
            int& tile = currentMap[change.row][change.col];
            if (hdr->playerId == g_localPlayerIdx &&
                ((tile == TILE_COIN && change.tile == TILE_EMPTY) ||
                 (tile == TILE_BOX_COIN && change.tile == TILE_USED_BLOCK)))
            {
                PlaySoundBuffer(coin_Sound);
                ++g_player.mario.coin;
            }
            tile = change.tile;
            break;
        }
        case PKT_ITEM_SPAWN:
        {
            if (!fromHost || pkt.size() != sizeof(PacketHeader) + sizeof(ItemSpawnData) ||
                hdr->size != pkt.size()) break;
            ItemSpawnData spawn;
            memcpy(&spawn, pkt.data() + sizeof(PacketHeader), sizeof(spawn));
            ApplyItemSpawn(spawn);
            break;
        }
        case PKT_GAME_EVENT:
        {
            if (pkt.size() < sizeof(PacketHeader) + sizeof(GameEventData)) break;
            const auto* evt = (const GameEventData*)(pkt.data() + sizeof(PacketHeader));
            switch (evt->eventType)
            {
            case EVT_DISCONNECT:
            {
                if (!received.disconnected) break;
                int pid = senderPlayerId;
                if (g_net.isHost)
                {
                    // 클라이언트 이탈: 슬롯 비우고 인원 감소
                    if (pid >= 1 && pid <= 2 && g_net.clientSockets[pid - 1] != INVALID_SOCKET)
                    {
                        closesocket(g_net.clientSockets[pid - 1]);  // 소켓은 메인 스레드가 닫음
                        g_net.clientSockets[pid - 1] = INVALID_SOCKET;
                        if (g_net.connectedCount > 0)
                            g_net.connectedCount--;
                        BroadcastLobbyStatus();  // 남은 클라이언트의 로비 표시 갱신
                    }
                }
                else if (pid == 0 && g_net.hostSocket != INVALID_SOCKET)
                {
                    // 호스트 연결 끊김: 더 보낼 곳이 없으므로 송신 중단 (화면 처리는 A-9 · C-4)
                    closesocket(g_net.hostSocket);
                    g_net.hostSocket     = INVALID_SOCKET;
                    g_net.myPlayerId     = -1;  // NetFlushSend 중단
                    g_net.connectedCount = 0;
                    g_net.lobbyMask      = 0;
                }
                break;
            }
            case EVT_LOBBY_STATUS:
            {
                if (g_net.isHost) break;
                g_net.lobbyMask      = evt->param & 0x07;
                g_net.connectedCount = 0;
                for (int i = 0; i < 3; i++)
                    if (g_net.lobbyMask & (1 << i)) g_net.connectedCount++;
                break;
            }
            case EVT_LIVES_CHANGED:
                g_sharedLives = evt->param;
                break;
            case EVT_GAME_START:
                if (!fromHost || g_game.gameStart || g_net.myPlayerId < 1) break;
                // 로비에서 스테이지를 준비한 뒤 다음 게임 패킷을 처리한다.
                g_game.multiStartPending = true;
                return;
            case EVT_GAME_OVER:
                g_game.gameState = GAME_OVER;
                break;
            case EVT_STAGE_CLEAR:
                g_game.gameState = GAME_CLEAR;
                break;
            default:
                break;
            }
            break;
        }
        case PKT_CONNECT_ACK:
        {
            if (g_net.isHost) break;
            if (pkt.size() < sizeof(PacketHeader) + sizeof(ConnectAckData)) break;
            const auto* ack = (const ConnectAckData*)(pkt.data() + sizeof(PacketHeader));
            if (ack->assignedId < 1 || ack->assignedId > 2) break;
            g_net.myPlayerId = ack->assignedId;
            g_sharedLives    = ack->sharedLives;
            g_localPlayerIdx = g_net.myPlayerId;
            break;
        }
        default:
            break;
        }
    }
}

void NetFlushSend()
{
    //플레이어 배열의 유효 인덱스는 0~2
    if (g_net.myPlayerId < 0 || g_net.myPlayerId >= 3) 
        return;

    uint8_t buf[sizeof(PacketHeader) + sizeof(PlayerStateData)];
    auto* hdr     = (PacketHeader*)buf;
    hdr->size     = (uint16_t)sizeof(buf);
    hdr->type     = PKT_PLAYER_STATE;
    hdr->playerId = (uint8_t)g_net.myPlayerId;

    const Player& me = g_allPlayers[g_net.myPlayerId].mario;
    auto* ps         = (PlayerStateData*)(buf + sizeof(PacketHeader));
    ps->worldX            = (int16_t)(me.x + (int)g_game.cameraX);  // 로컬 x는 화면 좌표
    ps->y                 = (int16_t)me.y;
    ps->vx                = me.isTransforming ? 0 : (int16_t)me.vx;
    ps->vy                = me.isTransforming ? 0 : (int16_t)me.vy;
    ps->coin              = (int16_t)me.coin;
    ps->life              = (uint8_t)me.life;
    ps->form              = (uint8_t)me.form;
    ps->charType          = (uint8_t)me.charType;
    ps->transformTarget   = (uint8_t)me.transformTarget;
    ps->width             = (uint8_t)me.width;
    ps->height            = (uint8_t)me.height;
    ps->walkMotion        = (uint8_t)me.walk_motion;
    ps->motionTimer       = (uint16_t)me.motion_timer;
    ps->tinoCooldownZ     = (uint8_t)me.tino_cooldown_z;
    ps->tinoCooldownSpace = (uint8_t)me.tino_cooldown_space;
    ps->flags             = 0;
    if (me.isJumping)        ps->flags |= FLAG_JUMPING;
    if (me.isFlying)         ps->flags |= FLAG_FLYING;
    if (me.isDead)           ps->flags |= FLAG_DEAD;
    if (me.star)             ps->flags |= FLAG_STAR;
    if (me.god || me.isTransforming) ps->flags |= FLAG_INVINCIBLE;
    if (me.isWalking)        ps->flags |= FLAG_WALKING;
    if (me.direction == DIR_RIGHT) ps->flags |= FLAG_FACING_RIGHT;
    if (me.fire_motion)      ps->flags |= FLAG_FIRE_MOTION;
    if (me.tino_motion)      ps->flags |= FLAG_TINO_MOTION;
    if (me.tino_fire_motion) ps->flags |= FLAG_TINO_FIRE_MOTION;
    if (me.gameover)         ps->flags |= FLAG_GAMEOVER;
    if (me.supergod)         ps->flags |= FLAG_SUPERGOD;
    if (me.isTransforming)   ps->flags |= FLAG_TRANSFORMING;

    if (g_net.isHost) SendToAll(buf, (int)sizeof(buf));
    else              SendToHost(buf, (int)sizeof(buf));
}

// ============================================================
//  A-6 — 송신 함수
// ============================================================

// send가 일부만 보낼 수 있으므로 전부 보낼 때까지 반복
// 송신 실패는 RecvThread의 EVT_DISCONNECT 경로에서 처리
static void SendFull(SOCKET sock, const void* data, int size)
{
    if (sock == INVALID_SOCKET || data == nullptr || size <= 0) return;

    const char* p = (const char*)data;
    int sent = 0;
    while (sent < size)
    {
        int r = send(sock, p + sent, size - sent, 0);
        if (r <= 0) return;
        sent += r;
    }
}

void SendToAll(const void* data, int size)
{
    if (!g_net.isHost) return;

    for (int i = 0; i < 2; i++)
        SendFull(g_net.clientSockets[i], data, size);
}

void SendToHost(const void* data, int size)
{
    if (g_net.isHost) return;

    SendFull(g_net.hostSocket, data, size);
}

void SendToOne(int playerId, const void* data, int size)
{
    if (!g_net.isHost) return;
    if (playerId < 1 || playerId > 2) return;  // 클라이언트 슬롯은 playerId 1, 2

    SendFull(g_net.clientSockets[playerId - 1], data, size);
}

// ============================================================
//  A-4 — 수신 큐 (11/5)
// ============================================================

static void RecvQueuePush(ReceivedPacket packet)
{
    EnterCriticalSection(&g_recvCS);
    g_recvQueue.push(std::move(packet));
    LeaveCriticalSection(&g_recvCS);
}

static bool RecvQueuePop(ReceivedPacket& out)
{
    EnterCriticalSection(&g_recvCS);
    if (g_recvQueue.empty())
    {
        LeaveCriticalSection(&g_recvCS);
        return false;
    }
    out = std::move(g_recvQueue.front());
    g_recvQueue.pop();
    LeaveCriticalSection(&g_recvCS);
    return true;
}

// ============================================================
//  A-5 — RecvThread (11/5)
// ============================================================

// 헤더 size 필드 기준 패킷 전체 수신 (blocking)
static bool RecvFull(SOCKET sock, std::vector<uint8_t>& out)
{
    // 1단계: 헤더 수신
    PacketHeader hdr;
    int received = 0;
    while (received < (int)sizeof(hdr))
    {
        int r = recv(sock, (char*)&hdr + received, (int)sizeof(hdr) - received, 0);
        if (r <= 0) return false;
        received += r;
    }

    // 비정상 패킷 크기 → 연결 끊기
    if (hdr.size < (uint16_t)sizeof(hdr) || hdr.size > 4096)
        return false;

    // 2단계: 나머지 페이로드 수신
    out.resize(hdr.size);
    memcpy(out.data(), &hdr, sizeof(hdr));
    received = (int)sizeof(hdr);
    while (received < (int)hdr.size)
    {
        int r = recv(sock, (char*)out.data() + received, (int)hdr.size - received, 0);
        if (r <= 0) return false;
        received += r;
    }
    return true;
}

static DWORD WINAPI RecvThread(LPVOID param)
{
    RecvThreadParam* p = (RecvThreadParam*)param;
    SOCKET sock     = p->sock;
    int    playerId = p->playerId;
    delete p;

    std::vector<uint8_t> pkt;
    while (RecvFull(sock, pkt))
    {
        RecvQueuePush({ std::move(pkt), playerId, false });
    }

    // 연결 끊김 → EVT_DISCONNECT 큐 삽입
    uint8_t buf[sizeof(PacketHeader) + sizeof(GameEventData)] = {};
    auto* hdr        = (PacketHeader*)buf;
    hdr->size        = (uint16_t)sizeof(buf);
    hdr->type        = PKT_GAME_EVENT;
    hdr->playerId    = (uint8_t)playerId;
    auto* evt        = (GameEventData*)(buf + sizeof(PacketHeader));
    evt->eventType   = EVT_DISCONNECT;
    evt->param       = (uint8_t)playerId;

    RecvQueuePush({ std::vector<uint8_t>(buf, buf + sizeof(buf)), playerId, true });

    return 0;
}


// B: packet helpers deliberately do not mutate game state or send sockets.
namespace {
bool ValidateMonsterSnapshot(uint8_t kind, const std::vector<MonsterEntry>& states)
{
    if (kind > MON_FIREBALL || states.size() > 100 ||
        (kind == MON_BROWN_TURTLE && !states.empty())) return false;

    bool seen[100] = {};
    for (const auto& state : states) {
        if (state.slot >= 100 || seen[state.slot] ||
            state.width == 0 || state.height == 0) return false;
        seen[state.slot] = true;
        uint8_t allowed = MON_FLAG_ALIVE | MON_FLAG_DEAD | MON_FLAG_ACTIVE | MON_FLAG_FALLING;
        if (kind == MON_TURTLE) allowed |= MON_FLAG_DAMAGE | MON_FLAG_FACING_LEFT;
        if (kind == MON_ANGEL_TURTLE) allowed |= MON_FLAG_GOING_UP;
        if (kind == MON_FIREBALL) allowed = MON_FLAG_ACTIVE;
        if (state.flags & ~allowed) return false;
        if (kind == MON_TURTLE) {
            if (state.state > 2) return false;
        } else if (kind == MON_ANGEL_TURTLE) {
            if (state.state > 1) return false;
        } else if (state.state != 0) return false;
        if (kind != MON_TURTLE && state.timerB != 0) return false;
    }
    return true;
}
}

bool BuildMonsterStatePacket(uint8_t kind, const std::vector<MonsterEntry>& states,
    std::vector<uint8_t>& packet)
{
    if (!ValidateMonsterSnapshot(kind, states)) return false;
    const size_t prefix = sizeof(PacketHeader) + sizeof(MonsterStateHeader);
    std::vector<uint8_t> result(prefix + states.size() * sizeof(MonsterEntry));
    PacketHeader header = { static_cast<uint16_t>(result.size()), PKT_MONSTER_STATE, 0 };
    MonsterStateHeader snapshot = { kind, static_cast<uint8_t>(states.size()) };
    memcpy(result.data(), &header, sizeof(header));
    memcpy(result.data() + sizeof(header), &snapshot, sizeof(snapshot));
    if (!states.empty()) memcpy(result.data() + prefix, states.data(), states.size() * sizeof(MonsterEntry));
    packet.swap(result);
    return true;
}

bool ParseMonsterStatePacket(const std::vector<uint8_t>& packet, uint8_t& kind,
    std::vector<MonsterEntry>& states)
{
    const size_t prefix = sizeof(PacketHeader) + sizeof(MonsterStateHeader);
    if (packet.size() < prefix) return false;
    PacketHeader header = {};
    MonsterStateHeader snapshot = {};
    memcpy(&header, packet.data(), sizeof(header));
    memcpy(&snapshot, packet.data() + sizeof(header), sizeof(snapshot));
    if (header.type != PKT_MONSTER_STATE || header.playerId != 0 ||
        header.size != packet.size() || snapshot.count > 100 ||
        packet.size() != prefix + snapshot.count * sizeof(MonsterEntry)) return false;
    std::vector<MonsterEntry> result(snapshot.count);
    if (!result.empty()) memcpy(result.data(), packet.data() + prefix, result.size() * sizeof(MonsterEntry));
    if (!ValidateMonsterSnapshot(snapshot.kind, result)) return false;
    kind = snapshot.kind;
    states.swap(result);
    return true;
}
