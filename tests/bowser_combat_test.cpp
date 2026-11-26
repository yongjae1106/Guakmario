#include "network.h"
#include "game.h"
#include "map.h"
#include "monster.h"
#include "player.h"
#include "sound.h"
#include <cassert>
#include <cstdio>

static void ResetCombat()
{
    g_game = {};
    g_game.isMulti = true;
    g_game.gameStart = true;
    g_game.stage = CASTLE_STAGE;
    g_game.cameraX = 500;
    g_net = {};
    g_net.isHost = true;
    g_net.hostSocket = INVALID_SOCKET;
    for (SOCKET& socket : g_net.clientSockets) socket = INVALID_SOCKET;
    g_net.myPlayerId = g_localPlayerIdx = 0;
    for (PlayerContext& player : g_allPlayers) player = {};
    for (Shot& shot : fireball) shot = {};
    for (Shot& shot : tinofire) shot = {};
    for (RemoteShots& pool : remoteShots) pool = {};
    g_monsters = {};
    Bowser& bowser = g_monsters.bowser;
    bowser.isAlive = true;
    bowser.hp = 100;
    bowser.x = 5000;
    bowser.y = 360;
    bowser.width = bowser.height = 120;
}

static void PutShotOnBowser(Shot& shot)
{
    shot = {};
    shot.x = g_monsters.bowser.x + 10;
    shot.y = g_monsters.bowser.y + 10;
    shot.width = shot.height = 10;
    shot.active = true;
}

static void StartBite(Player& player, int x, Direction direction)
{
    player.form = FORM_TINO;
    player.direction = direction;
    player.x = x;
    player.y = g_monsters.bowser.y;
    player.tino_motion = true;
    ++player.tinoBiteId;
}

static void TestProjectileDamage()
{
    ResetCombat();
    PutShotOnBowser(fireball[0]);
    PutShotOnBowser(tinofire[0]);
    HandleShotCollisions();
    assert(g_monsters.bowser.hp == 94 && !fireball[0].active);
    assert(tinofire[0].active && tinofire[0].hitBowser);
    HandleShotCollisions();
    assert(g_monsters.bowser.hp == 94);
    PutShotOnBowser(remoteShots[1].fireballs[0]);
    PutShotOnBowser(remoteShots[2].tinoFires[0]);
    HandleShotCollisions();
    assert(g_monsters.bowser.hp == 88 && !remoteShots[1].fireballs[0].active);
    assert(remoteShots[2].tinoFires[0].hitBowser);
    HandleShotCollisions();
    assert(g_monsters.bowser.hp == 88);

    ResetCombat();
    g_net.isHost = false;
    g_net.myPlayerId = g_localPlayerIdx = 1;
    PutShotOnBowser(fireball[0]);
    PutShotOnBowser(tinofire[0]);
    HandleShotCollisions();
    assert(g_monsters.bowser.hp == 100);
    assert(fireball[0].active && !tinofire[0].hitBowser);
    assert(!ApplyBowserHit(HIT_FIREBALL));
}

static void TestPlayerAttacks()
{
    ResetCombat();
    Player& host = g_allPlayers[0].mario;
    Player& distantClient = g_allPlayers[1].mario;
    Player& closeClient = g_allPlayers[2].mario;
    StartBite(host, 4450, DIR_RIGHT);
    StartBite(distantClient, 4450, DIR_RIGHT);
    StartBite(closeClient, 5130, DIR_LEFT);
    closeClient.isTransforming = true;
    HandleBowserPlayerAttacks();
    assert(g_monsters.bowser.hp == 90);

    closeClient.isTransforming = false;
    HandleBowserPlayerAttacks();
    assert(g_monsters.bowser.hp == 80);
    StartBite(closeClient, 5130, DIR_LEFT);
    HandleBowserPlayerAttacks();
    assert(g_monsters.bowser.hp == 70);
    HandleBowserPlayerAttacks();
    assert(g_monsters.bowser.hp == 70);

    distantClient.x = 5010;
    distantClient.star = true;
    HandleBowserPlayerAttacks();
    assert(g_monsters.bowser.hp == 0 && g_monsters.bowser.vy == -15);
    assert(!ApplyBowserHit(HIT_TINOFIRE));
}

int main()
{
    TestProjectileDamage();
    TestPlayerAttacks();
    puts("Bowser combat tests passed");
}
