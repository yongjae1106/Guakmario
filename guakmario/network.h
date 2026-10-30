#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdint.h>
#include <vector>
#pragma comment(lib, "Ws2_32.lib")

// ============================================================
//  패킷 구조체
// ============================================================

#pragma pack(1)

// 공통 헤더 — 모든 패킷 앞에 붙는 4바이트
struct PacketHeader {
    uint16_t size;      // 헤더 포함 전체 패킷 크기
    uint8_t  type;      // PacketType
    uint8_t  playerId;  // 송신자 ID (0~2)
};

// 플레이어 상태 페이로드 (PKT_PLAYER_STATE)
struct PlayerStateData {
    int16_t  x, y;          // 위치
    int16_t  vx, vy;        // 속도
    uint8_t  form;          // MarioForm
    uint8_t  flags;         // 비트플래그 (아래 PlayerStateFlags 참고)
    uint8_t  walkMotion;    // 걷기 애니메이션 프레임
    int16_t  coin;          // 개인 코인 수
    // life 없음 — g_sharedLives(공용 목숨)로 대체
};

// PlayerStateData.flags 비트 정의
enum PlayerStateFlags : uint8_t {
    FLAG_JUMPING      = 1 << 0,
    FLAG_FLYING       = 1 << 1,
    FLAG_DEAD         = 1 << 2,
    FLAG_STAR         = 1 << 3,
    FLAG_TRANSFORMING = 1 << 4,
    FLAG_INVINCIBLE   = 1 << 5,
};

// 게임 이벤트 페이로드 (PKT_GAME_EVENT)
struct GameEventData {
    uint8_t eventType;  // GameEventType
    uint8_t param;      // 이벤트별 부가 데이터
};

enum GameEventType : uint8_t {
    EVT_STAGE_CLEAR   = 1,  // 스테이지 클리어 (호스트 → 전체)
    EVT_GAME_OVER     = 2,  // 게임오버 (호스트 → 전체)
    EVT_GAME_START    = 3,  // 3인 접속 완료 → 게임 시작 (호스트 → 전체)
    EVT_LIVES_CHANGED = 4,  // 공용 목숨 변경 (호스트 → 전체), param = 새 목숨 수
    EVT_DISCONNECT    = 5,  // 플레이어 접속 끊김, param = playerId
    EVT_PLAYER_ARRIVED = 6, // 플레이어 클리어 지점 도착 (클라이언트 → 호스트)
};

// 접속 확인 페이로드 (PKT_CONNECT_ACK, 호스트 → 클라이언트)
struct ConnectAckData {
    uint8_t  assignedId;    // 부여된 playerId (1 또는 2)
    int32_t  sharedLives;   // 현재 공용 목숨
};

// 몬스터 상태 페이로드 (PKT_MONSTER_STATE)
// TODO: B 담당자와 MonsterContext 필드 확정 후 작성

#pragma pack()


// ============================================================
//  패킷 타입 열거
// ============================================================

enum PacketType : uint8_t {
    PKT_PLAYER_STATE  = 1,  // 위치·속도·폼·플래그 (매 프레임)
    PKT_ITEM_SPAWN    = 3,  // 아이템 생성 (호스트 → 전체)
    PKT_ITEM_PICKUP   = 4,  // 아이템 획득 요청/확정
    PKT_MONSTER_STATE = 5,  // 몬스터 배열 스냅샷 (호스트 → 전체, 5프레임마다)
    PKT_GAME_EVENT    = 6,  // 스테이지 클리어·게임오버·목숨 변경 등
    PKT_CONNECT_ACK   = 7,  // 접속 확인 + ID 부여 + 초기 상태
    PKT_PING          = 8,  // 연결 유지 확인
};


// ============================================================
//  네트워크 전역 상태
// ============================================================

struct NetContext {
    bool   isHost;
    int    myPlayerId;          // 0~2 (호스트=0)
    int    connectedCount;      // 현재 접속 인원 (호스트 자신 포함)
    SOCKET listenSock;          // 호스트 전용 listen 소켓
    SOCKET clientSockets[2];    // 호스트 전용 — 클라이언트 소켓 슬롯
    SOCKET hostSocket;          // 클라이언트 전용 — 호스트 연결 소켓
};

extern NetContext g_net;
extern int       g_sharedLives; // 공용 목숨 — 호스트가 관리, PKT_GAME_EVENT로 동기화


// ============================================================
//  공개 함수 선언
// ============================================================

// 초기화 / 종료
void NetInit(bool isHost, const char* hostIp = nullptr);
void NetShutdown();

// 로비: accept 폴링 (호스트 전용, GAME_LOBBY 루프에서 매 프레임 호출)
void NetPollAccept();

// 수신 큐 소비 (메인 스레드, UpdateGame() 첫 줄에서 호출)
void NetPollPackets();

// 송신
void SendToAll(const void* data, int size);             // 호스트 전용
void SendToHost(const void* data, int size);            // 클라이언트 전용
void SendToOne(int playerId, const void* data, int size); // 특정 플레이어

// 수신 큐 (내부용 — RecvThread에서 호출)
void PushRecvPacket(std::vector<uint8_t> pkt);
bool PopRecvPacket(std::vector<uint8_t>& out);
