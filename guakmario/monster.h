#pragma once
#include "item.h"
#define MAX_MONSTERS 100
#define MAX_TURTLES 100
#define MAX_BOWSER 1
#define MAX_FIREBALLS 100

struct Monster
{
    int x, y;
    int vx, vy;
    int width, height;
    int leftBound, rightBound;
    bool isFalling;
    bool isAlive;
    bool isDead;
    bool active;
    DWORD deadstart;
};

//turtle
typedef enum { NORMAL, SHELL, SPINNING } TurtleState;

struct Turtle
{
    int x, y;
    int vx, vy;
    int width, height;
    int direction;      // 0: left  1: right
    bool isFalling;
    bool isAlive;
    bool isDead;
    bool active;
    bool damage;

    TurtleState turtleState;
    int shellTimer;
    int damageTimer;
};

//angel turtle
enum { FLYING, HIDE };

struct AngelTurtle 
{
    int x, y;
    int width, height;
    int vy;
    int topY, bottomY;
    bool isFalling;
    bool isDead;
    bool goingUp;
    bool isAlive;
    int state;
    DWORD hideStartTime;
};

//boss bowser

struct Bowser 
{
    int hp;
    int x, y;
    int vx, vy;
    int width, height;
    bool ignore_tinofire;
    bool ignore_tinobite;
    bool isAlive;
    bool isDead;
    bool isJumping;
    bool isFalling;
    bool isFiring;
    int fireTimer;// 추가: 불 공격 중인지 여부
    int frame;

    int jumpTimer;  //랜덤하게 점프하기
    int jumpInterval;

    int startX;            // 시작 위치 x 좌표
    int direction;         // 이동 방향 (-1: 왼쪽, 1: 오른쪽)
    int moveDistance;      // 이동한 거리 누적
    int maxDistance;

    int fireInterval;

    int fireDuration; // 불 애니메이션 유지 시간용
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

void InitMonsters();
void InitMonsters2();
void InitMonsters3();
void InitTurtles();
void InitTurtles2();
void InitTurtles3();
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
