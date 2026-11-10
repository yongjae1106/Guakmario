// 네트워크 동작 확인용 콘솔 클라이언트
// 사용법: net_test_client.exe [호스트 IP]  (기본값 127.0.0.1)
//
// 1) 접속 후 PKT_CONNECT_ACK 출력
// 2) 호스트가 보내는 패킷 출력 (PKT_PLAYER_STATE는 값이 바뀔 때 최대 0.2초에 1번)
// 3) EVT_GAME_START 수신 후 좌우로 움직이는 PKT_PLAYER_STATE 송신

#include "../guakmario/network.h"
#include "../guakmario/config.h"
#include <atomic>
#include <cmath>
#include <cstdio>
#include <cstring>

static SOCKET           g_sock = INVALID_SOCKET;
static int              g_myId = -1;
static std::atomic<bool> g_gameStarted{ false };
static std::atomic<bool> g_running{ true };

static bool SendAll(const void* data, int len)
{
    const char* p = (const char*)data;
    int sent = 0;
    while (sent < len)
    {
        int r = send(g_sock, p + sent, len - sent, 0);
        if (r <= 0) return false;
        sent += r;
    }
    return true;
}

static bool RecvAll(void* data, int len)
{
    char* p = (char*)data;
    int received = 0;
    while (received < len)
    {
        int r = recv(g_sock, p + received, len - received, 0);
        if (r <= 0) return false;
        received += r;
    }
    return true;
}

// 게임 시작 후 약 30Hz로 좌우 왕복 좌표 송신
static DWORD WINAPI SendThread(LPVOID)
{
    const int baseX = 100 + g_myId * 200;
    const DWORD start = GetTickCount();

    while (g_running)
    {
        if (!g_gameStarted)
        {
            Sleep(30);
            continue;
        }

        const double t = (GetTickCount() - start) / 1000.0;

        uint8_t buf[sizeof(PacketHeader) + sizeof(PlayerStateData)] = {};
        auto* hdr     = (PacketHeader*)buf;
        hdr->size     = (uint16_t)sizeof(buf);
        hdr->type     = PKT_PLAYER_STATE;
        hdr->playerId = (uint8_t)g_myId;

        auto* ps = (PlayerStateData*)(buf + sizeof(PacketHeader));
        const double dir = cos(t * 2.0);
        ps->worldX     = (int16_t)(baseX + 100.0 * sin(t * 2.0));
        ps->y          = 480;  // 스테이지 1 지면 높이
        ps->vx         = (int16_t)(dir >= 0 ? 3 : -3);
        ps->life       = 3;
        ps->charType   = (uint8_t)g_myId;  // 1 = 루이지, 2 = 토드
        ps->width      = 40;
        ps->height     = 40;
        ps->walkMotion = (uint8_t)((int)(t * 10.0) % 3);
        ps->flags      = FLAG_WALKING;
        if (dir >= 0) ps->flags |= FLAG_FACING_RIGHT;

        if (!SendAll(buf, (int)sizeof(buf))) break;
        Sleep(33);
    }
    return 0;
}

static void PrintPlayerState(const PacketHeader& hdr, const PlayerStateData& ps)
{
    static PlayerStateData last[3] = {};
    static bool  seen[3] = {};
    static DWORD lastPrint = 0;
    static unsigned long total = 0;

    total++;
    if (hdr.playerId > 2) return;

    const bool changed = !seen[hdr.playerId] || memcmp(&last[hdr.playerId], &ps, sizeof(ps)) != 0;
    const DWORD now = GetTickCount();
    if (!changed || now - lastPrint < 200) return;

    printf("[STATE] from=%u worldX=%d y=%d vx=%d vy=%d life=%u form=%u char=%u size=%ux%u walk=%u timer=%u cool=%u/%u coin=%d flags=0x%04X (total=%lu)\n",
        hdr.playerId, ps.worldX, ps.y, ps.vx, ps.vy, ps.life, ps.form, ps.charType, ps.width, ps.height,
        ps.walkMotion, ps.motionTimer, ps.tinoCooldownZ, ps.tinoCooldownSpace, ps.coin, ps.flags, total);

    last[hdr.playerId] = ps;
    seen[hdr.playerId] = true;
    lastPrint = now;
}

int main(int argc, char* argv[])
{
    const char* ip = (argc > 1) ? argv[1] : "127.0.0.1";

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("WSAStartup failed\n");
        return 1;
    }

    g_sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(NET_PORT);
    inet_pton(AF_INET, ip, &addr.sin_addr);

    printf("connecting to %s:%d ...\n", ip, NET_PORT);
    if (connect(g_sock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
    {
        printf("connect failed: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    printf("connected, waiting for ACK\n");

    HANDLE sender = nullptr;
    std::vector<uint8_t> payload;

    while (true)
    {
        PacketHeader hdr;
        if (!RecvAll(&hdr, (int)sizeof(hdr))) break;

        if (hdr.size < sizeof(hdr) || hdr.size > 4096)
        {
            printf("[ERROR] invalid packet size: %u\n", hdr.size);
            break;
        }

        const int payloadSize = hdr.size - (int)sizeof(hdr);
        payload.resize(payloadSize);
        if (payloadSize > 0 && !RecvAll(payload.data(), payloadSize)) break;

        switch (hdr.type)
        {
        case PKT_CONNECT_ACK:
        {
            if (payloadSize < (int)sizeof(ConnectAckData)) break;
            const auto* ack = (const ConnectAckData*)payload.data();
            g_myId = ack->assignedId;
            printf("[ACK] assignedId=%u sharedLives=%d\n", ack->assignedId, ack->sharedLives);
            if (sender == nullptr)
                sender = CreateThread(nullptr, 0, SendThread, nullptr, 0, nullptr);
            break;
        }
        case PKT_GAME_EVENT:
        {
            if (payloadSize < (int)sizeof(GameEventData)) break;
            const auto* evt = (const GameEventData*)payload.data();
            printf("[EVENT] type=%u param=%u\n", evt->eventType, evt->param);
            if (evt->eventType == EVT_GAME_START)
                g_gameStarted = true;
            break;
        }
        case PKT_PLAYER_STATE:
        {
            if (payloadSize < (int)sizeof(PlayerStateData)) break;
            PrintPlayerState(hdr, *(const PlayerStateData*)payload.data());
            break;
        }
        default:
            printf("[PACKET] type=%u size=%u from=%u\n", hdr.type, hdr.size, hdr.playerId);
            break;
        }
    }

    printf("disconnected\n");
    g_running = false;
    closesocket(g_sock);
    if (sender != nullptr)
    {
        WaitForSingleObject(sender, 1000);
        CloseHandle(sender);
    }
    WSACleanup();
    return 0;
}
