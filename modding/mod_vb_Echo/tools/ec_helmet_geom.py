# -*- coding: utf-8 -*-
r"""Геометрия деталей шлема экзо Эхо (IMP RoninHeavy) по его поверхности — для концепта и для модели (этап 3б).

  python ec_helmet_geom.py [--debug=<папка>]     (Python из Blender 5.3: нужен numpy) — проверка: пазы, карты

Без Blender. Шлем читается odol_read (модель защищена — LOD ищется по структуре). Оси DayZ: лицо смотрит в -z.
* Карта глубины спереди: грани, смотрящие вперёд, растрируются на сетку (шаг STEP) — передняя поверхность z(x, y).
* Пазы дыхательной решётки — дыры в этой карте внутри пластины (сквозные: за ними тёмная внутренность шлема).
  Свет в пазу — лента по точному контуру паза (по строкам сетки), на INSET глубже поверхности по краям паза.
* Знак Эхо на висках — кольцо и пульс лентами, спроецированные на боковую поверхность (карта глубины сбоку).
"""
import math
import os
import sys

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
EDIT = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
sys.path.insert(0, os.path.join(EDIT, ".claude", "tools", "dayz-render"))
sys.path.insert(0, HERE)
import odol_read as O  # noqa: E402
import ec_geom as G  # noqa: E402

# оригиналы ODOL IMP (в мод не входят): в моде с 11-23 лежит собранная из них модель со светом (ec_helmet_model.py)
HELMET = os.path.join(HERE, "..", "исходники_моделей", "RoninHeavy.odol.p3d")
HELMET_F = os.path.join(HERE, "..", "исходники_моделей", "RoninHeavy_f.odol.p3d")
HELMET_DATA = os.path.join(HERE, "..", "VB_EchoClothes", "exo_set", "helmet", "ronin_heavy", "data")
STEP = 0.0005
INSET = 0.004
GRILL = (-0.085, 0.085, 1.49, 1.68)          # x0, x1, y0, y1 — область решётки
# высота знака Эхо на висках: в концепте 1,735 — на стыке верхней пластины и боковой плиты с рельсом; в текстуре
# нижняя половина кольца рассыпалась по краю плиты. 1,765 — целиком на верхней пластине, одна область UV (11-23)
EMBLEM_Y = 1.765


def load(path=HELMET, lod=0):
    l = O.read_visual_lod(path, lod)
    proxy = set()
    for s in l["selections"]:
        if s["name"].startswith("proxy:"):
            proxy.update(s["face_set"])
    tris = []
    for fi, f in enumerate(l["faces"]):
        if fi in proxy or len(set(f)) != len(f):
            continue
        for k in range(1, len(f) - 1):
            tris.append((f[0], f[k], f[k + 1]))
    return np.array(l["points"], dtype=np.float64), np.array(l["normals"], dtype=np.float64), np.array(tris)


def read_tga(path):
    """TGA без сжатия (ImageToPAA) -> массив (H, W, 4) 0..1, строка 0 — верх картинки."""
    d = open(path, "rb").read()
    idlen, cmtype, itype = d[0], d[1], d[2]
    w, h = int.from_bytes(d[12:14], "little"), int.from_bytes(d[14:16], "little")
    bpp, desc = d[16], d[17]
    if itype != 2 or bpp not in (24, 32):
        raise ValueError("TGA: тип %d, %d бит" % (itype, bpp))
    n = bpp // 8
    a = np.frombuffer(d, dtype=np.uint8, count=w * h * n, offset=18 + idlen).reshape(h, w, n).astype(np.float64)
    a = a[..., [2, 1, 0] + ([3] if n == 4 else [])] / 255.0
    if n == 3:
        a = np.dstack([a, np.ones((h, w))])
    if not (desc & 0x20):                    # начало снизу -> перевернуть
        a = a[::-1]
    return a


def raster(P, N, tris, axes, depth_axis, sign, box, step=STEP, normal_min=0.3, uv=None):
    """Карта глубины: проекция на плоскость axes=(a, b), глубина — координата depth_axis; берётся самая «внешняя»
    (sign=-1 — минимальная, например передняя z; +1 — максимальная, например правая x). Только грани, нормаль которых
    смотрит наружу (компонента по depth_axis * sign > normal_min). box = (a0, a1, b0, b1).
    uv — UV вершин (0..1, v сверху вниз): тогда вернёт ещё карту UV по точкам сетки (H, W, 2)."""
    a0, a1, b0, b1 = box
    W = int(round((a1 - a0) / step)) + 1
    H = int(round((b1 - b0) / step)) + 1
    D = np.full((H, W), np.nan)
    UVm = np.full((H, W, 2), np.nan) if uv is not None else None
    # нормаль — геометрическая и с обеих сторон: у части деталей (лицевая пластина с прорезями) нормали модели
    # смотрят внутрь — с фильтром по ним пластина пропадала и спереди была видна сетка за ней (11-23)
    gn = np.cross(P[tris[:, 1]] - P[tris[:, 0]], P[tris[:, 2]] - P[tris[:, 0]])
    gl = np.linalg.norm(gn, axis=1)
    keep = (gl > 1e-12) & (np.abs(gn[:, depth_axis]) > normal_min * np.maximum(gl, 1e-12))
    T = tris[keep]
    ia, ib = axes
    for t in T:
        a, b, c = P[t[0]], P[t[1]], P[t[2]]
        xs = np.array([a[ia], b[ia], c[ia]])
        ys = np.array([a[ib], b[ib], c[ib]])
        if xs.max() < a0 or xs.min() > a1 or ys.max() < b0 or ys.min() > b1:
            continue
        i0, i1 = max(0, int((xs.min() - a0) / step)), min(W - 1, int((xs.max() - a0) / step) + 1)
        j0, j1 = max(0, int((ys.min() - b0) / step)), min(H - 1, int((ys.max() - b0) / step) + 1)
        gx, gy = np.meshgrid(a0 + np.arange(i0, i1 + 1) * step, b0 + np.arange(j0, j1 + 1) * step)
        det = (ys[1] - ys[2]) * (xs[0] - xs[2]) + (xs[2] - xs[1]) * (ys[0] - ys[2])
        if abs(det) < 1e-14:
            continue
        l1 = ((ys[1] - ys[2]) * (gx - xs[2]) + (xs[2] - xs[1]) * (gy - ys[2])) / det
        l2 = ((ys[2] - ys[0]) * (gx - xs[2]) + (xs[0] - xs[2]) * (gy - ys[2])) / det
        l3 = 1 - l1 - l2
        inside = (l1 >= -1e-6) & (l2 >= -1e-6) & (l3 >= -1e-6)
        if not inside.any():
            continue
        z = l1 * a[depth_axis] + l2 * b[depth_axis] + l3 * c[depth_axis]
        sub = D[j0:j1 + 1, i0:i1 + 1]
        cur = np.where(np.isnan(sub), np.inf * -sign, sub)
        better = inside & ((z < cur) if sign < 0 else (z > cur))
        sub[better] = z[better]
        if UVm is not None:
            ua, ub, uc = uv[t[0]], uv[t[1]], uv[t[2]]
            su = UVm[j0:j1 + 1, i0:i1 + 1]
            su[better, 0] = (l1 * ua[0] + l2 * ub[0] + l3 * uc[0])[better]
            su[better, 1] = (l1 * ua[1] + l2 * ub[1] + l3 * uc[1])[better]
    if UVm is not None:
        return D, (a0, b0, step), UVm
    return D, (a0, b0, step)


def sample(tex, UVm):
    """Цвет текстуры (H, W, 4) по карте UV (nan -> nan)."""
    h, w = tex.shape[:2]
    ok = ~np.isnan(UVm[..., 0])
    out = np.full(UVm.shape[:2] + (4,), np.nan)
    u = np.clip((UVm[..., 0][ok] % 1.0) * w, 0, w - 1).astype(int)
    v = np.clip((UVm[..., 1][ok] % 1.0) * h, 0, h - 1).astype(int)
    out[ok] = tex[v, u]
    return out


def label(mask):
    """Связные области булевой маски (4-связность) -> список массивов индексов (j, i)."""
    H, W = mask.shape
    lab = np.zeros((H, W), dtype=np.int32)
    regions = []
    for j in range(H):
        for i in range(W):
            if mask[j, i] and not lab[j, i]:
                n = len(regions) + 1
                stack = [(j, i)]
                lab[j, i] = n
                pix = []
                while stack:
                    y, x = stack.pop()
                    pix.append((y, x))
                    for yy, xx in ((y - 1, x), (y + 1, x), (y, x - 1), (y, x + 1)):
                        if 0 <= yy < H and 0 <= xx < W and mask[yy, xx] and not lab[yy, xx]:
                            lab[yy, xx] = n
                            stack.append((yy, xx))
                regions.append(np.array(pix))
    return regions


def grill_slots(P, N, tris):
    """Пазы решётки: дыры в передней карте внутри пластины. -> (карта, сетка, список пазов {pix, bbox})."""
    D, grid = raster(P, N, tris, (0, 1), 2, -1, GRILL)
    # сквозь паз видна внутренняя поверхность шлема (тоже смотрит вперёд) — паз ищется по скачку глубины: точка
    # глубже передней поверхности по бокам (минимум z в окне ±WIN по горизонтали) больше чем на DEEP
    WIN, DEEP = 12, 0.006                                   # 12 точек = 6 мм; 6 мм глубже
    Dn = np.where(np.isnan(D), 1.0, D)
    H, W = Dn.shape
    mn = Dn.copy()
    for k in range(1, WIN + 1):
        mn[:, k:] = np.minimum(mn[:, k:], Dn[:, :-k])
        mn[:, :-k] = np.minimum(mn[:, :-k], Dn[:, k:])
    holes = (Dn - mn > DEEP) & (mn < 0.5)
    slots = []
    a0, b0, st = grid
    for reg in label(holes):
        if len(reg) < 40:                                  # меньше 10 мм² — шум растра
            continue
        js, is_ = reg[:, 0], reg[:, 1]
        h = (js.max() - js.min()) * st
        w = (is_.max() - is_.min()) * st
        if h < 0.02 or h < 2.5 * w:                        # пазы — узкие и длинные по вертикали
            continue
        slots.append({"pix": reg, "x": (a0 + is_.min() * st, a0 + is_.max() * st),
                      "y": (b0 + js.min() * st, b0 + js.max() * st), "area_mm2": len(reg) * st * st * 1e6})
    slots.sort(key=lambda s: s["x"][0])
    return D, grid, slots


def grill_slots_tex(P, N, tris, uv, tex, roi=(-0.07, 0.07, 1.50, 1.665)):
    """Прорези решётки по текстуре (11-23: сквозные только две, остальные нарисованы текстурой и картой нормалей):
    спереди по каждой точке сетки берётся цвет шлема по её UV; прорези — тёмные области внутри светлой пластины,
    узкие и вытянутые по вертикали, целиком внутри roi. Для каждой — сквозная ли (глубина внутри заметно больше, чем
    у краёв). -> (D, grid, список {pix, x, y, hole})."""
    D, grid, UVm = raster(P, N, tris, (0, 1), 2, -1, GRILL, uv=uv)
    a0, b0, st = grid
    Lm = sample(tex, UVm)[..., :3].mean(-1)
    H, W = D.shape
    jj, ii = np.mgrid[0:H, 0:W]
    xx, yy = a0 + ii * st, b0 + jj * st
    roi_m = (xx > roi[0]) & (xx < roi[1]) & (yy > roi[2]) & (yy < roi[3]) & ~np.isnan(Lm)
    vals = Lm[roi_m]
    thr = (np.percentile(vals, 10) + np.percentile(vals, 75)) / 2
    dark = roi_m & (Lm < thr)
    slots = []
    for reg in label(dark):
        if len(reg) < 60:
            continue
        js, is_ = reg[:, 0], reg[:, 1]
        x0, x1 = a0 + is_.min() * st, a0 + is_.max() * st
        y0, y1 = b0 + js.min() * st, b0 + js.max() * st
        h, w = y1 - y0, x1 - x0
        if h < 0.03 or h < 3 * w:
            continue
        if x0 <= roi[0] + st or x1 >= roi[1] - st or y0 <= roi[2] + st or y1 >= roi[3] - st:
            continue                                        # касается края — щёки и низ маски, не прорезь
        inner = np.nanmedian(D[js, is_])
        edges = []
        for j in np.unique(js)[::4]:
            row = is_[js == j]
            for k, sd in ((row.min() - 1, -1), (row.max() + 1, 1)):
                e = edge_depth(D, j, k, sd)
                if not np.isnan(e):
                    edges.append(e)
        hole = bool(edges) and inner > np.median(edges) + 0.004
        slots.append({"pix": reg, "x": (x0, x1), "y": (y0, y1), "hole": hole,
                      "area_mm2": len(reg) * st * st * 1e6})
    slots.sort(key=lambda s: s["x"][0])
    return D, grid, slots


def edge_depth(D, j, i, side):
    """Глубина поверхности у края паза: точка строки j сразу за краем паза слева (side=-1) или справа (+1)."""
    W = D.shape[1]
    k = i
    while 0 <= k < W and np.isnan(D[j, k]):
        k += side
    return D[j, k] if 0 <= k < W else np.nan


def slot_glow(D, grid, slot, inset=INSET, lift=0.0004, rows=2):
    """Лента света по контуру прорези: для каждой строки (шаг rows) отрезок от левого до правого края. Сквозная
    прорезь — свет внутри, на inset глубже поверхности у краёв; нарисованная — тонкая вставка на поверхности (lift
    наружу), глубина по самой поверхности в каждой точке. Нормаль — вперёд (-z).
    Сейчас всегда вставка по видимой поверхности: у сквозной это то, что видно сквозь неё — свет виден точно."""
    a0, b0, st = grid
    pix = slot["pix"]
    m = G.Mesh()
    prev = None
    for j in range(pix[:, 0].min(), pix[:, 0].max() + 1, rows):
        row = pix[pix[:, 0] == j][:, 1]
        if len(row) == 0:
            continue
        il, ir = row.min(), row.max()
        if slot["hole"] and inset < 0:        # свет за пластиной (не используется: см. ниже)
            zl, zr = edge_depth(D, j, il - 1, -1), edge_depth(D, j, ir + 1, 1)
            if np.isnan(zl) or np.isnan(zr):
                continue
            zl, zr = zl + inset, zr + inset
            xl, xr = a0 + (il - 0.5) * st, a0 + (ir + 0.5) * st     # чуть шире паза — края уходят под стенки
        else:
            zl, zr = D[j, il], D[j, ir]
            if np.isnan(zl) or np.isnan(zr):
                continue
            # сквозное место (видимая поверхность глубоко за пластиной): свет сразу за краем пластины, иначе под
            # углом его закрывают стенки (11-23: тёмный кончик средней прорези)
            el, er = edge_depth(D, j, il - 1, -1), edge_depth(D, j, ir + 1, 1)
            zl = el + 0.0015 if not np.isnan(el) and zl > el + 0.003 else zl - lift
            zr = er + 0.0015 if not np.isnan(er) and zr > er + 0.003 else zr - lift
            xl, xr = a0 + il * st, a0 + ir * st
        y = b0 + j * st
        cur = (m.add((xl, y, zl), (0, 0, -1), (0, 0)), m.add((xr, y, zr), (0, 0, -1), (1, 0)))
        if prev:
            m.faces.append([prev[0], prev[1], cur[1], cur[0]])
        prev = cur
    return G.fix_winding(m)


def helmet_texture(name="RoninHeavy_Black_echo_co"):
    """Текстура шлема как массив (через ImageToPAA во временный TGA)."""
    import subprocess
    import tempfile
    i2p = os.path.join("D:" + chr(92), "SteamLibrary", "steamapps", "common", "DayZ Tools", "Bin", "ImageToPAA",
                       "ImageToPAA.exe")
    src = os.path.join(HELMET_DATA, name + ".paa")
    with tempfile.TemporaryDirectory() as tmp:          # TGA 16 МБ — удаляется сразу после чтения (11-23)
        tga = os.path.join(tmp, name + ".tga")
        subprocess.run([i2p, src, tga], capture_output=True)
        return read_tga(tga)


def grill_glow_meshes(path=HELMET, tex=None):
    """Готовые ленты света во всех прорезях решётки (для концепта и модели)."""
    l = O.read_visual_lod(path, 0)
    P, N, tris = load(path)
    tex = helmet_texture() if tex is None else tex
    D, grid, slots = grill_slots_tex(P, N, tris, np.array(l["uv"]), tex)
    return [slot_glow(D, grid, s) for s in slots], slots


def rail_indicators(P):
    """Индикаторы на боковых рельсах (как в одобренном концепте): брусок 3×6×30 мм у крайней точки рельса по бокам
    на высоте 1,65–1,70, утоплен в поверхность на 0,5 мм (без щели)."""
    out = []
    for side in (-1, 1):
        m = (P[:, 0] * side > 0.118) & (P[:, 1] > 1.65) & (P[:, 1] < 1.70)
        if not m.any():
            continue
        c = P[m].mean(0)
        x = side * ((P[m][:, 0] * side).max() + 0.001)
        out.append(G.fix_winding(G.box((x, c[1], c[2]), (0.0015, 0.003, 0.015))))
    return out


# ------------------------------------------------------------------ знак Эхо на висках
def emblem_strokes(scale=1.0):
    """Знак Эхо (как на жилете): кольцо и линия пульса — ломаные в плоскости (u вправо, v вверх), метры."""
    ring = [(0.016 * scale * math.cos(2 * math.pi * k / 40), 0.016 * scale * math.sin(2 * math.pi * k / 40))
            for k in range(41)]
    pulse = [(px * scale, py * scale) for px, py in ((-0.024, 0), (-0.009, 0), (-0.005, 0.008), (-0.001, -0.012),
                                                     (0.003, 0.016), (0.007, 0), (0.024, 0))]
    return [ring, pulse]


def side_emblems(P, N, tris, center_y=None, center_z=0.0, width=0.0024, lift=0.0006):
    """Знак на каждом виске: ленты по ломаным, спроецированные на боковую поверхность (карта глубины по x)."""
    center_y = EMBLEM_Y if center_y is None else center_y
    out = []
    for side in (-1, 1):
        box = (center_z - 0.04, center_z + 0.04, center_y - 0.03, center_y + 0.03)       # (z, y)
        D, (a0, b0, st) = raster(P, N, tris, (2, 1), 0, side, box, step=0.0005)
        m = G.Mesh()

        def surf(u, v):
            z = center_z - side * u                         # вид сбоку: «вправо» на правом виске — к лицу (-z)
            y = center_y + v
            i, j = int(round((z - a0) / st)), int(round((y - b0) / st))
            j = min(max(j, 0), D.shape[0] - 1)
            i = min(max(i, 0), D.shape[1] - 1)
            x = D[j, i]
            if np.isnan(x):
                return None
            return np.array([x + side * lift, y, z])
        for stroke in emblem_strokes():
            prev = None
            for k in range(len(stroke) - 1):
                (u0, v0), (u1, v1) = stroke[k], stroke[k + 1]
                du, dv = u1 - u0, v1 - v0
                ln = math.hypot(du, dv) or 1.0
                nu, nv = -dv / ln * width / 2, du / ln * width / 2
                quad = [surf(u0 + nu, v0 + nv), surf(u0 - nu, v0 - nv), surf(u1 - nu, v1 - nv), surf(u1 + nu, v1 + nv)]
                if any(q is None for q in quad):
                    continue
                ids = [m.add(q, (side, 0, 0), (0, 0)) for q in quad]
                m.faces.append(ids)
        out.append(G.fix_winding(m))
    return out


def _seg_dist(px, py, a, b):
    ax, ay = a
    bx, by = b
    dx, dy = bx - ax, by - ay
    t = np.clip(((px - ax) * dx + (py - ay) * dy) / (dx * dx + dy * dy + 1e-12), 0, 1)
    return np.hypot(px - (ax + t * dx), py - (ay + t * dy))


def emblem_alpha(tex_h, tex_w, path=HELMET, center_y=None, center_z=0.0, width=0.0024, step=0.0002, aa=0.0004):
    """Знак Эхо на висках краской в текстуре (модель не трогается, знак есть и на шлеме на земле): раскладка — как
    side_emblems (одобренный концепт), только вместо полосок геометрии — альфа по UV. Боковая карта глубины с UV
    (шаг step), в каждой её точке — расстояние до ломаных знака в плоскости виска, точки переносятся в текстуру по
    своим UV. -> (альфа (tex_h, tex_w) 0..1, строка 0 — верх файла; [(сторона, u0, u1, v0, v1) в пикселях])."""
    center_y = EMBLEM_Y if center_y is None else center_y
    l = O.read_visual_lod(path, 0)
    P, N, tris = load(path)
    UV = np.array(l["uv"], dtype=np.float64)
    A = np.zeros((tex_h, tex_w))
    info = []
    for side in (-1, 1):
        box = (center_z - 0.04, center_z + 0.04, center_y - 0.03, center_y + 0.03)       # (z, y)
        D, (a0, b0, st), UVm = raster(P, N, tris, (2, 1), 0, side, box, step=step, uv=UV)
        H, W = D.shape
        jj, ii = np.mgrid[0:H, 0:W]
        u = side * (center_z - (a0 + ii * st))          # как в side_emblems: «вправо» на виске — к лицу
        v = (b0 + jj * st) - center_y
        d = np.full(D.shape, np.inf)
        for stroke in emblem_strokes():
            for k in range(len(stroke) - 1):
                d = np.minimum(d, _seg_dist(u, v, stroke[k], stroke[k + 1]))
        a = np.clip(0.5 - (d - width / 2) / aa, 0, 1)
        ok = (a > 0) & ~np.isnan(UVm[..., 0])
        if not ok.any():
            continue
        cols = np.clip((UVm[..., 0][ok] % 1.0) * tex_w, 0, tex_w - 1).astype(int)
        rows = np.clip((UVm[..., 1][ok] % 1.0) * tex_h, 0, tex_h - 1).astype(int)
        np.maximum.at(A, (rows, cols), a[ok])
        info.append((side, int(cols.min()), int(cols.max()), int(rows.min()), int(rows.max())))
    return A, info


if __name__ == "__main__":
    opts = {a.split("=", 1)[0][2:]: a.split("=", 1)[1] for a in sys.argv[1:] if a.startswith("--") and "=" in a}
    P, N, tris = load()
    l0 = O.read_visual_lod(HELMET, 0)
    D, grid, slots = grill_slots_tex(P, N, tris, np.array(l0["uv"]), helmet_texture())
    print("карта решётки", D.shape, "покрыто %.0f%%" % (100 * (~np.isnan(D)).mean()))
    for s in slots:
        print("  прорезь x %.4f..%.4f  y %.4f..%.4f  площадь %.0f мм²  %s" % (
            s["x"][0], s["x"][1], s["y"][0], s["y"][1], s["area_mm2"], "сквозная" if s["hole"] else "нарисована"))
    if opts.get("debug"):
        os.makedirs(opts["debug"], exist_ok=True)
        Dz = np.where(np.isnan(D), np.nanmax(D), D)
        img = (255 * (Dz.max() - Dz) / max(Dz.max() - Dz.min(), 1e-6)).astype(np.uint8)[::-1]   # ближе — светлее
        for s in slots:
            for j, i in s["pix"]:
                img[D.shape[0] - 1 - j, i] = 0
        with open(os.path.join(opts["debug"], "grill.pgm"), "wb") as f:
            f.write(b"P5 %d %d 255\n" % (img.shape[1], img.shape[0]) + img.tobytes())
        print("PGM ->", opts["debug"])
