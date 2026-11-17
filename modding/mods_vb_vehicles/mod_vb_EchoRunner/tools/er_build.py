# -*- coding: utf-8 -*-
r"""Сборка модели Echo Runner из MLOD ALEX (этап 2: кузов).

  python er_build.py <вариант> --R=0.46 --raise=0.03

  вариант  — папка в work/ (v092, v080 ...); туда же er_wheel.py кладёт колесо proxy/interceptor_wheel.p3d
  --R      — видимый радиус нового колеса
  --raise  — насколько верх шины поднимается относительно колеса ALEX (м)

Что делает:
  1. вырезает встроенные баки ALEX из всех LOD (визуальные, виды из кабины, Fire/View Geometry);
     вырезанное из LOD 1–4 пишет в work/<вариант>/tank_alex.p3d — для рендера и как основа навесного бака;
  2. подъём: колёса и подвеска опускаются на dc = R - R_ALEX - raise (кузов на месте), стойки и рычаги
     тянутся (вес по высоте/вылету), земля (pos_*, LandContact, damper_land) опускается на
     drop = dc + (R - R_ALEX);
  3. пишет work/<вариант>/interceptor.p3d, двери/капот копирует в work/<вариант>/proxy/.
Оси MLOD: y вверх, нос в -z, +x — левый борт (водитель).
"""
import os
import shutil
import sys

sys.path.insert(0, r"d:\DayZavr\DayZavr-Edit\.claude\tools\py3d")
import py3d  # noqa: E402
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from er_mlod import is_res, face_bbox, components, prune, copy_faces_to_lod  # noqa: E402

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


def clamp(v, a=0.0, b=1.0):
    return max(a, min(b, v))


def inside(bb, box):
    return all(box[i][0] <= bb[i][0] and bb[i][1] <= box[i][1] for i in range(3))


# ---------------------------------------------------------------- свои пути текстур (мод не зависит от пака ALEX)
ALEX_DATA = os.path.join(os.path.dirname(MOD), "купленные модели", "Interceptor", "Interceptor_byALEX", "data")
OUR_DATA = os.path.join(MOD, "VB_EchoRunner", "data")
SRC_PFX, DST_PFX = "interceptor_byalex\\data\\", "vb_echorunner\\data\\"
PARTS_PFX = "vb_echorunner\\data\\parts\\"
FOREIGN = {                                   # чужие моды из пака ALEX -> наши материалы
    "mod_tuning\\data\\plate\\numer_mil_co.paa": PARTS_PFX + "er_steel_rust_co.paa",
    "mod_tuning\\data\\plate\\numer.rvmat": PARTS_PFX + "er_steel_rust.rvmat",
}
USED = set()


def remap_path(path):
    pl = path.lower()
    if pl.startswith(SRC_PFX):
        USED.add(pl[len(SRC_PFX):])
        return DST_PFX + pl[len(SRC_PFX):]
    return FOREIGN.get(pl, path)


def remap_model(m):
    for lod in m.lods:
        for f in lod.faces:
            f.texture = remap_path(f.texture) if f.texture else f.texture
            f.material = remap_path(f.material) if f.material else f.material


def _alex_file(rel):
    """Файл ALEX без учёта регистра (Body_nohq.paa и т. п.)."""
    d, name = os.path.split(os.path.join(ALEX_DATA, rel))
    for f in os.listdir(d):
        if f.lower() == name.lower():
            return os.path.join(d, f)
    return None


def ensure_data():
    """Копирует использованные текстуры ALEX под свои пути (только если файла ещё нет — перекрашенные
    этапом 5 не затираются); бинарные rvmat ALEX переписывает текстом с нашими путями стадий."""
    sys.path.insert(0, r"d:\DayZavr\DayZavr-Edit\.claude\tools\dayz-render")
    import rvmat_read
    os.makedirs(OUR_DATA, exist_ok=True)
    todo = set(USED)
    for rel in sorted(USED):
        if rel.endswith(".rvmat"):
            src = _alex_file(rel)
            rv = rvmat_read.read_rvmat(src)
            lines = []
            for k in ("ambient", "diffuse", "forcedDiffuse", "emmisive", "specular"):
                if k in rv:
                    lines.append("%s[] = {%s};" % (k, ",".join("%g" % x for x in rv[k])))
            lines.append("specularPower = %g;" % rv.get("specularPower", 40))
            lines.append('PixelShaderID = "%s";' % rv.get("PixelShaderID", "Super"))
            lines.append('VertexShaderID = "%s";' % rv.get("VertexShaderID", "Super"))
            for st in sorted((k for k in rv if k.startswith("Stage")), key=lambda k: int(k[5:])):
                t = rv[st].get("texture", "")
                if t.lower().startswith(SRC_PFX):
                    todo.add(t.lower()[len(SRC_PFX):])
                    t = DST_PFX + t.lower()[len(SRC_PFX):]
                lines.append('class %s\n{\n\ttexture = "%s";\n\tuvSource = "%s";\n\tclass uvTransform\n\t{\n'
                             '\t\taside[] = {1,0,0};\n\t\tup[] = {0,1,0};\n\t\tdir[] = {0,0,1};\n\t\tpos[] = {0,0,0};\n'
                             '\t};\n};' % (st, t, rv[st].get("uvSource", "tex")))
            dst = os.path.join(OUR_DATA, rel)
            os.makedirs(os.path.dirname(dst), exist_ok=True)
            with open(dst, "w", encoding="ascii", newline="\r\n") as f:
                f.write("\n".join(lines) + "\n")
    copied = 0
    for rel in sorted(todo):
        if rel.endswith(".rvmat"):
            continue
        dst = os.path.join(OUR_DATA, rel)
        if os.path.exists(dst):
            continue
        src = _alex_file(rel)
        if src:
            os.makedirs(os.path.dirname(dst), exist_ok=True)
            shutil.copy2(src, dst)
            copied += 1
    print("  данные: путей ALEX %d, скопировано файлов %d -> %s" % (len(todo), copied, OUR_DATA))


def cut_tanks(m):
    tank_lods = []
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
        for r in (1.0, 2.0, 3.0, 4.0):
            if is_res(lod, r):
                tank_lods.append(copy_faces_to_lod(lod, rm, r))
        print("  баки: LOD %g — убрано граней %d" % (lod.resolution, len(rm)))
        prune(lod, rm)
    return tank_lods


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
    remap_model(m)
    tank = cut_tanks(m)
    lift(m)
    os.makedirs(os.path.join(OUTDIR, "proxy"), exist_ok=True)
    m.save(os.path.join(OUTDIR, "interceptor.p3d"))
    if tank:
        t = py3d.P3D()
        t.lods.extend(tank)
        t.save(os.path.join(OUTDIR, "tank_alex.p3d"))
    for n in ("interceptor_doors_driver", "interceptor_doors_codriver", "interceptor_doors_hood"):
        with open(os.path.join(BASE, "proxy", n + ".p3d"), "rb") as f:
            d = py3d.P3D(f)
        remap_model(d)
        d.save(os.path.join(OUTDIR, "proxy", n + ".p3d"))
    ensure_data()
    print("OK", OUTDIR)


main()
