# 함수명 불일치 분석 — guakmario

## 핵심 원칙

`Check~` 접두사는 "판정만 하고 bool 반환" 또는 "확인만"을 암시한다.
하지만 현재 코드에서 `Check~` 함수들은 내부에서 상태 변경, 사운드 재생, 사망 처리까지 전부 수행하고 있다.
`Handle~` 또는 `Apply~` 접두사가 의도에 더 맞다.

---

## 카테고리 1: `CheckCollision_*` — 실제로는 픽업/피격 로직 전체 처리 (item.cpp, shot.cpp)

| 현재 이름 | 실제 하는 일 | 제안 이름 |
|---|---|---|
| `CheckCollision_mushroom` | 버섯 비활성화, 사운드 재생, 변신 상태 설정 | `HandleMushroomPickup` |
| `CheckCollision_up_mushroom` | 버섯 비활성화, 사운드 재생, 라이프++ | `HandleUpMushroomPickup` |
| `CheckCollision_flower` | 꽃 비활성화, 사운드 재생, 변신 상태 설정 | `HandleFlowerPickup` |
| `CheckCollision_tino` | 티노 비활성화, 사운드 재생, 변신 상태 설정 | `HandleTinoPickup` |
| `CheckCollision_star` | 스타 비활성화, BGM 교체, 무적 상태 설정 | `HandleStarPickup` |
| `CheckCollision_fireball` | 몬스터 사망 처리, 파이어볼 비활성화 | `HandleFireballHit` |
| `CheckCollision_tinofire` | 몬스터 사망 처리, 이펙트 생성, 보서 hp 감소 | `HandleTinoFireHit` |

### 근거
- `CheckCollision_mushroom` (item.cpp:19): 충돌 시 `mushroom[i].active = false`, `PlaySoundBuffer(...)`, `g_game.gameState = GAME_TRANSFORMING` 등 상태 변경 3종 수행
- `CheckCollision_star` (item.cpp:117): 충돌 시 `PlayBGM(...)`, `g_player.mario.star = true`, `g_game.starStartTime = ...` 설정
- `CheckCollision_fireball` (shot.cpp:13): 충돌 시 몬스터 `isDead = true`, `isFalling = true`, `fireball[i].active = false` — 완전한 처치 로직

---

## 카테고리 2: `CheckMario*Collision` — 실제로는 피격/상태 변경 전체 처리 (monster.cpp)

| 현재 이름 | 실제 하는 일 | 제안 이름 |
|---|---|---|
| `CheckMarioMonsterCollision` | 밟기 처리(monster 사망), 측면 충돌 시 `damage_mario()` | `HandleMarioMonsterCollision` |
| `CheckMarioTurtleCollision` | 거북이 상태 전이(NORMAL→SHELL→SPINNING), 데미지, 킥 처리 | `HandleMarioTurtleCollision` |
| `CheckMarioAngelTurtleCollision` | 날개 거북이 상태 변경(FLYING→HIDE), 데미지 | `HandleMarioAngelTurtleCollision` |
| `CheckMarioBowserCollision` | 보서 `isDead=true`, `isFalling=true`, 또는 `damage_mario()` | `HandleMarioBowserCollision` |
| `CheckMarioFireballCollision` | 조건 통과 시 즉시 `damage_mario()` 호출 | `HandleMarioFireballCollision` |

### 근거
- `CheckMarioTurtleCollision` (monster.cpp:457): 내부에서 `turtleState = SHELL`, `turtleState = SPINNING`, `damage_mario()`, `PlaySoundBuffer(...)` 등 다수의 게임 로직 수행. 이름에서 예상되는 "판정 후 결과 반환" 역할이 아님.
- 래퍼인 `CheckAllMonsterCollisions` (monster.cpp:839)도 같은 맥락이므로 `HandleAllMonsterCollisions`로 변경 권장.

---

## 카테고리 3: `Update*` 함수 내 충돌 판정 혼재 (monster.cpp)

| 위치 | 문제 |
|---|---|
| `UpdateMonsters` (monster.cpp:219~238) | 루프 내부에서 마리오 밟기 판정 + 몬스터 사망 처리 수행. Update 함수가 충돌 처리까지 겸함. |

### 근거
`UpdateMonsters`는 이동/물리 업데이트 함수여야 하는데, 219번 줄부터 마리오 좌표와 비교해 `monsters[i].isAlive = false`, `g_player.mario.vy = -10` 등 충돌 결과까지 처리한다.
`CheckMarioMonsterCollision`과 역할이 중복되며, 어느 쪽에서 처리하는지 추적하기 어렵다.

**제안:** 밟기 판정 블록을 `UpdateMonsters`에서 제거하고 `HandleMarioMonsterCollision` 안으로 통합.

---

## 카테고리 4: 명칭 일관성 문제 — Mushroom 계열 (item.cpp)

| 현재 이름 | 문제 | 제안 이름 |
|---|---|---|
| `SpawnItem(int x, int y)` | mushroom만 스폰하는데 이름이 너무 일반적. 다른 함수들(`SpawnItem_star`, `SpawnItem_flower` 등)과 패턴도 다름. | `SpawnItem_mushroom` 또는 `SpawnMushroom` |
| `UpdateItems()` | mushroom만 업데이트하는데 suffix 없음. 다른 함수들(`UpdateItems_star`, `UpdateItems_flower`)과 일관성 없음. | `UpdateItems_mushroom` |

---

## 요약 — 우선순위 정리

| 우선순위 | 범위 | 작업 |
|---|---|---|
| 높음 | item.cpp, shot.cpp | `CheckCollision_*` → `Handle*Pickup` / `Handle*Hit` 으로 rename |
| 높음 | monster.cpp | `CheckMario*Collision` → `HandleMario*Collision` 으로 rename |
| 중간 | monster.cpp | `UpdateMonsters` 내 밟기 판정 분리 |
| 낮음 | item.cpp | `SpawnItem`, `UpdateItems` 네이밍 일관성 통일 |

rename은 .cpp와 .h 선언부 + 호출부(`CheckItemCollisions`, `CheckShotCollisions`, `CheckAllMonsterCollisions`, `UpdateAllMonsters`) 모두 변경 필요.
