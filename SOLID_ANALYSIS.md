# Guakmario — SOLID 원칙 분석 보고서

> 작성일: 2026-10-06
> 분석 기준 브랜치: main

---

## 총평

이 프로젝트는 WinAPI C++ 기반의 절차적 구조로 작성되어 있어 SOLID 원칙을 완전히 적용하기는 어렵지만,
각 원칙의 **정신**(책임 분리, 확장성, 의존 방향)은 충분히 평가 가능하다.
전반적으로 **SRP와 DIP 위반이 가장 심각**하며, 나머지 원칙들도 부분적으로 위반되고 있다.

---

## S — 단일 책임 원칙 (Single Responsibility Principle)

> "하나의 모듈은 변경될 이유가 하나여야 한다."

### 위반 사례

#### 1. `renderer.cpp` — 렌더링 중 게임 로직 실행 ⚠️ 심각
**파일:** `renderer.cpp:712`

```cpp
// Draw_mario() 내부에서 게임 로직 직접 호출
tino_attack → TinoAttack();
```

렌더러가 그리기를 담당하면서 동시에 적 피격 판정(TinoAttack)을 처리한다.
렌더링 타이밍에 따라 게임 로직이 실행되므로 프레임레이트와 게임 로직이 결합된다.

**영향:** 렌더링 로직을 바꾸면 전투 로직도 영향을 받는다.

---

#### 2. `item.cpp` — 하나의 파일에 너무 많은 책임 ⚠️ 심각
**파일:** `item.cpp` (873줄)

현재 `item.cpp`가 담당하는 역할:
- 아이템 물리 시뮬레이션 (UpdateItems × 5)
- 아이템 충돌 감지 (CheckCollision × 5)
- 발사체 물리 + 충돌 (Fireball, TinoFire)
- 마리오 변신 로직 (TransformToBig, TransformToFlower 등)
- 마리오 무적 상태 관리 (UpdateGodMode, UpdateStarMode)
- 발사체 스폰 (SpawnFireball, SpawnTinoFire)
- 티노 근접 공격 판정 (TinoAttack)

변신 로직이 바뀌어도, 발사체 물리가 바뀌어도, 무적 시스템이 바뀌어도 — 모두 같은 파일을 수정해야 한다.

---

#### 3. `main.cpp` — WndProc이 모든 것을 담당 ⚠️ 심각
**파일:** `main.cpp`

단일 `WndProc` 함수가 처리하는 것:
- 타이머 이벤트 → 게임 업데이트 호출
- 키보드 입력 처리 (점프, 공격, 발사)
- 렌더링 (WM_PAINT)
- 타이틀 화면 로직
- 쿨타임 관리

**특히 문제인 부분:** WM_KEYDOWN 핸들러 안에서 `SpawnFireball`, `TinoAttack`, `SpawnTinoFire`를 직접 호출 — 입력 처리가 게임 로직과 결합됨.

---

#### 4. `game.cpp` — `LoadStage()`의 복수 책임
**파일:** `game.cpp`

`LoadStage()` 하나가:
- BGM 설정
- 아이템 초기화
- 트랩 초기화
- 맵 초기화 + 포인터 전환
- 몬스터 초기화
- 마리오 위치 리셋
- 타이머 리셋

스테이지가 추가되거나 초기화 항목이 바뀔 때마다 이 함수를 수정해야 한다.

---

### 개선 방향

| 현재 | 개선 방향 |
|------|-----------|
| `TinoAttack()` in renderer | `UpdateGame()` 또는 `main.cpp` 입력 처리로 이동 |
| `item.cpp` God 파일 | `transform.cpp`, `shot.cpp`, `powerup.cpp` 등으로 분리 |
| `WndProc` God 함수 | 입력 처리 → `input.cpp`, 렌더 → renderer, 타이머 → game |
| `LoadStage()` | 각 시스템의 `Init(stage)` 호출을 각 모듈이 책임지도록 분산 |

---

## O — 개방-폐쇄 원칙 (Open/Closed Principle)

> "확장에는 열려 있고, 수정에는 닫혀 있어야 한다."

### 위반 사례

#### 1. 새 스테이지 추가 시 여러 곳 수정 필요
**파일:** `game.cpp`, `monster.cpp`, `map.cpp`, `sound.cpp`

스테이지 4를 추가하려면:
- `LoadStage()` → 분기 추가
- `InitMonsters(int stage)` → 데이터 추가
- `InitTurtles(int stage)` → 데이터 추가
- `SetStage_BGM()` → 분기 추가
- `map.cpp` → `InitMap4()` 추가

기존 코드를 여러 곳에서 수정해야 하므로 OCP 위반.

---

#### 2. 새 마리오 폼(형태) 추가 시 renderer.cpp 전면 수정
**파일:** `renderer.cpp`

현재 마리오 폼: small, big, flower, tino, star(×3), star_big(×3)
각 폼은 `renderer.cpp` 내 `Draw_mario()`의 if/else 체인에 하드코딩되어 있음 (약 700줄 분량).

새로운 폼을 추가하면 `Draw_mario()` 내부를 직접 수정해야 한다.

---

#### 3. `Player` 구조체의 상태 플래그 추가 방식
**파일:** `player.h`

현재 마리오의 파워업 상태가 개별 bool로 나열:
```cpp
bool isBig = false;
bool flower = false;
bool tino   = false;
bool star   = false;
bool god    = false;
bool supergod = false;
```

새 파워업 상태를 추가할 때마다 구조체, 렌더러, 충돌, 변신 로직 전부를 수정해야 한다.

---

### 개선 방향

| 현재 | 개선 방향 |
|------|-----------|
| 스테이지 분기 | `StageData` 테이블/배열로 데이터화 |
| `Draw_mario()` if/else | 폼별 함수 포인터 테이블 또는 상태 → 스프라이트 매핑 테이블 |
| `bool isBig, flower, tino...` | `enum MarioForm { SMALL, BIG, FLOWER, TINO }` + 단일 필드 |

---

## L — 리스코프 치환 원칙 (Liskov Substitution Principle)

> "하위 타입은 상위 타입을 대체할 수 있어야 한다."

### 현황

이 프로젝트는 상속을 사용하지 않으므로 전통적인 LSP 위반은 없다.
그러나 **구조적으로 LSP의 정신을 위반하는 패턴**이 있다.

#### 유사한 구조체가 공통 인터페이스 없이 나열됨
**파일:** `monster.h`

```cpp
struct Monster     { int x, y, vx, vy, width, height; bool isAlive, isDead, active; ... };
struct Turtle      { int x, y, vx, vy, width, height; bool isAlive, isDead, active; ... };
struct AngelTurtle { int x, y,     vy, width, height; bool isAlive, isDead; ... };
struct Bowser      { int x, y, vx, vy, width, height; bool isAlive, isDead; ... };
```

이 네 타입은 공통 필드를 가지고 있지만 서로 교체 불가능하며, 각자를 처리하는 함수도 4벌 따로 존재:
- `UpdateMonsters()`, `UpdateTurtles()`, `UpdateAngelTurtles()`, `UpdateBowser()`
- `CheckMarioMonsterCollision()`, `CheckMarioTurtleCollision()` ...

공통 베이스가 없기 때문에 "모든 적에 대해 동일한 로직"을 적용할 수 없다.

---

### 개선 방향

```cpp
// 공통 베이스 구조체 도입
struct Entity {
    int x, y, vx, vy;
    int width, height;
    bool isAlive, isDead, active;
};

struct Monster     : Entity { DWORD deadStart; };
struct Turtle      : Entity { TurtleState turtleState; int shellTimer; ... };
struct Bowser      : Entity { int hp; ... };
```

공통 베이스가 있으면 `UpdateAll(Entity* entities, int count)` 형태의 범용 함수 작성이 가능해진다.

---

## I — 인터페이스 분리 원칙 (Interface Segregation Principle)

> "클라이언트가 사용하지 않는 인터페이스에 의존하면 안 된다."

### 위반 사례

#### 1. `func.h` — 모든 것을 묶는 catch-all 헤더
**파일:** `func.h`

```cpp
#include "map.h"
#include "renderer.h"
#include "player.h"
#include "collision.h"
#include "game.h"
```

`func.h`를 include하면 map, renderer, player, collision, game 전부를 강제로 가져온다.
`collision.cpp`는 map이 필요하지만 renderer는 필요하지 않음에도 func.h를 통해 모두 포함된다.

**결과:** 헤더 하나를 수정하면 func.h를 포함하는 모든 파일이 재컴파일된다.

---

#### 2. `item.h` — 연관 없는 선언들이 한 헤더에 혼재
**파일:** `item.h`

아이템 구조체, 발사체 구조체, 충돌 함수, 변신 함수, 무적 함수가 모두 한 헤더에 있음.
`monster.cpp`가 `item.h`를 include할 때 발사체 스폰 함수까지 노출된다.

---

#### 3. `renderer.h` — 세분화되지 않은 Draw 함수 선언
**파일:** `renderer.h`

`Draw_mario()`, `Draw_Monsters()`, `Draw_Bowser()` 등이 모두 같은 헤더에 선언됨.
외부에서 필요한 것은 `Draw()` 하나뿐이나 내부 함수들이 전부 노출되어 있다.

---

### 개선 방향

| 현재 | 개선 방향 |
|------|-----------|
| `func.h` catch-all | 각 .cpp가 필요한 헤더만 직접 include |
| `item.h` 혼재 | `transform.h`, `shot.h`, `powerup.h`로 분리 |
| renderer 내부 함수 노출 | `static` 처리 또는 별도 내부 헤더로 분리 |

---

## D — 의존성 역전 원칙 (Dependency Inversion Principle)

> "고수준 모듈이 저수준 모듈에 직접 의존하면 안 된다. 둘 다 추상화에 의존해야 한다."

### 위반 사례

#### 1. 전역 상태를 통한 강결합 — 전체 구조의 핵심 문제 ⚠️ 심각

모든 모듈이 전역 상태에 직접 접근:

| 모듈 | g_player | g_game | g_monsters | g_images |
|------|:---:|:---:|:---:|:---:|
| renderer.cpp | ✓ (338회) | ✓ | ✓ | ✓ |
| monster.cpp | ✓ | ✓ | ✓ | - |
| item.cpp | ✓ | ✓ | ✓ | - |
| player.cpp | ✓ | ✓ | - | - |
| main.cpp | ✓ | ✓ | ✓ | - |

**문제:** 어떤 모듈이든 `g_player.mario.x`를 직접 읽고 쓸 수 있다.
`player.cpp`가 마리오 위치를 관리하는 "책임자"이지만 `monster.cpp`도 `renderer.cpp`도 마리오 위치를 직접 수정할 수 있다.

---

#### 2. 고수준 모듈이 저수준 구현에 직접 의존
**파일:** `game.cpp`

```cpp
// game.cpp (고수준 게임 루프)가 저수준 구현을 직접 호출
UpdateItems();           // item.cpp 직접 의존
UpdateMonsters();        // monster.cpp 직접 의존
CheckMarioMonsterCollision(); // collision 로직 직접 의존
```

추상화 계층 없이 고수준 모듈이 저수준 모듈에 직접 의존한다.

---

#### 3. `renderer.cpp`가 게임 상태 구조를 직접 알고 있음

renderer가 렌더링을 하려면 게임 상태를 알아야 하지만, 현재는 너무 많은 것을 알고 있다:
- 마리오의 내부 상태 (`isBig`, `flower`, `tino`, `star`, `god`, `supergod`)
- 몬스터 배열 구조 (`g_monsters.monsters[i].x`)
- 게임 상태 열거값 (`GAME_TRANSFORMING`, `GAME_FLOWER_TRANS` 등)

renderer가 "렌더링 데이터"가 아닌 "게임 내부 구현"에 직접 의존하고 있다.

---

### 개선 방향

```cpp
// 렌더링에 필요한 데이터만 담는 별도 구조체 도입 (RenderData 패턴)
struct MarioRenderData {
    int x, y, width, height;
    int direction;
    MarioForm form;       // enum으로 단일화
    bool isJumping, isWalking;
    // 렌더러가 알 필요 없는 god, supergod 등은 제외
};
// game.cpp가 렌더 데이터를 준비하고 renderer는 그것만 받아서 그림
```

---

## 종합 우선순위

| 우선순위 | 문제 | 원칙 | 난이도 |
|----------|------|------|--------|
| 🔴 높음 | `renderer.cpp`에서 `TinoAttack()` 호출 | SRP | 낮음 |
| 🔴 높음 | `bool isBig/flower/tino...` → `enum MarioForm` | OCP | 낮음 |
| 🔴 높음 | `item.cpp` God 파일 분리 | SRP | 중간 |
| 🟡 중간 | `Monster/Turtle/AngelTurtle/Bowser` 공통 베이스 도입 | LSP | 중간 |
| 🟡 중간 | `func.h` catch-all 제거 | ISP | 낮음 |
| 🟡 중간 | renderer 내부 함수 `static` 처리 | ISP | 낮음 |
| 🟢 낮음 | 스테이지 데이터 테이블화 | OCP | 중간 |
| 🟢 낮음 | `WndProc` 입력/렌더 분리 | SRP | 높음 |
| 🟢 낮음 | RenderData 도입으로 renderer-game 결합 해소 | DIP | 높음 |

---

## 참고: 잘 된 부분

- `GameContext`, `PlayerContext`, `MonsterState`, `ImageSet` 구조체 도입으로 전역 변수가 구조적으로 묶여 있음 ✓
- `InitMonstersFromData`, `InitTurtlesFromData` 헬퍼로 중복 제거 ✓
- `handle_transform()` 헬퍼로 변신 로직 3중복 제거 ✓
- `DrawSprite()` 추상화로 flip 로직 단일화 ✓
- 래퍼 함수(`UpdateAllItems` 등)로 `UpdateGame()` 간결화 ✓
