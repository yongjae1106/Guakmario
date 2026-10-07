#pragma once

// ============================================================
//  Game Configuration — 여기서 수치를 수정하세요
// ============================================================

// --- 스테이지 ---
#define STAGE_TIME_LIMIT        400     // 스테이지 제한 시간 (틱 단위)
#define COIN_1UP_THRESHOLD      99      // 1UP에 필요한 코인 수

// --- 플레이어 기본값 ---
#define PLAYER_INITIAL_LIFE     5       // 시작 목숨 수
#define PLAYER_WALK_SPEED       5       // 좌우 이동 속도 (px/frame)
#define PLAYER_JUMP_VY          (-19)   // 점프 초기 수직 속도
#define PLAYER_GRAVITY_MAX      15      // 최대 낙하 속도 (중력 상한)
#define PLAYER_FALL_DEATH_Y     800     // 이 y 좌표 아래로 떨어지면 사망

// --- 티노 바이트 (스페이스) ---
#define TINO_BITE_COOLDOWN      15      // 바이트 쿨타임 (프레임)
#define TINO_BITE_MOTION_FRAMES 30      // 바이트 모션 지속 프레임
#define TINO_BITE_WIDTH         50      // 바이트 히트박스 너비 (px)
#define TINO_BITE_HEIGHT        100     // 바이트 히트박스 높이 (px)
#define TINO_BITE_Y_OFFSET      (-15)   // 바이트 히트박스 y 오프셋
#define TINO_BITE_KNOCKBACK_VY  (-15)   // 바이트에 맞은 몬스터 날아가는 속도

// --- 티노 파이어 (z키) ---
#define TINO_FIRE_COOLDOWN      5       // 파이어 쿨타임 (프레임)
#define TINO_FIRE_MOTION_FRAMES 10      // 파이어 모션 지속 프레임

// --- 파이어플라워 (스페이스) ---
#define FLOWER_FIRE_MOTION_FRAMES 5     // 파이어볼 발사 모션 프레임

// --- 타이밍 (밀리초) ---
#define TRANSFORM_DURATION_MS   700     // 변신 연출 시간
#define GOD_MODE_DURATION_MS    1000    // 무적 지속 시간
#define STAR_MODE_DURATION_MS   10000   // 스타 무적 지속 시간
#define DEAD_ANIMATION_MS       2000    // 사망 후 부활까지 대기
#define DEATH_REVIVE_DELAY_MS   3000    // GAME_OVER 후 부활 딜레이
#define VICTORY_NEXT_STAGE_MS   5000    // 스테이지 클리어 후 다음 스테이지 전환
#define CLEAR_WALK_MS           5000    // 최종 클리어 걷기 연출 시간
#define CLEAR_RESET_MS          10000   // 최종 클리어 후 타이틀 복귀
