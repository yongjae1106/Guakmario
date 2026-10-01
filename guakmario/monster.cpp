#pragma once
#include "data.h"
#include "func.h"
#include "image.h"
#include "monster.h"
#include "sound.h"

Monster monsters[MAX_MONSTERS];
int monsterCount = 18;

Turtle turtles[MAX_TURTLES];
int turtleCount = 5;

AngelTurtle angelTurtles[MAX_TURTLES]; //���� �޸� �ź���
int angelTurtleCount = 10;

Bowser bowser; //���� ĳ����

Fireball fireballs[MAX_FIREBALLS];  //���� �Ҳ�

void InitMonsters()
{
    monsterCount = 18;  // ���� ����

    int tileX[18] = { 15, 28, 35,42,48,54,65,73,76,91,94,98,99,100,101,102,103,104 }; //x��ǥ
    int tileY[18] = { 12, 12, 12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12 };  //y��ǥ

    for (int i = 0; i < monsterCount; i++)
    {
        monsters[i].x = tileX[i] * TILE_SIZE;
        monsters[i].y = tileY[i] * TILE_SIZE;
        monsters[i].vx = -1;  // ������ ���� ����
        monsters[i].vy = 0;
        monsters[i].width = 20;
        monsters[i].height = 10;
        monsters[i].isFalling = false;
        monsters[i].isAlive = true;
        monsters[i].isDead = false;
        monsters[i].active = true;
    }

}
void InitMonsters2()
{
    monsterCount = 10;  // ���� ����

    int tileX[10] = { 30,31,32,35,99,100,101,50,51,52 }; //x��ǥ
    int tileY[10] = { 7,7,7,5,4,4,4,6,6,6 };  //y��ǥ

    for (int i = 0; i < monsterCount; i++)
    {
        monsters[i].x = tileX[i] * TILE_SIZE;
        monsters[i].y = tileY[i] * TILE_SIZE;
        monsters[i].vx = -1;  // ������ ���� ����
        monsters[i].vy = 0;
        monsters[i].width = 20;
        monsters[i].height = 20;
        monsters[i].isFalling = false;
        monsters[i].isAlive = true;
        monsters[i].isDead = false;
        monsters[i].active = true;
    }

}
void InitMonsters3()
{
    monsterCount = 11;  // ���� ����

    int tileX[18] = { 34,35,30,31,32,95,96,97,73,75,76 }; //x��ǥ
    int tileY[18] = { 9,9,9,9,9,9,9,9,9,9,9 };  //y��ǥ

    for (int i = 0; i < monsterCount; i++)
    {
        monsters[i].x = tileX[i] * TILE_SIZE;
        monsters[i].y = tileY[i] * TILE_SIZE;
        monsters[i].vx = -1;  // ������ ���� ����
        monsters[i].vy = 0;
        monsters[i].width = 20;
        monsters[i].height = 20;
        monsters[i].isFalling = false;
        monsters[i].isAlive = true;
        monsters[i].isDead = false;
        monsters[i].active = true;
    }

}

void InitTurtles()
{
    turtleCount = 5;

    int tileX[] = { 10, 33, 35, 50, 65 };
    int tileY[] = { 12, 12, 12, 12, 12 };

    for (int i = 0; i < turtleCount; i++)
    {
        turtles[i].x = tileX[i] * TILE_SIZE;
        turtles[i].y = tileY[i] * TILE_SIZE;
        turtles[i].vx = -1;
        turtles[i].vy = 0;
        turtles[i].width = 20;
        turtles[i].height = 20;
        turtles[i].direction = 0; // left
        turtles[i].isFalling = false;
        turtles[i].isAlive = true;
        turtles[i].isDead = false;
        turtles[i].active = true;
        turtles[i].turtleState = NORMAL;
        turtles[i].shellTimer = 0;
    }
}
void InitTurtles2()
{
    turtleCount = 5;

    int tileX[] = { 17, 33, 35, 50, 65 };
    int tileY[] = { 5, 12, 12, 12, 12 };

    for (int i = 0; i < turtleCount; i++)
    {
        turtles[i].x = tileX[i] * TILE_SIZE;
        turtles[i].y = tileY[i] * TILE_SIZE;
        turtles[i].vx = -1;
        turtles[i].vy = 0;
        turtles[i].width = 20;
        turtles[i].height = 20;
        turtles[i].direction = 0; // left
        turtles[i].isFalling = false;
        turtles[i].isAlive = true;
        turtles[i].isDead = false;
        turtles[i].active = true;
        turtles[i].turtleState = NORMAL;
        turtles[i].shellTimer = 0;
    }
}
void InitTurtles3()
{
    turtleCount = 1;

    int tileX[] = { 60 };
    int tileY[] = { 9 };

    for (int i = 0; i < turtleCount; i++)
    {
        turtles[i].x = tileX[i] * TILE_SIZE;
        turtles[i].y = tileY[i] * TILE_SIZE;
        turtles[i].vx = -1;
        turtles[i].vy = 0;
        turtles[i].width = 20;
        turtles[i].height = 20;
        turtles[i].isFalling = false;
        turtles[i].isAlive = true;
        turtles[i].isDead = false;
        turtles[i].active = true;
        turtles[i].turtleState = NORMAL;
        turtles[i].shellTimer = 0;
    }
}
void InitAngelTurtles()
{
    if (stage != 2) return;
    angelTurtleCount = 2;

    int tileX[] = { 21, 28 };
    int tileY[] = { 5, 5 };

    for (int i = 0; i < angelTurtleCount; i++) 
    {
        angelTurtles[i].x = tileX[i] * TILE_SIZE;
        angelTurtles[i].y = tileY[i] * TILE_SIZE;
        angelTurtles[i].width = 40;
        angelTurtles[i].height = 50;
        angelTurtles[i].vy = 1;
        angelTurtles[i].topY = angelTurtles[i].y - 30;  //���� ����
        angelTurtles[i].bottomY = angelTurtles[i].y + 30;  //���� ����
        angelTurtles[i].goingUp = false;
        angelTurtles[i].isAlive = true;
        angelTurtles[i].isDead = false;
        angelTurtles[i].state = FLYING;
    }
}

void InitBowser() 
{
    bowser.hp = 100;
    bowser.x = 125 * TILE_SIZE;  // ������ ��ġ�� ���� ����
    bowser.y = 9 * TILE_SIZE;   // �� ���� �����ǵ���
    bowser.vx = 0;
    bowser.vy = 0;
    bowser.width = 120;
    bowser.height = 120;
    bowser.isAlive = true;
    bowser.isDead = false;
    bowser.isJumping = false;
    bowser.jumpTimer = 0;

    bowser.startX = bowser.x;
    bowser.direction = 1;
    bowser.moveDistance = 0;
    bowser.maxDistance = 10 * TILE_SIZE;  // 6ĭ
    bowser.jumpTimer = 0;
    bowser.jumpInterval = 60 + rand() % 121;  // 60~180������

    bowser.jumpTimer = 0;
    bowser.jumpInterval = 60 + rand() % 121;

    bowser.fireTimer = 0;
    bowser.fireInterval = 60 + rand() % 60; // 1~2�� ����

    bowser.isFiring = false;
    bowser.fireDuration = 0;
}

//monster
//�浹ó��
void UpdateMonsters()
{
    for (int i = 0; i < monsterCount; i++)
    {
        if (!monsters[i].isAlive) continue;

        if (monsters[i].isFalling)
        {
            monsters[i].x += monsters[i].vx;
            monsters[i].vy += 1;
            if (monsters[i].vy > 10) monsters[i].vy = 10;
            monsters[i].y += monsters[i].vy;
            continue;
        }
        
        // ȭ�� �ȿ� ���� ���� ó��
        int screenX = monsters[i].x - cameraX;
        if (screenX + TILE_SIZE < 0 || screenX > SCREEN_WIDTH) continue;




        // ȭ�� ���� ó��
        if (stage > 1)
        {
            int nextX = monsters[i].x + monsters[i].vx;
            int footX = (monsters[i].vx < 0) ? nextX + 5 : nextX + monsters[i].width;
            int footTileX = footX / TILE_SIZE;
            int footTileY = monsters[i].y / TILE_SIZE + 1;

            bool willFall = true;
            if (footTileX >= 0 && footTileX < MAP_WIDTH && footTileY >= 0 && footTileY < MAP_HEIGHT)
            {
                int tile = currentMap[footTileY][footTileX];
                if (tile != 0 && tile != 2)
                    willFall = false;
            }

            if (willFall)
            {
                monsters[i].vx = -monsters[i].vx;
            }
        }
        monsters[i].x += monsters[i].vx;


        // Ÿ�� ��ǥ ���
        int left = monsters[i].x / TILE_SIZE;
        int right = (monsters[i].x + TILE_SIZE - 1) / TILE_SIZE;
        int top = monsters[i].y / TILE_SIZE;
        int bottom = (monsters[i].y + TILE_SIZE - 1) / TILE_SIZE;
        int middle = (monsters[i].y + monsters[i].height / 2 - 1) / TILE_SIZE;

        // �� ���̰ų� ���� �ε����� ���� ����
        if (monsters[i].vx < 0 &&
            (isSolidTile(currentMap[top][left]) || isSolidTile(currentMap[bottom][left]) || isSolidTile(currentMap[middle][left])))
        {
            monsters[i].x = (left + 1) * TILE_SIZE; // ��� ����
            monsters[i].vx = -monsters[i].vx;      // ���� ����
        }
        else if (monsters[i].vx > 0 &&
            (isSolidTile(currentMap[top][right]) || isSolidTile(currentMap[bottom][right]) || isSolidTile(currentMap[middle][right])))
        {
            monsters[i].x = right * TILE_SIZE - monsters[i].width; // ��� ����
            monsters[i].vx = -monsters[i].vx;                       // ���� ����
        }

        //��ĭ�̸� ��������
        int underX = (monsters[i].x + TILE_SIZE / 2) / TILE_SIZE;
        int underY = (monsters[i].y + TILE_SIZE) / TILE_SIZE;

        if (underX < 0 || underX >= MAP_WIDTH || underY >= MAP_HEIGHT || currentMap[underY][underX] == TILE_EMPTY)
        {
            monsters[i].vy += 1;
            if (monsters[i].vy > 10) monsters[i].vy = 10;
            monsters[i].y += monsters[i].vy;
            monsters[i].isFalling = true;
        }
        else
        {
            monsters[i].vy = 0;
            monsters[i].isFalling = false;
        }

        // ������ ���
        int marioLeft = mario.x + cameraX;
        int marioRight = marioLeft + mario.width;
        int marioBottom = mario.y + mario.height;

        int monsterLeft = monsters[i].x;
        int monsterRight = monsterLeft + monsters[i].width;
        int monsterTop = monsters[i].y;

        if (mario.vy > 0 &&
            marioBottom >= monsterTop &&
            marioBottom <= monsterTop + 10 &&
            marioRight > monsterLeft &&
            marioLeft < monsterRight)
        {

            PlaySoundBuffer(stomp_Sound);
            monsters[i].isAlive = false;
            mario.vy = -10;  // ������ Ƣ���
        }
    }
}
void CheckMarioMonsterCollision()
{
    for (int i = 0; i < monsterCount; i++)
    {
        if (!monsters[i].isAlive || monsters[i].isDead) continue;

        int marioLeft = mario.x;
        int marioRight = mario.x + mario.width;
        int marioTop = mario.y;
        int marioBottom = mario.y + mario.height;

        int monsterLeft = monsters[i].x - cameraX;
        int monsterRight = monsterLeft + monsters[i].width;
        int monsterTop = monsters[i].y;
        int monsterBottom = monsterTop + monsters[i].height;
        int monsterXWorld = monsters[i].x - cameraX; // ȭ�� ����� �� cameraX�� ������, ���� ���� ��ǥ�� ������� ��

        // ���� ���
        if (marioBottom >= monsterTop && marioTop < monsterTop &&
            marioRight > monsterLeft && marioLeft < monsterRight &&
            mario.vy > 0)
        {
            PlaySoundBuffer(stomp_Sound);
            monsters[i].isAlive = false;
            mario.vy = -10;
        }
        // �ε������
        else if (IsColliding(mario.x, mario.y, mario.width, mario.height,
            monsters[i].x - cameraX, monsters[i].y + 20, monsters[i].width, monsters[i].height) &&
            !monsters[i].isDead &&
            monsters[i].active)
        {
            // �� ������ ���� ƨ��Ը� ó��
            if (mario.star)
            {
                PlaySoundBuffer(kick_Sound);
                monsters[i].vy = -15;
                monsters[i].isDead = true;
                monsters[i].isFalling = true;
            }
            else
            {
                damage_mario();
            }
        }
    }
}

//turtle
void UpdateTurtles()
{
    for (int i = 0; i < turtleCount; i++)
    {
        if (!turtles[i].isAlive) continue;
        if (turtles[i].isFalling && turtles[i].turtleState == SPINNING)
        {
            turtles[i].x += turtles[i].vx;
            // �߷� ����
            int underX = (turtles[i].x + TILE_SIZE / 2) / TILE_SIZE;
            int underY = (turtles[i].y + TILE_SIZE) / TILE_SIZE;

            if (underX < 0 || underX >= MAP_WIDTH || underY >= MAP_HEIGHT || currentMap[underY][underX] == TILE_EMPTY)
            {
                turtles[i].vy += 1;
                if (turtles[i].vy > 10) turtles[i].vy = 10;
                turtles[i].y += turtles[i].vy;

                turtles[i].isFalling = true;
            }
            else
            {
                turtles[i].vy = 0;
                turtles[i].isFalling = false;
            }
            if (turtles[i].y > SCREEN_HEIGHT)
                turtles[i].isAlive = false;

            continue;
        }
        else if (turtles[i].isFalling)
        {
            turtles[i].x += turtles[i].vx;
            turtles[i].vy += 1;
            if (turtles[i].vy > 10) turtles[i].vy = 10;
            turtles[i].y += turtles[i].vy;
            continue;
        }
        turtles[i].direction = turtles[i].vx > 0 ? 1 : 0;

        // �������� �ȶ�����
        if(stage > 1 && turtles[i].turtleState == NORMAL)
        {
            int nextX = turtles[i].x + turtles[i].vx;
            int footX = (turtles[i].vx < 0) ? nextX : nextX + turtles[i].width;
            int footTileX = footX / TILE_SIZE;
            int footTileY = turtles[i].y / TILE_SIZE + 1;

            bool willFall = true;
            if (footTileX >= 0 && footTileX < MAP_WIDTH && footTileY >= 0 && footTileY < MAP_HEIGHT)
            {
                int tile = currentMap[footTileY][footTileX];
                if (tile != 0 && tile != 2)
                    willFall = false;
            }

            if (willFall)
            {
                turtles[i].vx = -turtles[i].vx;
            }
        }
        // �̵�
        turtles[i].x += turtles[i].vx;

        // �߷� ����
        int underX = (turtles[i].x + TILE_SIZE / 2) / TILE_SIZE;
        int underY = (turtles[i].y + TILE_SIZE) / TILE_SIZE;

        if (underX < 0 || underX >= MAP_WIDTH || underY >= MAP_HEIGHT || currentMap[underY][underX] == TILE_EMPTY)
        {
            turtles[i].vy += 1;
            if (turtles[i].vy > 10) turtles[i].vy = 10;
            turtles[i].y += turtles[i].vy;

            turtles[i].isFalling = true;
            continue;
        }
        else
        {
            turtles[i].vy = 0;
            turtles[i].isFalling = false;
        }


        // �� �浹
        int left = turtles[i].x / TILE_SIZE;
        int right = (turtles[i].x + TILE_SIZE - 1) / TILE_SIZE;
        int top = turtles[i].y / TILE_SIZE;
        int bottom = (turtles[i].y + TILE_SIZE - 1) / TILE_SIZE;
        int middle = (turtles[i].y + turtles[i].height / 2 - 1) / TILE_SIZE;

        if (turtles[i].vx < 0 &&
            (isSolidTile(currentMap[top][left]) || isSolidTile(currentMap[bottom][left]) || isSolidTile(currentMap[middle][left])))
        {
            turtles[i].x = (left + 1) * TILE_SIZE;
            turtles[i].vx = -turtles[i].vx;
        }
        else if (turtles[i].vx > 0 &&
            (isSolidTile(currentMap[top][right]) || isSolidTile(currentMap[bottom][right]) || isSolidTile(currentMap[middle][right])))
        {
            turtles[i].x = right * TILE_SIZE - turtles[i].width;
            turtles[i].vx = -turtles[i].vx;
        }

        // �� ���� ���� �ð�
        if (turtles[i].turtleState == SHELL &&
            GetTickCount() - turtles[i].shellTimer > 5000)
        {
            turtles[i].turtleState = NORMAL;
            turtles[i].vx = -1;
        }
        // spinning �����϶� ������ ��� ����� ������
        if (turtles[i].turtleState == SPINNING && !turtles[i].damage &&
            GetTickCount() - turtles[i].damageTimer > 100)
        {
            turtles[i].damage = true;
        }

        // ȸ�� ��� ���� ó��
        if (turtles[i].turtleState == SPINNING)
        {
            // (1) ���� ���� ó��
            for (int j = 0; j < monsterCount; j++)
            {
                if (!monsters[j].isAlive || monsters[j].isFalling) continue;

                if (IsColliding(turtles[i].x, turtles[i].y, turtles[i].width, turtles[i].height,
                    monsters[j].x, monsters[j].y, monsters[j].width, monsters[j].height))
                {
                    PlaySoundBuffer(kick_Sound);
                    monsters[j].isFalling = true;
                    monsters[j].vy = -8;
                    monsters[j].vx = (turtles[i].vx > 0) ? 2 : -2;
                }
            }

            // (2) �ȴ� �ź��� ���� ó��
            for (int j = 0; j < turtleCount; j++)
            {
                if (i == j) continue;
                if (!turtles[j].isAlive || turtles[j].isFalling) continue;
                if (turtles[j].turtleState != NORMAL) continue;

                if (IsColliding(turtles[i].x, turtles[i].y, turtles[i].width, turtles[i].height,
                    turtles[j].x, turtles[j].y, turtles[j].width, turtles[j].height))
                {
                    PlaySoundBuffer(kick_Sound);
                    turtles[j].isFalling = true;
                    turtles[j].vy = -8;
                    turtles[j].vx = (turtles[i].vx > 0) ? 2 : -2;
                }
            }

            // (3) ���� �ε����� ���� ����
            if (turtles[i].vx < 0 &&
                (isSolidTile(currentMap[top][left]) || isSolidTile(currentMap[bottom][left]) || isSolidTile(currentMap[middle][left])))
            {
                turtles[i].x = (left + 1) * TILE_SIZE;
                turtles[i].vx = -turtles[i].vx;
            }
            else if (turtles[i].vx > 0 &&
                (isSolidTile(currentMap[top][right]) || isSolidTile(currentMap[bottom][right]) || isSolidTile(currentMap[middle][right])))
            {
                turtles[i].x = right * TILE_SIZE - turtles[i].width;
                turtles[i].vx = -turtles[i].vx;
            }
        }
    }
}
void CheckMarioTurtleCollision()
{
    for (int i = 0; i < turtleCount; i++)
    {
        if (!turtles[i].isAlive) continue;

        int marioLeft = mario.x;
        int marioRight = mario.x + mario.width;
        int marioTop = mario.y;
        int marioBottom = mario.y + mario.height;

        int turtleLeft = turtles[i].x - 10 - cameraX;
        int turtleRight = turtleLeft + turtles[i].width + 20;
        int turtleTop = turtles[i].y;
        int turtleBottom = turtleTop + turtles[i].height;

        if (marioBottom >= turtleTop && marioTop < turtleTop &&
            marioRight > turtleLeft && marioLeft < turtleRight &&
            mario.vy > 0)
        {
            if (turtles[i].turtleState == NORMAL) // ����� ��
            {
                PlaySoundBuffer(stomp_Sound);
                turtles[i].turtleState = SHELL;
                turtles[i].vx = 0;
                turtles[i].shellTimer = GetTickCount();
            }
            else if (turtles[i].turtleState == SHELL) // ����λ��¸� �������
            {
                PlaySoundBuffer(kick_Sound);
                int marioCenter = mario.x + mario.width / 2;
                int turtleCenter = (turtles[i].x - cameraX) + turtles[i].width / 2;

                turtles[i].turtleState = SPINNING;
                turtles[i].vx = (marioCenter < turtleCenter) ? 8 : -8;
            }
            else if (turtles[i].turtleState == SPINNING) // spinning ������ ���¸� ����� ��
            {
                PlaySoundBuffer(stomp_Sound);
                turtles[i].turtleState = SHELL;
                turtles[i].vx = 0;
            }

            mario.vy = -10;  // �ݵ� ����
        }

        else if (IsColliding(mario.x, mario.y, mario.width, mario.height,
            turtleLeft, turtles[i].y + 20, turtles[i].width, turtles[i].height) &&
            !turtles[i].isDead &&
            turtles[i].active)
        {
            if (turtles[i].turtleState == SPINNING) // �¿��浹
            {
                if(turtles[i].damage) damage_mario();
            }
            else if (turtles[i].turtleState == NORMAL) 
            {
                // �� ������ ���� ƨ��Ը� ó��
                if (mario.star)
                {
                    PlaySoundBuffer(kick_Sound);
                    turtles[i].vy = -15;
                    turtles[i].isDead = true;
                    turtles[i].isFalling = true;
                }
                else
                {
                    damage_mario();
                }
            }
            else if (turtles[i].turtleState == SHELL)
            {
                if (mario.star)
                {
                    PlaySoundBuffer(kick_Sound);
                    turtles[i].vy = -15;
                    turtles[i].isDead = true;
                    turtles[i].isFalling = true;
                }
                else
                {
                    PlaySoundBuffer(kick_Sound);
                    turtles[i].damage = false;
                    turtles[i].damageTimer = GetTickCount();
                    int marioCenter = mario.x + mario.width / 2;
                    int turtleCenter = (turtles[i].x - cameraX) + turtles[i].width / 2;

                    turtles[i].turtleState = SPINNING;
                    turtles[i].vx = (marioCenter < turtleCenter) ? 8 : -8;
                }
            }
        }
    }
}

//�����޸� �ź���

void UpdateAngelTurtles()
{
    for (int i = 0; i < angelTurtleCount; i++) 
    {
        if (!angelTurtles[i].isAlive) continue;

        if (angelTurtles[i].state == FLYING)
        {
            if (angelTurtles[i].goingUp)
                angelTurtles[i].y -= angelTurtles[i].vy;
            else
                angelTurtles[i].y += angelTurtles[i].vy;

            if (angelTurtles[i].y <= angelTurtles[i].topY)
                angelTurtles[i].goingUp = false;
            else if (angelTurtles[i].y >= angelTurtles[i].bottomY)
                angelTurtles[i].goingUp = true;
        }
        else if (angelTurtles[i].state == HIDE)
        {
            // �߷� ����
            angelTurtles[i].vy += 1;
            if (angelTurtles[i].vy > 10) angelTurtles[i].vy = 10;
            angelTurtles[i].y += angelTurtles[i].vy;

            // ȭ�� �Ʒ��� �������� ����
            if (angelTurtles[i].y > SCREEN_HEIGHT)
                angelTurtles[i].isAlive = false;
        }
    }
}
void CheckMarioAngelTurtleCollision()
{
    for (int i = 0; i < angelTurtleCount; i++)
    {
        if (!angelTurtles[i].isAlive) continue;

        int marioLeft = mario.x;
        int marioRight = mario.x + mario.width;
        int marioTop = mario.y;
        int marioBottom = mario.y + mario.height;

        int turtleLeft = angelTurtles[i].x - cameraX;
        int turtleRight = turtleLeft + angelTurtles[i].width;
        int turtleTop = angelTurtles[i].y;
        int turtleBottom = turtleTop + angelTurtles[i].height;

        if (mario.vy > 0 &&
            marioBottom >= turtleTop &&
            marioBottom <= turtleTop + 10 &&
            marioRight > turtleLeft &&
            marioLeft < turtleRight) {

            PlaySoundBuffer(stomp_Sound);
            angelTurtles[i].state = HIDE;
            angelTurtles[i].vy = 2;
            angelTurtles[i].hideStartTime = GetTickCount();

            mario.vy = -15;
        }
        else if (IsColliding(mario.x, mario.y, mario.width, mario.height,
                turtleLeft, angelTurtles[i].y, angelTurtles[i].width, angelTurtles[i].height) &&
                angelTurtles[i].isAlive)
        {
            if (!mario.god && !mario.star) 
            {  // ���� ���� �ƴ� ���� ����
                damage_mario();
            }
        }
    }
}

//bowser
void UpdateBowser() 
{
    if (!bowser.isAlive) return;

    // === �̵� ===
    int moveSpeed = 2;
    bowser.x += moveSpeed * bowser.direction;
    bowser.moveDistance += moveSpeed;

    if (bowser.moveDistance >= bowser.maxDistance)
    {
        // ���� ��ȯ
        bowser.direction *= -1;
        bowser.moveDistance = 0;
    }

    // === �ұ�Ģ ���� ===
    bowser.jumpTimer++;
    if (bowser.jumpTimer > bowser.jumpInterval)
    {
        bowser.vy = -18;
        bowser.isJumping = true;
        bowser.jumpTimer = 0;
        bowser.jumpInterval = 60 + rand() % 121;
    }

    // === �߷� ���� ===
    bowser.vy += 1;
    if (bowser.vy > 10) bowser.vy = 10;
    bowser.y += bowser.vy;

    // === �ٴ� �浹 ===
    int tileX = (bowser.x + bowser.width / 2) / TILE_SIZE;
    int tileY = (bowser.y + bowser.height) / TILE_SIZE;
    if (currentMap[tileY][tileX] != 0 && !bowser.isFalling) 
    {
        bowser.y = tileY * TILE_SIZE - bowser.height;
        bowser.vy = 0;
        bowser.isJumping = false;
    }

    bowser.fireTimer++;
    if (bowser.fireTimer > bowser.fireInterval) 
    {
        for (int i = 0; i < MAX_FIREBALLS; i++) 
        {
            if (!fireballs[i].active)
            {
                // ������ ��ġ �������� ���� ����
                int dir = (mario.x + mario.width / 2 < bowser.x + bowser.width / 2) ? -1 : 1;

                fireballs[i].x = bowser.x + bowser.width / 2;
                fireballs[i].y = bowser.y + bowser.height / 2;
                fireballs[i].width = 40;
                fireballs[i].height = 20;
                fireballs[i].vx = 6 * dir;
                fireballs[i].vy = 0;
                fireballs[i].active = true;

                bowser.isFiring = true;
                bowser.fireDuration = 30;
                break;
            }
        }

        bowser.fireTimer = 0;
        bowser.fireInterval = 120 + rand() % 120;
    }


    // �һձ� ���� ���� �ð� ����
    if (bowser.isFiring)
    {
        bowser.fireDuration--;
        if (bowser.fireDuration <= 0)
            bowser.isFiring = false;
    }
    if (bowser.hp <= 0)
    {
        PlaySoundBuffer(bowserfalls_Sound);
        bowser.isFalling = true;
    }
    // �������
    if (bowser.y > SCREEN_HEIGHT)
    {
        PlaySoundBuffer(bowserdead_Sound);
        bowser.isAlive = false;
        for (int i = 137; i < MAP_WIDTH; i++)
        {
            for (int j = 0; j < MAP_HEIGHT; j++)
            {
                if (currentMap[j][i] == TILE_KOOPA_BLOCK)
                {
                    currentMap[j][i] = 0;
                }
            }
        }
    }

}
void UpdateFireballs() 
{
    for (int i = 0; i < MAX_FIREBALLS; i++) 
    {
        if (!fireballs[i].active) continue;
        fireballs[i].motion++;
        if (fireballs[i].motion > 6)
        {
            fireballs[i].motion = 0;
        }
        fireballs[i].x += fireballs[i].vx;

        // ���� ���ؿ��� ī�޶� ������ ������ ��Ȱ��ȭ
        if (fireballs[i].x < cameraX || fireballs[i].x > cameraX + SCREEN_WIDTH)
            fireballs[i].active = false;
    }
}

//�������� ���� �浹ó��

void CheckMarioBowserCollision()
{
    if (!bowser.isAlive) return;

    int bowserX = bowser.x - cameraX;

    if (IsColliding(mario.x, mario.y, mario.width, mario.height,
        bowserX, bowser.y, bowser.width, bowser.height) &&
        !bowser.isDead) 
    {
        // �� ������ ���� ƨ��Ը� ó��
        if (mario.star)
        {
            PlaySoundBuffer(kick_Sound);
            bowser.vy = -15;
            bowser.isDead = true;
            bowser.isFalling = true;
        }
        else
        {
            damage_mario();
        }
    }
}
void CheckMarioFireballCollision() 
{
    if (mario.star || mario.isDead || mario.god) return;

    for (int i = 0; i < MAX_FIREBALLS; i++) 
    {
        if (!fireballs[i].active) continue;

        // fireball�� ���� ��ǥ, mario�� ���� ��ǥ �������� ���
        int fireballX = fireballs[i].x - cameraX;
        
        if (IsColliding(mario.x, mario.y, mario.width, mario.height,
            fireballX, fireballs[i].y, fireballs[i].width, fireballs[i].height))
        {
            damage_mario();  // ������ ����
            return;
        }
    }
}


void damage_mario()
{
    if (mario.isBig)
    {
        // �׳� �۾����� ����
        PlaySoundBuffer(powerdown_Sound);
        mario.y += TILE_SIZE + 1;       // ��ġ ���߱�
        gameState = GAME_TRANSFORMING;
        transformStartTime = GetTickCount();
        // �ݵ� ���� �ϱ� ���� vy�� �״�� �����ϰų� 0����
    }
    else if (mario.flower)
    {
        PlaySoundBuffer(powerdown_Sound);
        gameState = GAME_FLOWER_TRANS;
        transformStartTime = GetTickCount();
        // �ݵ� ���� �ϱ� ���� vy�� �״�� �����ϰų� 0����
    }
    else if (mario.tino)
    {
        PlaySoundBuffer(powerdown_Sound);
        gameState = GAME_TINO_TRANS;
        transformStartTime = GetTickCount();
        // �ݵ� ���� �ϱ� ���� vy�� �״�� �����ϰų� 0����
    }
    else
    {
        // ���� ��� ����
        dead();
    }
}