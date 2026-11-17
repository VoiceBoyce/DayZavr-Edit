# -*- coding: utf-8 -*-
r"""Echo Runner, этап 2: детали «самосбора», части машины, рендеры (закрыто / открыто / салон / колесо).

  G:\SteamLibrary\steamapps\common\Blender\blender.exe -b --factory-startup --python er_stage2.py -- <папка вывода>
"""
import math
import os
import sys

import bpy
import numpy as np
from mathutils import Vector

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import er_model as M  # noqa: E402

ARGS = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
OUT = os.path.abspath(ARGS[0]) if ARGS else os.path.abspath("er_stage2_out")
os.makedirs(OUT, exist_ok=True)

bpy.ops.wm.read_factory_settings(use_empty=True)
scene = bpy.context.scene
COL = scene.collection

objs, pivots = M.build_all()
total = 0
for k, o in objs.items():
    nm, bd, tris = M.integrity(o)
    total += tris
    print("PART %-11s tris=%6d non_manifold=%d boundary=%d" % (k, tris, nm, bd))
print("TOTAL tris=%d" % total)
print("PIVOTS", {k: (tuple(round(c, 3) for c in v[0]), v[1]) for k, v in pivots.items()})

opened = {
    "door_l": M.opened_copy(objs["door_l"], pivots["door_l"], 72),
    "door_r": M.opened_copy(objs["door_r"], pivots["door_r"], 72),
    "hood": M.opened_copy(objs["hood"], pivots["hood"], 55),
    "engine_lid": M.opened_copy(objs["engine_lid"], pivots["engine_lid"], 48),
}


for k, o in opened.items():
    xs = [v.co.x for v in o.data.vertices]
    zs = [v.co.z for v in o.data.vertices]
    ys = [v.co.y for v in o.data.vertices]
    print("OPEN %-10s x[%.2f..%.2f] y[%.2f..%.2f] z[%.2f..%.2f]" % (k, min(xs), max(xs), min(ys), max(ys),
                                                                  min(zs), max(zs)))


def set_open(state):
    for k, o in opened.items():
        o.hide_render = not state
        objs[k].hide_render = state


# ------------------------------------------------------------------ человек 1,8 м, пол, свет
from er_model import Part, lathe  # noqa: E402

pm = Part("man_1_8m")
M.PALETTE["man"] = dict(color=(0.30, 0.32, 0.36), rough=0.7)
lathe(pm, (1.55, -3.0, 0.0), (0, 0, 1), [(0.0001, 0.0), (0.14, 0.0), (0.16, 0.85), (0.20, 1.30), (0.17, 1.45),
                                          (0.07, 1.52), (0.10, 1.60), (0.10, 1.72), (0.06, 1.79), (0.0001, 1.80)],
      "man", 24, closed=False)
man = pm.finish(60)

M.PALETTE["ground"] = dict(color=(0.55, 0.56, 0.58), rough=0.9)
pg = Part("floor")
v = [pg.bm.verts.new(c) for c in ((-400, -400, 0), (400, -400, 0), (400, 400, 0), (-400, 400, 0))]
pg.bm.faces.new(v).material_index = pg.mi("ground")
pg.finish()

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

# ------------------------------------------------------------------ рендер
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
scene.cycles.samples = 128
scene.cycles.use_denoising = True
scene.render.resolution_x = 1600
scene.render.resolution_y = 900
scene.render.image_settings.file_format = "PNG"
scene.render.image_settings.color_mode = "RGB"


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
    ("1_bok", camera("side", (12.0, -0.6, 0.62), (0.0, -0.6, 0.62), ortho=6.6), False, True),
    ("2_34_pered", camera("p34f", (-5.2, -6.8, 2.3), (0.0, -0.3, 0.55), lens=50), False, False),
    ("3_34_zad", camera("p34r", (5.0, 6.4, 2.1), (0.0, 0.3, 0.6), lens=50), False, False),
    ("4_pered", camera("front", (0.3, -12.0, 0.75), (0.3, 0.0, 0.75), ortho=3.9), False, True),
    ("5_zad", camera("rear", (0.3, 12.0, 0.75), (0.3, 0.0, 0.75), ortho=3.9), False, True),
    ("6_otkryto", camera("open", (4.9, -5.4, 3.3), (0.0, 0.0, 0.75), lens=45), True, False),
    ("7_salon", camera("cabin", (-2.4, -0.15, 1.5), (0.1, -0.05, 0.5), lens=28), True, False),
    ("8_koleso", camera("wheel", (-2.3, -2.5, 0.62), (-0.85, -1.0, 0.36), lens=45), False, False),
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

cw, ch, cols = 800, 450, 4
rows = (len(files) + cols - 1) // cols
sheet = np.ones((ch * rows, cw * cols, 4), np.float32)
for i, f in enumerate(files):
    im = bpy.data.images.load(f)
    w, h = im.size
    px = np.array(im.pixels[:], np.float32).reshape(h, w, 4)
    sm = px[(np.arange(ch) * h / ch).astype(int)][:, (np.arange(cw) * w / cw).astype(int)]
    r, c = i // cols, i % cols
    y0 = (rows - 1 - r) * ch
    sheet[y0:y0 + ch, c * cw:(c + 1) * cw] = sm
img = bpy.data.images.new("sheet", cw * cols, ch * rows, alpha=False)
img.pixels.foreach_set(sheet.ravel())
img.filepath_raw = os.path.join(OUT, "list_etap2.png")
img.file_format = "PNG"
img.save()
print("DONE", OUT)
