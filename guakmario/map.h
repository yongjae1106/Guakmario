#pragma once
#include "data.h"

extern int map1[MAP_HEIGHT][MAP_WIDTH];
extern int map2[MAP_HEIGHT][MAP_WIDTH];
extern int map3[MAP_HEIGHT][MAP_WIDTH];
extern int (*currentMap)[MAP_WIDTH];

void InitMap();
void InitMap2();
void InitMap3();

#define MAX_HAZARDS 100
struct Hazard
{
    int x, y;
    int width, height;
    int tileCount;
    bool active;
};
struct FireTrapPos
{
    int tileX;
    int tileY;
    int tileCount; // ���� �̹��� ��
};
extern FireTrapPos fireTrapPositions[];
extern int fireTrapPositionsCount;
extern Hazard fireTraps[MAX_HAZARDS];
extern int fireTrapCount;
void InitFireTraps();
void UpdateFireTraps();
void CheckMarioHazardCollision();
void trap_reset();
