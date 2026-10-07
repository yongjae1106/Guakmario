// 마리오 스타일 WinAPI 엔진 (카메라 포함, Y 고정)
#define _CRT_SECURE_NO_WARNINGS
#include <tchar.h>
#include "data.h"
#include "map.h"
#include "renderer.h"
#include "player.h"
#include "collision.h"
#include "game.h"
#include "image.h"
#include "item.h"
#include "monster.h"
#include "sound.h"
#include "input.h"
#include "resource1.h"

using namespace Gdiplus;

// 타이머 ID
constexpr UINT TIMER_GAME  = 1;   // 게임 업데이트 (물리, 충돌) — 15ms
constexpr UINT TIMER_WALK  = 2;   // 걷기 애니메이션              — 60ms
constexpr UINT TIMER_BLOCK = 3;   // 블록/별 애니메이션           — 120ms
constexpr UINT TIMER_TIME  = 4;   // 스테이지 타임 카운트다운     — 500ms
constexpr UINT TIMER_COOL  = 5;   // 스킬 쿨타임                  — 1000ms

// 윈도우 핸들
HWND hWnd;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (!g_game.gameStart)
    {
        switch (message)
        {
        case WM_CREATE:
        {
            SetTimer(hWnd, TIMER_GAME,  15,   NULL);    // 게임 업데이트 (물리, 충돌)
            SetTimer(hWnd, TIMER_WALK,  60,   NULL);    // 걷기 애니메이션
            SetTimer(hWnd, TIMER_BLOCK, 120,  NULL);    // 블록/별 애니메이션
            SetTimer(hWnd, TIMER_TIME,  500,  NULL);    // 스테이지 타임 카운트다운
            SetTimer(hWnd, TIMER_COOL,  1000, NULL);    // 스킬 쿨타임
            break;
        }
        case WM_KEYDOWN:
        {
            switch (wParam)
            {
            case VK_UP:
            {
                if (g_game.title_select == 1)
                {
                    g_game.title_select = 0;
                }
                break;
            }
            case VK_DOWN:
            {
                if (g_game.title_select == 0)
                {
                    g_game.title_select = 1;
                }
                break;
            }
            case VK_RETURN:
            {
                if (g_game.title_select == 0)
                {
                    mario_DC = CreateCompatibleDC(g_memDC);

                    LoadStage();

                    g_game.gameStart = true;
                }
                else if (g_game.title_select == 1)
                {
                    exit(1);
                }
            }
            }
            InvalidateRect(hWnd, NULL, FALSE);
            break;
        }
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            // 이미지 로드
            Graphics graphics(g_memDC);
            graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);

            RECT rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
            FillRect(g_memDC, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));  // 배경 클리어

            graphics.DrawImage(g_images.title_screen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);    // 타이틀

            // 커서
            if (g_game.title_select == 0)
            {
                graphics.DrawImage(g_images.title_cursor, 220, 380, 25, 25);
            }
            else if (g_game.title_select == 1)
            {
                graphics.DrawImage(g_images.title_cursor, 220, 420, 25, 25);
            }

            Draw_information();


            BitBlt(hdc, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, g_memDC, 0, 0, SRCCOPY);

            EndPaint(hWnd, &ps);
            break;
        }

        case WM_DESTROY:
        {
            RemoveFontResourceEx(L"SuperMarioBrosNES.ttf", FR_PRIVATE, 0);
            GdiplusShutdown(gdiplusToken);
            PostQuitMessage(0);
            break;
        }

        }
    }
    else
    {
        switch (message)
        {
        case WM_CREATE:
        {
            break;
        }

        case WM_TIMER:
        {
            switch (wParam)
            {
            case TIMER_GAME:
            {
                UpdateGame();
                break;
            }
            case TIMER_WALK:
            {
                if (g_player.mario.isWalking)
                {
                    if (g_player.mario.walk_motion < 2)
                    {
                        g_player.mario.walk_motion++;
                    }
                    else
                    {
                        g_player.mario.walk_motion = 0;
                    }
                }
                break;
            }
            case TIMER_BLOCK:
            {
                if (g_game.frameMotion < 6)
                {
                    g_game.frameMotion++;
                }
                else
                {
                    g_game.frameMotion = 0;
                }
                if (g_game.frameMotionStar < 2)
                {
                    g_game.frameMotionStar++;
                }
                else
                {
                    g_game.frameMotionStar = 0;
                }
                break;
            }
            case TIMER_TIME:
            {
                if (g_game.gameState == GAME_OVER || g_game.gameState == GAME_VICTORY || g_game.gameState == GAME_CLEAR) break;
                TickTimer();
                break;
            }
            case TIMER_COOL:
            {
                TickCooldowns();
            }
            }
            InvalidateRect(hWnd, NULL, FALSE);
            break;
        }
        case WM_CHAR:
        {
            HandleGameChar(wParam);
            break;
        }
        case WM_KEYDOWN:
        {
            HandleGameKeyDown(wParam);
            break;
        }
        case WM_KEYUP:
        {
            HandleGameKeyUp(wParam);
            break;
        }
        case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
            case(ID_TRANS_TINO):
            {
                if (g_player.mario.form != FORM_TINO && g_game.gameState == GAME_RUNNING)
                {
                    PlaySoundBuffer(powerup_Sound);
                    if (g_player.mario.form == FORM_SMALL)
                    {
                        g_player.mario.y -= TILE_SIZE + 1;       // 위치 맞추기
                    }
                    g_game.transformTarget = FORM_TINO;
                    g_game.gameState = GAME_TRANSFORMING;
                    g_game.transformStartTime = GetTickCount();
                }
                break;
            }
            case(ID_TRANS_FLOWER):
            {
                if (g_player.mario.form != FORM_FLOWER && g_game.gameState == GAME_RUNNING)
                {
                    PlaySoundBuffer(powerup_Sound);
                    if (g_player.mario.form == FORM_SMALL)
                    {
                        g_player.mario.y -= TILE_SIZE + 1;       // 위치 맞추기
                    }
                    g_game.transformTarget = FORM_FLOWER;
                    g_game.gameState = GAME_TRANSFORMING;
                    g_game.transformStartTime = GetTickCount();
                }
                break;
            }
            case(ID_TRANS_BIG):
            {
                if (g_player.mario.form == FORM_SMALL && g_game.gameState == GAME_RUNNING)
                {
                    PlaySoundBuffer(powerup_Sound);
                    g_player.mario.y -= TILE_SIZE + 1;       // 위치 맞추기
                    g_game.transformTarget = FORM_BIG;
                    g_game.gameState = GAME_TRANSFORMING;
                    g_game.transformStartTime = GetTickCount();
                }
                break;
            }
            case(ID_TRANS_STAR):
            {
                PlayBGM("resource\\sound\\bgm\\InvincibilityTheam.wav");
                g_player.mario.star = true;
                g_game.starStartTime = GetTickCount();
                break;
            }
            case(ID_STAGE_1):
            {
                LoadStage();
                break;
            }
            case(ID_STAGE_2):
            {
                g_game.stage = 2;
                LoadStage();
                break;
            }
            case(ID_STAGE_3):
            {
                g_game.stage = 3;
                LoadStage();
                break;
            }
            }
        break;
        }
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            // 이미지 로드
            Graphics graphics(g_memDC);
            graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);

            // 게임오버
            if (g_player.mario.gameover)
            {
                TCHAR gameover[30] = L"GAME OVER";
                TextOut(g_memDC, 360, 300, gameover, lstrlen(gameover));    // 게임오버 문구

                EndPaint(hWnd, &ps);
                break;
            }
            RECT rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
            FillRect(g_memDC, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));  // 배경 클리어

            Draw();

            Draw_information();

            BitBlt(hdc, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, g_memDC, 0, 0, SRCCOPY);

            EndPaint(hWnd, &ps);
            break;
        }

        case WM_DESTROY:
        {
            RemoveFontResourceEx(L"SuperMarioBrosNES.ttf", FR_PRIVATE, 0);
            KillTimer(hWnd, TIMER_GAME);
            KillTimer(hWnd, TIMER_WALK);
            KillTimer(hWnd, TIMER_BLOCK);
            KillTimer(hWnd, TIMER_TIME);
            GdiplusShutdown(gdiplusToken);
            PostQuitMessage(0);
            break;
        }
        }
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
{
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"MarioClass";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);					// 메뉴이름

    RegisterClass(&wc);
    hWnd = CreateWindow(L"MarioClass", L"SUPERMARIO BROS TINO", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, SCREEN_WIDTH, SCREEN_HEIGHT,
        NULL, NULL, hInstance, NULL);

    // GDI+ 기본설정
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    InitDirectSound(hWnd);
    sound_load();
    image_load();

    // 핸들 설정
    HDC hdc = GetDC(hWnd);
    g_memDC = CreateCompatibleDC(hdc);
    g_memBitmap = CreateCompatibleBitmap(hdc, SCREEN_WIDTH, SCREEN_HEIGHT);
    g_oldBitmap = (HBITMAP)SelectObject(g_memDC, g_memBitmap);
    ReleaseDC(hWnd, hdc);

    // 글꼴
    int loadedFonts = AddFontResourceEx(L"resource\\SuperMarioBrosNES.ttf", FR_PRIVATE, 0);
    if (loadedFonts > 0)
    {
        // 성공적으로 폰트 로드됨!
    }
    else
    {
        MessageBox(NULL, L"SuperMarioBrosNES.ttf 폰트 로드 실패!", L"Error", MB_OK);
        exit(1);
    }
    // 폰트 선택
    HFONT hFont = CreateFont(25, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_SWISS,
        L"Super Mario Bros. NES");
    HFONT hOldFont = (HFONT)SelectObject(g_memDC, hFont);
    SetTextColor(g_memDC, RGB(255, 255, 255));    // 흰색 글꼴
    SetBkMode(g_memDC, TRANSPARENT);    // 투명배경 글꼴

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) 
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}
