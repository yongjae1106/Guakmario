#pragma once
#include <windows.h>
#include <gdiplus.h>
#pragma comment(lib, "Gdiplus.lib")

using namespace Gdiplus;

extern HDC mario_DC;

extern GdiplusStartupInput gdiplusStartupInput;
extern ULONG_PTR gdiplusToken;

struct ImageSet {
    Image* peach = nullptr;

    Image* stage_1_dirt = nullptr;
    Image* stage_1_brick = nullptr;
    Image* stage_1_background = nullptr;
    Image* stage_2_background = nullptr;

    Image* mushroom_head_1 = nullptr;
    Image* mushroom_head_2 = nullptr;
    Image* mushroom_head_3 = nullptr;
    Image* mushroom_trunk_1 = nullptr;
    Image* mushroom_trunk_2 = nullptr;

    Image* cloud_block = nullptr;

    Image* stone_tile = nullptr;
    Image* koopa_block = nullptr;

    Image* fire_head = nullptr;
    Image* fire_body = nullptr;

    Image* fire_switch_tile = nullptr;

    Image* firetrap = nullptr;

    Image* pipe_1 = nullptr;    // 우하
    Image* pipe_2 = nullptr;    // 좌하
    Image* pipe_3 = nullptr;    // 우상
    Image* pipe_4 = nullptr;    // 좌상

    Image* item_block_1 = nullptr;
    Image* item_block_2 = nullptr;
    Image* item_block_3 = nullptr;
    Image* item_block_used = nullptr;
    Image* unbreakable_block = nullptr;

    Image* castle_1 = nullptr;
    Image* castle_blank = nullptr;

    Image* coin_1 = nullptr;
    Image* coin_2 = nullptr;
    Image* coin_3 = nullptr;

    Image* mario_stop = nullptr;
    Image* mario_walk_motion_1 = nullptr;
    Image* mario_walk_motion_2 = nullptr;
    Image* mario_walk_motion_3 = nullptr;
    Image* mario_jump = nullptr;
    Image* mario_dead = nullptr;
    Image* big_mario_stop = nullptr;
    Image* big_mario_walk_motion_1 = nullptr;
    Image* big_mario_walk_motion_2 = nullptr;
    Image* big_mario_walk_motion_3 = nullptr;
    Image* big_mario_jump = nullptr;
    Image* big_mario_change = nullptr;

    Image* tino_mario_stop = nullptr;
    Image* tino_mario_walk_motion_1 = nullptr;
    Image* tino_mario_walk_motion_2 = nullptr;
    Image* tino_mario_walk_motion_3 = nullptr;
    Image* tino_mario_jump = nullptr;
    Image* tino_mario_attack_1 = nullptr;
    Image* tino_mario_attack_2 = nullptr;
    Image* tino_mario_attack_3 = nullptr;
    Image* tino_mario_attack_4 = nullptr;
    Image* tino_mario_attack_5 = nullptr;
    Image* tino_mario_attack_6 = nullptr;
    Image* tino_mario_fire_1 = nullptr;
    Image* tino_mario_fire_2 = nullptr;
    Image* tino_mario_fire_R_1 = nullptr;
    Image* tino_mario_fire_R_2 = nullptr;
    Image* tino_mario_fire_fade_1 = nullptr;
    Image* tino_mario_fire_fade_2 = nullptr;
    Image* tino_mario_fire_fade_3 = nullptr;

    Image* flower_mario_stop = nullptr;
    Image* flower_mario_walk_motion_1 = nullptr;
    Image* flower_mario_walk_motion_2 = nullptr;
    Image* flower_mario_walk_motion_3 = nullptr;
    Image* flower_mario_jump = nullptr;
    Image* flower_mario_change = nullptr;
    Image* flower_mario_fire = nullptr;

    Image* star_mario_stop_1 = nullptr;
    Image* star_mario_walk_motion_1_1 = nullptr;
    Image* star_mario_walk_motion_2_1 = nullptr;
    Image* star_mario_walk_motion_3_1 = nullptr;
    Image* star_mario_jump_1 = nullptr;
    Image* star_big_mario_stop_1 = nullptr;
    Image* star_big_mario_walk_motion_1_1 = nullptr;
    Image* star_big_mario_walk_motion_2_1 = nullptr;
    Image* star_big_mario_walk_motion_3_1 = nullptr;
    Image* star_big_mario_jump_1 = nullptr;

    Image* star_mario_stop_2 = nullptr;
    Image* star_mario_walk_motion_1_2 = nullptr;
    Image* star_mario_walk_motion_2_2 = nullptr;
    Image* star_mario_walk_motion_3_2 = nullptr;
    Image* star_mario_jump_2 = nullptr;
    Image* star_big_mario_stop_2 = nullptr;
    Image* star_big_mario_walk_motion_1_2 = nullptr;
    Image* star_big_mario_walk_motion_2_2 = nullptr;
    Image* star_big_mario_walk_motion_3_2 = nullptr;
    Image* star_big_mario_jump_2 = nullptr;

    Image* star_mario_stop_3 = nullptr;
    Image* star_mario_walk_motion_1_3 = nullptr;
    Image* star_mario_walk_motion_2_3 = nullptr;
    Image* star_mario_walk_motion_3_3 = nullptr;
    Image* star_mario_jump_3 = nullptr;
    Image* star_big_mario_stop_3 = nullptr;
    Image* star_big_mario_walk_motion_1_3 = nullptr;
    Image* star_big_mario_walk_motion_2_3 = nullptr;
    Image* star_big_mario_walk_motion_3_3 = nullptr;
    Image* star_big_mario_jump_3 = nullptr;

    Image* item_mushroom = nullptr;
    Image* item_up_mushroom = nullptr;
    Image* item_star_1 = nullptr;
    Image* item_star_2 = nullptr;
    Image* item_star_3 = nullptr;
    Image* item_star_4 = nullptr;
    Image* item_flower_1 = nullptr;
    Image* item_flower_2 = nullptr;
    Image* item_flower_3 = nullptr;
    Image* item_flower_4 = nullptr;
    Image* item_tino = nullptr;

    Image* shot_fireball_1 = nullptr;
    Image* shot_fireball_2 = nullptr;
    Image* shot_fireball_3 = nullptr;
    Image* shot_fireball_4 = nullptr;
    Image* shot_fireball_fadeout_1 = nullptr;
    Image* shot_fireball_fadeout_2 = nullptr;
    Image* shot_fireball_fadeout_3 = nullptr;

    Image* monster1_motion1 = nullptr;
    Image* monster1_motion2 = nullptr;
    Image* monster1_dead = nullptr;

    Image* monster2_motion1 = nullptr;
    Image* monster2_motion2 = nullptr;
    Image* monster2_dead = nullptr;

    Image* monster3_motion1 = nullptr;
    Image* monster3_motion2 = nullptr;
    Image* monster3_dead = nullptr;

    Image* turtle_1 = nullptr;
    Image* turtle_2 = nullptr;
    Image* turtle_R_1 = nullptr;
    Image* turtle_R_2 = nullptr;
    Image* turtle_hide = nullptr;

    Image* brown_turtle_1 = nullptr;
    Image* brown_turtle_2 = nullptr;
    Image* brown_turtle_R_1 = nullptr;
    Image* brown_turtle_R_2 = nullptr;
    Image* brown_turtle_hide = nullptr;

    Image* angel_turtle_1 = nullptr;
    Image* angel_turtle_2 = nullptr;

    Image* bowser_walk_1 = nullptr;
    Image* bowser_walk_2 = nullptr;
    Image* bowser_fire_walk_1 = nullptr;
    Image* bowser_fire_walk_2 = nullptr;

    Image* bowser_fireball_1 = nullptr;
    Image* bowser_fireball_2 = nullptr;

    Image* screen_coin_1 = nullptr;
    Image* screen_coin_2 = nullptr;
    Image* screen_coin_3 = nullptr;
    Image* screen_coin_x = nullptr;

    Image* title_screen = nullptr;
    Image* title_cursor = nullptr;
    Image* title_dead = nullptr;

    Image* flag_stick = nullptr;
    Image* flag_marble = nullptr;
    Image* flag = nullptr;

    int mario_stop_Width = 0;
    int mario_stop_Height = 0;
};
extern ImageSet g_images;

extern void image_load();
