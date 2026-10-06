#include <tchar.h>
#include "data.h"
#include "image.h"
#include "item.h"
#include "sound.h"
#include "monster.h"
#include "map.h"
#include "func.h"

using namespace Gdiplus;

void movePlayer()
{
    if (g_player.keyState[VK_LEFT] && !g_player.keyState[VK_RIGHT])
    {
        g_player.mario.vx = -5;
        g_player.mario.direction = 0;
    }
    else if (g_player.keyState[VK_RIGHT] && !g_player.keyState[VK_LEFT])
    {
        g_player.mario.vx = 5;
        g_player.mario.direction = 1;
    }
    else if (g_player.keyState[VK_LEFT] && g_player.keyState[VK_RIGHT])
    {
        // 둘 다 눌린 경우 마지막 방향 유지
        g_player.mario.vx = (g_player.mario.direction == 0 ? -5 : 5);
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
    if (g_player.mario.vy > 15) g_player.mario.vy = 15;
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

        // 아이템 블럭 머리로 치기
        if (currentMap[top][blockX] == TILE_MYSTERY)
        {
            PlaySoundBuffer(powerup_appears_Sound);
            if (currentMap[top - 1][blockX] == TILE_COIN)
            {
                PlaySoundBuffer(coin_Sound);
                currentMap[top - 1][blockX] = 0;
                g_player.mario.coin++;
            }
            int itemX = blockX * TILE_SIZE;
            int itemY = top * TILE_SIZE;
            currentMap[top][blockX] = TILE_USED_BLOCK;
            SpawnItem(itemX, itemY);
        }
        else if (currentMap[top][blockX] == TILE_BOX_STAR)
        {
            PlaySoundBuffer(powerup_appears_Sound);
            if (currentMap[top - 1][blockX] == TILE_COIN)
            {
                PlaySoundBuffer(coin_Sound);
                currentMap[top - 1][blockX] = 0;
                g_player.mario.coin++;
            }
            int itemX = blockX * TILE_SIZE;
            int itemY = top * TILE_SIZE;
            currentMap[top][blockX] = TILE_USED_BLOCK;
            SpawnItem_star(itemX, itemY);
        }
        else if (currentMap[top][blockX] == TILE_BOX_FLOWER)
        {
            PlaySoundBuffer(powerup_appears_Sound);
            if (currentMap[top - 1][blockX] == TILE_COIN)
            {
                PlaySoundBuffer(coin_Sound);
                currentMap[top - 1][blockX] = 0;
                g_player.mario.coin++;
            }
            int itemX = blockX * TILE_SIZE;
            int itemY = top * TILE_SIZE;
            currentMap[top][blockX] = TILE_USED_BLOCK;
            SpawnItem_flower(itemX, itemY);
        }
        else if (currentMap[top][blockX] == TILE_BOX_TINO)
        {
            PlaySoundBuffer(powerup_appears_Sound);
            if (currentMap[top - 1][blockX] == TILE_COIN)
            {
                PlaySoundBuffer(coin_Sound);
                currentMap[top - 1][blockX] = 0;
                g_player.mario.coin++;
            }
            int itemX = blockX * TILE_SIZE;
            int itemY = top * TILE_SIZE;
            currentMap[top][blockX] = TILE_USED_BLOCK;
            SpawnItem_tino(itemX, itemY);
        }
        else if (currentMap[top][blockX] == TILE_BOX_UPMUSH)
        {
            PlaySoundBuffer(powerup_appears_Sound);
            if (currentMap[top - 1][blockX] == TILE_COIN)
            {
                PlaySoundBuffer(coin_Sound);
                currentMap[top - 1][blockX] = 0;
                g_player.mario.coin++;
            }
            int itemX = blockX * TILE_SIZE;
            int itemY = top * TILE_SIZE;
            currentMap[top][blockX] = TILE_USED_BLOCK;
            SpawnItem_up_mushroom(itemX, itemY);
        }
        else if (currentMap[top][blockX] == TILE_BOX_COIN)
        {
            PlaySoundBuffer(coin_Sound);
            if (currentMap[top - 1][blockX] == TILE_COIN)
            {
                PlaySoundBuffer(coin_Sound);
                currentMap[top - 1][blockX] = 0;
                g_player.mario.coin++;
            }
            int itemX = blockX * TILE_SIZE;
            int itemY = top * TILE_SIZE;
            currentMap[top][blockX] = TILE_USED_BLOCK;
            g_player.mario.coin++;
        }
        else if (currentMap[top][blockX] == TILE_BOX_STAR_HIDDEN)
        {
            PlaySoundBuffer(powerup_appears_Sound);
            if (currentMap[top - 1][blockX] == TILE_COIN)
            {
                PlaySoundBuffer(coin_Sound);
                currentMap[top - 1][blockX] = 0;
                g_player.mario.coin++;
            }
            int itemX = blockX * TILE_SIZE;
            int itemY = top * TILE_SIZE;
            currentMap[top][blockX] = TILE_USED_BLOCK;
            SpawnItem_star(itemX, itemY);
        }

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

    if (g_player.mario.y > 800)
    {
        dead();
    }
}
void UpdateDeadMotion()
{
    g_player.mario.y += g_player.mario.vy;
    g_player.mario.vy += 1;
    if (g_player.mario.vy > 15) g_player.mario.vy = 15;
}


void dead()
{
    g_pBGMBuffer->Stop();
    PlaySoundBuffer(die_Sound);
    g_game.gameState = GAME_OVER;
    g_player.mario.isDead = true;
    g_player.mario.isBig = false;
    g_player.mario.flower = false;
    g_player.mario.tino = false;
    g_game.deadStartTime = GetTickCount();

    g_player.mario.life--;
    g_game.stage_time = 400;

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
    g_player.mario = {};
    g_player.mario.x = 100;
    g_player.mario.y = 300;
    g_player.mario.life = life;
    g_player.mario.coin = coin;
    g_player.mario.width = 40;
    g_player.mario.height = 40;
    g_player.mario.direction = 1;
}

