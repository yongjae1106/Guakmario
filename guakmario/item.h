#pragma once
#include "data.h"
#define MAX_ITEMS 30
#define MAX_SHOT 10

struct Item
{
    int x, y;
    int vx, vy;
    int width, height;
    int spawnMotion;
    bool active;
    bool motion;

    Item(int _x = 0, int _y = 0, int _vx = 0, int _vy = 0, int _width = 30, int _height = 30, int _spawnMotion = 0, bool _active = false, bool _motion = false)
        : x(_x), y(_y), vx(_vx), vy(_vy), width(_width), height(_height), active(_active), spawnMotion(_spawnMotion), motion(_motion) {
    };
};
extern Item mushroom[MAX_ITEMS];
extern Item up_mushroom[MAX_ITEMS];
extern Item star[MAX_ITEMS];
extern Item flower[MAX_ITEMS];
extern Item tino[MAX_ITEMS];
extern int mushroom_count;

struct Shot
{
    int x, y, vx, vy;
    int width, height;
    int motion;
    int motion_fade;
    int duration;
    Direction direction;
    bool active;
    bool fade;
};
struct Shot_Effect
{
    int x, y;
    int timer;
    bool active;
};
extern Shot fireball[MAX_SHOT];
extern Shot tinofire[MAX_SHOT];
extern Shot_Effect tinofire_effect[MAX_SHOT];

void UpdateItems_mushroom();
void UpdateItems_up_mushroom();
void UpdateItems_star();
void UpdateItems_flower();
void UpdateItems_tino();
void UpdateShot_fireball();
void UpdateShot_tinofire();
void UpdateShot_tinofire_effect();
void OnMonsterHit(int x, int y);

void TransformToBig();
void TransformToSmall();
void TransformToFlower();
void TransformToTino();

void SpawnItem_mushroom(int x, int y);
void SpawnItem_up_mushroom(int x, int y);
void SpawnItem_star(int x, int y);
void SpawnItem_flower(int x, int y);
void SpawnItem_tino(int x, int y);
void SpawnFireball(int x, int y);
void SpawnTinoFire(int x, int y);

void HandleMushroomPickup();
void HandleUpMushroomPickup();
void HandleStarPickup();
void HandleFlowerPickup();
void HandleTinoPickup();
void HandleFireballHit();
void HandleTinoFireHit();

void TinoAttack();

void UpdateGodMode(DWORD _godstart);
void UpdateStarMode(DWORD _starstart);

void UpdateAllItems();
void UpdateAllShots();
void HandleItemCollisions();
void HandleShotCollisions();