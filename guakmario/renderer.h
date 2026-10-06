#pragma once
#include "data.h"

extern HDC g_memDC;
extern HBITMAP g_memBitmap;
extern HBITMAP g_oldBitmap;

extern HBRUSH sky_brush;
extern HBRUSH black_brush;

extern TCHAR life_print[32], life_print2[32], life_print3[32];
extern TCHAR time_print[32], time_print2[32];
extern TCHAR coin_print[32];
extern TCHAR stage_print[32], stage_print2[32];

void Draw();
void Draw_spawn_item();
void Draw_map();
void Draw_castle_blank();
void Draw_mario();
void Draw_item();
void Draw_information();
void Draw_background();
void Draw_fireball();
void Draw_Monsters();
void Draw_Turtles();
void Draw_Angel_Turtles();
void Draw_Bowser();
void Draw_Fireballs();
void DrawFireTraps();
