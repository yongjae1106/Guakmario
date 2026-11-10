"""
Mario_sprite.png 스캔 스크립트
numpy로 행/열 경계를 감지하여 각 셀의 좌표를 추출한다.
"""

from PIL import Image, ImageDraw
import numpy as np

SPRITE_PATH = r"guakmario\resource\mario\Mario_sprite.png"
OUT_GRID    = r"guakmario\resource\mario\sprite_scan_grid.png"
OUT_CSV     = r"guakmario\resource\mario\sprite_cells.csv"

def make_bg_mask(arr):
    """배경(파란색 계열) 픽셀 → True"""
    r, g, b = arr[:,:,0].astype(float), arr[:,:,1].astype(float), arr[:,:,2].astype(float)
    return (b > 100) & (b > r * 1.2) & (b >= g * 0.85)

def find_spans(flags_1d, min_gap=1, min_span=4):
    """False 구간(스프라이트) 의 (start, end) 목록 반환"""
    spans = []
    in_span = False
    start = 0
    n = len(flags_1d)
    for i in range(n):
        if not flags_1d[i] and not in_span:
            in_span = True
            start = i
        elif flags_1d[i] and in_span:
            if i - start >= min_span:
                spans.append([start, i - 1])
            in_span = False
    if in_span and n - start >= min_span:
        spans.append([start, n - 1])

    # 짧은 간격 병합
    merged = []
    for s, e in spans:
        if merged and s - merged[-1][1] <= min_gap:
            merged[-1][1] = e
        else:
            merged.append([s, e])
    return [(s, e) for s, e in merged]

def main():
    img = Image.open(SPRITE_PATH)
    arr = np.array(img)          # H x W x 4
    H, W = arr.shape[:2]
    print(f"이미지 크기: {W} x {H}")

    bg = make_bg_mask(arr)       # H x W  bool

    # ── 행 경계 감지 ──────────────────────────────────────────────────────────
    # 각 행에서 배경 픽셀 비율
    row_bg_ratio = bg.mean(axis=1)              # (H,)
    row_flags    = row_bg_ratio >= 0.88
    row_spans    = find_spans(row_flags, min_gap=2, min_span=6)
    print(f"감지된 행 블록: {len(row_spans)}")

    # ── 행별 열 경계 감지 ─────────────────────────────────────────────────────
    cells = []
    for ry, (ys, ye) in enumerate(row_spans):
        band_bg   = bg[ys:ye+1, :]              # 행 밴드 슬라이스
        col_ratio = band_bg.mean(axis=0)        # (W,)
        col_flags = col_ratio >= 0.85
        col_spans = find_spans(col_flags, min_gap=1, min_span=3)
        for cx, (xs, xe) in enumerate(col_spans):
            cells.append((ry, cx, xs, ys, xe - xs + 1, ye - ys + 1))

    print(f"총 셀 수: {len(cells)}")

    # ── CSV 저장 ──────────────────────────────────────────────────────────────
    with open(OUT_CSV, "w") as f:
        f.write("row,col,x,y,w,h\n")
        for ry, cx, x, y, w, h in cells:
            f.write(f"{ry},{cx},{x},{y},{w},{h}\n")
    print(f"CSV 저장: {OUT_CSV}")

    # ── 그리드 시각화 ─────────────────────────────────────────────────────────
    vis  = img.convert("RGB")
    draw = ImageDraw.Draw(vis)
    for ry, cx, x, y, w, h in cells:
        draw.rectangle([x, y, x+w-1, y+h-1], outline=(255, 0, 0), width=1)
        # 셀이 충분히 크면 인덱스 표시
        if w > 12 and h > 8:
            draw.text((x+1, y+1), f"{ry},{cx}", fill=(255, 255, 0))
    vis.save(OUT_GRID)
    print(f"그리드 이미지 저장: {OUT_GRID}")

    # ── 행별 요약 출력 ────────────────────────────────────────────────────────
    print("\n── 행별 요약 ──")
    prev_row = -1
    for ry, cx, x, y, w, h in cells:
        if ry != prev_row:
            col_count = sum(1 for c in cells if c[0] == ry)
            print(f"  행{ry:2d}  y={y:4d}  h={h:3d}  열수={col_count}")
            prev_row = ry

    # ── 첫 행 셀 상세 출력 ────────────────────────────────────────────────────
    print("\n── 행 0 셀 좌표 ──")
    for ry, cx, x, y, w, h in cells:
        if ry == 0:
            print(f"  [{ry},{cx}]  x={x:4d}  y={y:4d}  w={w:3d}  h={h:3d}")

if __name__ == "__main__":
    main()
