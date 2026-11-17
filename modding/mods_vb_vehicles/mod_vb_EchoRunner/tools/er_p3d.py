# -*- coding: utf-8 -*-
r"""Echo Runner, этап 4: сборка моделей для DayZ (MLOD .p3d через py3d) и model.cfg.

  G:\SteamLibrary\steamapps\common\Blender\blender.exe -b --factory-startup --python er_p3d.py

Пишет в mod_vb_EchoRunner\VB_EchoRunner\:
  echorunner.p3d                 кузов: LOD 1/2/3/4, ViewPilot 1100, Geometry, ViewGeometry, FireGeometry, Memory
  proxy\er_wheel.p3d             колесо (внешняя сторона +X), предмет
  proxy\er_door_driver.p3d       дверь водителя (+X, левый борт), предмет
  proxy\er_door_codriver.p3d     дверь пассажира (−X), предмет
  proxy\er_hood.p3d              передняя крышка (багажник)
  proxy\er_trunk.p3d             задняя крышка (мотор)
  model.cfg

Оси: всё строится в осях Blender (нос −Y, Z вверх, ЛЕВЫЙ борт +X), в конце py3d.blender_to_dayz()
(нос −Z, +X левый — как у ванили). Модель кузова поднята на Z_LIFT: ступица чуть выше радиуса колеса,
машина при спавне опускается на подвеске (у ванили так же).
Имена точек и выделений сверены с ванильной Саркой (справка/sedan_02_lods.txt) и scripts carscript.c.
"""
import math
import os
import sys

import bmesh
import bpy
from mathutils import Matrix, Vector

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, r"D:\DayZavr\DayZavr-Edit\.claude\tools\py3d")
import er_model as M  # noqa: E402
import py3d  # noqa: E402

assert py3d.IS_DAYZ_FORK
MOD = os.path.dirname(HERE)
ADDON = os.path.join(MOD, "VB_EchoRunner")
PFX = "VB_EchoRunner"
Z_LIFT = 0.025
MASS_CAR = 1230.0

# материал модели -> (текстура, rvmat)
TEXMAP = {"body": "body", "steel_in": "inner", "pipe": "metal_rust", "steel": "metal_rust", "copper": "copper",
          "hose": "rubber", "tire": "rubber", "clamp": "chrome", "chrome": "chrome", "black": "plastic",
          "dash": "plastic", "rim": "rim", "engine": "engine", "seat": "seat", "floor": "floor", "net": "net",
          "lamp": "lamp", "tail": "tail", "amber": "amber", "neon": "neon",
          "strap": "strap", "screen": "screen", "crate": "crate", "redpaint": "redpaint"}
GLASS = ("dz\\vehicles\\wheeled\\civiliansedan\\data\\sedan_glass_ca.paa",
         "dz\\vehicles\\wheeled\\sedan_02\\data\\glass_i.rvmat")


def tex_of(mname):
    if mname in ("glass", "glass_ws"):
        return GLASS
    key = TEXMAP.get(mname, "plastic")
    suf = "_ca" if key == "net" else "_co"
    return ("%s\\data\\er_%s%s.paa" % (PFX, key, suf), "%s\\data\\er_%s.rvmat" % (PFX, key))


# ------------------------------------------------------------------ построение LOD
class Builder:
    """Обёртка над py3d.LOD: точки, нормали (с дедупликацией), грани, выделения по индексам."""

    def __init__(self, res):
        self.lod = py3d.LOD()
        self.lod.resolution = float(res)
        self.nmap = {}
        self.sel = {}                                    # имя -> (set граней, set точек)

    def normal(self, n):
        key = (round(n[0], 4), round(n[1], 4), round(n[2], 4))
        i = self.nmap.get(key)
        if i is None:
            i = len(self.lod.facenormals)
            self.lod.facenormals.append(key)
            self.nmap[key] = i
        return i

    def point(self, co, flags=0, mass=None):
        p = py3d.Point()
        p.coords = (float(co[0]), float(co[1]), float(co[2]))
        p.flags = flags
        p.mass = mass
        self.lod.points.append(p)
        return len(self.lod.points) - 1

    def face(self, pidx, nidx, uvs=None, tex="", mat="", flags=0):
        f = py3d.Face(self.lod.points, self.lod.facenormals)
        f.flags = flags
        f.texture = tex
        f.material = mat
        for k, pi in enumerate(pidx):
            vx = py3d.Vertex(self.lod.points, self.lod.facenormals)
            vx.point_index = pi
            vx.normal_index = nidx[k] if isinstance(nidx, (list, tuple)) else nidx
            vx.uv = uvs[k] if uvs else (0.0, 0.0)
            f.vertices.append(vx)
        self.lod.faces.append(f)
        return len(self.lod.faces) - 1

    def add_sel(self, name, faces=(), points=()):
        fs, ps = self.sel.setdefault(name, (set(), set()))
        fs.update(faces)
        ps.update(points)
        for fi in faces:
            ps.update(v.point_index for v in self.lod.faces[fi].vertices)

    def mesh(self, obj, offset=Vector((0, 0, 0)), sel_fn=None, mats=None):
        """Видимая сетка объекта Blender: триангуляция n-угольников, нормали уголков, UV. sel_fn(poly, mname, c)
        -> список имён выделений для грани."""
        bm = bmesh.new()
        bm.from_mesh(obj.data)
        big = [f for f in bm.faces if len(f.verts) > 4]
        if big:
            bmesh.ops.triangulate(bm, faces=big)
        tmp = bpy.data.meshes.new("tmp_" + obj.name)
        bm.to_mesh(tmp)
        bm.free()
        tmp.shade_smooth()
        tmp.set_sharp_from_angle(angle=math.radians(35))
        cn = tmp.corner_normals
        uvl = tmp.uv_layers.active.data if tmp.uv_layers.active else None
        names = mats or [m.name for m in obj.data.materials]
        base = len(self.lod.points)
        for v in tmp.vertices:
            self.point(v.co + offset)
        out = {}
        for poly in tmp.polygons:
            mname = names[poly.material_index] if poly.material_index < len(names) else "plastic"
            tex, rv = tex_of(mname)
            pidx, nidx, uvs = [], [], []
            for li in poly.loop_indices:
                pidx.append(base + tmp.loops[li].vertex_index)
                nidx.append(self.normal(cn[li].vector))
                u, v = (uvl[li].uv if uvl else (0.0, 0.0))
                uvs.append((float(u), 1.0 - float(v)))
            fi = self.face(pidx, nidx, uvs, tex, rv)
            if sel_fn:
                for s in sel_fn(poly, mname, poly.center + offset):
                    out.setdefault(s, []).append(fi)
        bpy.data.meshes.remove(tmp)
        for s, fl in out.items():
            self.add_sel(s, faces=fl)
        return out

    def hull(self, pts, comp, extra=(), mass=0.0, flags=0):
        """Выпуклый компонент (для коллизий): выпуклая оболочка точек, нормали наружу."""
        bm = bmesh.new()
        vs = [bm.verts.new(Vector(p)) for p in pts]
        bmesh.ops.convex_hull(bm, input=vs)
        for v in [v for v in bm.verts if not v.link_faces]:
            bm.verts.remove(v)
        bmesh.ops.recalc_face_normals(bm, faces=bm.faces)
        bm.verts.index_update()
        per = mass / max(1, len(bm.verts)) if mass else None
        base = len(self.lod.points)
        for v in bm.verts:
            self.point(v.co, flags, per)
        fis = []
        for f in bm.faces:
            ni = self.normal(f.normal)
            fis.append(self.face([base + v.index for v in f.verts], ni))
        bm.free()
        for name in (comp,) + tuple(extra):
            self.add_sel(name, faces=fis)
        return fis

    def proxy(self, path, index, origin, rotation=None, bones=()):
        name = self.lod.add_proxy(path, index, tuple(origin), rotation, 0.001, "raw")
        fi = len(self.lod.faces) - 1
        for b in bones:
            self.add_sel(b, faces=[fi])
        return name

    def mem(self, name, *pts):
        idx = [self.point(p) for p in pts]
        self.add_sel(name, points=idx)

    def finish(self, props=None):
        for name, (fs, ps) in self.sel.items():
            self.lod.set_selection(name, face_idx=sorted(fs), point_idx=sorted(ps))
        for k, v in (props or {}).items():
            self.lod.properties[k] = str(v)
        return self.lod


def box_pts(c, h):
    return [(c[0] + sx * h[0], c[1] + sy * h[1], c[2] + sz * h[2]) for sx in (-1, 1) for sy in (-1, 1) for sz in (-1, 1)]


def decimated(obj, ratio):
    o = obj.copy()
    o.data = obj.data.copy()
    bpy.context.scene.collection.objects.link(o)
    mod = o.modifiers.new("dec", "DECIMATE")
    mod.ratio = ratio
    dg = bpy.context.evaluated_depsgraph_get()
    me = bpy.data.meshes.new_from_object(o.evaluated_get(dg))
    bpy.data.objects.remove(o)
    bm = bmesh.new()
    bm.from_mesh(me)
    loose = [v for v in bm.verts if not v.link_faces]
    if loose:
        bmesh.ops.delete(bm, geom=loose, context="VERTS")
    bm.to_mesh(me)
    bm.free()
    out = bpy.data.objects.new(obj.name + "_d", me)
    bpy.context.scene.collection.objects.link(out)
    return out


def lifted(v):
    return Vector((v[0], v[1], v[2] + Z_LIFT))


# ------------------------------------------------------------------ модель и данные
bpy.ops.wm.read_factory_settings(use_empty=True)
objs, pivots = M.build_all()
LIFT = Vector((0, 0, Z_LIFT))

WHEELS = {   # имя DayZ -> (центр ступицы в Blender до подъёма, сторона: +1 левый борт)
    "1_1": (Vector((M.TRACK / 2, M.Y_FA, M.HUB_Z)), 1), "2_1": (Vector((-M.TRACK / 2, M.Y_FA, M.HUB_Z)), -1),
    "1_2": (Vector((M.TRACK / 2, M.Y_RA, M.HUB_Z)), 1), "2_2": (Vector((-M.TRACK / 2, M.Y_RA, M.HUB_Z)), -1),
}
YAW180 = ((-1.0, 0.0, 0.0), (0.0, -1.0, 0.0), (0.0, 0.0, 1.0))


def center_of(obj):
    xs = [v.co.x for v in obj.data.vertices]
    ys = [v.co.y for v in obj.data.vertices]
    zs = [v.co.z for v in obj.data.vertices]
    return Vector(((min(xs) + max(xs)) / 2, (min(ys) + max(ys)) / 2, (min(zs) + max(zs)) / 2))


PARTS = {  # предмет: (объект, имя файла, выделение-действие, кость в кузове, масса, сторона для invview)
    "door_driver": (objs["door_r"], "er_door_driver", "doors_driver", "doors_driver", 25.0, Vector((1, 0, 0.3))),
    "door_codriver": (objs["door_l"], "er_door_codriver", "doors_codriver", "doors_codriver", 25.0,
                      Vector((-1, 0, 0.3))),
    "hood": (objs["hood"], "er_hood", "doors_hood", "doors_hood", 18.0, Vector((0, -0.4, 1))),
    "trunk": (objs["engine_lid"], "er_trunk", "doors_trunk", "doors_trunk", 22.0, Vector((0, 0.4, 1))),
}
CENTERS = {k: center_of(v[0]) for k, v in PARTS.items()}
# Начало координат дверей-«крыльев» — низко у борта, а не в центре двери: игра проверяет посадку по расстоянию
# от игрока до выделения двери (треугольник прокси) с порогом 1 м (CanReachSeatFromDoors). Центр «крыла»
# (x 0.6, высота 0.74) давал ~1.1 м — «недосягаемо» (стенд 2026-11-17). У Сарки прокси двери у борта.
for _k, _sx in (("door_driver", 1), ("door_codriver", -1)):
    CENTERS[_k] = Vector((_sx * 0.90, CENTERS[_k].y, 0.50))
WHEEL_OBJ = objs["wheel_f_r"]                    # колесо с внешней стороной +X (side=+1)
WHEEL_C = WHEELS["1_1"][0]
CREW = {"driver": ("\\dz\\vehicles\\wheeled\\proxies\\crew_driver", 1, "crewdriver"),
        "codriver": ("\\dz\\vehicles\\wheeled\\proxies\\crew_cargo", 1, "crewcodriver")}


def sel_body(poly, mname, c):
    s = []
    if mname == "body":
        s.append("camo")
    if mname == "lamp":
        s.append("light_1_1" if c.x > 0 else "light_2_1")
    if mname in ("tail", "amber"):
        if c.x > 0:
            s.append("light_brake_1_2" if c.x > 0.55 else "light_1_2")
        else:
            s.append("light_brake_2_2" if mname == "tail" else "light_2_2")
    if mname == "chrome" and -0.42 < c.y < -0.33 and 0.80 < c.z < 0.95 and c.x > 0.2:
        s.append("light_dashboard")
    if mname in ("glass", "glass_ws"):
        s.append("glass")
    if mname == "neon":
        s.append("neon")
    return s


def add_proxies(b):
    for n, (h, side) in WHEELS.items():
        idx = {"1_1": 1, "2_1": 2, "1_2": 3, "2_2": 4}[n]
        b.proxy("\\%s\\proxy\\er_wheel" % PFX, idx, lifted(h), None if side > 0 else YAW180, bones=["wheel_" + n])
    for k, (obj, fname, act, bone, mass, inv) in PARTS.items():
        b.proxy("\\%s\\proxy\\%s" % (PFX, fname), 1, lifted(CENTERS[k]), None, bones=[bone])
    for k, (path, idx, bone) in CREW.items():
        b.proxy(path, idx, lifted(M.SEAT[k]), M.crew_rotation(), bones=[bone])


def visual_lod(res, src, with_proxies=True):
    b = Builder(res)
    b.mesh(src["body"], LIFT, sel_body)
    b.mesh(src["pipes"], LIFT, lambda p, m, c: ["pipes"] + (["neon"] if m == "neon" else []))
    b.mesh(src["bumpers"], LIFT, lambda p, m, c: ["bumper_front" if c.y < 0 else "bumper_rear"])
    b.mesh(src["steering"], LIFT, lambda p, m, c: ["drivewheel"])
    if with_proxies:
        add_proxies(b)
    return b.finish()


# --- компоненты коллизии (оси Blender до подъёма)
FRONT = [(sx * 0.78, -2.12, z) for sx in (-1, 1) for z in (0.30, 0.70)] + \
        [(sx * 0.88, -0.70, z) for sx in (-1, 1) for z in (0.20, 0.85)]
MID = box_pts((0, 0.04, 0.47), (0.90, 0.74, 0.36))
REAR = [(sx * 0.88, 0.78, z) for sx in (-1, 1) for z in (0.20, 0.92)] + \
       [(sx * 0.78, 2.12, z) for sx in (-1, 1) for z in (0.30, 0.92)]
CABIN = [(sx * 0.86, -0.62, 0.88) for sx in (-1, 1)] + [(sx * 0.86, 1.68, 0.94) for sx in (-1, 1)] + \
        [(sx * 0.68, -0.20, 1.13 + M.ROOF_UP) for sx in (-1, 1)] + [(sx * 0.68, 0.80, 1.13 + M.ROOF_UP) for sx in (-1, 1)]


def L(pts):
    return [lifted(p) for p in pts]


def geometry_lod():
    b = Builder(1e13)
    n = 0
    for pts, share in ((FRONT, 0.24), (MID, 0.40), (REAR, 0.26), (CABIN, 0.06)):
        n += 1
        b.hull(L(pts), "Component%02d" % n, mass=MASS_CAR * share)
    for wn, (h, side) in WHEELS.items():
        n += 1
        c = h + Vector((-side * 0.03, 0, 0))
        b.hull(L(box_pts(c, (0.10, 0.15, 0.15))), "Component%02d" % n, ["wheel_%s_damper_land" % wn],
               mass=MASS_CAR * 0.01)
    return b.finish({"autocenter": "0"})


# View Geometry салона — оболочкой, как у Сарки: пол, пороги, лобовое, гребень крыши между петлями, задняя
# перегородка. Проём двери закрывает только сама дверь (прокси); иначе луч действия (ObjIntersectView)
# упирается в корпус и не доходит до коробки места — «Сесть» не появляется (стенд 2026-11-17).
SX = (-1, 1)
VIEW_CABIN = [
    box_pts((0, 0.04, 0.155), (0.88, 0.74, 0.035)),                                         # пол
    box_pts((0.85, -0.05, 0.25), (0.05, 0.55, 0.06)),                                       # порог левый
    box_pts((-0.85, -0.05, 0.25), (0.05, 0.55, 0.06)),                                      # порог правый
    [(s * 0.84, -0.66, 0.86) for s in SX] + [(s * 0.84, -0.62, 0.82) for s in SX] +
    [(s * 0.68, -0.20, 1.13 + M.ROOF_UP) for s in SX] + [(s * 0.68, -0.16, 1.09 + M.ROOF_UP) for s in SX],  # лобовое
    box_pts((0, 0.36, 1.11 + M.ROOF_UP), (M.DOOR["x_hinge"], 0.44, 0.03)),                  # гребень крыши
    [(s * 0.62, 0.80, 1.13 + M.ROOF_UP) for s in SX] + [(s * 0.86, 0.78, 0.90) for s in SX] +
    [(s * 0.86, 1.68, 0.92) for s in SX] + [(s * 0.86, 1.68, 0.96) for s in SX],            # заднее стекло
]


def collision_lod(res, kind):
    b = Builder(res)
    n = 0
    if kind == "fire":
        zones = {"FRONT": ["dmgzone_front"], "MID": ["dmgzone_chassis"], "REAR": ["dmgzone_back"],
                 "CABIN": ["dmgzone_roof"]}
        for key, pts in (("FRONT", FRONT), ("MID", MID), ("REAR", REAR), ("CABIN", CABIN)):
            n += 1
            b.hull(L(pts), "Component%02d" % n, zones[key])
        n += 1
        b.hull(L(box_pts((0, 1.60, 0.55), (0.35, 0.35, 0.20))), "Component%02d" % n, ["dmgzone_engine"])
        n += 1
        b.hull(L(box_pts((0, -1.30, 0.36), (0.40, 0.30, 0.10))), "Component%02d" % n, ["dmgzone_fueltank"])
    else:
        for pts in [FRONT, REAR] + VIEW_CABIN:
            n += 1
            b.hull(L(pts), "Component%02d" % n)
        for k in ("driver", "codriver"):            # коробка места: сиденье от тоннеля почти до проёма
            h = Vector(M.SEAT[k])
            sgn = 1 if h.x > 0 else -1
            n += 1
            b.hull(L(box_pts((h.x + sgn * 0.06, h.y - 0.15, h.z + 0.05), (0.32, 0.28, 0.20))),
                   "Component%02d" % n, ["seat_" + k])
        n += 1
        b.hull(L(box_pts((-0.97, -1.80, 0.66), (0.06, 0.07, 0.07))), "Component%02d" % n, ["refill"])
    add_proxies(b)
    return b.finish()


def memory_lod():
    b = Builder(1e15)
    for n, (h, side) in WHEELS.items():
        H = lifted(h)
        b.mem("wheel_%s_axis" % n, H - Vector((0.1, 0, 0)), H + Vector((0.1, 0, 0)))
        b.mem("wheel_%s_damper_land" % n, H)
        b.mem("wheel_%s_axis_damper" % n, H, H + Vector((0, 0, 1.0)))     # 1 м: смещение в метрах
        if n.endswith("_1"):
            b.mem("wheel_%s_axis_turn" % n, H, H + Vector((0, 0, 0.3)))
    for k, (obj, fname, act, bone, mass, inv) in PARTS.items():
        p, ax = pivots[{"door_driver": "door_r", "door_codriver": "door_l", "hood": "hood",
                        "trunk": "engine_lid"}[k]]
        p, ax = lifted(p), Vector(ax)
        span = (Vector((0, -0.40, 0)), Vector((0, 0.30, 0))) if ax.y else (Vector((-0.5, 0, 0)), Vector((0.5, 0, 0)))
        b.mem(bone + "_axis", p + span[0], p + span[1])
    st_c, st_ax = pivots["steering"]
    b.mem("drivewheel_axis", lifted(st_c), lifted(Vector(st_c) + Vector(st_ax) * 0.2))
    for k, sx in (("driver", 1), ("coDriver", -1)):
        b.mem("pos_" + k, lifted((sx * 1.55, -0.05, 0.0)))     # коробка «место свободно» 1.30–1.80 мимо кромки
                                                                # открытого «крыла» (x≈1.25)
        b.mem("pos_%s_dir" % k, lifted((sx * 1.00, -0.05, 0.0)))
    b.mem("seat_con_1_1", lifted((0.95, -0.05, 0.55)))
    b.mem("seat_con_2_1", lifted((-0.95, -0.05, 0.55)))
    for name, x in (("light_left", 0.58), ("light_right", -0.58)):
        b.mem(name, lifted((x, -2.17, 0.515)))
        b.mem(name + "_dir", lifted((x, -3.17, 0.47)))
    b.mem("light_reverse", lifted((0.0, 2.18, 0.60)))
    b.mem("ptcExhaust_start", lifted((0.58, 2.18, 0.215)))
    b.mem("ptcExhaust_end", lifted((0.58, 2.32, 0.24)))
    b.mem("ptcEnginePos", lifted((0.0, 1.60, 0.90)))
    b.mem("ptcCoolantPos", lifted((0.0, -1.95, 0.62)))
    b.mem("ptcDustPos", lifted((0.0, 2.25, 0.10)))
    b.mem("drown_engine", lifted((0.0, 1.60, 0.50)))
    zones = {"dmgZone_engine": (0, 1.6, 0.6), "dmgZone_front": (0, -2.0, 0.45), "dmgZone_back": (0, 2.05, 0.6),
             "dmgZone_roof": (0, 0.35, 1.10), "dmgZone_chassis": (0, 0.0, 0.30), "dmgZone_fuelTank": (0, -1.3, 0.36),
             "dmgZone_fender_1_1": (0.85, M.Y_FA, 0.6), "dmgZone_fender_2_1": (-0.85, M.Y_FA, 0.6),
             "dmgZone_fender_1_2": (0.85, M.Y_RA, 0.6), "dmgZone_fender_2_2": (-0.85, M.Y_RA, 0.6)}
    for k, v in zones.items():
        b.mem(k, lifted(v))
    b.mem("refill", lifted((-1.00, -1.80, 0.66)))
    for k, v in M.MOUNTS.items():                              # места под будущие обвесы
        b.mem("mount_" + k, lifted(v))
    return b.finish()


def save(model, path):
    # Уровни по возрастанию разрешения, как у ванили и Object Builder (сама сортировка невидимость деталей
    # не лечила: виноваты были флаги точек View Geometry, см. part_model).
    model.lods.sort(key=lambda l: l.resolution)
    py3d.blender_to_dayz(model)
    os.makedirs(os.path.dirname(path), exist_ok=True)
    errs = [f for f in model.validate() if f.severity == "ERROR"]
    warns = [f for f in model.validate() if f.severity != "ERROR"]
    model.save(path)
    print("SAVED %-28s lods=%d  err=%d warn=%d" % (os.path.basename(path), len(model.lods), len(errs), len(warns)))
    for f in errs + warns[:12]:
        print("   ", f.severity, f.code, "lod", f.lod, f.msg[:160])


# ------------------------------------------------------------------ кузов
src0 = {k: objs[k] for k in ("body", "pipes", "bumpers", "steering")}
car = py3d.P3D()
car.lods = [visual_lod(1.0, src0)]
for res, ratio in ((2.0, 0.5), (3.0, 0.25), (4.0, 0.1)):
    src = {k: decimated(objs[k], ratio) for k in src0}
    car.lods.append(visual_lod(res, src))
car.lods.append(visual_lod(1100.0, src0))                       # вид из салона
car.lods += [geometry_lod(), memory_lod(), collision_lod(6e15, "view"), collision_lod(7e15, "fire")]
tris = sum(len(f.vertices) - 2 for f in car.lods[0].faces)
print("LOD0 tris=%d points=%d normals=%d" % (tris, len(car.lods[0].points), len(car.lods[0].facenormals)))
save(car, os.path.join(ADDON, "echorunner.p3d"))


# ------------------------------------------------------------------ детали-предметы
def part_model(obj, center, act, mass, inv_dir, ratio2=0.4, hull_pts=None):
    m = py3d.P3D()
    off = -center
    lods = []
    b = Builder(1.0)
    b.mesh(obj, off, lambda p, mn, c: (["camo"] if mn == "body" else []) + (["glass"] if mn.startswith("glass") else []))
    lods.append(b.finish())
    d = decimated(obj, ratio2)
    b = Builder(2.0)
    b.mesh(d, off, lambda p, mn, c: ["camo"] if mn == "body" else [], mats=[x.name for x in obj.data.materials])
    lods.append(b.finish())
    pts = hull_pts or [v.co + off for v in obj.data.vertices]
    b = Builder(1e13)
    b.hull(pts, "Component01", mass=mass)
    lods.append(b.finish({"autocenter": "0"}))
    b = Builder(6e15)
    # Флаги точек не ставить: 0x3F = «на поверхности/держать высоту» (ClipLand*). На всей View Geometry
    # детали binarize считал модель приклеенной к земле, и игра её не рисовала (стенд 2026-11-17).
    b.hull(pts, "Component01", [act])
    lods.append(b.finish())
    b = Builder(7e15)
    b.hull(pts, "Component01", [act])
    lods.append(b.finish())
    b = Builder(1e15)
    span = max((max(p[i] for p in pts) - min(p[i] for p in pts)) for i in range(3))
    b.mem("invview", inv_dir.normalized() * max(1.07, span))
    lods.append(b.finish())
    m.lods = lods
    return m


for k, (obj, fname, act, bone, mass, inv) in PARTS.items():
    save(part_model(obj, CENTERS[k], act, mass, inv), os.path.join(ADDON, "proxy", fname + ".p3d"))
CYL = [Vector((x, M.R * math.cos(2 * math.pi * k / 16), M.R * math.sin(2 * math.pi * k / 16)))
       for k in range(16) for x in (-0.14, 0.14)]       # коллизия колеса — простой 16-гранный цилиндр
save(part_model(WHEEL_OBJ, WHEEL_C, "wheel", 20.0, Vector((1, 0, 0.2)), 0.35, CYL),
     os.path.join(ADDON, "proxy", "er_wheel.p3d"))

# ------------------------------------------------------------------ model.cfg
# У каждого колеса одна кость: на неё по порядку вращение, поворот, ход подвески (оси анимаций — неподвижные
# точки памяти, поэтому порядок «вращение -> поворот -> ход» даёт правильную картину).
BONES = ["drivewheel", "doors_driver", "doors_codriver", "doors_hood", "doors_trunk", "bumper_front", "bumper_rear",
         "crewdriver", "crewcodriver", "wheel_1_1", "wheel_2_1", "wheel_1_2", "wheel_2_2"]
SKEL = [(bn, "") for bn in BONES]

SRC_W = {"1_1": "frontleft", "2_1": "frontright", "1_2": "backleft", "2_2": "backright"}
# Знаки углов сверены с рабочей Candy_MustFrank (нос тоже в −Z), 2026-11-17. Поворот по правилу правой руки
# в осях DayZ: R_x(+θ) поднимает точку перед осью (z<0). Из Blender думать нельзя — перевод осей зеркальный.
# Колёса: все оси смотрят в +X (точки памяти «внутрь → наружу» только у левых), вперёд = −θ вокруг +X.
anims = []
for n, side in (("1_1", 1), ("2_1", -1), ("1_2", 1), ("2_2", -1)):
    anims.append(('wheel_%s' % n, 'rotation', 'wheel' + SRC_W[n], 'wheel_' + n, 'wheel_%s_axis' % n,
                  'sourceAddress = loop; minValue = 0; maxValue = 6.2831855; angle0 = 0; angle1 = -6.2831855;'))
    if n.endswith("_1"):
        anims.append(('turn_%s' % n, 'rotation', 'turn' + SRC_W[n], 'wheel_' + n, 'wheel_%s_axis_turn' % n,
                      'minValue = -1.5707964; maxValue = 1.5707964; angle0 = -1.5707964; angle1 = 1.5707964;'))
    anims.append(('damper_%s' % n, 'translation', 'damper' + SRC_W[n], 'wheel_' + n, 'wheel_%s_axis_damper' % n,
                  'minValue = 0; maxValue = 1; offset0 = -0.12; offset1 = 0.12;'))
anims.append(("DrivingWheel", "rotation", "steeringwheel", "drivewheel", "drivewheel_axis",
              "minValue = -1; maxValue = 1; angle0 = -2.9415927; angle1 = 2.9415927;"))   # ось руля к водителю
# Двери-«крылья»: ось +Z у крыши; водительская (+X) поднимается при +θ. Капот: петля сзади, ось +X → +θ;
# задняя крышка: петля спереди, ось +X → −θ. Проверено на стенде: прежние знаки открывали всё внутрь.
for nm, src, sel, ang in (("DoorsDriver", "doorsdriver", "doors_driver", 1.2566371),
                          ("DoorsCoDriver", "doorscodriver", "doors_codriver", -1.2566371),
                          ("DoorsHood", "doorshood", "doors_hood", 0.96),
                          ("DoorsTrunk", "doorstrunk", "doors_trunk", -0.84)):
    anims.append((nm, "rotation", src, sel, sel + "_axis", "minValue = 0; maxValue = 1; angle0 = 0; angle1 = %.7f;"
                  % ang))
for sel in ("bumper_front", "bumper_rear"):
    anims.append(("hide_" + sel, "hide", "hide_" + sel, sel, None, "minValue = 0; maxValue = 1; hideValue = 0.5;"))

SECTIONS = ["camo", "pipes", "neon", "glass", "light_1_1", "light_2_1", "light_1_2", "light_2_2", "light_brake_1_2",
            "light_brake_2_2", "light_reverse_1_2", "light_reverse_2_2", "light_dashboard", "drivewheel",
            "doors_driver", "doors_codriver", "doors_hood", "doors_trunk", "bumper_front", "bumper_rear"]
out = ["// Echo Runner model.cfg, generated by tools/er_p3d.py. Door/wheel angle signs to be checked in game.",
       "class CfgSkeletons", "{", "\tclass Default", "\t{", "\t\tisDiscrete = 1;", "\t\tskeletonInherit = \"\";",
       "\t\tskeletonBones[] = {};", "\t};", "\tclass EchoRunner_skeleton: Default", "\t{", "\t\tskeletonBones[] =",
       "\t\t{"]
out += ["\t\t\t\"%s\", \"%s\"%s" % (a, b, "," if i < len(SKEL) - 1 else "") for i, (a, b) in enumerate(SKEL)]
out += ["\t\t};", "\t};", "};", "class CfgModels", "{", "\tclass Default", "\t{", "\t\tsectionsInherit = \"\";",
        "\t\tsections[] = {};", "\t\tskeletonName = \"\";", "\t};", "\tclass echorunner: Default", "\t{",
        "\t\tskeletonName = \"EchoRunner_skeleton\";",
        "\t\tsections[] = {%s};" % ", ".join('"%s"' % s for s in SECTIONS), "\t\tclass Animations", "\t\t{"]
for nm, typ, src, sel, axis, rest in anims:
    out += ["\t\t\tclass %s" % nm, "\t\t\t{", "\t\t\t\ttype = \"%s\";" % typ, "\t\t\t\tsource = \"%s\";" % src,
            "\t\t\t\tselection = \"%s\";" % sel]
    if axis:
        out.append("\t\t\t\taxis = \"%s\";" % axis)
    out += ["\t\t\t\t" + r.strip() + ";" for r in rest.split(";") if r.strip()]
    out += ["\t\t\t};"]
out += ["\t\t};", "\t};"]
for fname in ["er_wheel"] + [v[1] for v in PARTS.values()]:
    out += ["\tclass %s: Default" % fname, "\t{", "\t\tsections[] = {\"camo\", \"glass\"};", "\t};"]
out += ["};", ""]
with open(os.path.join(ADDON, "model.cfg"), "w", encoding="utf-8") as f:
    f.write("\n".join(out))
print("MODEL.CFG bones=%d anims=%d" % (len(SKEL), len(anims)))
print("DONE")
