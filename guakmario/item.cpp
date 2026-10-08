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

// 마리오와 버섯 충돌 처리 함수
void HandleMushroomPickup()
{
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        if (!mushroom[i].active) continue;

        int mushroomXWorld = mushroom[i].x - g_game.cameraX; // 화면 출력할 때 cameraX를 뺏으니, 실제 월드 좌표는 더해줘야 함

        if (IsColliding(g_player.mario.x, g_player.mario.y, g_player.mario.width, g_player.mario.height,
            mushroomXWorld, mushroom[i].y, mushroom[i].width, mushroom[i].height))
        {
            PlaySoundBuffer(powerup_Sound);
            mushroom[i].active = false;  // 버섯 먹기
            if (g_player.mario.form == FORM_SMALL && g_game.gameState == GAME_RUNNING)
            {
                g_player.mario.y -= TILE_SIZE + 1;       // 위치 맞추기
                g_game.transformTarget = FORM_BIG;
                g_game.gameState = GAME_TRANSFORMING;
                g_game.transformStartTime = GetTickCount();
            }
            // TODO: 마리오의 상태 변화 (예: 성장, 점수 증가 등)
            // 예) score += 1000;
        }
    }
}
void HandleUpMushroomPickup()
{
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        if (!up_mushroom[i].active) continue;

        int mushroomXWorld = up_mushroom[i].x - g_game.cameraX; // 화면 출력할 때 cameraX를 뺏으니, 실제 월드 좌표는 더해줘야 함

        if (IsColliding(g_player.mario.x, g_player.mario.y, g_player.mario.width, g_player.mario.height,
            mushroomXWorld, up_mushroom[i].y, up_mushroom[i].width, up_mushroom[i].height))
        {
            PlaySoundBuffer(up_Sound);
            up_mushroom[i].active = false;  // 버섯 먹기
            g_player.mario.life++;
        }
    }
}
void HandleFlowerPickup()
{
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        if (!flower[i].active) continue;

        int flowerXWorld = flower[i].x - g_game.cameraX; // 화면 출력할 때 cameraX를 뺏으니, 실제 월드 좌표는 더해줘야 함

        if (IsColliding(g_player.mario.x, g_player.mario.y, g_player.mario.width, g_player.mario.height,
            flowerXWorld, flower[i].y, flower[i].width, flower[i].height))
        {
            PlaySoundBuffer(powerup_Sound);
            flower[i].active = false;  // 먹기
            if (g_player.mario.form != FORM_FLOWER && g_game.gameState == GAME_RUNNING)
            {
                if(g_player.mario.form == FORM_SMALL)
                {
                    g_player.mario.y -= TILE_SIZE + 1;       // 위치 맞추기
                }
                g_game.transformTarget = FORM_FLOWER;
                g_game.gameState = GAME_TRANSFORMING;
                g_game.transformStartTime = GetTickCount();
            }
            // TODO: 마리오의 상태 변화 (예: 성장, 점수 증가 등)
            // 예) score += 1000;
        }
    }
}
void HandleTinoPickup()
{
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        if (!tino[i].active) continue;

        int tinoXWorld = tino[i].x - g_game.cameraX; // 화면 출력할 때 cameraX를 뺏으니, 실제 월드 좌표는 더해줘야 함

        if (IsColliding(g_player.mario.x, g_player.mario.y, g_player.mario.width, g_player.mario.height,
            tinoXWorld, tino[i].y, tino[i].width, tino[i].height))
        {
            PlaySoundBuffer(powerup_Sound);
            tino[i].active = false;  // 먹기
            if (g_player.mario.form != FORM_TINO && g_game.gameState == GAME_RUNNING)
            {
                if (g_player.mario.form == FORM_SMALL)
                {
                    g_player.mario.y -= TILE_SIZE + 1;       // 위치 맞추기
                }
                g_game.transformTarget = FORM_TINO;
                g_game.gameState = GAME_TRANSFORMING;
                g_game.transformStartTime = GetTickCount();
            }
            // TODO: 마리오의 상태 변화 (예: 성장, 점수 증가 등)
            // 예) score += 1000;
        }
    }
}
void HandleStarPickup()
{
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        if (!star[i].active) continue;

        int starXWorld = star[i].x - g_game.cameraX; // 화면 출력할 때 cameraX를 뺏으니, 실제 월드 좌표는 더해줘야 함

        if (IsColliding(g_player.mario.x, g_player.mario.y, g_player.mario.width, g_player.mario.height,
            starXWorld, star[i].y, star[i].width, star[i].height))
        {
            star[i].active = false;  // 먹기
            if (!g_player.mario.star)
            {
                PlayBGM("resource\\sound\\bgm\\InvincibilityTheam.wav");
                g_player.mario.star = true;
                g_game.starStartTime = GetTickCount();
            }
            // TODO: 마리오의 상태 변화 (예: 성장, 점수 증가 등)
            // 예) score += 1000;
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
        int middle = (g_player.mario.y + g_player.mario.height / 2 - 1) / TILE_SIZE;
        
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
void SpawnItem_mushroom(int x, int y)
{
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        if (!mushroom[i].active)
        {
            mushroom[i].x = x;
            mushroom[i].y = y;
            mushroom[i].vx = 1; // 기본 속도
            mushroom[i].motion = true;
            break;
        }
    }
}
void SpawnItem_up_mushroom(int x, int y)
{
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        if (!up_mushroom[i].active)
        {
            up_mushroom[i].x = x;
            up_mushroom[i].y = y;
            up_mushroom[i].vx = 1; // 기본 속도
            up_mushroom[i].motion = true;
            break;
        }
    }
}
void SpawnItem_star(int x, int y)
{
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        if (!star[i].active)
        {
            star[i].x = x;
            star[i].y = y;
            star[i].vx = 2; // 기본 속도
            star[i].vy = -15;
            star[i].motion = true;
            break;
        }
    }
}
void SpawnItem_flower(int x, int y)
{
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        if (!flower[i].active)
        {
            flower[i].x = x;
            flower[i].y = y;
            flower[i].motion = true;
            break;
        }
    }
}
void SpawnItem_tino(int x, int y)
{
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        if (!tino[i].active)
        {
            tino[i].x = x;
            tino[i].y = y;
            tino[i].motion = true;
            break;
        }
    }
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
    HandleMushroomPickup();
    HandleUpMushroomPickup();
    HandleStarPickup();
    HandleFlowerPickup();
    HandleTinoPickup();
}
