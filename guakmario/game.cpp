#include <tchar.h>
#include "data.h"
#include "map.h"
#include "monster.h"
#include "item.h"
#include "sound.h"
#include "player.h"
#include "collision.h"
#include "game.h"
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

using namespace Gdiplus;

static void handle_transform(bool& flag, void (*primary_fn)(), void (*fallback_fn)())
{
    g_player.mario.god = true;
    DWORD now = GetTickCount();
    if (now - g_game.transformStartTime >= 700)
    {
        if (!flag)
            primary_fn();
        else if (g_player.mario.god)
        {
            fallback_fn();
            g_game.godstart = GetTickCount();
        }
        g_game.gameState = GAME_RUNNING;  // 다시 정상 진행
    }
}

void UpdateGame()
{
    // 변신 모션 (mushroom / flower / tino)
    if (g_game.gameState == GAME_TRANSFORMING)
    {
        handle_transform(g_player.mario.isBig,   TransformToBig,  TransformToSmall);
        return; // 게임 상태 업데이트 생략해서 "멈춘 듯한" 연출
    }
    else if (g_game.gameState == GAME_FLOWER_TRANS)
    {
        handle_transform(g_player.mario.flower,  TransformToFlower, TransformToBig);
        return;
    }
    else if (g_game.gameState == GAME_TINO_TRANS)
    {
        handle_transform(g_player.mario.tino,    TransformToTino,   TransformToBig);
        return;
    }
    // 승리 모션
    else if (g_game.gameState == GAME_VICTORY)
    {
        DWORD now = GetTickCount();
        g_player.mario.vx = 1;
        g_player.mario.x += g_player.mario.vx;
        if (now - g_game.victoryStart >= 5000)
        {
            g_game.stage++;
            LoadStage();
            g_game.gameState = GAME_RUNNING;
        }
    }
    // 최종 승리 모션
    else if (g_game.gameState == GAME_CLEAR)
    {
        if (GetTickCount() - g_game.clearStart >= 0 && GetTickCount() - g_game.clearStart <= 5000)
        {
            g_player.mario.vx = 1;
            g_player.mario.x += g_player.mario.vx;
        }
        else
        {
            g_game.gameClearText = true;
            g_player.mario.isWalking = false;
            g_player.mario.vx = 0;
        }
        if (GetTickCount() - g_game.clearStart >= 10000)
        {
            g_game.stage = 1;
            g_game.gameClearText = false;
            g_game.gameStart = false;
            ResetMario(5, 0);
            g_game.gameState = GAME_RUNNING;
        }
    }
    // 시망모션
    else if (g_game.gameState == GAME_OVER)
    {
        DWORD now = GetTickCount();
        if (g_player.mario.isDead)
        {
            static bool motion1;
            if (motion1) UpdateDeadMotion();
            if (now - g_game.deadStartTime >= 500 && !motion1)
            {
                g_player.mario.vy -= 14;
                motion1 = true;
            }
            if (now - g_game.deadStartTime >= 2000)
            {
                if (g_player.mario.life <= 0)
                {
                    g_game.stage = 1;
                    g_game.gameClearText = false;
                    g_game.gameStart = false;
                    ResetMario(5, 0);
                    g_game.gameState = GAME_RUNNING;
                    return;
                }
                g_player.mario.isDead = false;
                motion1 = false;
            }
            return;
        }
        if (now - g_game.deadStartTime >= 3000)
        {
            g_game.gameState = GAME_RUNNING;  // 다시 정상 진행
            resurrection();
        }

        return;
    }

    // 무적처리
    if (g_player.mario.god)
    {
        UpdateGodMode(g_game.godstart);
    }
    // 스타 무적처리
    else if (g_player.mario.star)
    {
        UpdateStarMode(g_game.starStartTime);
    }

    if (g_player.mario.coin > 99)
    {
        PlaySoundBuffer(up_Sound);
        g_player.mario.life++;
        g_player.mario.coin = 0;
    }

    UpdateMario_motion();
    UpdatePlayer();
    UpdateItems();
    UpdateItems_up_mushroom();
    UpdateItems_star();
    UpdateItems_flower();
    UpdateItems_tino();
    UpdateMonsters();
    UpdateTurtles();
    UpdateAngelTurtles();
    UpdateBowser();
    UpdateFireballs();
    UpdateFireTraps();
    UpdateShot_fireball();
    UpdateShot_tinofire();
    UpdateShot_tinofire_effect();

    CheckCollision_mushroom();
    CheckCollision_up_mushroom();
    CheckCollision_star();
    CheckCollision_flower();
    CheckCollision_tino();

    CheckCoinCollision();
    CheckMarioMonsterCollision();
    CheckMarioTurtleCollision();
    CheckMarioAngelTurtleCollision();

    CheckMarioHazardCollision();

    CheckMarioBowserCollision();
    CheckMarioFireballCollision();
    CheckCollision_fireball();
    CheckCollision_tinofire();
    CheckFlagCollision();
    if(g_game.stage == 3) CheckClearCollision();
}

void TickTimer()
{
    g_game.stage_time--;
    if (g_game.stage_time == 0)
    {
        dead();
    }
}

void ResetMonsters()
{
    for (int i = 0; i < MAX_MONSTERS; i++)
    {
        g_monsters.monsters[i].active = false;
    }
    for (int i = 0; i < MAX_TURTLES; i++)
    {
        g_monsters.turtles[i].active = false;
        g_monsters.angelTurtles[i].isAlive = false;
    }
}
void ResetItems()
{
    for (int i = 0; i < MAX_ITEMS; i++) // 아이템 초기화
    {
        mushroom[i].active = false;
        mushroom[i].motion = false;

        up_mushroom[i].active = false;
        up_mushroom[i].motion = false;

        flower[i].active = false;
        flower[i].motion = false;

        tino[i].active = false;
        tino[i].motion = false;

        star[i].active = false;
        star[i].motion = false;
    }
    for (int i = 0; i < MAX_SHOT; i++)
    {
        fireball[i].active = false;
        fireball[i].fade = false;
        tinofire[i].active = false;
        tinofire_effect[i].active = false;
        tinofire[i].fade = false;
    }
}
void LoadStage()
{
    SetStage_BGM();
    ResetItems();
    ResetTraps();
    g_player.mario.god = false;
    g_player.mario.star = false;

    if (g_game.stage == 1)
    {
        g_game.cameraX = 0;
        InitMap();           // 맵 초기화
        currentMap = map1;

        InitMonsters();     //몬스터 초기화
        InitTurtles();

        g_player.mario.x = 100;
        g_player.mario.y = 300;
        g_game.stage_time = 400;

    }
    else if (g_game.stage == 2)
    {
        ResetMonsters();
        g_game.cameraX = 0;
        InitMap2();  //맵 초기화
        currentMap = map2;

        InitMonsters2();     //몬스터 초기화
        InitTurtles2();
        InitAngelTurtles();

        g_player.mario.x = 100;
        g_player.mario.y = 300;
        g_game.stage_time = 400;
    }
    else if (g_game.stage == 3)
    {
        ResetMonsters();
        g_game.cameraX = 0;
        InitMap3();           // 맵 초기화
        currentMap = map3;

        InitFireTraps();
        InitMonsters3();     //몬스터 초기화
        InitTurtles3();
        InitBowser();

        g_player.mario.x = 100;
        g_player.mario.y = 300;
        g_game.stage_time = 400;
    }
}
