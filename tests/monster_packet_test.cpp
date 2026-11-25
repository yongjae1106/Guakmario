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
void HandleMultiplayerGameEvent(int, const GameEventData&) {}
void HandleMultiplayerPlayerState(int, int) {}
bool ConfirmItemPickup(int, const ItemPickupData&) { return false; }
void ApplyItemPickupResult(const ItemPickupData&) {}
void ApplyItemSpawn(const ItemSpawnData&) {}
void HandleMonsterHit(int, const MonsterHitData&) {}
void ApplyMonsterSnapshot(const std::vector<uint8_t>&) {}
void ApplyBossSnapshot(const std::vector<uint8_t>&) {}

static void TestBossStatePacket()
{
    BossStateData boss = {};
    boss.x = 5020;
    boss.y = 320;
    boss.vy = -18;
    boss.hp = 85;
    boss.width = 120;
    boss.height = 120;
    boss.flags = BOSS_FLAG_ALIVE | BOSS_FLAG_FIRING | BOSS_FLAG_JUMPING;
    boss.frame = 1;
    boss.moveSign = 1;
    boss.fireSign = -1;

    std::vector<BossFireballData> fireballs = { { 0, 5000, 400, -6, 0, 40, 20, 3 },
        { MAX_FIREBALLS - 1, -20, 450, 6, 0, 40, 20, 6 } };
    std::vector<uint8_t> packet;
    assert(BuildBossStatePacket(boss, fireballs, packet));
    BossStateData decoded = {};
    std::vector<BossFireballData> decodedFireballs;
    assert(ParseBossStatePacket(packet, decoded, decodedFireballs));
    boss.fireballCount = 2;
    assert(memcmp(&decoded, &boss, sizeof(boss)) == 0);
    assert(decodedFireballs.size() == fireballs.size());
    assert(memcmp(decodedFireballs.data(), fireballs.data(),
        fireballs.size() * sizeof(BossFireballData)) == 0);

    auto rejects = [&](std::vector<uint8_t> bad) {
        assert(!ParseBossStatePacket(bad, decoded, decodedFireballs));
        assert(decoded.hp == boss.hp && decodedFireballs.size() == fireballs.size());
    };
    auto bad = packet;
    bad.pop_back();
    rejects(bad);
    bad = packet;
    bad[3] = 1;
    rejects(bad);
    bad = packet;
    const size_t prefix = sizeof(PacketHeader) + sizeof(BossStateData);
    bad[prefix + sizeof(BossFireballData)] = bad[prefix]; // 중복 슬롯
    rejects(bad);
    bad = packet;
    bad[prefix] = MAX_FIREBALLS;
    rejects(bad);

    fireballs[1].motion = 7;
    assert(!BuildBossStatePacket(boss, fireballs, packet));
    boss.flags = BOSS_FLAG_DEAD | BOSS_FLAG_FALLING;
    boss.hp = 0;
    assert(BuildBossStatePacket(boss, {}, packet));
    assert(ParseBossStatePacket(packet, decoded, decodedFireballs));
    assert(decoded.hp == 0 && decodedFireballs.empty());
}

int main() {
    TestBossStatePacket();
    std::vector<uint8_t> packet;
    std::vector<MonsterEntry> states, decoded;
    uint8_t kind = 255;
    assert(BuildMonsterStatePacket(MON_NORMAL, states, packet));
    assert(packet.size() == 6);
    assert(ParseMonsterStatePacket(packet, kind, decoded) && decoded.empty());
    MonsterEntry a = {};
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
        assert(memcmp(decoded.data(), before.data(), before.size() * sizeof(MonsterEntry)) == 0);
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
