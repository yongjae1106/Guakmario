#pragma once
#include "item.h"
#define MAX_MONSTERS 100
#define MAX_TURTLES 100
#define MAX_BOWSER 1
#define MAX_FIREBALLS 100

struct Entity
{
    int x = 0, y = 0;
    int vx = 0, vy = 0;
    int width = 0, height = 0;
    bool isAlive = false;
    bool isDead = false;
    bool active = false;
    bool isFalling = false;
};

struct Monster : Entity
{
    int leftBound = 0, rightBound = 0;
    DWORD deadStart = 0;
};

//turtle
typedef enum { NORMAL, SHELL, SPINNING } TurtleState;

struct Turtle : Entity
{
    Direction direction = DIR_LEFT;
    bool damage = false;

    TurtleState turtleState = NORMAL;
    int shellTimer = 0;
    int damageTimer = 0;
};

//angel turtle
enum { FLYING, HIDE };

struct AngelTurtle : Entity
{
    int topY = 0, bottomY = 0;
    bool goingUp = false;
    int state = FLYING;
    DWORD hideStartTime = 0;
};

//boss bowser

struct Bowser : Entity
{
    int hp = 0;
    bool ignoreTinoFire = false;
    bool ignoreTinoBite = false;
    bool isJumping = false;
    bool isFiring = false;
    int fireTimer = 0;// 추가: 불 공격 중인지 여부
    int frame = 0;

    int jumpTimer = 0;  //랜덤하게 점프하기
    int jumpInterval = 0;

    int startX = 0;            // 시작 위치 x 좌표
    int moveSign = 1;          // 이동 배율: 1 = 오른쪽, -1 = 왼쪽 (속도에 곱해서 사용)
    int moveDistance = 0;      // 이동한 거리 누적
    int maxDistance = 0;

    int fireInterval = 0;

    int fireDuration = 0; // 불 애니메이션 유지 시간용
};
//fireball
struct Fireball 
{
    int x, y;
    int vx, vy;
    int width, height;
    int motion;
    bool active;
};

struct MonsterState {
    Monster     monsters[MAX_MONSTERS]       = {};
    int         monsterCount                 = 0;
    Turtle      turtles[MAX_TURTLES]         = {};
    int         turtleCount                  = 0;
    Turtle      brownTurtles[MAX_TURTLES]    = {};
    int         brownTurtleCount             = 0;
    AngelTurtle angelTurtles[MAX_TURTLES]    = {};
    int         angelTurtleCount             = 0;
    Fireball    fireballs[MAX_FIREBALLS]     = {};
    Bowser      bowser                       = {};
    Fireball    bowserFire                   = {};
};
extern MonsterState g_monsters;

void InitMonsters(int stage);
void InitTurtles(int stage);
void InitAngelTurtles();
void InitBowser();

void UpdateMonsters();
void CheckMarioMonsterCollision();

void UpdateTurtles();
void CheckMarioTurtleCollision();

void UpdateAngelTurtles();
void CheckMarioAngelTurtleCollision();

void UpdateBowser();
void UpdateFireballs();
void CheckMarioBowserCollision();
void CheckMarioFireballCollision();

void damage_mario();

void UpdateAllMonsters();
void CheckAllMonsterCollisions();
