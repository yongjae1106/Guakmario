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
    }
    // 클라이언트 connect 처리는 11/16 예정
}


// ============================================================
//  스텁 — 이후 단계에서 구현 예정
// ============================================================

void NetShutdown()
{
    // 11/16 예정
}

void NetPollAccept()
{
    // 10/30 예정
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
