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
                    SendMonsterHit(MON_NORMAL, j, HIT_FIREBALL, fireball[i].spawnId);
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
                    SendMonsterHit(MON_TURTLE, j, HIT_FIREBALL, fireball[i].spawnId);
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
                    SendMonsterHit(MON_ANGEL_TURTLE, j, HIT_FIREBALL, fireball[i].spawnId);
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
                SendMonsterHit(MON_BOSS, 0, HIT_FIREBALL);
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
                    SendMonsterHit(MON_NORMAL, j, HIT_TINOFIRE, tinofire[i].spawnId);
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
                    SendMonsterHit(MON_TURTLE, j, HIT_TINOFIRE, tinofire[i].spawnId);
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
                    SendMonsterHit(MON_ANGEL_TURTLE, j, HIT_TINOFIRE, tinofire[i].spawnId);
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
                SendMonsterHit(MON_BOSS, 0, HIT_TINOFIRE);
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
                SendMonsterHit(MON_NORMAL, j, HIT_TINOBITE);
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
                SendMonsterHit(MON_TURTLE, j, HIT_TINOBITE);
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
                SendMonsterHit(MON_ANGEL_TURTLE, j, HIT_TINOBITE);
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
            SendMonsterHit(MON_BOSS, 0, HIT_TINOBITE);
            g_monsters.bowser.ignoreTinoBite = true;
        }
    }
}
static bool IsSolidShotTile(int row, int column)
{
    return currentMap && row >= 0 && row < MAP_HEIGHT && column >= 0 && column < MAP_WIDTH &&
        isSolidTile(currentMap[row][column]);
}

static int ShotTileAt(int position)
{
    return position < 0 ? -1 : position / TILE_SIZE;
}

static void UpdateFireballs(Shot (&shots)[MAX_SHOT])
{
    for (Shot& shot : shots)
    {
        if (!shot.active) continue;
        shot.motion = (shot.motion + 1) % 4;
        shot.x += shot.vx;
        if (shot.x < 0 || shot.x >= MAP_WIDTH * TILE_SIZE)
        {
            shot.active = false;
            continue;
        }
        const int left = ShotTileAt(shot.x);
        const int edge = shot.vx < 0 ? left : ShotTileAt(shot.x + shot.width - 1);
        if (IsSolidShotTile(ShotTileAt(shot.y), edge) ||
            IsSolidShotTile(ShotTileAt(shot.y + shot.height - 1), edge) ||
            IsSolidShotTile(ShotTileAt(shot.y + 30 / 2 - 1), edge))
        {
            shot.active = false;
            shot.fade = true;
            continue;
        }

        if (++shot.vy > 10) shot.vy = 5;
        shot.y += shot.vy;
        if (shot.y >= MAP_HEIGHT * TILE_SIZE)
        {
            shot.active = false;
            continue;
        }
        const int right = ShotTileAt(shot.x + TILE_SIZE - 1);
        const int bottom = ShotTileAt(shot.y + TILE_SIZE - 1);
        if (shot.vy > 0 && (IsSolidShotTile(bottom, left) || IsSolidShotTile(bottom, right)))
        {
            shot.y = bottom * TILE_SIZE - shot.height - 10;
            shot.vy = -9;
        }
    }
}

static void UpdateTinoFires(Shot (&shots)[MAX_SHOT])
{
    for (Shot& shot : shots)
    {
        if (!shot.active) continue;
        shot.motion = (shot.motion + 1) % 7;
        if (++shot.duration >= 100)
        {
            shot.active = false;
            continue;
        }
        shot.x += shot.vx;
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
RemoteShots remoteShots[3] = {};

constexpr int PlayerShotSpeed = 7;

static int PlayerShotSize(ShotType type)
{
    return type == SHOT_FIREBALL ? 10 : TILE_SIZE * 2;
}

static int PlayerShotVelocity(Direction direction)
{
    return direction == DIR_LEFT ? -PlayerShotSpeed : PlayerShotSpeed;
}

static void InitializePlayerShot(Shot& shot, int x, int y, Direction direction, ShotType type)
{
    shot = {};
    shot.x = x;
    shot.y = y;
    shot.direction = direction;
    shot.vx = PlayerShotVelocity(direction);
    shot.width = PlayerShotSize(type);
    shot.height = shot.width;
    shot.active = true;
}

static void SpawnPlayerShot(Shot (&shots)[MAX_SHOT], int slot, int x, int y, ShotType type)
{
    static uint32_t nextSpawnId = 0;
    if (++nextSpawnId == 0) ++nextSpawnId;

    Shot& shot = shots[slot];
    InitializePlayerShot(shot, x, y, g_player.mario.direction, type);
    shot.spawnId = nextSpawnId;
    if (!IsMultiplayerGame() || !g_game.gameStart || g_net.myPlayerId < 0 || g_net.myPlayerId >= 3) return;

    struct Packet { PacketHeader header; ShotSpawnData spawn; };
    static_assert(sizeof(Packet) == sizeof(PacketHeader) + sizeof(ShotSpawnData));
    const Packet packet = {
        { sizeof(Packet), PKT_SHOT_SPAWN, static_cast<uint8_t>(g_net.myPlayerId) },
        { type, static_cast<uint8_t>(slot), static_cast<uint8_t>(shot.direction),
          static_cast<int16_t>(shot.x), static_cast<int16_t>(shot.y),
          static_cast<int16_t>(shot.vx), static_cast<int16_t>(shot.vy),
          static_cast<uint8_t>(shot.width), static_cast<uint8_t>(shot.height), shot.spawnId }
    };
    if (g_net.isHost) SendToAll(&packet, sizeof(packet));
    else SendToHost(&packet, sizeof(packet));
}

void SpawnFireball(int x, int y)
{
    static int slot = 0;
    SpawnPlayerShot(fireball, slot, x, y, SHOT_FIREBALL);
    slot = (slot + 1) % MAX_SHOT;
}

void SpawnTinoFire(int x, int y)
{
    static int slot = 0;
    SpawnPlayerShot(tinofire, slot, x, y, SHOT_TINOFIRE);
    slot = (slot + 1) % MAX_SHOT;
}

bool ApplyShotSpawn(int owner, const ShotSpawnData& spawn)
{
    if (owner < 0 || owner >= 3 || owner == g_localPlayerIdx || !spawn.spawnId ||
        spawn.slot >= MAX_SHOT || spawn.shotType > SHOT_TINOFIRE || spawn.direction > DIR_RIGHT ||
        spawn.x < 0 || spawn.x >= MAP_WIDTH * TILE_SIZE || spawn.y >= PLAYER_FALL_DEATH_Y) return false;

    const ShotType type = static_cast<ShotType>(spawn.shotType);
    const Direction direction = static_cast<Direction>(spawn.direction);
    const int size = PlayerShotSize(type);
    if (spawn.width != size || spawn.height != size ||
        spawn.vx != PlayerShotVelocity(direction) || spawn.vy != 0) return false;

    if (g_net.isHost)
    {
        const Player& player = g_allPlayers[owner].mario;
        const MarioForm requiredForm = type == SHOT_FIREBALL ? FORM_FLOWER : FORM_TINO;
        if (player.isDead || player.isTransforming || player.form != requiredForm ||
            abs(spawn.x - player.x) > TILE_SIZE || abs(spawn.y - player.y) > TILE_SIZE) return false;
    }

    RemoteShots& pool = remoteShots[owner];
    Shot& shot = type == SHOT_FIREBALL ? pool.fireballs[spawn.slot] : pool.tinoFires[spawn.slot];
    const uint32_t steps = spawn.spawnId - shot.spawnId;
    if (shot.spawnId && (steps == 0 || steps > INT32_MAX)) return false;
    InitializePlayerShot(shot, spawn.x, spawn.y, direction, type);
    shot.spawnId = spawn.spawnId;
    return true;
}

Shot* FindActivePlayerShot(int owner, ShotType type, uint32_t spawnId)
{
    if (owner < 0 || owner >= 3 || !spawnId) return nullptr;

    const bool local = owner == g_localPlayerIdx;
    Shot* pool = type == SHOT_FIREBALL ?
        (local ? fireball : remoteShots[owner].fireballs) :
        (local ? tinofire : remoteShots[owner].tinoFires);
    for (int slot = 0; slot < MAX_SHOT; ++slot)
        if (pool[slot].active && pool[slot].spawnId == spawnId)
            return &pool[slot];
    return nullptr;
}

void DespawnPlayerFireball(int owner, Shot& shot)
{
    shot.active = false;
    if (!IsMultiplayerGame() || !g_net.isHost) return;

    struct Packet { PacketHeader header; uint32_t spawnId; };
    static_assert(sizeof(Packet) == sizeof(PacketHeader) + sizeof(uint32_t));
    const Packet packet = {
        { sizeof(Packet), PKT_FIREBALL_DESPAWN, static_cast<uint8_t>(owner) }, shot.spawnId
    };
    SendToAll(&packet, sizeof(packet));
}

void ApplyFireballDespawn(int owner, uint32_t spawnId)
{
    Shot* shot = FindActivePlayerShot(owner, SHOT_FIREBALL, spawnId);
    if (shot) shot->active = false;
}

void UpdateAllShots()
{
    UpdateFireballs(fireball);
    UpdateTinoFires(tinofire);
    if (IsMultiplayerGame())
        for (RemoteShots& pool : remoteShots)
        {
            UpdateFireballs(pool.fireballs);
            UpdateTinoFires(pool.tinoFires);
        }
    UpdateShot_tinofire_effect();
}
void HandleShotCollisions()
{
    HandleFireballHit();
    HandleTinoFireHit();
}
