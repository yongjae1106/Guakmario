#include <tchar.h>
#include "data.h"
#include "map.h"
#include "monster.h"
#include "item.h"
#include "sound.h"
#include "player.h"
#include "collision.h"
#include "game.h"
#include "config.h"
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

using namespace Gdiplus;

static void handle_transform(MarioForm targetForm, void (*primary_fn)(), void (*fallback_fn)())
{
    g_player.mario.god = true;
    DWORD now = GetTickCount();
    if (now - g_game.transformStartTime >= TRANSFORM_DURATION_MS)
    {
        if (g_player.mario.form != targetForm)
            primary_fn();
        else
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
        switch (g_game.transformTarget)
        {
        case FORM_BIG:    handle_transform(FORM_BIG,    TransformToBig,    TransformToSmall); break;
        case FORM_FLOWER: handle_transform(FORM_FLOWER, TransformToFlower, TransformToBig);   break;
        case FORM_TINO:   handle_transform(FORM_TINO,   TransformToTino,   TransformToBig);   break;
        default: break;
        }
        return; // 게임 상태 업데이트 생략해서 "멈춘 듯한" 연출
    }
    // 승리 모션
    else if (g_game.gameState == GAME_VICTORY)
    {
        DWORD now = GetTickCount();
        g_player.mario.vx = 1;
        g_player.mario.x += g_player.mario.vx;
        if (now - g_game.victoryStart >= VICTORY_NEXT_STAGE_MS)
        {
            g_game.stage++;
            LoadStage();
            g_game.gameState = GAME_RUNNING;
        }
    }
    // 최종 승리 모션
    else if (g_game.gameState == GAME_CLEAR)
    {
        if (GetTickCount() - g_game.clearStart >= 0 && GetTickCount() - g_game.clearStart <= CLEAR_WALK_MS)
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
        if (GetTickCount() - g_game.clearStart >= CLEAR_RESET_MS)
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
            if (now - g_game.deadStartTime >= DEAD_ANIMATION_MS)
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
        if (now - g_game.deadStartTime >= DEATH_REVIVE_DELAY_MS)
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

    if (g_player.mario.coin > COIN_1UP_THRESHOLD)
    {
        PlaySoundBuffer(up_Sound);
        g_player.mario.life++;
        g_player.mario.coin = 0;
    }

    UpdateMario_motion();
    if (g_player.mario.tino_motion) TinoAttack();  // 바이트 모션 동안 매 프레임 히트박스 적용
    UpdatePlayer();
    UpdateAllItems();
    UpdateAllMonsters();
    UpdateAllShots();

    CheckItemCollisions();
    CheckCoinCollision();
    CheckAllMonsterCollisions();
    CheckShotCollisions();
    CheckFlagCollision();
    if (g_game.stage == 3) CheckClearCollision();
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
struct StageData {
    const char*       bgm;
    void            (*initMap)();
    int           (*mapData)[MAP_WIDTH];
    void            (*extraInit)();
};

static void InitStage3Extras() { InitFireTraps(); InitBowser(); }

static const StageData stages[] = {
    { nullptr,                                     nullptr,  nullptr, nullptr           }, // [0] 미사용
    { "resource\\sound\\bgm\\GroundTheme.wav",     InitMap,  map1,   nullptr            },
    { "resource\\sound\\bgm\\GroundTheme.wav",     InitMap2, map2,   InitAngelTurtles   },
    { "resource\\sound\\bgm\\CastleTheme.wav",     InitMap3, map3,   InitStage3Extras   },
};

void LoadStage()
{
    int s = g_game.stage;
    const StageData& sd = stages[s];

    PlayBGM(sd.bgm);
    ResetItems();
    ResetTraps();
    g_player.mario.god = false;
    g_player.mario.star = false;

    ResetMonsters();
    g_game.cameraX = 0;
    sd.initMap();
    currentMap = sd.mapData;

    InitMonsters(s);
    InitTurtles(s);
    if (sd.extraInit) sd.extraInit();

    g_player.mario.x = 100;
    g_player.mario.y = 300;
    g_game.stage_time = STAGE_TIME_LIMIT;
}
