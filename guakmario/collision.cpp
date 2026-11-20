#include "map.h"
#include "player.h"
#include "game.h"
#include "collision.h"
#include "item.h"
#include "sound.h"

constexpr int STAGE3_CLEAR_COLUMN = 139;

bool IsPlayerAtStageGoal(int playerId)
{
    if (playerId < 0 || playerId >= 3) return false;

    const Player& player = g_allPlayers[playerId].mario;
    const int worldX = player.x + (playerId == g_localPlayerIdx ? static_cast<int>(g_game.cameraX) : 0);
    if (g_game.stage == 3)
    {
        return IsColliding(worldX, player.y, player.width, player.height,
            STAGE3_CLEAR_COLUMN * TILE_SIZE, 0, TILE_SIZE, MAP_HEIGHT * TILE_SIZE);
    }
    if (g_game.stage != 1 && g_game.stage != 2) return false;

    for (int row = 0; row < MAP_HEIGHT; ++row)
    {
        for (int column = 0; column < MAP_WIDTH; ++column)
        {
            const int tile = currentMap[row][column];
            if (tile != TILE_FLAG && tile != TILE_FLAG_TOP) continue;
            if (IsColliding(worldX, player.y, player.width, player.height,
                column * TILE_SIZE, row * TILE_SIZE, 10, 30)) return true;
        }
    }
    return false;
}

void CheckFlagCollision()
{
    int left = (g_player.mario.x + g_game.cameraX) / TILE_SIZE;
    int right = (g_player.mario.x + g_player.mario.width - 1 + g_game.cameraX) / TILE_SIZE;
    int top = g_player.mario.y / TILE_SIZE;
    int bottom = (g_player.mario.y + g_player.mario.height - 1) / TILE_SIZE;
    int middle = (g_player.mario.y + g_player.mario.height / 2 - 1) / TILE_SIZE;
    for (int i = 0; i < MAP_HEIGHT; i++)
    {
        for (int j = 0; j < MAP_WIDTH; j++)
        {
            int screenX = j * TILE_SIZE - g_game.cameraX;
            int screenY = i * TILE_SIZE;
            if ((currentMap[i][j] == TILE_FLAG || currentMap[i][j] == TILE_FLAG_TOP) && IsColliding(g_player.mario.x, g_player.mario.y, g_player.mario.width, g_player.mario.height, screenX, screenY, 10, 30))
            {
                g_pBGMBuffer->Stop();
                PlaySoundBuffer(stage_clear_Sound);
                g_game.gameState = GAME_VICTORY;
                g_game.victoryStart = GetTickCount();
            }

        }
    }
}
// 최종클리어 함수
void CheckClearCollision()
{
    for (int i = 0; i < MAP_HEIGHT; i++)
    {
        for (int j = 0; j < MAP_WIDTH; j++)
        {
            int screenX = j * TILE_SIZE - g_game.cameraX;
            int screenY = i * TILE_SIZE;
            if (j == STAGE3_CLEAR_COLUMN && IsColliding(g_player.mario.x, g_player.mario.y, g_player.mario.width, g_player.mario.height, screenX, screenY, 40, 40))
            {
                g_pBGMBuffer->Stop();
                PlaySoundBuffer(world_clear_Sound);
                g_game.gameState = GAME_CLEAR;
                g_game.clearStart = GetTickCount();
            }

        }
    }
}
// 코인먹기 함수
void CheckCoinCollision()
{
    for (int i = 0; i < MAP_HEIGHT; i++)
    {
        for (int j = 0; j < MAP_WIDTH; j++)
        {
            int screenX = j * TILE_SIZE - g_game.cameraX;
            int screenY = i * TILE_SIZE;
            if (currentMap[i][j] == TILE_COIN && IsColliding(g_player.mario.x, g_player.mario.y, g_player.mario.width, g_player.mario.height, screenX, screenY, 30, 30))
            {
                PlaySoundBuffer(coin_Sound);
                currentMap[i][j] = 0;
                g_player.mario.coin++;
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

