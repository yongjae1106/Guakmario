#include "network.h" // winsock2 must precede windows.h in item.h
#include "item.h"
#include "player.h"
#include "game.h"
#include <cmath>

namespace {
constexpr ItemType itemTypes[] = { ITEM_MUSHROOM, ITEM_UP_MUSHROOM, ITEM_STAR, ITEM_FLOWER, ITEM_TINO };

Item* FindPickupItem(uint8_t itemType, uint8_t slot)
{
    if (slot >= MAX_ITEMS) return nullptr;
    switch (itemType) {
    case ITEM_MUSHROOM:    return &mushroom[slot];
    case ITEM_UP_MUSHROOM: return &up_mushroom[slot];
    case ITEM_STAR:        return &star[slot];
    case ITEM_FLOWER:      return &flower[slot];
    case ITEM_TINO:        return &tino[slot];
    default:              return nullptr;
    }
}

bool IsOnlineGame()
{
    return g_game.isMulti && !g_game.isRenderTest && g_game.gameStart;
}

bool CanPickUpItems(const Player& player)
{
    return !player.isDead && !player.gameover && !player.isTransforming &&
        player.width > 0 && player.height > 0;
}

bool OverlapsPickup(const Player& player, double worldX, const Item& item)
{
    if (!item.active || item.motion || item.width <= 0 || item.height <= 0 ||
        !std::isfinite(worldX)) return false;

    // Promote extents before adding them. Touching an edge is not a pickup.
    return worldX < static_cast<double>(item.x) + item.width &&
        worldX + player.width > item.x &&
        static_cast<double>(player.y) < static_cast<double>(item.y) + item.height &&
        static_cast<double>(player.y) + player.height > item.y;
}

void SendItemPickupPacket(const ItemPickupData& pickup)
{
    const ItemPickupPacket packet = {
        { sizeof(ItemPickupPacket), PKT_ITEM_PICKUP, static_cast<uint8_t>(g_net.myPlayerId) },
        pickup
    };
    if (g_net.isHost) SendToAll(&packet, sizeof(packet));
    else SendToHost(&packet, sizeof(packet));
}
}

bool HandleItemPickupRequest(int senderPlayerId, const ItemPickupData& request)
{
    // Single player keeps its existing collision/effect path.
    if (!IsOnlineGame() || g_game.gameState != GAME_RUNNING || !g_net.isHost ||
        g_net.myPlayerId != 0 || g_localPlayerIdx != 0) return false;

    if (senderPlayerId < 0 || senderPlayerId >= 3 ||
        request.pickerId != senderPlayerId || request.approved != 0) return false;
    // Slot 0 is the host. A connected count alone cannot identify a live slot.
    if (senderPlayerId != 0 &&
        g_net.clientSockets[senderPlayerId - 1] == INVALID_SOCKET) return false;

    const Player& player = g_allPlayers[senderPlayerId].mario;
    const Item* item = FindPickupItem(request.itemType, request.slot);
    // Host x is screen-relative; remote x and all item x are world-relative.
    const double worldX = static_cast<double>(player.x) +
        (senderPlayerId == 0 ? g_game.cameraX : 0.0);
    return item && CanPickUpItems(player) && OverlapsPickup(player, worldX, *item);
}

bool ConfirmItemPickup(int senderPlayerId, const ItemPickupData& request)
{
    if (!HandleItemPickupRequest(senderPlayerId, request)) return false;

    // Validate and consume on the same game thread before any effects or broadcast.
    FindPickupItem(request.itemType, request.slot)->active = false;
    if (request.itemType == ITEM_UP_MUSHROOM) AwardMultiplayerLife();
    if (senderPlayerId == g_localPlayerIdx)
        ApplyItemPickupEffect(static_cast<ItemType>(request.itemType));

    ItemPickupData approved = request;
    approved.approved = 1;
    SendItemPickupPacket(approved);
    return true;
}

void ApplyItemPickupResult(const ItemPickupData& result)
{
    if (!IsOnlineGame() || g_net.isHost || result.approved > 1 || result.pickerId >= 3)
        return;

    const bool isLocalPicker = result.pickerId == g_net.myPlayerId &&
        result.pickerId == g_localPlayerIdx;
    const ItemPickupData& pending = g_net.pendingItemPickup;
    const bool matchesPending = isLocalPicker && g_net.waitingForItemPickup &&
        result.itemType == pending.itemType && result.slot == pending.slot;
    if (matchesPending) g_net.waitingForItemPickup = false;

    if (result.approved == 0) return;
    Item* item = FindPickupItem(result.itemType, result.slot);
    if (!item || (!item->active && !item->motion && !matchesPending)) return;
    item->active = false;
    item->motion = false;
    if (isLocalPicker) ApplyItemPickupEffect(static_cast<ItemType>(result.itemType));
}

void ApplyItemSpawn(const ItemSpawnData& spawn)
{
    if (!IsOnlineGame() || g_net.isHost || spawn.width == 0 || spawn.height == 0 ||
        spawn.x < 0 || spawn.x >= MAP_WIDTH * TILE_SIZE ||
        spawn.y < 0 || spawn.y >= MAP_HEIGHT * TILE_SIZE ||
        (spawn.flags != ITEM_FLAG_ACTIVE && spawn.flags != ITEM_FLAG_MOTION) ||
        spawn.spawnMotion >= 18) return;

    Item* item = FindPickupItem(spawn.itemType, spawn.slot);
    if (!item) return;
    // 호스트가 재사용한 슬롯에 이전 아이템이 잠시 남아 있어도 새 생성을 적용한다.
    *item = Item(spawn.x, spawn.y, spawn.vx, spawn.vy, spawn.width, spawn.height,
        spawn.spawnMotion, spawn.flags == ITEM_FLAG_ACTIVE, spawn.flags == ITEM_FLAG_MOTION);
}

void HandleMultiplayerItemCollisions()
{
    const int playerId = g_localPlayerIdx;
    if (!IsOnlineGame() || g_game.gameState != GAME_RUNNING ||
        playerId < 0 || playerId >= 3 || playerId != g_net.myPlayerId ||
        (!g_net.isHost && (g_net.hostSocket == INVALID_SOCKET || g_net.waitingForItemPickup))) return;

    const Player& player = g_allPlayers[playerId].mario;
    if (!CanPickUpItems(player)) return;
    const double worldX = player.x + g_game.cameraX;

    for (ItemType type : itemTypes) {
        for (uint8_t slot = 0; slot < MAX_ITEMS; ++slot) {
            if (!OverlapsPickup(player, worldX, *FindPickupItem(type, slot))) continue;

            const ItemPickupData request = { type, slot, static_cast<uint8_t>(playerId), 0 };
            if (g_net.isHost) {
                ConfirmItemPickup(playerId, request);
            } else {
                // Wait for the result before requesting another item.
                g_net.pendingItemPickup = request;
                g_net.waitingForItemPickup = true;
                NetFlushSend();
                SendItemPickupPacket(request);
            }
            return;
        }
    }
}
