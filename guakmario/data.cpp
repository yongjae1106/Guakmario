#include "data.h"
#include "player.h"
#include "game.h"
#include "renderer.h"

Player mario = { 100, 300, 0, 0, 5, 0, 40, 40, 1, 0, 0, 0, 0, false, false, false, false, false, false, false, false, false, false, false, false, false, false};
// x, y, vx, xy, life, coin, width, height, direction, walk_motion, motion_timer, cooldown_z, cooldown_space, isJumping, isflying, isWalking, isDead, gameover, 
// isBig, god, star, flower, flower_motion, tino, tino_motion, tino_fire_motion, supergod

GameContext g_game;

bool keyState[256] = { false };

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