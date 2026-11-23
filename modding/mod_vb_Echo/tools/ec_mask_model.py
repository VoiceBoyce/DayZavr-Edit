# -*- coding: utf-8 -*-
r"""Сборка модели маски экзо Эхо (череп OOTS MaskGhost) — этап 3б.

  python ec_mask_model.py            (Python из Blender 5.3: нужен numpy)

Вход — оригинал ODOL OOTS (modding/mod_vb_Echo/исходники_моделей/MaskGhost.odol.p3d, в мод не входит).
Выход — MLOD VB_EchoClothes/exo_set/Mask/MaskGhost/MaskGhost.p3d + model.cfg; бинаризует pbo.ps1 -Binarize.
Выделение vbplate — пластина черепа (всё, что целиком перед лицом): под шлемом экзо прячется (simpleHiddenSelections,
скрипт VB_EC_Exo_Mask_Base), иначе торчит сквозь его лицевую пластину; балаклава остаётся.
Линзы в глазницах (LENSES) — решение пользователя 11-23: убрать («выглядит не очень»), глазницы как у оригинала.
Код линз оставлен (по плоскости переднего края глазницы, выделение vbglow).
"""
import os
import re
import sys
import tempfile

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import ec_servo_model as S  # noqa: E402  (общие пути, LodEditor, odol2mlod, py3d)
import ec_geom as G  # noqa: E402

O, M2 = S.O, S.M2
SRC = os.path.join(HERE, "..", "исходники_моделей", "MaskGhost.odol.p3d")
OUTDIR = os.path.join(HERE, "..", "VB_EchoClothes", "exo_set", "Mask", "MaskGhost")
DST = os.path.join(OUTDIR, "MaskGhost.p3d")
CFG = os.path.join(OUTDIR, "model.cfg")
LENS_UV = [(277, 421), (650, 420)]          # центры овалов линз на текстуре (пиксели файла 2048, сверху вниз)
LENSES = False                              # линзы в глазницах (11-23 пользователь: убрать)


def lens_meshes(od):
    """Линза на каждую глазницу. Развёртка пластины черепа общая на обе стороны (оба овала UV ложатся на обе
    глазницы), поэтому ободки берутся из обоих овалов и делятся по стороне лица (знак x). Линза — в своей глазнице,
    на 30 % глубины ободка от переднего края (лицо смотрит в -z): перед глазами персонажа, а не за ними (11-23:
    стояла на 4 мм вглубь и одной плашкой на оба глаза — в игре были видны глаза и кожа лица)."""
    UV = np.array(od["uv"], dtype=np.float64) * 2048.0
    P = np.array(od["points"], dtype=np.float64)
    rim = set()
    for f in od["faces"]:
        cu, cv = UV[list(f)].mean(0)
        if any(((cu - cx) / 130) ** 2 + ((cv - cy) / 90) ** 2 < 1.0 for cx, cy in LENS_UV):
            rim.update(f)
    out = []
    for side in (-1, 1):
        vs = {v for v in rim if P[v][0] * side > 0.004}
        if len(vs) < 8:
            continue
        pts = P[sorted(vs)]
        # проём наклонён по лицу (у носа впереди, снаружи глубже): передний край — по кругу самые передние точки,
        # линза — в плоскости этого края, на 3 мм глубже, чуть меньше проёма (11-23: плоская линза поперёк z
        # пересекала ободок — тёмные клинья в глазницах)
        cxy = pts[:, :2].mean(0)
        ang = np.arctan2(pts[:, 1] - cxy[1], pts[:, 0] - cxy[0])
        ring = []
        for k in range(24):
            a0 = -np.pi + 2 * np.pi * k / 24
            m = (ang >= a0) & (ang < a0 + 2 * np.pi / 24)
            if m.any():
                sel = pts[m]
                ring.append(sel[sel[:, 2].argmin()])
        ring = np.array(ring)
        c = ring.mean(0)
        n = np.linalg.svd(ring - c, full_matrices=False)[2][2]
        if n[2] > 0:
            n = -n                                      # нормаль — вперёд (лицо смотрит в -z)
        u = np.array([1.0, 0.0, 0.0]) - n * n[0]
        u /= np.linalg.norm(u)
        v = np.cross(n, u)
        ru = float(np.abs((ring - c) @ u).max()) * 1.0
        rv = float(np.abs((ring - c) @ v).max()) * 1.0
        ctr = c - n * 0.002
        R = np.stack([u, n, v], axis=1)                 # ellipsoid: x -> u, y (полюса) -> n, z -> v
        out.append((G.fix_winding(G.ellipsoid(ctr, (ru, 0.003, rv), R, 20, 8)), vs))
        print("     линза центр %s нормаль %s радиусы %.3f %.3f" % (np.round(ctr, 3), np.round(n, 2), ru, rv))
    return out


def main():
    tmp = os.path.join(tempfile.mkdtemp(), "MaskGhost.p3d")
    m = M2.convert(SRC, tmp, cfg=CFG)
    os.remove(tmp)
    os.rmdir(os.path.dirname(tmp))
    data = open(SRC, "rb").read()
    ver, nl, res, p = O.header(data)
    skel = M2.parse_skeleton(data, p)
    bones = skel["bones"] if skel else []
    lenses = [mesh for mesh, _ in lens_meshes(O.read_visual_lod(SRC, 0))] if LENSES else []   # по LOD0 (у дальних
    for li in range(nl):                                                     # LOD развёртка ободков другая)
        r = res[li]
        if r >= 1000:
            continue
        od = O.read_visual_lod(SRC, li)
        W = O.read_bone_weights(data, od) if bones else None
        if not W:
            print("   LOD", r, "без весов — пропуск")
            continue
        ed = S.LodEditor(m.lods[li], od, W, bones)
        P = np.array(od["points"], dtype=np.float64)
        lod = m.lods[li]
        plate = ed.sel("vbplate")
        n = 0
        for mesh in lenses:
            c = np.mean(mesh.pts, axis=0)
            vs = {i for i in range(len(P)) if np.linalg.norm(P[i] - c) < 0.05}
            nf = ed.add_mesh(mesh, S.TEX_GLOW, S.MAT_GLOW, "vbglow", vs)
            for fa in lod.faces[-nf:]:                       # линзы — тоже часть «лица» маски
                plate.faces[fa] = 1
                for vx in fa.vertices:
                    plate.points[lod.points[vx.point_index]] = 1
            n += 1
        # пластина черепа (всё, что целиком перед лицом: z < -0,05; балаклава уходит на затылок) — выделение
        # vbplate: под шлемом экзо прячется (simpleHiddenSelections), иначе торчит сквозь лицевую пластину шлема
        npl = 0
        for comp in G.components(od["points"], od["faces"]):
            zmax = max(P[v][2] for fi in comp for v in od["faces"][fi])
            if zmax >= -0.05:
                continue
            for fi in comp:
                fa = ed.mlod_face.get(fi)
                if fa is None:
                    continue
                plate.faces[fa] = 1
                for vx in fa.vertices:
                    plate.points[lod.points[vx.point_index]] = 1
                npl += 1
        print("   LOD", r, "линз", n, "граней пластины", npl)
    m.save(DST)
    print("MLOD ->", DST)
    cfg = open(CFG, encoding="utf-8").read()
    head, sep, tail = cfg.partition("class MaskGhost: Default")
    assert sep, "в model.cfg нет класса MaskGhost"

    def add_sections(mt):
        items = [x.strip() for x in mt.group(1).split(",") if x.strip()]
        for s_ in (('"vbglow"', '"vbplate"') if LENSES else ('"vbplate"',)):
            if s_ not in items:
                items.append(s_)
        return "sections[] = {%s};" % ", ".join(items)
    cfg = head + sep + re.sub(r"sections\[\] = \{([^}]*)\};", add_sections, tail, count=1)
    cfg = cfg.replace("// Восстановлено из ODOL утилитой odol2mlod.py (MaskGhost)",
                      "// Маска экзо Эхо: собрано tools/ec_mask_model.py из ODOL OOTS (odol2mlod с весами костей)")
    open(CFG, "w", encoding="utf-8").write(cfg)
    print("model.cfg ->", CFG)


if __name__ == "__main__":
    main()
