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
    if (keyState[VK_LEFT] && !keyState[VK_RIGHT])
    {
        if (mario.direction == 1)
        {
            mario_stop->RotateFlip(RotateNoneFlipX); // 좌우반전
            mario_walk_motion_1->RotateFlip(RotateNoneFlipX); // 좌우반전
            mario_walk_motion_2->RotateFlip(RotateNoneFlipX); // 좌우반전
            mario_walk_motion_3->RotateFlip(RotateNoneFlipX); // 좌우반전
            mario_jump->RotateFlip(RotateNoneFlipX); // 좌우반전
            big_mario_stop->RotateFlip(RotateNoneFlipX); // 좌우반전
            big_mario_walk_motion_1->RotateFlip(RotateNoneFlipX); // 좌우반전
            big_mario_walk_motion_2->RotateFlip(RotateNoneFlipX); // 좌우반전
            big_mario_walk_motion_3->RotateFlip(RotateNoneFlipX); // 좌우반전
            big_mario_jump->RotateFlip(RotateNoneFlipX); // 좌우반전

            flower_mario_change->RotateFlip(RotateNoneFlipX); // 좌우반전
            flower_mario_stop->RotateFlip(RotateNoneFlipX); // 좌우반전
            flower_mario_walk_motion_1->RotateFlip(RotateNoneFlipX); // 좌우반전
            flower_mario_walk_motion_2->RotateFlip(RotateNoneFlipX); // 좌우반전
            flower_mario_walk_motion_3->RotateFlip(RotateNoneFlipX); // 좌우반전
            flower_mario_jump->RotateFlip(RotateNoneFlipX); // 좌우반전
            flower_mario_fire->RotateFlip(RotateNoneFlipX); // 좌우반전

            tino_mario_stop->RotateFlip(RotateNoneFlipX); // 좌우반전
            tino_mario_walk_motion_1->RotateFlip(RotateNoneFlipX); // 좌우반전
            tino_mario_walk_motion_2->RotateFlip(RotateNoneFlipX); // 좌우반전
            tino_mario_walk_motion_3->RotateFlip(RotateNoneFlipX); // 좌우반전
            tino_mario_jump->RotateFlip(RotateNoneFlipX); // 좌우반전
            tino_mario_attack_1->RotateFlip(RotateNoneFlipX); // 좌우반전
            tino_mario_attack_2->RotateFlip(RotateNoneFlipX); // 좌우반전
            tino_mario_attack_3->RotateFlip(RotateNoneFlipX); // 좌우반전
            tino_mario_attack_4->RotateFlip(RotateNoneFlipX); // 좌우반전
            tino_mario_attack_5->RotateFlip(RotateNoneFlipX); // 좌우반전
            tino_mario_attack_6->RotateFlip(RotateNoneFlipX); // 좌우반전

            star_mario_stop_1->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_mario_walk_motion_1_1->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_mario_walk_motion_2_1->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_mario_walk_motion_3_1->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_mario_jump_1->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_big_mario_stop_1->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_big_mario_walk_motion_1_1->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_big_mario_walk_motion_2_1->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_big_mario_walk_motion_3_1->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_big_mario_jump_1->RotateFlip(RotateNoneFlipX); // 좌우반전

            star_mario_stop_2->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_mario_walk_motion_1_2->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_mario_walk_motion_2_2->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_mario_walk_motion_3_2->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_mario_jump_2->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_big_mario_stop_2->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_big_mario_walk_motion_1_2->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_big_mario_walk_motion_2_2->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_big_mario_walk_motion_3_2->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_big_mario_jump_2->RotateFlip(RotateNoneFlipX); // 좌우반전

            star_mario_stop_3->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_mario_walk_motion_1_3->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_mario_walk_motion_2_3->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_mario_walk_motion_3_3->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_mario_jump_3->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_big_mario_stop_3->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_big_mario_walk_motion_1_3->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_big_mario_walk_motion_2_3->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_big_mario_walk_motion_3_3->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_big_mario_jump_3->RotateFlip(RotateNoneFlipX); // 좌우반전
        }
        mario.vx = -5;
        mario.direction = 0;
    }
    else if (keyState[VK_RIGHT] && !keyState[VK_LEFT]) 
    {
        if (mario.direction == 0)
        {
            mario_stop->RotateFlip(RotateNoneFlipX); // 좌우반전
            mario_walk_motion_1->RotateFlip(RotateNoneFlipX); // 좌우반전
            mario_walk_motion_2->RotateFlip(RotateNoneFlipX); // 좌우반전
            mario_walk_motion_3->RotateFlip(RotateNoneFlipX); // 좌우반전
            mario_jump->RotateFlip(RotateNoneFlipX); // 좌우반전
            big_mario_stop->RotateFlip(RotateNoneFlipX); // 좌우반전
            big_mario_walk_motion_1->RotateFlip(RotateNoneFlipX); // 좌우반전
            big_mario_walk_motion_2->RotateFlip(RotateNoneFlipX); // 좌우반전
            big_mario_walk_motion_3->RotateFlip(RotateNoneFlipX); // 좌우반전
            big_mario_jump->RotateFlip(RotateNoneFlipX); // 좌우반전
            big_mario_change->RotateFlip(RotateNoneFlipX); // 좌우반전

            flower_mario_change->RotateFlip(RotateNoneFlipX); // 좌우반전
            flower_mario_stop->RotateFlip(RotateNoneFlipX); // 좌우반전
            flower_mario_walk_motion_1->RotateFlip(RotateNoneFlipX); // 좌우반전
            flower_mario_walk_motion_2->RotateFlip(RotateNoneFlipX); // 좌우반전
            flower_mario_walk_motion_3->RotateFlip(RotateNoneFlipX); // 좌우반전
            flower_mario_jump->RotateFlip(RotateNoneFlipX); // 좌우반전
            flower_mario_fire->RotateFlip(RotateNoneFlipX); // 좌우반전

            tino_mario_stop->RotateFlip(RotateNoneFlipX); // 좌우반전
            tino_mario_walk_motion_1->RotateFlip(RotateNoneFlipX); // 좌우반전
            tino_mario_walk_motion_2->RotateFlip(RotateNoneFlipX); // 좌우반전
            tino_mario_walk_motion_3->RotateFlip(RotateNoneFlipX); // 좌우반전
            tino_mario_jump->RotateFlip(RotateNoneFlipX); // 좌우반전
            tino_mario_attack_1->RotateFlip(RotateNoneFlipX); // 좌우반전
            tino_mario_attack_2->RotateFlip(RotateNoneFlipX); // 좌우반전
            tino_mario_attack_3->RotateFlip(RotateNoneFlipX); // 좌우반전
            tino_mario_attack_4->RotateFlip(RotateNoneFlipX); // 좌우반전
            tino_mario_attack_5->RotateFlip(RotateNoneFlipX); // 좌우반전
            tino_mario_attack_6->RotateFlip(RotateNoneFlipX); // 좌우반전

            star_mario_stop_1->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_mario_walk_motion_1_1->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_mario_walk_motion_2_1->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_mario_walk_motion_3_1->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_mario_jump_1->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_big_mario_stop_1->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_big_mario_walk_motion_1_1->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_big_mario_walk_motion_2_1->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_big_mario_walk_motion_3_1->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_big_mario_jump_1->RotateFlip(RotateNoneFlipX); // 좌우반전

            star_mario_stop_2->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_mario_walk_motion_1_2->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_mario_walk_motion_2_2->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_mario_walk_motion_3_2->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_mario_jump_2->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_big_mario_stop_2->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_big_mario_walk_motion_1_2->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_big_mario_walk_motion_2_2->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_big_mario_walk_motion_3_2->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_big_mario_jump_2->RotateFlip(RotateNoneFlipX); // 좌우반전

            star_mario_stop_3->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_mario_walk_motion_1_3->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_mario_walk_motion_2_3->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_mario_walk_motion_3_3->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_mario_jump_3->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_big_mario_stop_3->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_big_mario_walk_motion_1_3->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_big_mario_walk_motion_2_3->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_big_mario_walk_motion_3_3->RotateFlip(RotateNoneFlipX); // 좌우반전
            star_big_mario_jump_3->RotateFlip(RotateNoneFlipX); // 좌우반전
        }
        mario.vx = 5;
        mario.direction = 1;
    }
    else if (keyState[VK_LEFT] && keyState[VK_RIGHT])
    {
        // 둘 다 눌린 경우 마지막 방향 유지
        mario.vx = (mario.direction == 0 ? -5 : 5);
    }
    else 
    {
        mario.isWalking = false;
        mario.vx = 0;
        mario.walk_motion = 0;
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
    tick_motion(mario.fire_motion,      mario.motion_timer);
    tick_motion(mario.tino_motion,      mario.motion_timer);
    tick_motion(mario.tino_fire_motion, mario.motion_timer);
}

void UpdatePlayer()
{
    if(gameState != GAME_VICTORY && gameState != GAME_CLEAR)
    {
        movePlayer();
    }
    // 수평 이동
    mario.x += mario.vx;

    // 좌우 충돌 처리
    int left = (mario.x + cameraX) / TILE_SIZE;
    int right = (mario.x + mario.width - 1 + cameraX) / TILE_SIZE;
    int top = mario.y / TILE_SIZE;
    int bottom = (mario.y + mario.height - 1) / TILE_SIZE;
    int middle = (mario.y + mario.height / 2 - 1) / TILE_SIZE;

    // 블럭 왼쪽 벽 충돌
    if (!(mario.y < 0) && mario.vx < 0 &&
        (isSolidTile(currentMap[top][left]) || isSolidTile(currentMap[bottom][left]) || isSolidTile(currentMap[middle][left])))
    {
        mario.x = left * TILE_SIZE - cameraX + TILE_SIZE;
    }

    // 블럭 오른쪽 벽 충돌
    if (!(mario.y < 0) && mario.vx > 0 &&
        (isSolidTile(currentMap[top][right]) || isSolidTile(currentMap[bottom][right]) || isSolidTile(currentMap[middle][right])))
    {
        mario.x = right * TILE_SIZE - cameraX - mario.width;
    }

    // 중력 적용
    mario.isflying = true;
    mario.vy += 1;
    if (mario.vy > 15) mario.vy = 15;
    mario.y += mario.vy;

    // y축 충돌 다시 계산
    left = (mario.x + cameraX) / TILE_SIZE;
    right = (mario.x + mario.width - 1 + cameraX) / TILE_SIZE;
    top = mario.y / TILE_SIZE;
    bottom = (mario.y + mario.height - 1) / TILE_SIZE;

    // 아래 충돌
    if (!(mario.y < 0) && mario.vy > 0 && (isSolidTile(currentMap[bottom][left]) || isSolidTile(currentMap[bottom][right])))
    {
        if (currentMap[bottom][left] == TILE_LAVA_HEAD || currentMap[bottom][right] == TILE_LAVA_HEAD ||
            currentMap[bottom][left] == TILE_LAVA_BODY || currentMap[bottom][right] == TILE_LAVA_BODY)
        {
            dead();
        }
        int centerX = mario.x + TILE_SIZE / 2 + cameraX;
        int blockX = centerX / TILE_SIZE;
        
        mario.y = bottom * TILE_SIZE - mario.height;
        mario.vy = 0;
        mario.isJumping = false;
        mario.isflying = false;
    }
    // 위 충돌
    else if (!(mario.y < 0) && mario.vy < 0 && (isSolidTile(currentMap[top][left]) || isSolidTile(currentMap[top][right])))
    {
        // 마리오가 정통으로 친 블럭 찾기
        int centerX = mario.x + TILE_SIZE / 2 + cameraX;
        int blockX = centerX / TILE_SIZE;
        
        mario.y = (top + 1) * TILE_SIZE;
        mario.vy = 0;

        // 아이템 블럭 머리로 치기
        if (currentMap[top][blockX] == TILE_MYSTERY)
        {
            PlaySoundBuffer(powerup_appears_Sound);
            if (currentMap[top - 1][blockX] == TILE_COIN)
            {
                PlaySoundBuffer(coin_Sound);
                currentMap[top - 1][blockX] = 0;
                mario.coin++;
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
                mario.coin++;
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
                mario.coin++;
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
                mario.coin++;
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
                mario.coin++;
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
                mario.coin++;
            }
            int itemX = blockX * TILE_SIZE;
            int itemY = top * TILE_SIZE;
            currentMap[top][blockX] = TILE_USED_BLOCK;
            mario.coin++;
        }
        else if (currentMap[top][blockX] == TILE_BOX_STAR_HIDDEN)
        {
            PlaySoundBuffer(powerup_appears_Sound);
            if (currentMap[top - 1][blockX] == TILE_COIN)
            {
                PlaySoundBuffer(coin_Sound);
                currentMap[top - 1][blockX] = 0;
                mario.coin++;
            }
            int itemX = blockX * TILE_SIZE;
            int itemY = top * TILE_SIZE;
            currentMap[top][blockX] = TILE_USED_BLOCK;
            SpawnItem_star(itemX, itemY);
        }

    }


    // 마리오 왼쪽 벽 충돌
    if (mario.x < 0)
    {
        mario.x = 0;
    }
    // 카메라 이동
    if (mario.x > SCREEN_WIDTH / 2)
    {
        cameraX += mario.vx;
        mario.x = SCREEN_WIDTH / 2;
    }
    if (mario.x < SCREEN_WIDTH / 2 && cameraX > 0)
    {
        cameraX += mario.vx;
        mario.x = SCREEN_WIDTH / 2;
    }

    if (cameraX < 0) cameraX = 0;
    if (cameraX > MAP_WIDTH * TILE_SIZE - SCREEN_WIDTH)
        cameraX = MAP_WIDTH * TILE_SIZE - SCREEN_WIDTH;

    if (mario.y > 800)
    {
        dead();
    }
}
void UpdateDeadMotion()
{
    mario.y += mario.vy;
    mario.vy += 1;
    if (mario.vy > 15) mario.vy = 15;
}


void dead()
{
    g_pBGMBuffer->Stop();
    PlaySoundBuffer(die_Sound);
    gameState = GAME_OVER;
    mario.isDead = true;
    mario.isBig = false;
    mario.flower = false;
    mario.tino = false;
    deadStartTime = GetTickCount();

    mario.life--;
    stage_time = 400;

}
void resurrection()
{
    if (mario.direction == 0)
    {
        mario_stop->RotateFlip(RotateNoneFlipX); // 좌우반전
        mario_walk_motion_1->RotateFlip(RotateNoneFlipX); // 좌우반전
        mario_walk_motion_2->RotateFlip(RotateNoneFlipX); // 좌우반전
        mario_walk_motion_3->RotateFlip(RotateNoneFlipX); // 좌우반전
        mario_jump->RotateFlip(RotateNoneFlipX); // 좌우반전

        big_mario_stop->RotateFlip(RotateNoneFlipX); // 좌우반전
        big_mario_walk_motion_1->RotateFlip(RotateNoneFlipX); // 좌우반전
        big_mario_walk_motion_2->RotateFlip(RotateNoneFlipX); // 좌우반전
        big_mario_walk_motion_3->RotateFlip(RotateNoneFlipX); // 좌우반전
        big_mario_jump->RotateFlip(RotateNoneFlipX); // 좌우반전
    }
    item_reset();
    monster_reset();
    stage_load();
    cameraX = 0;
    gameState = GAME_RUNNING;
    ResetMario(mario.life, mario.coin);
}

void ResetMario(int life, int coin)
{
    mario = {};
    mario.x = 100;
    mario.y = 300;
    mario.life = life;
    mario.coin = coin;
    mario.width = 40;
    mario.height = 40;
    mario.direction = 1;
}

