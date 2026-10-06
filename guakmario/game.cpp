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
    mario.god = true;
    DWORD now = GetTickCount();
    if (now - g_game.transformStartTime >= 700)
    {
        if (!flag)
            primary_fn();
        else if (mario.god)
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
        handle_transform(mario.isBig,   transform_bigmario,  transform_smallmario);
        return; // 게임 상태 업데이트 생략해서 "멈춘 듯한" 연출
    }
    else if (g_game.gameState == GAME_FLOWER_TRANS)
    {
        handle_transform(mario.flower,  transform_to_flower, transform_bigmario);
        return;
    }
    else if (g_game.gameState == GAME_TINO_TRANS)
    {
        handle_transform(mario.tino,    transform_to_tino,   transform_bigmario);
        return;
    }
    // 승리 모션
    else if (g_game.gameState == GAME_VICTORY)
    {
        DWORD now = GetTickCount();
        mario.vx = 1;
        mario.x += mario.vx;
        if (now - g_game.victoryStart >= 5000)
        {
            g_game.stage++;
            stage_load();
            g_game.gameState = GAME_RUNNING;
        }
    }
    // 최종 승리 모션
    else if (g_game.gameState == GAME_CLEAR)
    {
        if (GetTickCount() - g_game.clearStart >= 0 && GetTickCount() - g_game.clearStart <= 5000)
        {
            mario.vx = 1;
            mario.x += mario.vx;
        }
        else
        {
            g_game.gameclear_text = true;
            mario.isWalking = false;
            mario.vx = 0;
        }
        if (GetTickCount() - g_game.clearStart >= 10000)
        {
            g_game.stage = 1;
            g_game.gameclear_text = false;
            g_game.gamestart = false;
            ResetMario(5, 0);
            g_game.gameState = GAME_RUNNING;
        }
    }
    // 시망모션
    else if (g_game.gameState == GAME_OVER)
    {
        DWORD now = GetTickCount();
        if (mario.isDead)
        {
            static bool motion1;
            if (motion1) UpdateDeadMotion();
            if (now - g_game.deadStartTime >= 500 && !motion1)
            {
                mario.vy -= 14;
                motion1 = true;
            }
            if (now - g_game.deadStartTime >= 2000)
            {
                if (mario.life <= 0)
                {
                    g_game.stage = 1;
                    g_game.gameclear_text = false;
                    g_game.gamestart = false;
                    ResetMario(5, 0);
                    g_game.gameState = GAME_RUNNING;
                    return;
                }
                mario.isDead = false;
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
    if (mario.god)
    {
        god_mario(g_game.godstart);
    }
    // 스타 무적처리
    else if (mario.star)
    {
        star_mario(g_game.starStartTime);
    }

    if (mario.coin > 99)
    {
        PlaySoundBuffer(up_Sound);
        mario.life++;
        mario.coin = 0;
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

    checkcollision_coin();
    CheckMarioMonsterCollision();
    CheckMarioTurtleCollision();
    CheckMarioAngelTurtleCollision();

    CheckMarioHazardCollision();

    CheckMarioBowserCollision();
    CheckMarioFireballCollision();
    CheckCollision_fireball();
    CheckCollision_tinofire();
    checkcollision_flag();
    if(g_game.stage == 3) checkcollision_clear();
}

void timegoes()
{
    g_game.stage_time--;
    if (g_game.stage_time == 0)
    {
        dead();
    }
}

void monster_reset()
{
    for (int i = 0; i < MAX_MONSTERS; i++)
    {
        monsters[i].active = false;
    }
    for (int i = 0; i < MAX_TURTLES; i++)
    {
        turtles[i].active = false;
        angelTurtles[i].isAlive = false;
    }
}
void item_reset()
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
void stage_load()
{
    SetStage_BGM();
    item_reset();
    trap_reset();
    mario.god = false;
    mario.star = false;

    if (g_game.stage == 1)
    {
        g_game.cameraX = 0;
        InitMap();           // 맵 초기화
        currentMap = map1;

        InitMonsters();     //몬스터 초기화
        InitTurtles();

        mario.x = 100;
        mario.y = 300;
        g_game.stage_time = 400;

    }
    else if (g_game.stage == 2)
    {
        monster_reset();
        g_game.cameraX = 0;
        InitMap2();  //맵 초기화
        currentMap = map2;

        InitMonsters2();     //몬스터 초기화
        InitTurtles2();
        InitAngelTurtles();

        mario.x = 100;
        mario.y = 300;
        g_game.stage_time = 400;
    }
    else if (g_game.stage == 3)
    {
        monster_reset();
        g_game.cameraX = 0;
        InitMap3();           // 맵 초기화
        currentMap = map3;

        InitFireTraps();
        InitMonsters3();     //몬스터 초기화
        InitTurtles3();
        InitBowser();

        mario.x = 100;
        mario.y = 300;
        g_game.stage_time = 400;
    }
}
