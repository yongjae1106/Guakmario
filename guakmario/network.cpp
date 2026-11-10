#include "network.h"
#include "config.h"
#include "player.h"
#include "game.h"
#include <queue>

// ============================================================
//  전역 변수 정의
// ============================================================

NetContext g_net       = {};
int        g_sharedLives = 3;


// ============================================================
//  수신 큐 / 크리티컬 섹션 (A-4, A-5에서 사용)
// ============================================================

static std::queue<std::vector<uint8_t>> g_recvQueue;
static CRITICAL_SECTION                 g_recvCS;
static bool                             g_recvCSInit = false;

struct RecvThreadParam {
    SOCKET sock;
    int    playerId;  // 1 or 2
};

static bool RecvPacketFull(SOCKET sock, std::vector<uint8_t>& out);
static DWORD WINAPI RecvThread(LPVOID param);


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

void NetInit(bool isHost, const char* hostIp)
{
    // 이전 소켓이 열려 있으면 먼저 정리 (재진입 대비)
    if (g_net.listenSock != INVALID_SOCKET)
    {
        closesocket(g_net.listenSock);
        g_net.listenSock = INVALID_SOCKET;
    }
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

    g_net.isHost          = isHost;
    g_net.myPlayerId      = isHost ? 0 : -1;   // 클라이언트는 ACK 받은 후 설정
    g_net.connectedCount  = isHost ? 1 : 0;    // 호스트 자신 포함
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

        int opt = 1;
        setsockopt(g_net.listenSock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

        sockaddr_in addr = {};
        addr.sin_family      = AF_INET;
        addr.sin_port        = htons(NET_PORT);
        addr.sin_addr.s_addr = INADDR_ANY;

        bind(g_net.listenSock, (sockaddr*)&addr, sizeof(addr));
        listen(g_net.listenSock, 2);    // backlog 2 (클라이언트 최대 2명)

        // listen 소켓만 non-blocking — NetPollAccept에서 매 프레임 폴링
        u_long mode = 1;
        ioctlsocket(g_net.listenSock, FIONBIO, &mode);
    }
    // 클라이언트 connect 처리는 11/16 예정
}


// ============================================================
//  PKT_CONNECT_ACK 전송 헬퍼 (내부용)
// ============================================================

static void SendConnectAck(SOCKET s, int assignedId)
{
    uint8_t buf[sizeof(PacketHeader) + sizeof(ConnectAckData)];

    auto* hdr  = (PacketHeader*)buf;
    hdr->size     = (uint16_t)sizeof(buf);
    hdr->type     = PKT_CONNECT_ACK;
    hdr->playerId = 0;  // 호스트 송신

    auto* ack     = (ConnectAckData*)(buf + sizeof(PacketHeader));
    ack->assignedId  = (uint8_t)assignedId;
    ack->sharedLives = g_sharedLives;

    send(s, (const char*)buf, sizeof(buf), 0);
}


// ============================================================
//  스텁 — 이후 단계에서 구현 예정
// ============================================================

void NetShutdown()
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
    if (g_recvCSInit)
    {
        DeleteCriticalSection(&g_recvCS);
        g_recvCSInit = false;
    }
    WSACleanup();
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

    SendConnectAck(s, slot + 1);  // playerId 1 또는 2 부여

    auto* tp = new RecvThreadParam{ s, slot + 1 };
    CreateThread(nullptr, 0, RecvThread, tp, 0, nullptr);
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

    for (int i = 0; i < 2; i++)
    {
        if (g_net.clientSockets[i] != INVALID_SOCKET)
            send(g_net.clientSockets[i], (const char*)buf, sizeof(buf), 0);
    }

    // 더 이상 accept 받지 않음 — listen 소켓 닫기
    closesocket(g_net.listenSock);
    g_net.listenSock = INVALID_SOCKET;
}

void NetPollPackets()
{
    std::vector<uint8_t> pkt;
    while (PopRecvPacket(pkt))
    {
        if (pkt.size() < sizeof(PacketHeader)) continue;
        const auto* hdr = (const PacketHeader*)pkt.data();

        switch (hdr->type)
        {
        case PKT_PLAYER_STATE:
        {
            if (pkt.size() < sizeof(PacketHeader) + sizeof(PlayerStateData)) break;
            int id = hdr->playerId;
            if (id < 0 || id > 2 || id == g_net.myPlayerId) break;
            const auto* ps = (const PlayerStateData*)(pkt.data() + sizeof(PacketHeader));
            Player& p  = g_allPlayers[id].mario;
            p.x                   = ps->worldX;  // 다른 플레이어는 월드 좌표로 보관
            p.y                   = ps->y;
            p.vx                  = ps->vx;
            p.vy                  = ps->vy;
            p.coin                = ps->coin;
            p.life                = ps->life;
            p.form                = (MarioForm)ps->form;
            p.charType            = (CharacterType)ps->charType;
            p.width               = ps->width;
            p.height              = ps->height;
            p.walk_motion         = ps->walkMotion;
            p.motion_timer        = ps->motionTimer;
            p.tino_cooldown_z     = ps->tinoCooldownZ;
            p.tino_cooldown_space = ps->tinoCooldownSpace;
            p.direction           = (ps->flags & FLAG_FACING_RIGHT) ? DIR_RIGHT : DIR_LEFT;
            p.isJumping           = (ps->flags & FLAG_JUMPING)          != 0;
            p.isFlying            = (ps->flags & FLAG_FLYING)           != 0;
            p.isWalking           = (ps->flags & FLAG_WALKING)          != 0;
            p.isDead              = (ps->flags & FLAG_DEAD)             != 0;
            p.gameover            = (ps->flags & FLAG_GAMEOVER)         != 0;
            p.star                = (ps->flags & FLAG_STAR)             != 0;
            p.god                 = (ps->flags & FLAG_INVINCIBLE)       != 0;
            p.supergod            = (ps->flags & FLAG_SUPERGOD)         != 0;
            p.fire_motion         = (ps->flags & FLAG_FIRE_MOTION)      != 0;
            p.tino_motion         = (ps->flags & FLAG_TINO_MOTION)      != 0;
            p.tino_fire_motion    = (ps->flags & FLAG_TINO_FIRE_MOTION) != 0;
            // transformTarget, FLAG_TRANSFORMING은 Player에 저장할 곳이 없어 렌더 통합 시 처리
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
                int pid = evt->param;
                if (g_net.isHost && pid >= 1 && pid <= 2)
                    g_net.clientSockets[pid - 1] = INVALID_SOCKET;
                if (g_net.connectedCount > 0)
                    g_net.connectedCount--;
                break;
            }
            case EVT_LIVES_CHANGED:
                g_sharedLives = evt->param;
                break;
            case EVT_GAME_START:
                g_game.gameStart = true;
                break;
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
    if (g_net.myPlayerId < 0) return;

    uint8_t buf[sizeof(PacketHeader) + sizeof(PlayerStateData)];
    auto* hdr     = (PacketHeader*)buf;
    hdr->size     = (uint16_t)sizeof(buf);
    hdr->type     = PKT_PLAYER_STATE;
    hdr->playerId = (uint8_t)g_net.myPlayerId;

    const Player& me = g_allPlayers[g_net.myPlayerId].mario;
    auto* ps         = (PlayerStateData*)(buf + sizeof(PacketHeader));
    ps->worldX            = (int16_t)(me.x + (int)g_game.cameraX);  // 로컬 x는 화면 좌표
    ps->y                 = (int16_t)me.y;
    ps->vx                = (int16_t)me.vx;
    ps->vy                = (int16_t)me.vy;
    ps->coin              = (int16_t)me.coin;
    ps->life              = (uint8_t)me.life;
    ps->form              = (uint8_t)me.form;
    ps->charType          = (uint8_t)me.charType;
    ps->transformTarget   = (uint8_t)g_game.transformTarget;
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
    if (me.god)              ps->flags |= FLAG_INVINCIBLE;
    if (me.isWalking)        ps->flags |= FLAG_WALKING;
    if (me.direction == DIR_RIGHT) ps->flags |= FLAG_FACING_RIGHT;
    if (me.fire_motion)      ps->flags |= FLAG_FIRE_MOTION;
    if (me.tino_motion)      ps->flags |= FLAG_TINO_MOTION;
    if (me.tino_fire_motion) ps->flags |= FLAG_TINO_FIRE_MOTION;
    if (me.gameover)         ps->flags |= FLAG_GAMEOVER;
    if (me.supergod)         ps->flags |= FLAG_SUPERGOD;
    if (g_game.gameState == GAME_TRANSFORMING) ps->flags |= FLAG_TRANSFORMING;

    auto sendAll = [](SOCKET s, const uint8_t* data, int len)
    {
        int sent = 0;
        while (sent < len)
        {
            int r = send(s, (const char*)data + sent, len - sent, 0);
            if (r <= 0) return;
            sent += r;
        }
    };

    if (g_net.isHost)
    {
        for (int i = 0; i < 2; i++)
            if (g_net.clientSockets[i] != INVALID_SOCKET)
                sendAll(g_net.clientSockets[i], buf, (int)sizeof(buf));
    }
    else
    {
        if (g_net.hostSocket != INVALID_SOCKET)
            sendAll(g_net.hostSocket, buf, (int)sizeof(buf));
    }
}

void SendToAll(const void* data, int size)
{
    // 11/11 예정
}

void SendToHost(const void* data, int size)
{
    // 11/11 예정
}

void SendToOne(int playerId, const void* data, int size)
{
    // 11/11 예정
}

// ============================================================
//  A-4 — 수신 큐 (11/5)
// ============================================================

void PushRecvPacket(std::vector<uint8_t> pkt)
{
    EnterCriticalSection(&g_recvCS);
    g_recvQueue.push(std::move(pkt));
    LeaveCriticalSection(&g_recvCS);
}

bool PopRecvPacket(std::vector<uint8_t>& out)
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
static bool RecvPacketFull(SOCKET sock, std::vector<uint8_t>& out)
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
    while (RecvPacketFull(sock, pkt))
    {
        PushRecvPacket(pkt);
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

    PushRecvPacket(std::vector<uint8_t>(buf, buf + sizeof(buf)));

    closesocket(sock);
    return 0;
}
