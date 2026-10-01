#pragma once
#include "data.h"

struct Player
{
    int x, y;
    int vx, vy;
    int life;
    int coin;
    int width, height;
    int direction;      // 0: left  1: right
    int walk_motion;
    int motion_timer;
    int tino_cooldown_z;
    int tino_cooldown_space;
    bool isJumping;
    bool isflying;
    bool isWalking;
    bool isDead;
    bool gameover;
    bool isBig;
    bool god;
    bool star;
    bool flower;
    bool fire_motion;
    bool tino;
    bool tino_motion;
    bool tino_fire_motion;
    bool supergod;
};
extern Player mario;
extern bool keyState[256];

void movePlayer();
void UpdatePlayer();
void UpdateMario_motion();
void UpdateDeadMotion();
void dead();
void resurrection();
