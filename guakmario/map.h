#pragma once
#include "data.h"

extern int map1[MAP_HEIGHT][MAP_WIDTH];
extern int map2[MAP_HEIGHT][MAP_WIDTH];
extern int map3[MAP_HEIGHT][MAP_WIDTH];
extern int (*currentMap)[MAP_WIDTH];

void InitMap();
void InitMap2();
void InitMap3();

constexpr int CASTLE_STAGE = 3;
constexpr int CASTLE_GATE_COLUMN = 138;
constexpr int CASTLE_GATE_TOP_ROW = 5;
constexpr int CASTLE_GATE_BOTTOM_ROW = 8;
bool IsCastleGateOpen();
void OpenCastleGate();

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
void ResetTraps();
