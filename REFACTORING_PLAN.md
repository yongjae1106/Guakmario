# Guakmario 리팩토링 계획

> 작성일: 2026-10-01
> 기준 브랜치: main

---

## 우선순위 기준

- **P1 (즉시)** : 버그 유발 가능성 또는 수정 비용이 낮고 효과가 큰 것
- **P2 (중기)** : 중복 제거 / 가독성 개선
- **P3 (장기)** : 구조적 개선 (범위가 넓어 별도 작업)

---

## P1 — 즉시 수정

### 1. 항상 참인 조건 제거
**파일:** `game.cpp:84`

```cpp
// 현재 (DWORD는 unsigned라 항상 0 이상)
if (now - victoryStart >= 0)

// 수정: 조건 제거, 내용만 남김
mario.vx = 1;
mario.x += mario.vx;
```

---

### 2. `IsColliding()`에서 게임 로직 분리
**파일:** `collision.cpp:80~90`

순수 AABB 충돌 함수에 `mario.god` 체크가 섞여 있음. 함수 이름과 역할이 불일치.

```cpp
// 현재
bool IsColliding(...) {
    if (mario.god) return false;  // 게임 로직이 여기 있으면 안 됨
    return (ax < bx+bw && ...);
}

// 수정: IsColliding은 순수 기하 계산만, 호출부에서 god 체크
bool IsColliding(...) {
    return (ax < bx+bw && ax+aw > bx && ay < by+bh && ay+ah > by);
}

// CheckMarioMonsterCollision() 등 호출부에서:
if (!mario.god && IsColliding(...)) { ... }
```

---

### 3. `extern Bowser bowser` 중복 선언 제거
**파일:** `monster.h:106,109`

```cpp
extern Bowser bowser;
extern Fireball bowserFire;

extern Bowser bowser;  // 중복 — 이 줄 삭제
```

---

### 4. `resurrection()`의 죽은 코드 제거
**파일:** `player.cpp:452~454`

```cpp
// 현재: 아래 두 줄은 바로 다음 struct 초기화로 덮어써짐 (무의미)
mario.x = 100;
mario.y = 300;
mario = { 100, 300, ... };

// 수정: 앞 두 줄 삭제
mario = { 100, 300, ... };
```

---

### 5. 매직 넘버 상수화
**파일:** `collision.cpp:38`

```cpp
// 현재
if (j == 139 && ...)

// 수정: data.h 또는 map.h에 상수 추가
#define STAGE3_CLEAR_COLUMN 139

if (j == STAGE3_CLEAR_COLUMN && ...)
```

---

### 6. `motion_timer` 중복 처리 함수화
**파일:** `player.cpp:174~206`

`fire_motion`, `tino_motion`, `tino_fire_motion` 세 개가 완전히 동일한 구조로 반복됨.

```cpp
// 현재: 같은 패턴 3번 반복
if (mario.fire_motion) {
    if (mario.motion_timer > 0) mario.motion_timer--;
    else mario.fire_motion = false;
}
if (mario.tino_motion) {
    if (mario.motion_timer > 0) mario.motion_timer--;
    else mario.tino_motion = false;
}
// ...

// 수정: 헬퍼 함수 하나로
static void tick_motion(bool& flag, int& timer) {
    if (flag) {
        if (timer > 0) timer--;
        else flag = false;
    }
}

void UpdateMario_motion() {
    tick_motion(mario.fire_motion,      mario.motion_timer);
    tick_motion(mario.tino_motion,      mario.motion_timer);
    tick_motion(mario.tino_fire_motion, mario.motion_timer);
}
```

---

## P2 — 중기 개선

### 7. 변신 상태 코드 통합
**파일:** `game.cpp:18~79`

`GAME_TRANSFORMING`, `GAME_FLOWER_TRANS`, `GAME_TINO_TRANS` 블럭이 거의 동일.

```cpp
// 현재: 3개 블럭 반복
else if (gameState == GAME_TRANSFORMING) { ... 700ms 대기 후 transform_bigmario() ... }
else if (gameState == GAME_FLOWER_TRANS) { ... 700ms 대기 후 transform_to_flower() ... }
else if (gameState == GAME_TINO_TRANS)   { ... 700ms 대기 후 transform_to_tino() ... }

// 수정: 함수 포인터 또는 상태 테이블로 통합
struct TransformEntry { GameState state; bool Player::* flag; void (*transform_fn)(); };
static TransformEntry table[] = {
    { GAME_TRANSFORMING, &Player::isBig,   transform_bigmario   },
    { GAME_FLOWER_TRANS, &Player::flower,  transform_to_flower  },
    { GAME_TINO_TRANS,   &Player::tino,    transform_to_tino    },
};
// 하나의 루프로 처리
```

---

### 8. `Player` 구조체 초기화 개선
**파일:** `game.cpp:115,139`, `player.cpp:454`

26개짜리 위치 기반 초기화 3곳 존재. 필드 순서 바뀌면 전부 틀림.

```cpp
// 현재
mario = { 100, 300, 0, 0, 5, 0, 40, 40, 1, 0, 0, 0, 0, false, false, ... };

// 수정 방안 A: 리셋 함수 만들기
void ResetMario(int life, int coin) {
    mario = {};           // 전체 0/false 초기화
    mario.x = 100; mario.y = 300;
    mario.width = 40; mario.height = 40;
    mario.direction = 1;
    mario.life = life;
    mario.coin = coin;
}

// 수정 방안 B: Player 구조체에 기본값 설정 (생성자 활용)
```

---

### 9. `InitMonsters` 스테이지별 중복 제거
**파일:** `monster.cpp:21~86`

`InitMonsters()`, `InitMonsters2()`, `InitMonsters3()` 가 for 루프 본문이 동일하고 위치 배열만 다름.

```cpp
// 수정: 공통 함수로 추출
void InitMonstersFromData(const int* tileX, const int* tileY, int count, int w, int h) {
    monsterCount = count;
    for (int i = 0; i < count; i++) {
        monsters[i] = {};
        monsters[i].x = tileX[i] * TILE_SIZE;
        monsters[i].y = tileY[i] * TILE_SIZE;
        monsters[i].vx = -1;
        monsters[i].width = w; monsters[i].height = h;
        monsters[i].isAlive = true;
        monsters[i].active = true;
    }
}

void InitMonsters()  { static int x[]={15,28,...}; static int y[]={12,...}; InitMonstersFromData(x,y,18,20,10); }
void InitMonsters2() { static int x[]={30,31,...}; static int y[]={7,...};  InitMonstersFromData(x,y,10,20,20); }
void InitMonsters3() { static int x[]={34,35,...}; static int y[]={9,...};  InitMonstersFromData(x,y,11,20,20); }
```

---

### 10. 스프라이트 방향 전환 방식 변경
**파일:** `player.cpp:14~168`

방향이 바뀔 때마다 50개 이상의 `Image*`에 직접 `RotateFlip()` 호출 중.
이미지 객체 자체를 영구 변형시키기 때문에 상태 관리가 불명확하고 코드가 방대함.

```cpp
// 현재: 방향 전환 시 모든 스프라이트에 RotateFlip() 일괄 호출

// 수정: 이미지는 항상 오른쪽 방향 원본 유지
//       렌더링 시 direction == 0이면 DrawImage에 좌우반전 매트릭스 적용
void DrawSprite(Graphics& g, Image* img, int x, int y, int w, int h, bool flipX) {
    if (flipX) {
        // Matrix로 좌우 반전 후 그리기
        Matrix m(-1, 0, 0, 1, (REAL)(x + w), (REAL)y);
        g.SetTransform(&m);
        g.DrawImage(img, 0, 0, w, h);
        g.ResetTransform();
    } else {
        g.DrawImage(img, x, y, w, h);
    }
}
// movePlayer()에서 RotateFlip 블럭 전부 제거
```

> 이 항목은 renderer.cpp 수정도 같이 필요하므로 범위가 가장 넓음.

---

## P3 — 장기 구조 개선 (논의 후 진행)

### 11. 전역 변수 정리
현재 `mario`, `cameraX`, `gameState`, 모든 스프라이트 포인터 등이
`extern`으로 헤더에 선언되어 어디서든 접근 가능.

- 단계적으로 관련 상태를 구조체/네임스페이스로 묶는 방향 검토
- 당장 전면 리팩토링보다는 신규 기능 추가 시 새 패턴 적용 권장

### 12. 인코딩 통일
`monster.h`의 Bowser 구조체 주석이 CP949로 저장되어 일부 환경에서 깨짐.
전체 파일을 UTF-8 (BOM 없음)으로 통일 권장.

---

## 작업 순서 제안

| 단계 | 항목 | 예상 난이도 |
|------|------|------------|
| 1 | #1 항상 참 조건, #3 중복 extern, #4 죽은 코드 | 매우 낮음 |
| 2 | #5 매직 넘버, #6 motion_timer 함수화 | 낮음 |
| 3 | #2 IsColliding 분리, #7 변신 상태 통합 | 중간 |
| 4 | #8 Player 초기화, #9 InitMonsters 통합 | 중간 |
| 5 | #10 스프라이트 방향 전환 방식 변경 | 높음 |
| 6 | #11 전역 변수 정리, #12 인코딩 통일 | 높음 (별도 논의) |

---

*이 계획서는 검토 후 항목별로 승인/제외/우선순위 조정 가능.*
