#include "player.h"
#include "game.h"
#include <cassert>
#include <cstdio>

PlayerContext g_allPlayers[3] = {};
int g_localPlayerIdx = 0;
GameContext g_game = {};

void SetStage_BGM() {}

static void TestIndependentTransformations()
{
    g_game.isMulti = true;
    Player& first = g_allPlayers[0].mario;
    Player& second = g_allPlayers[1].mario;
    first.x = 120;
    first.y = 300;
    first.vy = -5;
    first.god = true;
    first.godStartTime = 77;
    StartPlayerTransformation(FORM_FLOWER);
    const DWORD firstStart = first.transformStartTime;

    assert(g_game.gameState == GAME_RUNNING);
    assert(first.isTransforming && !second.isTransforming);
    assert(first.form == FORM_SMALL && first.transformTarget == FORM_FLOWER);
    assert(first.god && first.godStartTime == 77);
    StartPlayerTransformation(FORM_TINO);
    assert(first.transformTarget == FORM_FLOWER && first.transformStartTime == firstStart);

    g_localPlayerIdx = 1;
    StartPlayerTransformation(FORM_TINO);
    assert(second.isTransforming && second.transformTarget == FORM_TINO);

    UpdatePlayerTransformation(first, firstStart + TRANSFORM_DURATION_MS - 1);
    assert(first.isTransforming && first.x == 120 && first.y == 300 && first.vy == -5);
    UpdatePlayerTransformation(first, firstStart + TRANSFORM_DURATION_MS);
    assert(!first.isTransforming && first.form == FORM_FLOWER);
    assert(first.x == 120 && first.y + first.height == 300 + TILE_SIZE && first.vy == -5);
    assert(first.god && first.godStartTime == 77 && second.isTransforming);

    UpdatePlayerTransformation(second, second.transformStartTime + TRANSFORM_DURATION_MS);
    assert(!second.isTransforming && second.form == FORM_TINO && !second.god);
}

static void TestDamageAndSinglePlayer()
{
    g_localPlayerIdx = 0;
    Player& player = g_player.mario;
    player = {};
    player.form = FORM_BIG;
    player.height = 2 * TILE_SIZE;
    StartPlayerTransformation(FORM_SMALL);
    const DWORD end = player.transformStartTime + TRANSFORM_DURATION_MS;
    UpdatePlayerTransformation(player, end);
    assert(!player.isTransforming && player.form == FORM_SMALL && player.height == TILE_SIZE);
    assert(player.y == 300 + TILE_SIZE && player.god && player.godStartTime == end);

    g_game = {};
    player = {};
    StartPlayerTransformation(FORM_BIG);
    assert(g_game.gameState == GAME_TRANSFORMING && g_game.transformTarget == FORM_BIG);
    assert(!player.isTransforming && player.y == 300 - (TILE_SIZE + 1));
}

int main()
{
    TestIndependentTransformations();
    TestDamageAndSinglePlayer();
    std::puts("Player transformation tests passed");
}
