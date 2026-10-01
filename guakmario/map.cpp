#include "map.h"
#include "game.h"
#include "monster.h"
#include "player.h"
#include "sound.h"

int map1[MAP_HEIGHT][MAP_WIDTH] = { 0 };
int map2[MAP_HEIGHT][MAP_WIDTH] = { 0 };
int map3[MAP_HEIGHT][MAP_WIDTH] = { 0 };
int (*currentMap)[MAP_WIDTH] = map1;

//3스테이지 파이어트랩 위치 데이터 정의
FireTrapPos fireTrapPositions[] = 
{
    {10, 6, 4},
    {15, 5, 4}, // x, y는 타일 좌표, 세 번째는 타일 수
    {20, 6, 4},
    {28, 6, 4}, // 위에서 아래 방향
    {24, 5, 4}, // 아래에서 위 방향
    {50, 3, 6},
    {52, 4, 6},
    {55, 3, 6},
    {60, 1,8},
    {70, 1, 8},
    {78, 4, 6},
    {79, 4, 6},
    {76,3,6},
    {81,3,6},
    {105,1,4},
    {107,1,4}
};
Hazard fireTraps[MAX_HAZARDS];
int fireTrapCount = 3;
int fireTrapPositionsCount = sizeof(fireTrapPositions) / sizeof(FireTrapPos);  // 총 개수

// 불 타이머 상태 변수
int fireIntervals[MAP_WIDTH] = { 0 };
int fireTimers[MAP_WIDTH] = { 0 };
int fireIntervalsOn[MAP_WIDTH] = { 0 };
int fireIntervalsOff[MAP_WIDTH] = { 0 };
bool fireState[MAP_WIDTH] = { true };

void InitFireTraps() 
{

    fireTrapCount = 0;
    int n = fireTrapPositionsCount;  // 배열 크기 활용

    for (int i = 0; i < n && fireTrapCount < MAX_HAZARDS; i++) {
        fireTraps[fireTrapCount++] = {
            fireTrapPositions[i].tileX * TILE_SIZE + 20,                // x
            fireTrapPositions[i].tileY * TILE_SIZE,                // y
            10,                                             // width
            TILE_SIZE * fireTrapPositions[i].tileCount,            // height
            fireTrapPositions[i].tileCount,                        // tileCount
            true                                                   // active
        };

    }
}

void UpdateFireTraps() 
{
    if (stage != 3) return; // 3스테이지가 아닐 때 동작 안 하게

    for (int x = 0; x < MAP_WIDTH; x++)
    {
        fireTimers[x]++;
        if (fireState[x]) {
            if (fireTimers[x] > fireIntervalsOn[x]) 
            {
                fireState[x] = false;
                fireTimers[x] = 0;
                fireIntervalsOff[x] = 60 + rand() % 121;
            }
        }
        else {
            if (fireTimers[x] > fireIntervalsOff[x]) 
            {
                fireState[x] = true;
                fireTimers[x] = 0;
                fireIntervalsOn[x] = 30 + rand() % 61;
            }
        }
    }

    for (int i = 0; i < fireTrapCount; i++)
    {
        int tileX = fireTraps[i].x / TILE_SIZE;
        fireTraps[i].active = fireState[tileX];
    }
}

void CheckMarioHazardCollision()
{
    if (mario.star || mario.isDead || mario.god || mario.supergod) return;

    RECT rMario = 
    {
        mario.x, mario.y,
        mario.x + mario.width,
        mario.y + mario.height
    };

    for (int i = 0; i < fireTrapCount; i++) 
    {
        if (!fireTraps[i].active) continue;

        RECT rTrap = {
            fireTraps[i].x - cameraX, fireTraps[i].y,
            fireTraps[i].x - cameraX + fireTraps[i].width,
            fireTraps[i].y + fireTraps[i].height
        };

        RECT intersect;
        if (IntersectRect(&intersect, &rMario, &rTrap))
        {
            // 파이어트랩 충돌 감지
            printf("파이어트랩 충돌! [%d] at (%d, %d)\n", i, fireTraps[i].x, fireTraps[i].y);

            dead();
            return;
        }
    }
}

void trap_reset()
{
    for (int i = 0; i < MAX_HAZARDS; i++)
    {
        fireTraps[i].active = false;
    }
}

void InitMap() //맵 1 구조
{
    // 1. 땅
    for (int j = 0; j < MAP_WIDTH; j++)
    {
        map1[MAP_HEIGHT - 2][j] = 1;
        map1[MAP_HEIGHT - 1][j] = 1;
    }
    map1[9][1] = 65;    // 보여주기식 스타박스
    //벽돌
    map1[6][16] = 10;
    map1[6][18] = 10;
    map1[10][15] = 10;
    map1[10][16] = 10;
    map1[10][18] = 10;
    map1[10][19] = 10;
    map1[10][30] = 10;
    map1[10][32] = 10;
    map1[10][75] = 10;
    map1[10][76] = 10;
    map1[10][77] = 10;
    map1[7][77] = 10;
    map1[7][45] = 10;
    map1[7][44] = 10;
    map1[7][46] = 10;
    map1[7][43] = 10;
    map1[7][47] = 10;
    map1[9][49] = 10;
    map1[9][50] = 10;

    // 0. 구멍
    map1[14][21] = 0;
    map1[13][21] = 0;
    map1[14][22] = 0;
    map1[13][22] = 0;
    map1[14][23] = 0;
    map1[13][23] = 0;
    map1[14][45] = 0;
    map1[13][45] = 0;
    map1[14][68] = 0;
    map1[13][68] = 0;
    map1[14][69] = 0;
    map1[13][69] = 0;
    map1[14][70] = 0;
    map1[13][70] = 0;
    map1[14][85] = 0;
    map1[13][85] = 0;
    map1[14][86] = 0;
    map1[13][86] = 0;
    map1[14][87] = 0;
    map1[13][87] = 0;

    // 2. 코인
    map1[12][14] = 2;
    map1[5][17] = 2;
    map1[9][15] = 2;
    map1[9][18] = 2;
    map1[9][76] = 2;
    map1[9][31] = 2;
    map1[10][60] = 2;
    map1[6][43] = 2;
    map1[6][44] = 2;
    map1[6][45] = 2;
    map1[6][78] = 2;

    //4. 파이프
    map1[10][55] = 43; // 좌상단
    map1[10][56] = 42; // 우상단
    map1[11][55] = 41; // 좌하단
    map1[11][56] = 40; // 우하단
    map1[12][55] = 41;
    map1[12][56] = 40;


    map1[11][38] = 43; // 좌하단
    map1[11][39] = 42; // 우하단
    map1[12][38] = 41;
    map1[12][39] = 40;
    map1[13][38] = 41;
    map1[13][39] = 40;
    map1[14][38] = 41;
    map1[14][39] = 40;



    // 5. 계단 
    map1[14][80] = 5;
    map1[13][81] = 5; map1[14][81] = 5;
    map1[12][82] = 5; map1[13][82] = 5; map1[14][82] = 5;
    map1[11][83] = 5; map1[12][83] = 5; map1[13][83] = 5; map1[14][83] = 5;
    map1[10][84] = 5; map1[11][84] = 5; map1[12][84] = 5; map1[13][84] = 5; map1[14][84] = 5;

    map1[14][88] = 5;
    map1[13][88] = 5;
    map1[12][88] = 5;
    map1[11][88] = 5;
    map1[10][88] = 5;
    map1[14][89] = 5;
    map1[13][89] = 5;
    map1[12][89] = 5;
    map1[11][89] = 5;
    map1[14][90] = 5;
    map1[13][90] = 5;
    map1[12][90] = 5;
    map1[14][91] = 5;
    map1[13][91] = 5;
    map1[14][92] = 5;

    // 6. 미스테리 박스
    map1[9][10] = 6;
    map1[6][17] = 64;
    map1[10][17] = 64;
    map1[10][31] = 64;
    map1[10][60] = 64;
    map1[20][76] = 64;
    map1[10][62] = 64;
    map1[10][64] = 64;
    map1[6][62] = 62;
    map1[7][78] = 64;
    map1[7][73] = 64;

    // 깃발
    map1[MAP_HEIGHT - 3][108] = 5;
    // 7. 깃발
    for (int y = 3; y <= 11; y++)
    {
        map1[y][108] = 7;
    }
    // 8. 깃발 꼭짓점
    map1[2][108] = 8;

    // 9. 성
    map1[12][115] = 9;


}

// 맵 2 구조
void InitMap2() 
{
    // 0: 구멍 1: 땅 2: 코인 3:굼바 4:파이프 5:계단 6:미스테리박스 7:깃발 8: 깃발꼭짓점 9:성 10: 벽돌 
    // 90. 버섯머리1 91. 버섯머리2 92. 버섯머리3 11. 버섯줄기 12. 버섯줄기2 13.구름 14: 불기둥스위치 15: 회색벽돌
    // 16: 사용된블럭 17:용암head 18:용암body 60:스타박스 61:꽃박스 62: 티노박스 63: 생명버섯박스 64: 코인박스 999: 피치공주

    currentMap = map2;

    memset(currentMap, 0, sizeof(map2));  //맵 초기화


    // 1. 시작 벽돌(=땅)


    for (int y = 13; y <= 14; y++)
    {
        for (int x = 0; x <= 3; x++)
        {
            map2[y][x] = 1;
        }
    }

    for (int y = 13; y <= 14; y++)
    {
        for (int x = 111; x < 140; x++)
        {
            map2[y][x] = 1;
        }
    }

    // 6. 미스테리 박스
    map2[2][69] = 64;
    map2[2][71] = 6;
    map2[6][75] = 64;
    map2[8][102] = 61;

    //13. 구름
    map2[8][30] = 13; map2[8][31] = 13; map2[8][32] = 13; map2[8][33] = 13;
    map2[6][34] = 13; map2[6][35] = 13; map2[6][36] = 13;
    map2[6][44] = 13; map2[6][45] = 13; map2[6][46] = 13;
    map2[7][50] = 13; map2[7][51] = 13; map2[7][52] = 13; map2[7][53] = 13;
    map2[6][55] = 13; map2[6][56] = 13;
    map2[13][48] = 13; map2[13][49] = 13; map2[13][50] = 13; map2[13][51] = 13; map2[13][52] = 13; map2[13][53] = 13; map2[13][54] = 13; map2[13][55] = 13; map2[13][56] = 13; map2[13][57] = 13;
    map2[8][95] = 13; map2[8][96] = 13; map2[8][97] = 13;
    map2[12][97] = 13; map2[12][98] = 13; map2[12][101] = 13; map2[12][101] = 13; map2[12][102] = 13; map2[12][103] = 13;
    map2[12][104] = 13;
    map2[7][63] = 13; map2[7][64] = 13; map2[7][65] = 13;
    map2[5][68] = 13; map2[5][69] = 13; map2[5][70] = 13; map2[5][71] = 13; map2[5][72] = 13;
    map2[3][77] = 13; map2[3][78] = 13; map2[3][79] = 13;
    map2[12][66] = 13; map2[12][67] = 13; map2[12][68] = 13; map2[12][69] = 13; map2[12][70] = 13; map2[12][71] = 13; map2[12][72] = 13; map2[12][73] = 13; map2[12][74] = 13; map2[12][75] = 13; map2[12][76] = 13;
    map2[5][101] = 13; map2[5][100] = 13; map2[5][99] = 13;
    map2[5][102] = 13;
    map2[5][105] = 13; map2[5][106] = 13; map2[5][107] = 13;
    map2[4][22] = 13; map2[4][23] = 13; map2[4][24] = 13;




    // 9. 버섯 머리
    map2[11][6] = 90; map2[11][7] = 91; map2[11][8] = 92;
    map2[10][8] = 90; map2[10][9] = 91; map2[10][10] = 92;
    map2[6][13] = 90; map2[6][14] = 91; map2[6][15] = 91; map2[6][16] = 91; map2[6][17] = 92;
    map2[11][17] = 90; map2[11][18] = 91; map2[11][19] = 91; map2[11][20] = 92;
    map2[8][24] = 90; map2[8][25] = 91; map2[8][26] = 92;
    map2[4][38] = 90; map2[4][39] = 91; map2[4][40] = 91; map2[4][41] = 91; map2[4][42] = 92;
    map2[10][42] = 90; map2[10][43] = 91; map2[10][44] = 92;
    map2[10][42] = 90; map2[10][43] = 91; map2[10][44] = 92;
    map2[3][58] = 90; map2[3][59] = 91; map2[3][60] = 91; map2[3][61] = 91;  map2[3][62] = 92;
    map2[6][84] = 90; map2[6][85] = 91; map2[6][86] = 92;
    map2[4][90] = 90; map2[4][91] = 91; map2[4][92] = 92;
    map2[9][79] = 90; map2[9][80] = 91; map2[9][81] = 91; map2[9][82] = 91; map2[9][83] = 92;
    map2[10][106] = 90; map2[10][107] = 91; map2[10][108] = 92;

    // 10. 버섯 줄기
    for (int y = 12; y <= 14; y++) map2[y][7] = 11;
    for (int y = 11; y <= 14; y++) map2[y][9] = 11;
    for (int y = 7; y <= 14; y++) map2[y][15] = 11;
    for (int y = 12; y <= 14; y++) map2[y][18] = 11;
    for (int y = 9; y <= 14; y++) map2[y][25] = 11;
    for (int y = 5; y <= 14; y++) map2[y][40] = 11;
    for (int y = 11; y <= 14; y++) map2[y][43] = 11;
    for (int y = 4; y <= 14; y++) map2[y][60] = 11;
    for (int y = 7; y <= 14; y++) map2[y][85] = 11;
    for (int y = 5; y <= 14; y++) map2[y][91] = 11;
    for (int y = 10; y <= 14; y++) map2[y][81] = 11;
    for (int y = 11; y <= 14; y++) map2[y][107] = 11;


    // 2. 코인
    map2[5][13] = 2;
    map2[5][14] = 2;
    map2[5][15] = 2;
    map2[5][16] = 2;
    map2[5][17] = 2;
    map2[6][51] = 2;
    map2[11][68] = 2;
    map2[11][69] = 2;
    map2[11][70] = 2;
    map2[12][48] = 2;
    map2[12][49] = 2;
    map2[12][52] = 2;
    map2[12][53] = 2;
    map2[12][54] = 2;
    map2[11][101] = 2;
    map2[11][102] = 2;
    map2[11][103] = 2;

    map2[9][53] = 63;

    // 깃발
    map2[MAP_HEIGHT - 3][115] = 5;
    // 7. 깃발
    for (int y = 3; y <= 11; y++)
    {
        map2[y][115] = 7;
    }
    // 8. 깃발 꼭짓점
    map2[2][115] = 8;
    
    // 9. 성
    map2[12][121] = 9;

}
//맵 3 구조
void InitMap3()
{
    // 0: 구멍 1: 땅 2: 코인 3:굼바 4:파이프 5:계단 6:미스테리박스 7:깃발 8: 깃발꼭짓점 9:성 10: 벽돌 
    // 90. 버섯머리1 91. 버섯머리2 92. 버섯머리3 11. 버섯줄기 12. 버섯줄기2 13.구름 14: 불기둥스위치 15: 회색벽돌
    // 16: 사용된블럭 17:용암head 18:용암body 19: 쿠파블럭 60:스타박스 61:꽃박스 62: 티노박스 63: 생명버섯박스 64: 코인박스
    currentMap = map3;


    memset(map3, 0, sizeof(map3));  // 초기화


    // 바닥 (전체 회색 벽돌)
    for (int x = 0; x < MAP_WIDTH; x++)
    {
        for (int y = 10; y <= 14; y++)
        {
            map3[y][x] = 15;
        }
    }

    //윗부분
    for (int x = 0; x < MAP_WIDTH; x++)
    {
        for (int y = 0; y <= 0; y++)
        {
            map3[y][x] = 15;
        }
    }

    for (int x = 0; x < 30; x++)
    {
        for (int y = 1; y <= 4; y++)
        {
            map3[y][x] = 15;
        }
    }

    for (int x = 50; x < 56; x++)
    {
        for (int y = 1; y <= 2; y++)
        {
            map3[y][x] = 15;
        }
    }

    for (int x = 76; x < 82; x++)
    {
        for (int y = 1; y <= 2; y++)
        {
            map3[y][x] = 15;
        }
    }


    for (int x = 95; x < 103; x++)
    {
        for (int y = 1; y <= 3; y++)
        {
            map3[y][x] = 15;
        }
    }

    for (int x = 110; x < 115; x++)
    {
        for (int y = 1; y <= 2; y++)
        {
            map3[y][x] = 15;
        }
    }

    for (int y = 0; y <= 14; y++)
    {
        for (int x = 137; x <= 139; x++)
        {
            map3[y][x] = 15;
        }
    }

    map3[7][120] = 61; // 보스전아이템 꽃
    map3[7][121] = 62; // 보스전아이템 티노

    //map3[7][3] = 60;    // 시작지점 테스트 스타
    //map3[7][6] = 62;    // 시작지점 테스트 티노

    //구멍
    for (int y = 10; y <= 14; y++)
    {
        for (int x = 30; x <= 31; x++)
        {
            map3[y][x] = 0;
        }
    }

    for (int y = 10; y <= 14; y++)
    {
        for (int x = 37; x <= 39; x++)
        {
            map3[y][x] = 0;
        }
    }

    for (int y = 10; y <= 14; y++)
    {
        for (int x = 42; x <= 44; x++)
        {
            map3[y][x] = 0;
        }
    }

    for (int y = 10; y <= 12; y++)
    {
        for (int x = 103; x <= 109; x++)
        {
            map3[y][x] = 0;
        }
    }

    for (int y = 5; y <= 7; y++)
    {
        for (int x = 137; x <= 139; x++)
        {
            map3[y][x] = 0;
        }
    }

    for (int x = 116; x <= 136; x++)
    {
        map3[10][x] = 0;
        map3[11][x] = 19;
    }
    for (int y = 12; y <= 14; y++)
    {
        for (int x = 116; x <= 136; x++)
        {
            map3[y][x] = 0;
        }
    }

    map3[6][105] = 15;
    map3[6][106] = 15;
    map3[6][107] = 15;

    // 2. 코인
    map3[3][104] = 2;
    map3[3][106] = 62;   // 난이도 높은 티노
    map3[3][108] = 2;
    map3[8][104] = 2;
    map3[8][106] = 2;
    map3[8][108] = 2;
    map3[6][12] = 2;
    map3[6][13] = 2;
    map3[6][17] = 2;
    map3[6][18] = 2;
    map3[6][21] = 2;
    map3[6][22] = 2;
    map3[6][23] = 2;
    map3[6][26] = 2;
    map3[6][30] = 2;
    map3[6][31] = 2;
    map3[6][37] = 2;
    map3[6][38] = 2;
    map3[6][39] = 2;
    map3[6][42] = 2;
    map3[6][43] = 2;
    map3[6][44] = 2;
    map3[6][56] = 2;
    map3[6][57] = 2;
    map3[6][58] = 2;

    map3[6][62] = 2;
    map3[7][63] = 2;
    map3[6][64] = 2;
    map3[7][65] = 2;
    map3[6][66] = 2;
    map3[7][67] = 2;
    map3[6][68] = 2;

    map3[6][71] = 2;
    map3[7][72] = 2;
    map3[6][73] = 2;
    map3[7][74] = 2;
    map3[6][75] = 2;

    //13. 불구덩이
    for (int i = 12; i <= 14; i++)
    {
        map3[i][30] = 18;
        map3[i][31] = 18;

        map3[i][37] = 18;
        map3[i][38] = 18;
        map3[i][39] = 18;

        map3[i][42] = 18;
        map3[i][43] = 18;
        map3[i][44] = 18;

    }

    map3[11][30] = 17;
    map3[11][31] = 17;

    map3[11][37] = 17;
    map3[11][38] = 17;
    map3[11][39] = 17;

    map3[11][42] = 17;
    map3[11][43] = 17;
    map3[11][44] = 17;


    for (int y = 13; y <= 14; y++)
    {
        for (int x = 116; x <= 136; x++)
        {
            map3[y][x] = 18;
        }
    }

    for (int x = 116; x <= 136; x++)
    {
        map3[12][x] = 17;
    }


    //14 불 나오는 스위치
    map3[5][10] = 14;
    map3[5][20] = 14;
    map3[5][28] = 14;
    map3[9][15] = 14;
    map3[9][24] = 14;
    map3[9][50] = 14;
    map3[3][52] = 14;
    map3[9][55] = 14;
    map3[9][60] = 14;
    map3[9][70] = 14;
    map3[3][78] = 14;
    map3[3][79] = 14;
    map3[9][76] = 14;
    map3[9][81] = 14;
    map3[0][105] = 14;
    map3[0][107] = 14;


    /*스위치에서 나오는 불꽃
    for (int i = 6; i <= 9; i++) {
        map3[i][10] = 20;
    }

    for (int i = 5; i <= 8; i++) {
        map3[i][15] = 20;
    }

    for (int i = 6; i <= 9; i++) {
        map3[i][28] = 20;
    }


    for (int i = 6; i <= 9; i++) {
        map3[i][28] = 20;
    }
    */

    //18. 쿠파블럭 벽
    map3[5][138] = 19;
    map3[6][138] = 19;
    map3[7][138] = 19;

    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        map3[y][155] = 15;
        map3[y][156] = 15;
        map3[y][157] = 15;
        map3[y][158] = 15;
        map3[y][159] = 15;
        map3[y][160] = 15;
        map3[y][161] = 15;
        map3[y][162] = 15;
        map3[y][163] = 15;
    }
    map3[8][147] = 999;
}
