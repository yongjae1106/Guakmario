 #include "item.h"
#include "player.h"
#include "game.h"
#include "sound.h"

void TransformToFlower() { g_player.mario.form = FORM_FLOWER; g_player.mario.height = 80; }
void TransformToTino()   { g_player.mario.form = FORM_TINO;   g_player.mario.height = 80; }
void TransformToBig()    { g_player.mario.form = FORM_BIG;    g_player.mario.height = 80; }
void TransformToSmall()  { g_player.mario.form = FORM_SMALL;  g_player.mario.height = 40; }

void UpdateGodMode(DWORD _godstart)
{
    DWORD now = GetTickCount();
    if (now - _godstart >= 1000)
    {
        g_player.mario.god = false;
    }
}
void UpdateStarMode(DWORD _starstart)
{
    DWORD now = GetTickCount();
    if (now - _starstart >= 10000)
    {
        SetStage_BGM();
        g_player.mario.star = false;
    }
}
