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
    int16_t  worldX, y;         // worldX = x + cameraX (월드 좌표)
    int16_t  vx, vy;            // 속도
    int16_t  coin;              // 개인 코인 수
    uint8_t  life;              // 값 미러링 (기준은 g_sharedLives)
    uint8_t  form;              // MarioForm
    uint8_t  charType;          // CharacterType
    uint8_t  transformTarget;   // 변신 목표 MarioForm
    uint8_t  width, height;
    uint8_t  walkMotion;        // 걷기 애니메이션 프레임
    uint16_t motionTimer;       // 공격 모션 프레임
    uint8_t  tinoCooldownZ, tinoCooldownSpace;
    uint16_t flags;             // PlayerStateFlags
};

// PlayerStateData.flags 비트 정의
enum PlayerStateFlags : uint16_t {
    FLAG_JUMPING          = 1 << 0,
    FLAG_FLYING           = 1 << 1,
    FLAG_DEAD             = 1 << 2,
    FLAG_STAR             = 1 << 3,
    FLAG_TRANSFORMING     = 1 << 4,
    FLAG_INVINCIBLE       = 1 << 5,
    FLAG_WALKING          = 1 << 6,
    FLAG_FACING_RIGHT     = 1 << 7,
    FLAG_FIRE_MOTION      = 1 << 8,
    FLAG_TINO_MOTION      = 1 << 9,
    FLAG_TINO_FIRE_MOTION = 1 << 10,
    FLAG_GAMEOVER         = 1 << 11,
    FLAG_SUPERGOD         = 1 << 12,
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
    EVT_PLAYER_DIED    = 7, // 플레이어 사망 (클라이언트 → 호스트 → 전체), param = playerId
    EVT_PLAYER_REVIVED = 8, // 플레이어 부활 (호스트 → 전체), param = playerId
    EVT_STAGE_LOAD     = 9, // 스테이지 로드 (호스트 → 전체), param = 스테이지 번호
};

// 접속 확인 페이로드 (PKT_CONNECT_ACK, 호스트 → 클라이언트)
struct ConnectAckData {
    uint8_t  assignedId;    // 부여된 playerId (1 또는 2)
    int32_t  sharedLives;   // 현재 공용 목숨
};

// 발사체 생성 (PKT_SHOT_SPAWN) — 발사자는 헤더의 playerId
enum ShotType : uint8_t { SHOT_FIREBALL = 0, SHOT_TINOFIRE = 1 };

struct ShotSpawnData {
    uint8_t shotType, slot, direction;
    int16_t x, y, vx, vy;
    uint8_t width, height;
};

// 아이템 생성 (PKT_ITEM_SPAWN, 호스트 → 전체)
enum ItemType : uint8_t {
    ITEM_MUSHROOM = 0, ITEM_UP_MUSHROOM = 1, ITEM_STAR = 2, ITEM_FLOWER = 3, ITEM_TINO = 4,
};

enum ItemSpawnFlags : uint8_t {
    ITEM_FLAG_ACTIVE = 1 << 0,
    ITEM_FLAG_MOTION = 1 << 1,
};

struct ItemSpawnData {
    uint8_t itemType, slot, spawnMotion;
    uint8_t flags;      // ItemSpawnFlags
    int16_t x, y, vx, vy;
    uint8_t width, height;
};

// 아이템 획득 (PKT_ITEM_PICKUP, 요청 → 호스트 → 확정 전체)
struct ItemPickupData {
    uint8_t itemType, slot;
    uint8_t pickerId;   // 획득 요청자
    uint8_t approved;   // 요청 0, 호스트 확정 1
};

// 맵 타일 변경 (PKT_TILE_CHANGE, 호스트 → 전체)
struct TileChangeData {
    uint8_t  row, col;
    uint16_t tile;      // TILE_INVISIBLE = 999
};

// 블록 타격 (PKT_BLOCK_HIT, 클라이언트 → 호스트)
struct BlockHitData {
    uint8_t row, col;
};

// 몬스터 종류
enum MonsterKind : uint8_t {
    MON_NORMAL = 0, MON_TURTLE = 1, MON_BROWN_TURTLE = 2,
    MON_ANGEL_TURTLE = 3, MON_FIREBALL = 4, MON_BOSS = 5,
};

enum HitType : uint8_t {
    HIT_STOMP = 0, HIT_FIREBALL = 1, HIT_TINOFIRE = 2,
    HIT_TINOBITE = 3, HIT_STAR = 4, HIT_SHELL = 5,
};

// 몬스터 타격 (PKT_MONSTER_HIT, 클라이언트 → 호스트)
struct MonsterHitData {
    uint8_t kind;       // MonsterKind
    uint8_t slot;
    uint8_t hitType;    // HitType
};

// 몬스터 상태 (PKT_MONSTER_STATE, 호스트 → 전체) — 종류별 1패킷, 활성 개체만
// 패킷에 없는 슬롯은 해당 종류의 비활성으로 처리
struct MonsterStateHeader {
    uint8_t kind;       // MonsterKind
    uint8_t count;      // 뒤따르는 MonsterEntry 개수
};

enum MonsterEntryFlags : uint8_t {
    MON_FLAG_ALIVE       = 1 << 0,
    MON_FLAG_DEAD        = 1 << 1,
    MON_FLAG_ACTIVE      = 1 << 2,
    MON_FLAG_FALLING     = 1 << 3,
    MON_FLAG_DAMAGE      = 1 << 4,
    MON_FLAG_GOING_UP    = 1 << 5,
    MON_FLAG_FACING_LEFT = 1 << 6,
};

// 개발일정의 MonsterStateData: 몬스터 한 개체의 전송 상태.
struct MonsterStateData {
    uint8_t  slot;
    int16_t  x, y, vx, vy;
    uint8_t  width, height;
    uint8_t  state;     // TurtleState 또는 AngelTurtle state
    uint8_t  flags;     // MonsterEntryFlags
    uint16_t timerA;    // MON_NORMAL·ANGEL: 사망/숨기 경과 ms, TURTLE: SHELL 경과 ms, FIREBALL: motion
    uint16_t timerB;    // TURTLE: SPINNING 피해 대기 경과 ms (시작 시각 아님)
};

using MonsterEntry = MonsterStateData; // 기존 A/C 코드 이름과 호환
static_assert(sizeof(MonsterStateData) == 17, "Monster state wire size changed");
static_assert(sizeof(MonsterStateHeader) == 2, "Monster header wire size changed");
static_assert(sizeof(PacketHeader) == 4, "Packet header wire size changed");

// 보스 상태 (PKT_BOSS_STATE, 호스트 → 전체)
enum BossStateFlags : uint8_t {
    BOSS_FLAG_ALIVE       = 1 << 0,
    BOSS_FLAG_DEAD        = 1 << 1,
    BOSS_FLAG_ACTIVE      = 1 << 2,
    BOSS_FLAG_FALLING     = 1 << 3,
    BOSS_FLAG_IGNORE_FIRE = 1 << 4,
    BOSS_FLAG_IGNORE_BITE = 1 << 5,
    BOSS_FLAG_JUMPING     = 1 << 6,
    BOSS_FLAG_FIRING      = 1 << 7,
};

struct BossStateData {
    int16_t  x, y, vx, vy, hp;
    int16_t  startX, moveDistance, maxDistance;
    uint8_t  width, height;
    uint8_t  flags;     // BossStateFlags
    uint8_t  frame;
    int8_t   moveSign;
    uint16_t fireTimer, fireDuration, fireInterval;
    uint16_t jumpTimer, jumpInterval;
    int16_t  fireX, fireY, fireVx, fireVy;  // bowserFire
    uint8_t  fireWidth, fireHeight, fireMotion, fireActive;
};

// 함정 상태 (PKT_HAZARD_STATE, 호스트 → 전체)
struct HazardStateHeader {
    uint8_t count;      // 뒤따르는 HazardEntry 개수
};

struct HazardEntry {
    uint8_t slot, tileCount, active;
    int16_t x, y, width, height;
};

// 게임 동기화 (PKT_GAME_SYNC, 호스트 → 전체, 1초마다)
struct GameSyncData {
    uint8_t stage;
    uint8_t gameState;  // GameState
    uint8_t sharedLives;
    uint8_t frameMotion, frameMotionStar;
    int16_t stageTime;
};

// 연결 유지 (PKT_PING)
struct PingData {
    uint32_t timestamp;
};

#pragma pack()


// ============================================================
//  패킷 타입 열거
// ============================================================

enum PacketType : uint8_t {
    PKT_PLAYER_STATE  = 1,  // 위치·속도·폼·플래그 (매 프레임)
    PKT_SHOT_SPAWN    = 2,  // 발사체 생성 (전원 → 호스트 → 나머지)
    PKT_ITEM_SPAWN    = 3,  // 아이템 생성 (호스트 → 전체)
    PKT_ITEM_PICKUP   = 4,  // 아이템 획득 요청/확정
    PKT_MONSTER_STATE = 5,  // 몬스터 배열 스냅샷 (호스트 → 전체, 5프레임마다)
    PKT_GAME_EVENT    = 6,  // 스테이지 클리어·게임오버·목숨 변경 등
    PKT_CONNECT_ACK   = 7,  // 접속 확인 + ID 부여 + 초기 상태
    PKT_PING          = 8,  // 연결 유지 확인
    PKT_TILE_CHANGE   = 9,  // 맵 타일 변경 (호스트 → 전체)
    PKT_BLOCK_HIT     = 10, // 블록 타격 (클라이언트 → 호스트)
    PKT_MONSTER_HIT   = 11, // 몬스터 타격 (클라이언트 → 호스트)
    PKT_BOSS_STATE    = 12, // 보스 상태 (호스트 → 전체, 5프레임마다)
    PKT_HAZARD_STATE  = 13, // 함정 상태 (호스트 → 전체, 5프레임마다)
    PKT_GAME_SYNC     = 14, // 스테이지·시간·목숨 (호스트 → 전체, 1초마다)
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
    char   localIp[16];         // 로컬 IP 문자열 (예: "192.168.0.5")
};

extern NetContext g_net;
extern int       g_sharedLives; // 공용 목숨 — 호스트가 관리, PKT_GAME_EVENT로 동기화


// ============================================================
//  공개 함수 선언
// ============================================================

// 초기화 / 종료
void NetFetchLocalIp();  // WSAStartup + IP 조회만 (소켓 미생성)
void NetInit(bool isHost, const char* hostIp = nullptr);
void NetShutdown();

// 로비: accept 폴링 (호스트 전용, GAME_LOBBY 루프에서 매 프레임 호출)
void NetPollAccept();
void NetBroadcastGameStart();  // connectedCount == 3 → GAME_START 브로드캐스트 + listen 소켓 닫기

// 수신 큐 소비 (메인 스레드, UpdateGame() 첫 줄에서 호출)
void NetPollPackets();

// 자신의 플레이어 상태 직렬화 → 송신 (UpdateGame() 마지막에 호출)
void NetFlushSend();

// 송신
void SendToAll(const void* data, int size);             // 호스트 전용
void SendToHost(const void* data, int size);            // 클라이언트 전용
void SendToOne(int playerId, const void* data, int size); // 특정 플레이어

// 수신 큐 (내부용 — RecvThread에서 호출)
void PushRecvPacket(std::vector<uint8_t> pkt);
bool PopRecvPacket(std::vector<uint8_t>& out);

// B: 종류별 완전 스냅샷 인코딩/디코딩. 실패 시 출력 인자는 유지한다.
// kind 2는 예약: 빈 스냅샷만 허용. 보스는 별도 PKT_BOSS_STATE 사용.
// Parse 호출 전 실제 호스트 연결에서 수신했는지 호출자가 확인해야 한다.
bool BuildMonsterStatePacket(uint8_t kind, const std::vector<MonsterStateData>& states,
    std::vector<uint8_t>& packet);
bool ParseMonsterStatePacket(const std::vector<uint8_t>& packet, uint8_t& kind,
    std::vector<MonsterStateData>& states);
