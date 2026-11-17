# -*- coding: utf-8 -*-
r"""Рендер машины из MLOD: кузов + детали-прокси (колёса, двери, капот), несколько видов (Blender, Cycles).

  blender.exe -b --factory-startup --python er_render.py -- <car.p3d> <папка вывода>
      --proxydir=<папка с MLOD деталей> [--root=<корень текстур>[;<ещё>]]
      [--views=f34,r34,side,side2,front,rear,top] [--size=1400] [--samples=48] [--lod=1]
      [--extra=<доп.p3d>[;...]]  (детали в координатах машины, без прокси: черновики навесных)
Оси и прокси — см. er_common.py.
"""
import math
import os
import sys

import bpy
import numpy as np

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import er_common as C  # noqa: E402

args = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
opts = {a.split("=", 1)[0][2:]: (a.split("=", 1)[1] if "=" in a else True) for a in args if a.startswith("--")}
pos = [a for a in args if not a.startswith("--")]
MODEL, OUTDIR = pos[0], pos[1]
os.makedirs(OUTDIR, exist_ok=True)
if opts.get("root"):
    C.set_roots(str(opts["root"]).split(";"))
SIZE = int(opts.get("size", 1400))
SAMPLES = int(opts.get("samples", 48))
VIEWS = str(opts.get("views", "f34,r34,side,side2,front,rear,top")).split(",")

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

extra = [e for e in str(opts.get("extra", "")).split(";") if e]
objs, _ = C.load_car(MODEL, opts.get("proxydir", ""), float(opts.get("lod", 1)), True, extra)
objs = list(objs.values())


def used_verts(o):
    used = sorted({vi for p in o.data.polygons for vi in p.vertices})
    return np.array([tuple(o.data.vertices[i].co) for i in used]) if used else np.zeros((0, 3))


allv = np.concatenate([used_verts(o) for o in objs])
lo, hi = allv.min(0), allv.max(0)
center = (lo + hi) / 2
dims = hi - lo
print("DIMS (Blender x,y,z)", [round(float(d), 3) for d in dims], "low z", round(float(lo[2]), 3))

bpy.ops.mesh.primitive_plane_add(size=40, location=(center[0], center[1], float(lo[2]) + 0.002))
bpy.context.active_object.is_shadow_catcher = True

world = bpy.data.worlds.new("w")
world.use_nodes = True
world.node_tree.nodes["Background"].inputs["Color"].default_value = (0.42, 0.44, 0.47, 1)
world.node_tree.nodes["Background"].inputs["Strength"].default_value = float(opts.get("world", 0.9))
scene.world = world
sun = bpy.data.objects.new("sun", bpy.data.lights.new("sun", "SUN"))
sun.data.energy = float(opts.get("sun", 3.2))
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
    "f34r": ((-0.80, -1.0, 0.38), False),
    "r34": ((-0.80, 1.0, 0.38), False),
    "r34l": ((0.80, 1.0, 0.38), False),
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
