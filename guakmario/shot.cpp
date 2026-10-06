#include "item.h"
#include "data.h"
#include "player.h"
#include "game.h"
#include "map.h"
#include "collision.h"
#include "image.h"
#include "sound.h"
#include "monster.h"

void CheckCollision_fireball()
{
    for (int i = 0; i < MAX_SHOT; i++)
    {
        if (!fireball[i].active) continue;

        int fireballXWorld = fireball[i].x - g_game.cameraX;

        for (int j = 0; j < MAX_MONSTERS; j++)
        {
            int monsterXWorld = g_monsters.monsters[j].x - g_game.cameraX;
            if (!g_monsters.monsters[j].isDead && g_monsters.monsters[j].isAlive && fireball[i].active) // 여러 조건 같이 확인
            {
                if (IsColliding(monsterXWorld, g_monsters.monsters[j].y, g_monsters.monsters[j].width, g_monsters.monsters[j].height,
                    fireballXWorld, fireball[i].y, fireball[i].width, fireball[i].height))
                {
                    PlaySoundBuffer(kick_Sound);
                    g_monsters.monsters[j].vy = -15;
                    g_monsters.monsters[j].isDead = true;
                    g_monsters.monsters[j].isFalling = true;
                    fireball[i].active = false;
                    break; // 이미 충돌했으면 더 검사 X
                }
            }
        }

        for (int j = 0; j < MAX_TURTLES; j++)
        {
            int turtleXWorld = g_monsters.turtles[j].x - g_game.cameraX;
            if (!g_monsters.turtles[j].isDead && g_monsters.turtles[j].isAlive && fireball[i].active)
            {
                if (IsColliding(turtleXWorld, g_monsters.turtles[j].y, g_monsters.turtles[j].width, g_monsters.turtles[j].height,
                    fireballXWorld, fireball[i].y, fireball[i].width, fireball[i].height))
                {
                    PlaySoundBuffer(kick_Sound);
                    g_monsters.turtles[j].vy = -15;
                    g_monsters.turtles[j].isDead = true;
                    g_monsters.turtles[j].isFalling = true;
                    fireball[i].active = false;
                    break;
                }
            }
        }
        for (int j = 0; j < MAX_TURTLES; j++)
        {
            int turtleXWorld = g_monsters.angelTurtles[j].x - g_game.cameraX;
            if (!g_monsters.angelTurtles[j].isDead && g_monsters.angelTurtles[j].isAlive && fireball[i].active)
            {
                if (IsColliding(turtleXWorld, g_monsters.angelTurtles[j].y, g_monsters.angelTurtles[j].width, g_monsters.angelTurtles[j].height,
                    fireballXWorld, fireball[i].y, fireball[i].width, fireball[i].height))
                {
                    PlaySoundBuffer(kick_Sound);
                    g_monsters.angelTurtles[j].vy = -15;
                    g_monsters.angelTurtles[j].isDead = true;
                    g_monsters.angelTurtles[j].isFalling = true;
                    fireball[i].active = false;
                    break;
                }
            }
        }

        int bowserXWorld = g_monsters.bowser.x - g_game.cameraX;
        if (!g_monsters.bowser.isDead && g_monsters.bowser.isAlive && fireball[i].active)
        {
            if (IsColliding(bowserXWorld, g_monsters.bowser.y, g_monsters.bowser.width, g_monsters.bowser.height,
                fireballXWorld, fireball[i].y, fireball[i].width, fireball[i].height))
            {
                PlaySoundBuffer(bump_Sound);
                g_monsters.bowser.hp--;
                fireball[i].active = false;
            }
        }
    }
}
void CheckCollision_tinofire()
{
    Graphics graphics(g_memDC);
    graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);
    for (int i = 0; i < MAX_SHOT; i++)
    {
        if (!tinofire[i].active) continue;

        int tinofireXWorld = tinofire[i].x - g_game.cameraX;

        for (int j = 0; j < MAX_MONSTERS; j++)
        {
            int monsterXWorld = g_monsters.monsters[j].x - g_game.cameraX;
            if (!g_monsters.monsters[j].isDead && g_monsters.monsters[j].isAlive && tinofire[i].active) // 여러 조건 같이 확인
            {
                if (IsColliding(monsterXWorld, g_monsters.monsters[j].y, g_monsters.monsters[j].width, g_monsters.monsters[j].height,
                    tinofireXWorld, tinofire[i].y, tinofire[i].width, tinofire[i].height))
                {
                    PlaySoundBuffer(kick_Sound);
                    g_monsters.monsters[j].vy = -15;
                    g_monsters.monsters[j].isDead = true;
                    g_monsters.monsters[j].isFalling = true;
                    OnMonsterHit(g_monsters.monsters[j].x, g_monsters.monsters[j].y);
                    break; // 이미 충돌했으면 더 검사 X
                }
            }
        }

        for (int j = 0; j < MAX_TURTLES; j++)
        {
            int turtleXWorld = g_monsters.turtles[j].x - g_game.cameraX;
            if (!g_monsters.turtles[j].isDead && g_monsters.turtles[j].isAlive && tinofire[i].active)
            {
                if (IsColliding(turtleXWorld, g_monsters.turtles[j].y, g_monsters.turtles[j].width, g_monsters.turtles[j].height,
                    tinofireXWorld, tinofire[i].y, tinofire[i].width, tinofire[i].height))
                {
                    PlaySoundBuffer(kick_Sound);
                    g_monsters.turtles[j].vy = -15;
                    g_monsters.turtles[j].isDead = true;
                    g_monsters.turtles[j].isFalling = true;
                    OnMonsterHit(g_monsters.turtles[j].x, g_monsters.turtles[j].y);
                    break;
                }
            }
        }
        for (int j = 0; j < MAX_TURTLES; j++)
        {
            int angelturtleXWorld = g_monsters.angelTurtles[j].x - g_game.cameraX;
            if (!g_monsters.angelTurtles[j].isDead && g_monsters.angelTurtles[j].isAlive && tinofire[i].active)
            {
                if (IsColliding(angelturtleXWorld, g_monsters.angelTurtles[j].y + 20, g_monsters.angelTurtles[j].width, g_monsters.angelTurtles[j].height,
                    tinofireXWorld, tinofire[i].y, tinofire[i].width, tinofire[i].height))
                {
                    PlaySoundBuffer(kick_Sound);
                    g_monsters.angelTurtles[j].vy = -15;
                    g_monsters.angelTurtles[j].isDead = true;
                    g_monsters.angelTurtles[j].isFalling = true;
                    OnMonsterHit(g_monsters.angelTurtles[j].x, g_monsters.angelTurtles[j].y);
                    break;
                }
            }
        }

        int bowserXWorld = g_monsters.bowser.x - g_game.cameraX;
        if (!g_monsters.bowser.isDead && g_monsters.bowser.isAlive && tinofire[i].active)
        {
            if (IsColliding(bowserXWorld, g_monsters.bowser.y, g_monsters.bowser.width, g_monsters.bowser.height,
                tinofireXWorld, tinofire[i].y, tinofire[i].width, tinofire[i].height) && !g_monsters.bowser.ignoreTinoFire)
            {
                PlaySoundBuffer(kick_Sound);
                OnMonsterHit(g_monsters.bowser.x, g_monsters.bowser.y);
                g_monsters.bowser.hp -= 5;
                g_monsters.bowser.ignoreTinoFire = true;
            }
        }
    }
}
void OnMonsterHit(int x, int y)
{
    static int tinofire_effect_count = 0;

    if (tinofire_effect_count >= MAX_SHOT)
    {
        tinofire_effect_count = 0;
    }
    tinofire_effect[tinofire_effect_count].x = x;
    tinofire_effect[tinofire_effect_count].y = y;
    tinofire_effect[tinofire_effect_count].timer = 0;
    tinofire_effect[tinofire_effect_count].active = true;
    tinofire_effect_count++;
}
void TinoAttack()
{
    for (int j = 0; j < MAX_MONSTERS; j++)
    {
        int monsterXWorld = g_monsters.monsters[j].x - g_game.cameraX;
        if (!g_monsters.monsters[j].isDead && g_monsters.monsters[j].isAlive) // 여러 조건 같이 확인
        {
            if (IsColliding(monsterXWorld, g_monsters.monsters[j].y, g_monsters.monsters[j].width, g_monsters.monsters[j].height,
                (g_player.mario.direction == 0) ? g_player.mario.x - 50 :g_player.mario.x, g_player.mario.y - 15, 50, 100))
            {
                PlaySoundBuffer(kick_Sound);
                g_monsters.monsters[j].vy = -15;
                g_monsters.monsters[j].isDead = true;
                g_monsters.monsters[j].isFalling = true;
            }
        }
    }

    for (int j = 0; j < MAX_TURTLES; j++)
    {
        int turtleXWorld = g_monsters.turtles[j].x - g_game.cameraX;
        if (!g_monsters.turtles[j].isDead && g_monsters.turtles[j].isAlive)
        {
            if (IsColliding(turtleXWorld, g_monsters.turtles[j].y, g_monsters.turtles[j].width, g_monsters.turtles[j].height,
                (g_player.mario.direction == 0) ? g_player.mario.x - 50 : g_player.mario.x, g_player.mario.y - 15, 50, 100))
            {
                PlaySoundBuffer(kick_Sound);
                g_monsters.turtles[j].vy = -15;
                g_monsters.turtles[j].isDead = true;
                g_monsters.turtles[j].isFalling = true;
            }
        }
    }
    for (int j = 0; j < MAX_TURTLES; j++)
    {
        int turtleXWorld = g_monsters.angelTurtles[j].x - g_game.cameraX;
        if (!g_monsters.angelTurtles[j].isDead && g_monsters.angelTurtles[j].isAlive)
        {
            if (IsColliding(turtleXWorld, g_monsters.angelTurtles[j].y, g_monsters.angelTurtles[j].width, g_monsters.angelTurtles[j].height,
                (g_player.mario.direction == 0) ? g_player.mario.x - 50 : g_player.mario.x, g_player.mario.y - 15, 50, 100))
            {
                PlaySoundBuffer(kick_Sound);
                g_monsters.angelTurtles[j].vy = -15;
                g_monsters.angelTurtles[j].isDead = true;
                g_monsters.angelTurtles[j].isFalling = true;
                break;
            }
        }
    }

    int bowserXWorld = g_monsters.bowser.x - g_game.cameraX;
    if (!g_monsters.bowser.isDead && g_monsters.bowser.isAlive)
    {
        if (IsColliding(bowserXWorld, g_monsters.bowser.y, g_monsters.bowser.width, g_monsters.bowser.height,
            (g_player.mario.direction == 0) ? g_player.mario.x - 50 : g_player.mario.x, g_player.mario.y - 15, 50, 100) && !g_monsters.bowser.ignoreTinoBite)
        {
            PlaySoundBuffer(kick_Sound);
            g_monsters.bowser.hp -= 10;
            g_monsters.bowser.ignoreTinoBite = true;
        }
    }
}
void UpdateShot_fireball()
{
    for (int i = 0; i < MAX_SHOT; i++)
    {
        if (!fireball[i].active) continue;
        fireball[i].motion++;
        if (fireball[i].motion > 3)
        {
            fireball[i].motion = 0;
        }

        fireball[i].x += fireball[i].vx;    // 수평이동

        // 충돌 계산
        int left = fireball[i].x / TILE_SIZE;
        int right = (fireball[i].x + fireball[i].width - 1) / TILE_SIZE;
        int top = fireball[i].y / TILE_SIZE;
        int bottom = (fireball[i].y + fireball[i].height - 1) / TILE_SIZE;
        int middle = (fireball[i].y + 30 / 2 - 1) / TILE_SIZE;

        // 2️⃣ 수평 충돌 처리
        if (fireball[i].vx < 0 &&
            (isSolidTile(currentMap[top][left]) || isSolidTile(currentMap[bottom][left]) || isSolidTile(currentMap[middle][left])))
        {
            fireball[i].active = false;
            fireball[i].fade = true;
        }
        else if (fireball[i].vx > 0 &&
            (isSolidTile(currentMap[top][right]) || isSolidTile(currentMap[bottom][right]) || isSolidTile(currentMap[middle][right])))
        {
            fireball[i].active = false;
            fireball[i].fade = true;
        }

        // 3️⃣ 수직 낙하 (중력)
        fireball[i].vy += 1;
        if (fireball[i].vy > 10) fireball[i].vy = 5;
        fireball[i].y += fireball[i].vy;

        left = fireball[i].x / TILE_SIZE;
        right = (fireball[i].x + TILE_SIZE - 1) / TILE_SIZE;
        top = fireball[i].y / TILE_SIZE;
        bottom = (fireball[i].y + TILE_SIZE - 1) / TILE_SIZE;

        // 아래 충돌 처리
        if (fireball[i].vy > 0 && (isSolidTile(currentMap[bottom][left]) || isSolidTile(currentMap[bottom][right])))
        {
            fireball[i].y = bottom * TILE_SIZE - fireball[i].height - 10;
            fireball[i].vy = -9;
        }
    }
}
void UpdateShot_tinofire()
{
    for (int i = 0; i < MAX_SHOT; i++)
    {
        if (!tinofire[i].active) continue;
        tinofire[i].motion++;
        tinofire[i].duration++;
        if (tinofire[i].motion > 6)
        {
            tinofire[i].motion = 0;
        }
        if (tinofire[i].duration == 100)
        {
            tinofire[i].active = false;
            continue;
        }

        tinofire[i].x += tinofire[i].vx;    // 수평이동
    }
}
void UpdateShot_tinofire_effect()
{
    for (int i = 0; i < MAX_SHOT; i++)
    {
        if (tinofire_effect[i].active)
        {
            tinofire_effect[i].timer++;
            if (tinofire_effect[i].timer >= 15)
            {
                tinofire_effect[i].timer = 0;
                tinofire_effect[i].active = false;
            }
        }
    }
}
void SpawnFireball(int x, int y)
{
    static int fireball_count = 0;

    if (fireball_count >= MAX_SHOT)
    {
        fireball_count = 0;
    }
    fireball[fireball_count].x = x;
    fireball[fireball_count].y = y;
    fireball[fireball_count].vx = (g_player.mario.direction == 0) ? -7 : 7;
    fireball[fireball_count].vy = 0;
    fireball[fireball_count].width = 10;
    fireball[fireball_count].height = 10;
    fireball[fireball_count].active = true;
    fireball_count++;
}
void SpawnTinoFire(int x, int y)
{
    static int tinofire_count = 0;

    if (tinofire_count >= MAX_SHOT)
    {
        tinofire_count = 0;
    }
    tinofire[tinofire_count].x = x;
    tinofire[tinofire_count].y = y;
    tinofire[tinofire_count].vx = (g_player.mario.direction == 0) ? -7 : 7;
    tinofire[tinofire_count].vy = 0;
    tinofire[tinofire_count].motion = 0;
    tinofire[tinofire_count].motion_fade = 0;
    tinofire[tinofire_count].duration = 0;
    tinofire[tinofire_count].direction = g_player.mario.direction;
    tinofire[tinofire_count].width = TILE_SIZE * 2;
    tinofire[tinofire_count].height = TILE_SIZE * 2;
    tinofire[tinofire_count].active = true;
    tinofire[tinofire_count].fade = false;
    tinofire_count++;
}
void UpdateAllShots()
{
    UpdateShot_fireball();
    UpdateShot_tinofire();
    UpdateShot_tinofire_effect();
}
void CheckShotCollisions()
{
    CheckCollision_fireball();
    CheckCollision_tinofire();
}
