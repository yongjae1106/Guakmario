#include "image.h"

HDC mario_DC;

GdiplusStartupInput gdiplusStartupInput;
ULONG_PTR gdiplusToken = 0;

ImageSet g_images;

// 이미지 로드
void image_load()
{
    g_images.peach = new Image(L"resource\\peach\\peach.png");
    // stage_1
    g_images.stage_1_dirt = new Image(L"resource\\tile\\stage_1_dirt.png");
    if (g_images.stage_1_dirt->GetLastStatus() != Ok)
    {
        MessageBox(NULL, L"stage_1_dirt 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.stage_1_brick = new Image(L"resource\\tile\\stage_1_brick.png");
    if (g_images.stage_1_brick->GetLastStatus() != Ok)
    {
        MessageBox(NULL, L"stage_1_brick 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.stage_1_background = new Image(L"resource\\background\\stage_1_background.png");
    g_images.stage_2_background = new Image(L"resource\\background\\stage_2_background.png");
    if (g_images.stage_1_background->GetLastStatus() != Ok)
    {
        MessageBox(NULL, L"stage_1_background 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    // stage_2
    g_images.mushroom_head_1 = new Image(L"resource\\tile\\mushroom_head_1.png");
    g_images.mushroom_head_2 = new Image(L"resource\\tile\\mushroom_head_2.png");
    g_images.mushroom_head_3 = new Image(L"resource\\tile\\mushroom_head_3.png");
    g_images.mushroom_trunk_1 = new Image(L"resource\\tile\\mushroom_trunk_1.png");
    g_images.mushroom_trunk_2 = new Image(L"resource\\tile\\mushroom_trunk_2.png");

    g_images.cloud_block = new Image(L"resource\\tile\\cloud_block.png");

    //stage_3 tile
    g_images.koopa_block = new Image(L"resource\\tile\\koopa_block.png");
    g_images.stone_tile = new Image(L"resource\\tile\\stone_tile.png");
    if (g_images.stone_tile->GetLastStatus() != Ok)
    {
        MessageBox(NULL, L"stone_tile 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.fire_switch_tile = new Image(L"resource\\tile\\fire_switch_tile.png");
    if (g_images.fire_switch_tile->GetLastStatus() != Ok)
    {
        MessageBox(NULL, L"fire_switch_tile 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.firetrap = new Image(L"resource\\tile\\fire.png");
    if (g_images.firetrap->GetLastStatus() != Ok)
    {
        MessageBox(NULL, L"firetrap 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }

    //g_game.stage 3 fire
    g_images.fire_head = new Image(L"resource\\tile\\fire_head.png");
    if (g_images.fire_head->GetLastStatus() != Ok)
    {
        MessageBox(NULL, L"fire_head 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.fire_body = new Image(L"resource\\tile\\fire_body.png");
    if (g_images.fire_body->GetLastStatus() != Ok)
    {
        MessageBox(NULL, L"fire_body 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }

    // pipe
    g_images.pipe_1 = new Image(L"resource\\pipe\\pipe_1.png");
    if (g_images.pipe_1->GetLastStatus() != Ok)
    {
        MessageBox(NULL, L"pipe_1 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.pipe_2 = new Image(L"resource\\pipe\\pipe_2.png");
    if (g_images.pipe_2->GetLastStatus() != Ok)
    {
        MessageBox(NULL, L"pipe_2 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.pipe_3 = new Image(L"resource\\pipe\\pipe_3.png");
    if (g_images.pipe_3->GetLastStatus() != Ok)
    {
        MessageBox(NULL, L"pipe_3 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.pipe_4 = new Image(L"resource\\pipe\\pipe_4.png");
    if (g_images.pipe_4->GetLastStatus() != Ok)
    {
        MessageBox(NULL, L"pipe_4 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }

    // default block
    g_images.item_block_1 = new Image(L"resource\\tile\\item_block_1.png");
    if (g_images.item_block_1->GetLastStatus() != Ok)
    {
        MessageBox(NULL, L"item_block_1 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.item_block_2 = new Image(L"resource\\tile\\item_block_2.png");
    if (g_images.item_block_2->GetLastStatus() != Ok)
    {
        MessageBox(NULL, L"item_block_2 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.item_block_3 = new Image(L"resource\\tile\\item_block_3.png");
    if (g_images.item_block_3->GetLastStatus() != Ok)
    {
        MessageBox(NULL, L"item_block_3 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.item_block_used = new Image(L"resource\\tile\\item_block_used.png");
    if (g_images.item_block_used->GetLastStatus() != Ok)
    {
        MessageBox(NULL, L"item_block_used 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.unbreakable_block = new Image(L"resource\\tile\\unbreakable_block.png");
    if (g_images.unbreakable_block->GetLastStatus() != Ok)
    {
        MessageBox(NULL, L"unbreakable_block 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }

    g_images.castle_1 = new Image(L"resource\\tile\\castle_1.png");
    g_images.castle_blank = new Image(L"resource\\tile\\castle_blank.png");


    // coin
    g_images.coin_1 = new Image(L"resource\\tile\\coin_1.png");
    if (g_images.coin_1->GetLastStatus() != Ok)
    {
        MessageBox(NULL, L"coin_1 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.coin_2 = new Image(L"resource\\tile\\coin_2.png");
    if (g_images.coin_2->GetLastStatus() != Ok)
    {
        MessageBox(NULL, L"coin_2 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.coin_3 = new Image(L"resource\\tile\\coin_3.png");
    if (g_images.coin_3->GetLastStatus() != Ok)
    {
        MessageBox(NULL, L"coin_3 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }



    // mario
    g_images.mario_stop = new Image(L"resource\\mario\\mario_stop.png");
    if (g_images.mario_stop->GetLastStatus() != Ok) {
        MessageBox(NULL, L"mario_stop 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.mario_walk_motion_1 = new Image(L"resource\\mario\\mario_walk_motion_1.png");
    if (g_images.mario_walk_motion_1->GetLastStatus() != Ok) {
        MessageBox(NULL, L"mario_walk_motion_1 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.mario_walk_motion_2 = new Image(L"resource\\mario\\mario_walk_motion_2.png");
    if (g_images.mario_walk_motion_2->GetLastStatus() != Ok) {
        MessageBox(NULL, L"mario_walk_motion_2 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.mario_walk_motion_3 = new Image(L"resource\\mario\\mario_walk_motion_3.png");
    if (g_images.mario_walk_motion_3->GetLastStatus() != Ok) {
        MessageBox(NULL, L"mario_walk_motion_3 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.mario_jump = new Image(L"resource\\mario\\mario_jump.png");
    if (g_images.mario_jump->GetLastStatus() != Ok) {
        MessageBox(NULL, L"mario_jump 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.mario_dead = new Image(L"resource\\mario\\mario_dead.png");
    if (g_images.mario_dead->GetLastStatus() != Ok) {
        MessageBox(NULL, L"mario_dead 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    // bigmario
    g_images.big_mario_stop = new Image(L"resource\\mario\\big_mario_stop.png");
    if (g_images.big_mario_stop->GetLastStatus() != Ok) {
        MessageBox(NULL, L"big_mario_stop 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.big_mario_walk_motion_1 = new Image(L"resource\\mario\\big_mario_stop.png");
    if (g_images.big_mario_walk_motion_1->GetLastStatus() != Ok) {
        MessageBox(NULL, L"big_mario_walk_motion_1 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.big_mario_walk_motion_2 = new Image(L"resource\\mario\\big_mario_walk_motion_2.png");
    if (g_images.big_mario_walk_motion_2->GetLastStatus() != Ok) {
        MessageBox(NULL, L"big_mario_walk_motion_2 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.big_mario_walk_motion_3 = new Image(L"resource\\mario\\big_mario_walk_motion_3.png");
    if (g_images.big_mario_walk_motion_3->GetLastStatus() != Ok) {
        MessageBox(NULL, L"big_mario_walk_motion_3 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.big_mario_jump = new Image(L"resource\\mario\\big_mario_jump.png");
    if (g_images.big_mario_jump->GetLastStatus() != Ok) {
        MessageBox(NULL, L"big_mario_jump 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.big_mario_change = new Image(L"resource\\mario\\big_mario_change.png");
    if (g_images.big_mario_change->GetLastStatus() != Ok) {
        MessageBox(NULL, L"big_mario_change 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }

    // tino mario
    g_images.tino_mario_stop = new Image(L"resource\\mario\\tino\\Tino_mario_stop.png");
    g_images.tino_mario_jump = new Image(L"resource\\mario\\tino\\Tino_mario_jump.png");
    g_images.tino_mario_walk_motion_1 = new Image(L"resource\\mario\\tino\\Tino_mario_walk_motion_1.png");
    g_images.tino_mario_walk_motion_2 = new Image(L"resource\\mario\\tino\\Tino_mario_walk_motion_2.png");
    g_images.tino_mario_walk_motion_3 = new Image(L"resource\\mario\\tino\\Tino_mario_walk_motion_3.png");
    g_images.tino_mario_attack_1 = new Image(L"resource\\mario\\tino\\Tino_mario_attack_1.png");
    g_images.tino_mario_attack_2 = new Image(L"resource\\mario\\tino\\Tino_mario_attack_2.png");
    g_images.tino_mario_attack_3 = new Image(L"resource\\mario\\tino\\Tino_mario_attack_3.png");
    g_images.tino_mario_attack_4 = new Image(L"resource\\mario\\tino\\Tino_mario_attack_4.png");
    g_images.tino_mario_attack_5 = new Image(L"resource\\mario\\tino\\Tino_mario_attack_5.png");
    g_images.tino_mario_attack_6 = new Image(L"resource\\mario\\tino\\Tino_mario_attack_6.png");
    g_images.tino_mario_fire_1 = new Image(L"resource\\mario\\tino\\Tino_mario_fire_1.png");
    g_images.tino_mario_fire_2 = new Image(L"resource\\mario\\tino\\Tino_mario_fire_2.png");
    g_images.tino_mario_fire_R_1 = new Image(L"resource\\mario\\tino\\Tino_mario_fire_R_1.png");
    g_images.tino_mario_fire_R_2 = new Image(L"resource\\mario\\tino\\Tino_mario_fire_R_2.png");
    g_images.tino_mario_fire_fade_1 = new Image(L"resource\\mario\\tino\\Tino_mario_fire_fade_1.png");
    g_images.tino_mario_fire_fade_2 = new Image(L"resource\\mario\\tino\\Tino_mario_fire_fade_2.png");
    g_images.tino_mario_fire_fade_3 = new Image(L"resource\\mario\\tino\\Tino_mario_fire_fade_3.png");

    // flower mario
    g_images.flower_mario_stop = new Image(L"resource\\mario\\flower\\flower_mario_stop.png");
    g_images.flower_mario_jump = new Image(L"resource\\mario\\flower\\flower_mario_jump.png");
    g_images.flower_mario_walk_motion_1 = new Image(L"resource\\mario\\flower\\flower_mario_walk_motion_1.png");
    g_images.flower_mario_walk_motion_2 = new Image(L"resource\\mario\\flower\\flower_mario_walk_motion_2.png");
    g_images.flower_mario_walk_motion_3 = new Image(L"resource\\mario\\flower\\flower_mario_walk_motion_3.png");
    g_images.flower_mario_change = new Image(L"resource\\mario\\flower\\flower_mario_change.png");
    g_images.flower_mario_fire = new Image(L"resource\\mario\\flower\\flower_mario_fire.png");

    // star mario
    g_images.star_mario_stop_1 = new Image(L"resource\\mario\\star\\star_mario_stop_1.png");
    g_images.star_mario_stop_2 = new Image(L"resource\\mario\\star\\star_mario_stop_2.png");
    g_images.star_mario_stop_3 = new Image(L"resource\\mario\\star\\star_mario_stop_3.png");
    g_images.star_mario_walk_motion_1_1 = new Image(L"resource\\mario\\star\\star_mario_walk_motion_1_1.png");
    g_images.star_mario_walk_motion_1_2 = new Image(L"resource\\mario\\star\\star_mario_walk_motion_1_2.png");
    g_images.star_mario_walk_motion_1_3 = new Image(L"resource\\mario\\star\\star_mario_walk_motion_1_3.png");
    g_images.star_mario_walk_motion_2_1 = new Image(L"resource\\mario\\star\\star_mario_walk_motion_2_1.png");
    g_images.star_mario_walk_motion_2_2 = new Image(L"resource\\mario\\star\\star_mario_walk_motion_2_2.png");
    g_images.star_mario_walk_motion_2_3 = new Image(L"resource\\mario\\star\\star_mario_walk_motion_2_3.png");
    g_images.star_mario_walk_motion_3_1 = new Image(L"resource\\mario\\star\\star_mario_walk_motion_3_1.png");
    g_images.star_mario_walk_motion_3_2 = new Image(L"resource\\mario\\star\\star_mario_walk_motion_3_2.png");
    g_images.star_mario_walk_motion_3_3 = new Image(L"resource\\mario\\star\\star_mario_walk_motion_3_3.png");
    g_images.star_mario_jump_1 = new Image(L"resource\\mario\\star\\star_mario_jump_1.png");
    g_images.star_mario_jump_2 = new Image(L"resource\\mario\\star\\star_mario_jump_2.png");
    g_images.star_mario_jump_3 = new Image(L"resource\\mario\\star\\star_mario_jump_3.png");

    g_images.star_big_mario_stop_1 = new Image(L"resource\\mario\\star\\star_big_mario_stop_1.png");
    g_images.star_big_mario_stop_2 = new Image(L"resource\\mario\\star\\star_big_mario_stop_2.png");
    g_images.star_big_mario_stop_3 = new Image(L"resource\\mario\\star\\star_big_mario_stop_3.png");
    g_images.star_big_mario_walk_motion_1_1 = new Image(L"resource\\mario\\star\\star_big_mario_walk_motion_1_1.png");
    g_images.star_big_mario_walk_motion_1_2 = new Image(L"resource\\mario\\star\\star_big_mario_walk_motion_1_2.png");
    g_images.star_big_mario_walk_motion_1_3 = new Image(L"resource\\mario\\star\\star_big_mario_walk_motion_1_3.png");
    g_images.star_big_mario_walk_motion_2_1 = new Image(L"resource\\mario\\star\\star_big_mario_walk_motion_2_1.png");
    g_images.star_big_mario_walk_motion_2_2 = new Image(L"resource\\mario\\star\\star_big_mario_walk_motion_2_2.png");
    g_images.star_big_mario_walk_motion_2_3 = new Image(L"resource\\mario\\star\\star_big_mario_walk_motion_2_3.png");
    g_images.star_big_mario_walk_motion_3_1 = new Image(L"resource\\mario\\star\\star_big_mario_walk_motion_3_1.png");
    g_images.star_big_mario_walk_motion_3_2 = new Image(L"resource\\mario\\star\\star_big_mario_walk_motion_3_2.png");
    g_images.star_big_mario_walk_motion_3_3 = new Image(L"resource\\mario\\star\\star_big_mario_walk_motion_3_3.png");
    g_images.star_big_mario_jump_1 = new Image(L"resource\\mario\\star\\star_big_mario_jump_1.png");
    g_images.star_big_mario_jump_2 = new Image(L"resource\\mario\\star\\star_big_mario_jump_2.png");
    g_images.star_big_mario_jump_3 = new Image(L"resource\\mario\\star\\star_big_mario_jump_3.png");

    // items
    g_images.item_mushroom = new Image(L"resource\\items\\mushroom.png");
    g_images.item_up_mushroom = new Image(L"resource\\items\\up_mushroom.png");
    if (g_images.item_mushroom->GetLastStatus() != Ok) {
        MessageBox(NULL, L"item_mushroom 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.item_star_1 = new Image(L"resource\\items\\star_1.png");
    if (g_images.item_star_1->GetLastStatus() != Ok) {
        MessageBox(NULL, L"item_star_1 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.item_star_2 = new Image(L"resource\\items\\star_2.png");
    if (g_images.item_star_2->GetLastStatus() != Ok) {
        MessageBox(NULL, L"item_star_2 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.item_star_3 = new Image(L"resource\\items\\star_3.png");
    if (g_images.item_star_3->GetLastStatus() != Ok) {
        MessageBox(NULL, L"item_star_3 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.item_star_4 = new Image(L"resource\\items\\star_4.png");
    if (g_images.item_star_4->GetLastStatus() != Ok) {
        MessageBox(NULL, L"item_star_4 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.item_flower_1 = new Image(L"resource\\items\\flower_1.png");
    g_images.item_flower_2 = new Image(L"resource\\items\\flower_2.png");
    g_images.item_flower_3 = new Image(L"resource\\items\\flower_3.png");
    g_images.item_flower_4 = new Image(L"resource\\items\\flower_4.png");
    g_images.item_tino = new Image(L"resource\\items\\tino.png");

    g_images.shot_fireball_1 = new Image(L"resource\\items\\shot\\fireball_1.png");
    g_images.shot_fireball_2 = new Image(L"resource\\items\\shot\\fireball_2.png");
    g_images.shot_fireball_3 = new Image(L"resource\\items\\shot\\fireball_3.png");
    g_images.shot_fireball_4 = new Image(L"resource\\items\\shot\\fireball_4.png");
    g_images.shot_fireball_fadeout_1 = new Image(L"resource\\items\\shot\\fireball_fadeout_1.png");
    g_images.shot_fireball_fadeout_2 = new Image(L"resource\\items\\shot\\fireball_fadeout_2.png");
    g_images.shot_fireball_fadeout_3 = new Image(L"resource\\items\\shot\\fireball_fadeout_3.png");

    //monster 1
    g_images.monster1_motion1 = new Image(L"resource\\monster\\monster1_motion1.png");
    if (g_images.monster1_motion1->GetLastStatus() != Ok) {
        MessageBox(NULL, L"monster1_motion1 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.monster1_motion2 = new Image(L"resource\\monster\\monster1_motion2.png");
    if (g_images.monster1_motion2->GetLastStatus() != Ok) {
        MessageBox(NULL, L"monster1_motion2 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.monster1_dead = new Image(L"resource\\monster\\monster1_dead.png");
    if (g_images.monster1_dead->GetLastStatus() != Ok) {
        MessageBox(NULL, L"monster1_dead 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    //monster 2
    g_images.monster2_motion1 = new Image(L"resource\\monster\\monster2_motion1.png");
    if (g_images.monster2_motion1->GetLastStatus() != Ok) {
        MessageBox(NULL, L"monster1_motion1 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.monster2_motion2 = new Image(L"resource\\monster\\monster2_motion2.png");
    if (g_images.monster2_motion2->GetLastStatus() != Ok) {
        MessageBox(NULL, L"monster2_motion2 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.monster2_dead = new Image(L"resource\\monster\\monster2_dead.png");
    if (g_images.monster2_dead->GetLastStatus() != Ok) {
        MessageBox(NULL, L"monster2_dead 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    //monster 3
    g_images.monster3_motion1 = new Image(L"resource\\monster\\monster3_motion1.png");
    if (g_images.monster3_motion1->GetLastStatus() != Ok) {
        MessageBox(NULL, L"monster3_motion1 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.monster3_motion2 = new Image(L"resource\\monster\\monster3_motion2.png");
    if (g_images.monster3_motion2->GetLastStatus() != Ok) {
        MessageBox(NULL, L"monster3_motion2 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.monster3_dead = new Image(L"resource\\monster\\monster3_dead.png");
    if (g_images.monster3_dead->GetLastStatus() != Ok) {
        MessageBox(NULL, L"monster3_dead 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }

    //turtle
    g_images.turtle_1 = new Image(L"resource\\monster\\turtle_1.png");
    g_images.turtle_2 = new Image(L"resource\\monster\\turtle_2.png");
    g_images.turtle_R_1 = new Image(L"resource\\monster\\turtle_R_1.png");
    g_images.turtle_R_2 = new Image(L"resource\\monster\\turtle_R_2.png");
    g_images.turtle_hide = new Image(L"resource\\monster\\turtle_hide.png");

    g_images.brown_turtle_1 = new Image(L"resource\\monster\\brown_turtle_1.png");
    g_images.brown_turtle_2 = new Image(L"resource\\monster\\brown_turtle_2.png");
    g_images.brown_turtle_R_1 = new Image(L"resource\\monster\\brown_turtle_R_1.png");
    g_images.brown_turtle_R_2 = new Image(L"resource\\monster\\brown_turtle_R_2.png");
    g_images.brown_turtle_hide = new Image(L"resource\\monster\\brown_turtle_hide.png");

    //angel turtle
    g_images.angel_turtle_1 = new Image(L"resource\\monster\\angel_turtle_1.png");
    if (g_images.angel_turtle_1->GetLastStatus() != Ok) {
        MessageBox(NULL, L"angel_turtle_1 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.angel_turtle_2 = new Image(L"resource\\monster\\angel_turtle_2.png");
    if (g_images.angel_turtle_2->GetLastStatus() != Ok) {
        MessageBox(NULL, L"angel_turtle_2 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }

    //g_monsters.bowser
    g_images.bowser_walk_1 = new Image(L"resource\\monster\\bowser_walk_1.png");
    if (g_images.bowser_walk_1->GetLastStatus() != Ok) {
        MessageBox(NULL, L"bowser_walk_1 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.bowser_walk_2 = new Image(L"resource\\monster\\bowser_walk_2.png");
    if (g_images.bowser_walk_2->GetLastStatus() != Ok) {
        MessageBox(NULL, L"bowser_walk_2 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.bowser_fire_walk_1 = new Image(L"resource\\monster\\bowser_fire_walk_1.png");
    if (g_images.bowser_fire_walk_1->GetLastStatus() != Ok) {
        MessageBox(NULL, L"bowser_fire_walk_1 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.bowser_fire_walk_2 = new Image(L"resource\\monster\\bowser_fire_walk_2.png");
    if (g_images.bowser_fire_walk_2->GetLastStatus() != Ok) {
        MessageBox(NULL, L"bowser_fire_walk_2 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }

    //fireball
    g_images.bowser_fireball_1 = new Image(L"resource\\monster\\bowser_fireball_1.png");
    g_images.bowser_fireball_2 = new Image(L"resource\\monster\\bowser_fireball_1.png");

    // screen
    g_images.screen_coin_1 = new Image(L"resource\\screen\\screen_coin_1.png");
    if (g_images.screen_coin_1->GetLastStatus() != Ok) {
        MessageBox(NULL, L"screen_coin_1 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.screen_coin_2 = new Image(L"resource\\screen\\screen_coin_2.png");
    if (g_images.screen_coin_2->GetLastStatus() != Ok) {
        MessageBox(NULL, L"screen_coin_2 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.screen_coin_3 = new Image(L"resource\\screen\\screen_coin_3.png");
    if (g_images.screen_coin_3->GetLastStatus() != Ok) {
        MessageBox(NULL, L"screen_coin_3 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.screen_coin_x = new Image(L"resource\\screen\\screen_coin_x.png");
    if (g_images.screen_coin_x->GetLastStatus() != Ok) {
        MessageBox(NULL, L"screen_coin_x 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }

    // title
    g_images.title_screen = new Image(L"resource\\title\\title_screen.png");
    if (g_images.title_screen->GetLastStatus() != Ok) {
        MessageBox(NULL, L"title_screen 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.title_cursor = new Image(L"resource\\title\\title_cursor.png");
    if (g_images.title_cursor->GetLastStatus() != Ok) {
        MessageBox(NULL, L"title_cursor 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.title_dead = new Image(L"resource\\title\\title_dead.png");
    if (g_images.title_dead->GetLastStatus() != Ok) {
        MessageBox(NULL, L"title_dead 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }

    // g_images.flag
    g_images.flag_stick = new Image(L"resource\\tile\\flag_stick.png");
    if (g_images.flag_stick->GetLastStatus() != Ok) {
        MessageBox(NULL, L"flag_stick 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.flag_marble = new Image(L"resource\\tile\\flag_marble.png");
    if (g_images.flag_marble->GetLastStatus() != Ok) {
        MessageBox(NULL, L"flag_marble 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    g_images.flag = new Image(L"resource\\tile\\flag.png");
    if (g_images.flag->GetLastStatus() != Ok) {
        MessageBox(NULL, L"flag 이미지 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
}
