#include "network.h" // winsock2 must precede windows.h in item.h
#include "item.h"
#include "player.h"
#include "game.h"
#include <cmath>

namespace {
const Item* FindPickupItem(uint8_t itemType, uint8_t slot)
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
}

bool HandleItemPickupRequest(int senderPlayerId, const ItemPickupData& request)
{
    // Single player keeps its existing collision/effect path.
    if (!g_game.isMulti || g_game.isRenderTest || !g_game.gameStart ||
        g_game.gameState != GAME_RUNNING || !g_net.isHost ||
        g_net.myPlayerId != 0 || g_localPlayerIdx != 0) return false;

    if (senderPlayerId < 0 || senderPlayerId >= 3 ||
        request.pickerId != senderPlayerId || request.approved != 0) return false;
    // Slot 0 is the host. A connected count alone cannot identify a live slot.
    if (senderPlayerId != 0 &&
        g_net.clientSockets[senderPlayerId - 1] == INVALID_SOCKET) return false;

    const Player& player = g_allPlayers[senderPlayerId].mario;
    if (player.isDead || player.gameover || player.isTransforming ||
        player.width <= 0 || player.height <= 0) return false;

    const Item* item = FindPickupItem(request.itemType, request.slot);
    if (item == nullptr || !item->active || item->motion ||
        item->width <= 0 || item->height <= 0) return false;

    // Host x is screen-relative; remote x and all item x are world-relative.
    const double worldX = static_cast<double>(player.x) +
        (senderPlayerId == 0 ? g_game.cameraX : 0.0);
    if (!std::isfinite(worldX)) return false;
    // Promote before adding extents to avoid integer overflow on malformed state.
    // Strict overlap matches IsColliding: touching an edge is not a pickup.
    return worldX < static_cast<double>(item->x) + item->width &&
        worldX + player.width > item->x &&
        static_cast<double>(player.y) < static_cast<double>(item->y) + item->height &&
        static_cast<double>(player.y) + player.height > item->y;
}
