# Guakmario 코드 정리 계획

> 작성일: 2026-10-06
> 기준 브랜치: main

---

## 진행 상태 범례
- [ ] 미완료
- [x] 완료

---

## 단계 1 — 중복 함수 제거

### 1-1. `IsColliding_item` 제거
**파일:** `collision.cpp`, `collision.h`

`IsColliding`과 `IsColliding_item`이 완전히 동일한 구현. `IsColliding_item` 제거 후 호출부를 `IsColliding`으로 교체.

- [x] `collision.cpp`에서 `IsColliding_item` 정의 제거
- [x] `collision.h`에서 선언 제거
- [x] 호출부 전체를 `IsColliding`으로 교체

---

## 단계 2 — 함수명 통일 (snake_case → PascalCase)

현재 `checkcollision_flag`, `transform_bigmario`, `god_mario` 같은 snake_case 함수들이
`CheckMarioMonsterCollision`, `UpdatePlayer` 같은 PascalCase 함수들과 혼재.

### 2-1. collision.cpp
| 현재 | 변경 후 |
|------|---------|
| `checkcollision_flag()` | `CheckFlagCollision()` |
| `checkcollision_clear()` | `CheckClearCollision()` |
| `checkcollision_coin()` | `CheckCoinCollision()` |

- [x] 함수 정의 및 선언 이름 변경
- [x] `game.cpp` 호출부 수정

### 2-2. item.cpp — 변신
| 현재 | 변경 후 |
|------|---------|
| `transform_bigmario()` | `TransformToBig()` |
| `transform_smallmario()` | `TransformToSmall()` |
| `transform_to_flower()` | `TransformToFlower()` |
| `transform_to_tino()` | `TransformToTino()` |

- [x] 함수 정의 및 선언 이름 변경
- [x] 호출부 수정 (game.cpp, item.cpp)

### 2-3. item.cpp — 기타
| 현재 | 변경 후 |
|------|---------|
| `god_mario()` | `UpdateGodMode()` |
| `star_mario()` | `UpdateStarMode()` |
| `fireball_spawn()` | `SpawnFireball()` |
| `tinofire_spawn()` | `SpawnTinoFire()` |
| `tino_attack()` | `TinoAttack()` |

- [x] 함수 정의 및 선언 이름 변경
- [x] 호출부 수정

### 2-4. game.cpp
| 현재 | 변경 후 |
|------|---------|
| `stage_load()` | `LoadStage()` |
| `monster_reset()` | `ResetMonsters()` |
| `item_reset()` | `ResetItems()` |
| `timegoes()` | `TickTimer()` |

- [x] 함수 정의 및 선언 이름 변경
- [x] 호출부 수정

### 2-5. map.cpp
| 현재 | 변경 후 |
|------|---------|
| `trap_reset()` | `ResetTraps()` |

- [x] 함수 정의 및 선언 이름 변경
- [x] 호출부 수정

---

## 단계 3 — 변수명 통일 (camelCase)

### 3-1. Player 구조체 (`player.h`)
| 현재 | 변경 후 |
|------|---------|
| `isflying` | `isFlying` |

- [x] 선언 및 모든 참조 수정

### 3-2. Monster 구조체 (`monster.h`)
| 현재 | 변경 후 |
|------|---------|
| `deadstart` | `deadStart` |

- [x] 선언 및 모든 참조 수정

### 3-3. GameContext 구조체 (`game.h`)
| 현재 | 변경 후 |
|------|---------|
| `gamestart` | `gameStart` |
| `gameclear_text` | `gameClearText` |
| `frame_motion` | `frameMotion` |
| `frame_motion_star` | `frameMotionStar` |

- [x] 선언 및 모든 참조 수정

### 3-4. Item 구조체 (`item.h`)
| 현재 | 변경 후 |
|------|---------|
| `spawn_motion` | `spawnMotion` |

- [x] 선언 및 모든 참조 수정

### 3-5. Bowser 구조체 (`monster.h`)
| 현재 | 변경 후 |
|------|---------|
| `ignore_tinofire` | `ignoreTinoFire` |
| `ignore_tinobite` | `ignoreTinoBite` |

- [x] 선언 및 모든 참조 수정

---

## 단계 4 — Update/Check 함수 묶기

세부 함수들은 유지하되, `game.cpp`에서 개별 호출하는 부분을 래퍼 함수로 묶어 `UpdateGame()`을 간결하게.

### 4-1. `UpdateAllItems()` — item.cpp에 추가
```
UpdateItems()
UpdateItems_up_mushroom()
UpdateItems_star()
UpdateItems_flower()
UpdateItems_tino()
```

### 4-2. `UpdateAllShots()` — item.cpp에 추가
```
UpdateShot_fireball()
UpdateShot_tinofire()
UpdateShot_tinofire_effect()
```

### 4-3. `CheckItemCollisions()` — item.cpp에 추가
```
CheckCollision_mushroom()
CheckCollision_up_mushroom()
CheckCollision_star()
CheckCollision_flower()
CheckCollision_tino()
```

### 4-4. `CheckShotCollisions()` — item.cpp에 추가
```
CheckCollision_fireball()
CheckCollision_tinofire()
```

### 4-5. `UpdateAllMonsters()` — monster.cpp에 추가
```
UpdateMonsters()
UpdateTurtles()
UpdateAngelTurtles()
UpdateBowser()
UpdateFireballs()
UpdateFireTraps()
```

### 4-6. `CheckAllMonsterCollisions()` — monster.cpp에 추가
```
CheckMarioMonsterCollision()
CheckMarioTurtleCollision()
CheckMarioAngelTurtleCollision()
CheckMarioHazardCollision()
CheckMarioBowserCollision()
CheckMarioFireballCollision()
```

- [x] 래퍼 함수 추가
- [x] `game.cpp`의 `UpdateGame()` 호출부 단순화

---

## 단계 5 — Init 함수 스테이지 파라미터화

### 5-1. `InitMonsters(int stage)` 통합
현재 `InitMonsters()`, `InitMonsters2()`, `InitMonsters3()`로 분리된 것을 파라미터로 통합.

### 5-2. `InitTurtles(int stage)` 통합
현재 `InitTurtles()`, `InitTurtles2()`, `InitTurtles3()`로 분리된 것을 파라미터로 통합.

- [ ] 통합 함수 작성
- [ ] `stage_load()` 호출부 수정
- [ ] 기존 함수 제거

---

## 작업 순서

| 단계 | 내용 | 범위 | 난이도 |
|------|------|------|--------|
| 1 | 중복 함수 제거 (`IsColliding_item`) | collision | 낮음 |
| 2 | 함수명 통일 (snake_case → PascalCase) | 전체 | 낮음 |
| 3 | 변수명 통일 (camelCase) | 전체 | 낮음 |
| 4 | Update/Check 래퍼 함수 묶기 | item, monster, game | 중간 |
| 5 | Init 함수 파라미터화 | monster, game | 중간 |
