#pragma once
#include <stdio.h>
#include <windows.h>
#include <gdiplus.h>
#pragma comment(lib, "Gdiplus.lib")
#pragma comment(lib, "Msimg32.lib")

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 640
#define TILE_SIZE 40
#define MAP_WIDTH 200
#define MAP_HEIGHT 15

enum TileType
{
    TILE_EMPTY        = 0,
    TILE_GROUND       = 1,
    TILE_COIN         = 2,
    TILE_STAIR        = 5,
    TILE_MYSTERY      = 6,
    TILE_FLAG         = 7,
    TILE_FLAG_TOP     = 8,
    TILE_CASTLE       = 9,
    TILE_BRICK        = 10,
    TILE_MUSHROOM_T1  = 11,
    TILE_MUSHROOM_T2  = 12,
    TILE_CLOUD        = 13,
    TILE_FIRE_SWITCH  = 14,
    TILE_STONE        = 15,
    TILE_USED_BLOCK   = 16,
    TILE_LAVA_HEAD    = 17,
    TILE_LAVA_BODY    = 18,
    TILE_KOOPA_BLOCK  = 19,
    TILE_FIRE_FLAME   = 20,
    TILE_PIPE_RB      = 40,
    TILE_PIPE_LB      = 41,
    TILE_PIPE_RT      = 42,
    TILE_PIPE_LT      = 43,
    TILE_BOX_STAR     = 60,
    TILE_BOX_FLOWER   = 61,
    TILE_BOX_TINO     = 62,
    TILE_BOX_UPMUSH   = 63,
    TILE_BOX_COIN     = 64,
    TILE_BOX_STAR_HIDDEN = 65,
    TILE_MUSHROOM_H1  = 90,
    TILE_MUSHROOM_H2  = 91,
    TILE_MUSHROOM_H3  = 92,
    TILE_INVISIBLE    = 999,
};

