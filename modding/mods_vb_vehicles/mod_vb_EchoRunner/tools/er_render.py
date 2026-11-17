# -*- coding: utf-8 -*-
r"""Рендер машины из MLOD: кузов + детали-прокси (колёса, двери, капот), несколько видов (Blender, Cycles).

  blender.exe -b --factory-startup --python er_render.py -- <car.p3d> <папка вывода>
      --proxydir=<папка с MLOD деталей> --root=<корень текстур>[;<ещё>]
      [--views=f34,r34,side,side2,front,rear,top] [--size=1400] [--samples=48] [--lod=1]
      [--extra=<доп.p3d>[;...]]  (детали в координатах машины, без прокси: черновики навесных)

Оси: MLOD DayZ (x, y вверх, z; нос в -z) -> Blender (x, z, y): нос в -Y, верх +Z, водитель (+x) = левый борт.
Порядок вершин MLOD (векторное произведение внутрь) после отражения осей смотрит наружу — порядок не меняем,
нормали MLOD меняют знак. UV: v -> 1 - v.
Прокси: py3d.get_proxies() даёт «сырую» рамку (x, y, z); строки матрицы ODOL = (-x, z, y)
(сверено конвертером odol2mlod), точка детали c -> anchor + c0*(-x) + c1*z + c2*y.
"""
import math
import os
import sys

import bpy
import numpy as np

TOOLS = r"d:\DayZavr\DayZavr-Edit\.claude\tools"
sys.path.insert(0, os.path.join(TOOLS, "py3d"))
sys.path.insert(0, os.path.join(TOOLS, "dayz-render"))
import py3d  # noqa: E402
import rvmat_read  # noqa: E402
import subprocess  # noqa: E402

BS = chr(92)
IMAGE_TO_PAA = r"D:\SteamLibrary\steamapps\common\DayZ Tools\Bin\ImageToPAA\ImageToPAA.exe"
CACHE = os.path.join(TOOLS, "dayz-render", "texcache")

args = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
opts = {a.split("=", 1)[0][2:]: (a.split("=", 1)[1] if "=" in a else True) for a in args if a.startswith("--")}
pos = [a for a in args if not a.startswith("--")]
MODEL, OUTDIR = pos[0], pos[1]
os.makedirs(OUTDIR, exist_ok=True)
ROOTS = [r for r in str(opts.get("root", "")).split(";") if r] + ["P:" + BS]
PROXYDIR = opts.get("proxydir", "")
SIZE = int(opts.get("size", 1400))
SAMPLES = int(opts.get("samples", 48))
LODRES = float(opts.get("lod", 1))
VIEWS = str(opts.get("views", "f34,r34,side,side2,front,rear,top")).split(",")
MAXEDGE = float(opts.get("maxedge", 1.5))


def resolve(vpath):
    vp = vpath.replace("/", BS).lstrip(BS)
    for r in ROOTS:
        p = os.path.join(r, vp)
        if os.path.exists(p):
            return p
    return None


def paa_png(vpath):
    src = resolve(vpath)
    if not src:
        print("WARN нет текстуры", vpath)
        return None
    os.makedirs(CACHE, exist_ok=True)
    dst = os.path.join(CACHE, vpath.lower().replace(BS, "__").replace("/", "__").replace(":", "") + ".tga")
    if not os.path.exists(dst) or os.path.getmtime(dst) < os.path.getmtime(src):
        subprocess.run([IMAGE_TO_PAA, src, dst], capture_output=True)
    return dst if os.path.exists(dst) else None


def proc_color(s):
    try:
        inside = s[s.index("color(") + 6:s.rindex(")")]
        v = [float(x) for x in inside.split(",")[:4]]
        return tuple(v + [1.0] * (4 - len(v)))
    except Exception:
        return None


_img = {}


def load_img(vpath, noncolor=False):
    key = (vpath.lower(), noncolor)
    if key not in _img:
        p = paa_png(vpath)
        im = bpy.data.images.load(p) if p else None
        if im and noncolor:
            im.colorspace_settings.name = "Non-Color"
        _img[key] = im
    return _img[key]


_mats = {}


def material(tex, rvmat):
    key = (tex.lower(), (rvmat or "").lower())
    if key in _mats:
        return _mats[key]
    mat = bpy.data.materials.new(os.path.basename(tex.replace(BS, "/")) + "|" + os.path.basename((rvmat or "").replace(BS, "/")))
    mat.use_nodes = True
    nt = mat.node_tree
    Nn, L = nt.nodes, nt.links
    bsdf = Nn["Principled BSDF"]
    uvn = Nn.new("ShaderNodeUVMap")
    uvn.uv_map = "UV"

    def tnode(im):
        t = Nn.new("ShaderNodeTexImage")
        t.image = im
        L.new(uvn.outputs["UV"], t.inputs["Vector"])
        return t

    rv = {}
    if rvmat:
        rp = resolve(rvmat)
        if rp:
            try:
                rv = rvmat_read.read_rvmat(rp)
            except Exception as e:
                print("WARN rvmat", rvmat, e)
    stages = {k: v.get("texture", "") for k, v in rv.items() if isinstance(v, dict) and k.startswith("Stage")}
    alpha = False
    if tex.startswith("#"):
        bsdf.inputs["Base Color"].default_value = proc_color(tex) or (0.5, 0.5, 0.5, 1)
    elif tex:
        im = load_img(tex)
        if im:
            t = tnode(im)
            L.new(t.outputs["Color"], bsdf.inputs["Base Color"])
            if "_ca." in tex.lower():
                L.new(t.outputs["Alpha"], bsdf.inputs["Alpha"])
                alpha = True
        else:
            bsdf.inputs["Base Color"].default_value = (0.5, 0.5, 0.5, 1)
    else:
        d = rv.get("diffuse") or [0.5, 0.5, 0.5, 1]
        bsdf.inputs["Base Color"].default_value = tuple(list(d[:3]) + [1])
    nohq = next((v for k, v in sorted(stages.items()) if "_nohq" in v.lower()), None)
    if nohq and not nohq.startswith("#"):
        im = load_img(nohq, True)
        if im:
            t = tnode(im)
            sep = Nn.new("ShaderNodeSeparateColor")
            L.new(t.outputs["Color"], sep.inputs["Color"])
            inv = Nn.new("ShaderNodeMath")
            inv.operation = "SUBTRACT"
            inv.inputs[0].default_value = 1.0
            L.new(sep.outputs["Green"], inv.inputs[1])
            comb = Nn.new("ShaderNodeCombineColor")
            L.new(sep.outputs["Red"], comb.inputs["Red"])
            L.new(inv.outputs["Value"], comb.inputs["Green"])
            L.new(sep.outputs["Blue"], comb.inputs["Blue"])
            nm = Nn.new("ShaderNodeNormalMap")
            nm.uv_map = "UV"
            L.new(comb.outputs["Color"], nm.inputs["Color"])
            L.new(nm.outputs["Normal"], bsdf.inputs["Normal"])
    smdi = next((v for v in stages.values() if "_smdi" in v.lower()), None)
    if smdi and not smdi.startswith("#") and load_img(smdi, True):
        t = tnode(load_img(smdi, True))
        sps = Nn.new("ShaderNodeSeparateColor")
        L.new(t.outputs["Color"], sps.inputs["Color"])
        rough = Nn.new("ShaderNodeMapRange")
        rough.inputs["To Min"].default_value = 0.85
        rough.inputs["To Max"].default_value = 0.12
        L.new(sps.outputs["Blue"], rough.inputs["Value"])
        L.new(rough.outputs["Result"], bsdf.inputs["Roughness"])
        L.new(sps.outputs["Green"], bsdf.inputs["Specular IOR Level"])
    else:
        bsdf.inputs["Roughness"].default_value = 0.6
        bsdf.inputs["Specular IOR Level"].default_value = 0.3
    em = rv.get("emmisive") or [0, 0, 0, 1]
    if max(em[:3]) > 0.001:
        bsdf.inputs["Emission Color"].default_value = (min(1, em[0]), min(1, em[1]), min(1, em[2]), 1)
        bsdf.inputs["Emission Strength"].default_value = max(1.0, max(em[:3]))
    if alpha:
        try:
            mat.surface_render_method = "BLENDED"
        except Exception:
            pass
    _mats[key] = mat
    return mat


def load_p3d(path):
    with open(path, "rb") as f:
        return py3d.P3D(f)


def lod_of(m, res):
    best = min(m.lods, key=lambda l: abs(l.resolution - res) / max(res, 1e-9))
    return best


def mesh_from_lod(lod, name, xform=None):
    """xform: (anchor, A0, A1, A2) — c -> anchor + c0*A0 + c1*A1 + c2*A2 (DayZ coords)."""
    proxy_faces = set()
    for sname, sel in lod.selections.items():
        if sname.startswith("proxy:"):
            proxy_faces.update(id(f) for f in sel.faces)
    P = np.array([p.coords for p in lod.points], dtype=np.float64) if lod.points else np.zeros((0, 3))
    Nrm = np.array(lod.facenormals, dtype=np.float64) if lod.facenormals else np.zeros((0, 3))
    if xform is not None:
        a, A0, A1, A2 = (np.array(v, dtype=np.float64) for v in xform)
        M = np.stack([A0, A1, A2])          # строки — оси детали в координатах машины
        P = a + P @ M
        if len(Nrm):
            Nrm = Nrm @ M
    PB = P[:, [0, 2, 1]]
    NB = -Nrm[:, [0, 2, 1]] if len(Nrm) else Nrm
    faces, uvs, nrms, keys, mats = [], [], [], [], {}
    for fa in lod.faces:
        if id(fa) in proxy_faces:
            continue
        idx = [v.point_index for v in fa.vertices]
        if len(set(idx)) != len(idx):
            continue
        if MAXEDGE and max(np.linalg.norm(P[idx[k]] - P[idx[(k + 1) % len(idx)]]) for k in range(len(idx))) > MAXEDGE:
            continue
        faces.append(idx)
        for v in fa.vertices:
            uvs.extend([v.uv[0], 1.0 - v.uv[1]])
            nrms.append(NB[v.normal_index] if len(NB) else (0, 0, 1))
        k = (fa.texture or "", fa.material or "")
        if k not in mats:
            mats[k] = len(mats)
        keys.append(mats[k])
    me = bpy.data.meshes.new(name)
    me.from_pydata(PB.tolist(), [], faces)
    uvl = me.uv_layers.new(name="UV")
    uvl.data.foreach_set("uv", uvs)
    for k in sorted(mats, key=lambda k: mats[k]):
        me.materials.append(material(*k))
    me.polygons.foreach_set("material_index", keys)
    me.polygons.foreach_set("use_smooth", [True] * len(faces))
    try:
        nn = np.array(nrms, dtype=np.float64)
        nn /= np.maximum(np.linalg.norm(nn, axis=1, keepdims=True), 1e-9)
        if not me.validate(verbose=False) and len(me.loops) == len(nn):
            me.normals_split_custom_set(nn.tolist())
    except Exception as e:
        print("WARN normals", name, e)
    me.update()
    ob = bpy.data.objects.new(name, me)
    bpy.context.scene.collection.objects.link(ob)
    return ob


# ---------------------------------------------------------------- сцена
bpy.ops.wm.read_factory_settings(use_empty=True)
scene = bpy.context.scene
scene.render.engine = "CYCLES"
gpu = False
try:
    cp = bpy.context.preferences.addons["cycles"].preferences
    for dt in ("OPTIX", "CUDA"):
        try:
            cp.compute_device_type = dt
            cp.get_devices()
            if any(d.type == dt for d in cp.devices):
                for d in cp.devices:
                    d.use = d.type == dt
                gpu = True
                break
        except Exception:
            pass
except Exception:
    pass
scene.cycles.device = "GPU" if gpu else "CPU"
scene.cycles.samples = SAMPLES
scene.cycles.use_denoising = True
scene.render.film_transparent = True
scene.render.image_settings.file_format = "PNG"
scene.render.image_settings.color_mode = "RGBA"
for vt in ("AgX", "Filmic"):
    try:
        scene.view_settings.view_transform = vt
        break
    except TypeError:
        pass

car = load_p3d(MODEL)
lod = lod_of(car, LODRES)
objs = [mesh_from_lod(lod, "car")]
print("CAR LOD", lod.resolution, "faces", len(lod.faces))
for pr in lod.get_proxies():
    path = pr["path"] if "path" in pr else pr["name"][6:].rsplit(".", 1)[0]
    base = os.path.basename(path.replace(BS, "/")).lower()
    src = os.path.join(PROXYDIR, base + ".p3d") if PROXYDIR else ""
    if not (src and os.path.exists(src)):
        print("PROXY skip", path)
        continue
    x, y, z = pr["frame"]
    xf = (pr["anchor"], tuple(-c for c in x), z, y)
    sub = lod_of(load_p3d(src), LODRES)
    objs.append(mesh_from_lod(sub, base, xf))
    print("PROXY", base, [round(c, 3) for c in pr["anchor"]])
for ex in [e for e in str(opts.get("extra", "")).split(";") if e]:
    objs.append(mesh_from_lod(lod_of(load_p3d(ex), LODRES), os.path.basename(ex)))

def used_verts(o):
    used = sorted({vi for p in o.data.polygons for vi in p.vertices})
    return np.array([tuple(o.data.vertices[i].co) for i in used]) if used else np.zeros((0, 3))


allv = np.concatenate([used_verts(o) for o in objs])
lo, hi = allv.min(0), allv.max(0)
center = (lo + hi) / 2
dims = hi - lo
print("DIMS (Blender x,y,z)", [round(float(d), 3) for d in dims], "low z", round(float(lo[2]), 3))

# земля — ловец теней
bpy.ops.mesh.primitive_plane_add(size=40, location=(center[0], center[1], float(lo[2]) + 0.002))
ground = bpy.context.active_object
ground.is_shadow_catcher = True

world = bpy.data.worlds.new("w")
world.use_nodes = True
world.node_tree.nodes["Background"].inputs["Color"].default_value = (0.42, 0.44, 0.47, 1)
world.node_tree.nodes["Background"].inputs["Strength"].default_value = 0.9
scene.world = world
sun = bpy.data.objects.new("sun", bpy.data.lights.new("sun", "SUN"))
sun.data.energy = 3.2
sun.data.angle = math.radians(8)
sun.rotation_euler = (math.radians(40), math.radians(-25), math.radians(-35))
scene.collection.objects.link(sun)

tgt = bpy.data.objects.new("target", None)
tgt.location = tuple(center)
scene.collection.objects.link(tgt)
cam_d = bpy.data.cameras.new("cam")
cam = bpy.data.objects.new("cam", cam_d)
scene.collection.objects.link(cam)
scene.camera = cam
tc = cam.constraints.new("TRACK_TO")
tc.target = tgt
tc.track_axis = "TRACK_NEGATIVE_Z"
tc.up_axis = "UP_Y"

VIEWDEF = {   # направление на камеру (Blender), орто?
    "f34": ((0.80, -1.0, 0.38), False),
    "r34": ((-0.80, 1.0, 0.38), False),
    "side": ((1.0, 0.0, 0.0), True),      # левый борт (водитель)
    "side2": ((-1.0, 0.0, 0.0), True),    # правый борт
    "front": ((0.0, -1.0, 0.0), True),
    "rear": ((0.0, 1.0, 0.0), True),
    "top": ((0.0, 0.0001, 1.0), True),
}
R = float(np.linalg.norm(dims)) / 2
for v in VIEWS:
    d, ortho = VIEWDEF[v]
    d = np.array(d) / np.linalg.norm(d)
    if ortho:
        cam_d.type = "ORTHO"
        # ширина кадра по видимым осям
        if v in ("side", "side2"):
            w, h = dims[1], dims[2]
        elif v in ("front", "rear"):
            w, h = dims[0], dims[2]
        else:
            w, h = dims[0], dims[1]
        cam_d.ortho_scale = float(max(w, h)) * 1.08
        dist = R * 4
        scene.render.resolution_x = SIZE
        scene.render.resolution_y = int(SIZE * min(1.0, max(0.35, float(h / w) * 1.08 + 0.02))) if w > h else SIZE
    else:
        cam_d.type = "PERSP"
        cam_d.lens = 50
        cam_d.sensor_width = 36
        dist = R / math.sin(math.atan(18.0 / 50.0)) * 1.05
        scene.render.resolution_x = SIZE
        scene.render.resolution_y = int(SIZE * 0.66)
    cam_d.clip_start = 0.05
    cam_d.clip_end = dist * 10
    cam.location = tuple(center + d * dist)
    scene.render.filepath = os.path.join(OUTDIR, v + ".png")
    bpy.ops.render.render(write_still=True)
    print("RENDERED", scene.render.filepath)
