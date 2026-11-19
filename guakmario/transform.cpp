 #include "item.h"
#include "player.h"
#include "game.h"
#include "sound.h"
#include "config.h"

void TransformToFlower() { g_player.mario.form = FORM_FLOWER; g_player.mario.height = 80; }
void TransformToTino()   { g_player.mario.form = FORM_TINO;   g_player.mario.height = 80; }
void TransformToBig()    { g_player.mario.form = FORM_BIG;    g_player.mario.height = 80; }
void TransformToSmall()  { g_player.mario.form = FORM_SMALL;  g_player.mario.height = 40; }

void StartPlayerTransformation(MarioForm target)
{
    Player& player = g_player.mario;
    if (g_game.gameState != GAME_RUNNING || player.isDead || player.isTransforming || player.form == target)
        return;

    const DWORD now = GetTickCount();
    if (g_game.isMulti)
    {
        player.transformTarget = target;
        player.transformStartTime = now;
        player.isTransforming = true;
        return;
    }

    if (player.form == FORM_SMALL)
        player.y -= TILE_SIZE + 1;
    else if (target == FORM_SMALL)
        player.y += TILE_SIZE + 1;

    // 기존 싱글 피격 연출은 현재 폼을 사용해 하위 폼으로 전환한다.
    const bool powerDown = target == FORM_SMALL || (target == FORM_BIG && player.form != FORM_SMALL);
    g_game.transformTarget = powerDown ? player.form : target;
    g_game.transformStartTime = now;
    g_game.gameState = GAME_TRANSFORMING;
}

void UpdatePlayerTransformation(Player& player, DWORD now)
{
    if (!player.isTransforming || now - player.transformStartTime < TRANSFORM_DURATION_MS) return;

    const MarioForm target = player.transformTarget;
    const int height = target == FORM_SMALL ? TILE_SIZE : 2 * TILE_SIZE;
    if ((target == FORM_SMALL || (target == FORM_BIG && player.form != FORM_SMALL)) && !player.god)
    {
        player.god = true;
        player.godStartTime = now;
    }
    // 변신 전 발 위치에 맞춰 새 충돌 상자의 높이를 적용한다.
    player.y += player.height - height;
    player.height = height;
    player.form = target;
    player.isTransforming = false;
}

void UpdateGodMode(DWORD _godstart)
{
    DWORD now = GetTickCount();
    if (now - _godstart >= GOD_MODE_DURATION_MS)
    {
        g_player.mario.god = false;
    }
}
void UpdateStarMode(DWORD _starstart)
{
    DWORD now = GetTickCount();
    if (now - _starstart >= STAR_MODE_DURATION_MS)
    {
        SetStage_BGM();
        g_player.mario.star = false;
    }
}
