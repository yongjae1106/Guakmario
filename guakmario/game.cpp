#include "network.h"  // winsock2.h → windows.h 순서 보장 (반드시 최상단)
#include <tchar.h>
#include "data.h"
#include "map.h"
#include "monster.h"
#include "item.h"
#include "sound.h"
#include "player.h"
#include "collision.h"
#include "game.h"
#include "config.h"
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

using namespace Gdiplus;

namespace {
enum class RespawnPhase { Ready, Waiting, AwaitingSnapshot };

struct RespawnState {
    RespawnPhase phase = RespawnPhase::Ready;
    DWORD deathTime = 0;
};

RespawnState respawnStates[3];
constexpr uint8_t ALL_PLAYERS_ARRIVED = 0x07;
uint8_t arrivedPlayers = 0;

void ApplySharedLives(int lives)
{
    g_sharedLives = lives;
    for (PlayerContext& player : g_allPlayers)
        player.mario.life = lives;
}

bool IsMultiplayerHostPlaying()
{
    return IsMultiplayerGame() && g_game.gameStart && g_net.isHost &&
        g_game.gameState == GAME_RUNNING;
}

bool IsCastleStageRunning()
{
    return g_game.stage == CASTLE_STAGE && g_game.gameState == GAME_RUNNING;
}

void ApplyCastleGateOpened()
{
    OpenCastleGate();
    FinishBowserDefeat();
    for (Fireball& fireball : g_monsters.fireballs) fireball.active = false;
}

void ConfirmCastleGateOpened()
{
    const Bowser& bowser = g_monsters.bowser;
    if (!IsMultiplayerHostPlaying() || !IsCastleStageRunning() || IsCastleGateOpen() ||
        bowser.isAlive || !bowser.isDead) return;

    ApplyCastleGateOpened();
    NetSendGameEvent(EVT_CASTLE_GATE_OPENED, CASTLE_STAGE);
}

void ApplyMultiplayerGameOver()
{
    if (g_game.gameState == GAME_OVER) return;

    ApplySharedLives(0);
    g_game.gameState = GAME_OVER;
    g_game.deadStartTime = GetTickCount();
    g_game.multiplayerDeathMotionStarted = false;
    g_player.mario.isDead = false;
    g_player.mario.isTransforming = false;
    g_player.mario.gameover = true;
    if (g_pBGMBuffer) g_pBGMBuffer->Stop();
    PlaySoundBuffer(gameover_Sound);
}

void RecordMultiplayerDeath(int playerId)
{
    if (!IsMultiplayerHostPlaying() || g_sharedLives <= 0 ||
        playerId < 0 || playerId >= 3) return;

    RespawnState& respawn = respawnStates[playerId];
    if (respawn.phase != RespawnPhase::Ready || !g_allPlayers[playerId].mario.isDead) return;

    respawn = { RespawnPhase::Waiting, GetTickCount() };
    ApplySharedLives(g_sharedLives - 1);
    NetSendGameEvent(EVT_LIVES_CHANGED, static_cast<uint8_t>(g_sharedLives));
    if (g_sharedLives == 0)
    {
        ApplyMultiplayerGameOver();
        NetSendGameEvent(EVT_GAME_OVER, 0);
    }
}

void UpdateMultiplayerRespawns()
{
    if (!IsMultiplayerHostPlaying() || g_sharedLives <= 0) return;

    const DWORD now = GetTickCount();
    for (int id = 0; id < 3; ++id)
    {
        RespawnState& respawn = respawnStates[id];
        if (respawn.phase != RespawnPhase::Waiting ||
            now - respawn.deathTime < DEATH_REVIVE_DELAY_MS) continue;

        if (id == g_localPlayerIdx)
        {
            resurrection();
            respawn = {};
        }
        else
        {
            // 이전 생명의 사망 알림은 부활 상태를 수신할 때까지 무시한다.
            respawn.phase = RespawnPhase::AwaitingSnapshot;
        }
        NetSendGameEvent(EVT_PLAYER_REVIVED, static_cast<uint8_t>(id));
    }
}

void UpdateMultiplayerDeathMotion()
{
    const DWORD elapsed = GetTickCount() - g_game.deadStartTime;
    if (elapsed < 500 || elapsed >= DEAD_ANIMATION_MS) return;

    if (!g_game.multiplayerDeathMotionStarted)
    {
        g_player.mario.vy -= 14;
        g_game.multiplayerDeathMotionStarted = true;
    }
    UpdateDeadMotion();
}

void FinishMultiplayerMatch()
{
    NetShutdown();
    g_game.gameStart = false;
    g_game.gameState = GAME_RUNNING;
    g_game.isMulti = false;
    g_game.multiStartPending = false;
    g_game.gameClearText = false;
    g_game.multiplayerArrived = false;
    g_game.stage = 1;
    g_localPlayerIdx = 0;
    g_player = {};
}

void StopLocalPlayerActions()
{
    Player& player = g_player.mario;
    player.vx = 0;
    player.vy = 0;
    player.isWalking = false;
    player.isJumping = false;
    player.isFlying = false;
    player.walk_motion = 0;
    player.fire_motion = false;
    player.tino_motion = false;
    player.tino_fire_motion = false;
    for (bool& pressed : g_player.keyState) pressed = false;
}

void LoadMultiplayerStage(int stage)
{
    arrivedPlayers = 0;
    for (RespawnState& respawn : respawnStates) respawn = {};
    g_game.multiplayerArrived = false;
    g_game.gameClearText = false;
    g_game.multiplayerDeathMotionStarted = false;
    g_game.stage = stage;
    g_game.gameState = GAME_RUNNING;
    g_net.waitingForItemPickup = false;

    StopLocalPlayerActions();
    LoadStage();
    ApplySharedLives(g_sharedLives);
}

void BeginMultiplayerStageClear()
{
    if (g_pBGMBuffer) g_pBGMBuffer->Stop();
    if (g_game.stage == CASTLE_STAGE)
    {
        g_game.gameState = GAME_CLEAR;
        g_game.clearStart = GetTickCount();
        PlaySoundBuffer(world_clear_Sound);
    }
    else
    {
        g_game.gameState = GAME_VICTORY;
        g_game.victoryStart = GetTickCount();
        PlaySoundBuffer(stage_clear_Sound);
    }
}

void RecordMultiplayerArrival(int playerId)
{
    if (!IsMultiplayerHostPlaying() || g_net.connectedCount != 3 ||
        playerId < 0 || playerId >= 3) return;

    const uint8_t playerBit = static_cast<uint8_t>(1 << playerId);
    const Player& player = g_allPlayers[playerId].mario;
    if ((arrivedPlayers & playerBit) || player.isDead || player.gameover ||
        player.isTransforming || !IsPlayerAtStageGoal(playerId)) return;

    arrivedPlayers |= playerBit;
    if (arrivedPlayers == ALL_PLAYERS_ARRIVED)
    {
        BeginMultiplayerStageClear();
        NetSendGameEvent(EVT_STAGE_CLEAR, static_cast<uint8_t>(g_game.stage));
    }
}

void WaitAtMultiplayerGoal()
{
    const int playerId = g_net.myPlayerId;
    if (playerId < 0 || playerId >= 3 || !IsPlayerAtStageGoal(playerId)) return;

    g_game.multiplayerArrived = true;
    StopLocalPlayerActions();

    if (g_net.isHost)
        RecordMultiplayerArrival(playerId);
    else
    {
        NetFlushSend();
        NetSendGameEvent(EVT_PLAYER_ARRIVED, static_cast<uint8_t>(playerId));
    }
}

void WalkAfterMultiplayerClear()
{
    Player& player = g_player.mario;
    UpdatePlayerTransformation(player, GetTickCount());
    const int previousWorldX = player.x + static_cast<int>(g_game.cameraX);
    player.direction = DIR_RIGHT;
    player.vx = 1;
    if (g_game.gameState == GAME_CLEAR)
    {
        const int remaining = CASTLE_PRINCESS_COLUMN * TILE_SIZE - previousWorldX - player.width;
        if (remaining <= 0)
        {
            player.isWalking = false;
            player.vx = 0;
            return;
        }
        player.vx = remaining < PLAYER_WALK_SPEED ? remaining : PLAYER_WALK_SPEED;
    }
    UpdatePlayer();
    player.isWalking = player.x + static_cast<int>(g_game.cameraX) > previousWorldX;
}

bool CanUpdateLocalPlayer()
{
    return !g_player.mario.isTransforming && (!IsMultiplayerGame() ||
        (g_game.gameState == GAME_RUNNING && !g_player.mario.isDead && !g_game.multiplayerArrived));
}
}

bool IsMultiplayerGame()
{
    return g_game.isMulti && !g_game.isRenderTest;
}

void StartMultiplayerMatch()
{
    for (PlayerContext& player : g_allPlayers) player = {};
    LoadMultiplayerStage(1);
    g_game.gameStart = true;
}

void ReportMultiplayerDeath()
{
    const int playerId = g_net.myPlayerId;
    if (!g_game.gameStart || playerId < 0 || playerId >= 3) return;
    if (g_net.isHost)
    {
        RecordMultiplayerDeath(playerId);
    }
    else
    {
        NetFlushSend();
        NetSendGameEvent(EVT_PLAYER_DIED, static_cast<uint8_t>(playerId));
    }
}

void ReportBowserDefeat()
{
    if (g_game.stage != CASTLE_STAGE) return;
    if (!IsMultiplayerGame())
    {
        OpenCastleGate();
        return;
    }
    ConfirmCastleGateOpened();
}

void AwardMultiplayerLife()
{
    if (!IsMultiplayerHostPlaying() || g_sharedLives <= 0 || g_sharedLives >= UINT8_MAX) return;
    ApplySharedLives(g_sharedLives + 1);
    NetSendGameEvent(EVT_LIVES_CHANGED, static_cast<uint8_t>(g_sharedLives));
}

void HandleMultiplayerPlayerState(int playerId, int previousCoins)
{
    if (!IsMultiplayerHostPlaying() || playerId < 1 || playerId >= 3) return;

    Player& player = g_allPlayers[playerId].mario;
    RespawnState& respawn = respawnStates[playerId];
    if (!player.isDead && respawn.phase == RespawnPhase::AwaitingSnapshot) respawn = {};
    if (respawn.phase == RespawnPhase::Ready && previousCoins <= COIN_1UP_THRESHOLD &&
        player.coin > COIN_1UP_THRESHOLD)
        AwardMultiplayerLife();
}

void HandleMultiplayerGameEvent(int senderPlayerId, const GameEventData& event)
{
    if (!IsMultiplayerGame() || !g_game.gameStart) return;

    if (g_net.isHost)
    {
        if (event.eventType == EVT_PLAYER_DIED && event.param == senderPlayerId)
            RecordMultiplayerDeath(senderPlayerId);
        else if (event.eventType == EVT_PLAYER_ARRIVED && event.param == senderPlayerId)
            RecordMultiplayerArrival(senderPlayerId);
        return;
    }

    if (senderPlayerId != 0) return;
    switch (event.eventType)
    {
    case EVT_CASTLE_GATE_OPENED:
        if (IsCastleStageRunning() && event.param == CASTLE_STAGE && !IsCastleGateOpen())
            ApplyCastleGateOpened();
        break;
    case EVT_STAGE_CLEAR:
        if (g_game.gameState == GAME_RUNNING && g_game.multiplayerArrived && event.param == g_game.stage)
            BeginMultiplayerStageClear();
        break;
    case EVT_STAGE_LOAD:
        if (g_game.gameState == GAME_VICTORY && event.param == g_game.stage + 1 && event.param <= 3)
        {
            LoadMultiplayerStage(event.param);
            NetSendGameEvent(EVT_STAGE_READY, event.param);
        }
        break;
    case EVT_LIVES_CHANGED:
        if (g_game.gameState != GAME_OVER) ApplySharedLives(event.param);
        break;
    case EVT_GAME_OVER:
        ApplyMultiplayerGameOver();
        break;
    case EVT_PLAYER_REVIVED:
        if (g_game.gameState == GAME_RUNNING && event.param == g_net.myPlayerId &&
            g_player.mario.isDead && g_sharedLives > 0)
            resurrection();
        break;
    }
}

static void handle_transform(MarioForm targetForm, void (*primary_fn)(), void (*fallback_fn)())
{
    g_player.mario.god = true;
    DWORD now = GetTickCount();
    if (now - g_game.transformStartTime >= TRANSFORM_DURATION_MS)
    {
        if (g_player.mario.form != targetForm)
            primary_fn();
        else
        {
            fallback_fn();
            g_player.mario.godStartTime = now;
        }
        g_game.gameState = GAME_RUNNING;  // 다시 정상 진행
    }
}

static void UpdateGameState()
{
    // 변신 모션 (mushroom / flower / tino)
    if (g_game.gameState == GAME_TRANSFORMING)
    {
        switch (g_game.transformTarget)
        {
        case FORM_BIG:    handle_transform(FORM_BIG,    TransformToBig,    TransformToSmall); break;
        case FORM_FLOWER: handle_transform(FORM_FLOWER, TransformToFlower, TransformToBig);   break;
        case FORM_TINO:   handle_transform(FORM_TINO,   TransformToTino,   TransformToBig);   break;
        default: break;
        }
        return; // 게임 상태 업데이트 생략해서 "멈춘 듯한" 연출
    }
    // 승리 모션
    else if (g_game.gameState == GAME_VICTORY)
    {
        DWORD now = GetTickCount();
        if (IsMultiplayerGame())
        {
            if (now - g_game.victoryStart < VICTORY_NEXT_STAGE_MS)
                WalkAfterMultiplayerClear();
            else if (g_net.isHost)
            {
                LoadMultiplayerStage(g_game.stage + 1);
                NetSendGameEvent(EVT_STAGE_LOAD, static_cast<uint8_t>(g_game.stage));
            }
            return;
        }
        g_player.mario.vx = 1;
        g_player.mario.x += g_player.mario.vx;
        if (now - g_game.victoryStart >= VICTORY_NEXT_STAGE_MS)
        {
            g_game.stage++;
            LoadStage();
            g_game.gameState = GAME_RUNNING;
        }
    }
    // 최종 승리 모션
    else if (g_game.gameState == GAME_CLEAR)
    {
        if (IsMultiplayerGame())
        {
            const DWORD elapsed = GetTickCount() - g_game.clearStart;
            if (elapsed >= CLEAR_RESET_MS)
                FinishMultiplayerMatch();
            else if (elapsed < CLEAR_WALK_MS)
                WalkAfterMultiplayerClear();
            else
            {
                g_game.gameClearText = true;
                g_player.mario.isWalking = false;
                g_player.mario.vx = 0;
            }
            return;
        }
        if (GetTickCount() - g_game.clearStart >= 0 && GetTickCount() - g_game.clearStart <= CLEAR_WALK_MS)
        {
            g_player.mario.vx = 1;
            g_player.mario.x += g_player.mario.vx;
        }
        else
        {
            g_game.gameClearText = true;
            g_player.mario.isWalking = false;
            g_player.mario.vx = 0;
        }
        if (GetTickCount() - g_game.clearStart >= CLEAR_RESET_MS)
        {
            g_game.stage = 1;
            g_game.gameClearText = false;
            g_game.gameStart = false;
            ResetMario(5, 0);
            g_game.gameState = GAME_RUNNING;
        }
    }
    // 시망모션
    else if (g_game.gameState == GAME_OVER)
    {
        DWORD now = GetTickCount();
        if (IsMultiplayerGame())
        {
            if (now - g_game.deadStartTime >= DEATH_REVIVE_DELAY_MS)
                FinishMultiplayerMatch();
            return;
        }

        if (g_player.mario.isDead)
        {
            static bool motion1;
            if (motion1) UpdateDeadMotion();
            if (now - g_game.deadStartTime >= 500 && !motion1)
            {
                g_player.mario.vy -= 14;
                motion1 = true;
            }
            if (now - g_game.deadStartTime >= DEAD_ANIMATION_MS)
            {
                if (g_player.mario.life <= 0)
                {
                    g_game.stage = 1;
                    g_game.gameClearText = false;
                    g_game.gameStart = false;
                    ResetMario(5, 0);
                    g_game.gameState = GAME_RUNNING;
                    return;
                }
                g_player.mario.isDead = false;
                motion1 = false;
            }
            return;
        }
        if (now - g_game.deadStartTime >= DEATH_REVIVE_DELAY_MS)
        {
            g_game.gameState = GAME_RUNNING;  // 다시 정상 진행
            resurrection();
        }

        return;
    }

    // 무적처리
    if (IsMultiplayerGame() && g_player.mario.isDead)
    {
        UpdateMultiplayerDeathMotion();
    }
    else if (g_player.mario.god)
    {
        UpdateGodMode(g_player.mario.godStartTime);
    }
    // 스타 무적처리
    else if (g_player.mario.star)
    {
        UpdateStarMode(g_game.starStartTime);
    }

    if ((!IsMultiplayerGame() || !g_player.mario.isDead) &&
        g_player.mario.coin > COIN_1UP_THRESHOLD)
    {
        PlaySoundBuffer(up_Sound);
        if (IsMultiplayerGame())
        {
            if (g_net.isHost) AwardMultiplayerLife();
            else NetFlushSend();
        }
        else
        {
            g_player.mario.life++;
        }
        g_player.mario.coin = 0;
    }

    if (g_game.isRenderTest)
    {
        const DWORD now = GetTickCount();
        for (PlayerContext& context : g_allPlayers)
            UpdatePlayerTransformation(context.mario, now);
    }
    else if (g_game.isMulti)
    {
        UpdatePlayerTransformation(g_player.mario, GetTickCount());
    }
    if (CanUpdateLocalPlayer())
    {
        UpdateMario_motion();
        if (g_player.mario.tino_motion) TinoAttack();  // 바이트 모션 동안 매 프레임 히트박스 적용
        UpdatePlayer();
        if (IsMultiplayerGame() && CanUpdateLocalPlayer()) WaitAtMultiplayerGoal();
    }
    if (IsMultiplayerGame() && g_game.gameState != GAME_RUNNING) return;

    UpdateAllItems();
    UpdateAllMonsters();
    UpdateAllShots();

    if (CanUpdateLocalPlayer()) HandleItemCollisions();
    if (CanUpdateLocalPlayer())
    {
        CheckCoinCollision();
        HandleAllMonsterCollisions();
    }
    HandleShotCollisions();
    if (!IsMultiplayerGame() && CanUpdateLocalPlayer())
    {
        CheckFlagCollision();
        if (g_game.stage == 3) CheckClearCollision();
    }
}

void UpdateGame()
{
    const bool networkGame = IsMultiplayerGame();
    if (networkGame)
    {
        NetPollPackets();
        if (g_net.connectedCount < 3 && (g_game.gameState == GAME_RUNNING || g_game.gameState == GAME_VICTORY))
        {
            FinishMultiplayerMatch();
            return;
        }
        UpdateMultiplayerRespawns();
    }

    UpdateGameState();

    // 변신,사망 연출로 상태 갱신이 일찍 끝나도 이번 틱의 결과를 전송
    if (networkGame && g_game.gameStart) NetFlushSend();
}

void TickTimer()
{
    if (IsMultiplayerGame() && (g_player.mario.isDead || g_game.multiplayerArrived)) return;
    g_game.stage_time--;
    if (g_game.stage_time == 0)
    {
        if (IsMultiplayerGame()) g_game.stage_time = STAGE_TIME_LIMIT;
        dead();
    }
}

void ResetMonsters()
{
    g_monsters = MonsterState{};
}
void ResetItems()
{
    for (int i = 0; i < MAX_ITEMS; i++) // 아이템 초기화
    {
        mushroom[i].active = false;
        mushroom[i].motion = false;

        up_mushroom[i].active = false;
        up_mushroom[i].motion = false;

        flower[i].active = false;
        flower[i].motion = false;

        tino[i].active = false;
        tino[i].motion = false;

        star[i].active = false;
        star[i].motion = false;
    }
    for (int i = 0; i < MAX_SHOT; i++)
    {
        fireball[i].active = false;
        fireball[i].fade = false;
        tinofire[i].active = false;
        tinofire_effect[i].active = false;
        tinofire[i].fade = false;
    }
}
struct StageData {
    const char*       bgm;
    void            (*initMap)();
    int           (*mapData)[MAP_WIDTH];
    void            (*extraInit)();
};

static void InitStage3Extras() { InitFireTraps(); InitBowser(); }

static const StageData stages[] = {
    { nullptr,                                     nullptr,  nullptr, nullptr           }, // [0] 미사용
    { "resource\\sound\\bgm\\GroundTheme.wav",     InitMap,  map1,   nullptr            },
    { "resource\\sound\\bgm\\GroundTheme.wav",     InitMap2, map2,   InitAngelTurtles   },
    { "resource\\sound\\bgm\\CastleTheme.wav",     InitMap3, map3,   InitStage3Extras   },
};

void LoadStage()
{
    int s = g_game.stage;
    const StageData& sd = stages[s];

    PlayBGM(sd.bgm);
    ResetItems();
    ResetTraps();
    g_player.mario.god = false;
    g_player.mario.star = false;
    g_player.mario.isTransforming = false;

    ResetMonsters();
    g_game.cameraX = 0;
    sd.initMap();
    currentMap = sd.mapData;

    InitMonsters(s);
    InitTurtles(s);
    if (sd.extraInit) sd.extraInit();

    g_player.mario.x = 100;
    g_player.mario.y = 300;
    g_game.stage_time = STAGE_TIME_LIMIT;

    // 멀티/렌더 테스트 모드: 나머지 2개 슬롯을 화면 내 다른 위치에 배치
    if (g_game.isMulti)
    {
        static const CharacterType charTypes[3] = { CHAR_MARIO, CHAR_LUIGI, CHAR_TOAD };
        for (int i = 0; i < 3; i++)
        {
            if (i == g_localPlayerIdx) continue;
            g_allPlayers[i].mario = {};
            g_allPlayers[i].mario.x        = 100 + i * 200;
            g_allPlayers[i].mario.y        = 300;
            g_allPlayers[i].mario.width    = 40;
            g_allPlayers[i].mario.height   = 40;
            g_allPlayers[i].mario.charType = charTypes[i];
        }
        // 로컬 플레이어도 슬롯에 맞는 캐릭터 타입 설정
        g_allPlayers[g_localPlayerIdx].mario.charType = charTypes[g_localPlayerIdx];
    }
}

