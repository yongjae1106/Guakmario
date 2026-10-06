#pragma once
#include "data.h"

enum MarioForm { FORM_SMALL, FORM_BIG, FORM_FLOWER, FORM_TINO };

struct Player
{
    int x = 100, y = 300;
    int vx = 0, vy = 0;
    int life = 5;
    int coin = 0;
    int width = 40, height = 40;
    int direction = 1;
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
};

struct PlayerContext {
    Player mario;
    bool keyState[256] = {};
};
extern PlayerContext g_player;

void movePlayer();
void UpdatePlayer();
void UpdateMario_motion();
void UpdateDeadMotion();
void dead();
void resurrection();
void ResetMario(int life, int coin);
