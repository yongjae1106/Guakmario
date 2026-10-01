# Guakmario 리패키징 계획

## 현재 파일 구조 (Step 5 완료 후)

```
guakmario/
├── main.cpp          - WinAPI 진입점 + WndProc (키 입력, 타이머 혼재)
├── data.h/cpp        - 시스템 include + #define 상수 + TileType enum + 전역 변수 정의
├── map.h/cpp         - 맵 배열 + InitMap1/2/3 + FireTrap 로직
├── renderer.h/cpp    - 렌더링 전담 (Draw* 15개) + GDI 핸들/브러시/TCHAR 버퍼 선언
├── player.h/cpp      - 플레이어 전담 (Player 구조체, movePlayer, UpdatePlayer, dead 등)
├── collision.h/cpp   - 충돌 전담 (isSolidTile, IsColliding, checkcollision_* 등)
├── game.h/cpp        - 게임 흐름 전담 (GameState, UpdateGame, stage_load 등)
├── func.h            - umbrella (map/renderer/player/collision/game 일괄 include)
├── func.cpp          - 빈 파일
├── image.h/cpp       - Image* 전역 포인터 ~200개
├── monster.h/cpp     - 몬스터/거북이/천사거북이/보스 구조체 + 로직
├── item.h/cpp        - 아이템/발사체 구조체 + 로직 + 변신 처리
└── sound.h/cpp       - 사운드 로드 + 재생
```

**include 구조 (순환 없음):**
```
func.h
  ├── map.h       → data.h
  ├── renderer.h  → data.h
  ├── player.h    → data.h
  ├── collision.h → data.h
  └── game.h      → data.h
```

**각 헤더 책임:**
| 헤더 | 선언 내용 |
|------|-----------|
| `data.h` | `#define` 상수 5개, `TileType` enum |
| `player.h` | `struct Player`, `mario`, `keyState`, 플레이어 함수 |
| `game.h` | `enum GameState`, 게임 상태 변수, 게임 흐름 함수 |
| `renderer.h` | GDI 핸들, 브러시, TCHAR 버퍼, Draw* 함수 |
| `collision.h` | `isSolidTile`, `IsColliding*`, `checkcollision_*` |
| `map.h` | 맵 배열, `currentMap`, `InitMap*`, FireTrap |

---

## 완료된 작업

### ✅ Step 1. 죽은 코드 제거
- `func.cpp` 상단 주석처리된 구버전 `InitMap()` 블록 (~170줄) 삭제
- 중복 타일 타입 주석 3곳 삭제

### ✅ Step 2. TileType enum 교체
- `data.h`에 `TileType` enum 추가 (34개 타일 값)
- `func.cpp`, `monster.cpp`, `item.cpp` 전체의 매직 넘버 비교를 enum으로 치환

### ✅ Step 2-5. isSolidTile() 헬퍼 도입
- `bool isSolidTile(int tile)` 추가 → `collision.h/cpp`에 위치
- 4줄짜리 AND 충돌 체인 → 1줄로 교체 (~22곳)
- 버그 수정: `up_mushroom` 충돌 조건 `!= 2` 매직 넘버 제거
- 버그 수정: 용암 충돌 중복 OR (`left || left`) → `left || right` 교정

### ✅ Step 3. map.h/cpp 책임 복원
- 맵 배열(`map1/2/3`, `currentMap`) 선언/정의 → `map.h/cpp`로 이동
- `InitMap/2/3` 구현 → `func.cpp`에서 `map.cpp`로 이동 (~544줄)
- `data.h/cpp`에서 맵 관련 항목 + 미사용 `int map[]` 제거

### ✅ Step 4. func.cpp 분리
- `func.cpp` (~2,033줄) → 역할별 4개 파일로 분리
  - `renderer.h/cpp` — Draw* 15개 (~1,180줄)
  - `player.h/cpp` — movePlayer, UpdatePlayer, UpdateMario_motion, UpdateDeadMotion, dead, resurrection (~447줄)
  - `collision.h/cpp` — isSolidTile, IsColliding*, checkcollision_* (~94줄)
  - `game.h/cpp` — UpdateGame, timegoes, stage_load, monster_reset, item_reset (~301줄)
- `func.h` → umbrella로 교체, `func.cpp` → 빈 파일
- `guakmario.vcxproj` → 새 파일 4쌍 추가
- include 순환 버그 수정: `map.h`에서 `func.h` 제거, `func.h`가 `map.h`를 포함

### ✅ Step 5. data.h 슬림화
- `struct Player` + `mario`, `keyState` → `player.h`로 이동
- `enum GameState` + DWORD 타이머 6개 + `stage/cameraX/frame_motion` 등 → `game.h`로 이동
- GDI 핸들, 브러시, TCHAR 버퍼 → `renderer.h`로 이동
- `data.h` → 시스템 include + `#define` 상수 + `TileType` enum만 남김
- 빌드 후 발생한 미인식 식별자 수정:
  - `collision.cpp` → `#include "func.h"` 추가
  - `map.cpp` → `#include "game.h"` 추가
  - `sound.cpp` → `#include "game.h"` 추가

---

## 남은 작업

### ⬜ Step 6. image.h 구조체화 (선택)

Image* 포인터 ~200개를 카테고리별 구조체로 묶어 가독성 향상.

```cpp
struct MarioImages   { Image* stop; Image* walk[3]; Image* jump; Image* dead; };
struct TileImages    { Image* ground; Image* brick; Image* mystery; ... };
struct MonsterImages { Image* goomba[2]; Image* turtle[4]; ... };
```

---

### ⬜ Step 7. WndProc 정리 (선택)

- `GAME_TITLE` GameState 추가 → `gamestart` bool 플래그 제거
- 키 입력 처리를 `HandleInput()` 함수로 분리
- WndProc: 메시지 수신 + `InvalidateRect` 호출만 담당

---

## 작업 순서

```
✅ Step 1    죽은 코드 제거
✅ Step 2    TileType enum 교체
✅ Step 2-5  isSolidTile() 헬퍼 도입
✅ Step 3    map.h/cpp 책임 복원
✅ Step 4    func.cpp 분리
✅ Step 5    data.h 슬림화
⬜ Step 6    image.h 구조체화 (선택)
⬜ Step 7    WndProc 정리 (선택)
```

---

## 주의사항

- 각 Step 완료 후 Visual Studio x64 Debug 빌드 확인
- 전역 변수 선언 위치 변경 시 → 해당 변수를 사용하는 모든 cpp에 새 헤더 include 추가
- `using namespace Gdiplus`는 렌더링 관련 각 cpp 파일에서 유지
- Step 6~7은 선택 사항으로 게임 동작에는 영향 없음
