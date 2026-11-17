# -*- coding: utf-8 -*-
r"""Echo Runner, этап 3: развёртка, текстуры, PAA, rvmat и рендеры в цвете.

  G:\SteamLibrary\steamapps\common\Blender\blender.exe -b --factory-startup --python er_stage3.py -- <папка рендеров>

Пишет:
  mod_vb_EchoRunner\tex_src\er_*.tga           — исходники текстур (пересоздаются этим скриптом)
  mod_vb_EchoRunner\VB_EchoRunner\data\er_*.paa — текстуры для игры (ImageToPAA)
  mod_vb_EchoRunner\VB_EchoRunner\data\er_*.rvmat — материалы (Super, шаблон vanilla aircraft_battery.rvmat)
"""
import math
import os
import subprocess
import sys
import time

import bpy
import numpy as np
from mathutils import Vector

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import er_model as M  # noqa: E402
import er_textures as TX  # noqa: E402

MOD = os.path.dirname(HERE)
TEX_SRC = os.path.join(MOD, "tex_src")
DATA = os.path.join(MOD, "VB_EchoRunner", "data")
PBO_DATA = "VB_EchoRunner\\data\\"
IMG2PAA = r"D:\SteamLibrary\steamapps\common\DayZ Tools\Bin\ImageToPAA\ImageToPAA.exe"

ARGS = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
OUT = os.path.abspath(ARGS[0]) if ARGS else os.path.join(MOD, "рендеры", "этап3")
os.makedirs(OUT, exist_ok=True)

# материал модели -> набор текстур
TEXMAP = {"body": "body", "steel_in": "inner", "pipe": "metal_rust", "steel": "metal_rust", "copper": "copper",
          "hose": "rubber", "tire": "rubber", "clamp": "chrome", "chrome": "chrome", "black": "plastic",
          "dash": "plastic", "rim": "rim", "engine": "engine", "seat": "seat", "floor": "floor", "net": "net",
          "lamp": "lamp", "tail": "tail", "amber": "amber"}
# rvmat: блик, сила блика, френель, отражение
RV = {
    "body":       ((0.80, 0.80, 0.82), 120, (0.6, 0.45), "dz\\data\\data\\env_land_co.paa"),
    "metal_rust": ((0.30, 0.28, 0.26), 40, (0.4, 0.4), None),
    "copper":     ((0.70, 0.55, 0.45), 90, (0.6, 0.45), "dz\\data\\data\\env_land_co.paa"),
    "rubber":     ((0.10, 0.10, 0.10), 20, (0.3, 0.3), None),
    "plastic":    ((0.25, 0.25, 0.25), 40, (0.4, 0.4), None),
    "chrome":     ((0.90, 0.90, 0.92), 500, (0.71, 0.18), "dz\\data\\data\\env_land_chrome_co.paa"),
    "inner":      ((0.25, 0.25, 0.25), 40, (0.4, 0.4), None),
    "engine":     ((0.45, 0.45, 0.45), 60, (0.5, 0.4), None),
    "seat":       ((0.20, 0.20, 0.20), 30, (0.4, 0.4), None),
    "floor":      ((0.10, 0.10, 0.10), 20, (0.3, 0.3), None),
    "rim":        ((0.40, 0.40, 0.40), 60, (0.5, 0.4), None),
    "net":        ((0.40, 0.40, 0.40), 60, (0.5, 0.4), None),
    "lamp":       ((0.90, 0.90, 0.90), 300, (0.7, 0.2), "dz\\data\\data\\env_land_co.paa"),
    "tail":       ((0.80, 0.80, 0.80), 300, (0.7, 0.2), "dz\\data\\data\\env_land_co.paa"),
    "amber":      ((0.80, 0.80, 0.80), 300, (0.7, 0.2), "dz\\data\\data\\env_land_co.paa"),
}


def stage(n, tex, ident=True):
    a, u = ("1.0,0.0,0.0", "0.0,1.0,0.0") if ident else ("0.0,0.0,0.0", "0.0,0.0,0.0")
    return ("class Stage%d\n{\n\ttexture = \"%s\";\n\tuvSource = \"tex\";\n\tclass uvTransform\n\t{\n"
            "\t\taside[] = {%s};\n\t\tup[] = {%s};\n\t\tdir[] = {0.0,0.0,0.0};\n\t\tpos[] = {0.0,0.0,0.0};\n"
            "\t};\n};\n") % (n, tex, a, u)


def rvmat_text(name, alpha=False):
    spec, spow, fr, env = RV[name]
    p = PBO_DATA + "er_" + name
    head = ("// Echo Runner: %s. Шаблон Super — vanilla DZ\\vehicles\\parts\\data\\aircraft_battery.rvmat.\n"
            "ambient[] = {1.0,1.0,1.0,1.0};\ndiffuse[] = {1.0,1.0,1.0,1.0};\nforcedDiffuse[] = {0.0,0.0,0.0,0.0};\n"
            "emmisive[] = {0.0,0.0,0.0,1.0};\nspecular[] = {%.2f,%.2f,%.2f,1.0};\nspecularPower = %d;\n"
            "PixelShaderID = \"Super\";\nVertexShaderID = \"Super\";\n") % (name, spec[0], spec[1], spec[2], spow)
    if alpha:
        head += "renderFlags[] = {\"AlphaTest32\"};\n"
    body = (stage(1, p + "_nohq.paa") + stage(2, "#(argb,8,8,3)color(0.5,0.5,0.5,1,DT)", False)
            + stage(3, "#(argb,8,8,3)color(0,0,0,0,MC)") + stage(4, "#(argb,8,8,3)color(1,1,1,1,AS)")
            + stage(5, p + "_smdi.paa") + stage(6, "#(ai,64,64,1)fresnel(%.2f,%.2f)" % fr)
            + stage(7, env or "#(argb,8,8,3)color(0,0,0,1,CO)"))
    return head + body


# ------------------------------------------------------------------ сцена и модель
bpy.ops.wm.read_factory_settings(use_empty=True)
scene = bpy.context.scene
COL = scene.collection
t0 = time.time()
objs, pivots = M.build_all()
print("MODEL %.1fs" % (time.time() - t0))
t0 = time.time()
TEX = TX.all_textures()
print("TEXTURES %.1fs" % (time.time() - t0))

def export_all():
    os.makedirs(DATA, exist_ok=True)
    srcs = TX.export_sources(TEX, TEX_SRC)
    bad = []
    for s in srcs:
        dst = os.path.join(DATA, os.path.splitext(os.path.basename(s))[0] + ".paa")
        r = subprocess.run([IMG2PAA, s, dst], capture_output=True, text=True)
        if r.returncode != 0 or not os.path.isfile(dst):
            bad.append((os.path.basename(s), r.returncode, (r.stdout + r.stderr).strip()[-200:]))
    print("PAA %d of %d ok" % (len(srcs) - len(bad), len(srcs)))
    for b in bad:
        print("PAA_FAIL", b)
    for name in TEX:
        with open(os.path.join(DATA, "er_%s.rvmat" % name), "w", encoding="utf-8") as f:
            f.write(rvmat_text(name, alpha=TEX[name]["alpha"] is not None))
    print("RVMAT", len(TEX))


# ------------------------------------------------------------------ материалы предпросмотра
def image(name, arr, noncolor=False, alpha=None):
    h, w = arr.shape[:2]
    rgba = np.ones((h, w, 4), np.float32)
    if arr.ndim == 2:
        rgba[..., :3] = arr[..., None]
    else:
        rgba[..., :3] = arr[..., :3]
    if alpha is not None:
        rgba[..., 3] = alpha
    im = bpy.data.images.new(name, w, h, alpha=True)
    if noncolor:
        im.colorspace_settings.name = "Non-Color"
    im.pixels.foreach_set(np.ascontiguousarray(rgba[::-1]).ravel())
    return im


PREV = {}


def preview_mat(name):
    if name in PREV:
        return PREV[name]
    t = TEX[name]
    m = bpy.data.materials.new("pv_" + name)
    try:
        m.use_nodes = True
    except Exception:
        pass
    nt = m.node_tree
    pr = next(n for n in nt.nodes if n.type == "BSDF_PRINCIPLED")
    tco = nt.nodes.new("ShaderNodeTexImage")
    tco.image = image(name + "_co", t["co"], alpha=t["alpha"])
    nt.links.new(tco.outputs["Color"], pr.inputs["Base Color"])
    if t["alpha"] is not None:
        nt.links.new(tco.outputs["Alpha"], pr.inputs["Alpha"])
    tr = nt.nodes.new("ShaderNodeTexImage")
    tr.image = image(name + "_r", t["rough"], noncolor=True)
    nt.links.new(tr.outputs["Color"], pr.inputs["Roughness"])
    tm = nt.nodes.new("ShaderNodeTexImage")
    tm.image = image(name + "_m", t["metal"], noncolor=True)
    nt.links.new(tm.outputs["Color"], pr.inputs["Metallic"])
    tn = nt.nodes.new("ShaderNodeTexImage")
    tn.image = image(name + "_n", t["nohq_gl"], noncolor=True)
    nm = nt.nodes.new("ShaderNodeNormalMap")
    nt.links.new(tn.outputs["Color"], nm.inputs["Color"])
    nt.links.new(nm.outputs["Normal"], pr.inputs["Normal"])
    PREV[name] = m
    return m


for o in objs.values():
    for i, sl in enumerate(o.data.materials):
        key = TEXMAP.get(sl.name)
        if key:
            o.data.materials[i] = preview_mat(key)

opened = {k: M.opened_copy(objs[k], pivots[k], a) for k, a in
          (("door_l", 72), ("door_r", 72), ("hood", 55), ("engine_lid", 48))}


def set_open(state):
    for k, o in opened.items():
        o.hide_render = not state
        objs[k].hide_render = state


# ------------------------------------------------------------------ свет, пол, человек
M.PALETTE["ground"] = dict(color=(0.55, 0.56, 0.58), rough=0.9)
M.PALETTE["man"] = dict(color=(0.30, 0.32, 0.36), rough=0.7)
pg = M.Part("floor")
v = [pg.bm.verts.new(c) for c in ((-400, -400, 0), (400, -400, 0), (400, 400, 0), (-400, 400, 0))]
pg.bm.faces.new(v).material_index = pg.mi("ground")
pg.finish()
pm = M.Part("man_1_8m")
M.lathe(pm, (1.55, -3.0, 0.0), (0, 0, 1), [(0.0001, 0.0), (0.14, 0.0), (0.16, 0.85), (0.20, 1.30), (0.17, 1.45),
                                            (0.07, 1.52), (0.10, 1.60), (0.10, 1.72), (0.06, 1.79), (0.0001, 1.80)],
        "man", 24, closed=False)
man = pm.finish(60)

world = bpy.data.worlds.new("w")
try:
    world.use_nodes = True
except Exception:
    pass
bg = next(n for n in world.node_tree.nodes if n.type == "BACKGROUND")
bg.inputs["Color"].default_value = (0.62, 0.66, 0.72, 1)
bg.inputs["Strength"].default_value = 0.9
scene.world = world
sun_d = bpy.data.lights.new("sun", "SUN")
sun_d.energy = 3.2
sun_d.angle = math.radians(8)
sun = bpy.data.objects.new("sun", sun_d)
sun.rotation_euler = (math.radians(50), 0.0, math.radians(-35))
COL.objects.link(sun)

scene.render.engine = "CYCLES"
try:
    cp = bpy.context.preferences.addons["cycles"].preferences
    for dt in ("OPTIX", "CUDA"):
        try:
            cp.compute_device_type = dt
            cp.get_devices()
            if any(d.type == dt for d in cp.devices):
                for d in cp.devices:
                    d.use = d.type == dt
                scene.cycles.device = "GPU"
                break
        except Exception:
            pass
except Exception:
    pass
scene.cycles.samples = 160
scene.cycles.use_denoising = True
scene.render.resolution_x = 1600
scene.render.resolution_y = 900
scene.render.image_settings.file_format = "PNG"
scene.render.image_settings.color_mode = "RGB"


# ------------------------------------------------------------------ запекание теней (AO) в атлас кузова
def bake_ao():
    shell = [objs[k] for k in ("body", "door_l", "door_r", "hood", "engine_lid")]
    hidden = list(opened.values()) + [man, bpy.data.objects["floor"]]
    for o in hidden:
        o.hide_render = True
    ao_img = bpy.data.images.new("ao_bake", TX.RES, TX.RES, alpha=False, float_buffer=True)
    ao_img.colorspace_settings.name = "Non-Color"
    ao_img.pixels.foreach_set(np.ones(TX.RES * TX.RES * 4, np.float32))
    dummy = bpy.data.images.new("ao_dummy", 16, 16)
    for m in {m for o in shell for m in o.data.materials}:
        n = m.node_tree.nodes.new("ShaderNodeTexImage")
        n.image = ao_img if m.name == "pv_body" else dummy
        m.node_tree.nodes.active = n
    vl = scene.view_layers[0]
    for o in scene.objects:
        if o is not None:
            o.select_set(False, view_layer=vl)
    for o in shell:
        o.select_set(True, view_layer=vl)
    vl.objects.active = shell[0]
    scene.world.light_settings.distance = 0.6
    old = scene.cycles.samples
    scene.cycles.samples = 96
    t0 = time.time()
    bpy.ops.object.bake(type="AO", margin=8, use_clear=False)
    scene.cycles.samples = old
    print("AO bake %.1fs" % (time.time() - t0))
    ao = np.array(ao_img.pixels[:], np.float32).reshape(TX.RES, TX.RES, 4)[::-1, :, 0]
    for o in hidden:
        o.hide_render = False
    return np.clip(ao, 0, 1)


def apply_ao(ao):
    t = TEX["body"]
    k = 0.30 + 0.70 * ao ** 1.25
    t["co"] = t["co"] * k[..., None]
    t["smdi"][..., 1] *= 0.45 + 0.55 * ao
    t["smdi"][..., 2] *= 0.55 + 0.45 * ao
    t["rough"] = np.clip(1.0 - t["smdi"][..., 2] * 0.92, 0.04, 1.0)
    for name, arr in (("body_co", t["co"]), ("body_r", t["rough"])):
        im = bpy.data.images[name]
        rgba = np.ones((TX.RES, TX.RES, 4), np.float32)
        rgba[..., :3] = arr[..., None] if arr.ndim == 2 else arr
        im.pixels.foreach_set(np.ascontiguousarray(rgba[::-1]).ravel())
    small = (np.clip(ao[::4, ::4], 0, 1))
    im = image("ao_view", small)
    im.filepath_raw = os.path.join(OUT, "ao_kuzova.png")
    im.file_format = "PNG"
    im.save()


ao = bake_ao()
apply_ao(ao)
export_all()


def camera(name, loc, target, ortho=None, lens=45):
    cd = bpy.data.cameras.new(name)
    if ortho:
        cd.type = "ORTHO"
        cd.ortho_scale = ortho
    else:
        cd.lens = lens
    cd.clip_start = 0.05
    cd.clip_end = 300
    cam = bpy.data.objects.new(name, cd)
    COL.objects.link(cam)
    cam.location = loc
    cam.rotation_euler = (Vector(target) - Vector(loc)).to_track_quat("-Z", "Y").to_euler()
    return cam


VIEWS = [
    ("1_bok_levyi", camera("sideL", (-12.0, 0.0, 0.62), (0.0, 0.0, 0.62), ortho=4.9), False, False),
    ("2_bok_pravyi", camera("sideR", (12.0, 0.0, 0.62), (0.0, 0.0, 0.62), ortho=4.9), False, False),
    ("3_34_pered", camera("p34f", (-5.2, -6.8, 2.3), (0.0, -0.3, 0.55), lens=50), False, False),
    ("4_34_zad", camera("p34r", (5.0, 6.4, 2.1), (0.0, 0.3, 0.6), lens=50), False, False),
    ("5_otkryto", camera("open", (4.9, -5.4, 3.3), (0.0, 0.0, 0.75), lens=45), True, False),
    ("6_salon", camera("cabin", (-2.4, -0.15, 1.5), (0.1, -0.05, 0.5), lens=28), True, False),
    ("7_krylo", camera("fender", (-2.4, -2.3, 0.75), (-0.9, -1.25, 0.45), lens=45), False, False),
    ("8_zad_chetvert", camera("quarter", (2.5, 2.4, 0.8), (0.9, 1.3, 0.55), lens=45), False, False),
]
files = []
for tag, cam, is_open, with_man in VIEWS:
    scene.camera = cam
    set_open(is_open)
    man.hide_render = not with_man
    path = os.path.join(OUT, tag + ".png")
    scene.render.filepath = path
    bpy.ops.render.render(write_still=True)
    files.append(path)
    print("RENDER", path)

# атлас кузова для просмотра (уменьшенный)
co = TEX["body"]["co"]
small = co[::2, ::2]
im = image("atlas_view", small)
im.filepath_raw = os.path.join(OUT, "atlas_kuzova.png")
im.file_format = "PNG"
im.save()

cw, ch, cols = 800, 450, 4
rows = (len(files) + cols - 1) // cols
sheet = np.ones((ch * rows, cw * cols, 4), np.float32)
for i, f in enumerate(files):
    img = bpy.data.images.load(f)
    w, h = img.size
    px = np.array(img.pixels[:], np.float32).reshape(h, w, 4)
    sm = px[(np.arange(ch) * h / ch).astype(int)][:, (np.arange(cw) * w / cw).astype(int)]
    r, c = i // cols, i % cols
    y0 = (rows - 1 - r) * ch
    sheet[y0:y0 + ch, c * cw:(c + 1) * cw] = sm
img = bpy.data.images.new("sheet", cw * cols, ch * rows, alpha=False)
img.pixels.foreach_set(sheet.ravel())
img.filepath_raw = os.path.join(OUT, "list_etap3.png")
img.file_format = "PNG"
img.save()
print("DONE", OUT)
