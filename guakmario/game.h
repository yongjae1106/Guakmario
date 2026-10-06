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
extern GameState gameState;
extern DWORD transformStartTime;
extern DWORD starStartTime;
extern DWORD deadStartTime;
extern DWORD godstart;
extern DWORD victoryStart;
extern DWORD clearStart;

extern bool gamestart;
extern int title_select;

extern double cameraX;
extern int worldMarioX;
extern int stage;
extern int stage_time;
extern int frame_motion;
extern int frame_motion_star;
extern bool gameclear_text;

void UpdateGame();
void timegoes();
void stage_load();
void monster_reset();
void item_reset();
