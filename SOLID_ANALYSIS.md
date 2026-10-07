# Guakmario — SOLID 원칙 분석 보고서

> 작성일: 2026-10-06 / 최종 업데이트: 2026-10-07
> 분석 기준 브랜치: main

---

## 총평

SOLID 원칙 개선 작업 완료. 최초 분석의 모든 항목이 적용됨.

---

## S — 단일 책임 원칙 (Single Responsibility Principle)

### ✅ 해결된 사례

#### 1. `renderer.cpp` — TinoAttack 분리 완료
`Draw_mario()` 내부에서 호출되던 `TinoAttack()`을 `input.cpp`의 `HandleGameKeyDown()`으로 이동.
렌더러는 이제 순수하게 그리기만 담당한다.

#### 2. `item.cpp` 분리 완료
873줄 God 파일을 책임별로 분리:
- `item.cpp` — 아이템 물리 + 충돌 (버섯, 꽃, 별, 티노 아이템)
- `transform.cpp` — 마리오 변신 로직 (`TransformToBig`, `TransformToFlower`, `TransformToTino`, `TransformToSmall`)
- `shot.cpp` — 발사체 물리 + 충돌 + 스폰 + `TinoAttack`

#### 3. `WndProc` 입력 분리 완료
게임 내 키 입력 처리를 `input.cpp` / `input.h`로 분리:
- `HandleGameKeyDown(WPARAM)` — 점프, 티노 바이트
- `HandleGameKeyUp(WPARAM)` — 파이어볼 발사
- `HandleGameChar(WPARAM)` — 티노 불 발사 (`z` 키)
- `TickCooldowns()` — 쿨타임 감소

`WndProc`의 `WM_KEYDOWN`/`WM_KEYUP`/`WM_CHAR`/`WM_TIMER case 5`는 각각 한 줄 위임으로 단순화.

#### 4. `LoadStage()` — 스테이지 테이블로 개선
`StageData` 구조체 + `stages[]` 배열을 `game.cpp`에 도입.
새 스테이지 추가 시 테이블에 항목 하나만 추가하면 됨. (→ OCP 항목과 동일)

---

## O — 개방-폐쇄 원칙 (Open/Closed Principle)

### ✅ 해결된 사례

#### 1. 스테이지 데이터 테이블화 완료
`game.cpp`에 `StageData` 테이블 도입:

```cpp
struct StageData {
    const char*  bgm;
    void       (*initMap)();
    int        (*mapData)[MAP_WIDTH];
    void       (*extraInit)();
};

static const StageData stages[] = {
    { nullptr,                               nullptr,  nullptr, nullptr          },
    { "resource\\...\\GroundTheme.wav",      InitMap,  map1,   nullptr           },
    { "resource\\...\\GroundTheme.wav",      InitMap2, map2,   InitAngelTurtles  },
    { "resource\\...\\CastleTheme.wav",      InitMap3, map3,   InitStage3Extras  },
};
```

스테이지 4 추가: 테이블에 행 하나 추가. `LoadStage()` 코드 수정 불필요.

`sound.cpp`의 `SetStage_BGM()`도 동일 패턴의 배열로 리팩토링.

#### 2. `bool isBig/flower/tino` → `enum MarioForm` 완료
```cpp
// data.h
enum MarioForm { FORM_SMALL, FORM_BIG, FORM_FLOWER, FORM_TINO };
```

`Player` 구조체에 `MarioForm form` 단일 필드. 변신 시 `form`만 변경하면 렌더러/충돌 로직이 자동으로 분기.

### ⚠️ 남은 사례

#### `Draw_mario()` 폼별 if/else 체인
`renderer.cpp`의 `Draw_mario()`는 여전히 폼별 if/else 체인. 새 폼 추가 시 renderer 수정 필요.
함수 포인터 테이블로 개선 가능하나 미적용.

---

## L — 리스코프 치환 원칙 (Liskov Substitution Principle)

### ✅ 해결된 사례

#### 공통 베이스 `Entity` 도입 완료
`monster.h`에 `Entity` 베이스 구조체 도입:

```cpp
struct Entity {
    int x = 0, y = 0;
    int vx = 0, vy = 0;
    int width = 0, height = 0;
    bool isAlive = false;
    bool isDead = false;
    bool active = false;
    bool isFalling = false;
};

struct Monster     : Entity { DWORD deadStart; ... };
struct Turtle      : Entity { TurtleState turtleState; ... };
struct AngelTurtle : Entity { int topY, bottomY; ... };
struct Bowser      : Entity { int hp; ... };
```

Update/Check 함수는 여전히 타입별로 분리되어 있으나 공통 필드 접근은 일관됨.

---

## I — 인터페이스 분리 원칙 (Interface Segregation Principle)

### ✅ 해결된 사례

#### 1. `func.h` catch-all 제거 완료
`func.h`가 삭제됨. 각 `.cpp`가 필요한 헤더만 직접 include.

#### 2. renderer 내부 함수 `static` 처리 완료
`Draw_mario()`, `Draw_Monsters()` 등 renderer 내부 함수들이 `static`으로 처리되어 외부 노출 없음.
`renderer.h`에는 `Draw()`, `Draw_information()`, `DrawSprite()` 등 공개 인터페이스만 선언.

### ⚠️ 남은 사례

#### `item.h` — 변신/발사체 선언 혼재
`item.h`에 아이템 구조체 + `TransformTo*` + 무적 함수 선언이 함께 있음.
`monster.cpp`가 `item.h`를 include 시 불필요한 변신 함수까지 노출됨.
`transform.h`로 분리 가능하나 미적용.

---

## D — 의존성 역전 원칙 (Dependency Inversion Principle)

### ✅ 해결된 사례

#### 1. `MarioRenderData` 도입 완료
`player.h`에 렌더링 전용 뷰 구조체 추가:

```cpp
struct MarioRenderData {
    int x, y, width, height, direction;
    MarioForm form;
    bool isWalking, isJumping, isFlying, isDead;
    bool star, fire_motion, tino_motion, tino_fire_motion;
    int walk_motion, motion_timer;
    int life, coin;
    int tino_cooldown_z, tino_cooldown_space;
};
MarioRenderData BuildMarioRenderData();
```

`renderer.cpp`의 `Draw_mario()`, `Draw_information()`이 `g_player.mario` 대신 `MarioRenderData`를 받음.
렌더러가 `god`, `supergod`, `vx`, `vy` 등 렌더링 무관 필드에 직접 접근하지 않음.

#### 2. `GameState` 통합 완료
`GAME_FLOWER_TRANS`, `GAME_TINO_TRANS` 제거. `GAME_TRANSFORMING` 하나로 통합.
변신 대상은 `GameContext::transformTarget: MarioForm`으로 분리:

```cpp
enum GameState { GAME_RUNNING, GAME_TRANSFORMING, GAME_VICTORY, GAME_CLEAR, GAME_OVER };
// GameContext:
MarioForm transformTarget = FORM_SMALL;
```

### ⚠️ 남은 사례

#### 전역 상태를 통한 강결합
`renderer.cpp`가 여전히 `g_monsters`에 직접 접근.
`monster.cpp`, `item.cpp`도 `g_player.mario`에 직접 읽기/쓰기.
완전한 DIP 적용은 아키텍처 전면 개편이 필요해 현실적으로 미적용.

---

## 종합 현황

| 항목 | 원칙 | 상태 |
|------|------|------|
| `TinoAttack()` renderer에서 input으로 이동 | SRP | ✅ 완료 |
| `item.cpp` 분리 → `transform.cpp`, `shot.cpp` | SRP | ✅ 완료 |
| `WndProc` 입력 → `input.cpp` | SRP | ✅ 완료 |
| 스테이지 데이터 테이블화 (`StageData`) | OCP | ✅ 완료 |
| `enum MarioForm` 도입 | OCP | ✅ 완료 |
| `Entity` 공통 베이스 도입 | LSP | ✅ 완료 |
| `func.h` 제거, 직접 include | ISP | ✅ 완료 |
| renderer 내부 함수 `static` | ISP | ✅ 완료 |
| `MarioRenderData` 도입 | DIP | ✅ 완료 |
| `GameState` 통합 + `transformTarget` | DIP/SRP | ✅ 완료 |
| `Draw_mario()` 폼별 함수포인터 테이블 | OCP | ⏸ 미적용 |
| `item.h` → `transform.h` 분리 | ISP | ⏸ 미적용 |
| `g_monsters` renderer 직접접근 해소 | DIP | ⏸ 미적용 (아키텍처 전면개편 필요) |

---

## 잘 된 부분

- `GameContext`, `PlayerContext`, `MonsterState`, `ImageSet` 구조체로 전역 변수 구조화 ✓
- `InitMonstersFromData`, `InitTurtlesFromData` 헬퍼로 중복 제거 ✓
- `handle_transform()` 헬퍼로 변신 로직 3중복 제거 ✓
- `DrawSprite()` 추상화로 flip 로직 단일화 ✓
- 래퍼 함수(`UpdateAllItems`, `UpdateAllMonsters` 등)로 `UpdateGame()` 간결화 ✓
- `StageData` 테이블 + `stages[]`로 스테이지 추가 시 코드 수정 최소화 ✓
- `MarioRenderData`로 렌더러와 게임 로직 부분 분리 ✓
