#include <tchar.h>
#include "data.h"
#include "image.h"
#include "item.h"
#include "sound.h"
#include "monster.h"
#include "map.h"
#include "player.h"
#include "game.h"
#include "renderer.h"

using namespace Gdiplus;

static void Draw_spawn_item();
static void Draw_map();
static void Draw_castle_blank();
static void Draw_mario(const MarioRenderData& mario);
static void Draw_item();
static void Draw_background();
static void Draw_fireball();
static void Draw_Monsters();
static void Draw_Turtles();
static void Draw_Angel_Turtles();
static void Draw_Bowser();
static void Draw_Fireballs();
static void DrawFireTraps();

static void DrawSprite(Graphics& g, Image* img, int x, int y, int w, int h, bool flipX)
{
    if (flipX)
    {
        Matrix m(-1.0f, 0.0f, 0.0f, 1.0f, (REAL)(x + w), 0.0f);
        g.SetTransform(&m);
        g.DrawImage(img, 0, y, w, h);
        g.ResetTransform();
    }
    else
    {
        g.DrawImage(img, x, y, w, h);
    }
}

void Draw()
{
    RECT rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    if (g_game.stage == 1 || g_game.stage == 2) FillRect(g_memDC, &rect, sky_brush);
    else if (g_game.stage == 3) FillRect(g_memDC, &rect, black_brush);
    Draw_background();
    Graphics graphics(g_memDC);
    graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);

    Draw_spawn_item();
    Draw_map();
    Draw_item();
    Draw_Monsters();
    Draw_Turtles();
    Draw_Angel_Turtles();
    Draw_Bowser();
    Draw_Fireballs();
    Draw_fireball();
    DrawFireTraps();

    const MarioRenderData mario = BuildMarioRenderData();
    const bool flipX = (mario.direction == 0);

    // mario
    if (g_game.gameState == GAME_TRANSFORMING && g_game.transformTarget == FORM_BIG && mario.form == FORM_SMALL)
    {
        if ((GetTickCount() / 100) % 2 == 0)
        {
            DrawSprite(graphics, g_images.big_mario_change, mario.x, mario.y, mario.width, TILE_SIZE * 2, flipX);
        }
        else
        {
            DrawSprite(graphics, g_images.mario_stop, mario.x, mario.y + TILE_SIZE, mario.width, mario.height, flipX);
        }
    }
    else if (g_game.gameState == GAME_TRANSFORMING && g_game.transformTarget == FORM_BIG && mario.form == FORM_BIG)
    {
        if ((GetTickCount() / 100) % 2 == 0)
        {
            DrawSprite(graphics, g_images.big_mario_change, mario.x, mario.y - TILE_SIZE, mario.width, mario.height, flipX);
        }
        else
        {
            DrawSprite(graphics, g_images.mario_stop, mario.x, mario.y, mario.width, TILE_SIZE, flipX);
        }
    }
    else if (g_game.gameState == GAME_TRANSFORMING && g_game.transformTarget == FORM_FLOWER && mario.form == FORM_SMALL)    // small mario >> flower
    {
        if ((GetTickCount() / 100) % 2 == 0)
        {
            DrawSprite(graphics, g_images.flower_mario_change, mario.x, mario.y, mario.width, TILE_SIZE * 2, flipX);
        }
        else
        {
            DrawSprite(graphics, g_images.mario_stop, mario.x, mario.y + TILE_SIZE, mario.width, mario.height, flipX);
        }
    }
    else if (g_game.gameState == GAME_TRANSFORMING && g_game.transformTarget == FORM_FLOWER && mario.form == FORM_BIG)     // big mario >> flower
    {
        if ((GetTickCount() / 100) % 2 == 0)
        {
            DrawSprite(graphics, g_images.flower_mario_stop, mario.x, mario.y, mario.width, mario.height, flipX);
        }
        else
        {
            DrawSprite(graphics, g_images.big_mario_stop, mario.x, mario.y, mario.width, mario.height, flipX);
        }
    }
    else if (g_game.gameState == GAME_TRANSFORMING && g_game.transformTarget == FORM_FLOWER && mario.form == FORM_TINO)     // tino >> flower
    {
        if ((GetTickCount() / 100) % 2 == 0)
        {
            DrawSprite(graphics, g_images.tino_mario_stop, mario.x, mario.y, mario.width, mario.height, flipX);
        }
        else
        {
            DrawSprite(graphics, g_images.flower_mario_stop, mario.x, mario.y, mario.width, mario.height, flipX);
        }
    }
    else if (g_game.gameState == GAME_TRANSFORMING && g_game.transformTarget == FORM_FLOWER && mario.form == FORM_FLOWER)     // flower >> big mario
    {
        if ((GetTickCount() / 100) % 2 == 0)
        {
            DrawSprite(graphics, g_images.flower_mario_stop, mario.x, mario.y, mario.width, mario.height, flipX);
        }
        else
        {
            DrawSprite(graphics, g_images.big_mario_stop, mario.x, mario.y, mario.width, mario.height, flipX);
        }
    }
    // tino
    else if (g_game.gameState == GAME_TRANSFORMING && g_game.transformTarget == FORM_TINO && mario.form == FORM_SMALL)    // small mario >> tino
    {
        if ((GetTickCount() / 100) % 2 == 0)
        {
            DrawSprite(graphics, g_images.tino_mario_stop, mario.x, mario.y, mario.width, TILE_SIZE * 2, flipX);
        }
        else
        {
            DrawSprite(graphics, g_images.mario_stop, mario.x, mario.y + TILE_SIZE, mario.width, mario.height, flipX);
        }
    }
    else if (g_game.gameState == GAME_TRANSFORMING && g_game.transformTarget == FORM_TINO && mario.form == FORM_BIG)     // big mario >> tino
    {
        if ((GetTickCount() / 100) % 2 == 0)
        {
            DrawSprite(graphics, g_images.tino_mario_stop, mario.x, mario.y, mario.width, mario.height, flipX);
        }
        else
        {
            DrawSprite(graphics, g_images.big_mario_stop, mario.x, mario.y, mario.width, mario.height, flipX);
        }
    }
    else if (g_game.gameState == GAME_TRANSFORMING && g_game.transformTarget == FORM_TINO && mario.form == FORM_FLOWER)     // flower >> tino
    {
        if ((GetTickCount() / 100) % 2 == 0)
        {
            DrawSprite(graphics, g_images.tino_mario_stop, mario.x, mario.y, mario.width, mario.height, flipX);
        }
        else
        {
            DrawSprite(graphics, g_images.flower_mario_stop, mario.x, mario.y, mario.width, mario.height, flipX);
        }
    }
    else if (g_game.gameState == GAME_TRANSFORMING && g_game.transformTarget == FORM_TINO && mario.form == FORM_TINO)     // tino >> big mario
    {
        if ((GetTickCount() / 100) % 2 == 0)
        {
            DrawSprite(graphics, g_images.tino_mario_stop, mario.x, mario.y, mario.width, mario.height, flipX);
        }
        else
        {
            DrawSprite(graphics, g_images.big_mario_stop, mario.x, mario.y, mario.width, mario.height, flipX);
        }
    }
    // gameover
    else if (g_game.gameState == GAME_OVER && !mario.isDead)
    {
        graphics.DrawImage(g_images.title_dead, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        Draw_information();
        TCHAR life_print_dead[32], world_dead[32];
        _stprintf_s(world_dead, _T("WORLD %d"), g_game.stage);
        _stprintf_s(life_print_dead, _T("%d"), mario.life);

        TextOut(g_memDC, 300, 207, world_dead, lstrlen(world_dead));
        TextOut(g_memDC, 430, 295, life_print_dead, lstrlen(life_print_dead));
        return;
    }
    else
    {
        if(!mario.isDead)Draw_mario(mario);
    }

    if (mario.isDead)
    {
        DrawSprite(graphics, g_images.mario_dead, mario.x, mario.y, mario.width, TILE_SIZE, flipX);
    }

    Draw_castle_blank();
}
static void Draw_map()
{
    // 0: 구멍 1: 땅 2: 코인 3:굼바 4:파이프 5:계단 6:미스테리박스 7:깃발 8: 깃발꼭짓점 9:성 10: 벽돌 
    // 90. 버섯머리1 91. 버섯머리2 92. 버섯머리3 11. 버섯줄기 12. 버섯줄기2 13.구름 16: 사용된블럭 60:스타박스 61:꽃박스
    Graphics graphics(g_memDC);
    graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);

    // 맵 타일 그리기
    for (int i = 0; i < MAP_HEIGHT; i++)
    {
        for (int j = 0; j < MAP_WIDTH; j++)
        {
            int screenX = j * TILE_SIZE - g_game.cameraX;
            int screenY = i * TILE_SIZE;
            if (currentMap[i][j] == TILE_GROUND)         // 장애물
            {
                if (screenX + TILE_SIZE >= 0 && screenX < SCREEN_WIDTH)
                {
                    graphics.DrawImage(g_images.stage_1_dirt, screenX, screenY, TILE_SIZE + 1, TILE_SIZE + 1);
                }
            }
            else if (currentMap[i][j] == TILE_BRICK)         // 벽돌
            {
                if (screenX + TILE_SIZE >= 0 && screenX < SCREEN_WIDTH)
                {
                    graphics.DrawImage(g_images.stage_1_brick, screenX, screenY, TILE_SIZE + 1, TILE_SIZE + 1);
                }
            }
            else if (currentMap[i][j] == TILE_COIN)         // 코인
            {
                if (screenX + TILE_SIZE >= 0 && screenX < SCREEN_WIDTH)
                {
                    if (g_game.frameMotion == 0 || (g_game.frameMotion >= 4 && g_game.frameMotion < 7))
                    {
                        graphics.DrawImage(g_images.coin_1, screenX, screenY, TILE_SIZE + 1, TILE_SIZE);
                    }
                    else if (g_game.frameMotion == 1 || g_game.frameMotion == 3)
                    {
                        graphics.DrawImage(g_images.coin_2, screenX, screenY, TILE_SIZE + 1, TILE_SIZE);
                    }
                    else if (g_game.frameMotion == 2)
                    {
                        graphics.DrawImage(g_images.coin_3, screenX, screenY, TILE_SIZE + 1, TILE_SIZE);
                    }
                }
            }
            else if (currentMap[i][j] == TILE_STAIR)    // 계단
            {
                if (screenX + TILE_SIZE >= 0 && screenX < SCREEN_WIDTH)
                {
                    graphics.DrawImage(g_images.unbreakable_block, screenX, screenY, TILE_SIZE + 1, TILE_SIZE + 1);
                }
            }
            else if (currentMap[i][j] == TILE_MYSTERY || currentMap[i][j] == TILE_BOX_STAR || currentMap[i][j] == TILE_BOX_FLOWER || currentMap[i][j] == TILE_BOX_TINO || currentMap[i][j] == TILE_BOX_UPMUSH || currentMap[i][j] == TILE_BOX_COIN)    // 아이템 블럭
            {
                if (screenX + TILE_SIZE >= 0 && screenX < SCREEN_WIDTH)
                {
                    if (g_game.frameMotion == 0 || (g_game.frameMotion >= 4 && g_game.frameMotion < 7))
                    {
                        graphics.DrawImage(g_images.item_block_1, screenX, screenY, TILE_SIZE + 1, TILE_SIZE + 1);
                    }
                    else if (g_game.frameMotion == 1 || g_game.frameMotion == 3)
                    {
                        graphics.DrawImage(g_images.item_block_2, screenX, screenY, TILE_SIZE + 1, TILE_SIZE + 1);
                    }
                    else if (g_game.frameMotion == 2)
                    {
                        graphics.DrawImage(g_images.item_block_3, screenX, screenY, TILE_SIZE + 1, TILE_SIZE + 1);
                    }
                }
            }
            // 깃발 봉
            else if (currentMap[i][j] == TILE_FLAG)
            {
                if (screenX + TILE_SIZE >= 0 && screenX < SCREEN_WIDTH)
                {
                    graphics.DrawImage(g_images.flag_stick, screenX, screenY, TILE_SIZE + 1, TILE_SIZE + 1);
                }
            }
            // 깃발 꼭대기
            else if (currentMap[i][j] == TILE_FLAG_TOP)
            {
                if (screenX + TILE_SIZE >= 0 && screenX < SCREEN_WIDTH)
                {
                    graphics.DrawImage(g_images.flag_marble, screenX, screenY, TILE_SIZE + 1, TILE_SIZE + 1);
                }
            }
            // 성
            else if (currentMap[i][j] == TILE_CASTLE)
            {
                if (screenX + TILE_SIZE >= 0 && screenX < SCREEN_WIDTH)
                {
                    graphics.DrawImage(g_images.castle_1, screenX - TILE_SIZE * 3, screenY - TILE_SIZE * 4, (TILE_SIZE + 1) * 5, (TILE_SIZE + 1) * 5);
                }
            }
            // 버섯머리1
            else if (currentMap[i][j] == TILE_MUSHROOM_H1)
            {
                if (screenX + TILE_SIZE >= 0 && screenX < SCREEN_WIDTH)
                {
                    graphics.DrawImage(g_images.mushroom_head_1, screenX, screenY, TILE_SIZE + 1, TILE_SIZE + 1);
                }
            }
            else if (currentMap[i][j] == TILE_MUSHROOM_H2)
            {
                if (screenX + TILE_SIZE >= 0 && screenX < SCREEN_WIDTH)
                {
                    graphics.DrawImage(g_images.mushroom_head_2, screenX, screenY, TILE_SIZE + 1, TILE_SIZE + 1);
                }
            }
            else if (currentMap[i][j] == TILE_MUSHROOM_H3)
            {
                if (screenX + TILE_SIZE >= 0 && screenX < SCREEN_WIDTH)
                {
                    graphics.DrawImage(g_images.mushroom_head_3, screenX, screenY, TILE_SIZE + 1, TILE_SIZE + 1);
                }
            }
            // 버섯줄기
            else if (currentMap[i][j] == TILE_MUSHROOM_T1)
            {
                if (screenX + TILE_SIZE >= 0 && screenX < SCREEN_WIDTH)
                {
                    graphics.DrawImage(g_images.mushroom_trunk_1, screenX, screenY, TILE_SIZE + 1, TILE_SIZE + 1);
                }
            }
            else if (currentMap[i][j] == TILE_MUSHROOM_T2)
            {
                if (screenX + TILE_SIZE >= 0 && screenX < SCREEN_WIDTH)
                {
                    graphics.DrawImage(g_images.mushroom_trunk_2, screenX, screenY, TILE_SIZE + 1, TILE_SIZE + 1);
                }
            }
            else if (currentMap[i][j] == TILE_CLOUD)
            {
                if (screenX + TILE_SIZE >= 0 && screenX < SCREEN_WIDTH)
                {
                    graphics.DrawImage(g_images.cloud_block, screenX, screenY, TILE_SIZE + 1, TILE_SIZE + 1);
                }
            }
            else if (currentMap[i][j] == TILE_FIRE_SWITCH)
            {
                if (screenX + TILE_SIZE >= 0 && screenX < SCREEN_WIDTH)
                {
                    graphics.DrawImage(g_images.fire_switch_tile, screenX, screenY, TILE_SIZE + 1, TILE_SIZE + 1);
                }
                }
            //스테이지 3 블럭
            else if (currentMap[i][j] == TILE_STONE)
            {
                if (screenX + TILE_SIZE >= 0 && screenX < SCREEN_WIDTH)
                {
                    graphics.DrawImage(g_images.stone_tile, screenX, screenY, TILE_SIZE + 1, TILE_SIZE + 1);
                }
            }
            // 사용된 아이템블럭
            else if (currentMap[i][j] == TILE_USED_BLOCK)
            {
                if (screenX + TILE_SIZE >= 0 && screenX < SCREEN_WIDTH)
                {
                    graphics.DrawImage(g_images.item_block_used, screenX, screenY, TILE_SIZE + 1, TILE_SIZE + 1);
                }
             }
            //불 블럭
            else if (currentMap[i][j] == TILE_LAVA_HEAD)
            {
                if (screenX + TILE_SIZE >= 0 && screenX < SCREEN_WIDTH)
                {
                    graphics.DrawImage(g_images.fire_head, screenX, screenY, TILE_SIZE + 1, TILE_SIZE + 1);
                }
            }
            else if (currentMap[i][j] == TILE_LAVA_BODY)
            {
                if (screenX + TILE_SIZE >= 0 && screenX < SCREEN_WIDTH)
                {
                    graphics.DrawImage(g_images.fire_body, screenX, screenY, TILE_SIZE + 1, TILE_SIZE + 1);
                }
            }
            else if (currentMap[i][j] == TILE_KOOPA_BLOCK)
            {
                if (screenX + TILE_SIZE >= 0 && screenX < SCREEN_WIDTH)
                {
                    graphics.DrawImage(g_images.koopa_block, screenX, screenY, TILE_SIZE + 1, TILE_SIZE + 1);
                }
             }
            // 파이프
            else if (currentMap[i][j] == TILE_PIPE_RB) // 우하
            {
                if (screenX + TILE_SIZE >= 0 && screenX < SCREEN_WIDTH)
                {
                    graphics.DrawImage(g_images.pipe_1, screenX, screenY, TILE_SIZE + 1, TILE_SIZE + 1);
                }
            }
            else if (currentMap[i][j] == TILE_PIPE_LB) // 좌하
            {
                if (screenX + TILE_SIZE >= 0 && screenX < SCREEN_WIDTH)
                {
                    graphics.DrawImage(g_images.pipe_2, screenX, screenY, TILE_SIZE + 1, TILE_SIZE + 1);
                }
            }
            else if (currentMap[i][j] == TILE_PIPE_RT) // 좌상
            {
                if (screenX + TILE_SIZE >= 0 && screenX < SCREEN_WIDTH)
                {
                    graphics.DrawImage(g_images.pipe_3, screenX, screenY, TILE_SIZE + 1, TILE_SIZE + 1);
                }
            }
            else if (currentMap[i][j] == TILE_PIPE_LT) // 좌상
            {
                if (screenX + TILE_SIZE >= 0 && screenX < SCREEN_WIDTH)
                {
                    graphics.DrawImage(g_images.pipe_4, screenX, screenY, TILE_SIZE + 1, TILE_SIZE + 1);
                }
            }
            else if (currentMap[i][j] == TILE_INVISIBLE)
            {
                if (screenX + TILE_SIZE >= 0 && screenX < SCREEN_WIDTH)
                {
                    graphics.DrawImage(g_images.peach, screenX, screenY, TILE_SIZE + 1, TILE_SIZE * 2);
                }
           }
            
        }
    }

}
static void Draw_castle_blank()
{
    Graphics graphics(g_memDC);
    graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);

    for (int i = 0; i < MAP_HEIGHT; i++)
    {
        for (int j = 0; j < MAP_WIDTH; j++)
        {
            int screenX = j * TILE_SIZE - g_game.cameraX;
            int screenY = i * TILE_SIZE;
            if (currentMap[i][j] == TILE_CASTLE)
            {
                if (screenX + TILE_SIZE >= 0 && screenX < SCREEN_WIDTH)
                {
                    graphics.DrawImage(g_images.castle_blank, screenX - TILE_SIZE, screenY - TILE_SIZE, TILE_SIZE + 1, (TILE_SIZE + 1) * 2);
                }
            }

        }
    }
}
static void Draw_mario(const MarioRenderData& mario)
{
    Graphics graphics(g_memDC);
    graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);
    const bool flipX = (mario.direction == 0);

    if (mario.form == FORM_SMALL && !mario.star) // small mario
    {
        if (mario.isJumping)
        {
            DrawSprite(graphics, g_images.mario_jump, mario.x, mario.y, mario.width, mario.height, flipX);
        }
        else if (mario.isWalking)
        {
            if (mario.walk_motion == 0)
            {
                DrawSprite(graphics, g_images.mario_walk_motion_1, mario.x, mario.y, mario.width, mario.height, flipX);
            }
            else if (mario.walk_motion == 1)
            {
                DrawSprite(graphics, g_images.mario_walk_motion_2, mario.x, mario.y, mario.width, mario.height, flipX);
            }
            else if (mario.walk_motion == 2)
            {
                DrawSprite(graphics, g_images.mario_walk_motion_3, mario.x, mario.y, mario.width, mario.height, flipX);
            }
        }
        else
        {
            DrawSprite(graphics, g_images.mario_stop, mario.x, mario.y, mario.width, mario.height, flipX);
        }
    }
    else if (mario.form == FORM_BIG && !mario.star) // big mario
    {
        if (mario.isJumping)
        {
            DrawSprite(graphics, g_images.big_mario_jump, mario.x, mario.y, mario.width, mario.height, flipX);
        }
        else if (mario.isWalking)
        {
            if (mario.walk_motion == 0)
            {
                DrawSprite(graphics, g_images.big_mario_walk_motion_1, mario.x, mario.y, mario.width, mario.height, flipX);
            }
            else if (mario.walk_motion == 1)
            {
                DrawSprite(graphics, g_images.big_mario_walk_motion_2, mario.x, mario.y, mario.width, mario.height, flipX);
            }
            else if (mario.walk_motion == 2)
            {
                DrawSprite(graphics, g_images.big_mario_walk_motion_3, mario.x, mario.y, mario.width, mario.height, flipX);
            }
        }
        else
        {
            DrawSprite(graphics, g_images.big_mario_stop, mario.x, mario.y, mario.width, mario.height, flipX);
        }
    }
    else if (mario.form == FORM_SMALL && mario.star) // star small mario
    {
        if (g_game.frameMotionStar == 0)
        {
            if (mario.isJumping)
            {
                DrawSprite(graphics, g_images.star_mario_jump_1, mario.x, mario.y, mario.width, mario.height, flipX);
            }
            else if (mario.isWalking)
            {
                if (mario.walk_motion == 0)
                {
                    DrawSprite(graphics, g_images.star_mario_walk_motion_1_1, mario.x, mario.y, mario.width, mario.height, flipX);
                }
                else if (mario.walk_motion == 1)
                {
                    DrawSprite(graphics, g_images.star_mario_walk_motion_2_1, mario.x, mario.y, mario.width, mario.height, flipX);
                }
                else if (mario.walk_motion == 2)
                {
                    DrawSprite(graphics, g_images.star_mario_walk_motion_3_1, mario.x, mario.y, mario.width, mario.height, flipX);
                }
            }
            else
            {
                DrawSprite(graphics, g_images.star_mario_stop_1, mario.x, mario.y, mario.width, mario.height, flipX);
            }
        }
        else if (g_game.frameMotionStar == 1)
        {
            if (mario.isJumping)
            {
                DrawSprite(graphics, g_images.star_mario_jump_2, mario.x, mario.y, mario.width, mario.height, flipX);
            }
            else if (mario.isWalking)
            {
                if (mario.walk_motion == 0)
                {
                    DrawSprite(graphics, g_images.star_mario_walk_motion_1_2, mario.x, mario.y, mario.width, mario.height, flipX);
                }
                else if (mario.walk_motion == 1)
                {
                    DrawSprite(graphics, g_images.star_mario_walk_motion_2_2, mario.x, mario.y, mario.width, mario.height, flipX);
                }
                else if (mario.walk_motion == 2)
                {
                    DrawSprite(graphics, g_images.star_mario_walk_motion_3_2, mario.x, mario.y, mario.width, mario.height, flipX);
                }
            }
            else
            {
                DrawSprite(graphics, g_images.star_mario_stop_2, mario.x, mario.y, mario.width, mario.height, flipX);
            }
        }
        else if (g_game.frameMotionStar == 2)
        {
            if (mario.isJumping)
            {
                DrawSprite(graphics, g_images.star_mario_jump_3, mario.x, mario.y, mario.width, mario.height, flipX);
            }
            else if (mario.isWalking)
            {
                if (mario.walk_motion == 0)
                {
                    DrawSprite(graphics, g_images.star_mario_walk_motion_1_3, mario.x, mario.y, mario.width, mario.height, flipX);
                }
                else if (mario.walk_motion == 1)
                {
                    DrawSprite(graphics, g_images.star_mario_walk_motion_2_3, mario.x, mario.y, mario.width, mario.height, flipX);
                }
                else if (mario.walk_motion == 2)
                {
                    DrawSprite(graphics, g_images.star_mario_walk_motion_3_3, mario.x, mario.y, mario.width, mario.height, flipX);
                }
            }
            else
            {
                DrawSprite(graphics, g_images.star_mario_stop_3, mario.x, mario.y, mario.width, mario.height, flipX);
            }
        }
    }
    else if (mario.form != FORM_SMALL && mario.star) // star big mario
    {
        if (g_game.frameMotionStar == 0)
        {
            if (mario.isJumping)
            {
                DrawSprite(graphics, g_images.star_big_mario_jump_1, mario.x, mario.y, mario.width, mario.height, flipX);
            }
            else if (mario.isWalking)
            {
                if (mario.walk_motion == 0)
                {
                    DrawSprite(graphics, g_images.star_big_mario_walk_motion_1_1, mario.x, mario.y, mario.width, mario.height, flipX);
                }
                else if (mario.walk_motion == 1)
                {
                    DrawSprite(graphics, g_images.star_big_mario_walk_motion_2_1, mario.x, mario.y, mario.width, mario.height, flipX);
                }
                else if (mario.walk_motion == 2)
                {
                    DrawSprite(graphics, g_images.star_big_mario_walk_motion_3_1, mario.x, mario.y, mario.width, mario.height, flipX);
                }
            }
            else
            {
                DrawSprite(graphics, g_images.star_big_mario_stop_1, mario.x, mario.y, mario.width, mario.height, flipX);
            }
        }
        else if (g_game.frameMotionStar == 1)
        {
            if (mario.isJumping)
            {
                DrawSprite(graphics, g_images.star_big_mario_jump_2, mario.x, mario.y, mario.width, mario.height, flipX);
            }
            else if (mario.isWalking)
            {
                if (mario.walk_motion == 0)
                {
                    DrawSprite(graphics, g_images.star_big_mario_walk_motion_1_2, mario.x, mario.y, mario.width, mario.height, flipX);
                }
                else if (mario.walk_motion == 1)
                {
                    DrawSprite(graphics, g_images.star_big_mario_walk_motion_2_2, mario.x, mario.y, mario.width, mario.height, flipX);
                }
                else if (mario.walk_motion == 2)
                {
                    DrawSprite(graphics, g_images.star_big_mario_walk_motion_3_2, mario.x, mario.y, mario.width, mario.height, flipX);
                }
            }
            else
            {
                DrawSprite(graphics, g_images.star_big_mario_stop_2, mario.x, mario.y, mario.width, mario.height, flipX);
            }
        }
        else if (g_game.frameMotionStar == 2)
        {
            if (mario.isJumping)
            {
                DrawSprite(graphics, g_images.star_big_mario_jump_3, mario.x, mario.y, mario.width, mario.height, flipX);
            }
            else if (mario.isWalking)
            {
                if (mario.walk_motion == 0)
                {
                    DrawSprite(graphics, g_images.star_big_mario_walk_motion_1_3, mario.x, mario.y, mario.width, mario.height, flipX);
                }
                else if (mario.walk_motion == 1)
                {
                    DrawSprite(graphics, g_images.star_big_mario_walk_motion_2_3, mario.x, mario.y, mario.width, mario.height, flipX);
                }
                else if (mario.walk_motion == 2)
                {
                    DrawSprite(graphics, g_images.star_big_mario_walk_motion_3_3, mario.x, mario.y, mario.width, mario.height, flipX);
                }
            }
            else
            {
                DrawSprite(graphics, g_images.star_big_mario_stop_3, mario.x, mario.y, mario.width, mario.height, flipX);
            }
        }
    }
    else if (mario.form == FORM_FLOWER) // flower mario
    {
        if (mario.fire_motion)
        {
            DrawSprite(graphics, g_images.flower_mario_fire, mario.x, mario.y, mario.width, mario.height, flipX);
        }
        else if (mario.isJumping)
        {
            DrawSprite(graphics, g_images.flower_mario_jump, mario.x, mario.y, mario.width, mario.height, flipX);
        }
        else if (mario.isWalking)
        {
            if (mario.walk_motion == 0)
            {
                DrawSprite(graphics, g_images.flower_mario_walk_motion_1, mario.x, mario.y, mario.width, mario.height, flipX);
            }
            else if (mario.walk_motion == 1)
            {
                DrawSprite(graphics, g_images.flower_mario_walk_motion_2, mario.x, mario.y, mario.width, mario.height, flipX);
            }
            else if (mario.walk_motion == 2)
            {
                DrawSprite(graphics, g_images.flower_mario_walk_motion_3, mario.x, mario.y, mario.width, mario.height, flipX);
            }
        }
        else
        {
            DrawSprite(graphics, g_images.flower_mario_stop, mario.x, mario.y, mario.width, mario.height, flipX);
        }
    }
    else if (mario.form == FORM_TINO) // tino mario
    {
        if (mario.tino_motion)
        {
            if (mario.motion_timer >= 0 && mario.motion_timer <= 5)
            {
                DrawSprite(graphics, g_images.tino_mario_attack_6, mario.x - 25, mario.y - 15, 100, 100, flipX);
            }
            else if (mario.motion_timer > 5 && mario.motion_timer <= 10)
            {
                DrawSprite(graphics, g_images.tino_mario_attack_5, mario.x - 25, mario.y - 15, 100, 100, flipX);
            }
            else if (mario.motion_timer > 10 && mario.motion_timer <= 15)
            {
                DrawSprite(graphics, g_images.tino_mario_attack_4, mario.x - 25, mario.y - 15, 100, 100, flipX);
            }
            else if (mario.motion_timer > 15 && mario.motion_timer <= 20)
            {
                DrawSprite(graphics, g_images.tino_mario_attack_3, mario.x - 25, mario.y - 15, 100, 100, flipX);
            }
            else if (mario.motion_timer > 20 && mario.motion_timer <= 25)
            {
                DrawSprite(graphics, g_images.tino_mario_attack_2, mario.x - 25, mario.y - 15, 100, 100, flipX);
            }
            else if (mario.motion_timer > 25 && mario.motion_timer <= 30)
            {
                DrawSprite(graphics, g_images.tino_mario_attack_1, mario.x - 25, mario.y - 15, 100, 100, flipX);
            }
        }
        else if (mario.tino_fire_motion)
        {
            DrawSprite(graphics, g_images.tino_mario_attack_2, mario.x - 25, mario.y - 15, 100, 100, flipX);
        }
        else if (mario.isJumping)
        {
            DrawSprite(graphics, g_images.tino_mario_jump, mario.x, mario.y, mario.width, mario.height, flipX);
        }
        else if (mario.isWalking)
        {
            if (mario.walk_motion == 0)
            {
                DrawSprite(graphics, g_images.tino_mario_walk_motion_1, mario.x, mario.y, mario.width, mario.height, flipX);
            }
            else if (mario.walk_motion == 1)
            {
                DrawSprite(graphics, g_images.tino_mario_walk_motion_2, mario.x, mario.y, mario.width, mario.height, flipX);
            }
            else if (mario.walk_motion == 2)
            {
                DrawSprite(graphics, g_images.tino_mario_walk_motion_3, mario.x, mario.y, mario.width, mario.height, flipX);
            }
        }
        else
        {
            DrawSprite(graphics, g_images.tino_mario_stop, mario.x, mario.y, mario.width, mario.height, flipX);
        }
    }
}
static void Draw_spawn_item()
{
    Graphics graphics(g_memDC);
    graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);

    // 스폰모션그리기
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        // 버섯
        if (mushroom[i].motion)
        {
            graphics.DrawImage(g_images.item_mushroom, mushroom[i].x - g_game.cameraX, mushroom[i].y, 40, 40);
        }
        // 생명 버섯
        if (up_mushroom[i].motion)
        {
            graphics.DrawImage(g_images.item_up_mushroom, up_mushroom[i].x - g_game.cameraX, up_mushroom[i].y, 40, 40);
        }
        // 스타
        if (star[i].motion)
        {
            if (g_game.frameMotion == 0 || g_game.frameMotion == 4 || g_game.frameMotion == 5 || g_game.frameMotion == 6)
            {
                graphics.DrawImage(g_images.item_star_1, star[i].x - g_game.cameraX, star[i].y, 40, 40);
            }
            else if (g_game.frameMotion == 1)
            {
                graphics.DrawImage(g_images.item_star_2, star[i].x - g_game.cameraX, star[i].y, 40, 40);
            }
            else if (g_game.frameMotion == 2)
            {
                graphics.DrawImage(g_images.item_star_3, star[i].x - g_game.cameraX, star[i].y, 40, 40);
            }
            else if (g_game.frameMotion == 3)
            {
                graphics.DrawImage(g_images.item_star_4, star[i].x - g_game.cameraX, star[i].y, 40, 40);
            }
        }
        // 꽃
        if (flower[i].motion) // 꽃
        {
            if (g_game.frameMotion == 0 || g_game.frameMotion == 4 || g_game.frameMotion == 5 || g_game.frameMotion == 6)
            {
                graphics.DrawImage(g_images.item_flower_1, flower[i].x - g_game.cameraX, flower[i].y, 40, 40);
            }
            else if (g_game.frameMotion == 1)
            {
                graphics.DrawImage(g_images.item_flower_2, flower[i].x - g_game.cameraX, flower[i].y, 40, 40);
            }
            else if (g_game.frameMotion == 2)
            {
                graphics.DrawImage(g_images.item_flower_3, flower[i].x - g_game.cameraX, flower[i].y, 40, 40);
            }
            else if (g_game.frameMotion == 3)
            {
                graphics.DrawImage(g_images.item_flower_4, flower[i].x - g_game.cameraX, flower[i].y, 40, 40);
            }
        }
        // 티노
        if (tino[i].motion)
        {
            graphics.DrawImage(g_images.item_tino, tino[i].x - g_game.cameraX, tino[i].y, 40, 40);
        }
    }

}
static void Draw_item()
{
    Graphics graphics(g_memDC);
    graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);

    for (int i = 0; i < MAX_ITEMS; i++)
    {
        // 버섯
        if (mushroom[i].active) // 버섯
        {
            graphics.DrawImage(g_images.item_mushroom, mushroom[i].x - g_game.cameraX, mushroom[i].y, 40, 40);
        }
        if (up_mushroom[i].active) // 생명버섯
        {
            graphics.DrawImage(g_images.item_up_mushroom, up_mushroom[i].x - g_game.cameraX, up_mushroom[i].y, 40, 40);
        }
        // 스타
        if (star[i].active) // 스타
        {
            if(g_game.frameMotion == 0 || g_game.frameMotion == 4 || g_game.frameMotion == 5 || g_game.frameMotion == 6)
            {
                graphics.DrawImage(g_images.item_star_1, star[i].x - g_game.cameraX, star[i].y, 40, 40);
            }
            else if (g_game.frameMotion == 1)
            {
                graphics.DrawImage(g_images.item_star_2, star[i].x - g_game.cameraX, star[i].y, 40, 40);
            }
            else if (g_game.frameMotion == 2)
            {
                graphics.DrawImage(g_images.item_star_3, star[i].x - g_game.cameraX, star[i].y, 40, 40);
            }
            else if (g_game.frameMotion == 3)
            {
                graphics.DrawImage(g_images.item_star_4, star[i].x - g_game.cameraX, star[i].y, 40, 40);
            }
        }
        // 꽃
        if (flower[i].active) // 꽃
        {
            if (g_game.frameMotion == 0 || g_game.frameMotion == 4 || g_game.frameMotion == 5 || g_game.frameMotion == 6)
            {
                graphics.DrawImage(g_images.item_flower_1, flower[i].x - g_game.cameraX, flower[i].y, 40, 40);
            }
            else if (g_game.frameMotion == 1)
            {
                graphics.DrawImage(g_images.item_flower_2, flower[i].x - g_game.cameraX, flower[i].y, 40, 40);
            }
            else if (g_game.frameMotion == 2)
            {
                graphics.DrawImage(g_images.item_flower_3, flower[i].x - g_game.cameraX, flower[i].y, 40, 40);
            }
            else if (g_game.frameMotion == 3)
            {
                graphics.DrawImage(g_images.item_flower_4, flower[i].x - g_game.cameraX, flower[i].y, 40, 40);
            }
        }
        // 티노
        if (tino[i].active)
        {
            graphics.DrawImage(g_images.item_tino, tino[i].x - g_game.cameraX, tino[i].y, 40, 40);
        }

    }

}
static void Draw_fireball()
{
    Graphics graphics(g_memDC);
    graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);

    for (int i = 0; i < MAX_SHOT; i++)
    {
        // flower fire
        if (!fireball[i].active) continue;
        int screenX = fireball[i].x - g_game.cameraX;
        int screenY = fireball[i].y;
        if (fireball[i].motion == 0)
        {
            graphics.DrawImage(g_images.shot_fireball_1, screenX, screenY, 20, 20);
        }
        else if (fireball[i].motion == 1)
        {
            graphics.DrawImage(g_images.shot_fireball_2, screenX, screenY, 20, 20);
        }
        else if (fireball[i].motion == 2)
        {
            graphics.DrawImage(g_images.shot_fireball_3, screenX, screenY, 20, 20);
        }
        else if (fireball[i].motion == 3)
        {
            graphics.DrawImage(g_images.shot_fireball_4, screenX, screenY, 20, 20);
        }

    }
    for (int i = 0; i < MAX_SHOT; i++)
    {
        // tino fire
        if (tinofire_effect[i].active)
        {
            int screenX = tinofire_effect[i].x - g_game.cameraX;
            int screenY = tinofire_effect[i].y;
            if(tinofire_effect[i].timer < 5)
            {
                graphics.DrawImage(g_images.tino_mario_fire_fade_1, screenX, screenY - 40, TILE_SIZE * 2, TILE_SIZE * 2);
            }
            else if (tinofire_effect[i].timer < 10)
            {
                graphics.DrawImage(g_images.tino_mario_fire_fade_2, screenX, screenY - 40, TILE_SIZE * 2, TILE_SIZE * 2);
            }
            else if (tinofire_effect[i].timer < 15)
            {
                graphics.DrawImage(g_images.tino_mario_fire_fade_3, screenX, screenY - 40, TILE_SIZE * 2, TILE_SIZE * 2);
            }
        }
        if (!tinofire[i].active) continue;
        int screenX = tinofire[i].x - g_game.cameraX;
        int screenY = tinofire[i].y;
        if (tinofire[i].direction == 0)
        {
            if (tinofire[i].motion < 3)
            {
                graphics.DrawImage(g_images.tino_mario_fire_1, screenX, screenY, TILE_SIZE * 2, TILE_SIZE * 2);
            }
            else 
            {
                graphics.DrawImage(g_images.tino_mario_fire_2, screenX, screenY, TILE_SIZE * 2, TILE_SIZE * 2);
            }
        }
        else if (tinofire[i].direction == 1)
        {
            if (tinofire[i].motion < 3)
            {
                graphics.DrawImage(g_images.tino_mario_fire_R_1, screenX, screenY, TILE_SIZE * 2, TILE_SIZE * 2);
            }
            else
            {
                graphics.DrawImage(g_images.tino_mario_fire_R_2, screenX, screenY, TILE_SIZE * 2, TILE_SIZE * 2);
            }
        }
    }
}
void Draw_information()
{
    const MarioRenderData mario = BuildMarioRenderData();
    Graphics graphics(g_memDC);
    graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);
    TCHAR cooldown_z[32], cooldown_space[32];
    TCHAR clear_text_1[32];
    TCHAR clear_text_2[32];

    // 타이틀 스크린
    _stprintf_s(life_print, _T("MARIO"));
    _stprintf_s(life_print2, _T("%05d"), mario.life);
    _stprintf_s(time_print, _T("TIME"));
    _stprintf_s(time_print2, _T("%03d"), g_game.stage_time);
    _stprintf_s(coin_print, _T("%02d"), mario.coin);
    _stprintf_s(time_print2, _T("%03d"), g_game.stage_time);
    _stprintf_s(stage_print, _T("WORLD"));
    _stprintf_s(stage_print2, _T("%d"), g_game.stage);
    _stprintf_s(cooldown_z, _T("Z (fire): %d"), mario.tino_cooldown_z);
    _stprintf_s(cooldown_space, _T("SPACE (bite): %d"), mario.tino_cooldown_space);
    _stprintf_s(clear_text_1, _T("THANK YOU MARIO!"));
    _stprintf_s(clear_text_2, _T("YOUR QUEST IS OVER.!"));


    TextOut(g_memDC, 80, 20, life_print, lstrlen(life_print));         // 생명
    TextOut(g_memDC, 80, 40, life_print2, lstrlen(life_print2));         // 생명

    TextOut(g_memDC, 620, 20, time_print, lstrlen(time_print));         // 시간
    TextOut(g_memDC, 640, 40, time_print2, lstrlen(time_print2));         // 시간


    // 코인
    if (g_game.frameMotion < 4)
    {
        graphics.DrawImage(g_images.screen_coin_1, 260, 40, 25, 25);
    }
    else if (g_game.frameMotion == 4 || g_game.frameMotion == 6)
    {
        graphics.DrawImage(g_images.screen_coin_2, 260, 40, 25, 25);
    }
    else if (g_game.frameMotion == 5)
    {
        graphics.DrawImage(g_images.screen_coin_3, 260, 40, 25, 25);
    }
    graphics.DrawImage(g_images.screen_coin_x, 290, 40, 25, 25);
    TextOut(g_memDC, 320, 40, coin_print, lstrlen(coin_print));         // 코인

    TextOut(g_memDC, 440, 20, stage_print, lstrlen(stage_print));         // 월드
    TextOut(g_memDC, 480, 40, stage_print2, lstrlen(stage_print2));

    if(mario.form == FORM_TINO)
    {
        TextOut(g_memDC, 80, 80, cooldown_z, lstrlen(cooldown_z));         // 쿨타임
        TextOut(g_memDC, 80, 100, cooldown_space, lstrlen(cooldown_space));         // 쿨타임
    }
    if (g_game.gameClearText)
    {
        TextOut(g_memDC, 240, 160, clear_text_1, lstrlen(clear_text_1));         
        TextOut(g_memDC, 200, 240, clear_text_2, lstrlen(clear_text_2));        
    }

}

static void Draw_background()
{
    Graphics graphics(g_memDC);
    graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);

    if (g_game.stage == 1)
    {
        graphics.DrawImage(g_images.stage_1_background, -g_game.cameraX, -36, 4600, SCREEN_HEIGHT);
    }
    else if (g_game.stage == 2)
    {
        graphics.DrawImage(g_images.stage_2_background, -g_game.cameraX, -36, 4600, SCREEN_HEIGHT);
    }
}
static void Draw_Monsters()
{
    Graphics graphics(g_memDC);
    graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);

    for (int i = 0; i < g_monsters.monsterCount; i++)
    {
        if (!g_monsters.monsters[i].active)
            continue;

        int screenX = g_monsters.monsters[i].x - g_game.cameraX;
        int screenY = g_monsters.monsters[i].y;

        if (!g_monsters.monsters[i].isAlive)
        {
            if (g_game.stage == 1)
                graphics.DrawImage(g_images.monster1_dead, screenX, screenY, TILE_SIZE, TILE_SIZE);
            else if (g_game.stage == 2)
                graphics.DrawImage(g_images.monster2_dead, screenX, screenY, TILE_SIZE, TILE_SIZE);
            else if (g_game.stage == 3)
                graphics.DrawImage(g_images.monster3_dead, screenX, screenY, TILE_SIZE, TILE_SIZE);

            DWORD now = GetTickCount();
            if (g_monsters.monsters[i].deadStart == 0)
                g_monsters.monsters[i].deadStart = now;
            if (now - g_monsters.monsters[i].deadStart >= 300)
                g_monsters.monsters[i].active = false;

            continue;
        }

        if (screenX + TILE_SIZE < 0 || screenX >= SCREEN_WIDTH) continue;

        if (g_game.stage == 1)
            graphics.DrawImage((g_game.frameMotion % 2 == 0) ? g_images.monster1_motion1 : g_images.monster1_motion2, screenX, screenY, TILE_SIZE, TILE_SIZE);
        else if (g_game.stage == 2)
            graphics.DrawImage((g_game.frameMotion % 2 == 0) ? g_images.monster2_motion1 : g_images.monster2_motion2, screenX, screenY, TILE_SIZE, TILE_SIZE);
        else if (g_game.stage == 3)
            graphics.DrawImage((g_game.frameMotion % 2 == 0) ? g_images.monster3_motion1 : g_images.monster3_motion2, screenX, screenY, TILE_SIZE, TILE_SIZE);
    }
}
static void Draw_Turtles()
{
    Graphics graphics(g_memDC);
    graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);
    for (int i = 0; i < g_monsters.turtleCount; i++)
    {
        if (!g_monsters.turtles[i].isAlive) continue;

        int drawX = g_monsters.turtles[i].x - g_game.cameraX;
        int drawY = g_monsters.turtles[i].y;

        switch (g_monsters.turtles[i].turtleState)
        {
        case NORMAL:
        {
            if(g_monsters.turtles[i].direction == 0)
            {
                graphics.DrawImage((g_game.stage == 3)
                    ? ((g_game.frameMotion % 2 == 0) ? g_images.brown_turtle_1 : g_images.brown_turtle_2)
                    : ((g_game.frameMotion % 2 == 0) ? g_images.turtle_1 : g_images.turtle_2), drawX, drawY - TILE_SIZE, TILE_SIZE, TILE_SIZE * 2);
            }
            else if (g_monsters.turtles[i].direction == 1)
            {
                graphics.DrawImage((g_game.stage == 3) 
                    ? ((g_game.frameMotion % 2 == 0) ? g_images.brown_turtle_R_1 : g_images.brown_turtle_R_2) 
                    : ((g_game.frameMotion % 2 == 0) ? g_images.turtle_R_1 : g_images.turtle_R_2), drawX, drawY - TILE_SIZE, TILE_SIZE, TILE_SIZE * 2);
            }
            break;
        }
        case SHELL:
        case SPINNING:
        {
            graphics.DrawImage(g_game.stage==3 ? g_images.brown_turtle_hide : g_images.turtle_hide, drawX, drawY, 40, 40);
            break;
        }
        }
    }
}
static void Draw_Angel_Turtles()
{
    Graphics graphics(g_memDC);
    graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);
    for (int i = 0; i < g_monsters.angelTurtleCount; i++)
    {
        if (!g_monsters.angelTurtles[i].isAlive) continue;

        int drawX = g_monsters.angelTurtles[i].x - g_game.cameraX;
        int drawY = g_monsters.angelTurtles[i].y;

        switch (g_monsters.angelTurtles[i].state)
        {
        case FLYING:
        {
            graphics.DrawImage((g_game.frameMotion % 2 == 0) ? g_images.angel_turtle_1 : g_images.angel_turtle_2, drawX, drawY - TILE_SIZE, TILE_SIZE, TILE_SIZE + 20);
            break;
        }
        case HIDE:
        {
            graphics.DrawImage(g_images.turtle_hide, drawX, drawY, 40, 40);
            break;
        }
        }
    }
}
// g_monsters.bowser
static void Draw_Bowser()
{
    if (!g_monsters.bowser.isAlive) return;

    Graphics graphics(g_memDC);
    graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);

    int screenX = g_monsters.bowser.x - g_game.cameraX;
    int screenY = g_monsters.bowser.y;

    // 걷기 또는 불뿜기 애니메이션 프레임 처리
    static int walkFrame = 0;
    static DWORD lastFrameTime = GetTickCount();

    if (GetTickCount() - lastFrameTime > 200) { // 0.2초마다 프레임 전환
        walkFrame = (walkFrame + 1) % 2;
        lastFrameTime = GetTickCount();
    }

    // 불을 뿜는 중이면 fire 이미지 사용
    if (g_monsters.bowser.isFiring)
    {
        if (walkFrame == 0)
            graphics.DrawImage(g_images.bowser_fire_walk_1, screenX, screenY, g_monsters.bowser.width, g_monsters.bowser.height);
        else
            graphics.DrawImage(g_images.bowser_fire_walk_2, screenX, screenY, g_monsters.bowser.width, g_monsters.bowser.height);
    }
    else {
        if (walkFrame == 0)
            graphics.DrawImage(g_images.bowser_walk_1, screenX, screenY, g_monsters.bowser.width, g_monsters.bowser.height);
        else
            graphics.DrawImage(g_images.bowser_walk_2, screenX, screenY, g_monsters.bowser.width, g_monsters.bowser.height);
    }
}
static void Draw_Fireballs()
{
    Graphics graphics(g_memDC);
    graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);

    for (int i = 0; i < MAX_FIREBALLS; i++) 
    {
        if (!g_monsters.fireballs[i].active) continue;

        int sx = g_monsters.fireballs[i].x - g_game.cameraX;
        int sy = g_monsters.fireballs[i].y;

        if(fireball[i].motion < 3)
        {
            graphics.DrawImage(g_images.bowser_fireball_1, sx, sy, TILE_SIZE + 30, 30);
        }
        else
        {
            graphics.DrawImage(g_images.bowser_fireball_2, sx, sy, TILE_SIZE + 30, 30);
        }
    }
}
static void DrawFireTraps()
{
    Graphics graphics(g_memDC);
    graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);

    for (int i = 0; i < fireTrapCount; i++) 
    {
        if (!fireTraps[i].active) continue;

        for (int j = 0; j < fireTraps[i].tileCount; j++)
        {
            int drawX = fireTraps[i].x - g_game.cameraX - 20;
            int drawY = fireTraps[i].y + j * TILE_SIZE;

            graphics.DrawImage(g_images.firetrap, drawX, drawY, TILE_SIZE, TILE_SIZE);
        }
    }
}
// 방향키 함수 
