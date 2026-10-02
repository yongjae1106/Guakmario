#include "func.h"
#include "item.h"
#include "sound.h"

constexpr int STAGE3_CLEAR_COLUMN = 139;

void checkcollision_flag()
{
    int left = (mario.x + cameraX) / TILE_SIZE;
    int right = (mario.x + mario.width - 1 + cameraX) / TILE_SIZE;
    int top = mario.y / TILE_SIZE;
    int bottom = (mario.y + mario.height - 1) / TILE_SIZE;
    int middle = (mario.y + mario.height / 2 - 1) / TILE_SIZE;
    for (int i = 0; i < MAP_HEIGHT; i++)
    {
        for (int j = 0; j < MAP_WIDTH; j++)
        {
            int screenX = j * TILE_SIZE - cameraX;
            int screenY = i * TILE_SIZE;
            if ((currentMap[i][j] == TILE_FLAG || currentMap[i][j] == TILE_FLAG_TOP) && IsColliding_item(mario.x, mario.y, mario.width, mario.height, screenX, screenY, 10, 30))
            {
                g_pBGMBuffer->Stop();
                PlaySoundBuffer(stage_clear_Sound);
                gameState = GAME_VICTORY;
                victoryStart = GetTickCount();
            }

        }
    }
}
// 최종클리어 함수
void checkcollision_clear()
{
    for (int i = 0; i < MAP_HEIGHT; i++)
    {
        for (int j = 0; j < MAP_WIDTH; j++)
        {
            int screenX = j * TILE_SIZE - cameraX;
            int screenY = i * TILE_SIZE;
            if (j == STAGE3_CLEAR_COLUMN && IsColliding_item(mario.x, mario.y, mario.width, mario.height, screenX, screenY, 40, 40))
            {
                g_pBGMBuffer->Stop();
                PlaySoundBuffer(world_clear_Sound);
                gameState = GAME_CLEAR;
                clearStart = GetTickCount();
            }

        }
    }
}

// 코인먹기 함수
void checkcollision_coin()
{
    for (int i = 0; i < MAP_HEIGHT; i++)
    {
        for (int j = 0; j < MAP_WIDTH; j++)
        {
            int screenX = j * TILE_SIZE - cameraX;
            int screenY = i * TILE_SIZE;
            if (currentMap[i][j] == TILE_COIN && IsColliding_item(mario.x, mario.y, mario.width, mario.height, screenX, screenY, 30, 30))
            {
                PlaySoundBuffer(coin_Sound);
                currentMap[i][j] = 0;
                mario.coin++;
            }
        }
    }
}

// 해당 타일이 물리적으로 막히는 타일인지 판별
// TILE_EMPTY(구멍), TILE_COIN, TILE_FLAG, TILE_FLAG_TOP은 통과 가능
bool isSolidTile(int tile)
{
    return tile != TILE_EMPTY
        && tile != TILE_COIN
        && tile != TILE_FLAG
        && tile != TILE_FLAG_TOP;
}

// 충돌 확인 함수
bool IsColliding(int ax, int ay, int aw, int ah, int bx, int by, int bw, int bh)
{
    return (ax < bx + bw &&
        ax + aw > bx &&
        ay < by + bh &&
        ay + ah > by);
}
bool IsColliding_item(int ax, int ay, int aw, int ah, int bx, int by, int bw, int bh)
{
    return (ax < bx + bw &&
        ax + aw > bx &&
        ay < by + bh &&
        ay + ah > by);
}

