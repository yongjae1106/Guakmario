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
    bool  gamestart          = false;
    int   title_select       = 0;
    double cameraX           = 0;
    int   worldMarioX        = 0;
    int   stage              = 1;
    int   stage_time         = 400;
    int   frame_motion       = 0;
    int   frame_motion_star  = 0;
    bool  gameclear_text     = false;
};
extern GameContext g_game;

void UpdateGame();
void timegoes();
void stage_load();
void monster_reset();
void item_reset();
