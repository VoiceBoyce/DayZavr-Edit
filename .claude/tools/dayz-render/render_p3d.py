# -*- coding: utf-8 -*-
r"""Рендер бинарной модели DayZ (ODOL) в PNG на прозрачном фоне (Blender, Cycles).

Запуск:
  blender.exe -b --factory-startup --python render_p3d.py -- <model.p3d> <выход.png> --root=<папка мода>
      [--size=2000] [--samples=256] [--elev=22] [--azim=-28] [--frame=1.1] [--flat] [--margin=0.04]
--root — папка, от которой отсчитываются пути текстур внутри модели (для EFT — mod_EscapeFromDayZ,
где лежит папка EFT = префикс eft). Ванильные dz\... ищутся на P:\.
Материал на секцию: Stage0 = текстура секции (_co/_ca), из rvmat — Stage1 (_nohq, DirectX → инверсия G),
_smdi (G — блик, B — глянец), emmisive. Если в rvmat нет карт — блик из specular/specularPower.
"""
import math
import os
import subprocess
import sys

import bpy
import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import odol_read  # noqa: E402
import rvmat_read  # noqa: E402

BS = chr(92)
IMAGE_TO_PAA = os.path.join("D:" + BS, "SteamLibrary", "steamapps", "common", "DayZ Tools", "Bin", "ImageToPAA",
                            "ImageToPAA.exe")
CACHE = os.path.join(HERE, "texcache")

args = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
opts = {a.split("=", 1)[0][2:]: (a.split("=", 1)[1] if "=" in a else True) for a in args if a.startswith("--")}
pos = [a for a in args if not a.startswith("--")]
MODEL, OUT = pos[0], pos[1]
ROOTS = [opts.get("root", ""), "P:" + BS]
SIZE = int(opts.get("size", 2000))
SAMPLES = int(opts.get("samples", 256))


PREFIXES = {}                                    # --prefix=vb_ab=D:\...\VB_AnomaliesBosses[;другой=...]
for pair in str(opts.get("prefix", "")).split(";"):
    if "=" in pair:
        k, v = pair.split("=", 1)
        PREFIXES[k.strip().lower().rstrip(BS)] = v.strip()


def resolve(vpath):
    vp = vpath.replace("/", BS).lstrip(BS)
    low = vp.lower()
    for k, v in PREFIXES.items():
        if low.startswith(k + BS):
            p = os.path.join(v, vp[len(k) + 1:])
            if os.path.exists(p):
                return p
    for r in ROOTS:
        if r:
            p = os.path.join(r, vp)
            if os.path.exists(p):
                return p
    return None


def paa_png(vpath):
    """Путь текстуры из модели -> TGA в кэше (ImageToPAA читает PAA)."""
    src = resolve(vpath)
    if not src:
        print("WARN нет текстуры", vpath)
        return None
    os.makedirs(CACHE, exist_ok=True)
    dst = os.path.join(CACHE, vpath.replace(BS, "__").replace("/", "__").replace(":", "") + ".tga")
    if not os.path.exists(dst):
        subprocess.run([IMAGE_TO_PAA, src, dst], capture_output=True)
    return dst if os.path.exists(dst) else None


def proc_color(s):
    """#(argb,8,8,3)color(r,g,b,a,...) -> (r,g,b,a)"""
    try:
        inside = s[s.index("color(") + 6:s.rindex(")")]
        v = [float(x) for x in inside.split(",")[:4]]
        return tuple(v + [1.0] * (4 - len(v)))
    except Exception:
        return None


# ---------------------------------------------------------------- геометрия
lod = odol_read.read_visual_lod(MODEL, int(opts.get("lod", 0)))
P = np.array(lod["points"], dtype=np.float64)
N = np.array(lod["normals"], dtype=np.float64)
UV = np.array(lod["uv"], dtype=np.float64)
P = P[:, [0, 2, 1]]                              # DayZ (x, y вверх, z) -> Blender (x, y, z вверх)
N = N[:, [0, 2, 1]]
if opts.get("sel"):                              # hiddenSelections из конфига: {"selections":[], "textures":[], "materials":[]}
    import json
    cfg = json.load(open(opts["sel"], encoding="utf-8"))
    by_name = {s["name"].lower(): s for s in lod["selections"]}
    order = []
    for i, nm in enumerate(cfg.get("selections", [])):
        s = by_name.get(nm.lower())
        if not s:
            print("WARN выделения нет в модели:", nm)
            continue
        order.append((len(s["face_set"]), i, s))
    for _, i, s in sorted(order, key=lambda t: -t[0]):   # крупные сначала, мелкие (экран, стекло) поверх
        tex = (cfg.get("textures") or [])[i] if i < len(cfg.get("textures") or []) else ""
        mat = (cfg.get("materials") or [])[i] if i < len(cfg.get("materials") or []) else ""
        for fi in s["face_set"]:
            if tex:
                lod["face_tex"][fi] = tex
            if mat:
                lod["face_mat"][fi] = mat
        print("SEL", s["name"], len(s["face_set"]), os.path.basename(tex.replace(BS, "/")), os.path.basename(mat.replace(BS, "/")))
faces, face_src = [], []                        # без вырожденных граней (повтор вершины роняет Blender на нормалях)
MAXEDGE = float(opts.get("maxedge", 0) or 0)     # --maxedge=1.5: выкинуть огромные грани (скрытые «лучи фар» у модовых машин)
for fi, f in enumerate(lod["faces"]):
    if MAXEDGE and max(math.dist(lod["points"][f[k]], lod["points"][f[(k + 1) % len(f)]]) for k in range(len(f))) > MAXEDGE:
        continue
    if len(set(f)) == len(f) >= 3:
        faces.append(list(f))
        face_src.append(fi)
lod["face_tex"] = [lod["face_tex"][i] for i in face_src]
lod["face_mat"] = [lod["face_mat"][i] for i in face_src]
if len(face_src) != len(lod["faces"]):
    print("WARN вырожденных граней пропущено:", len(lod["faces"]) - len(face_src))
if opts.get("upside"):                           # перевернуть: поворот 180° вокруг оси Y Blender (обход граней не меняется)
    P[:, [0, 2]] *= -1                           # перстень EFT_SkullRing лежит печаткой вниз
    N[:, [0, 2]] *= -1
if opts.get("flat"):                             # положить «плашмя»: тонкая ось — вертикаль
    ext = P.max(0) - P.min(0)
    ax = int(np.argmin(ext))
    if ax != 2:
        order = [0, 1, 2]
        order[ax], order[2] = order[2], order[ax]
        P, N = P[:, order], N[:, order]
        faces = [f[::-1] for f in faces]         # перестановка осей — отражение, разворачиваем грани

# согласовать порядок обхода граней с нормалями модели (наружу)
agree = 0
for f in faces[: min(len(faces), 4000)]:
    a, b, c = P[f[0]], P[f[1]], P[f[2]]
    fn = np.cross(b - a, c - a)
    agree += 1 if np.dot(fn, N[f[0]] + N[f[1]] + N[f[2]]) > 0 else -1
if agree < 0:
    faces = [f[::-1] for f in faces]
print("GEOM points", len(P), "faces", len(faces), "winding", "flipped" if agree < 0 else "kept", agree)

P[:, 2] -= P[:, 2].min()
lo, hi = P.min(0), P.max(0)
dims = hi - lo
s = float(max(dims))
center = tuple((lo + hi) / 2)
print("DIMS m", [round(float(x), 4) for x in dims], "textures", lod["textures"], "materials",
      [m["name"] for m in lod["materials"]])

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
scene.render.resolution_x = scene.render.resolution_y = SIZE
scene.render.film_transparent = True
scene.render.image_settings.file_format = "PNG"
scene.render.image_settings.color_mode = "RGBA"
scene.render.image_settings.color_depth = "8"
scene.render.filepath = OUT
for vt in ("AgX", "Filmic"):
    try:
        scene.view_settings.view_transform = vt
        break
    except TypeError:
        pass
for lk in ("AgX - Medium High Contrast", "Medium High Contrast"):
    try:
        scene.view_settings.look = lk
        break
    except TypeError:
        pass

_img_cache = {}


def load_img(vpath, noncolor=False):
    key = (vpath, noncolor)
    if key in _img_cache:
        return _img_cache[key]
    path = paa_png(vpath)
    im = None
    if path:
        im = bpy.data.images.load(path)
        if noncolor:
            im.colorspace_settings.name = "Non-Color"
    _img_cache[key] = im
    return im


def make_material(tex, rvmat_path):
    name = (os.path.basename(tex.replace(BS, "/")) or "notex") + "|" + os.path.basename(
        (rvmat_path or "").replace(BS, "/"))
    mat = bpy.data.materials.new(name)
    mat.use_nodes = True
    nt = mat.node_tree
    Nn, L = nt.nodes, nt.links
    bsdf = Nn["Principled BSDF"]
    uvn = Nn.new("ShaderNodeUVMap")
    uvn.uv_map = "UV"

    def tex_node(im):
        t = Nn.new("ShaderNodeTexImage")
        t.image = im
        t.interpolation = "Cubic"
        L.new(uvn.outputs["UV"], t.inputs["Vector"])
        return t

    rv = {}
    if rvmat_path:
        rp = resolve(rvmat_path)
        if rp:
            try:
                rv = rvmat_read.read_rvmat(rp)
            except Exception as e:
                print("WARN rvmat", rvmat_path, e)
        else:
            print("WARN нет rvmat", rvmat_path)
    stage_tex = {k: v.get("texture", "") for k, v in rv.items() if isinstance(v, dict) and k.startswith("Stage")}
    diffuse = rv.get("diffuse") or [1, 1, 1, 1]

    # цвет
    has_alpha = False
    if tex.startswith("#"):
        c = proc_color(tex) or (0.5, 0.5, 0.5, 1)
        bsdf.inputs["Base Color"].default_value = c
    elif tex:
        im = load_img(tex)
        if im:
            t = tex_node(im)
            L.new(t.outputs["Color"], bsdf.inputs["Base Color"])
            low = tex.lower()
            if "_ca." in low or low.endswith("_ca.paa"):
                L.new(t.outputs["Alpha"], bsdf.inputs["Alpha"])
                has_alpha = True
        else:
            bsdf.inputs["Base Color"].default_value = (0.5, 0.5, 0.5, 1)
    else:
        bsdf.inputs["Base Color"].default_value = tuple(list(diffuse[:3]) + [1])

    # нормали
    nohq = next((v for k, v in sorted(stage_tex.items()) if "_nohq" in v.lower() and k != "StageTI"), None)
    if nohq and not nohq.startswith("#"):
        im = load_img(nohq, True)
        if im:
            t = tex_node(im)
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

    # блик / глянец
    smdi = next((v for v in stage_tex.values() if "_smdi" in v.lower()), None)
    spec = rv.get("specular") or [0.2, 0.2, 0.2, 1]
    spow = float(rv.get("specularPower", 40) or 40)
    spec_lvl = min(1.0, float(sum(spec[:3]) / 3.0) * 2.0)
    gloss = min(1.0, math.log10(max(spow, 1.0)) / 3.0)          # 1 -> 0, 1000 -> 1
    if smdi and not smdi.startswith("#"):
        im = load_img(smdi, True)
        if im:
            t = tex_node(im)
            sps = Nn.new("ShaderNodeSeparateColor")
            L.new(t.outputs["Color"], sps.inputs["Color"])
            rough = Nn.new("ShaderNodeMapRange")
            rough.inputs["To Min"].default_value = 0.85
            rough.inputs["To Max"].default_value = 0.08
            L.new(sps.outputs["Blue"], rough.inputs["Value"])
            L.new(rough.outputs["Result"], bsdf.inputs["Roughness"])
            L.new(sps.outputs["Green"], bsdf.inputs["Specular IOR Level"])
    else:
        bsdf.inputs["Roughness"].default_value = 0.85 - 0.75 * gloss
        bsdf.inputs["Specular IOR Level"].default_value = spec_lvl

    # свечение
    em = rv.get("emmisive") or [0, 0, 0, 1]
    if max(em[:3]) > 0.001:
        bsdf.inputs["Emission Color"].default_value = (min(1, em[0]), min(1, em[1]), min(1, em[2]), 1)
        bsdf.inputs["Emission Strength"].default_value = max(em[:3]) if max(em[:3]) > 1 else 1.0
    if has_alpha:
        try:
            mat.surface_render_method = "BLENDED"
        except Exception:
            pass
    print("MAT", name, "nohq" if nohq else "-", "smdi" if smdi else "-", "spec", round(spec_lvl, 2), "gloss",
          round(gloss, 2))
    return mat


# ---------------------------------------------------------------- меш
me = bpy.data.meshes.new("model")
me.from_pydata(P.tolist(), [], faces)
uvl = me.uv_layers.new(name="UV")
loop_uv = []
for f in faces:
    for vi in f:
        loop_uv.extend([UV[vi, 0], 1.0 - UV[vi, 1]])
uvl.data.foreach_set("uv", loop_uv)
keys = []
mats = {}
TEX_OVERRIDE = opts.get("tex")                  # hiddenSelectionsTextures[0]: вместо первой текстуры модели
FIRST_TEX = lod["textures"][0] if lod["textures"] else None
for fi, f in enumerate(faces):
    t = lod["face_tex"][fi] or ""
    if TEX_OVERRIDE and FIRST_TEX and t == FIRST_TEX:
        t = TEX_OVERRIDE
    k = (t, lod["face_mat"][fi] or "")
    if k not in mats:
        mats[k] = len(mats)
        me.materials.append(make_material(*k))
    keys.append(mats[k])
me.polygons.foreach_set("material_index", keys)
me.polygons.foreach_set("use_smooth", [True] * len(faces))
try:                                             # нормали модели (как в игре)
    nrm = N / np.maximum(np.linalg.norm(N, axis=1, keepdims=True), 1e-9)
    if me.validate(verbose=False) or len(me.polygons) != len(faces) or len(me.vertices) != len(nrm):
        raise RuntimeError("меш исправлен Blender, нормали модели не ставлю (сглаживание Blender)")
    me.normals_split_custom_set_from_vertices(nrm.tolist())
except Exception as e:
    print("WARN normals", e)
me.update()
ob = bpy.data.objects.new("model", me)
scene.collection.objects.link(ob)

world = bpy.data.worlds.new("w")
world.use_nodes = True
world.node_tree.nodes["Background"].inputs["Color"].default_value = (0.05, 0.055, 0.06, 1)
world.node_tree.nodes["Background"].inputs["Strength"].default_value = 1.0
scene.world = world


def area(name, loc, power, size, color):
    ld = bpy.data.lights.new(name, "AREA")
    ld.energy = power
    ld.size = size
    ld.color = color
    lo_ = bpy.data.objects.new(name, ld)
    lo_.location = (center[0] + loc[0], center[1] + loc[1], center[2] + loc[2])
    tc = lo_.constraints.new("TRACK_TO")
    tc.target = ob
    tc.track_axis = "TRACK_NEGATIVE_Z"
    tc.up_axis = "UP_Y"
    scene.collection.objects.link(lo_)


k = (s / 0.05) ** 2 * float(opts.get("light", 1.0))
area("key", (-3.0 * s, -3.2 * s, 3.6 * s), 1.1 * k, 2.5 * s, (1.0, 0.95, 0.88))
area("fill", (3.6 * s, -2.4 * s, 1.4 * s), 0.35 * k, 3.0 * s, (0.85, 0.9, 1.0))
area("rim", (1.2 * s, 3.0 * s, 2.8 * s), 1.6 * k, 1.6 * s, (0.85, 0.9, 1.0))

cam_d = bpy.data.cameras.new("cam")
cam_d.lens = 85
cam_d.sensor_width = 36
r = 0.5 * float(np.linalg.norm(dims))
dist = r / math.sin(math.atan(18.0 / 85.0)) * float(opts.get("frame", 1.1))
cam_d.clip_start = dist * 0.01                   # мелкие предметы (кольцо 2 см): стандартные 0.1 м режут всё
cam_d.clip_end = dist * 100
elev, azim = math.radians(float(opts.get("elev", 22))), math.radians(float(opts.get("azim", -28)))
cam = bpy.data.objects.new("cam", cam_d)
cam.location = (center[0] + dist * math.cos(elev) * math.sin(azim),
                center[1] - dist * math.cos(elev) * math.cos(azim),
                center[2] + dist * math.sin(elev))
tgt = bpy.data.objects.new("target", None)
tgt.location = center
scene.collection.objects.link(tgt)
tc = cam.constraints.new("TRACK_TO")
tc.target = tgt
tc.track_axis = "TRACK_NEGATIVE_Z"
tc.up_axis = "UP_Y"
scene.collection.objects.link(cam)
scene.camera = cam

bpy.ops.render.render(write_still=True)
print("RENDERED", OUT)


def crop_to_object(path, margin=0.04):
    im = bpy.data.images.load(path)
    w, h = im.size
    px = np.array(im.pixels[:], dtype=np.float32).reshape(h, w, 4)
    ys, xs = np.nonzero(px[..., 3] > 0.003)
    x0, x1, y0, y1 = xs.min(), xs.max() + 1, ys.min(), ys.max() + 1
    side = int(max(x1 - x0, y1 - y0) * (1 + 2 * margin))
    out = np.zeros((side, side, 4), dtype=np.float32)
    ox, oy = (side - (x1 - x0)) // 2, (side - (y1 - y0)) // 2
    out[oy:oy + y1 - y0, ox:ox + x1 - x0] = px[y0:y1, x0:x1]
    new = bpy.data.images.new("crop", side, side, alpha=True)
    new.pixels.foreach_set(out.ravel())
    new.filepath_raw = path
    new.file_format = "PNG"
    new.save()
    print("CROPPED", "%dx%d" % (side, side), "object %dx%d" % (x1 - x0, y1 - y0))


if not opts.get("nocrop"):
    crop_to_object(OUT, float(opts.get("margin", 0.04)))
