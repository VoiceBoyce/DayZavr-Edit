# -*- coding: utf-8 -*-
r"""Рендер концептов устройств базы (MLOD из eb_models.py): день и ночь, квадратный кадр с запасом.

  blender.exe -b --factory-startup --python eb_render.py -- <папка p3d> <папка вывода> [--only=wind,solar]
      [--size=1000] [--samples=48] [--azim=-35] [--elev=22]
Выход: <папка вывода>/<имя>_day.png и <имя>_night.png. Материалы и оси — er_common.py Echo Runner.
"""
import math
import os
import sys

import bpy
import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.abspath(os.path.join(HERE, "..", "..", "mods_vb_vehicles", "mod_vb_EchoRunner", "tools")))
import er_common as C  # noqa: E402

C.MAXEDGE = 0          # у машины длинные грани отсекаются как прокси; мачты и плиты устройств длиннее 1,5 м — нужны
# vb_echobase\data\... -> mod_vb_Echo\VB_EchoBase\data; вложения станции: бак Echo Runner и аккумулятор Эхо
C.set_roots([os.path.abspath(os.path.join(HERE, "..")),
             os.path.abspath(os.path.join(HERE, "..", "..", "mods_vb_vehicles", "mod_vb_EchoRunner")),
             os.path.abspath(os.path.join(HERE, "..", "..", "mods_vb_vehicles", "mod_vb_EchoVehicleParts"))])

args = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
opts = {a.split("=", 1)[0][2:]: (a.split("=", 1)[1] if "=" in a else True) for a in args if a.startswith("--")}
pos = [a for a in args if not a.startswith("--")]
SRC, OUTDIR = pos[0], pos[1]
ONLY = set(str(opts.get("only", "")).split(",")) - {""}
SIZE = int(opts.get("size", 1000))
SAMPLES = int(opts.get("samples", 48))
AZIM = math.radians(float(opts.get("azim", -35)))
ELEV = math.radians(float(opts.get("elev", 22)))
PROXIES = str(opts.get("proxies", ""))         # папка с моделями для прокси (имя файла = имя прокси)
os.makedirs(OUTDIR, exist_ok=True)


def setup_scene():
    bpy.ops.wm.read_factory_settings(use_empty=True)
    C._mats.clear()        # кэши er_common держат материалы и картинки старой сцены — после сброса они удалены
    C._img.clear()
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
    sc.render.resolution_x = SIZE
    sc.render.resolution_y = SIZE
    for vt in ("AgX", "Filmic"):
        try:
            sc.view_settings.view_transform = vt
            break
        except TypeError:
            pass
    return sc


def used_verts(o):
    used = sorted({vi for p in o.data.polygons for vi in p.vertices})
    return np.array([tuple(o.data.vertices[i].co) for i in used]) if used else np.zeros((0, 3))


for fn in sorted(os.listdir(SRC)):
    if not fn.endswith(".p3d"):
        continue
    name = fn[3:-4] if fn.startswith("eb_") else fn[:-4]
    if ONLY and name not in ONLY:
        continue
    for mode in ("day", "night"):
        sc = setup_scene()
        objs, _ = C.load_car(os.path.join(SRC, fn), PROXIES, 1.0, True)
        allv = np.concatenate([used_verts(o) for o in objs.values()])
        lo, hi = allv.min(0), allv.max(0)
        center = (lo + hi) / 2
        R = float(np.linalg.norm(hi - lo)) / 2
        bpy.ops.mesh.primitive_plane_add(size=max(40, R * 20), location=(center[0], center[1], float(lo[2]) + 0.001))
        bpy.context.active_object.is_shadow_catcher = True
        world = bpy.data.worlds.new("w")
        world.use_nodes = True
        bg = world.node_tree.nodes["Background"]
        if mode == "day":
            bg.inputs["Color"].default_value = (0.42, 0.44, 0.47, 1)
            bg.inputs["Strength"].default_value = 0.9
        else:
            bg.inputs["Color"].default_value = (0.05, 0.06, 0.09, 1)
            bg.inputs["Strength"].default_value = 0.3
        sc.world = world
        sun = bpy.data.objects.new("sun", bpy.data.lights.new("sun", "SUN"))
        sun.data.energy = 3.2 if mode == "day" else 0.1
        sun.data.angle = math.radians(8)
        sun.rotation_euler = (math.radians(40), math.radians(-25), math.radians(-35))
        sc.collection.objects.link(sun)
        tgt = bpy.data.objects.new("target", None)
        tgt.location = tuple(center)
        sc.collection.objects.link(tgt)
        cd = bpy.data.cameras.new("cam")
        cd.lens = 50
        cd.sensor_width = 36
        cam = bpy.data.objects.new("cam", cd)
        sc.collection.objects.link(cam)
        sc.camera = cam
        tc = cam.constraints.new("TRACK_TO")
        tc.target = tgt
        tc.track_axis = "TRACK_NEGATIVE_Z"
        tc.up_axis = "UP_Y"
        dist = R / math.sin(math.atan(18.0 / 50.0)) * 1.12
        d = np.array([math.sin(AZIM) * math.cos(ELEV), -math.cos(AZIM) * math.cos(ELEV), math.sin(ELEV)])
        cam.location = tuple(center + d * dist)
        cd.clip_start = 0.02
        cd.clip_end = dist * 10
        sc.render.filepath = os.path.join(OUTDIR, "%s_%s.png" % (name, mode))
        bpy.ops.render.render(write_still=True)
        print("RENDERED", sc.render.filepath)
