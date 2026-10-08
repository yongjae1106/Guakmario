#pragma once
#include "data.h"
#include "player.h"
#include "game.h"
#include "map.h"
#include "collision.h"
#include "image.h"
#include "monster.h"
#include "sound.h"

MonsterState g_monsters;

static void InitMonstersFromData(const int* tileX, const int* tileY, int count, int w, int h)
{
    g_monsters.monsterCount = count;
    for (int i = 0; i < g_monsters.monsterCount; i++)
    {
        g_monsters.monsters[i].x = tileX[i] * TILE_SIZE;
        g_monsters.monsters[i].y = tileY[i] * TILE_SIZE;
        g_monsters.monsters[i].vx = -1;  // 왼쪽으로 이동
        g_monsters.monsters[i].vy = 0;
        g_monsters.monsters[i].width = w;
        g_monsters.monsters[i].height = h;
        g_monsters.monsters[i].isFalling = false;
        g_monsters.monsters[i].isAlive = true;
        g_monsters.monsters[i].isDead = false;
        g_monsters.monsters[i].active = true;
    }
}

void InitMonsters(int stage)
{
    static const int tileX1[] = { 15, 28, 35,42,48,54,65,73,76,91,94,98,99,100,101,102,103,104 };
    static const int tileY1[] = { 12, 12, 12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12 };
    static const int tileX2[] = { 30,31,32,35,99,100,101,50,51,52 };
    static const int tileY2[] = { 7,7,7,5,4,4,4,6,6,6 };
    static const int tileX3[] = { 34,35,30,31,32,95,96,97,73,75,76 };
    static const int tileY3[] = { 9,9,9,9,9,9,9,9,9,9,9 };

    if      (stage == 1) InitMonstersFromData(tileX1, tileY1, 18, 20, 10);
    else if (stage == 2) InitMonstersFromData(tileX2, tileY2, 10, 20, 20);
    else if (stage == 3) InitMonstersFromData(tileX3, tileY3, 11, 20, 20);
}

static void InitTurtlesFromData(const int* tileX, const int* tileY, int count)
{
    g_monsters.turtleCount = count;
    for (int i = 0; i < count; i++)
    {
        g_monsters.turtles[i] = {};
        g_monsters.turtles[i].x         = tileX[i] * TILE_SIZE;
        g_monsters.turtles[i].y         = tileY[i] * TILE_SIZE;
        g_monsters.turtles[i].vx        = -1;
        g_monsters.turtles[i].width     = 20;
        g_monsters.turtles[i].height    = 20;
        g_monsters.turtles[i].direction = DIR_LEFT;
        g_monsters.turtles[i].isAlive   = true;
        g_monsters.turtles[i].active    = true;
        g_monsters.turtles[i].turtleState = NORMAL;
    }
}

void InitTurtles(int stage)
{
    static const int tileX1[] = { 10, 33, 35, 50, 65 };
    static const int tileY1[] = { 12, 12, 12, 12, 12 };
    static const int tileX2[] = { 17, 33, 35, 50, 65 };
    static const int tileY2[] = { 5,  12, 12, 12, 12 };
    static const int tileX3[] = { 60 };
    static const int tileY3[] = { 9  };

    if      (stage == 1) InitTurtlesFromData(tileX1, tileY1, 5);
    else if (stage == 2) InitTurtlesFromData(tileX2, tileY2, 5);
    else if (stage == 3) InitTurtlesFromData(tileX3, tileY3, 1);
}
void InitAngelTurtles()
{
    if (g_game.stage != 2) return;
    g_monsters.angelTurtleCount = 2;

    int tileX[] = { 21, 28 };
    int tileY[] = { 5, 5 };

    for (int i = 0; i < g_monsters.angelTurtleCount; i++) 
    {
        g_monsters.angelTurtles[i].x = tileX[i] * TILE_SIZE;
        g_monsters.angelTurtles[i].y = tileY[i] * TILE_SIZE;
        g_monsters.angelTurtles[i].width = 40;
        g_monsters.angelTurtles[i].height = 60;
        g_monsters.angelTurtles[i].vy = 1;
        g_monsters.angelTurtles[i].topY = g_monsters.angelTurtles[i].y - 30;  // 위쪽 이동 한계
        g_monsters.angelTurtles[i].bottomY = g_monsters.angelTurtles[i].y + 30;  // 아래쪽 이동 한계
        g_monsters.angelTurtles[i].goingUp = false;
        g_monsters.angelTurtles[i].isAlive = true;
        g_monsters.angelTurtles[i].isDead = false;
        g_monsters.angelTurtles[i].state = FLYING;
    }
}

void InitBowser() 
{
    g_monsters.bowser.hp = 100;
    g_monsters.bowser.x = 125 * TILE_SIZE;  // 보서의 시작 위치 (오른쪽 끝)
    g_monsters.bowser.y = 9 * TILE_SIZE;   // 맵 바닥에 위치하도록
    g_monsters.bowser.vx = 0;
    g_monsters.bowser.vy = 0;
    g_monsters.bowser.width = 120;
    g_monsters.bowser.height = 120;
    g_monsters.bowser.isAlive = true;
    g_monsters.bowser.isDead = false;
    g_monsters.bowser.isJumping = false;
    g_monsters.bowser.jumpTimer = 0;

    g_monsters.bowser.startX = g_monsters.bowser.x;
    g_monsters.bowser.moveSign = 1;
    g_monsters.bowser.moveDistance = 0;
    g_monsters.bowser.maxDistance = 10 * TILE_SIZE;  // 6칸
    g_monsters.bowser.jumpTimer = 0;
    g_monsters.bowser.jumpInterval = 60 + rand() % 121;  // 60~180프레임마다

    g_monsters.bowser.jumpTimer = 0;
    g_monsters.bowser.jumpInterval = 60 + rand() % 121;

    g_monsters.bowser.fireTimer = 0;
    g_monsters.bowser.fireInterval = 60 + rand() % 60; // 1~2초 간격

    g_monsters.bowser.isFiring = false;
    g_monsters.bowser.fireDuration = 0;
}

//monster
// 충돌처리
void UpdateMonsters()
{
    for (int i = 0; i < g_monsters.monsterCount; i++)
    {
        if (!g_monsters.monsters[i].isAlive) continue;

        if (g_monsters.monsters[i].isFalling)
        {
            g_monsters.monsters[i].x += g_monsters.monsters[i].vx;
            g_monsters.monsters[i].vy += 1;
            if (g_monsters.monsters[i].vy > 10) g_monsters.monsters[i].vy = 10;
            g_monsters.monsters[i].y += g_monsters.monsters[i].vy;
            if (g_monsters.monsters[i].isDead && g_monsters.monsters[i].y > SCREEN_HEIGHT + 100)
                g_monsters.monsters[i].isAlive = false;  // 화면 밖 낙하 완료 → 정리
            continue;
        }
        
        // 화면 안에 있는 몬스터만 처리
        int screenX = g_monsters.monsters[i].x - g_game.cameraX;
        if (screenX + TILE_SIZE < 0 || screenX > SCREEN_WIDTH) continue;




        // 화면 밖 처리 (낙하 체크)
        if (g_game.stage > 1)
        {
            int nextX = g_monsters.monsters[i].x + g_monsters.monsters[i].vx;
            int footX = (g_monsters.monsters[i].vx < 0) ? nextX + 5 : nextX + g_monsters.monsters[i].width;
            int footTileX = footX / TILE_SIZE;
            int footTileY = g_monsters.monsters[i].y / TILE_SIZE + 1;

            bool willFall = true;
            if (footTileX >= 0 && footTileX < MAP_WIDTH && footTileY >= 0 && footTileY < MAP_HEIGHT)
            {
                int tile = currentMap[footTileY][footTileX];
                if (tile != 0 && tile != 2)
                    willFall = false;
            }

            if (willFall)
            {
                g_monsters.monsters[i].vx = -g_monsters.monsters[i].vx;
            }
        }
        g_monsters.monsters[i].x += g_monsters.monsters[i].vx;


        // 타일 좌표 계산
        int left = g_monsters.monsters[i].x / TILE_SIZE;
        int right = (g_monsters.monsters[i].x + TILE_SIZE - 1) / TILE_SIZE;
        int top = g_monsters.monsters[i].y / TILE_SIZE;
        int bottom = (g_monsters.monsters[i].y + TILE_SIZE - 1) / TILE_SIZE;
        int middle = (g_monsters.monsters[i].y + g_monsters.monsters[i].height / 2 - 1) / TILE_SIZE;

        // 벽 또는 낭떠러지 감지 시 방향 전환
        if (g_monsters.monsters[i].vx < 0 &&
            (isSolidTile(currentMap[top][left]) || isSolidTile(currentMap[bottom][left]) || isSolidTile(currentMap[middle][left])))
        {
            g_monsters.monsters[i].x = (left + 1) * TILE_SIZE; // 위치 조정
            g_monsters.monsters[i].vx = -g_monsters.monsters[i].vx;      // 방향 반전
        }
        else if (g_monsters.monsters[i].vx > 0 &&
            (isSolidTile(currentMap[top][right]) || isSolidTile(currentMap[bottom][right]) || isSolidTile(currentMap[middle][right])))
        {
            g_monsters.monsters[i].x = right * TILE_SIZE - g_monsters.monsters[i].width; // 위치 조정
            g_monsters.monsters[i].vx = -g_monsters.monsters[i].vx;                       // 방향 반전
        }

        // 낭떠러지면 낙하처리
        int underX = (g_monsters.monsters[i].x + TILE_SIZE / 2) / TILE_SIZE;
        int underY = (g_monsters.monsters[i].y + TILE_SIZE) / TILE_SIZE;

        if (underX < 0 || underX >= MAP_WIDTH || underY >= MAP_HEIGHT || currentMap[underY][underX] == TILE_EMPTY)
        {
            g_monsters.monsters[i].vy += 1;
            if (g_monsters.monsters[i].vy > 10) g_monsters.monsters[i].vy = 10;
            g_monsters.monsters[i].y += g_monsters.monsters[i].vy;
            g_monsters.monsters[i].isFalling = true;
        }
        else
        {
            g_monsters.monsters[i].vy = 0;
            g_monsters.monsters[i].isFalling = false;
        }

    }
}
void HandleMarioMonsterCollision()
{
    for (int i = 0; i < g_monsters.monsterCount; i++)
    {
        if (!g_monsters.monsters[i].isAlive || g_monsters.monsters[i].isDead) continue;

        int marioLeft = g_player.mario.x;
        int marioRight = g_player.mario.x + g_player.mario.width;
        int marioTop = g_player.mario.y;
        int marioBottom = g_player.mario.y + g_player.mario.height;

        int monsterLeft = g_monsters.monsters[i].x - g_game.cameraX;
        int monsterRight = monsterLeft + g_monsters.monsters[i].width;
        int monsterTop = g_monsters.monsters[i].y;
        int monsterBottom = monsterTop + g_monsters.monsters[i].height;
        // 밟기 판정
        if (marioBottom >= monsterTop && marioTop < monsterTop &&
            marioRight > monsterLeft && marioLeft < monsterRight &&
            g_player.mario.vy > 0)
        {
            PlaySoundBuffer(stomp_Sound);
            g_monsters.monsters[i].isAlive = false;
            g_player.mario.vy = -10;
        }
        // 측면 충돌
        else if (IsColliding(g_player.mario.x, g_player.mario.y, g_player.mario.width, g_player.mario.height,
            g_monsters.monsters[i].x - g_game.cameraX, g_monsters.monsters[i].y + 20, g_monsters.monsters[i].width, g_monsters.monsters[i].height) &&
            !g_monsters.monsters[i].isDead &&
            g_monsters.monsters[i].active)
        {
            // 스타 파워면 튕겨내기 처리
            if (g_player.mario.star)
            {
                PlaySoundBuffer(kick_Sound);
                g_monsters.monsters[i].vy = -15;
                g_monsters.monsters[i].isDead = true;
                g_monsters.monsters[i].isFalling = true;
            }
            else
            {
                damage_mario();
            }
        }
    }
}

//turtle
void UpdateTurtles()
{
    for (int i = 0; i < g_monsters.turtleCount; i++)
    {
        if (!g_monsters.turtles[i].isAlive) continue;
        if (g_monsters.turtles[i].isFalling && g_monsters.turtles[i].turtleState == SPINNING)
        {
            g_monsters.turtles[i].x += g_monsters.turtles[i].vx;
            // 중력 적용
            int underX = (g_monsters.turtles[i].x + TILE_SIZE / 2) / TILE_SIZE;
            int underY = (g_monsters.turtles[i].y + TILE_SIZE) / TILE_SIZE;

            if (underX < 0 || underX >= MAP_WIDTH || underY >= MAP_HEIGHT || currentMap[underY][underX] == TILE_EMPTY)
            {
                g_monsters.turtles[i].vy += 1;
                if (g_monsters.turtles[i].vy > 10) g_monsters.turtles[i].vy = 10;
                g_monsters.turtles[i].y += g_monsters.turtles[i].vy;

                g_monsters.turtles[i].isFalling = true;
            }
            else
            {
                g_monsters.turtles[i].vy = 0;
                g_monsters.turtles[i].isFalling = false;
            }
            if (g_monsters.turtles[i].y > SCREEN_HEIGHT)
                g_monsters.turtles[i].isAlive = false;

            continue;
        }
        else if (g_monsters.turtles[i].isFalling)
        {
            g_monsters.turtles[i].x += g_monsters.turtles[i].vx;
            g_monsters.turtles[i].vy += 1;
            if (g_monsters.turtles[i].vy > 10) g_monsters.turtles[i].vy = 10;
            g_monsters.turtles[i].y += g_monsters.turtles[i].vy;
            continue;
        }
        g_monsters.turtles[i].direction = g_monsters.turtles[i].vx > 0 ? DIR_RIGHT : DIR_LEFT;

        // 낭떠러지 앞에서 방향전환
        if(g_game.stage > 1 && g_monsters.turtles[i].turtleState == NORMAL)
        {
            int nextX = g_monsters.turtles[i].x + g_monsters.turtles[i].vx;
            int footX = (g_monsters.turtles[i].vx < 0) ? nextX : nextX + g_monsters.turtles[i].width;
            int footTileX = footX / TILE_SIZE;
            int footTileY = g_monsters.turtles[i].y / TILE_SIZE + 1;

            bool willFall = true;
            if (footTileX >= 0 && footTileX < MAP_WIDTH && footTileY >= 0 && footTileY < MAP_HEIGHT)
            {
                int tile = currentMap[footTileY][footTileX];
                if (tile != 0 && tile != 2)
                    willFall = false;
            }

            if (willFall)
            {
                g_monsters.turtles[i].vx = -g_monsters.turtles[i].vx;
            }
        }
        // 이동
        g_monsters.turtles[i].x += g_monsters.turtles[i].vx;

        // 중력 적용
        int underX = (g_monsters.turtles[i].x + TILE_SIZE / 2) / TILE_SIZE;
        int underY = (g_monsters.turtles[i].y + TILE_SIZE) / TILE_SIZE;

        if (underX < 0 || underX >= MAP_WIDTH || underY >= MAP_HEIGHT || currentMap[underY][underX] == TILE_EMPTY)
        {
            g_monsters.turtles[i].vy += 1;
            if (g_monsters.turtles[i].vy > 10) g_monsters.turtles[i].vy = 10;
            g_monsters.turtles[i].y += g_monsters.turtles[i].vy;

            g_monsters.turtles[i].isFalling = true;
            continue;
        }
        else
        {
            g_monsters.turtles[i].vy = 0;
            g_monsters.turtles[i].isFalling = false;
        }


        // 벽 충돌
        int left = g_monsters.turtles[i].x / TILE_SIZE;
        int right = (g_monsters.turtles[i].x + TILE_SIZE - 1) / TILE_SIZE;
        int top = g_monsters.turtles[i].y / TILE_SIZE;
        int bottom = (g_monsters.turtles[i].y + TILE_SIZE - 1) / TILE_SIZE;
        int middle = (g_monsters.turtles[i].y + g_monsters.turtles[i].height / 2 - 1) / TILE_SIZE;

        if (g_monsters.turtles[i].vx < 0 &&
            (isSolidTile(currentMap[top][left]) || isSolidTile(currentMap[bottom][left]) || isSolidTile(currentMap[middle][left])))
        {
            g_monsters.turtles[i].x = (left + 1) * TILE_SIZE;
            g_monsters.turtles[i].vx = -g_monsters.turtles[i].vx;
        }
        else if (g_monsters.turtles[i].vx > 0 &&
            (isSolidTile(currentMap[top][right]) || isSolidTile(currentMap[bottom][right]) || isSolidTile(currentMap[middle][right])))
        {
            g_monsters.turtles[i].x = right * TILE_SIZE - g_monsters.turtles[i].width;
            g_monsters.turtles[i].vx = -g_monsters.turtles[i].vx;
        }

        // 쉘 상태 유지 시간 초과
        if (g_monsters.turtles[i].turtleState == SHELL &&
            GetTickCount() - g_monsters.turtles[i].shellTimer > 5000)
        {
            g_monsters.turtles[i].turtleState = NORMAL;
            g_monsters.turtles[i].vx = -1;
        }
        // spinning 상태일 때 잠시 후 데미지 활성화
        if (g_monsters.turtles[i].turtleState == SPINNING && !g_monsters.turtles[i].damage &&
            GetTickCount() - g_monsters.turtles[i].damageTimer > 100)
        {
            g_monsters.turtles[i].damage = true;
        }

        // 회전 껍데기 충돌 처리
        if (g_monsters.turtles[i].turtleState == SPINNING)
        {
            // (1) 몬스터 타격 처리
            for (int j = 0; j < g_monsters.monsterCount; j++)
            {
                if (!g_monsters.monsters[j].isAlive || g_monsters.monsters[j].isFalling) continue;

                if (IsColliding(g_monsters.turtles[i].x, g_monsters.turtles[i].y, g_monsters.turtles[i].width, g_monsters.turtles[i].height,
                    g_monsters.monsters[j].x, g_monsters.monsters[j].y, g_monsters.monsters[j].width, g_monsters.monsters[j].height))
                {
                    PlaySoundBuffer(kick_Sound);
                    g_monsters.monsters[j].isFalling = true;
                    g_monsters.monsters[j].vy = -8;
                    g_monsters.monsters[j].vx = (g_monsters.turtles[i].vx > 0) ? 2 : -2;
                }
            }

            // (2) 다른 거북이와 충돌 처리
            for (int j = 0; j < g_monsters.turtleCount; j++)
            {
                if (i == j) continue;
                if (!g_monsters.turtles[j].isAlive || g_monsters.turtles[j].isFalling) continue;
                if (g_monsters.turtles[j].turtleState != NORMAL) continue;

                if (IsColliding(g_monsters.turtles[i].x, g_monsters.turtles[i].y, g_monsters.turtles[i].width, g_monsters.turtles[i].height,
                    g_monsters.turtles[j].x, g_monsters.turtles[j].y, g_monsters.turtles[j].width, g_monsters.turtles[j].height))
                {
                    PlaySoundBuffer(kick_Sound);
                    g_monsters.turtles[j].isFalling = true;
                    g_monsters.turtles[j].vy = -8;
                    g_monsters.turtles[j].vx = (g_monsters.turtles[i].vx > 0) ? 2 : -2;
                }
            }

            // (3) 벽과 충돌 시 방향 전환
            if (g_monsters.turtles[i].vx < 0 &&
                (isSolidTile(currentMap[top][left]) || isSolidTile(currentMap[bottom][left]) || isSolidTile(currentMap[middle][left])))
            {
                g_monsters.turtles[i].x = (left + 1) * TILE_SIZE;
                g_monsters.turtles[i].vx = -g_monsters.turtles[i].vx;
            }
            else if (g_monsters.turtles[i].vx > 0 &&
                (isSolidTile(currentMap[top][right]) || isSolidTile(currentMap[bottom][right]) || isSolidTile(currentMap[middle][right])))
            {
                g_monsters.turtles[i].x = right * TILE_SIZE - g_monsters.turtles[i].width;
                g_monsters.turtles[i].vx = -g_monsters.turtles[i].vx;
            }
        }
    }
}
void HandleMarioTurtleCollision()
{
    for (int i = 0; i < g_monsters.turtleCount; i++)
    {
        if (!g_monsters.turtles[i].isAlive) continue;

        int marioLeft = g_player.mario.x;
        int marioRight = g_player.mario.x + g_player.mario.width;
        int marioTop = g_player.mario.y;
        int marioBottom = g_player.mario.y + g_player.mario.height;

        int turtleLeft = g_monsters.turtles[i].x - 10 - g_game.cameraX;
        int turtleRight = turtleLeft + g_monsters.turtles[i].width + 20;
        int turtleTop = g_monsters.turtles[i].y;
        int turtleBottom = turtleTop + g_monsters.turtles[i].height;

        if (marioBottom >= turtleTop && marioTop < turtleTop &&
            marioRight > turtleLeft && marioLeft < turtleRight &&
            g_player.mario.vy > 0)
        {
            if (g_monsters.turtles[i].turtleState == NORMAL) // 걷는 상태일 때
            {
                PlaySoundBuffer(stomp_Sound);
                g_monsters.turtles[i].turtleState = SHELL;
                g_monsters.turtles[i].vx = 0;
                g_monsters.turtles[i].shellTimer = GetTickCount();
            }
            else if (g_monsters.turtles[i].turtleState == SHELL) // 껍데기 상태를 걷어차기
            {
                PlaySoundBuffer(kick_Sound);
                int marioCenter = g_player.mario.x + g_player.mario.width / 2;
                int turtleCenter = (g_monsters.turtles[i].x - g_game.cameraX) + g_monsters.turtles[i].width / 2;

                g_monsters.turtles[i].turtleState = SPINNING;
                g_monsters.turtles[i].vx = (marioCenter < turtleCenter) ? 8 : -8;
            }
            else if (g_monsters.turtles[i].turtleState == SPINNING) // spinning 중인 상태를 밟으면
            {
                PlaySoundBuffer(stomp_Sound);
                g_monsters.turtles[i].turtleState = SHELL;
                g_monsters.turtles[i].vx = 0;
            }

            g_player.mario.vy = -10;  // 반동 점프
        }

        else if (IsColliding(g_player.mario.x, g_player.mario.y, g_player.mario.width, g_player.mario.height,
            turtleLeft, g_monsters.turtles[i].y + 20, g_monsters.turtles[i].width, g_monsters.turtles[i].height) &&
            !g_monsters.turtles[i].isDead &&
            g_monsters.turtles[i].active)
        {
            if (g_monsters.turtles[i].turtleState == SPINNING) // 측면 충돌
            {
                if(g_monsters.turtles[i].damage) damage_mario();
            }
            else if (g_monsters.turtles[i].turtleState == NORMAL) 
            {
                // 스타 파워면 튕겨내기 처리
                if (g_player.mario.star)
                {
                    PlaySoundBuffer(kick_Sound);
                    g_monsters.turtles[i].vy = -15;
                    g_monsters.turtles[i].isDead = true;
                    g_monsters.turtles[i].isFalling = true;
                }
                else
                {
                    damage_mario();
                }
            }
            else if (g_monsters.turtles[i].turtleState == SHELL)
            {
                if (g_player.mario.star)
                {
                    PlaySoundBuffer(kick_Sound);
                    g_monsters.turtles[i].vy = -15;
                    g_monsters.turtles[i].isDead = true;
                    g_monsters.turtles[i].isFalling = true;
                }
                else
                {
                    PlaySoundBuffer(kick_Sound);
                    g_monsters.turtles[i].damage = false;
                    g_monsters.turtles[i].damageTimer = GetTickCount();
                    int marioCenter = g_player.mario.x + g_player.mario.width / 2;
                    int turtleCenter = (g_monsters.turtles[i].x - g_game.cameraX) + g_monsters.turtles[i].width / 2;

                    g_monsters.turtles[i].turtleState = SPINNING;
                    g_monsters.turtles[i].vx = (marioCenter < turtleCenter) ? 8 : -8;
                }
            }
        }
    }
}

// 날개 거북이

void UpdateAngelTurtles()
{
    for (int i = 0; i < g_monsters.angelTurtleCount; i++) 
    {
        if (!g_monsters.angelTurtles[i].isAlive) continue;

        if (g_monsters.angelTurtles[i].state == FLYING)
        {
            if (g_monsters.angelTurtles[i].goingUp)
                g_monsters.angelTurtles[i].y -= g_monsters.angelTurtles[i].vy;
            else
                g_monsters.angelTurtles[i].y += g_monsters.angelTurtles[i].vy;

            if (g_monsters.angelTurtles[i].y <= g_monsters.angelTurtles[i].topY)
                g_monsters.angelTurtles[i].goingUp = false;
            else if (g_monsters.angelTurtles[i].y >= g_monsters.angelTurtles[i].bottomY)
                g_monsters.angelTurtles[i].goingUp = true;
        }
        else if (g_monsters.angelTurtles[i].state == HIDE)
        {
            // 중력 적용
            g_monsters.angelTurtles[i].vy += 1;
            if (g_monsters.angelTurtles[i].vy > 10) g_monsters.angelTurtles[i].vy = 10;
            g_monsters.angelTurtles[i].y += g_monsters.angelTurtles[i].vy;

            // 화면 아래로 내려가면 비활성화
            if (g_monsters.angelTurtles[i].y > SCREEN_HEIGHT)
                g_monsters.angelTurtles[i].isAlive = false;
        }
    }
}
void HandleMarioAngelTurtleCollision()
{
    for (int i = 0; i < g_monsters.angelTurtleCount; i++)
    {
        if (!g_monsters.angelTurtles[i].isAlive) continue;

        int marioLeft = g_player.mario.x;
        int marioRight = g_player.mario.x + g_player.mario.width;
        int marioTop = g_player.mario.y;
        int marioBottom = g_player.mario.y + g_player.mario.height;

        int turtleLeft = g_monsters.angelTurtles[i].x - g_game.cameraX;
        int turtleRight = turtleLeft + g_monsters.angelTurtles[i].width;
        int turtleTop = g_monsters.angelTurtles[i].y - TILE_SIZE;
        int turtleBottom = turtleTop + g_monsters.angelTurtles[i].height;

        if (g_player.mario.vy > 0 &&
            marioBottom >= turtleTop &&
            marioBottom <= turtleTop + 10 &&
            marioRight > turtleLeft &&
            marioLeft < turtleRight) {

            PlaySoundBuffer(stomp_Sound);
            g_monsters.angelTurtles[i].state = HIDE;
            g_monsters.angelTurtles[i].vy = 2;
            g_monsters.angelTurtles[i].hideStartTime = GetTickCount();

            g_player.mario.vy = -15;
        }
        else if (IsColliding(g_player.mario.x, g_player.mario.y, g_player.mario.width, g_player.mario.height,
                turtleLeft, g_monsters.angelTurtles[i].y, g_monsters.angelTurtles[i].width, g_monsters.angelTurtles[i].height) &&
                g_monsters.angelTurtles[i].isAlive)
        {
            if (!g_player.mario.god && !g_player.mario.star) 
            {  // 무적 상태가 아닐 때 데미지
                damage_mario();
            }
        }
    }
}

//g_monsters.bowser
void UpdateBowser() 
{
    if (!g_monsters.bowser.isAlive) return;

    // === 이동 ===
    int moveSpeed = 2;
    g_monsters.bowser.x += moveSpeed * g_monsters.bowser.moveSign;
    g_monsters.bowser.moveDistance += moveSpeed;

    if (g_monsters.bowser.moveDistance >= g_monsters.bowser.maxDistance)
    {
        // 방향 전환
        g_monsters.bowser.moveSign *= -1;
        g_monsters.bowser.moveDistance = 0;
    }

    // === 불규칙 점프 ===
    g_monsters.bowser.jumpTimer++;
    if (g_monsters.bowser.jumpTimer > g_monsters.bowser.jumpInterval)
    {
        g_monsters.bowser.vy = -18;
        g_monsters.bowser.isJumping = true;
        g_monsters.bowser.jumpTimer = 0;
        g_monsters.bowser.jumpInterval = 60 + rand() % 121;
    }

    // === 중력 적용 ===
    g_monsters.bowser.vy += 1;
    if (g_monsters.bowser.vy > 10) g_monsters.bowser.vy = 10;
    g_monsters.bowser.y += g_monsters.bowser.vy;

    // === 바닥 충돌 ===
    int tileX = (g_monsters.bowser.x + g_monsters.bowser.width / 2) / TILE_SIZE;
    int tileY = (g_monsters.bowser.y + g_monsters.bowser.height) / TILE_SIZE;
    if (currentMap[tileY][tileX] != 0 && !g_monsters.bowser.isFalling) 
    {
        g_monsters.bowser.y = tileY * TILE_SIZE - g_monsters.bowser.height;
        g_monsters.bowser.vy = 0;
        g_monsters.bowser.isJumping = false;
    }

    g_monsters.bowser.fireTimer++;
    if (g_monsters.bowser.fireTimer > g_monsters.bowser.fireInterval) 
    {
        for (int i = 0; i < MAX_FIREBALLS; i++) 
        {
            if (!g_monsters.fireballs[i].active)
            {
                // 마리오 위치 기반으로 방향 결정
                int dir = (g_player.mario.x + g_player.mario.width / 2 < g_monsters.bowser.x + g_monsters.bowser.width / 2) ? -1 : 1;

                g_monsters.fireballs[i].x = g_monsters.bowser.x + g_monsters.bowser.width / 2;
                g_monsters.fireballs[i].y = g_monsters.bowser.y + g_monsters.bowser.height / 2;
                g_monsters.fireballs[i].width = 40;
                g_monsters.fireballs[i].height = 20;
                g_monsters.fireballs[i].vx = 6 * dir;
                g_monsters.fireballs[i].vy = 0;
                g_monsters.fireballs[i].active = true;

                g_monsters.bowser.isFiring = true;
                g_monsters.bowser.fireDuration = 30;
                break;
            }
        }

        g_monsters.bowser.fireTimer = 0;
        g_monsters.bowser.fireInterval = 120 + rand() % 120;
    }


    // 발사 중 불꽃 애니메이션 시간 감소
    if (g_monsters.bowser.isFiring)
    {
        g_monsters.bowser.fireDuration--;
        if (g_monsters.bowser.fireDuration <= 0)
            g_monsters.bowser.isFiring = false;
    }
    if (g_monsters.bowser.hp <= 0)
    {
        PlaySoundBuffer(bowserfalls_Sound);
        g_monsters.bowser.isFalling = true;
    }
    // 화면 밖으로 나가면 비활성화
    if (g_monsters.bowser.y > SCREEN_HEIGHT)
    {
        PlaySoundBuffer(bowserdead_Sound);
        g_monsters.bowser.isAlive = false;
        for (int i = 137; i < MAP_WIDTH; i++)
        {
            for (int j = 0; j < MAP_HEIGHT; j++)
            {
                if (currentMap[j][i] == TILE_KOOPA_BLOCK)
                {
                    currentMap[j][i] = 0;
                }
            }
        }
    }

}
void UpdateFireballs() 
{
    for (int i = 0; i < MAX_FIREBALLS; i++) 
    {
        if (!g_monsters.fireballs[i].active) continue;
        g_monsters.fireballs[i].motion++;
        if (g_monsters.fireballs[i].motion > 6)
        {
            g_monsters.fireballs[i].motion = 0;
        }
        g_monsters.fireballs[i].x += g_monsters.fireballs[i].vx;

        // 화면 범위를 벗어나면 파이어볼 비활성화
        if (g_monsters.fireballs[i].x < g_game.cameraX || g_monsters.fireballs[i].x > g_game.cameraX + SCREEN_WIDTH)
            g_monsters.fireballs[i].active = false;
    }
}

// 마리오 보서 충돌처리

void HandleMarioBowserCollision()
{
    if (!g_monsters.bowser.isAlive) return;

    int bowserX = g_monsters.bowser.x - g_game.cameraX;

    if (IsColliding(g_player.mario.x, g_player.mario.y, g_player.mario.width, g_player.mario.height,
        bowserX, g_monsters.bowser.y, g_monsters.bowser.width, g_monsters.bowser.height) &&
        !g_monsters.bowser.isDead) 
    {
        // 스타 파워면 튕겨내기 처리
        if (g_player.mario.star)
        {
            PlaySoundBuffer(kick_Sound);
            g_monsters.bowser.vy = -15;
            g_monsters.bowser.isDead = true;
            g_monsters.bowser.isFalling = true;
        }
        else
        {
            damage_mario();
        }
    }
}
void HandleMarioFireballCollision()
{
    if (g_player.mario.star || g_player.mario.isDead || g_player.mario.god) return;

    for (int i = 0; i < MAX_FIREBALLS; i++) 
    {
        if (!g_monsters.fireballs[i].active) continue;

        // 파이어볼 화면 좌표, 마리오 화면 좌표 기준으로 계산
        int fireballX = g_monsters.fireballs[i].x - g_game.cameraX;
        
        if (IsColliding(g_player.mario.x, g_player.mario.y, g_player.mario.width, g_player.mario.height,
            fireballX, g_monsters.fireballs[i].y, g_monsters.fireballs[i].width, g_monsters.fireballs[i].height))
        {
            damage_mario();  // 데미지 적용
            return;
        }
    }
}


void damage_mario()
{
    if (g_player.mario.god || g_player.mario.star) return;

    if (g_player.mario.form == FORM_BIG)
    {
        // 작은 마리오로 변신
        PlaySoundBuffer(powerdown_Sound);
        g_player.mario.y += TILE_SIZE + 1;       // 위치 낮추기
        g_game.transformTarget = FORM_BIG;
        g_game.gameState = GAME_TRANSFORMING;
        g_game.transformStartTime = GetTickCount();
        // 반동 점프 방지를 위해 vy 유지 또는 0으로
    }
    else if (g_player.mario.form == FORM_FLOWER)
    {
        PlaySoundBuffer(powerdown_Sound);
        g_game.transformTarget = FORM_FLOWER;
        g_game.gameState = GAME_TRANSFORMING;
        g_game.transformStartTime = GetTickCount();
        // 반동 점프 방지를 위해 vy 유지 또는 0으로
    }
    else if (g_player.mario.form == FORM_TINO)
    {
        PlaySoundBuffer(powerdown_Sound);
        g_game.transformTarget = FORM_TINO;
        g_game.gameState = GAME_TRANSFORMING;
        g_game.transformStartTime = GetTickCount();
        // 반동 점프 방지를 위해 vy 유지 또는 0으로
    }
    else
    {
        // 작은 마리오 사망
        dead();
    }
}

void UpdateAllMonsters()
{
    UpdateMonsters();
    UpdateTurtles();
    UpdateAngelTurtles();
    UpdateBowser();
    UpdateFireballs();
    UpdateFireTraps();
}

void HandleAllMonsterCollisions()
{
    HandleMarioMonsterCollision();
    HandleMarioTurtleCollision();
    HandleMarioAngelTurtleCollision();
    CheckMarioHazardCollision();
    HandleMarioBowserCollision();
    HandleMarioFireballCollision();
}