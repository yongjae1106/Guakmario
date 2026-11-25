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

static int TileColumnAt(int worldX)
{
    return worldX < 0 ? -1 : worldX / TILE_SIZE;
}

static int PlayerCollisionTile(int row, int column)
{
    if (!currentMap || row < 0 || row >= MAP_HEIGHT || column < 0 || column >= MAP_WIDTH)
        return TILE_EMPTY;
    return currentMap[row][column];
}

int CameraXForPlayer(int worldX)
{
    const int desired = worldX - SCREEN_WIDTH / 2;
    constexpr int rightEdge = MAP_WIDTH * TILE_SIZE - SCREEN_WIDTH;
    if (desired < 0) return 0;
    if (desired > rightEdge) return rightEdge;
    return desired;
}

using BlockItemSpawner = void (*)(int, int);

static BlockItemSpawner GetBlockItemSpawner(int tile)
{
    switch (tile)
    {
    case TILE_MYSTERY:         return SpawnItem_mushroom;
    case TILE_BOX_STAR:
    case TILE_BOX_STAR_HIDDEN: return SpawnItem_star;
    case TILE_BOX_FLOWER:      return SpawnItem_flower;
    case TILE_BOX_TINO:        return SpawnItem_tino;
    case TILE_BOX_UPMUSH:      return SpawnItem_up_mushroom;
    default:                  return nullptr;
    }
}

void ApplyTileChange(int playerId, const TileChangeData& change)
{
    if (!currentMap || change.row >= MAP_HEIGHT || change.col >= MAP_WIDTH) return;

    int& tile = currentMap[change.row][change.col];
    const bool givesCoin = (tile == TILE_COIN && change.tile == TILE_EMPTY) ||
        (tile == TILE_BOX_COIN && change.tile == TILE_USED_BLOCK);
    const bool opensItemBlock = change.tile == TILE_USED_BLOCK && GetBlockItemSpawner(tile);
    if (!givesCoin && !opensItemBlock) return;

    tile = change.tile;
    if (playerId != g_localPlayerIdx) return;

    PlaySoundBuffer(givesCoin ? coin_Sound : powerup_appears_Sound);
    if (givesCoin) ++g_player.mario.coin;
}

static void CommitTileChange(int row, int column, TileType tile, int playerId)
{
    struct Packet { PacketHeader header; TileChangeData change; };
    static_assert(sizeof(Packet) == sizeof(PacketHeader) + sizeof(TileChangeData));
    const Packet packet = {
        { sizeof(Packet), PKT_TILE_CHANGE, static_cast<uint8_t>(playerId) },
        { static_cast<uint8_t>(row), static_cast<uint8_t>(column), static_cast<uint16_t>(tile) }
    };
    ApplyTileChange(playerId, packet.change);
    if (IsMultiplayerGame() && g_net.isHost) SendToAll(&packet, sizeof(packet));
}

static void RequestTileHit(int row, int column)
{
    struct Packet { PacketHeader header; BlockHitData hit; };
    static_assert(sizeof(Packet) == sizeof(PacketHeader) + sizeof(BlockHitData));
    const Packet packet = {
        { sizeof(Packet), PKT_BLOCK_HIT, static_cast<uint8_t>(g_net.myPlayerId) },
        { static_cast<uint8_t>(row), static_cast<uint8_t>(column) }
    };
    SendToHost(&packet, sizeof(packet));
}

void CollectMapCoin(int row, int column)
{
    if (PlayerCollisionTile(row, column) != TILE_COIN) return;

    if (IsMultiplayerGame() && !g_net.isHost) RequestTileHit(row, column);
    else CommitTileChange(row, column, TILE_EMPTY, g_localPlayerIdx);
}

static void HandleBlockHit(int row, int column, int playerId)
{
    const int tile = PlayerCollisionTile(row, column);
    const BlockItemSpawner spawn = GetBlockItemSpawner(tile);
    if (!spawn && tile != TILE_BOX_COIN) return;

    if (IsMultiplayerGame() && !g_net.isHost)
    {
        RequestTileHit(row, column);
        return;
    }

    if (PlayerCollisionTile(row - 1, column) == TILE_COIN)
        CommitTileChange(row - 1, column, TILE_EMPTY, playerId);
    CommitTileChange(row, column, TILE_USED_BLOCK, playerId);
    if (spawn) spawn(column * TILE_SIZE, row * TILE_SIZE);
}

void HandleRemoteTileHit(int playerId, const BlockHitData& hit)
{
    if (playerId < 1 || playerId >= 3) return;

    const Player& player = g_allPlayers[playerId].mario;
    if (player.isDead || player.isTransforming) return;

    const int tile = PlayerCollisionTile(hit.row, hit.col);
    const int tileX = hit.col * TILE_SIZE;
    const int tileY = hit.row * TILE_SIZE;
    // 요청이 해당 프레임의 플레이어 상태보다 먼저 도착하므로 직전 위치를 한 프레임 확장한다.
    constexpr int maxHeadMovement = -PLAYER_JUMP_VY;
    if (tile == TILE_COIN)
    {
        if (IsColliding(player.x - PLAYER_WALK_SPEED, player.y - maxHeadMovement,
            player.width + 2 * PLAYER_WALK_SPEED, player.height + maxHeadMovement + PLAYER_GRAVITY_MAX,
            tileX, tileY, 30, 30))
            CommitTileChange(hit.row, hit.col, TILE_EMPTY, playerId);
        return;
    }

    const int headCenter = player.x + TILE_SIZE / 2;
    const int tileBottom = tileY + TILE_SIZE;
    if (headCenter >= tileX - PLAYER_WALK_SPEED && headCenter < tileX + TILE_SIZE + PLAYER_WALK_SPEED &&
        player.y >= tileBottom && player.y <= tileBottom + maxHeadMovement)
        HandleBlockHit(hit.row, hit.col, playerId);
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
    int left = TileColumnAt(g_player.mario.x + g_game.cameraX);
    int right = TileColumnAt(g_player.mario.x + g_player.mario.width - 1 + g_game.cameraX);
    int top = g_player.mario.y / TILE_SIZE;
    int bottom = (g_player.mario.y + g_player.mario.height - 1) / TILE_SIZE;
    int middle = (g_player.mario.y + g_player.mario.height / 2 - 1) / TILE_SIZE;

    // 블럭 왼쪽 벽 충돌
    if (!(g_player.mario.y < 0) && g_player.mario.vx < 0 &&
        (isSolidTile(PlayerCollisionTile(top, left)) ||
         isSolidTile(PlayerCollisionTile(bottom, left)) ||
         isSolidTile(PlayerCollisionTile(middle, left))))
    {
        g_player.mario.x = left * TILE_SIZE - g_game.cameraX + TILE_SIZE;
    }

    // 블럭 오른쪽 벽 충돌
    if (!(g_player.mario.y < 0) && g_player.mario.vx > 0 &&
        (isSolidTile(PlayerCollisionTile(top, right)) ||
         isSolidTile(PlayerCollisionTile(bottom, right)) ||
         isSolidTile(PlayerCollisionTile(middle, right))))
    {
        g_player.mario.x = right * TILE_SIZE - g_game.cameraX - g_player.mario.width;
    }

    // 중력 적용
    g_player.mario.isFlying = true;
    g_player.mario.vy += 1;
    if (g_player.mario.vy > PLAYER_GRAVITY_MAX) g_player.mario.vy = PLAYER_GRAVITY_MAX;
    g_player.mario.y += g_player.mario.vy;

    // y축 충돌 다시 계산
    left = TileColumnAt(g_player.mario.x + g_game.cameraX);
    right = TileColumnAt(g_player.mario.x + g_player.mario.width - 1 + g_game.cameraX);
    top = g_player.mario.y / TILE_SIZE;
    bottom = (g_player.mario.y + g_player.mario.height - 1) / TILE_SIZE;

    // 아래 충돌
    const int floorLeft = PlayerCollisionTile(bottom, left);
    const int floorRight = PlayerCollisionTile(bottom, right);
    if (!(g_player.mario.y < 0) && g_player.mario.vy > 0 && (isSolidTile(floorLeft) || isSolidTile(floorRight)))
    {
        if (floorLeft == TILE_LAVA_HEAD || floorRight == TILE_LAVA_HEAD ||
            floorLeft == TILE_LAVA_BODY || floorRight == TILE_LAVA_BODY)
        {
            dead();
        }
        g_player.mario.y = bottom * TILE_SIZE - g_player.mario.height;
        g_player.mario.vy = 0;
        g_player.mario.isJumping = false;
        g_player.mario.isFlying = false;
    }
    // 위 충돌
    else if (!(g_player.mario.y < 0) && g_player.mario.vy < 0 &&
        (isSolidTile(PlayerCollisionTile(top, left)) || isSolidTile(PlayerCollisionTile(top, right))))
    {
        // 마리오가 정통으로 친 블럭 찾기
        int centerX = g_player.mario.x + TILE_SIZE / 2 + g_game.cameraX;
        int blockX = TileColumnAt(centerX);

        g_player.mario.y = (top + 1) * TILE_SIZE;
        g_player.mario.vy = 0;

        HandleBlockHit(top, blockX, g_localPlayerIdx);

    }


    // 벽 충돌로 보정된 월드 위치를 유지하면서 카메라를 이동한다.
    int worldX = g_player.mario.x + static_cast<int>(g_game.cameraX);
    const int rightEdge = MAP_WIDTH * TILE_SIZE - g_player.mario.width;
    if (worldX < 0) worldX = 0;
    if (worldX > rightEdge) worldX = rightEdge;
    const int cameraX = CameraXForPlayer(worldX);
    g_game.cameraX = cameraX;
    g_player.mario.x = worldX - cameraX;

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
    const bool online = IsMultiplayerGame();
    Player& player = g_player.mario;
    if (online && (player.isDead || g_game.gameState != GAME_RUNNING || g_game.multiplayerArrived)) return;

    if (!online && g_pBGMBuffer) g_pBGMBuffer->Stop();
    PlaySoundBuffer(die_Sound);
    player.isDead = true;
    player.isTransforming = false;
    player.form = FORM_SMALL;
    g_game.deadStartTime = GetTickCount();

    if (online)
    {
        g_game.multiplayerDeathMotionStarted = false;
        ReportMultiplayerDeath();
        return;
    }

    g_game.stage_time = STAGE_TIME_LIMIT;
    g_game.gameState = GAME_OVER;
    --player.life;
}
void resurrection()
{
    if (IsMultiplayerGame())
    {
        if (g_player.mario.star) SetStage_BGM();
        g_game.cameraX = 0;
        ResetMario(g_sharedLives, g_player.mario.coin);
        g_game.multiplayerDeathMotionStarted = false;
        return;
    }

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
    const uint32_t tinoBiteId = g_player.mario.tinoBiteId;
    g_player.mario = {};
    g_player.mario.charType = charType;
    g_player.mario.tinoBiteId = tinoBiteId;
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
    p.life = IsMultiplayerGame() ? g_sharedLives : state.life;
    p.form = (MarioForm)state.form;
    p.charType = (CharacterType)state.charType;
    p.width = state.width;
    p.height = state.height;
    p.walk_motion = state.walkMotion;
    p.motion_timer = state.motionTimer;
    p.tino_cooldown_z = state.tinoCooldownZ;
    p.tino_cooldown_space = state.tinoCooldownSpace;
    p.tinoBiteId = state.tinoBiteId;
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
