# -*- coding: utf-8 -*-
r"""Сборка модели Echo Runner из MLOD ALEX (этап 2: кузов).

  python er_build.py <вариант> --R=0.46 --raise=0.03

  вариант  — папка в work/ (v092, v080 ...); туда же er_wheel.py кладёт колесо proxy/interceptor_wheel.p3d
  --R      — видимый радиус нового колеса
  --raise  — насколько верх шины поднимается относительно колеса ALEX (м)

Что делает:
  1. вырезает встроенные баки ALEX из всех LOD (визуальные, виды из кабины, Fire/View Geometry);
     вырезанное из LOD 1 пишет в work/<вариант>/tank_alex.p3d — для рендера и как основа навесного бака;
  2. подъём: колёса и подвеска опускаются на dc = R - R_ALEX - raise (кузов на месте), стойки и рычаги
     тянутся (вес по высоте/вылету), земля (pos_*, LandContact, damper_land) опускается на
     drop = dc + (R - R_ALEX);
  3. пишет work/<вариант>/interceptor.p3d, двери/капот копирует в work/<вариант>/proxy/.
Оси MLOD: y вверх, нос в -z, +x — левый борт (водитель).
"""
import os
import shutil
import sys
from collections import defaultdict

sys.path.insert(0, r"d:\DayZavr\DayZavr-Edit\.claude\tools\py3d")
import py3d  # noqa: E402

HERE = os.path.dirname(os.path.abspath(__file__))
MOD = os.path.dirname(HERE)
BASE = os.path.join(MOD, "основа_ALEX_mlod")

args = sys.argv[1:]
opts = {a.split("=", 1)[0][2:]: (a.split("=", 1)[1] if "=" in a else True) for a in args if a.startswith("--")}
VAR = [a for a in args if not a.startswith("--")][0]
OUTDIR = os.path.join(MOD, "work", VAR)
R_NEW = float(opts.get("R", 0.46))
RAISE = float(opts.get("raise", 0.03))
R_ALEX = 0.366
DC = R_NEW - R_ALEX - RAISE
DROP = DC + (R_NEW - R_ALEX)

TANK_BOX = ((-0.56, 0.56), (0.85, 1.48), (1.45, 2.66))      # встроенные баки + колпачки + счётчик между ними
FRONT_Y_AXLE, REAR_Y_AXLE = 0.446, 0.394
SUSP = ["susp_damper_1_1", "susp_hub_1_1", "wheel_1_1", "susp_damper_2_1", "susp_hub_2_1", "wheel_2_1",
        "susp_axle_2_dummy", "susp_axle_2", "susp_damper_1_2", "susp_damper_2_2", "wheel_1_2", "wheel_2_2"]
FRONT_STRUT = {"susp_damper_1_1", "susp_hub_1_1", "susp_damper_2_1", "susp_hub_2_1"}
AXIS_MEM = [s + "_axis" for s in SUSP] + ["wheel_1_1_axis", "wheel_2_1_axis", "wheel_1_2_axis", "wheel_2_2_axis"]
GROUND_MEM = ["pos_driver", "pos_driver_dir", "pos_codriver", "pos_codriver_dir"]


def is_res(lod, r):
    return abs(lod.resolution / r - 1) < 1e-3


def clamp(v, a=0.0, b=1.0):
    return max(a, min(b, v))


def inside(bb, box):
    return all(box[i][0] <= bb[i][0] and bb[i][1] <= box[i][1] for i in range(3))


def face_bbox(lod, faces):
    pts = [lod.points[v.point_index].coords for fa in faces for v in fa.vertices]
    return [(min(c[i] for c in pts), max(c[i] for c in pts)) for i in range(3)]


def components(lod, faces):
    """Связные куски (склейка по индексу точки и по совпадающим координатам — швы UV)."""
    parent = {}

    def find(a):
        while parent.setdefault(a, a) != a:
            parent[a] = parent[parent[a]]
            a = parent[a]
        return a
    key = [tuple(round(c, 4) for c in p.coords) for p in lod.points]
    for fa in faces:
        ks = [key[v.point_index] for v in fa.vertices]
        for k in ks[1:]:
            ra, rb = find(ks[0]), find(k)
            if ra != rb:
                parent[ra] = rb
    comp = defaultdict(list)
    for fa in faces:
        comp[find(key[fa.vertices[0].point_index])].append(fa)
    return list(comp.values())


def prune(lod, rm_faces):
    """Удалить грани (и точки, которые были только у них). Списки меняются на месте — на них ссылаются
    Vertex/Selection."""
    rm = {id(f) for f in rm_faces}
    keep = [f for f in lod.faces if id(f) not in rm]
    used_keep = {v.point_index for f in keep for v in f.vertices}
    used_rm = {v.point_index for f in rm_faces for v in f.vertices}
    drop_pts = used_rm - used_keep
    new_index, new_points = {}, []
    for i, p in enumerate(lod.points):
        if i in drop_pts:
            continue
        new_index[i] = len(new_points)
        new_points.append(p)
    for f in keep:
        for v in f.vertices:
            v.point_index = new_index[v.point_index]
    dropped = {id(lod.points[i]) for i in drop_pts}
    lod.faces[:] = keep
    lod.points[:] = new_points
    for name in list(lod.selections):
        sel = lod.selections[name]
        sel.points = {p: w for p, w in sel.points.items() if id(p) not in dropped}
        sel.faces = {f: w for f, w in sel.faces.items() if id(f) not in rm}
        if not sel.points and not sel.faces:
            del lod.selections[name]


def copy_faces_to_lod(src, faces, res=1.0):
    out = py3d.LOD()
    out.resolution = res
    pmap, nmap = {}, {}
    for fa in faces:
        nf = py3d.Face(out.points, out.facenormals)
        nf.texture, nf.material, nf.flags = fa.texture, fa.material, fa.flags
        for v in fa.vertices:
            if v.point_index not in pmap:
                p = py3d.Point()
                p.coords = src.points[v.point_index].coords
                pmap[v.point_index] = len(out.points)
                out.points.append(p)
            if v.normal_index not in nmap:
                nmap[v.normal_index] = len(out.facenormals)
                out.facenormals.append(src.facenormals[v.normal_index])
            nv = py3d.Vertex(out.points, out.facenormals)
            nv.point_index, nv.normal_index, nv.uv = pmap[v.point_index], nmap[v.normal_index], v.uv
            nf.vertices.append(nv)
        out.faces.append(nf)
    return out


def cut_tanks(m):
    tank_lod1 = None
    for lod in m.lods:
        proxyf = set()
        for n, s in lod.selections.items():
            if n.startswith("proxy:"):
                proxyf.update(id(x) for x in s.faces)
        rm = []
        if lod.resolution < 1e4:                                   # визуальные и виды из кабины
            cand = [f for f in lod.faces if id(f) not in proxyf]
            for c in components(lod, cand):
                if inside(face_bbox(lod, c), TANK_BOX):
                    rm.extend(c)
        elif is_res(lod, 6e15) or is_res(lod, 7e15):               # Fire / View Geometry: целые компоненты
            for n, s in list(lod.selections.items()):
                if not n.lower().startswith("component") or not s.faces:
                    continue
                pts = [p.coords for p in s.points]
                bb = [(min(c[i] for c in pts), max(c[i] for c in pts)) for i in range(3)]
                if inside(bb, TANK_BOX):
                    rm.extend(s.faces.keys())
        if not rm:
            continue
        if is_res(lod, 1.0):
            tank_lod1 = copy_faces_to_lod(lod, rm)
        print("  баки: LOD %g — убрано граней %d" % (lod.resolution, len(rm)))
        prune(lod, rm)
    return tank_lod1


def lift(m):
    for lod in m.lods:
        moved = {}
        # кости колёс и подвески
        sels = [s for s in SUSP if s in lod.selections]
        if sels and lod.resolution < 1e12:
            ytop = {}
            for s in sels:
                ys = [p.coords[1] for p in lod.selections[s].points]
                ytop[s] = max(ys) if ys else 0
            for s in sels:
                for p in lod.selections[s].points:
                    if id(p) in moved:
                        continue
                    w = 1.0
                    if s in FRONT_STRUT:                            # стойка тянется: низ целиком, верх на месте
                        yt = ytop[s]
                        w = clamp((yt - p.coords[1]) / max(yt - (FRONT_Y_AXLE + 0.05), 1e-3))
                    moved[id(p)] = (p, w)
            # неподвижная ходовая: рычаги спереди наклоняются, задний мост опускается
            gear = set()
            for s in ("hodovka", "hodovka_anm"):
                if s in lod.selections:
                    gear.update(lod.selections[s].points.keys())
            for p in gear:
                if id(p) in moved:
                    continue
                x, y, z = p.coords
                w = 0.0
                if -1.75 <= z <= -0.85:
                    w = clamp((abs(x) - 0.25) / 0.37) * clamp((0.75 - y) / 0.25)
                elif 1.25 <= z <= 2.1:
                    w = clamp((0.62 - y) / 0.14)
                if w > 0:
                    moved[id(p)] = (p, w)
        # оси в памяти
        if is_res(lod, 1e15):
            for s in AXIS_MEM:
                if s in lod.selections:
                    for p in lod.selections[s].points:
                        moved[id(p)] = (p, 1.0)
        # Geometry: wheel_*_damper_land — коробки вокруг колёс (по 150 кг), едут вместе с колесом
        if is_res(lod, 1e13):
            for n, s in lod.selections.items():
                if n.endswith("_damper_land"):
                    for p in s.points:
                        moved[id(p)] = (p, 1.0)
        for p, w in moved.values():
            x, y, z = p.coords
            p.coords = (x, y - DC * w, z)
        # земля
        ground = []
        if is_res(lod, 1e15):
            for s in GROUND_MEM:
                if s in lod.selections:
                    ground.extend(lod.selections[s].points)
        if is_res(lod, 2e15):                                       # LandContact: 4 точки под колёсами
            ground.extend(lod.points)
        seen = set()
        for p in ground:
            if id(p) in seen:
                continue
            seen.add(id(p))
            x, y, z = p.coords
            p.coords = (x, y - DROP, z)
        if moved or ground:
            print("  подъём: LOD %g — точек подвески %d, земли %d" % (lod.resolution, len(moved), len(seen)))


def main():
    print("вариант %s: R %.3f, верх шины +%.3f, колёса вниз на %.3f, земля вниз на %.3f (подъём кузова)"
          % (VAR, R_NEW, RAISE, DC, DROP))
    with open(os.path.join(BASE, "interceptor.p3d"), "rb") as f:
        m = py3d.P3D(f)
    tank = cut_tanks(m)
    lift(m)
    os.makedirs(os.path.join(OUTDIR, "proxy"), exist_ok=True)
    m.save(os.path.join(OUTDIR, "interceptor.p3d"))
    if tank is not None:
        t = py3d.P3D()
        t.lods.append(tank)
        t.save(os.path.join(OUTDIR, "tank_alex.p3d"))
    for n in ("interceptor_doors_driver", "interceptor_doors_codriver", "interceptor_doors_hood"):
        shutil.copy2(os.path.join(BASE, "proxy", n + ".p3d"), os.path.join(OUTDIR, "proxy", n + ".p3d"))
    print("OK", OUTDIR)


main()
