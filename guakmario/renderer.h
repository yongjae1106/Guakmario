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
void Draw_information();
