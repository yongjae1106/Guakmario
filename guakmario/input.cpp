#include "input.h"
#include "player.h"
#include "game.h"
#include "item.h"
#include "monster.h"
#include "sound.h"

void HandleGameKeyDown(WPARAM wParam)
{
    if (g_game.gameState == GAME_OVER || g_game.gameState == GAME_VICTORY || g_game.gameState == GAME_CLEAR) return;
    g_player.keyState[wParam] = true;
    g_player.mario.isWalking = true;
    switch (wParam)
    {
    case VK_UP:
    {
        if (!g_player.mario.isJumping && !g_player.mario.isFlying)
        {
            if (g_player.mario.form == FORM_BIG || g_player.mario.form == FORM_FLOWER)
                PlaySoundBuffer(jump_big_Sound);
            else
                PlaySoundBuffer(jump_small_Sound);
            g_player.mario.vy = -19;
            g_player.mario.isJumping = true;
        }
        break;
    }
    case VK_SPACE:
    {
        if (g_player.mario.form == FORM_TINO)
        {
            if (g_player.mario.tino_cooldown_space > 0) break;
            g_monsters.bowser.ignoreTinoBite = false;
            g_player.mario.tino_motion = true;
            g_player.mario.motion_timer = 30;
            TinoAttack();
            g_player.mario.god = true;
            if (!g_player.mario.supergod) g_game.godstart = GetTickCount();
            g_player.mario.tino_cooldown_space = 15;
        }
        break;
    }
    }
}

void HandleGameKeyUp(WPARAM wParam)
{
    g_player.keyState[wParam] = false;
    switch (wParam)
    {
    case VK_SPACE:
    {
        if (g_player.mario.form == FORM_FLOWER)
        {
            static bool flower_delay_bool = false;
            static DWORD flower_delay;
            if (!flower_delay_bool)
            {
                flower_delay = GetTickCount();
                flower_delay_bool = true;
            }
            if (GetTickCount() - flower_delay >= 50)
            {
                PlaySoundBuffer(fireball_Sound);
                SpawnFireball(g_player.mario.x + g_game.cameraX, g_player.mario.y);
                g_player.mario.fire_motion = true;
                g_player.mario.motion_timer = 5;
                flower_delay_bool = false;
            }
        }
        break;
    }
    }
}

void HandleGameChar(WPARAM wParam)
{
    switch (wParam)
    {
    case ('z'):
    {
        if (g_player.mario.form == FORM_TINO)
        {
            if (g_player.mario.tino_cooldown_z > 0) break;
            g_monsters.bowser.ignoreTinoFire = false;
            PlaySoundBuffer(bowserfire_Sound);
            g_player.mario.tino_fire_motion = true;
            g_player.mario.motion_timer = 10;
            SpawnTinoFire(g_player.mario.x + g_game.cameraX, g_player.mario.y);
            g_player.mario.tino_cooldown_z = 5;
        }
        break;
    }
    }
}

void TickCooldowns()
{
    if (g_player.mario.tino_cooldown_z > 0)
        g_player.mario.tino_cooldown_z--;
    if (g_player.mario.tino_cooldown_space > 0)
        g_player.mario.tino_cooldown_space--;
}
