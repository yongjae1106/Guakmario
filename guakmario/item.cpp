#include "network.h" // winsock2 must precede windows.h in item.h
#include "item.h"
#include "data.h"
#include "player.h"
#include "game.h"
#include "map.h"
#include "collision.h"
#include "sound.h"

Item up_mushroom[MAX_ITEMS];
Item mushroom[MAX_ITEMS];
Item star[MAX_ITEMS];
Item flower[MAX_ITEMS];
Item tino[MAX_ITEMS];
Shot fireball[MAX_SHOT];
Shot tinofire[MAX_SHOT];
Shot_Effect tinofire_effect[MAX_SHOT];

void ApplyItemPickupEffect(ItemType type)
{
    Player& player = g_player.mario;
    if (type == ITEM_UP_MUSHROOM) {
        PlaySoundBuffer(up_Sound);
        ++player.life;
        return;
    }
    if (type == ITEM_STAR) {
        if (!player.star) {
            PlayBGM("resource\\sound\\bgm\\InvincibilityTheam.wav");
            player.star = true;
            g_game.starStartTime = GetTickCount();
        }
        return;
    }

    MarioForm target;
    switch (type) {
    case ITEM_MUSHROOM: target = FORM_BIG; break;
    case ITEM_FLOWER: target = FORM_FLOWER; break;
    case ITEM_TINO: target = FORM_TINO; break;
    default: return;
    }

    PlaySoundBuffer(powerup_Sound);
    if (type == ITEM_MUSHROOM && player.form != FORM_SMALL) return;
    StartPlayerTransformation(target);
}

static void HandleLocalItemCollisions(Item (&items)[MAX_ITEMS], ItemType type)
{
    for (Item& item : items) {
        if (!item.active) continue;

        const int screenX = static_cast<int>(item.x - g_game.cameraX);
        const Player& player = g_player.mario;
        if (IsColliding(player.x, player.y, player.width, player.height,
            screenX, item.y, item.width, item.height)) {
            item.active = false;
            ApplyItemPickupEffect(type);
        }
    }
}

// 버섯 움직임
void UpdateItems_mushroom()
{
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        // 모션
        if (mushroom[i].motion)
        {
            mushroom[i].y -= 3;
            mushroom[i].spawnMotion++;
            if (mushroom[i].spawnMotion == 18)
            {
                mushroom[i].motion = false;
                mushroom[i].active = true;
                mushroom[i].spawnMotion = 0;
            }
        }
        if (!mushroom[i].active) continue;

        // 1️⃣ 수평 이동
        mushroom[i].x += mushroom[i].vx;

        // 충돌 계산
        int left = mushroom[i].x / TILE_SIZE;
        int right = (mushroom[i].x + TILE_SIZE - 1) / TILE_SIZE;
        int top = mushroom[i].y / TILE_SIZE;
        int bottom = (mushroom[i].y + TILE_SIZE - 1) / TILE_SIZE;

        // 2️⃣ 수평 충돌 처리
        if (mushroom[i].vx < 0 && (isSolidTile(currentMap[top][left]) || isSolidTile(currentMap[bottom][left])))
        {
            mushroom[i].x = (left + 1) * TILE_SIZE; // 경계 맞춤
            mushroom[i].vx = -mushroom[i].vx;      // 방향 반전
        }
        else if (mushroom[i].vx > 0 && (isSolidTile(currentMap[top][right]) || isSolidTile(currentMap[bottom][right])))
        {
            mushroom[i].x = right * TILE_SIZE - mushroom[i].width; // 경계 맞춤
            mushroom[i].vx = -mushroom[i].vx;                       // 방향 반전
        }

        // 3️⃣ 수직 낙하 (중력)
        mushroom[i].vy += 1;
        if (mushroom[i].vy > 10) mushroom[i].vy = 10;
        mushroom[i].y += mushroom[i].vy;

        left = mushroom[i].x / TILE_SIZE;
        right = (mushroom[i].x + TILE_SIZE - 1) / TILE_SIZE;
        top = mushroom[i].y / TILE_SIZE;
        bottom = (mushroom[i].y + TILE_SIZE - 1) / TILE_SIZE;

        // 아래 충돌 처리
        if (mushroom[i].vy > 0 && (isSolidTile(currentMap[bottom][left]) || isSolidTile(currentMap[bottom][right])))
        {
            mushroom[i].y = bottom * TILE_SIZE - TILE_SIZE; // 위치 보정 (딱 맞게)
            mushroom[i].vy = 0;

            // 땅에 닿았으면 떨림 방지용으로 소폭 위치 보정 (필요시)
            // mushroom[i].x = round(mushroom[i].x / (float)TILE_SIZE) * TILE_SIZE;
        }
    }
}
void UpdateItems_up_mushroom()
{
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        // 모션
        if (up_mushroom[i].motion)
        {
            up_mushroom[i].y -= 3;
            up_mushroom[i].spawnMotion++;
            if (up_mushroom[i].spawnMotion == 18)
            {
                up_mushroom[i].motion = false;
                up_mushroom[i].active = true;
                up_mushroom[i].spawnMotion = 0;
            }
        }
        if (!up_mushroom[i].active) continue;

        // 1️⃣ 수평 이동
        up_mushroom[i].x += up_mushroom[i].vx;

        // 충돌 계산
        int left = up_mushroom[i].x / TILE_SIZE;
        int right = (up_mushroom[i].x + TILE_SIZE - 1) / TILE_SIZE;
        int top = up_mushroom[i].y / TILE_SIZE;
        int bottom = (up_mushroom[i].y + TILE_SIZE - 1) / TILE_SIZE;

        // 2️⃣ 수평 충돌 처리
        if (up_mushroom[i].vx < 0 && (isSolidTile(currentMap[top][left]) || isSolidTile(currentMap[bottom][left])))
        {
            up_mushroom[i].x = (left + 1) * TILE_SIZE; // 경계 맞춤
            up_mushroom[i].vx = -up_mushroom[i].vx;      // 방향 반전
        }
        else if (up_mushroom[i].vx > 0 && (isSolidTile(currentMap[top][right]) || isSolidTile(currentMap[bottom][right])))
        {
            up_mushroom[i].x = right * TILE_SIZE - up_mushroom[i].width; // 경계 맞춤
            up_mushroom[i].vx = -up_mushroom[i].vx;                       // 방향 반전
        }

        // 3️⃣ 수직 낙하 (중력)
        up_mushroom[i].vy += 1;
        if (up_mushroom[i].vy > 10) up_mushroom[i].vy = 10;
        up_mushroom[i].y += up_mushroom[i].vy;

        left = up_mushroom[i].x / TILE_SIZE;
        right = (up_mushroom[i].x + TILE_SIZE - 1) / TILE_SIZE;
        top = up_mushroom[i].y / TILE_SIZE;
        bottom = (up_mushroom[i].y + TILE_SIZE - 1) / TILE_SIZE;

        // 아래 충돌 처리
        if (up_mushroom[i].vy > 0 && (isSolidTile(currentMap[bottom][left]) || isSolidTile(currentMap[bottom][right])))
        {
            up_mushroom[i].y = bottom * TILE_SIZE - TILE_SIZE; // 위치 보정 (딱 맞게)
            up_mushroom[i].vy = 0;

        }
    }
}
// 스타 움직임
void UpdateItems_star()
{
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        // 모션
        if (star[i].motion)
        {
            star[i].y -= 3;
            star[i].spawnMotion++;
            if (star[i].spawnMotion == 18)
            {
                star[i].motion = false;
                star[i].active = true;
                star[i].spawnMotion = 0;
            }
        }
        if (!star[i].active) continue;

        // 1️⃣ 수평 이동
        star[i].x += star[i].vx;

        // 충돌 계산
        int left = star[i].x / TILE_SIZE;
        int right = (star[i].x + TILE_SIZE - 1) / TILE_SIZE;
        int top = star[i].y / TILE_SIZE;
        int bottom = (star[i].y + TILE_SIZE - 1) / TILE_SIZE;
        int middle = (star[i].y + star[i].height / 2 - 1) / TILE_SIZE;
        
        // 2️⃣ 수평 충돌 처리
        if (star[i].vx < 0 && 
            (isSolidTile(currentMap[top][left]) || isSolidTile(currentMap[bottom][left]) || isSolidTile(currentMap[middle][left])))
        {
            star[i].x = (left + 1) * TILE_SIZE; // 경계 맞춤
            star[i].vx = -star[i].vx;      // 방향 반전
        }
        else if (star[i].vx > 0 && 
            (isSolidTile(currentMap[top][right]) || isSolidTile(currentMap[bottom][right]) || isSolidTile(currentMap[middle][right])))
        {
            star[i].x = right * TILE_SIZE - star[i].width; // 경계 맞춤
            star[i].vx = -star[i].vx;                       // 방향 반전
        }

        // 3️⃣ 수직 낙하 (중력)
        star[i].vy += 1;
        if (star[i].vy > 10) star[i].vy = 10;
        star[i].y += star[i].vy;

        left = star[i].x / TILE_SIZE;
        right = (star[i].x + TILE_SIZE - 1) / TILE_SIZE;
        top = star[i].y / TILE_SIZE;
        bottom = (star[i].y + TILE_SIZE - 1) / TILE_SIZE;

        // 위 충돌
        if (star[i].vy < 0 && (isSolidTile(currentMap[top][left]) || isSolidTile(currentMap[top][right])))
        {
            // 마리오가 정통으로 친 블럭 찾기
            int centerX = star[i].x + TILE_SIZE / 2 + g_game.cameraX;
            int blockX = centerX / TILE_SIZE;

            star[i].y = (top + 1) * TILE_SIZE;
            star[i].vy = 0;
        }

        // 아래 충돌 처리
        if (star[i].vy > 0 && (isSolidTile(currentMap[bottom][left]) || isSolidTile(currentMap[bottom][right])))
        {
            star[i].y = bottom * TILE_SIZE - TILE_SIZE; // 위치 보정 (딱 맞게)
            star[i].vy = -15;

            // 땅에 닿았으면 떨림 방지용으로 소폭 위치 보정 (필요시)
            // star[i].x = round(star[i].x / (float)TILE_SIZE) * TILE_SIZE;
        }
    }
}
void UpdateItems_flower()
{

    for (int i = 0; i < MAX_ITEMS; i++)
    {
        // 모션
        if (flower[i].motion)
        {
            flower[i].y -= 2;
            flower[i].spawnMotion++;
            if (flower[i].spawnMotion == 18)
            {
                flower[i].motion = false;
                flower[i].active = true;
                flower[i].spawnMotion = 0;
            }
        }
        if (!flower[i].active) continue;
    }
}
void UpdateItems_tino()
{
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        // 모션
        if (tino[i].motion)
        {
            tino[i].y -= 2;
            tino[i].spawnMotion++;
            if (tino[i].spawnMotion == 18)
            {
                tino[i].motion = false;
                tino[i].active = true;
                tino[i].spawnMotion = 0;
            }
        }
        if (!tino[i].active) continue;
    }
}
static void SpawnItem(Item (&items)[MAX_ITEMS], ItemType type, int x, int y, int vx = 0, int vy = 0)
{
    const bool online = g_game.isMulti && !g_game.isRenderTest;
    if (online && !g_net.isHost) return;

    for (uint8_t slot = 0; slot < MAX_ITEMS; ++slot) {
        Item& item = items[slot];
        if (item.active || item.motion) continue;

        item = Item(x, y, vx, vy, 30, 30, 0, false, true);
        if (online) {
            struct Packet {
                PacketHeader header;
                ItemSpawnData spawn;
            };
            const Packet packet = {
                { sizeof(Packet), PKT_ITEM_SPAWN, 0 },
                { type, slot, 0, ITEM_FLAG_MOTION,
                  static_cast<int16_t>(x), static_cast<int16_t>(y),
                  static_cast<int16_t>(vx), static_cast<int16_t>(vy),
                  static_cast<uint8_t>(item.width), static_cast<uint8_t>(item.height) }
            };
            SendToAll(&packet, sizeof(packet));
        }
        return;
    }
}

void SpawnItem_mushroom(int x, int y)
{
    SpawnItem(mushroom, ITEM_MUSHROOM, x, y, 1);
}

void SpawnItem_up_mushroom(int x, int y)
{
    SpawnItem(up_mushroom, ITEM_UP_MUSHROOM, x, y, 1);
}

void SpawnItem_star(int x, int y)
{
    SpawnItem(star, ITEM_STAR, x, y, 2, -15);
}

void SpawnItem_flower(int x, int y)
{
    SpawnItem(flower, ITEM_FLOWER, x, y);
}

void SpawnItem_tino(int x, int y)
{
    SpawnItem(tino, ITEM_TINO, x, y);
}

void UpdateAllItems()
{
    UpdateItems_mushroom();
    UpdateItems_up_mushroom();
    UpdateItems_star();
    UpdateItems_flower();
    UpdateItems_tino();
}

void HandleItemCollisions()
{
    if (g_game.isMulti && !g_game.isRenderTest) {
        HandleMultiplayerItemCollisions();
        return;
    }

    HandleLocalItemCollisions(mushroom, ITEM_MUSHROOM);
    HandleLocalItemCollisions(up_mushroom, ITEM_UP_MUSHROOM);
    HandleLocalItemCollisions(star, ITEM_STAR);
    HandleLocalItemCollisions(flower, ITEM_FLOWER);
    HandleLocalItemCollisions(tino, ITEM_TINO);
}
