#include "network.h"
#include "player.h"
#include "game.h"
#include <cassert>
#include <cstdio>

// Standalone packet tests: unrelated network game-loop dependencies are stubs.
PlayerContext g_allPlayers[3] = {};
int g_localPlayerIdx = 0;
GameContext g_game = {};
bool ApplyRemotePlayerState(int, const PlayerStateData&) { return false; }

int main() {
    std::vector<uint8_t> packet;
    std::vector<MonsterStateData> states, decoded;
    uint8_t kind = 255;
    assert(BuildMonsterStatePacket(MON_NORMAL, states, packet));
    assert(packet.size() == 6);
    assert(ParseMonsterStatePacket(packet, kind, decoded) && decoded.empty());
    MonsterStateData a = {};
    a.slot = 2; a.x = -123; a.width = 32; a.height = 32; a.flags = MON_FLAG_ACTIVE;
    states.push_back(a); a.slot = 99; a.timerA = 65535; states.push_back(a);
    assert(BuildMonsterStatePacket(MON_NORMAL, states, packet));
    assert(packet.size() == 40);
    assert(ParseMonsterStatePacket(packet, kind, decoded));
    assert(kind == MON_NORMAL && decoded.size() == 2 && decoded[0].x == -123 && decoded[1].slot == 99 && decoded[1].timerA == 65535);
    const auto valid = packet;
    auto rejects = [&](std::vector<uint8_t> bad) {
        auto before = decoded;
        uint8_t previous = kind;
        assert(!ParseMonsterStatePacket(bad, kind, decoded));
        assert(kind == previous && decoded.size() == before.size());
        assert(memcmp(decoded.data(), before.data(), before.size() * sizeof(MonsterStateData)) == 0);
    };
    auto bad = valid; bad.pop_back(); rejects(bad);
    bad = valid; bad.push_back(0); rejects(bad);
    bad = valid; bad[0] = 0; rejects(bad);
    bad = valid; bad[2] = PKT_BOSS_STATE; rejects(bad);
    bad = valid; bad[3] = 1; rejects(bad);
    bad = valid; bad[4] = MON_BOSS; rejects(bad);
    bad = valid; bad[5] = 101; rejects(bad);
    bad = valid; bad[23] = 2; rejects(bad); // duplicate second slot
    bad = valid; bad[6] = 100; rejects(bad);
    bad = valid; bad[18] = 128; rejects(bad); // unknown flags
    bad = valid; bad[17] = 1; rejects(bad); // normal monster state reserved
    bad = valid; bad[15] = 0; rejects(bad); // zero width
    for (size_t n = 0; n < 6; ++n) rejects(std::vector<uint8_t>(n));
    states[1].slot = 2;
    assert(!BuildMonsterStatePacket(MON_NORMAL, states, packet) && packet == valid);
    states.clear();
    for (int i = 0; i < 100; ++i) { a.slot = static_cast<uint8_t>(i); states.push_back(a); }
    assert(BuildMonsterStatePacket(MON_NORMAL, states, packet) && packet.size() == 1706);
    assert(ParseMonsterStatePacket(packet, kind, decoded) && decoded.size() == 100);
    states.push_back(a); assert(!BuildMonsterStatePacket(MON_NORMAL, states, packet));
    states.assign(1, a);
    assert(!BuildMonsterStatePacket(MON_BROWN_TURTLE, states, packet));
    for (uint8_t k : {MON_NORMAL, MON_TURTLE, MON_BROWN_TURTLE, MON_ANGEL_TURTLE, MON_FIREBALL}) {
        assert(BuildMonsterStatePacket(k, {}, packet));
        assert(ParseMonsterStatePacket(packet, kind, decoded) && kind == k && decoded.empty());
    }
    a.state = 2; a.flags = MON_FLAG_ALIVE | MON_FLAG_DAMAGE; a.timerB = 100;
    assert(BuildMonsterStatePacket(MON_TURTLE, {a}, packet));
    assert(ParseMonsterStatePacket(packet, kind, decoded) && decoded[0].timerB == 100);
    a.state = 3; assert(!BuildMonsterStatePacket(MON_TURTLE, {a}, packet));
    a.state = 1; a.flags = MON_FLAG_ALIVE | MON_FLAG_GOING_UP; a.timerB = 0;
    assert(BuildMonsterStatePacket(MON_ANGEL_TURTLE, {a}, packet));
    a.state = 2; assert(!BuildMonsterStatePacket(MON_ANGEL_TURTLE, {a}, packet));
    puts("Monster packet tests passed");
}
