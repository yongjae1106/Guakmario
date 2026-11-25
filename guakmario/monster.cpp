#pragma once
#include "network.h"
#include "data.h"
#include "player.h"
#include "game.h"
#include "map.h"
#include "collision.h"
#include "image.h"
#include "item.h"
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
    if (IsMultiplayerGame() && !g_net.isHost) return;

    Bowser& bowser = g_monsters.bowser;
    bowser = {};
    bowser.hp = BOWSER_MAX_HP;
    bowser.x = 125 * TILE_SIZE;
    bowser.y = 9 * TILE_SIZE;
    bowser.width = 120;
    bowser.height = 120;
    bowser.isAlive = true;
    bowser.active = true;
    bowser.maxDistance = 10 * TILE_SIZE;
    bowser.jumpInterval = 60 + rand() % 121;
    bowser.fireInterval = 60 + rand() % 60;
}

//monster
// 충돌처리
static int MonsterCollisionTile(int row, int column)
{
    if (row < 0 || row >= MAP_HEIGHT || column < 0 || column >= MAP_WIDTH) return TILE_EMPTY;
    return currentMap[row][column];
}

void UpdateMonsters()
{
    const DWORD now = GetTickCount();
    int cameras[3] = { static_cast<int>(g_game.cameraX) };
    int cameraCount = 1;
    if (IsMultiplayerGame())
    {
        for (int id = 1; id < 3; ++id)
        {
            if (g_net.clientSockets[id - 1] != INVALID_SOCKET)
                cameras[cameraCount++] = CameraXForPlayer(g_allPlayers[id].mario.x);
        }
    }

    for (int i = 0; i < g_monsters.monsterCount; i++)
    {
        Monster& monster = g_monsters.monsters[i];
        if (!monster.active) continue;
        if (!monster.isAlive)
        {
            // 밟힌 몬스터의 표시 시간도 상태를 계산하는 쪽에서 관리한다.
            if (monster.deadStart == 0) monster.deadStart = now;
            if (now - monster.deadStart >= 300) monster.active = false;
            continue;
        }

        if (g_monsters.monsters[i].isFalling)
        {
            g_monsters.monsters[i].x += g_monsters.monsters[i].vx;
            g_monsters.monsters[i].vy += 1;
            if (g_monsters.monsters[i].vy > 10) g_monsters.monsters[i].vy = 10;
            g_monsters.monsters[i].y += g_monsters.monsters[i].vy;
            if (monster.y > SCREEN_HEIGHT + 100)
            {
                monster.isAlive = false;
                monster.active = false;
            }
            continue;
        }
        
        bool visible = false;
        for (int camera = 0; camera < cameraCount; ++camera)
        {
            const int screenX = monster.x - cameras[camera];
            if (screenX + TILE_SIZE > 0 && screenX < SCREEN_WIDTH)
            {
                visible = true;
                break;
            }
        }
        if (!visible) continue;

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
            (isSolidTile(MonsterCollisionTile(top, left)) || isSolidTile(MonsterCollisionTile(bottom, left)) ||
             isSolidTile(MonsterCollisionTile(middle, left))))
        {
            g_monsters.monsters[i].x = (left + 1) * TILE_SIZE; // 위치 조정
            g_monsters.monsters[i].vx = -g_monsters.monsters[i].vx;      // 방향 반전
        }
        else if (g_monsters.monsters[i].vx > 0 &&
            (isSolidTile(MonsterCollisionTile(top, right)) || isSolidTile(MonsterCollisionTile(bottom, right)) ||
             isSolidTile(MonsterCollisionTile(middle, right))))
        {
            g_monsters.monsters[i].x = right * TILE_SIZE - g_monsters.monsters[i].width; // 위치 조정
            g_monsters.monsters[i].vx = -g_monsters.monsters[i].vx;                       // 방향 반전
        }

        // 낭떠러지면 낙하처리
        int underX = (g_monsters.monsters[i].x + TILE_SIZE / 2) / TILE_SIZE;
        int underY = (g_monsters.monsters[i].y + TILE_SIZE) / TILE_SIZE;

        if (MonsterCollisionTile(underY, underX) == TILE_EMPTY)
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
void SendMonsterHit(MonsterKind kind, int slot, HitType hitType, uint32_t shotId)
{
    if (slot < 0 || slot >= MAX_MONSTERS) return;
    const MonsterHitData hit = { kind, static_cast<uint8_t>(slot), hitType, shotId };
    if (!IsMultiplayerGame() || g_net.isHost) {
        HandleMonsterHit(g_localPlayerIdx, hit);
        return;
    }
    const MonsterHitPacket packet = {
        { sizeof(MonsterHitPacket), PKT_MONSTER_HIT, static_cast<uint8_t>(g_localPlayerIdx) }, hit
    };
    SendToHost(&packet, sizeof(packet));
}

void HandleMonsterHit(int senderId, const MonsterHitData& hit)
{
    if (senderId < 0 || senderId >= 3 || hit.hitType > HIT_SHELL) return;
    // 등껍질끼리의 물리 충돌은 호스트의 UpdateTurtles에서 직접 판정한다.
    if (hit.hitType == HIT_SHELL) return;
    Entity* target = nullptr;
    switch (hit.kind) {
    case MON_NORMAL:
        if (hit.slot < g_monsters.monsterCount) target = &g_monsters.monsters[hit.slot];
        break;
    case MON_TURTLE:
        if (hit.slot < g_monsters.turtleCount) target = &g_monsters.turtles[hit.slot];
        break;
    case MON_ANGEL_TURTLE:
        if (hit.slot < g_monsters.angelTurtleCount) target = &g_monsters.angelTurtles[hit.slot];
        break;
    }
    if (!target || !target->isAlive || target->isDead) return;
    if (hit.hitType == HIT_FIREBALL || hit.hitType == HIT_TINOFIRE)
    {
        const ShotType type = hit.hitType == HIT_FIREBALL ? SHOT_FIREBALL : SHOT_TINOFIRE;
        Shot* shot = FindActivePlayerShot(senderId, type, hit.shotId);
        if (!shot) return;
        // 원격 투사체와 몬스터 스냅샷 사이의 이동을 한 타일 허용한다.
        if (senderId != g_localPlayerIdx &&
            !IsColliding(shot->x - TILE_SIZE, shot->y - TILE_SIZE,
                shot->width + 2 * TILE_SIZE, shot->height + 2 * TILE_SIZE,
                target->x, target->y, target->width, target->height)) return;
        if (hit.hitType == HIT_FIREBALL) DespawnPlayerFireball(senderId, *shot);
    }
    const Player& attacker = g_allPlayers[senderId].mario;
    const int attackerX = attacker.x + (senderId == g_localPlayerIdx ? static_cast<int>(g_game.cameraX) : 0);
    if (senderId != g_localPlayerIdx && (hit.hitType == HIT_STOMP ||
        hit.hitType == HIT_STAR || hit.hitType == HIT_TINOBITE)) {
        // 수신된 플레이어 위치와 스냅샷 사이의 이동 오차를 한 타일 허용한다.
        const int reach = TILE_SIZE + (hit.hitType == HIT_TINOBITE ? TINO_BITE_WIDTH : 0);
        if (attackerX + attacker.width + reach < target->x || target->x + target->width + reach < attackerX ||
            attacker.y + attacker.height + TILE_SIZE < target->y || target->y + target->height + TILE_SIZE < attacker.y)
            return;
    }
    if (hit.hitType == HIT_STOMP) {
        if (hit.kind == MON_NORMAL) target->isAlive = false;
        else if (hit.kind == MON_ANGEL_TURTLE) {
            AngelTurtle& angel = g_monsters.angelTurtles[hit.slot];
            if (angel.state == HIDE) return;
            angel.state = HIDE;
            angel.vy = 2;
            angel.hideStartTime = GetTickCount();
        } else if (hit.kind == MON_TURTLE) {
            Turtle& turtle = g_monsters.turtles[hit.slot];
            const DWORD now = GetTickCount();
            if (IsMultiplayerGame() && turtle.lastNetworkStomp && now - turtle.lastNetworkStomp < 100) return;
            turtle.lastNetworkStomp = now;
            if (turtle.turtleState == SHELL) {
                turtle.turtleState = SPINNING;
                turtle.vx = attackerX + attacker.width / 2 < turtle.x + turtle.width / 2 ? 8 : -8;
                turtle.damage = false;
                turtle.damageTimer = now;
            } else {
                turtle.turtleState = SHELL;
                turtle.vx = 0;
                turtle.shellTimer = now;
            }
        }
        return;
    }
    target->vy = hit.hitType == HIT_TINOBITE ? TINO_BITE_KNOCKBACK_VY : -15;
    target->isDead = true;
    target->isFalling = true;
}

void HandleMarioMonsterCollision()
{
    for (int i = 0; i < g_monsters.monsterCount && !g_player.mario.isTransforming; i++)
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
            SendMonsterHit(MON_NORMAL, i, HIT_STOMP);
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
                SendMonsterHit(MON_NORMAL, i, HIT_STAR);
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

            if (MonsterCollisionTile(underY, underX) == TILE_EMPTY)
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
            if (g_monsters.turtles[i].y > SCREEN_HEIGHT)
                g_monsters.turtles[i].isAlive = false;
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

        if (MonsterCollisionTile(underY, underX) == TILE_EMPTY)
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

        const bool leftWall = isSolidTile(MonsterCollisionTile(top, left)) ||
            isSolidTile(MonsterCollisionTile(bottom, left)) || isSolidTile(MonsterCollisionTile(middle, left));
        const bool rightWall = isSolidTile(MonsterCollisionTile(top, right)) ||
            isSolidTile(MonsterCollisionTile(bottom, right)) || isSolidTile(MonsterCollisionTile(middle, right));

        if (g_monsters.turtles[i].vx < 0 && leftWall)
        {
            g_monsters.turtles[i].x = (left + 1) * TILE_SIZE;
            g_monsters.turtles[i].vx = -g_monsters.turtles[i].vx;
        }
        else if (g_monsters.turtles[i].vx > 0 && rightWall)
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
            if (g_monsters.turtles[i].vx < 0 && leftWall)
            {
                g_monsters.turtles[i].x = (left + 1) * TILE_SIZE;
                g_monsters.turtles[i].vx = -g_monsters.turtles[i].vx;
            }
            else if (g_monsters.turtles[i].vx > 0 && rightWall)
            {
                g_monsters.turtles[i].x = right * TILE_SIZE - g_monsters.turtles[i].width;
                g_monsters.turtles[i].vx = -g_monsters.turtles[i].vx;
            }
        }
    }
}
void HandleMarioTurtleCollision()
{
    for (int i = 0; i < g_monsters.turtleCount && !g_player.mario.isTransforming; i++)
    {
        if (!g_monsters.turtles[i].isAlive || g_monsters.turtles[i].isDead) continue;

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
            PlaySoundBuffer(g_monsters.turtles[i].turtleState == SHELL ? kick_Sound : stomp_Sound);
            SendMonsterHit(MON_TURTLE, i, HIT_STOMP);
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
                    SendMonsterHit(MON_TURTLE, i, HIT_STAR);
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
                    SendMonsterHit(MON_TURTLE, i, HIT_STAR);
                }
                else
                {
                    PlaySoundBuffer(kick_Sound);
                    SendMonsterHit(MON_TURTLE, i, HIT_STOMP);
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
    for (int i = 0; i < g_monsters.angelTurtleCount && !g_player.mario.isTransforming; i++)
    {
        if (!g_monsters.angelTurtles[i].isAlive || g_monsters.angelTurtles[i].isDead) continue;

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
            SendMonsterHit(MON_ANGEL_TURTLE, i, HIT_STOMP);

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

void FinishBowserDefeat()
{
    Bowser& bowser = g_monsters.bowser;
    if (bowser.isAlive) PlaySoundBuffer(bowserdead_Sound);
    bowser.isAlive = false;
    bowser.isDead = true;
    bowser.isFiring = false;
}

static int BowserTargetX(int centerX)
{
    const Player& local = g_player.mario;
    const int localCenterX = local.x + g_game.cameraX + local.width / 2;
    if (!IsMultiplayerGame()) return localCenterX;

    int targetX = centerX + SCREEN_WIDTH + 1;
    for (int id = 0; id < 3; ++id) {
        const Player& player = g_allPlayers[id].mario;
        if (!NetIsPlayerConnected(id) || player.isDead) continue;
        const int playerX = id == g_localPlayerIdx ? localCenterX : player.x + player.width / 2;
        if (abs(playerX - centerX) < abs(targetX - centerX)) targetX = playerX;
    }
    return targetX;
}

void UpdateBowser()
{
    Bowser& bowser = g_monsters.bowser;
    if (!bowser.isAlive) return;

    if (!bowser.isFalling && bowser.hp <= 0)
    {
        PlaySoundBuffer(bowserfalls_Sound);
        bowser.hp = 0;
        bowser.isDead = true;
        bowser.isFalling = true;
        bowser.isFiring = false;
        bowser.isJumping = false;
        bowser.vx = 0;
    }

    if (bowser.isFalling)
    {
        if (bowser.vy < 10) ++bowser.vy;
        bowser.y += bowser.vy;
        if (bowser.y > SCREEN_HEIGHT)
        {
            FinishBowserDefeat();
            ReportBowserDefeat();
        }
        return;
    }

    if (++bowser.animationTicks == 13) {
        bowser.animationTicks = 0;
        bowser.frame = 1 - bowser.frame;
    }

    // === 이동 ===
    const int moveSpeed = 2;
    bowser.vx = moveSpeed * bowser.moveSign;
    bowser.x += bowser.vx;
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
    if (tileY >= 0 && tileY < MAP_HEIGHT && tileX >= 0 && tileX < MAP_WIDTH &&
        currentMap[tileY][tileX] != TILE_EMPTY)
    {
        g_monsters.bowser.y = tileY * TILE_SIZE - g_monsters.bowser.height;
        g_monsters.bowser.vy = 0;
        g_monsters.bowser.isJumping = false;
    }

    const int centerX = bowser.x + bowser.width / 2;
    const int targetX = BowserTargetX(centerX);
    if ((!IsMultiplayerGame() || abs(targetX - centerX) <= SCREEN_WIDTH) &&
        ++bowser.fireTimer > bowser.fireInterval)
    {
        for (Fireball& fireball : g_monsters.fireballs)
        {
            if (fireball.active) continue;
            bowser.fireSign = targetX < centerX ? -1 : 1;
            fireball = { centerX, bowser.y + bowser.height / 2,
                6 * bowser.fireSign, 0, 40, 20, 0, true };
            bowser.isFiring = true;
            bowser.fireDuration = 30;
            break;
        }
        bowser.fireTimer = 0;
        bowser.fireInterval = 120 + rand() % 120;
    }


    // 발사 중 불꽃 애니메이션 시간 감소
    if (g_monsters.bowser.isFiring)
    {
        g_monsters.bowser.fireDuration--;
        if (g_monsters.bowser.fireDuration <= 0)
            g_monsters.bowser.isFiring = false;
    }
}
void UpdateFireballs()
{
    const bool multiplayer = IsMultiplayerGame();
    const int left = multiplayer ? 0 : g_game.cameraX;
    const int right = multiplayer ? MAP_WIDTH * TILE_SIZE : g_game.cameraX + SCREEN_WIDTH;
    for (Fireball& fireball : g_monsters.fireballs)
    {
        if (!fireball.active) continue;
        fireball.motion = (fireball.motion + 1) % 7;
        fireball.x += fireball.vx;
        fireball.y += fireball.vy;
        if (fireball.x + fireball.width <= left || fireball.x >= right)
            fireball.active = false;
    }
}

bool ApplyBowserHit(HitType type)
{
    Bowser& bowser = g_monsters.bowser;
    if (g_game.stage != CASTLE_STAGE || !bowser.isAlive || bowser.isDead || bowser.hp <= 0 ||
        (IsMultiplayerGame() && (!g_net.isHost || !g_game.gameStart || g_game.gameState != GAME_RUNNING)))
        return false;

    int damage = 0;
    switch (type)
    {
    case HIT_FIREBALL: damage = 1; break;
    case HIT_TINOFIRE: damage = 5; break;
    case HIT_TINOBITE: damage = 10; break;
    case HIT_STAR:
        damage = bowser.hp;
        bowser.vy = -15;
        break;
    default: return false;
    }
    bowser.hp = damage >= bowser.hp ? 0 : bowser.hp - damage;
    PlaySoundBuffer(type == HIT_FIREBALL ? bump_Sound : kick_Sound);
    return true;
}

void HandleBowserPlayerAttacks()
{
    if (IsMultiplayerGame() && !g_net.isHost) return;
    Bowser& bowser = g_monsters.bowser;
    if (!bowser.isAlive || bowser.isDead || bowser.hp <= 0) return;

    for (int id = 0; id < 3; ++id)
    {
        const bool local = id == g_localPlayerIdx;
        if (!local && !IsMultiplayerGame()) continue;
        const Player& attacker = g_allPlayers[id].mario;
        if (attacker.isDead || attacker.gameover || attacker.isTransforming) continue;

        const int worldX = attacker.x + (local ? static_cast<int>(g_game.cameraX) : 0);
        if (attacker.star && IsColliding(worldX, attacker.y, attacker.width, attacker.height,
            bowser.x, bowser.y, bowser.width, bowser.height) && ApplyBowserHit(HIT_STAR)) return;

        if (attacker.form != FORM_TINO || !attacker.tino_motion || attacker.tinoBiteId == 0 ||
            attacker.tinoBiteId == bowser.lastBiteHits[id]) continue;
        const int biteX = attacker.direction == DIR_LEFT ? worldX - TINO_BITE_WIDTH : worldX + attacker.width;
        if (IsColliding(biteX, attacker.y + TINO_BITE_Y_OFFSET, TINO_BITE_WIDTH, TINO_BITE_HEIGHT,
            bowser.x, bowser.y, bowser.width, bowser.height) && ApplyBowserHit(HIT_TINOBITE))
            bowser.lastBiteHits[id] = attacker.tinoBiteId;
    }
}

void HandleMarioBowserCollision()
{
    if (!g_monsters.bowser.isAlive) return;

    int bowserX = g_monsters.bowser.x - g_game.cameraX;

    if (IsColliding(g_player.mario.x, g_player.mario.y, g_player.mario.width, g_player.mario.height,
        bowserX, g_monsters.bowser.y, g_monsters.bowser.width, g_monsters.bowser.height) &&
        !g_monsters.bowser.isDead) 
    {
        damage_mario();
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
    if (g_player.mario.isDead || g_player.mario.god || g_player.mario.star || g_player.mario.isTransforming) return;

    if (g_player.mario.form != FORM_SMALL)
    {
        PlaySoundBuffer(powerdown_Sound);
        const MarioForm target = g_player.mario.form == FORM_BIG ? FORM_SMALL : FORM_BIG;
        StartPlayerTransformation(target);
    }
    else
    {
        // 작은 마리오 사망
        dead();
    }
}

namespace {
uint16_t ElapsedTimer(DWORD now, DWORD start)
{
    const DWORD elapsed = now - start;
    return static_cast<uint16_t>(elapsed > UINT16_MAX ? UINT16_MAX : elapsed);
}

MonsterEntry PackMonster(const Entity& entity, int slot)
{
    MonsterEntry entry = {};
    entry.slot = static_cast<uint8_t>(slot);
    entry.x = static_cast<int16_t>(entity.x);
    entry.y = static_cast<int16_t>(entity.y);
    entry.vx = static_cast<int16_t>(entity.vx);
    entry.vy = static_cast<int16_t>(entity.vy);
    entry.width = static_cast<uint8_t>(entity.width);
    entry.height = static_cast<uint8_t>(entity.height);
    if (entity.isAlive) entry.flags |= MON_FLAG_ALIVE;
    if (entity.isDead) entry.flags |= MON_FLAG_DEAD;
    if (entity.active) entry.flags |= MON_FLAG_ACTIVE;
    if (entity.isFalling) entry.flags |= MON_FLAG_FALLING;
    return entry;
}

void BroadcastKind(uint8_t kind, const std::vector<MonsterEntry>& entries)
{
    std::vector<uint8_t> packet;
    if (BuildMonsterStatePacket(kind, entries, packet))
        SendToAll(packet.data(), static_cast<int>(packet.size()));
}

void BroadcastMonsterState()
{
    const DWORD now = GetTickCount();
    std::vector<MonsterEntry> entries;
    for (int i = 0; i < g_monsters.monsterCount; ++i) {
        const Monster& monster = g_monsters.monsters[i];
        if (!monster.active) continue;
        auto entry = PackMonster(monster, i);
        if (monster.deadStart) entry.timerA = ElapsedTimer(now, monster.deadStart);
        entries.push_back(entry);
    }
    BroadcastKind(MON_NORMAL, entries);
    entries.clear();
    for (int i = 0; i < g_monsters.turtleCount; ++i) {
        const Turtle& turtle = g_monsters.turtles[i];
        if (!turtle.isAlive) continue;
        auto entry = PackMonster(turtle, i);
        entry.state = turtle.turtleState;
        if (turtle.damage) entry.flags |= MON_FLAG_DAMAGE;
        if (turtle.direction == DIR_LEFT) entry.flags |= MON_FLAG_FACING_LEFT;
        if (turtle.turtleState == SHELL) entry.timerA = ElapsedTimer(now, turtle.shellTimer);
        if (turtle.turtleState == SPINNING) entry.timerB = ElapsedTimer(now, turtle.damageTimer);
        entries.push_back(entry);
    }
    BroadcastKind(MON_TURTLE, entries);
    entries.clear();
    BroadcastKind(MON_BROWN_TURTLE, entries);
    for (int i = 0; i < g_monsters.angelTurtleCount; ++i) {
        const AngelTurtle& angel = g_monsters.angelTurtles[i];
        if (!angel.isAlive) continue;
        auto entry = PackMonster(angel, i);
        entry.state = static_cast<uint8_t>(angel.state);
        if (angel.goingUp) entry.flags |= MON_FLAG_GOING_UP;
        if (angel.state == HIDE) entry.timerA = ElapsedTimer(now, angel.hideStartTime);
        entries.push_back(entry);
    }
    BroadcastKind(MON_ANGEL_TURTLE, entries);
}

template<class T, size_t N, class ApplyDetails>
void ReplaceMonsterSlots(T (&slots)[N], int& count, const std::vector<MonsterEntry>& entries,
    ApplyDetails applyDetails)
{
    static_assert(N >= 100, "Snapshot slots must fit the local array");
    for (int i = 0; i < count; ++i) {
        slots[i].active = false;
        slots[i].isAlive = false;
        slots[i].isDead = false;
    }
    count = 0;
    for (const auto& entry : entries) {
        auto& entity = slots[entry.slot];
        entity.x = entry.x;
        entity.y = entry.y;
        entity.vx = entry.vx;
        entity.vy = entry.vy;
        entity.width = entry.width;
        entity.height = entry.height;
        entity.active = (entry.flags & MON_FLAG_ACTIVE) != 0;
        entity.isAlive = (entry.flags & MON_FLAG_ALIVE) != 0;
        entity.isDead = (entry.flags & MON_FLAG_DEAD) != 0;
        entity.isFalling = (entry.flags & MON_FLAG_FALLING) != 0;
        applyDetails(entity, entry);
        if (entry.slot >= count) count = entry.slot + 1;
    }
}
}

void ApplyMonsterSnapshot(const std::vector<uint8_t>& packet)
{
    uint8_t kind;
    std::vector<MonsterEntry> entries;
    if (!ParseMonsterStatePacket(packet, kind, entries)) return;
    const DWORD now = GetTickCount();
    switch (kind) {
    case MON_NORMAL:
        ReplaceMonsterSlots(g_monsters.monsters, g_monsters.monsterCount, entries,
            [now](Monster& m, const MonsterEntry& e) {
                m.deadStart = !m.isAlive || m.isDead ? now - e.timerA : 0;
            });
        break;
    case MON_TURTLE:
        ReplaceMonsterSlots(g_monsters.turtles, g_monsters.turtleCount, entries,
            [now](Turtle& t, const MonsterEntry& e) {
                t.turtleState = static_cast<TurtleState>(e.state);
                t.direction = (e.flags & MON_FLAG_FACING_LEFT) ? DIR_LEFT : DIR_RIGHT;
                t.damage = (e.flags & MON_FLAG_DAMAGE) != 0;
                t.shellTimer = now - e.timerA;
                t.damageTimer = now - e.timerB;
            });
        break;
    case MON_BROWN_TURTLE:
        g_monsters.brownTurtleCount = 0;
        break;
    case MON_ANGEL_TURTLE:
        ReplaceMonsterSlots(g_monsters.angelTurtles, g_monsters.angelTurtleCount, entries,
            [now](AngelTurtle& a, const MonsterEntry& e) {
                a.state = e.state;
                a.goingUp = (e.flags & MON_FLAG_GOING_UP) != 0;
                a.hideStartTime = now - e.timerA;
            });
        break;
    }
}

static void BroadcastBossState()
{
    const Bowser& bowser = g_monsters.bowser;
    BossStateData state = {};
    state.x = static_cast<int16_t>(bowser.x);
    state.y = static_cast<int16_t>(bowser.y);
    state.vx = static_cast<int16_t>(bowser.vx);
    state.vy = static_cast<int16_t>(bowser.vy);
    state.hp = static_cast<int16_t>(bowser.hp);
    state.width = static_cast<uint8_t>(bowser.width);
    state.height = static_cast<uint8_t>(bowser.height);
    state.frame = static_cast<uint8_t>(bowser.frame);
    state.moveSign = static_cast<int8_t>(bowser.moveSign);
    state.fireSign = static_cast<int8_t>(bowser.fireSign);
    if (bowser.isAlive) state.flags |= BOSS_FLAG_ALIVE;
    if (bowser.isDead) state.flags |= BOSS_FLAG_DEAD;
    if (bowser.active) state.flags |= BOSS_FLAG_ACTIVE;
    if (bowser.isFalling) state.flags |= BOSS_FLAG_FALLING;
    if (bowser.isJumping) state.flags |= BOSS_FLAG_JUMPING;
    if (bowser.isFiring) state.flags |= BOSS_FLAG_FIRING;

    std::vector<BossFireballData> fireballs;
    for (int slot = 0; slot < MAX_FIREBALLS; ++slot) {
        const Fireball& fireball = g_monsters.fireballs[slot];
        if (!fireball.active) continue;
        fireballs.push_back({ static_cast<uint8_t>(slot), static_cast<int16_t>(fireball.x),
            static_cast<int16_t>(fireball.y), static_cast<int16_t>(fireball.vx),
            static_cast<int16_t>(fireball.vy), static_cast<uint8_t>(fireball.width),
            static_cast<uint8_t>(fireball.height), static_cast<uint8_t>(fireball.motion) });
    }
    std::vector<uint8_t> packet;
    if (BuildBossStatePacket(state, fireballs, packet))
        SendToAll(packet.data(), static_cast<int>(packet.size()));
}

void ApplyBossSnapshot(const std::vector<uint8_t>& packet)
{
    BossStateData state = {};
    std::vector<BossFireballData> fireballs;
    if (!ParseBossStatePacket(packet, state, fireballs)) return;

    Bowser& bowser = g_monsters.bowser;
    if (bowser.isAlive && !(state.flags & BOSS_FLAG_ALIVE))
        PlaySoundBuffer(bowserdead_Sound);
    else if (!bowser.isFalling && (state.flags & BOSS_FLAG_FALLING) && (state.flags & BOSS_FLAG_ALIVE))
        PlaySoundBuffer(bowserfalls_Sound);

    bowser.x = state.x;
    bowser.y = state.y;
    bowser.vx = state.vx;
    bowser.vy = state.vy;
    bowser.hp = state.hp;
    bowser.width = state.width;
    bowser.height = state.height;
    bowser.frame = state.frame;
    bowser.moveSign = state.moveSign;
    bowser.fireSign = state.fireSign;
    bowser.isAlive = (state.flags & BOSS_FLAG_ALIVE) != 0;
    bowser.isDead = (state.flags & BOSS_FLAG_DEAD) != 0;
    bowser.active = (state.flags & BOSS_FLAG_ACTIVE) != 0;
    bowser.isFalling = (state.flags & BOSS_FLAG_FALLING) != 0;
    bowser.isJumping = (state.flags & BOSS_FLAG_JUMPING) != 0;
    bowser.isFiring = (state.flags & BOSS_FLAG_FIRING) != 0;

    for (Fireball& fireball : g_monsters.fireballs) fireball.active = false;
    for (const auto& fireball : fireballs) {
        g_monsters.fireballs[fireball.slot] = { fireball.x, fireball.y, fireball.vx, fireball.vy,
            fireball.width, fireball.height, fireball.motion, true };
    }
}

void UpdateAllMonsters()
{
    const bool multiplayer = IsMultiplayerGame();
    if (!multiplayer || g_net.isHost) {
        UpdateMonsters();
        UpdateTurtles();
        UpdateAngelTurtles();
        UpdateBowser();
        UpdateFireballs();
        if (multiplayer && ++g_monsters.snapshotFrame == 5) {
            g_monsters.snapshotFrame = 0;
            BroadcastMonsterState();
            if (g_game.stage == CASTLE_STAGE) BroadcastBossState();
        }
    }
    UpdateFireTraps();
}

void HandleAllMonsterCollisions()
{
    HandleMarioMonsterCollision();
    HandleMarioTurtleCollision();
    HandleMarioAngelTurtleCollision();
    if (g_player.mario.isTransforming) return;
    CheckMarioHazardCollision();
    HandleMarioBowserCollision();
    HandleMarioFireballCollision();
}
