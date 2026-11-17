#pragma once
#include "data.h"
#include "config.h"

struct Player
{
    int x = 100, y = 300;
    int vx = 0, vy = 0;
    int life = PLAYER_INITIAL_LIFE;
    int coin = 0;
    int width = 40, height = 40;
    Direction direction = DIR_RIGHT;
    int walk_motion = 0;
    int motion_timer = 0;
    int tino_cooldown_z = 0;
    int tino_cooldown_space = 0;
    bool isJumping = false;
    bool isFlying = false;
    bool isWalking = false;
    bool isDead = false;
    bool gameover = false;
    MarioForm form = FORM_SMALL;
    bool god = false;
    bool star = false;
    bool fire_motion = false;
    bool tino_motion = false;
    bool tino_fire_motion = false;
    bool supergod = false;
    bool isTransforming = false;
    MarioForm transformTarget = FORM_SMALL;
    CharacterType charType = CHAR_MARIO;
};

struct PlayerContext {
    Player mario;
    bool keyState[256] = {};
};
// 멀티플레이어 배열 (싱글: [0]만 사용, 멀티: [0~2])
extern PlayerContext g_allPlayers[3];
extern int           g_localPlayerIdx;   // 현재 로컬 조작 대상 (기본 0)
#define g_player g_allPlayers[g_localPlayerIdx]

// 렌더러가 필요한 데이터만 담는 뷰 — Player 내부 구현(god/vx/vy 등)을 노출하지 않음
struct MarioRenderData {
    int x, y, width, height; Direction direction;
    MarioForm form;
    CharacterType charType;
    bool isWalking, isJumping, isFlying, isDead;
    bool star, fire_motion, tino_motion, tino_fire_motion;
    int walk_motion, motion_timer;
    int life, coin;
    int tino_cooldown_z, tino_cooldown_space;
};
MarioRenderData BuildMarioRenderData();
MarioRenderData BuildMarioRenderDataFor(int idx);  // 렌더 테스트용 — 임의 슬롯 빌드

void movePlayer();
void UpdatePlayer();
void UpdateMario_motion();
void UpdateDeadMotion();
void dead();
void resurrection();
void ResetMario(int life, int coin);

struct PlayerStateData;
// Main thread only. Remote x is stored in world coordinates.
bool ApplyRemotePlayerState(int playerId, const PlayerStateData& state);
