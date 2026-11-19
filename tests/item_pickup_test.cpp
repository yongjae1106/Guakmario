#include "network.h"
#include "item.h"
#include "player.h"
#include "game.h"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <limits>

// Real pickup logic with isolated game state and captured network/effect calls.
NetContext g_net = {};
GameContext g_game = {};
PlayerContext g_allPlayers[3] = {};
int g_localPlayerIdx = 0;
Item mushroom[MAX_ITEMS], up_mushroom[MAX_ITEMS], star[MAX_ITEMS];
Item flower[MAX_ITEMS], tino[MAX_ITEMS];

static int broadcasts, requests, appliedEffects, statesSent;
static PacketHeader lastHeader;
static ItemPickupData lastPickup;

static void CapturePickup(const void* data, int size)
{
    assert(size == sizeof(PacketHeader) + sizeof(ItemPickupData));
    memcpy(&lastHeader, data, sizeof(lastHeader));
    memcpy(&lastPickup, static_cast<const uint8_t*>(data) + sizeof(lastHeader), sizeof(lastPickup));
    assert(lastHeader.size == size && lastHeader.type == PKT_ITEM_PICKUP);
}

void SendToAll(const void* data, int size)
{
    ++broadcasts;
    CapturePickup(data, size);
}

void SendToHost(const void* data, int size)
{
    assert(statesSent == requests + 1);
    ++requests;
    CapturePickup(data, size);
}

void NetFlushSend() { ++statesSent; }

void ApplyItemPickupEffect(ItemType type)
{
    ++appliedEffects;
    if (type == ITEM_UP_MUSHROOM) ++g_player.mario.life;
}

static void Reset()
{
    broadcasts = requests = appliedEffects = statesSent = 0;
    g_game = GameContext{};
    g_game.isMulti = true;
    g_game.gameStart = true;
    g_game.cameraX = 500;
    g_net = NetContext{};
    g_net.isHost = true;
    g_net.myPlayerId = 0;
    g_net.connectedCount = 3;
    g_net.clientSockets[0] = static_cast<SOCKET>(10);
    g_net.clientSockets[1] = static_cast<SOCKET>(11);
    g_localPlayerIdx = 0;
    for (int id = 0; id < 3; ++id) {
        g_allPlayers[id] = PlayerContext{};
        g_allPlayers[id].mario.x = id == 0 ? 100 : 600;
        g_allPlayers[id].mario.y = 100;
    }
    for (Item* items : {mushroom, up_mushroom, star, flower, tino}) {
        for (int i = 0; i < MAX_ITEMS; ++i) items[i] = Item{};
        items[0] = Item(600, 100, 0, 0, 30, 30, 0, true, false);
    }
}

static void TestConfirmation()
{
    Reset();
    ItemPickupData request = { ITEM_UP_MUSHROOM, 0, 1, 0 };
    assert(!ConfirmItemPickup(2, request));
    assert(up_mushroom[0].active && broadcasts == 0);

    assert(ConfirmItemPickup(1, request));
    assert(!up_mushroom[0].active && broadcasts == 1 && appliedEffects == 0);
    assert(lastHeader.playerId == 0 && lastPickup.approved == 1);
    assert(lastPickup.pickerId == 1 && lastPickup.slot == 0 && lastPickup.itemType == ITEM_UP_MUSHROOM);
    assert(!ConfirmItemPickup(1, request));
    request.pickerId = 2;
    assert(!ConfirmItemPickup(2, request));
    assert(broadcasts == 1);

    Reset();
    const int hostLife = g_player.mario.life;
    HandleMultiplayerItemCollisions();
    assert(!mushroom[0].active && broadcasts == 1 && appliedEffects == 1);
    assert(lastPickup.pickerId == 0 && lastPickup.approved == 1);
    request.pickerId = 0;
    assert(ConfirmItemPickup(0, request));
    assert(!ConfirmItemPickup(0, request) && g_player.mario.life == hostLife + 1);

    Reset();
    g_net.isHost = false;
    g_net.myPlayerId = g_localPlayerIdx = 1;
    g_net.hostSocket = static_cast<SOCKET>(20);
    g_player.mario.x = 100;
    mushroom[0].active = false;
    const int clientLife = g_player.mario.life;
    HandleMultiplayerItemCollisions();
    assert(up_mushroom[0].active && g_player.mario.life == clientLife);
    assert(requests == 1 && statesSent == 1 && lastHeader.playerId == 1 && lastPickup.approved == 0);
    assert(lastPickup.pickerId == 1 && lastPickup.itemType == ITEM_UP_MUSHROOM);
    HandleMultiplayerItemCollisions();
    assert(g_net.waitingForItemPickup && requests == 1 && statesSent == 1);
    const ItemPickupData unrelated = { ITEM_MUSHROOM, 0, 1, 0 };
    ApplyItemPickupResult(unrelated);
    assert(g_net.waitingForItemPickup);
    ApplyItemPickupResult(lastPickup);
    assert(!g_net.waitingForItemPickup && up_mushroom[0].active && appliedEffects == 0);
    HandleMultiplayerItemCollisions();
    assert(g_net.waitingForItemPickup && requests == 2 && statesSent == 2);

    const ItemPickupData approval = { ITEM_UP_MUSHROOM, 0, 1, 1 };
    ApplyItemPickupResult(approval);
    ApplyItemPickupResult(approval);
    assert(!g_net.waitingForItemPickup && !up_mushroom[0].active && g_player.mario.life == clientLife + 1);
    assert(appliedEffects == 1);

    const ItemSpawnData spawn = { ITEM_UP_MUSHROOM, 0, 0, ITEM_FLAG_MOTION, 600, 100, 1, 0, 30, 30 };
    ApplyItemSpawn(spawn);
    assert(!up_mushroom[0].active && up_mushroom[0].motion && up_mushroom[0].vx == 1);
    const ItemPickupData remoteApproval = { ITEM_UP_MUSHROOM, 0, 2, 1 };
    ApplyItemPickupResult(remoteApproval);
    assert(!up_mushroom[0].active && !up_mushroom[0].motion && appliedEffects == 1);
}

int main()
{
    int checks = 0;
    ItemPickupData request = { ITEM_MUSHROOM, 0, 1, 0 };
    auto check = [&](int sender, bool expected) {
        assert(HandleItemPickupRequest(sender, request) == expected);
        ++checks;
    };
    Reset();
    for (int id = 0; id < 3; ++id) {
        request.pickerId = static_cast<uint8_t>(id);
        for (uint8_t type : {ITEM_MUSHROOM, ITEM_UP_MUSHROOM, ITEM_STAR, ITEM_FLOWER, ITEM_TINO}) {
            request.itemType = type;
            check(id, true);
        }
    }
    request = { ITEM_MUSHROOM, 0, 1, 0 };
    // Validation alone must not consume an item, grant effects, or confirm the request.
    const int life = g_allPlayers[1].mario.life;
    check(1, true); check(1, true);
    assert(mushroom[0].active && g_allPlayers[1].mario.life == life);
    assert(g_allPlayers[1].mario.form == FORM_SMALL && !g_allPlayers[1].mario.star);
    assert(request.approved == 0 && g_game.gameState == GAME_RUNNING);

    check(-1, false); check(3, false); check(0, false); check(2, false);
    request.pickerId = 255; check(1, false); request.pickerId = 1;
    request.approved = 1; check(1, false); request.approved = 255; check(1, false); request.approved = 0;
    request.itemType = 5; check(1, false); request.itemType = 255; check(1, false); request.itemType = ITEM_MUSHROOM;
    request.slot = MAX_ITEMS; check(1, false); request.slot = 255; check(1, false);
    request.slot = MAX_ITEMS - 1; check(1, false);
    mushroom[MAX_ITEMS - 1] = mushroom[0]; check(1, true); request.slot = 0;
    g_net.clientSockets[0] = INVALID_SOCKET; check(1, false); Reset();
    g_net.clientSockets[0] = INVALID_SOCKET; request.pickerId = 2; check(2, true); Reset(); request.pickerId = 1;
    g_net.isHost = false; check(1, false); Reset();
    g_net.myPlayerId = 1; check(1, false); Reset();
    g_localPlayerIdx = 1; check(1, false); Reset();
    g_game.isMulti = false; check(1, false); Reset();
    g_game.isRenderTest = true; check(1, false); Reset();
    g_game.gameStart = false; check(1, false); Reset();
    for (GameState state : {GAME_TRANSFORMING, GAME_VICTORY, GAME_CLEAR, GAME_OVER}) {
        g_game.gameState = state; check(1, false);
    }
    Reset();
    g_allPlayers[1].mario.isDead = true; check(1, false); Reset();
    g_allPlayers[1].mario.gameover = true; check(1, false); Reset();
    g_allPlayers[1].mario.isTransforming = true; check(1, false); Reset();
    g_allPlayers[1].mario.width = 0; check(1, false); Reset();
    g_allPlayers[1].mario.height = -1; check(1, false); Reset();
    mushroom[0].active = false; check(1, false); Reset();
    mushroom[0].motion = true; check(1, false); Reset();
    mushroom[0].width = 0; check(1, false); Reset();
    mushroom[0].height = -1; check(1, false); Reset();
    g_allPlayers[1].mario.x = 100; check(1, false); Reset(); // remote x must not add host camera
    g_allPlayers[1].mario.x = 560; check(1, false); // touching left edge
    g_allPlayers[1].mario.x = 561; check(1, true);
    g_allPlayers[1].mario.x = 630; check(1, false); Reset();
    g_allPlayers[1].mario.y = 60; check(1, false);
    g_allPlayers[1].mario.y = 61; check(1, true);
    g_allPlayers[1].mario.y = 130; check(1, false); Reset();
    request.pickerId = 0; g_game.cameraX = 0; check(0, false);
    g_game.cameraX = 500.5; check(0, true);
    g_game.cameraX = std::numeric_limits<double>::quiet_NaN(); check(0, false);
    g_game.cameraX = std::numeric_limits<double>::infinity(); check(0, false); Reset();
    request.pickerId = 1;
    g_allPlayers[1].mario.x = (std::numeric_limits<int>::max)();
    mushroom[0].x = (std::numeric_limits<int>::max)(); check(1, true);
    g_allPlayers[1].mario.x = (std::numeric_limits<int>::min)(); check(1, false);
    printf("Item pickup validation: %d checks passed\n", checks);
    TestConfirmation();
    puts("Item pickup confirmation and duplicate tests passed");
}
