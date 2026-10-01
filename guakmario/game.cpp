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

void UpdateGame()
{
    // mushroom 변신 모션
    if (gameState == GAME_TRANSFORMING) 
    {
        mario.god = true;
        DWORD now = GetTickCount();
        if (now - transformStartTime >= 700) 
        {
            if (!mario.isBig)
            {
                transform_bigmario();
            }
            else if (mario.god)
            {
                transform_smallmario();
                godstart = GetTickCount();
            }
            gameState = GAME_RUNNING;  // 다시 정상 진행
        }

        return; // 게임 상태 업데이트 생략해서 "멈춘 듯한" 연출
    }
    // flower 변신 모션
    else if (gameState == GAME_FLOWER_TRANS)
    {
        mario.god = true;
        DWORD now = GetTickCount();
        if (now - transformStartTime >= 700)
        {
            if (!mario.flower)
            {
                transform_to_flower();
            }
            else if (mario.god)
            {
                transform_bigmario();
                godstart = GetTickCount();
            }
            gameState = GAME_RUNNING;  // 다시 정상 진행
        }

        return; // 게임 상태 업데이트 생략해서 "멈춘 듯한" 연출
    }
    // tino 변신 모션
    else if (gameState == GAME_TINO_TRANS)
    {
        mario.god = true;
        DWORD now = GetTickCount();
        if (now - transformStartTime >= 700)
        {
            if (!mario.tino)
            {
                transform_to_tino();
            }
            else if (mario.god)
            {
                transform_bigmario();
                godstart = GetTickCount();
            }
            gameState = GAME_RUNNING;  // 다시 정상 진행
        }

        return; // 게임 상태 업데이트 생략해서 "멈춘 듯한" 연출
    }
    // 승리 모션
    else if (gameState == GAME_VICTORY)
    {
        DWORD now = GetTickCount();
        mario.vx = 1;
        mario.x += mario.vx;
        if (now - victoryStart >= 5000)
        {
            stage++;
            stage_load();
            gameState = GAME_RUNNING;
        }
    }
    // 최종 승리 모션
    else if (gameState == GAME_CLEAR)
    {
        if (GetTickCount() - clearStart >= 0 && GetTickCount() - clearStart <= 5000)
        {
            mario.vx = 1;
            mario.x += mario.vx;
        }
        else
        {
            gameclear_text = true;
            mario.isWalking = false;
            mario.vx = 0;
        }
        if (GetTickCount() - clearStart >= 10000)
        {
            stage = 1;
            gameclear_text = false;
            gamestart = false;
            mario = { 100, 300, 0, 0, 5, 0, 40, 40, 1, 0, 0, 0, 0, false, false, false, false, false, false, false, false, false, false, false, false, false, false };
            gameState = GAME_RUNNING;
        }
    }
    // 시망모션
    else if (gameState == GAME_OVER)
    {
        DWORD now = GetTickCount();
        if (mario.isDead)
        {
            static bool motion1;
            if (motion1) UpdateDeadMotion();
            if (now - deadStartTime >= 500 && !motion1)
            {
                mario.vy -= 14;
                motion1 = true;
            }
            if (now - deadStartTime >= 2000)
            {
                if (mario.life <= 0)
                {
                    stage = 1;
                    gameclear_text = false;
                    gamestart = false;
                    mario = { 100, 300, 0, 0, 5, 0, 40, 40, 1, 0, 0, 0, 0, false, false, false, false, false, false, false, false, false, false, false, false, false, false };
                    gameState = GAME_RUNNING;
                    return;
                }
                mario.isDead = false;
                motion1 = false;
            }
            return;
        }
        if (now - deadStartTime >= 3000)
        {
            gameState = GAME_RUNNING;  // 다시 정상 진행
            resurrection();
        }

        return;
    }

    // 무적처리
    if (mario.god)
    {
        god_mario(godstart);
    }
    // 스타 무적처리
    else if (mario.star)
    {
        star_mario(starStartTime);
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
    if(stage == 3) checkcollision_clear();
}

void timegoes()
{
    stage_time--;
    if (stage_time == 0)
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

    if (stage == 1)
    {
        cameraX = 0;
        InitMap();           // 맵 초기화
        currentMap = map1;

        InitMonsters();     //몬스터 초기화
        InitTurtles();

        mario.x = 100;
        mario.y = 300;
        stage_time = 400;

    }
    else if (stage == 2)
    {
        monster_reset();
        cameraX = 0;
        InitMap2();  //맵 초기화
        currentMap = map2;

        InitMonsters2();     //몬스터 초기화
        InitTurtles2();
        InitAngelTurtles();

        mario.x = 100;
        mario.y = 300;
        stage_time = 400;
    }
    else if (stage == 3)
    {
        monster_reset();
        cameraX = 0;
        InitMap3();           // 맵 초기화
        currentMap = map3;

        InitFireTraps();
        InitMonsters3();     //몬스터 초기화
        InitTurtles3();
        InitBowser();

        mario.x = 100;
        mario.y = 300;
        stage_time = 400;
    }
}
