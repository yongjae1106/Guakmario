#include "network.h"
#include <queue>

// ============================================================
//  전역 변수 정의
// ============================================================

NetContext g_net       = {};
int        g_sharedLives = 3;

static constexpr int NET_PORT = 9000;


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
    g_net.isHost          = isHost;
    g_net.myPlayerId      = isHost ? 0 : -1;   // 클라이언트는 ACK 받은 후 설정
    g_net.connectedCount  = isHost ? 1 : 0;    // 호스트 자신 포함
    g_net.listenSock      = INVALID_SOCKET;
    g_net.clientSockets[0] = INVALID_SOCKET;
    g_net.clientSockets[1] = INVALID_SOCKET;
    g_net.hostSocket      = INVALID_SOCKET;

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
    // 11/16 예정
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

    // TODO(11/2): CreateThread(RecvThread, s) 추가
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
    // 11/6 예정
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

static std::queue<std::vector<uint8_t>> g_recvQueue;
static CRITICAL_SECTION                 g_recvCS;

void PushRecvPacket(std::vector<uint8_t> pkt)
{
    // 11/4 예정
}

bool PopRecvPacket(std::vector<uint8_t>& out)
{
    // 11/4 예정
    return false;
}
