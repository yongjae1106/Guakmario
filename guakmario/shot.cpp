#include "network.h"
#include "item.h"
#include "data.h"
#include "renderer.h"
#include "player.h"
#include "game.h"
#include "map.h"
#include "collision.h"
#include "image.h"
#include "sound.h"
#include "monster.h"
#include "config.h"

void HandleFireballHit()
{
    for (int i = 0; i < MAX_SHOT; i++)
    {
        if (!fireball[i].active) continue;

        int fireballScreenX = fireball[i].x - g_game.cameraX;

        for (int j = 0; j < MAX_MONSTERS; j++)
        {
            int monsterScreenX = g_monsters.monsters[j].x - g_game.cameraX;
            if (!g_monsters.monsters[j].isDead && g_monsters.monsters[j].isAlive && fireball[i].active) // 여러 조건 같이 확인
            {
                if (IsColliding(monsterScreenX, g_monsters.monsters[j].y, g_monsters.monsters[j].width, g_monsters.monsters[j].height,
                    fireballScreenX, fireball[i].y, fireball[i].width, fireball[i].height))
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
            int turtleScreenX = g_monsters.turtles[j].x - g_game.cameraX;
            if (!g_monsters.turtles[j].isDead && g_monsters.turtles[j].isAlive && fireball[i].active)
            {
                if (IsColliding(turtleScreenX, g_monsters.turtles[j].y, g_monsters.turtles[j].width, g_monsters.turtles[j].height,
                    fireballScreenX, fireball[i].y, fireball[i].width, fireball[i].height))
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
            int turtleScreenX = g_monsters.angelTurtles[j].x - g_game.cameraX;
            if (!g_monsters.angelTurtles[j].isDead && g_monsters.angelTurtles[j].isAlive && fireball[i].active)
            {
                if (IsColliding(turtleScreenX, g_monsters.angelTurtles[j].y, g_monsters.angelTurtles[j].width, g_monsters.angelTurtles[j].height,
                    fireballScreenX, fireball[i].y, fireball[i].width, fireball[i].height))
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

        int bowserScreenX = g_monsters.bowser.x - g_game.cameraX;
        if (!g_monsters.bowser.isDead && g_monsters.bowser.isAlive && fireball[i].active)
        {
            if (IsColliding(bowserScreenX, g_monsters.bowser.y, g_monsters.bowser.width, g_monsters.bowser.height,
                fireballScreenX, fireball[i].y, fireball[i].width, fireball[i].height))
            {
                PlaySoundBuffer(bump_Sound);
                g_monsters.bowser.hp--;
                fireball[i].active = false;
            }
        }
    }
}
void HandleTinoFireHit()
{
    Graphics graphics(g_memDC);
    graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);
    for (int i = 0; i < MAX_SHOT; i++)
    {
        if (!tinofire[i].active) continue;

        int tinofireScreenX = tinofire[i].x - g_game.cameraX;

        for (int j = 0; j < MAX_MONSTERS; j++)
        {
            int monsterScreenX = g_monsters.monsters[j].x - g_game.cameraX;
            if (!g_monsters.monsters[j].isDead && g_monsters.monsters[j].isAlive && tinofire[i].active) // 여러 조건 같이 확인
            {
                if (IsColliding(monsterScreenX, g_monsters.monsters[j].y, g_monsters.monsters[j].width, g_monsters.monsters[j].height,
                    tinofireScreenX, tinofire[i].y, tinofire[i].width, tinofire[i].height))
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
            int turtleScreenX = g_monsters.turtles[j].x - g_game.cameraX;
            if (!g_monsters.turtles[j].isDead && g_monsters.turtles[j].isAlive && tinofire[i].active)
            {
                if (IsColliding(turtleScreenX, g_monsters.turtles[j].y, g_monsters.turtles[j].width, g_monsters.turtles[j].height,
                    tinofireScreenX, tinofire[i].y, tinofire[i].width, tinofire[i].height))
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
            int angelturtleScreenX = g_monsters.angelTurtles[j].x - g_game.cameraX;
            if (!g_monsters.angelTurtles[j].isDead && g_monsters.angelTurtles[j].isAlive && tinofire[i].active)
            {
                if (IsColliding(angelturtleScreenX, g_monsters.angelTurtles[j].y + 20, g_monsters.angelTurtles[j].width, g_monsters.angelTurtles[j].height,
                    tinofireScreenX, tinofire[i].y, tinofire[i].width, tinofire[i].height))
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

        int bowserScreenX = g_monsters.bowser.x - g_game.cameraX;
        if (!g_monsters.bowser.isDead && g_monsters.bowser.isAlive && tinofire[i].active)
        {
            if (IsColliding(bowserScreenX, g_monsters.bowser.y, g_monsters.bowser.width, g_monsters.bowser.height,
                tinofireScreenX, tinofire[i].y, tinofire[i].width, tinofire[i].height) && !g_monsters.bowser.ignoreTinoFire)
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
    // 마리오가 바라보는 방향 앞 50px 범위가 바이트 히트박스
    const int biteX      = (g_player.mario.direction == DIR_LEFT)
                         ? g_player.mario.x - TINO_BITE_WIDTH
                         : g_player.mario.x + g_player.mario.width;
    const int biteY      = g_player.mario.y + TINO_BITE_Y_OFFSET;
    const int biteWidth  = TINO_BITE_WIDTH;
    const int biteHeight = TINO_BITE_HEIGHT;

    for (int j = 0; j < MAX_MONSTERS; j++)
    {
        int monsterScreenX = g_monsters.monsters[j].x - g_game.cameraX;
        if (!g_monsters.monsters[j].isDead && g_monsters.monsters[j].isAlive)
        {
            if (IsColliding(monsterScreenX, g_monsters.monsters[j].y, g_monsters.monsters[j].width, g_monsters.monsters[j].height,
                biteX, biteY, biteWidth, biteHeight))
            {
                PlaySoundBuffer(kick_Sound);
                g_monsters.monsters[j].isDead    = true;
                g_monsters.monsters[j].isFalling = true;
                g_monsters.monsters[j].vy        = TINO_BITE_KNOCKBACK_VY;
            }
        }
    }

    for (int j = 0; j < MAX_TURTLES; j++)
    {
        int turtleScreenX = g_monsters.turtles[j].x - g_game.cameraX;
        if (!g_monsters.turtles[j].isDead && g_monsters.turtles[j].isAlive)
        {
            if (IsColliding(turtleScreenX, g_monsters.turtles[j].y, g_monsters.turtles[j].width, g_monsters.turtles[j].height,
                biteX, biteY, biteWidth, biteHeight))
            {
                PlaySoundBuffer(kick_Sound);
                g_monsters.turtles[j].isDead    = true;
                g_monsters.turtles[j].isFalling = true;
                g_monsters.turtles[j].vy        = TINO_BITE_KNOCKBACK_VY;
            }
        }
    }

    for (int j = 0; j < MAX_TURTLES; j++)
    {
        int turtleScreenX = g_monsters.angelTurtles[j].x - g_game.cameraX;
        if (!g_monsters.angelTurtles[j].isDead && g_monsters.angelTurtles[j].isAlive)
        {
            if (IsColliding(turtleScreenX, g_monsters.angelTurtles[j].y, g_monsters.angelTurtles[j].width, g_monsters.angelTurtles[j].height,
                biteX, biteY, biteWidth, biteHeight))
            {
                PlaySoundBuffer(kick_Sound);
                g_monsters.angelTurtles[j].isDead    = true;
                g_monsters.angelTurtles[j].isFalling = true;
                g_monsters.angelTurtles[j].vy        = TINO_BITE_KNOCKBACK_VY;
                break;
            }
        }
    }

    int bowserScreenX = g_monsters.bowser.x - g_game.cameraX;
    if (!g_monsters.bowser.isDead && g_monsters.bowser.isAlive)
    {
        if (IsColliding(bowserScreenX, g_monsters.bowser.y, g_monsters.bowser.width, g_monsters.bowser.height,
            biteX, biteY, biteWidth, biteHeight) && !g_monsters.bowser.ignoreTinoBite)
        {
            PlaySoundBuffer(kick_Sound);
            g_monsters.bowser.hp -= 10;
            g_monsters.bowser.ignoreTinoBite = true;
        }
    }
}
static void UpdateFireballs(Shot (&shots)[MAX_SHOT])
{
    for (int i = 0; i < MAX_SHOT; i++)
    {
        if (!shots[i].active) continue;
        shots[i].motion++;
        if (shots[i].motion > 3)
        {
            shots[i].motion = 0;
        }

        shots[i].x += shots[i].vx;    // 수평이동

        // 충돌 계산
        int left = shots[i].x / TILE_SIZE;
        int right = (shots[i].x + shots[i].width - 1) / TILE_SIZE;
        int top = shots[i].y / TILE_SIZE;
        int bottom = (shots[i].y + shots[i].height - 1) / TILE_SIZE;
        int middle = (shots[i].y + 30 / 2 - 1) / TILE_SIZE;

        // 2️⃣ 수평 충돌 처리
        if (left < 0 || right >= MAP_WIDTH || top < 0 || bottom >= MAP_HEIGHT || middle >= MAP_HEIGHT)
        {
            shots[i].active = false;
            continue;
        }
        if (shots[i].vx < 0 &&
            (isSolidTile(currentMap[top][left]) || isSolidTile(currentMap[bottom][left]) || isSolidTile(currentMap[middle][left])))
        {
            shots[i].active = false;
            shots[i].fade = true;
        }
        else if (shots[i].vx > 0 &&
            (isSolidTile(currentMap[top][right]) || isSolidTile(currentMap[bottom][right]) || isSolidTile(currentMap[middle][right])))
        {
            shots[i].active = false;
            shots[i].fade = true;
        }

        // 3️⃣ 수직 낙하 (중력)
        shots[i].vy += 1;
        if (shots[i].vy > 10) shots[i].vy = 5;
        shots[i].y += shots[i].vy;

        left = shots[i].x / TILE_SIZE;
        right = (shots[i].x + TILE_SIZE - 1) / TILE_SIZE;
        top = shots[i].y / TILE_SIZE;
        bottom = (shots[i].y + TILE_SIZE - 1) / TILE_SIZE;

        // 아래 충돌 처리
        if (left < 0 || right >= MAP_WIDTH || bottom < 0 || bottom >= MAP_HEIGHT)
        {
            shots[i].active = false;
            continue;
        }
        if (shots[i].vy > 0 && (isSolidTile(currentMap[bottom][left]) || isSolidTile(currentMap[bottom][right])))
        {
            shots[i].y = bottom * TILE_SIZE - shots[i].height - 10;
            shots[i].vy = -9;
        }
    }
}
static void UpdateTinoFires(Shot (&shots)[MAX_SHOT])
{
    for (int i = 0; i < MAX_SHOT; i++)
    {
        if (!shots[i].active) continue;
        shots[i].motion++;
        shots[i].duration++;
        if (shots[i].motion > 6)
        {
            shots[i].motion = 0;
        }
        if (shots[i].duration == 100)
        {
            shots[i].active = false;
            continue;
        }

        shots[i].x += shots[i].vx;    // 수평이동
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
static void SendShotSpawn(ShotType type, int slot, const Shot& shot)
{
    if (!g_game.isMulti || g_game.isRenderTest || g_net.myPlayerId < 0 || g_net.myPlayerId > 2) return;
    struct Packet { PacketHeader header; ShotSpawnData spawn; };
    static_assert(sizeof(Packet) == sizeof(PacketHeader) + sizeof(ShotSpawnData));
    const Packet packet = {
        { sizeof(Packet), PKT_SHOT_SPAWN, static_cast<uint8_t>(g_net.myPlayerId) },
        { type, static_cast<uint8_t>(slot), static_cast<uint8_t>(shot.direction),
          static_cast<int16_t>(shot.x), static_cast<int16_t>(shot.y),
          static_cast<int16_t>(shot.vx), static_cast<int16_t>(shot.vy),
          static_cast<uint8_t>(shot.width), static_cast<uint8_t>(shot.height) }
    };
    if (g_net.isHost) SendToAll(&packet, sizeof(packet));
    else SendToHost(&packet, sizeof(packet));
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
    fireball[fireball_count].vx = (g_player.mario.direction == DIR_LEFT) ? -7 : 7;
    fireball[fireball_count].vy = 0;
    fireball[fireball_count].width = 10;
    fireball[fireball_count].height = 10;
    fireball[fireball_count].active = true;
    fireball[fireball_count].direction = g_player.mario.direction;
    SendShotSpawn(SHOT_FIREBALL, fireball_count, fireball[fireball_count]);
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
    tinofire[tinofire_count].vx = (g_player.mario.direction == DIR_LEFT) ? -7 : 7;
    tinofire[tinofire_count].vy = 0;
    tinofire[tinofire_count].motion = 0;
    tinofire[tinofire_count].motion_fade = 0;
    tinofire[tinofire_count].duration = 0;
    tinofire[tinofire_count].direction = g_player.mario.direction;
    tinofire[tinofire_count].width = TILE_SIZE * 2;
    tinofire[tinofire_count].height = TILE_SIZE * 2;
    tinofire[tinofire_count].active = true;
    tinofire[tinofire_count].fade = false;
    SendShotSpawn(SHOT_TINOFIRE, tinofire_count, tinofire[tinofire_count]);
    tinofire_count++;
}
RemoteShots remoteShots[3] = {};

bool ApplyShotSpawn(int owner, const ShotSpawnData& spawn)
{
    if (owner < 0 || owner >= 3 || owner == g_localPlayerIdx ||
        spawn.slot >= MAX_SHOT || spawn.shotType > SHOT_TINOFIRE ||
        spawn.direction > DIR_RIGHT || !spawn.width || !spawn.height) return false;
    auto& pool = remoteShots[owner];
    Shot& shot = spawn.shotType == SHOT_FIREBALL ? pool.fireballs[spawn.slot] : pool.tinoFires[spawn.slot];
    shot = {};
    shot.x = spawn.x;
    shot.y = spawn.y;
    shot.vx = spawn.vx;
    shot.vy = spawn.vy;
    shot.width = spawn.width;
    shot.height = spawn.height;
    shot.direction = static_cast<Direction>(spawn.direction);
    shot.active = true;
    return true;
}

void UpdateRemoteShots()
{
    for (auto& pool : remoteShots)
    {
        UpdateFireballs(pool.fireballs);
        UpdateTinoFires(pool.tinoFires);
    }
}

void UpdateAllShots()
{
    UpdateFireballs(fireball);
    UpdateTinoFires(tinofire);
    UpdateShot_tinofire_effect();
}
void HandleShotCollisions()
{
    HandleFireballHit();
    HandleTinoFireHit();
}
