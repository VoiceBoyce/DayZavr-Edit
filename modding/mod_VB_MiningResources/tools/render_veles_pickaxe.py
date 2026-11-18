# -*- coding: utf-8 -*-
r"""Рендер «Велесовой кирки» на утверждение (Blender, Cycles, прозрачный фон).

  blender.exe -b --factory-startup --python render_veles_pickaxe.py -- --out=<папка png>
      [--model=<vb_mr_pickaxe_veles.p3d>] [--vanilla=P:\DZ\weapons\melee\blade\pickaxe.p3d] [--png=<папка png
      генератора, если PAA ещё нет>] [--shots=main,head,rune,amber,compare,glow] [--lod=0] [--size=1600]
      [--samples=128] [--prefix=VB_MR_Pickaxe_Veles]

Модель — MLOD из генератора (tools\gen_veles_pickaxe.py) или ODOL из собранного PBO; у каждой грани своя текстура
и rvmat, как в игре. Шейдер: _co — цвет, _nohq — нормали (DirectX -> инверсия G), _smdi — блик (G) и глянец (B);
навершие и золото — металл, янтарь — глянец со слабым свечением, vb_mr_veles_glow_on — свечение Stage8 × emmisive.
Снимки:
  main    — общий вид (VB_MR_Pickaxe_Veles.png);
  head    — навершие крупно, rune — руна на щеке;
  amber   — янтарь в корневой лапе;
  compare — рядом с ванильной киркой, тот же масштаб (ортографическая камера сбоку: слева ваниль, справа новая);
  glow    — навершие ночью со светящимся золотом (материал vb_mr_veles_glow_on).
"""
import math
import os
import subprocess
import sys
import tempfile

import bpy
import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
MOD_ROOT = os.path.dirname(HERE)
REPO = os.path.normpath(os.path.join(MOD_ROOT, "..", ".."))
sys.path.insert(0, os.path.join(REPO, ".claude", "tools", "dayz-render"))
sys.path.insert(0, os.path.join(REPO, ".claude", "tools", "py3d"))
import odol_read  # noqa: E402
import rvmat_read  # noqa: E402

BS = chr(92)
IMAGE_TO_PAA = os.path.join("D:" + BS, "SteamLibrary", "steamapps", "common", "DayZ Tools", "Bin", "ImageToPAA",
                            "ImageToPAA.exe")

args = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
opts = {a.split("=", 1)[0][2:]: (a.split("=", 1)[1] if "=" in a else True) for a in args if a.startswith("--")}
OUT = opts["out"]
MODEL = opts.get("model", os.path.join(MOD_ROOT, "VB_MiningResources", "data", "veles", "vb_mr_pickaxe_veles.p3d"))
VANILLA = opts.get("vanilla", "P:" + BS + BS.join(["DZ", "weapons", "melee", "blade", "pickaxe.p3d"]))
PNG = opts.get("png")
SHOTS = str(opts.get("shots", "main,head,rune,amber,compare,glow")).split(",")
LOD = int(opts.get("lod", 0))
SIZE = int(opts.get("size", 1600))
SAMPLES = int(opts.get("samples", 128))
PREFIX = opts.get("prefix", "VB_MR_Pickaxe_Veles")
CACHE = os.path.join(tempfile.gettempdir(), "vb_mr_veles_texcache")      # PAA -> TGA, не в папку рендеров


def resolve(vpath):
    vp = vpath.replace("/", BS).lstrip(BS)
    if PNG:
        p = os.path.join(PNG, os.path.splitext(os.path.basename(vp))[0] + ".png")
        if os.path.exists(p):
            return p
    for root in (MOD_ROOT, "P:" + BS):
        p = os.path.join(root, vp)
        if os.path.exists(p):
            return p
    return None


def tex_file(vpath):
    src = resolve(vpath)
    if not src:
        print("WARN нет текстуры", vpath)
        return None
    if src.lower().endswith(".png"):
        return src
    os.makedirs(CACHE, exist_ok=True)
    dst = os.path.join(CACHE, vpath.replace(BS, "__").replace("/", "__").replace(":", "") + ".tga")
    if not os.path.exists(dst) or os.path.getmtime(dst) < os.path.getmtime(src):
        subprocess.run([IMAGE_TO_PAA, src, dst], capture_output=True)
    return dst if os.path.exists(dst) else None


# ---------------------------------------------------------------- модель
def read_model(path, lod_index=0):
    """Визуальный LOD в осях Blender: точки, грани, UV и нормали на угол, (текстура, материал) на грань."""
    head = open(path, "rb").read(4)
    if head == b"ODOL":
        lod = odol_read.read_visual_lod(path, lod_index)
        P = np.array(lod["points"], np.float64)[:, [0, 2, 1]]
        N = np.array(lod["normals"], np.float64)[:, [0, 2, 1]]
        UV = np.array(lod["uv"], np.float64)
        faces = list(lod["faces"])
        luv = [[UV[v] for v in f] for f in faces]
        ln = [[N[v] for v in f] for f in faces]
        keys = [(t or "", m or "") for t, m in zip(lod["face_tex"], lod["face_mat"])]
        return P, faces, luv, ln, keys
    import py3d
    with open(path, "rb") as f:
        m = py3d.P3D(f)
    lod = m.lods[lod_index]
    P = np.array([p.coords for p in lod.points], np.float64)[:, [0, 2, 1]]
    FN = -np.array(lod.facenormals, np.float64)[:, [0, 2, 1]]     # MLOD: нормали внутрь
    faces, luv, ln, keys = [], [], [], []
    for fc in lod.faces:
        faces.append([v.point_index for v in fc.vertices])
        luv.append([v.uv for v in fc.vertices])
        ln.append([FN[v.normal_index] for v in fc.vertices])
        keys.append((fc.texture or "", fc.material or ""))
    return P, faces, luv, ln, keys


def fix_winding(P, faces, luv, ln):
    agree = 0
    for f, nn in list(zip(faces, ln))[:4000]:
        a, b, c = P[f[0]], P[f[1]], P[f[2]]
        agree += 1 if np.dot(np.cross(b - a, c - a), np.sum(nn, axis=0)) > 0 else -1
    if agree < 0:
        faces = [f[::-1] for f in faces]
        luv = [u[::-1] for u in luv]
        ln = [n[::-1] for n in ln]
    return faces, luv, ln


# ---------------------------------------------------------------- сцена
def setup_scene():
    bpy.ops.wm.read_factory_settings(use_empty=True)
    sc = bpy.context.scene
    sc.render.engine = "CYCLES"
    try:
        cp = bpy.context.preferences.addons["cycles"].preferences
        for dt in ("OPTIX", "CUDA"):
            try:
                cp.compute_device_type = dt
                cp.get_devices()
                if any(d.type == dt for d in cp.devices):
                    for d in cp.devices:
                        d.use = d.type == dt
                    sc.cycles.device = "GPU"
                    break
            except Exception:
                pass
    except Exception:
        pass
    sc.cycles.samples = SAMPLES
    sc.cycles.use_denoising = True
    sc.render.film_transparent = True
    sc.render.image_settings.file_format = "PNG"
    sc.render.image_settings.color_mode = "RGBA"
    for vt in ("AgX", "Filmic"):
        try:
            sc.view_settings.view_transform = vt
            break
        except TypeError:
            pass
    for lk in ("AgX - Medium High Contrast", "Medium High Contrast"):
        try:
            sc.view_settings.look = lk
            break
        except TypeError:
            pass
    return sc


def set_world(sc, night=False):
    w = bpy.data.worlds.new("w")
    w.use_nodes = True
    nt = w.node_tree
    bg = nt.nodes["Background"]
    tc = nt.nodes.new("ShaderNodeTexCoord")
    sep = nt.nodes.new("ShaderNodeSeparateXYZ")
    ramp = nt.nodes.new("ShaderNodeValToRGB")
    mr = nt.nodes.new("ShaderNodeMapRange")
    mr.inputs["From Min"].default_value = -1.0
    mr.inputs["From Max"].default_value = 1.0
    nt.links.new(tc.outputs["Generated"], sep.inputs["Vector"])
    nt.links.new(sep.outputs["Z"], mr.inputs["Value"])
    nt.links.new(mr.outputs["Result"], ramp.inputs["Fac"])
    if night:
        ramp.color_ramp.elements[0].color = (0.004, 0.005, 0.008, 1)
        ramp.color_ramp.elements[1].color = (0.020, 0.028, 0.050, 1)
        bg.inputs["Strength"].default_value = 0.6
    else:
        ramp.color_ramp.elements[0].position = 0.35
        ramp.color_ramp.elements[0].color = (0.035, 0.032, 0.03, 1)
        ramp.color_ramp.elements[1].position = 0.85
        ramp.color_ramp.elements[1].color = (0.58, 0.60, 0.63, 1)
        e = ramp.color_ramp.elements.new(0.55)
        e.color = (0.28, 0.27, 0.25, 1)
        bg.inputs["Strength"].default_value = 0.9
    nt.links.new(ramp.outputs["Color"], bg.inputs["Color"])
    sc.world = w


def clear_objects():
    for ob in list(bpy.data.objects):
        bpy.data.objects.remove(ob, do_unlink=True)
    for coll in (bpy.data.meshes, bpy.data.lights, bpy.data.cameras, bpy.data.curves):
        for it in list(coll):
            coll.remove(it)


IMG_CACHE = {}


def load_img(vpath, noncolor=False):
    key = (vpath, noncolor)
    if key in IMG_CACHE:
        return IMG_CACHE[key]
    p = tex_file(vpath) if vpath and not vpath.startswith("#") else None
    im = None
    if p:
        im = bpy.data.images.load(p)
        if noncolor:
            im.colorspace_settings.name = "Non-Color"
    IMG_CACHE[key] = im
    return im


MAT_CACHE = {}


def make_material(tex, rvpath, glow_on=False):
    key = (tex, rvpath, glow_on)
    if key in MAT_CACHE:
        return MAT_CACHE[key]
    name = os.path.basename(rvpath.replace(BS, "/")).lower()
    if glow_on and "vb_mr_veles_glow" in name:
        rvpath = rvpath.replace("vb_mr_veles_glow.rvmat", "vb_mr_veles_glow_on.rvmat")
        name = "vb_mr_veles_glow_on.rvmat"
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
    im = load_img(tex)
    if im:
        L.new(tex_node(im).outputs["Color"], bsdf.inputs["Base Color"])
    rv = {}
    rp = resolve(rvpath) if rvpath else None
    if rp:
        rv = rvmat_read.read_rvmat(rp)
    st = {k: v.get("texture", "") for k, v in rv.items() if isinstance(v, dict) and k.startswith("Stage")}
    metal = ("metal" in name) or ("glow" in name)
    amber = "amber" in name
    vanilla = "pickaxe.rvmat" in name
    nohq = st.get("Stage1", "")
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
    spec = rv.get("specular") or [0.3, 0.3, 0.3, 1]
    spow = float(rv.get("specularPower", 40) or 40)
    lvl = min(1.0, float(sum(spec[:3]) / 3.0))
    gloss = min(1.0, math.log10(max(spow, 1.0)) / 2.4)
    # DayZ Super — не PBR: у металла текстура остаётся рассеянным цветом, поэтому не 1.0 (иначе навершие чёрное)
    bsdf.inputs["Metallic"].default_value = (0.95 if "glow" in name else 0.70) if metal else (0.45 if vanilla else 0.0)
    smdi = st.get("Stage5", "")
    if smdi and not smdi.startswith("#"):
        im = load_img(smdi, True)
        if im:
            t = tex_node(im)
            sp = Nn.new("ShaderNodeSeparateColor")
            L.new(t.outputs["Color"], sp.inputs["Color"])
            mul = Nn.new("ShaderNodeMath")
            mul.operation = "MULTIPLY"
            mul.inputs[1].default_value = gloss
            L.new(sp.outputs["Blue"], mul.inputs[0])
            rough = Nn.new("ShaderNodeMapRange")
            rough.inputs["To Min"].default_value = 0.92
            rough.inputs["To Max"].default_value = 0.10 if metal else (0.03 if amber else 0.08)
            L.new(mul.outputs["Value"], rough.inputs["Value"])
            L.new(rough.outputs["Result"], bsdf.inputs["Roughness"])
            sl = Nn.new("ShaderNodeMath")
            sl.operation = "MULTIPLY"
            sl.inputs[1].default_value = lvl if metal else min(lvl, 1.0) * 0.6
            L.new(sp.outputs["Green"], sl.inputs[0])
            L.new(sl.outputs["Value"], bsdf.inputs["Specular IOR Level"])
    else:
        bsdf.inputs["Roughness"].default_value = 0.22 if metal else 0.5
    if amber:
        try:
            bsdf.inputs["Coat Weight"].default_value = 0.6
            bsdf.inputs["Coat Roughness"].default_value = 0.04
        except KeyError:
            pass
    em = rv.get("emmisive") or [0, 0, 0, 1]
    e_tex = st.get("Stage8", "")
    em_s = float(max(em[:3]))
    if em_s > 0 and e_tex:
        if e_tex.startswith("#"):
            import re
            mm = re.search(r"color\(([\d.]+),([\d.]+),([\d.]+)", e_tex)
            c = [float(x) for x in mm.groups()] if mm else [1, 1, 1]
            bsdf.inputs["Emission Color"].default_value = (c[0], c[1], c[2], 1)
        else:
            im = load_img(e_tex)
            if im:
                L.new(tex_node(im).outputs["Color"], bsdf.inputs["Emission Color"])
        bsdf.inputs["Emission Strength"].default_value = em_s * float(opts.get("emk", 0.7))
    MAT_CACHE[key] = mat
    return mat


def build_object(name, P, faces, luv, ln, keys, offset=(0, 0, 0), glow_on=False):
    me = bpy.data.meshes.new(name)
    me.from_pydata((P + np.asarray(offset)).tolist(), [], faces)
    uvl = me.uv_layers.new(name="UV")
    flat = []
    for f in luv:
        for uv in f:
            flat += [uv[0], 1.0 - uv[1]]
    uvl.data.foreach_set("uv", flat)
    slots = {}
    for k in keys:
        if k not in slots:
            slots[k] = len(slots)
            me.materials.append(make_material(k[0], k[1], glow_on))
    me.polygons.foreach_set("material_index", [slots[k] for k in keys])
    me.polygons.foreach_set("use_smooth", [True] * len(faces))
    lnn = []
    for f in ln:
        for n in f:
            v = np.asarray(n, float)
            lnn.append(tuple(v / max(np.linalg.norm(v), 1e-9)))
    try:
        if me.validate(verbose=False) or len(me.polygons) != len(faces):
            raise RuntimeError("Blender исправил меш")
        me.normals_split_custom_set(lnn)
    except Exception as e:
        print("WARN normals", e)
    me.update()
    ob = bpy.data.objects.new(name, me)
    bpy.context.scene.collection.objects.link(ob)
    return ob


def area(sc, name, loc, target, power, size, color, glossy=True):
    ld = bpy.data.lights.new(name, "AREA")
    ld.energy = power
    ld.size = size
    ld.color = color
    o = bpy.data.objects.new(name, ld)
    o.location = tuple(np.asarray(target) + np.asarray(loc))
    o.visible_glossy = glossy
    t = bpy.data.objects.new(name + "_t", None)
    t.location = tuple(target)
    sc.collection.objects.link(t)
    c = o.constraints.new("TRACK_TO")
    c.target = t
    c.track_axis = "TRACK_NEGATIVE_Z"
    c.up_axis = "UP_Y"
    sc.collection.objects.link(o)


def lights(sc, target, s, night=False):
    k = (s / 0.05) ** 2
    if night:
        area(sc, "moon", (2.0 * s, -3.0 * s, 4.0 * s), target, 0.45 * k, 3.0 * s, (0.55, 0.65, 1.0))
        area(sc, "rim", (-2.5 * s, 3.0 * s, 1.5 * s), target, 0.25 * k, 2.0 * s, (0.5, 0.6, 1.0))
        # золотой свет в точке памяти vbmr_light (оси DayZ (0.0415, 0.6605, 0) -> Blender (0.0415, 0, 0.6605)),
        # к ней скрипт «Чует жилу» прикрепит свой свет
        ld = bpy.data.lights.new("vbmr_light", "POINT")
        ld.energy = 0.12
        ld.shadow_soft_size = 0.008
        ld.color = (1.0, 0.78, 0.38)
        o = bpy.data.objects.new("vbmr_light", ld)
        o.location = (0.0415, 0.0, 0.6605)
        sc.collection.objects.link(o)
        return
    area(sc, "key", (2.6 * s, -3.0 * s, 3.4 * s), target, 1.0 * k, 2.5 * s, (1.0, 0.95, 0.88))
    area(sc, "fill", (3.4 * s, 2.6 * s, 1.0 * s), target, 0.35 * k, 3.0 * s, (0.85, 0.9, 1.0), glossy=False)
    area(sc, "rim", (-2.5 * s, 2.0 * s, 2.6 * s), target, 1.2 * k, 1.6 * s, (0.85, 0.9, 1.0))


def camera(sc, target, half, elev, azim, lens=85, ortho=None):
    cam_d = bpy.data.cameras.new("cam")
    if ortho:
        cam_d.type = "ORTHO"
        cam_d.ortho_scale = ortho
        dist = 4.0
    else:
        cam_d.lens = lens
        dist = half / math.sin(math.atan(18.0 / lens)) * 1.05
    cam_d.sensor_width = 36
    cam_d.clip_start = dist * 0.01
    cam_d.clip_end = dist * 100
    e, a = math.radians(elev), math.radians(azim)
    cam = bpy.data.objects.new("cam", cam_d)
    cam.location = (target[0] + dist * math.cos(e) * math.sin(a), target[1] - dist * math.cos(e) * math.cos(a),
                    target[2] + dist * math.sin(e))
    tgt = bpy.data.objects.new("cam_t", None)
    tgt.location = tuple(target)
    sc.collection.objects.link(tgt)
    c = cam.constraints.new("TRACK_TO")
    c.target = tgt
    c.track_axis = "TRACK_NEGATIVE_Z"
    c.up_axis = "UP_Y"
    sc.collection.objects.link(cam)
    sc.camera = cam


def label(sc, text, loc, size, rot=(math.radians(90), 0, math.radians(90))):
    cu = bpy.data.curves.new("t", "FONT")
    cu.body = text
    cu.size = size
    cu.align_x = "CENTER"
    cu.align_y = "CENTER"
    o = bpy.data.objects.new("t", cu)
    o.location = loc
    o.rotation_euler = rot
    m = bpy.data.materials.new("lab")
    m.use_nodes = True
    b = m.node_tree.nodes["Principled BSDF"]
    b.inputs["Base Color"].default_value = (0.9, 0.9, 0.9, 1)
    b.inputs["Emission Color"].default_value = (0.9, 0.9, 0.9, 1)
    b.inputs["Emission Strength"].default_value = 1.0
    cu.materials.append(m)
    sc.collection.objects.link(o)


def render(sc, path, crop_margin=0.04):
    sc.render.resolution_x = sc.render.resolution_y = SIZE
    sc.render.filepath = path
    bpy.ops.render.render(write_still=True)
    crop(path, crop_margin)
    print("RENDER", path)


def crop(path, margin):
    im = bpy.data.images.load(path)
    w, h = im.size
    px = np.array(im.pixels[:], np.float32).reshape(h, w, 4)
    bpy.data.images.remove(im)
    ys, xs = np.nonzero(px[..., 3] > 0.003)
    if len(xs) == 0:
        return
    x0, x1, y0, y1 = xs.min(), xs.max() + 1, ys.min(), ys.max() + 1
    side = int(max(x1 - x0, y1 - y0) * (1 + 2 * margin))
    out = np.zeros((side, side, 4), np.float32)
    ox, oy = (side - (x1 - x0)) // 2, (side - (y1 - y0)) // 2
    out[oy:oy + y1 - y0, ox:ox + x1 - x0] = px[y0:y1, x0:x1]
    new = bpy.data.images.new("tmp", side, side, alpha=True)
    new.pixels.foreach_set(np.ascontiguousarray(out, np.float32).ravel())
    new.filepath_raw = path
    new.file_format = "PNG"
    new.save()
    bpy.data.images.remove(new)


def main():
    os.makedirs(OUT, exist_ok=True)
    sc = setup_scene()
    P, faces, luv, ln, keys = read_model(MODEL, LOD)
    faces, luv, ln = fix_winding(P, faces, luv, ln)
    lo, hi = P.min(0), P.max(0)
    print("MODEL %s LOD %d: граней %d, габарит x %.3f..%.3f y %.3f..%.3f z %.3f..%.3f" % (
        os.path.basename(MODEL), LOD, len(faces), lo[0], hi[0], lo[1], hi[1], lo[2], hi[2]))
    suffix = "" if LOD == 0 else "_lod%d" % LOD
    shots = {
        "main": dict(target=(0.0, 0.0, 0.23), half=0.50, elev=12, azim=72, file=PREFIX + suffix + ".png"),
        "head": dict(target=(0.0, 0.0, 0.655), half=0.27, elev=20, azim=70, file=PREFIX + "_head" + suffix + ".png"),
        "rune": dict(target=(0.03, 0.0, 0.655), half=0.055, elev=10, azim=80, file=PREFIX + "_rune" + suffix + ".png"),
        "amber": dict(target=(0.0, 0.0, -0.195), half=0.065, elev=24, azim=58, file=PREFIX + "_amber" + suffix + ".png"),
        "roots": dict(target=(0.0, 0.0, 0.53), half=0.11, elev=8, azim=60, file=PREFIX + "_roots" + suffix + ".png"),
        "glow": dict(target=(0.0, 0.0, 0.655), half=0.22, elev=14, azim=78, file=PREFIX + "_glow" + suffix + ".png"),
    }
    for shot in SHOTS:
        clear_objects()
        if shot == "compare":
            set_world(sc)
            Pv, fv, uvv, lnv, kv = read_model(VANILLA, 0)
            fv, uvv, lnv = fix_winding(Pv, fv, uvv, lnv)
            build_object("vanilla", Pv, fv, uvv, lnv, kv, offset=(0, -0.31, 0))
            build_object("veles", P, faces, luv, ln, keys, offset=(0, 0.31, 0))
            label(sc, "ваниль", (0.06, -0.31, -0.32), 0.05)
            label(sc, "Велесова", (0.06, 0.31, -0.32), 0.05)
            tgt = (0.0, 0.0, 0.20)
            lights(sc, tgt, 0.6)
            camera(sc, tgt, 0.6, 0, 90, ortho=1.30)
            render(sc, os.path.join(OUT, PREFIX + "_compare" + suffix + ".png"), 0.03)
            continue
        cfg = shots[shot]
        night = shot == "glow"
        set_world(sc, night)
        build_object("veles", P, faces, luv, ln, keys, glow_on=night)
        lights(sc, cfg["target"], cfg["half"], night)
        camera(sc, cfg["target"], cfg["half"], cfg["elev"], cfg["azim"])
        render(sc, os.path.join(OUT, cfg["file"]), 0.03 if shot in ("main",) else 0.0)


main()
