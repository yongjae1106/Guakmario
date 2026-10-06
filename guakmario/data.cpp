#include "data.h"
#include "player.h"
#include "game.h"
#include "renderer.h"

PlayerContext g_player;

GameContext g_game;

HDC g_memDC;				// ���� �޸� DC
HBITMAP g_memBitmap;		// ���� ��Ʈ��
HBITMAP g_oldBitmap;		// SelectObject ���� ��Ʈ��

// �귯��
HBRUSH sky_brush = CreateSolidBrush(RGB(148, 148, 255));
HBRUSH black_brush = CreateSolidBrush(RGB(0, 0, 0));

// ���� �ؽ�Ʈ
TCHAR life_print[32], life_print2[32], life_print3[32];
TCHAR time_print[32], time_print2[32];
TCHAR coin_print[32];
TCHAR stage_print[32], stage_print2[32];