#include "network.h" // Winsock must precede windows.h.
#include <tchar.h>
#include "data.h"
#include "image.h"
#include "item.h"
#include "sound.h"
#include "monster.h"
#include "map.h"
#include "player.h"
#include "game.h"
#include "map.h"
#include "collision.h"
#include "config.h"

using namespace Gdiplus;

void movePlayer()
{
    if (g_player.keyState[VK_LEFT] && !g_player.keyState[VK_RIGHT])
    {
        g_player.mario.vx = -PLAYER_WALK_SPEED;
        g_player.mario.direction = DIR_LEFT;
    }
    else if (g_player.keyState[VK_RIGHT] && !g_player.keyState[VK_LEFT])
    { 
        g_player.mario.vx = PLAYER_WALK_SPEED;
        g_player.mario.direction = DIR_RIGHT;
    }
    else if (g_player.keyState[VK_LEFT] && g_player.keyState[VK_RIGHT])
    {
        // 둘 다 눌린 경우 마지막 방향 유지
        g_player.mario.vx = (g_player.mario.direction == DIR_LEFT ? -PLAYER_WALK_SPEED : PLAYER_WALK_SPEED);
    }
    else 
    {
        g_player.mario.isWalking = false;
        g_player.mario.vx = 0;
        g_player.mario.walk_motion = 0;
    }
}

// 물리엔진

static void tick_motion(bool& flag, int& timer)
{
    if (flag)
    {
        if (timer > 0) timer--;
        else flag = false;
    }
}

void UpdateMario_motion()
{
    tick_motion(g_player.mario.fire_motion,      g_player.mario.motion_timer);
    tick_motion(g_player.mario.tino_motion,      g_player.mario.motion_timer);
    tick_motion(g_player.mario.tino_fire_motion, g_player.mario.motion_timer);
}

static void SetBlockTile(int row, int col, TileType tile, int hitterId)
{
    currentMap[row][col] = tile;
    if (g_game.isMulti && !g_game.isRenderTest && g_net.isHost)
    {
        struct Packet { PacketHeader header; TileChangeData change; };
        static_assert(sizeof(Packet) == sizeof(PacketHeader) + sizeof(TileChangeData));
        const Packet packet = {
            { sizeof(Packet), PKT_TILE_CHANGE, static_cast<uint8_t>(hitterId) },
            { static_cast<uint8_t>(row), static_cast<uint8_t>(col), static_cast<uint16_t>(tile) }
        };
        SendToAll(&packet, sizeof(packet));
    }
}

static void SendTileRequest(int row, int col)
{
    struct Packet { PacketHeader header; BlockHitData hit; };
    static_assert(sizeof(Packet) == sizeof(PacketHeader) + sizeof(BlockHitData));
    const Packet request = {
        { sizeof(Packet), PKT_BLOCK_HIT, static_cast<uint8_t>(g_net.myPlayerId) },
        { static_cast<uint8_t>(row), static_cast<uint8_t>(col) }
    };
    SendToHost(&request, sizeof(request));
}

void CollectMapCoin(int row, int col, int collectorId)
{
    if (row < 0 || row >= MAP_HEIGHT || col < 0 || col >= MAP_WIDTH ||
        currentMap[row][col] != TILE_COIN) return;
    if (g_game.isMulti && !g_game.isRenderTest && !g_net.isHost)
    {
        SendTileRequest(row, col);
        return;
    }
    SetBlockTile(row, col, TILE_EMPTY, collectorId);
    if (collectorId == g_localPlayerIdx)
    {
        PlaySoundBuffer(coin_Sound);
        ++g_player.mario.coin;
    }
}

void HandleBlockHit(int row, int col, int hitterId)
{
    if (row < 0 || row >= MAP_HEIGHT || col < 0 || col >= MAP_WIDTH) return;

    void (*spawn)(int, int) = nullptr;
    switch (currentMap[row][col])
    {
    case TILE_MYSTERY:         spawn = SpawnItem_mushroom; break;
    case TILE_BOX_STAR:
    case TILE_BOX_STAR_HIDDEN: spawn = SpawnItem_star; break;
    case TILE_BOX_FLOWER:      spawn = SpawnItem_flower; break;
    case TILE_BOX_TINO:        spawn = SpawnItem_tino; break;
    case TILE_BOX_UPMUSH:      spawn = SpawnItem_up_mushroom; break;
    case TILE_BOX_COIN:        break;
    default: return;
    }

    const bool online = g_game.isMulti && !g_game.isRenderTest;
    if (online && !g_net.isHost)
    {
        SendTileRequest(row, col);
        return;
    }

    PlaySoundBuffer(spawn ? powerup_appears_Sound : coin_Sound);
    if (row > 0) CollectMapCoin(row - 1, col, hitterId);
    SetBlockTile(row, col, TILE_USED_BLOCK, hitterId);
    if (spawn) spawn(col * TILE_SIZE, row * TILE_SIZE);
    else if (hitterId == g_localPlayerIdx) ++g_player.mario.coin;
}

void UpdatePlayer()
{
    if(g_game.gameState != GAME_VICTORY && g_game.gameState != GAME_CLEAR)
    {
        movePlayer();
    }
    // 수평 이동
    g_player.mario.x += g_player.mario.vx;

    // 좌우 충돌 처리
    int left = (g_player.mario.x + g_game.cameraX) / TILE_SIZE;
    int right = (g_player.mario.x + g_player.mario.width - 1 + g_game.cameraX) / TILE_SIZE;
    int top = g_player.mario.y / TILE_SIZE;
    int bottom = (g_player.mario.y + g_player.mario.height - 1) / TILE_SIZE;
    int middle = (g_player.mario.y + g_player.mario.height / 2 - 1) / TILE_SIZE;

    // 블럭 왼쪽 벽 충돌
    if (!(g_player.mario.y < 0) && g_player.mario.vx < 0 &&
        (isSolidTile(currentMap[top][left]) || isSolidTile(currentMap[bottom][left]) || isSolidTile(currentMap[middle][left])))
    {
        g_player.mario.x = left * TILE_SIZE - g_game.cameraX + TILE_SIZE;
    }

    // 블럭 오른쪽 벽 충돌
    if (!(g_player.mario.y < 0) && g_player.mario.vx > 0 &&
        (isSolidTile(currentMap[top][right]) || isSolidTile(currentMap[bottom][right]) || isSolidTile(currentMap[middle][right])))
    {
        g_player.mario.x = right * TILE_SIZE - g_game.cameraX - g_player.mario.width;
    }

    // 중력 적용
    g_player.mario.isFlying = true;
    g_player.mario.vy += 1;
    if (g_player.mario.vy > PLAYER_GRAVITY_MAX) g_player.mario.vy = PLAYER_GRAVITY_MAX;
    g_player.mario.y += g_player.mario.vy;

    // y축 충돌 다시 계산
    left = (g_player.mario.x + g_game.cameraX) / TILE_SIZE;
    right = (g_player.mario.x + g_player.mario.width - 1 + g_game.cameraX) / TILE_SIZE;
    top = g_player.mario.y / TILE_SIZE;
    bottom = (g_player.mario.y + g_player.mario.height - 1) / TILE_SIZE;

    // 아래 충돌
    if (!(g_player.mario.y < 0) && g_player.mario.vy > 0 && (isSolidTile(currentMap[bottom][left]) || isSolidTile(currentMap[bottom][right])))
    {
        if (currentMap[bottom][left] == TILE_LAVA_HEAD || currentMap[bottom][right] == TILE_LAVA_HEAD ||
            currentMap[bottom][left] == TILE_LAVA_BODY || currentMap[bottom][right] == TILE_LAVA_BODY)
        {
            dead();
        }
        int centerX = g_player.mario.x + TILE_SIZE / 2 + g_game.cameraX;
        int blockX = centerX / TILE_SIZE;
        
        g_player.mario.y = bottom * TILE_SIZE - g_player.mario.height;
        g_player.mario.vy = 0;
        g_player.mario.isJumping = false;
        g_player.mario.isFlying = false;
    }
    // 위 충돌
    else if (!(g_player.mario.y < 0) && g_player.mario.vy < 0 && (isSolidTile(currentMap[top][left]) || isSolidTile(currentMap[top][right])))
    {
        // 마리오가 정통으로 친 블럭 찾기
        int centerX = g_player.mario.x + TILE_SIZE / 2 + g_game.cameraX;
        int blockX = centerX / TILE_SIZE;
        
        g_player.mario.y = (top + 1) * TILE_SIZE;
        g_player.mario.vy = 0;

        HandleBlockHit(top, blockX, g_localPlayerIdx);

    }


    // 마리오 왼쪽 벽 충돌
    if (g_player.mario.x < 0)
    {
        g_player.mario.x = 0;
    }
    // 카메라 이동
    if (g_player.mario.x > SCREEN_WIDTH / 2)
    {
        g_game.cameraX += g_player.mario.vx;
        g_player.mario.x = SCREEN_WIDTH / 2;
    }
    if (g_player.mario.x < SCREEN_WIDTH / 2 && g_game.cameraX > 0)
    {
        g_game.cameraX += g_player.mario.vx;
        g_player.mario.x = SCREEN_WIDTH / 2;
    }

    if (g_game.cameraX < 0) g_game.cameraX = 0;
    if (g_game.cameraX > MAP_WIDTH * TILE_SIZE - SCREEN_WIDTH)
        g_game.cameraX = MAP_WIDTH * TILE_SIZE - SCREEN_WIDTH;

    if (g_player.mario.y > PLAYER_FALL_DEATH_Y)
    {
        dead();
    }
}
void UpdateDeadMotion()
{
    g_player.mario.y += g_player.mario.vy;
    g_player.mario.vy += 1;
    if (g_player.mario.vy > PLAYER_GRAVITY_MAX) g_player.mario.vy = PLAYER_GRAVITY_MAX;
}


void dead()
{
    g_pBGMBuffer->Stop();
    PlaySoundBuffer(die_Sound);
    g_game.gameState = GAME_OVER;
    g_player.mario.isDead = true;
    g_player.mario.isTransforming = false;
    g_player.mario.form = FORM_SMALL;
    g_game.deadStartTime = GetTickCount();

    g_player.mario.life--;
    g_game.stage_time = STAGE_TIME_LIMIT;

}
void resurrection()
{
    ResetItems();
    ResetMonsters();
    LoadStage();
    g_game.cameraX = 0;
    g_game.gameState = GAME_RUNNING;
    ResetMario(g_player.mario.life, g_player.mario.coin);
}

void ResetMario(int life, int coin)
{
    const CharacterType charType = g_game.isMulti ? g_player.mario.charType : CHAR_MARIO;
    g_player.mario = {};
    g_player.mario.charType = charType;
    g_player.mario.x = 100;
    g_player.mario.y = 300;
    g_player.mario.life = life;
    g_player.mario.coin = coin;
    g_player.mario.width = 40;
    g_player.mario.height = 40;
    g_player.mario.direction = DIR_RIGHT;
}

MarioRenderData BuildMarioRenderData()
{
    return BuildMarioRenderDataFor(g_localPlayerIdx);
}

MarioRenderData BuildMarioRenderDataFor(int idx)
{
    const Player& m = g_allPlayers[idx].mario;
    return {
        m.x, m.y, m.width, m.height, m.direction,
        m.form, m.charType,
        m.isWalking, m.isJumping, m.isFlying, m.isDead,
        m.star, m.fire_motion, m.tino_motion, m.tino_fire_motion,
        m.walk_motion, m.motion_timer,
        m.life, m.coin,
        m.tino_cooldown_z, m.tino_cooldown_space,
        m.isTransforming, m.transformTarget
    };
}

bool ApplyRemotePlayerState(int playerId, const PlayerStateData& state)
{
    if (playerId < 0 || playerId >= 3 || playerId == g_localPlayerIdx)
        return false;
    if (state.form > FORM_TINO || state.charType > CHAR_TOAD ||
        state.transformTarget > FORM_TINO || state.width == 0 || state.height == 0)
        return false;

    Player& p  = g_allPlayers[playerId].mario;
    p.x = state.worldX;  // 다른 플레이어는 월드 좌표로 보관
    p.y = state.y;
    p.vx = state.vx;
    p.vy = state.vy;
    p.coin = state.coin;
    p.life = state.life;
    p.form = (MarioForm)state.form;
    p.charType = (CharacterType)state.charType;
    p.width = state.width;
    p.height = state.height;
    p.walk_motion = state.walkMotion;
    p.motion_timer = state.motionTimer;
    p.tino_cooldown_z = state.tinoCooldownZ;
    p.tino_cooldown_space = state.tinoCooldownSpace;
    p.direction = (state.flags & FLAG_FACING_RIGHT) ? DIR_RIGHT : DIR_LEFT;
    p.isJumping = (state.flags & FLAG_JUMPING)          != 0;
    p.isFlying = (state.flags & FLAG_FLYING)           != 0;
    p.isWalking = (state.flags & FLAG_WALKING)          != 0;
    p.isDead = (state.flags & FLAG_DEAD)     != 0;
    p.gameover = (state.flags & FLAG_GAMEOVER)         != 0;
    p.star = (state.flags & FLAG_STAR)     != 0;
    p.god = (state.flags & FLAG_INVINCIBLE)       != 0;
    p.supergod = (state.flags & FLAG_SUPERGOD)         != 0;
    p.fire_motion = (state.flags & FLAG_FIRE_MOTION)      != 0;
    p.tino_motion = (state.flags & FLAG_TINO_MOTION)      != 0;
    p.tino_fire_motion = (state.flags & FLAG_TINO_FIRE_MOTION) != 0;
    p.isTransforming = (state.flags & FLAG_TRANSFORMING) != 0;
    p.transformTarget = static_cast<MarioForm>(state.transformTarget);
    return true;
}
