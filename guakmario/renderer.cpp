#include <tchar.h>
#include "network.h"  // winsock2.h를 windows.h보다 먼저 포함
#include "data.h"
#include "image.h"
#include "item.h"
#include "sound.h"
#include "monster.h"
#include "map.h"
#include "player.h"
#include "game.h"
#include "renderer.h"

using namespace Gdiplus;

static void DrawLobbyCharacterIcons(Graphics& graphics);

void DrawLobbyScreen()
{
    Graphics graphics(g_memDC);
    graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);
    // 로비 이미지의 투명 영역에 이전 화면이나 문구가 남지 않도록 타이틀 배경으로 덮는다.
    graphics.DrawImage(g_images.title_screen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    graphics.DrawImage(g_images.title_screen_multi, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    DrawLobbyCharacterIcons(graphics);
    // 클라이언트는 START 버튼이 없으므로 커서를 항상 EXIT에 둔다.
    const bool cursorOnStart = g_net.isHost && g_game.multi_select == 0;
    graphics.DrawImage(g_images.title_cursor, cursorOnStart ? 250 : 400, 475, 25, 25);

    // GDI+ 배경과 커서를 그린 뒤 GDI 텍스트를 출력한다.
    graphics.Flush(FlushIntentionSync);
    const COLORREF previousColor = SetTextColor(g_memDC, RGB(0, 0, 0));
    const int previousBkMode = SetBkMode(g_memDC, TRANSPARENT);

    // 접속 인원은 호스트 자신을 포함한 네트워크 상태를 사용한다.
    TCHAR playerCount[32];
    _stprintf_s(playerCount, _T("PLAYERS %d / 3"), g_net.connectedCount);
    const TCHAR* lobbyStatus;

    if (g_net.isHost)
    {
        lobbyStatus = g_net.connectedCount == 3
            ? _T("READY TO START") : _T("WAITING FOR PLAYERS");
    }
    else
    {
        // 클라이언트는 ACK로 부여된 ID를 기준으로 연결 중과 시작 대기를 구분한다.
        lobbyStatus = g_net.myPlayerId >= 1 && g_net.myPlayerId <= 2
            ? _T("WAITING FOR HOST...") : _T("CONNECTING TO HOST...");
    }

    // 호스트 IP — 호스트는 자기 IP, 클라이언트는 입력한 IP
    TCHAR hostIp[32];
    _stprintf_s(hostIp, _T("%S"), g_net.isHost ? g_net.localIp : g_game.ipInputBuf);
    TextOut(g_memDC, 230, 237, _T("PLAYER 1 (HOST)"), 15);
    TextOut(g_memDC, 230, 262, hostIp, lstrlen(hostIp));

    // PLAYER 2 · 3 접속 현황 — 클라이언트도 호스트가 보낸 EVT_LOBBY_STATUS로 같은 화면을 본다.
    for (int id = 1; id <= 2; id++)
    {
        TCHAR playerLabel[24];
        if (id == g_net.myPlayerId && !g_net.isHost)
            _stprintf_s(playerLabel, _T("PLAYER %d (YOU)"), id + 1);
        else
            _stprintf_s(playerLabel, _T("PLAYER %d"), id + 1);
        TextOut(g_memDC, 230, 323 + (id - 1) * 80, playerLabel, lstrlen(playerLabel));
        const TCHAR* connection = NetIsPlayerConnected(id) ? _T("CONNECTED") : _T("WAITING");
        TextOut(g_memDC, 230, 347 + (id - 1) * 80, connection, lstrlen(connection));
    }

    SetTextColor(g_memDC, RGB(255, 255, 255));
    if (g_net.isHost)
        TextOut(g_memDC, 280, 477, _T("START"), 5);   // 게임 시작은 호스트만
    TextOut(g_memDC, 430, 477, _T("EXIT"), 4);

    // 하단 안내만 작은 글꼴로 출력하고 이후 HUD를 위해 원래 글꼴을 복원한다.
    HFONT infoFont = CreateFont(18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Super Mario Bros. NES"));
    HGDIOBJ previousFont = nullptr;
    if (infoFont) previousFont = SelectObject(g_memDC, infoFont);

    const UINT previousTextAlign = SetTextAlign(g_memDC, TA_CENTER | TA_TOP);
    TextOut(g_memDC, SCREEN_WIDTH / 2, 530, playerCount, lstrlen(playerCount));
    TextOut(g_memDC, SCREEN_WIDTH / 2, 550, lobbyStatus, lstrlen(lobbyStatus));
    SetTextAlign(g_memDC, previousTextAlign);

    if (infoFont)
    {
        SelectObject(g_memDC, previousFont);
        DeleteObject(infoFont);
    }

    // 같은 DC를 사용하는 HUD 출력에 영향을 주지 않도록 텍스트 설정을 복원한다.
    SetBkMode(g_memDC, previousBkMode);
    SetTextColor(g_memDC, previousColor);
}

static void Draw_luigi(const MarioRenderData& p);
static void Draw_toad(const MarioRenderData& p);
static void Draw_spawn_item();
static void Draw_map();
static void Draw_map_background();
static void Draw_castle_blank();
static void Draw_mario(const MarioRenderData& mario);
static void Draw_item();
static void Draw_background();
static void Draw_fireball();
static void Draw_Monsters();
static void Draw_Turtles();
static void Draw_Angel_Turtles();
static void Draw_Bowser();
static void Draw_Fireballs();
static void DrawFireTraps();

static void DrawSprite(Graphics& g, Image* img, int x, int y, int w, int h, bool flipX)
{
    if (flipX)
    {
        Matrix m(-1.0f, 0.0f, 0.0f, 1.0f, (REAL)(x + w), 0.0f);
        g.SetTransform(&m);
        g.DrawImage(img, 0, y, w, h);
        g.ResetTransform();
    }
    else
    {
        g.DrawImage(img, x, y, w, h);
    }
}

// ─────────────────────────────────────────────
//  스프라이트 시트 슬라이싱 헬퍼
// ─────────────────────────────────────────────
static void DrawSheetSprite(Graphics& g, Image* sheet,
    int srcX, int srcY, int srcW, int srcH,
    int dstX, int dstY, int dstW, int dstH,
    bool flipX)
{
    Rect destRect(dstX, dstY, dstW, dstH);
    if (flipX)
    {
        Matrix m(-1.0f, 0.0f, 0.0f, 1.0f, (REAL)(dstX + dstW), 0.0f);
        g.SetTransform(&m);
        Rect flippedDst(0, dstY, dstW, dstH);
        g.DrawImage(sheet, flippedDst, srcX, srcY, srcW, srcH, UnitPixel);
        g.ResetTransform();
    }
    else
    {
        g.DrawImage(sheet, destRect, srcX, srcY, srcW, srcH, UnitPixel);
    }
}

// ─────────────────────────────────────────────
//  Luigi 스프라이트 시트 좌표 (Mario_sprite.png)
//  스캔 결과 기준 실제 좌표 사용
// ─────────────────────────────────────────────
namespace Luigi {
    static const int FRAME_W  = 32;
    static const int X_IDLE   = 1;
    static const int X_WALK1  = 75;
    static const int X_WALK2  = 108;
    static const int X_WALK3  = 141;
    static const int X_JUMP   = 215;
    static const int X_DEAD   = 627;  // Small 행 전용: Dead 칸 (520은 수영 마지막 칸)
    static const int X_ATTACK = 627;  // Fire 행 전용: 불 발사 포즈 (col18)
    static const int SY = 553;  // Small 행 y
    static const int SH = 16;   // Small 프레임 세로
    static const int BY = 583;  // Super/Big 행 y
    static const int BH = 32;   // Super/Big 프레임 세로
    static const int FY = 629;  // Fire(Flower) 행 y
    static const int FH = 32;   // Fire 프레임 세로
    static const int TY = 705;  // Tino 행 y (5번째 행)
    static const int TH = 32;   // Tino 프레임 세로
}

// ─────────────────────────────────────────────
//  Toad 스프라이트 시트 좌표 (Mario_sprite.png)
// ─────────────────────────────────────────────
namespace Toad {
    static const int FRAME_W  = 32;
    static const int X_IDLE   = 1;
    static const int X_WALK1  = 75;
    static const int X_WALK2  = 108;
    static const int X_WALK3  = 141;
    static const int X_JUMP   = 215;
    static const int X_DEAD   = 627;  // Small 행 전용: Dead 칸 (520은 수영 마지막 칸)
    static const int X_ATTACK = 627;  // Fire 행 전용: 불 발사 포즈 (col18)
    static const int SY = 1082; // Small 행 y
    static const int SH = 16;   // Small 프레임 세로
    static const int BY = 1112; // Super/Big 행 y
    static const int BH = 32;   // Super/Big 프레임 세로
    static const int FY = 1158; // Fire(Flower) 행 y
    static const int FH = 32;   // Fire 프레임 세로
    static const int TY = 1334; // Tino 행 y (7번째 행)
    static const int TH = 32;   // Tino 프레임 세로
}

static void Draw_luigi(const MarioRenderData& p)
{
    Image* sheet = g_images.mario_sprite_sheet;
    if (!sheet) return;
    Graphics g(g_memDC);
    g.SetInterpolationMode(InterpolationModeNearestNeighbor);
    const bool flip = (p.direction == DIR_LEFT);

    // tino_motion: 별도 attack 이미지 사용 (mario와 동일한 방식)
    if (p.form == FORM_TINO && p.tino_motion)
    {
        Image* const attackImgs[6] = {
            g_images.tino_luigi_attack_6, g_images.tino_luigi_attack_5,
            g_images.tino_luigi_attack_4, g_images.tino_luigi_attack_3,
            g_images.tino_luigi_attack_2, g_images.tino_luigi_attack_1
        };
        int fi = p.motion_timer / 5;
        if (fi < 0) fi = 0;
        if (fi > 5) fi = 5;
        DrawSprite(g, attackImgs[fi], p.x - 25, p.y - 15, 100, 100, flip);
        return;
    }
    if (p.form == FORM_TINO && p.tino_fire_motion)
    {
        DrawSprite(g, g_images.tino_luigi_attack_2, p.x - 25, p.y - 15, 100, 100, flip);
        return;
    }

    // 행(srcY, srcH) 선택
    int srcY, srcH;
    if (p.isDead || p.form == FORM_SMALL)  // Dead 프레임은 Small 행에만 있음
    {
        srcY = Luigi::SY;
        srcH = Luigi::SH;
    }
    else if (p.form == FORM_FLOWER)
    {
        srcY = Luigi::FY;
        srcH = Luigi::FH;
    }
    else if (p.form == FORM_TINO)
    {
        srcY = Luigi::TY;
        srcH = Luigi::TH;
    }
    else  // FORM_BIG
    {
        srcY = Luigi::BY;
        srcH = Luigi::BH;
    }

    // 열(srcX) 선택
    // 소스 셀은 32px 너비지만 실제 캐릭터 콘텐츠는 중앙 16px에 위치
    // → srcW=16(내용 영역), srcH 그대로 → dstW×dstH 균등 스케일
    static const int CONTENT_W  = 16;  // 실제 콘텐츠 너비
    static const int CONTENT_OX = 8;   // 셀 좌측에서 콘텐츠 시작 오프셋

    int srcX;
    if (p.isDead)
        srcX = Luigi::X_DEAD   + CONTENT_OX;
    else if (p.form == FORM_FLOWER && p.fire_motion)
        srcX = Luigi::X_ATTACK + CONTENT_OX;
    else if (p.isJumping)
        srcX = Luigi::X_JUMP   + CONTENT_OX;
    else if (p.isWalking)
    {
        // 각 프레임 콘텐츠 중심 기준 offset → 목적지에서 중심이 고정됨
        // Walk1: center=15→off=7, Walk2: center=16→off=8, Walk3: center=16→off=8
        const int walkX[3]  = { Luigi::X_WALK1, Luigi::X_WALK2, Luigi::X_WALK3 };
        const int walkOX[3] = { 7, 8, 8 };
        int wi = p.walk_motion % 3;
        srcX = walkX[wi] + walkOX[wi];
    }
    else
        srcX = Luigi::X_IDLE   + CONTENT_OX;

    const int dstW = p.width;
    const int dstH = p.isDead ? TILE_SIZE : p.height;  // 마리오 dead와 동일하게 한 칸 높이

    DrawSheetSprite(g, sheet,
        srcX, srcY, CONTENT_W, srcH,
        p.x, p.y, dstW, dstH,
        flip);
}

static void Draw_toad(const MarioRenderData& p)
{
    Image* sheet = g_images.mario_sprite_sheet;
    if (!sheet) return;
    Graphics g(g_memDC);
    g.SetInterpolationMode(InterpolationModeNearestNeighbor);
    const bool flip = (p.direction == DIR_LEFT);

    // tino_motion: 별도 attack 이미지 사용 (mario와 동일한 방식)
    if (p.form == FORM_TINO && p.tino_motion)
    {
        Image* const attackImgs[6] = {
            g_images.tino_toad_attack_6, g_images.tino_toad_attack_5,
            g_images.tino_toad_attack_4, g_images.tino_toad_attack_3,
            g_images.tino_toad_attack_2, g_images.tino_toad_attack_1
        };
        int fi = p.motion_timer / 5;
        if (fi < 0) fi = 0;
        if (fi > 5) fi = 5;
        DrawSprite(g, attackImgs[fi], p.x - 25, p.y - 15, 100, 100, flip);
        return;
    }
    if (p.form == FORM_TINO && p.tino_fire_motion)
    {
        DrawSprite(g, g_images.tino_toad_attack_2, p.x - 25, p.y - 15, 100, 100, flip);
        return;
    }

    int srcY, srcH;
    if (p.isDead || p.form == FORM_SMALL)  // Dead 프레임은 Small 행에만 있음
    {
        srcY = Toad::SY;
        srcH = Toad::SH;
    }
    else if (p.form == FORM_FLOWER)
    {
        srcY = Toad::FY;
        srcH = Toad::FH;
    }
    else if (p.form == FORM_TINO)
    {
        srcY = Toad::TY;
        srcH = Toad::TH;
    }
    else
    {
        srcY = Toad::BY;
        srcH = Toad::BH;
    }

    static const int CONTENT_W  = 16;
    static const int CONTENT_OX = 8;

    int srcX;
    if (p.isDead)
        srcX = Toad::X_DEAD   + CONTENT_OX;
    else if (p.form == FORM_FLOWER && p.fire_motion)
        srcX = Toad::X_ATTACK + CONTENT_OX;
    else if (p.isJumping)
        srcX = Toad::X_JUMP   + CONTENT_OX;
    else if (p.isWalking)
    {
        // Toad Walk1: center=15→off=7, Walk2: center=15→off=7, Walk3: center=16→off=8
        const int walkX[3]  = { Toad::X_WALK1, Toad::X_WALK2, Toad::X_WALK3 };
        const int walkOX[3] = { 7, 7, 8 };
        int wi = p.walk_motion % 3;
        srcX = walkX[wi] + walkOX[wi];
    }
    else
        srcX = Toad::X_IDLE   + CONTENT_OX;

    const int dstW = p.width;
    const int dstH = p.isDead ? TILE_SIZE : p.height;  // 마리오 dead와 동일하게 한 칸 높이

    DrawSheetSprite(g, sheet,
        srcX, srcY, CONTENT_W, srcH,
        p.x, p.y, dstW, dstH,
        flip);
}

// 로비: 접속한 플레이어 칸의 실루엣을 플레이어 번호에 맞는 캐릭터로 바꾼다 (P2 루이지, P3 토드).
// 좌표는 title_screen_multi.png(256x240)의 아이콘 칸을 화면(800x640) 크기로 옮긴 값이다.
static void DrawLobbyCharacterIcons(Graphics& graphics)
{
    struct LobbySlot { int playerId; int iconY; CharacterType charType; };
    static const LobbySlot slots[] = {
        { 1, 325, CHAR_LUIGI },  // 원본 122~137행
        { 2, 408, CHAR_TOAD  },  // 원본 153~168행
    };
    constexpr int ICON_X = 150, ICON_W = 44, ICON_H = 43;  // 원본 48~61열, P1 마리오 아이콘과 같은 크기
    SolidBrush slotFill(Color(255, 255, 206, 197));        // 아이콘 칸 바탕색

    for (const LobbySlot& slot : slots)
    {
        if (!NetIsPlayerConnected(slot.playerId)) continue;
        graphics.FillRectangle(&slotFill, ICON_X - 2, slot.iconY - 1, ICON_W + 4, ICON_H + 2);  // 실루엣 지우기
        graphics.Flush(FlushIntentionSync);

        MarioRenderData icon = {};
        icon.x = ICON_X;
        icon.y = slot.iconY;
        icon.width = ICON_W;
        icon.height = ICON_H;
        icon.direction = DIR_RIGHT;
        icon.form = FORM_SMALL;
        icon.charType = slot.charType;
        if (slot.charType == CHAR_LUIGI) Draw_luigi(icon);
        else Draw_toad(icon);
    }
}

static void DrawPlayerTransformation(MarioRenderData mario)
{
    const int bottom = mario.y + mario.height;
    if ((GetTickCount() / 100) % 2 == 0) mario.form = mario.transformTarget;
    mario.height = mario.form == FORM_SMALL ? TILE_SIZE : 2 * TILE_SIZE;
    mario.y = bottom - mario.height;
    mario.isWalking = false;
    mario.isJumping = false;
    mario.isFlying = false;
    mario.fire_motion = false;
    mario.tino_motion = false;
    mario.tino_fire_motion = false;

    switch (mario.charType)
    {
    case CHAR_LUIGI: Draw_luigi(mario); break;
    case CHAR_TOAD:  Draw_toad(mario); break;
    default:         Draw_mario(mario); break;
    }
}

void Draw()
{
    Draw_map_background();
    Graphics graphics(g_memDC);
    graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);

    Draw_spawn_item();
    Draw_map();
    Draw_item();
    Draw_Monsters();
    Draw_Turtles();
    Draw_Angel_Turtles();
    Draw_Bowser();
    Draw_Fireballs();
    Draw_fireball();
    DrawFireTraps();

    const MarioRenderData mario = BuildMarioRenderData();
    const bool flipX = (mario.direction == DIR_LEFT);

    // 캐릭터별 change 이미지 선택
    auto getChangeImg = [&](bool flower) -> Image* {
        if (mario.charType == CHAR_LUIGI) return g_images.big_luigi_change;
        if (mario.charType == CHAR_TOAD)  return g_images.big_toad_change;
        return flower ? g_images.flower_mario_change : g_images.big_mario_change;
    };
    // 캐릭터별 small idle 그리기
    auto drawSmallIdle = [&](int x, int y) {
        MarioRenderData tmp = mario; tmp.form = FORM_SMALL;
        tmp.isWalking = false; tmp.isJumping = false;
        tmp.x = x; tmp.y = y;
        if      (mario.charType == CHAR_LUIGI) { tmp.height = 40; Draw_luigi(tmp); }
        else if (mario.charType == CHAR_TOAD)  { tmp.height = 40; Draw_toad(tmp); }
        else DrawSprite(graphics, g_images.mario_stop, x, y, mario.width, mario.height, flipX);
    };
    // 모든 폼 idle 렌더 헬퍼 (변신 애니메이션용)
    auto drawIdle = [&](MarioForm form, int x, int y, int w, int h) {
        MarioRenderData tmp = mario;
        tmp.form = form; tmp.x = x; tmp.y = y; tmp.width = w; tmp.height = h;
        tmp.isWalking = false; tmp.isJumping = false;
        tmp.tino_motion = false; tmp.tino_fire_motion = false; tmp.fire_motion = false;
        switch (mario.charType) {
            case CHAR_LUIGI: Draw_luigi(tmp); break;
            case CHAR_TOAD:  Draw_toad(tmp);  break;
            default:         Draw_mario(tmp); break;
        }
    };

    const bool isAltChar = (mario.charType == CHAR_LUIGI || mario.charType == CHAR_TOAD);

    // mario
    if (mario.isTransforming && !mario.isDead)
    {
        DrawPlayerTransformation(mario);
    }
    else if (g_game.gameState == GAME_TRANSFORMING && g_game.transformTarget == FORM_BIG && mario.form == FORM_SMALL)
    {
        if ((GetTickCount() / 100) % 2 == 0)
            DrawSprite(graphics, getChangeImg(false), mario.x, mario.y,
                mario.width, TILE_SIZE * 2, flipX);
        else
            drawSmallIdle(mario.x, mario.y + TILE_SIZE);
    }
    else if (g_game.gameState == GAME_TRANSFORMING && g_game.transformTarget == FORM_BIG && mario.form == FORM_BIG)
    {
        if ((GetTickCount() / 100) % 2 == 0)
            DrawSprite(graphics, getChangeImg(false), mario.x, mario.y - TILE_SIZE, mario.width, mario.height, flipX);
        else
            drawSmallIdle(mario.x, mario.y);
    }
    else if (g_game.gameState == GAME_TRANSFORMING && g_game.transformTarget == FORM_FLOWER && mario.form == FORM_SMALL)    // small >> flower
    {
        if ((GetTickCount() / 100) % 2 == 0)
            drawIdle(FORM_FLOWER, mario.x, mario.y,            mario.width, TILE_SIZE * 2);
        else
            drawIdle(FORM_SMALL,  mario.x, mario.y + TILE_SIZE, mario.width, mario.height);
    }
    else if (g_game.gameState == GAME_TRANSFORMING && g_game.transformTarget == FORM_FLOWER && mario.form == FORM_BIG)     // big >> flower
    {
        const bool tick = (GetTickCount() / 100) % 2 == 0;
        drawIdle(tick ? FORM_FLOWER : FORM_BIG, mario.x, mario.y, mario.width, mario.height);
    }
    else if (g_game.gameState == GAME_TRANSFORMING && g_game.transformTarget == FORM_FLOWER && mario.form == FORM_TINO)    // tino >> flower
    {
        const bool tick = (GetTickCount() / 100) % 2 == 0;
        drawIdle(tick ? FORM_FLOWER : FORM_TINO, mario.x, mario.y, mario.width, mario.height);
    }
    else if (g_game.gameState == GAME_TRANSFORMING && g_game.transformTarget == FORM_FLOWER && mario.form == FORM_FLOWER)  // flower >> big
    {
        const bool tick = (GetTickCount() / 100) % 2 == 0;
        drawIdle(tick ? FORM_BIG : FORM_FLOWER, mario.x, mario.y, mario.width, mario.height);
    }
    // tino
    else if (g_game.gameState == GAME_TRANSFORMING && g_game.transformTarget == FORM_TINO && mario.form == FORM_SMALL)    // small >> tino
    {
        if ((GetTickCount() / 100) % 2 == 0)
            drawIdle(FORM_TINO,  mario.x, mario.y,             mario.width, TILE_SIZE * 2);
        else
            drawIdle(FORM_SMALL, mario.x, mario.y + TILE_SIZE, mario.width, mario.height);
    }
    else if (g_game.gameState == GAME_TRANSFORMING && g_game.transformTarget == FORM_TINO && mario.form == FORM_BIG)      // big >> tino
    {
        const bool tick = (GetTickCount() / 100) % 2 == 0;
        drawIdle(tick ? FORM_TINO : FORM_BIG, mario.x, mario.y, mario.width, mario.height);
    }
    else if (g_game.gameState == GAME_TRANSFORMING && g_game.transformTarget == FORM_TINO && mario.form == FORM_FLOWER)   // flower >> tino
    {
        const bool tick = (GetTickCount() / 100) % 2 == 0;
        drawIdle(tick ? FORM_TINO : FORM_FLOWER, mario.x, mario.y, mario.width, mario.height);
    }
    else if (g_game.gameState == GAME_TRANSFORMING && g_game.transformTarget == FORM_TINO && mario.form == FORM_TINO)     // tino >> big
    {
        const bool tick = (GetTickCount() / 100) % 2 == 0;
        drawIdle(tick ? FORM_BIG : FORM_TINO, mario.x, mario.y, mario.width, mario.height);
    }
    // gameover
    else if (g_game.gameState == GAME_OVER && !mario.isDead)
    {
        graphics.DrawImage(g_images.title_dead, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        Draw_information();
        TCHAR life_print_dead[32], world_dead[32];
        _stprintf_s(world_dead, _T("WORLD %d"), g_game.stage);
        _stprintf_s(life_print_dead, _T("%d"), mario.life);

        TextOut(g_memDC, 300, 207, world_dead, lstrlen(world_dead));
        TextOut(g_memDC, 430, 295, life_print_dead, lstrlen(life_print_dead));
        return;
    }
    else
    {
        if (!mario.isDead)
        {
            switch (mario.charType)
            {
            case CHAR_LUIGI: Draw_luigi(mario); break;
            case CHAR_TOAD:  Draw_toad(mario);  break;
            default:         Draw_mario(mario);  break;
            }
        }
    }

    if (mario.isDead)
    {
        if (mario.charType == CHAR_MARIO)
            DrawSprite(graphics, g_images.mario_dead, mario.x, mario.y, mario.width, TILE_SIZE, flipX);
        else
        {
            // Luigi/Toad dead 프레임도 스프라이트 시트에서
            MarioRenderData tmp = mario;
            tmp.isDead = true;
            if (mario.charType == CHAR_LUIGI) Draw_luigi(tmp);
            else                              Draw_toad(tmp);
        }
    }

    // 멀티/렌더 테스트 모드: 나머지 2개 슬롯도 같이 그림 (월드 좌표 → 스크린 좌표 변환)
    if (g_game.isMulti)
    {
        for (int _ri = 0; _ri < 3; _ri++)
        {
            if (_ri == g_localPlayerIdx) continue;
            MarioRenderData rm = BuildMarioRenderDataFor(_ri);
            rm.x -= (int)g_game.cameraX;  // 월드 좌표 → 스크린 좌표
            if (rm.isTransforming && !rm.isDead)
            {
                DrawPlayerTransformation(rm);
                continue;
            }
            switch (rm.charType)
            {
            case CHAR_LUIGI: Draw_luigi(rm); break;
            case CHAR_TOAD:  Draw_toad(rm);  break;
            default:
                if (rm.isDead)
                    DrawSprite(graphics, g_images.mario_dead, rm.x, rm.y, rm.width, TILE_SIZE, rm.direction == DIR_LEFT);
                else
                    Draw_mario(rm);
                break;
            }
        }
    }

    Draw_castle_blank();
}
// ─────────────────────────────────────────────
//  맵 레이어 캐시
//  배경·고정 타일은 맵 전체 크기 비트맵에 한 번만 그려 두고, 매 프레임 카메라 위치만큼 복사한다.
//  코인·아이템 블록처럼 애니메이션되는 타일만 매 프레임 직접 그린다.
//  currentMap을 매 프레임 사본과 비교해, 바뀐 칸이 있으면 그 열 주변만 다시 그린다.
// ─────────────────────────────────────────────
namespace {
constexpr int MAP_PIXEL_WIDTH = MAP_WIDTH * TILE_SIZE;
// 화면(640)이 맵(15칸 = 600)보다 높으므로 레이어는 둘 중 큰 높이로 만든다. 복사 영역이 비트맵을 벗어나면 AlphaBlend가 실패한다.
constexpr int MAP_PIXEL_HEIGHT = MAP_HEIGHT * TILE_SIZE > SCREEN_HEIGHT ? MAP_HEIGHT * TILE_SIZE : SCREEN_HEIGHT;
constexpr int TILE_SPRITE_REACH = 4;  // 성처럼 칸 밖으로 넓게 그려지는 타일의 최대 범위 (열)

struct MapLayer {
    HDC dc = nullptr;
    HBITMAP bitmap = nullptr;
    void* bits = nullptr;
};

MapLayer s_backgroundLayer;  // 하늘·배경 (불투명)
MapLayer s_tileLayer;        // 고정 타일 (premultiplied alpha)
int s_cachedTiles[MAP_HEIGHT][MAP_WIDTH] = {};
const void* s_cachedMap = nullptr;
int s_cachedStage = -1;

bool CreateMapLayer(MapLayer& layer)
{
    if (layer.dc && layer.bits) return true;
    BITMAPINFO info = {};
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = MAP_PIXEL_WIDTH;
    info.bmiHeader.biHeight = -MAP_PIXEL_HEIGHT;  // top-down
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    if (!layer.dc) layer.dc = CreateCompatibleDC(g_memDC);
    if (!layer.dc) return false;
    layer.bitmap = CreateDIBSection(layer.dc, &info, DIB_RGB_COLORS, &layer.bits, nullptr, 0);
    if (!layer.bitmap || !layer.bits) return false;
    SelectObject(layer.dc, layer.bitmap);
    return true;
}

bool IsAnimatedTile(int tile)
{
    switch (tile) {
    case TILE_COIN:
    case TILE_MYSTERY:
    case TILE_BOX_STAR:
    case TILE_BOX_FLOWER:
    case TILE_BOX_TINO:
    case TILE_BOX_UPMUSH:
    case TILE_BOX_COIN:
        return true;
    default:
        return false;
    }
}

bool IsCachedTile(int tile)
{
    return tile != TILE_EMPTY && !IsAnimatedTile(tile);
}

// 고정 타일 한 칸을 (screenX, screenY)에 그린다. 그릴 이미지가 없는 타일은 무시한다.
void DrawStaticTile(Graphics& graphics, int tile, int screenX, int screenY)
{
    Image* image = nullptr;
    switch (tile) {
    case TILE_GROUND:        image = g_images.stage_1_dirt; break;      // 장애물
    case TILE_BRICK:         image = g_images.stage_1_brick; break;     // 벽돌
    case TILE_STAIR:         image = g_images.unbreakable_block; break; // 계단
    case TILE_FLAG:          image = g_images.flag_stick; break;        // 깃발 봉
    case TILE_FLAG_TOP:      image = g_images.flag_marble; break;       // 깃발 꼭대기
    case TILE_MUSHROOM_H1:   image = g_images.mushroom_head_1; break;
    case TILE_MUSHROOM_H2:   image = g_images.mushroom_head_2; break;
    case TILE_MUSHROOM_H3:   image = g_images.mushroom_head_3; break;
    case TILE_MUSHROOM_T1:   image = g_images.mushroom_trunk_1; break;
    case TILE_MUSHROOM_T2:   image = g_images.mushroom_trunk_2; break;
    case TILE_CLOUD:         image = g_images.cloud_block; break;
    case TILE_FIRE_SWITCH:   image = g_images.fire_switch_tile; break;
    case TILE_STONE:         image = g_images.stone_tile; break;        // 스테이지 3 블럭
    case TILE_USED_BLOCK:    image = g_images.item_block_used; break;   // 사용된 아이템블럭
    case TILE_LAVA_HEAD:     image = g_images.fire_head; break;         // 불 블럭
    case TILE_LAVA_BODY:     image = g_images.fire_body; break;
    case TILE_KOOPA_BLOCK:   image = g_images.koopa_block; break;
    case TILE_PIPE_RB:       image = g_images.pipe_1; break;            // 파이프 우하
    case TILE_PIPE_LB:       image = g_images.pipe_2; break;            // 좌하
    case TILE_PIPE_RT:       image = g_images.pipe_3; break;            // 우상
    case TILE_PIPE_LT:       image = g_images.pipe_4; break;            // 좌상
    case TILE_CASTLE:        // 성: 칸 기준 왼쪽 3칸, 위쪽 4칸까지 크게 그린다.
        graphics.DrawImage(g_images.castle_1, screenX - TILE_SIZE * 3, screenY - TILE_SIZE * 4,
            (TILE_SIZE + 1) * 5, (TILE_SIZE + 1) * 5);
        return;
    case TILE_INVISIBLE:     // 피치: 두 칸 높이
        graphics.DrawImage(g_images.peach, screenX, screenY, TILE_SIZE + 1, TILE_SIZE * 2);
        return;
    default:
        return;
    }
    graphics.DrawImage(image, screenX, screenY, TILE_SIZE + 1, TILE_SIZE + 1);
}

void RenderBackgroundLayer()
{
    GdiFlush();
    RECT rect = { 0, 0, MAP_PIXEL_WIDTH, MAP_PIXEL_HEIGHT };
    FillRect(s_backgroundLayer.dc, &rect, g_game.stage == 3 ? black_brush : sky_brush);
    Graphics graphics(s_backgroundLayer.dc);
    graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);
    if (g_game.stage == 1)
        graphics.DrawImage(g_images.stage_1_background, 0, -36, 4600, SCREEN_HEIGHT);
    else if (g_game.stage == 2)
        graphics.DrawImage(g_images.stage_2_background, 0, -36, 4600, SCREEN_HEIGHT);
}

// firstColumn~lastColumn 영역을 비우고, 그 영역에 걸치는 고정 타일을 원래 순서(행 → 열)대로 다시 그린다.
void RenderTileColumns(int firstColumn, int lastColumn)
{
    if (firstColumn < 0) firstColumn = 0;
    if (lastColumn >= MAP_WIDTH) lastColumn = MAP_WIDTH - 1;
    if (firstColumn > lastColumn) return;

    GdiFlush();
    Bitmap layer(MAP_PIXEL_WIDTH, MAP_PIXEL_HEIGHT, MAP_PIXEL_WIDTH * 4, PixelFormat32bppPARGB,
        static_cast<BYTE*>(s_tileLayer.bits));
    Graphics graphics(&layer);
    graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);
    const Rect area(firstColumn * TILE_SIZE, 0, (lastColumn - firstColumn + 1) * TILE_SIZE, MAP_PIXEL_HEIGHT);
    graphics.SetClip(area);
    graphics.SetCompositingMode(CompositingModeSourceCopy);
    SolidBrush clear(Color(0, 0, 0, 0));
    graphics.FillRectangle(&clear, area);
    graphics.SetCompositingMode(CompositingModeSourceOver);

    const int from = firstColumn - TILE_SPRITE_REACH < 0 ? 0 : firstColumn - TILE_SPRITE_REACH;
    const int to = lastColumn + TILE_SPRITE_REACH >= MAP_WIDTH ? MAP_WIDTH - 1 : lastColumn + TILE_SPRITE_REACH;
    for (int i = 0; i < MAP_HEIGHT; i++)
        for (int j = from; j <= to; j++)
            DrawStaticTile(graphics, currentMap[i][j], j * TILE_SIZE, i * TILE_SIZE);
}

// 스테이지가 바뀌면 전체를, 타일이 바뀌면 그 열 주변만 다시 그린다.
bool SyncMapLayers()
{
    if (!currentMap || !CreateMapLayer(s_backgroundLayer) || !CreateMapLayer(s_tileLayer)) return false;

    if (s_cachedMap != currentMap || s_cachedStage != g_game.stage)
    {
        s_cachedMap = currentMap;
        s_cachedStage = g_game.stage;
        memcpy(s_cachedTiles, currentMap, sizeof(s_cachedTiles));
        RenderBackgroundLayer();
        RenderTileColumns(0, MAP_WIDTH - 1);
        return true;
    }

    bool changedColumns[MAP_WIDTH] = {};
    int changedCount = 0;
    for (int i = 0; i < MAP_HEIGHT; i++)
    {
        for (int j = 0; j < MAP_WIDTH; j++)
        {
            const int before = s_cachedTiles[i][j];
            const int after = currentMap[i][j];
            if (before == after) continue;
            s_cachedTiles[i][j] = after;
            // 애니메이션 타일과 빈칸 사이의 변화(코인 획득 등)는 캐시에 영향이 없다.
            if ((IsCachedTile(before) || IsCachedTile(after)) && !changedColumns[j])
            {
                changedColumns[j] = true;
                ++changedCount;
            }
        }
    }
    if (changedCount > 20)
    {
        RenderTileColumns(0, MAP_WIDTH - 1);  // 맵 재구성 등 큰 변경
    }
    else if (changedCount > 0)
    {
        for (int j = 0; j < MAP_WIDTH; j++)
            if (changedColumns[j]) RenderTileColumns(j, j);
    }
    return true;
}

int CameraPixelX()
{
    int cameraX = static_cast<int>(g_game.cameraX);
    if (cameraX < 0) cameraX = 0;
    if (cameraX > MAP_PIXEL_WIDTH - SCREEN_WIDTH) cameraX = MAP_PIXEL_WIDTH - SCREEN_WIDTH;
    return cameraX;
}
}

// 하늘·배경: 캐시된 배경 레이어를 복사하고, 캐시를 만들지 못했으면 직접 그린다.
static void Draw_map_background()
{
    if (SyncMapLayers())
    {
        GdiFlush();
        BitBlt(g_memDC, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, s_backgroundLayer.dc, CameraPixelX(), 0, SRCCOPY);
        return;
    }
    RECT rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    if (g_game.stage == 1 || g_game.stage == 2) FillRect(g_memDC, &rect, sky_brush);
    else if (g_game.stage == 3) FillRect(g_memDC, &rect, black_brush);
    Draw_background();
}

static void Draw_map()
{
    // 0: 구멍 1: 땅 2: 코인 3:굼바 4:파이프 5:계단 6:미스테리박스 7:깃발 8: 깃발꼭짓점 9:성 10: 벽돌
    // 90. 버섯머리1 91. 버섯머리2 92. 버섯머리3 11. 버섯줄기 12. 버섯줄기2 13.구름 16: 사용된블럭 60:스타박스 61:꽃박스
    if (SyncMapLayers())
    {
        GdiFlush();
        const BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
        AlphaBlend(g_memDC, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
            s_tileLayer.dc, CameraPixelX(), 0, SCREEN_WIDTH, SCREEN_HEIGHT, blend);
    }

    Graphics graphics(g_memDC);
    graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);

    // 캐시를 만들지 못했으면 고정 타일도 여기서 직접 그린다.
    const bool cached = s_tileLayer.bits != nullptr;
    const int cameraX = static_cast<int>(g_game.cameraX);
    int firstColumn = cameraX / TILE_SIZE - TILE_SPRITE_REACH;
    int lastColumn = (cameraX + SCREEN_WIDTH) / TILE_SIZE + TILE_SPRITE_REACH;
    if (firstColumn < 0) firstColumn = 0;
    if (lastColumn >= MAP_WIDTH) lastColumn = MAP_WIDTH - 1;
    for (int i = 0; i < MAP_HEIGHT; i++)
    {
        for (int j = firstColumn; j <= lastColumn; j++)
        {
            const int tile = currentMap[i][j];
            int screenX = j * TILE_SIZE - g_game.cameraX;
            int screenY = i * TILE_SIZE;
            if (!IsAnimatedTile(tile))
            {
                if (!cached) DrawStaticTile(graphics, tile, screenX, screenY);
                continue;
            }
            if (screenX + TILE_SIZE < 0 || screenX >= SCREEN_WIDTH) continue;
            if (tile == TILE_COIN)
            {
                if (g_game.frameMotion == 0 || (g_game.frameMotion >= 4 && g_game.frameMotion < 7))
                    graphics.DrawImage(g_images.coin_1, screenX, screenY, TILE_SIZE + 1, TILE_SIZE);
                else if (g_game.frameMotion == 1 || g_game.frameMotion == 3)
                    graphics.DrawImage(g_images.coin_2, screenX, screenY, TILE_SIZE + 1, TILE_SIZE);
                else if (g_game.frameMotion == 2)
                    graphics.DrawImage(g_images.coin_3, screenX, screenY, TILE_SIZE + 1, TILE_SIZE);
            }
            else
            {
                if (g_game.frameMotion == 0 || (g_game.frameMotion >= 4 && g_game.frameMotion < 7))
                    graphics.DrawImage(g_images.item_block_1, screenX, screenY, TILE_SIZE + 1, TILE_SIZE + 1);
                else if (g_game.frameMotion == 1 || g_game.frameMotion == 3)
                    graphics.DrawImage(g_images.item_block_2, screenX, screenY, TILE_SIZE + 1, TILE_SIZE + 1);
                else if (g_game.frameMotion == 2)
                    graphics.DrawImage(g_images.item_block_3, screenX, screenY, TILE_SIZE + 1, TILE_SIZE + 1);
            }
        }
    }
}
static void Draw_castle_blank()
{
    Graphics graphics(g_memDC);
    graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);

    for (int i = 0; i < MAP_HEIGHT; i++)
    {
        for (int j = 0; j < MAP_WIDTH; j++)
        {
            int screenX = j * TILE_SIZE - g_game.cameraX;
            int screenY = i * TILE_SIZE;
            if (currentMap[i][j] == TILE_CASTLE)
            {
                if (screenX + TILE_SIZE >= 0 && screenX < SCREEN_WIDTH)
                {
                    graphics.DrawImage(g_images.castle_blank, screenX - TILE_SIZE, screenY - TILE_SIZE, TILE_SIZE + 1, (TILE_SIZE + 1) * 2);
                }
            }

        }
    }
}
static void Draw_mario(const MarioRenderData& mario)
{
    Graphics graphics(g_memDC);
    graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);
    const bool flipX = (mario.direction == DIR_LEFT);

    if (mario.form == FORM_SMALL && !mario.star) // small mario
    {
        if (mario.isJumping)
        {
            DrawSprite(graphics, g_images.mario_jump, mario.x, mario.y, mario.width, mario.height, flipX);
        }
        else if (mario.isWalking)
        {
            if (mario.walk_motion == 0)
            {
                DrawSprite(graphics, g_images.mario_walk_motion_1, mario.x, mario.y, mario.width, mario.height, flipX);
            }
            else if (mario.walk_motion == 1)
            {
                DrawSprite(graphics, g_images.mario_walk_motion_2, mario.x, mario.y, mario.width, mario.height, flipX);
            }
            else if (mario.walk_motion == 2)
            {
                DrawSprite(graphics, g_images.mario_walk_motion_3, mario.x, mario.y, mario.width, mario.height, flipX);
            }
        }
        else
        {
            DrawSprite(graphics, g_images.mario_stop, mario.x, mario.y, mario.width, mario.height, flipX);
        }
    }
    else if (mario.form == FORM_BIG && !mario.star) // big mario
    {
        if (mario.isJumping)
        {
            DrawSprite(graphics, g_images.big_mario_jump, mario.x, mario.y, mario.width, mario.height, flipX);
        }
        else if (mario.isWalking)
        {
            if (mario.walk_motion == 0)
            {
                DrawSprite(graphics, g_images.big_mario_walk_motion_1, mario.x, mario.y, mario.width, mario.height, flipX);
            }
            else if (mario.walk_motion == 1)
            {
                DrawSprite(graphics, g_images.big_mario_walk_motion_2, mario.x, mario.y, mario.width, mario.height, flipX);
            }
            else if (mario.walk_motion == 2)
            {
                DrawSprite(graphics, g_images.big_mario_walk_motion_3, mario.x, mario.y, mario.width, mario.height, flipX);
            }
        }
        else
        {
            DrawSprite(graphics, g_images.big_mario_stop, mario.x, mario.y, mario.width, mario.height, flipX);
        }
    }
    else if (mario.form == FORM_SMALL && mario.star) // star small mario
    {
        if (g_game.frameMotionStar == 0)
        {
            if (mario.isJumping)
            {
                DrawSprite(graphics, g_images.star_mario_jump_1, mario.x, mario.y, mario.width, mario.height, flipX);
            }
            else if (mario.isWalking)
            {
                if (mario.walk_motion == 0)
                {
                    DrawSprite(graphics, g_images.star_mario_walk_motion_1_1, mario.x, mario.y, mario.width, mario.height, flipX);
                }
                else if (mario.walk_motion == 1)
                {
                    DrawSprite(graphics, g_images.star_mario_walk_motion_2_1, mario.x, mario.y, mario.width, mario.height, flipX);
                }
                else if (mario.walk_motion == 2)
                {
                    DrawSprite(graphics, g_images.star_mario_walk_motion_3_1, mario.x, mario.y, mario.width, mario.height, flipX);
                }
            }
            else
            {
                DrawSprite(graphics, g_images.star_mario_stop_1, mario.x, mario.y, mario.width, mario.height, flipX);
            }
        }
        else if (g_game.frameMotionStar == 1)
        {
            if (mario.isJumping)
            {
                DrawSprite(graphics, g_images.star_mario_jump_2, mario.x, mario.y, mario.width, mario.height, flipX);
            }
            else if (mario.isWalking)
            {
                if (mario.walk_motion == 0)
                {
                    DrawSprite(graphics, g_images.star_mario_walk_motion_1_2, mario.x, mario.y, mario.width, mario.height, flipX);
                }
                else if (mario.walk_motion == 1)
                {
                    DrawSprite(graphics, g_images.star_mario_walk_motion_2_2, mario.x, mario.y, mario.width, mario.height, flipX);
                }
                else if (mario.walk_motion == 2)
                {
                    DrawSprite(graphics, g_images.star_mario_walk_motion_3_2, mario.x, mario.y, mario.width, mario.height, flipX);
                }
            }
            else
            {
                DrawSprite(graphics, g_images.star_mario_stop_2, mario.x, mario.y, mario.width, mario.height, flipX);
            }
        }
        else if (g_game.frameMotionStar == 2)
        {
            if (mario.isJumping)
            {
                DrawSprite(graphics, g_images.star_mario_jump_3, mario.x, mario.y, mario.width, mario.height, flipX);
            }
            else if (mario.isWalking)
            {
                if (mario.walk_motion == 0)
                {
                    DrawSprite(graphics, g_images.star_mario_walk_motion_1_3, mario.x, mario.y, mario.width, mario.height, flipX);
                }
                else if (mario.walk_motion == 1)
                {
                    DrawSprite(graphics, g_images.star_mario_walk_motion_2_3, mario.x, mario.y, mario.width, mario.height, flipX);
                }
                else if (mario.walk_motion == 2)
                {
                    DrawSprite(graphics, g_images.star_mario_walk_motion_3_3, mario.x, mario.y, mario.width, mario.height, flipX);
                }
            }
            else
            {
                DrawSprite(graphics, g_images.star_mario_stop_3, mario.x, mario.y, mario.width, mario.height, flipX);
            }
        }
    }
    else if (mario.form != FORM_SMALL && mario.star) // star big mario
    {
        if (g_game.frameMotionStar == 0)
        {
            if (mario.isJumping)
            {
                DrawSprite(graphics, g_images.star_big_mario_jump_1, mario.x, mario.y, mario.width, mario.height, flipX);
            }
            else if (mario.isWalking)
            {
                if (mario.walk_motion == 0)
                {
                    DrawSprite(graphics, g_images.star_big_mario_walk_motion_1_1, mario.x, mario.y, mario.width, mario.height, flipX);
                }
                else if (mario.walk_motion == 1)
                {
                    DrawSprite(graphics, g_images.star_big_mario_walk_motion_2_1, mario.x, mario.y, mario.width, mario.height, flipX);
                }
                else if (mario.walk_motion == 2)
                {
                    DrawSprite(graphics, g_images.star_big_mario_walk_motion_3_1, mario.x, mario.y, mario.width, mario.height, flipX);
                }
            }
            else
            {
                DrawSprite(graphics, g_images.star_big_mario_stop_1, mario.x, mario.y, mario.width, mario.height, flipX);
            }
        }
        else if (g_game.frameMotionStar == 1)
        {
            if (mario.isJumping)
            {
                DrawSprite(graphics, g_images.star_big_mario_jump_2, mario.x, mario.y, mario.width, mario.height, flipX);
            }
            else if (mario.isWalking)
            {
                if (mario.walk_motion == 0)
                {
                    DrawSprite(graphics, g_images.star_big_mario_walk_motion_1_2, mario.x, mario.y, mario.width, mario.height, flipX);
                }
                else if (mario.walk_motion == 1)
                {
                    DrawSprite(graphics, g_images.star_big_mario_walk_motion_2_2, mario.x, mario.y, mario.width, mario.height, flipX);
                }
                else if (mario.walk_motion == 2)
                {
                    DrawSprite(graphics, g_images.star_big_mario_walk_motion_3_2, mario.x, mario.y, mario.width, mario.height, flipX);
                }
            }
            else
            {
                DrawSprite(graphics, g_images.star_big_mario_stop_2, mario.x, mario.y, mario.width, mario.height, flipX);
            }
        }
        else if (g_game.frameMotionStar == 2)
        {
            if (mario.isJumping)
            {
                DrawSprite(graphics, g_images.star_big_mario_jump_3, mario.x, mario.y, mario.width, mario.height, flipX);
            }
            else if (mario.isWalking)
            {
                if (mario.walk_motion == 0)
                {
                    DrawSprite(graphics, g_images.star_big_mario_walk_motion_1_3, mario.x, mario.y, mario.width, mario.height, flipX);
                }
                else if (mario.walk_motion == 1)
                {
                    DrawSprite(graphics, g_images.star_big_mario_walk_motion_2_3, mario.x, mario.y, mario.width, mario.height, flipX);
                }
                else if (mario.walk_motion == 2)
                {
                    DrawSprite(graphics, g_images.star_big_mario_walk_motion_3_3, mario.x, mario.y, mario.width, mario.height, flipX);
                }
            }
            else
            {
                DrawSprite(graphics, g_images.star_big_mario_stop_3, mario.x, mario.y, mario.width, mario.height, flipX);
            }
        }
    }
    else if (mario.form == FORM_FLOWER) // flower mario
    {
        if (mario.fire_motion)
        {
            DrawSprite(graphics, g_images.flower_mario_fire, mario.x, mario.y, mario.width, mario.height, flipX);
        }
        else if (mario.isJumping)
        {
            DrawSprite(graphics, g_images.flower_mario_jump, mario.x, mario.y, mario.width, mario.height, flipX);
        }
        else if (mario.isWalking)
        {
            if (mario.walk_motion == 0)
            {
                DrawSprite(graphics, g_images.flower_mario_walk_motion_1, mario.x, mario.y, mario.width, mario.height, flipX);
            }
            else if (mario.walk_motion == 1)
            {
                DrawSprite(graphics, g_images.flower_mario_walk_motion_2, mario.x, mario.y, mario.width, mario.height, flipX);
            }
            else if (mario.walk_motion == 2)
            {
                DrawSprite(graphics, g_images.flower_mario_walk_motion_3, mario.x, mario.y, mario.width, mario.height, flipX);
            }
        }
        else
        {
            DrawSprite(graphics, g_images.flower_mario_stop, mario.x, mario.y, mario.width, mario.height, flipX);
        }
    }
    else if (mario.form == FORM_TINO) // tino mario
    {
        if (mario.tino_motion)
        {
            if (mario.motion_timer >= 0 && mario.motion_timer <= 5)
            {
                DrawSprite(graphics, g_images.tino_mario_attack_6, mario.x - 25, mario.y - 15, 100, 100, flipX);
            }
            else if (mario.motion_timer > 5 && mario.motion_timer <= 10)
            {
                DrawSprite(graphics, g_images.tino_mario_attack_5, mario.x - 25, mario.y - 15, 100, 100, flipX);
            }
            else if (mario.motion_timer > 10 && mario.motion_timer <= 15)
            {
                DrawSprite(graphics, g_images.tino_mario_attack_4, mario.x - 25, mario.y - 15, 100, 100, flipX);
            }
            else if (mario.motion_timer > 15 && mario.motion_timer <= 20)
            {
                DrawSprite(graphics, g_images.tino_mario_attack_3, mario.x - 25, mario.y - 15, 100, 100, flipX);
            }
            else if (mario.motion_timer > 20 && mario.motion_timer <= 25)
            {
                DrawSprite(graphics, g_images.tino_mario_attack_2, mario.x - 25, mario.y - 15, 100, 100, flipX);
            }
            else if (mario.motion_timer > 25 && mario.motion_timer <= 30)
            {
                DrawSprite(graphics, g_images.tino_mario_attack_1, mario.x - 25, mario.y - 15, 100, 100, flipX);
            }
        }
        else if (mario.tino_fire_motion)
        {
            DrawSprite(graphics, g_images.tino_mario_attack_2, mario.x - 25, mario.y - 15, 100, 100, flipX);
        }
        else if (mario.isJumping)
        {
            DrawSprite(graphics, g_images.tino_mario_jump, mario.x, mario.y, mario.width, mario.height, flipX);
        }
        else if (mario.isWalking)
        {
            if (mario.walk_motion == 0)
            {
                DrawSprite(graphics, g_images.tino_mario_walk_motion_1, mario.x, mario.y, mario.width, mario.height, flipX);
            }
            else if (mario.walk_motion == 1)
            {
                DrawSprite(graphics, g_images.tino_mario_walk_motion_2, mario.x, mario.y, mario.width, mario.height, flipX);
            }
            else if (mario.walk_motion == 2)
            {
                DrawSprite(graphics, g_images.tino_mario_walk_motion_3, mario.x, mario.y, mario.width, mario.height, flipX);
            }
        }
        else
        {
            DrawSprite(graphics, g_images.tino_mario_stop, mario.x, mario.y, mario.width, mario.height, flipX);
        }
    }
}
static void Draw_spawn_item()
{
    Graphics graphics(g_memDC);
    graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);

    // 스폰모션그리기
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        // 버섯
        if (mushroom[i].motion)
        {
            graphics.DrawImage(g_images.item_mushroom, mushroom[i].x - g_game.cameraX, mushroom[i].y, 40, 40);
        }
        // 생명 버섯
        if (up_mushroom[i].motion)
        {
            graphics.DrawImage(g_images.item_up_mushroom, up_mushroom[i].x - g_game.cameraX, up_mushroom[i].y, 40, 40);
        }
        // 스타
        if (star[i].motion)
        {
            if (g_game.frameMotion == 0 || g_game.frameMotion == 4 || g_game.frameMotion == 5 || g_game.frameMotion == 6)
            {
                graphics.DrawImage(g_images.item_star_1, star[i].x - g_game.cameraX, star[i].y, 40, 40);
            }
            else if (g_game.frameMotion == 1)
            {
                graphics.DrawImage(g_images.item_star_2, star[i].x - g_game.cameraX, star[i].y, 40, 40);
            }
            else if (g_game.frameMotion == 2)
            {
                graphics.DrawImage(g_images.item_star_3, star[i].x - g_game.cameraX, star[i].y, 40, 40);
            }
            else if (g_game.frameMotion == 3)
            {
                graphics.DrawImage(g_images.item_star_4, star[i].x - g_game.cameraX, star[i].y, 40, 40);
            }
        }
        // 꽃
        if (flower[i].motion) // 꽃
        {
            if (g_game.frameMotion == 0 || g_game.frameMotion == 4 || g_game.frameMotion == 5 || g_game.frameMotion == 6)
            {
                graphics.DrawImage(g_images.item_flower_1, flower[i].x - g_game.cameraX, flower[i].y, 40, 40);
            }
            else if (g_game.frameMotion == 1)
            {
                graphics.DrawImage(g_images.item_flower_2, flower[i].x - g_game.cameraX, flower[i].y, 40, 40);
            }
            else if (g_game.frameMotion == 2)
            {
                graphics.DrawImage(g_images.item_flower_3, flower[i].x - g_game.cameraX, flower[i].y, 40, 40);
            }
            else if (g_game.frameMotion == 3)
            {
                graphics.DrawImage(g_images.item_flower_4, flower[i].x - g_game.cameraX, flower[i].y, 40, 40);
            }
        }
        // 티노
        if (tino[i].motion)
        {
            graphics.DrawImage(g_images.item_tino, tino[i].x - g_game.cameraX, tino[i].y, 40, 40);
        }
    }

}
static void Draw_item()
{
    Graphics graphics(g_memDC);
    graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);

    for (int i = 0; i < MAX_ITEMS; i++)
    {
        // 버섯
        if (mushroom[i].active) // 버섯
        {
            graphics.DrawImage(g_images.item_mushroom, mushroom[i].x - g_game.cameraX, mushroom[i].y, 40, 40);
        }
        if (up_mushroom[i].active) // 생명버섯
        {
            graphics.DrawImage(g_images.item_up_mushroom, up_mushroom[i].x - g_game.cameraX, up_mushroom[i].y, 40, 40);
        }
        // 스타
        if (star[i].active) // 스타
        {
            if(g_game.frameMotion == 0 || g_game.frameMotion == 4 || g_game.frameMotion == 5 || g_game.frameMotion == 6)
            {
                graphics.DrawImage(g_images.item_star_1, star[i].x - g_game.cameraX, star[i].y, 40, 40);
            }
            else if (g_game.frameMotion == 1)
            {
                graphics.DrawImage(g_images.item_star_2, star[i].x - g_game.cameraX, star[i].y, 40, 40);
            }
            else if (g_game.frameMotion == 2)
            {
                graphics.DrawImage(g_images.item_star_3, star[i].x - g_game.cameraX, star[i].y, 40, 40);
            }
            else if (g_game.frameMotion == 3)
            {
                graphics.DrawImage(g_images.item_star_4, star[i].x - g_game.cameraX, star[i].y, 40, 40);
            }
        }
        // 꽃
        if (flower[i].active) // 꽃
        {
            if (g_game.frameMotion == 0 || g_game.frameMotion == 4 || g_game.frameMotion == 5 || g_game.frameMotion == 6)
            {
                graphics.DrawImage(g_images.item_flower_1, flower[i].x - g_game.cameraX, flower[i].y, 40, 40);
            }
            else if (g_game.frameMotion == 1)
            {
                graphics.DrawImage(g_images.item_flower_2, flower[i].x - g_game.cameraX, flower[i].y, 40, 40);
            }
            else if (g_game.frameMotion == 2)
            {
                graphics.DrawImage(g_images.item_flower_3, flower[i].x - g_game.cameraX, flower[i].y, 40, 40);
            }
            else if (g_game.frameMotion == 3)
            {
                graphics.DrawImage(g_images.item_flower_4, flower[i].x - g_game.cameraX, flower[i].y, 40, 40);
            }
        }
        // 티노
        if (tino[i].active)
        {
            graphics.DrawImage(g_images.item_tino, tino[i].x - g_game.cameraX, tino[i].y, 40, 40);
        }

    }

}
static void DrawShotPool(Graphics& graphics, const Shot* fireballs, const Shot* tinoFires,
                         const Shot_Effect* effects)
{
    for (int i = 0; i < MAX_SHOT; i++)
    {
        // flower fire
        if (!fireballs[i].active) continue;
        int screenX = fireballs[i].x - g_game.cameraX;
        int screenY = fireballs[i].y;
        if (fireballs[i].motion == 0)
        {
            graphics.DrawImage(g_images.shot_fireball_1, screenX, screenY, 20, 20);
        }
        else if (fireballs[i].motion == 1)
        {
            graphics.DrawImage(g_images.shot_fireball_2, screenX, screenY, 20, 20);
        }
        else if (fireballs[i].motion == 2)
        {
            graphics.DrawImage(g_images.shot_fireball_3, screenX, screenY, 20, 20);
        }
        else if (fireballs[i].motion == 3)
        {
            graphics.DrawImage(g_images.shot_fireball_4, screenX, screenY, 20, 20);
        }

    }
    for (int i = 0; i < MAX_SHOT; i++)
    {
        // tino fire
        if (effects && effects[i].active)
        {
            int screenX = effects[i].x - g_game.cameraX;
            int screenY = effects[i].y;
            if(effects[i].timer < 5)
            {
                graphics.DrawImage(g_images.tino_mario_fire_fade_1, screenX, screenY - 40, TILE_SIZE * 2, TILE_SIZE * 2);
            }
            else if (effects[i].timer < 10)
            {
                graphics.DrawImage(g_images.tino_mario_fire_fade_2, screenX, screenY - 40, TILE_SIZE * 2, TILE_SIZE * 2);
            }
            else if (effects[i].timer < 15)
            {
                graphics.DrawImage(g_images.tino_mario_fire_fade_3, screenX, screenY - 40, TILE_SIZE * 2, TILE_SIZE * 2);
            }
        }
        if (!tinoFires[i].active) continue;
        int screenX = tinoFires[i].x - g_game.cameraX;
        int screenY = tinoFires[i].y;
        if (tinoFires[i].direction == DIR_LEFT)
        {
            if (tinoFires[i].motion < 3)
            {
                graphics.DrawImage(g_images.tino_mario_fire_1, screenX, screenY, TILE_SIZE * 2, TILE_SIZE * 2);
            }
            else 
            {
                graphics.DrawImage(g_images.tino_mario_fire_2, screenX, screenY, TILE_SIZE * 2, TILE_SIZE * 2);
            }
        }
        else if (tinoFires[i].direction == DIR_RIGHT)
        {
            if (tinoFires[i].motion < 3)
            {
                graphics.DrawImage(g_images.tino_mario_fire_R_1, screenX, screenY, TILE_SIZE * 2, TILE_SIZE * 2);
            }
            else
            {
                graphics.DrawImage(g_images.tino_mario_fire_R_2, screenX, screenY, TILE_SIZE * 2, TILE_SIZE * 2);
            }
        }
    }
}
static void Draw_fireball()
{
    Graphics graphics(g_memDC);
    graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);
    DrawShotPool(graphics, fireball, tinofire, tinofire_effect);
    if (g_game.isMulti && !g_game.isRenderTest)
        for (const auto& pool : remoteShots)
            DrawShotPool(graphics, pool.fireballs, pool.tinoFires, nullptr);
}
void Draw_information()
{
    const MarioRenderData mario = BuildMarioRenderData();
    Graphics graphics(g_memDC);
    graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);
    TCHAR cooldown_z[32], cooldown_space[32];
    TCHAR clear_text_1[32];
    TCHAR clear_text_2[32];

    // 타이틀 스크린
    _stprintf_s(life_print, _T("MARIO"));
    _stprintf_s(life_print2, _T("%05d"), mario.life);
    _stprintf_s(time_print, _T("TIME"));
    _stprintf_s(time_print2, _T("%03d"), g_game.stage_time);
    _stprintf_s(coin_print, _T("%02d"), mario.coin);
    _stprintf_s(time_print2, _T("%03d"), g_game.stage_time);
    _stprintf_s(stage_print, _T("WORLD"));
    _stprintf_s(stage_print2, _T("%d"), g_game.stage);
    _stprintf_s(cooldown_z, _T("Z (fire): %d"), mario.tino_cooldown_z);
    _stprintf_s(cooldown_space, _T("SPACE (bite): %d"), mario.tino_cooldown_space);
    _stprintf_s(clear_text_1, _T("THANK YOU MARIO!"));
    _stprintf_s(clear_text_2, _T("YOUR QUEST IS OVER.!"));


    TextOut(g_memDC, 80, 20, life_print, lstrlen(life_print));         // 생명
    TextOut(g_memDC, 80, 40, life_print2, lstrlen(life_print2));         // 생명

    TextOut(g_memDC, 620, 20, time_print, lstrlen(time_print));         // 시간
    TextOut(g_memDC, 640, 40, time_print2, lstrlen(time_print2));         // 시간


    // 코인
    if (g_game.frameMotion < 4)
    {
        graphics.DrawImage(g_images.screen_coin_1, 260, 40, 25, 25);
    }
    else if (g_game.frameMotion == 4 || g_game.frameMotion == 6)
    {
        graphics.DrawImage(g_images.screen_coin_2, 260, 40, 25, 25);
    }
    else if (g_game.frameMotion == 5)
    {
        graphics.DrawImage(g_images.screen_coin_3, 260, 40, 25, 25);
    }
    graphics.DrawImage(g_images.screen_coin_x, 290, 40, 25, 25);
    TextOut(g_memDC, 320, 40, coin_print, lstrlen(coin_print));         // 코인

    TextOut(g_memDC, 440, 20, stage_print, lstrlen(stage_print));         // 월드
    TextOut(g_memDC, 480, 40, stage_print2, lstrlen(stage_print2));

    if(mario.form == FORM_TINO)
    {
        TextOut(g_memDC, 80, 80, cooldown_z, lstrlen(cooldown_z));         // 쿨타임
        TextOut(g_memDC, 80, 100, cooldown_space, lstrlen(cooldown_space));         // 쿨타임
    }
    if (g_game.gameClearText)
    {
        TextOut(g_memDC, 240, 160, clear_text_1, lstrlen(clear_text_1));
        TextOut(g_memDC, 200, 240, clear_text_2, lstrlen(clear_text_2));
    }

    // 렌더 테스트 HUD
    if (g_game.isRenderTest)
    {
        TCHAR rtxt[32];
        _stprintf_s(rtxt, _T("[TAB] CTRL: P%d"), g_localPlayerIdx + 1);
        TextOut(g_memDC, 300, 80, rtxt, lstrlen(rtxt));
    }
}

static void Draw_background()
{
    Graphics graphics(g_memDC);
    graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);

    if (g_game.stage == 1)
    {
        graphics.DrawImage(g_images.stage_1_background, -g_game.cameraX, -36, 4600, SCREEN_HEIGHT);
    }
    else if (g_game.stage == 2)
    {
        graphics.DrawImage(g_images.stage_2_background, -g_game.cameraX, -36, 4600, SCREEN_HEIGHT);
    }
}
static void Draw_Monsters()
{
    Graphics graphics(g_memDC);
    graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);

    for (int i = 0; i < g_monsters.monsterCount; i++)
    {
        if (!g_monsters.monsters[i].active)
            continue;

        int screenX = g_monsters.monsters[i].x - g_game.cameraX;
        int screenY = g_monsters.monsters[i].y;

        if (!g_monsters.monsters[i].isAlive || g_monsters.monsters[i].isDead)
        {
            if (g_game.stage == 1)
                graphics.DrawImage(g_images.monster1_dead, screenX, screenY, TILE_SIZE, TILE_SIZE);
            else if (g_game.stage == 2)
                graphics.DrawImage(g_images.monster2_dead, screenX, screenY, TILE_SIZE, TILE_SIZE);
            else if (g_game.stage == 3)
                graphics.DrawImage(g_images.monster3_dead, screenX, screenY, TILE_SIZE, TILE_SIZE);

            continue;
        }

        if (screenX + TILE_SIZE < 0 || screenX >= SCREEN_WIDTH) continue;

        if (g_game.stage == 1)
            graphics.DrawImage((g_game.frameMotion % 2 == 0) ? g_images.monster1_motion1 : g_images.monster1_motion2, screenX, screenY, TILE_SIZE, TILE_SIZE);
        else if (g_game.stage == 2)
            graphics.DrawImage((g_game.frameMotion % 2 == 0) ? g_images.monster2_motion1 : g_images.monster2_motion2, screenX, screenY, TILE_SIZE, TILE_SIZE);
        else if (g_game.stage == 3)
            graphics.DrawImage((g_game.frameMotion % 2 == 0) ? g_images.monster3_motion1 : g_images.monster3_motion2, screenX, screenY, TILE_SIZE, TILE_SIZE);
    }
}
static void Draw_Turtles()
{
    Graphics graphics(g_memDC);
    graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);
    for (int i = 0; i < g_monsters.turtleCount; i++)
    {
        if (!g_monsters.turtles[i].isAlive) continue;

        int drawX = g_monsters.turtles[i].x - g_game.cameraX;
        int drawY = g_monsters.turtles[i].y;

        switch (g_monsters.turtles[i].turtleState)
        {
        case NORMAL:
        {
            if(g_monsters.turtles[i].direction == DIR_LEFT)
            {
                graphics.DrawImage((g_game.stage == 3)
                    ? ((g_game.frameMotion % 2 == 0) ? g_images.brown_turtle_1 : g_images.brown_turtle_2)
                    : ((g_game.frameMotion % 2 == 0) ? g_images.turtle_1 : g_images.turtle_2), drawX, drawY - TILE_SIZE, TILE_SIZE, TILE_SIZE * 2);
            }
            else if (g_monsters.turtles[i].direction == DIR_RIGHT)
            {
                graphics.DrawImage((g_game.stage == 3) 
                    ? ((g_game.frameMotion % 2 == 0) ? g_images.brown_turtle_R_1 : g_images.brown_turtle_R_2) 
                    : ((g_game.frameMotion % 2 == 0) ? g_images.turtle_R_1 : g_images.turtle_R_2), drawX, drawY - TILE_SIZE, TILE_SIZE, TILE_SIZE * 2);
            }
            break;
        }
        case SHELL:
        case SPINNING:
        {
            graphics.DrawImage(g_game.stage==3 ? g_images.brown_turtle_hide : g_images.turtle_hide, drawX, drawY, 40, 40);
            break;
        }
        }
    }
}
static void Draw_Angel_Turtles()
{
    Graphics graphics(g_memDC);
    graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);
    for (int i = 0; i < g_monsters.angelTurtleCount; i++)
    {
        if (!g_monsters.angelTurtles[i].isAlive) continue;

        int drawX = g_monsters.angelTurtles[i].x - g_game.cameraX;
        int drawY = g_monsters.angelTurtles[i].y;

        switch (g_monsters.angelTurtles[i].state)
        {
        case FLYING:
        {
            graphics.DrawImage((g_game.frameMotion % 2 == 0) ? g_images.angel_turtle_1 : g_images.angel_turtle_2, drawX, drawY - TILE_SIZE, TILE_SIZE, TILE_SIZE + 20);
            break;
        }
        case HIDE:
        {
            graphics.DrawImage(g_images.turtle_hide, drawX, drawY, 40, 40);
            break;
        }
        }
    }
}
static void Draw_Bowser()
{
    const Bowser& bowser = g_monsters.bowser;
    if (!bowser.isAlive) return;

    Graphics graphics(g_memDC);
    graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);

    Image* sprite;
    if (bowser.isFiring)
        sprite = bowser.frame == 0 ? g_images.bowser_fire_walk_1 : g_images.bowser_fire_walk_2;
    else
        sprite = bowser.frame == 0 ? g_images.bowser_walk_1 : g_images.bowser_walk_2;
    const int facing = bowser.isFiring ? bowser.fireSign : bowser.moveSign;
    DrawSprite(graphics, sprite, bowser.x - g_game.cameraX, bowser.y,
        bowser.width, bowser.height, facing > 0);
}

static void Draw_Fireballs()
{
    Graphics graphics(g_memDC);
    graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);

    for (const Fireball& fireball : g_monsters.fireballs)
    {
        if (!fireball.active) continue;
        Image* sprite = fireball.motion < 3 ? g_images.bowser_fireball_1 : g_images.bowser_fireball_2;
        DrawSprite(graphics, sprite, fireball.x - g_game.cameraX, fireball.y,
            TILE_SIZE + 30, 30, fireball.vx > 0);
    }
}
static void DrawFireTraps()
{
    Graphics graphics(g_memDC);
    graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);

    for (int i = 0; i < fireTrapCount; i++) 
    {
        if (!fireTraps[i].active) continue;
        const int trapScreenX = fireTraps[i].x - static_cast<int>(g_game.cameraX) - 20;
        if (trapScreenX + TILE_SIZE < 0 || trapScreenX >= SCREEN_WIDTH) continue;  // 화면 밖 함정은 그리지 않는다.

        for (int j = 0; j < fireTraps[i].tileCount; j++)
        {
            int drawX = fireTraps[i].x - g_game.cameraX - 20;
            int drawY = fireTraps[i].y + j * TILE_SIZE;

            graphics.DrawImage(g_images.firetrap, drawX, drawY, TILE_SIZE, TILE_SIZE);
        }
    }
}
// 방향키 함수 
