#pragma once
#include "data.h"

enum GameState
{
    GAME_RUNNING,
    GAME_TRANSFORMING,  // 변신 중 (어떤 폼으로인지는 transformTarget 참조)
    GAME_VICTORY,
    GAME_CLEAR,
    GAME_OVER
};
struct GameContext {
    GameState gameState      = GAME_RUNNING;
    DWORD transformStartTime = 0;
    DWORD starStartTime      = 0;
    DWORD deadStartTime      = 0;
    DWORD godstart           = 0;
    DWORD victoryStart       = 0;
    DWORD clearStart         = 0;
    MarioForm transformTarget    = FORM_SMALL;   // GAME_TRANSFORMING 시 목표 폼
    bool  gameStart          = false;
    bool  showMultiSelect    = false;  // 방 만들기/방 참가 선택 화면
    bool  showMultiScreen    = false;  // 호스트 로비 화면
    int   title_select       = 0;
    int   multi_select       = 0;   // 선택 커서 (0=왼쪽, 1=오른쪽)
    double cameraX           = 0;
    int   worldMarioX        = 0;
    int   stage              = 1;
    int   stage_time         = 400;
    int   frameMotion       = 0;
    int   frameMotionStar  = 0;
    bool  gameClearText     = false;
};
extern GameContext g_game;

void UpdateGame();
void TickTimer();
void LoadStage();
void ResetMonsters();
void ResetItems();
