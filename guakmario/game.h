#pragma once
#include "data.h"

enum GameState
{
    GAME_RUNNING,
    GAME_TRANSFORMING,
    GAME_FLOWER_TRANS,
    GAME_TINO_TRANS,
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
    bool  gameStart          = false;
    int   title_select       = 0;
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
