# -*- coding: utf-8 -*-
r"""Echo Runner: проверка посадки — манекены в позе водителя/пассажира в точках SEAT.

  blender.exe -b --factory-startup --python er_check_seat.py -- <папка вывода>

Манекен ≈ поза сидящего DayZ: торс 0.56 м с наклоном назад 15°, голова сверху (макушка ~0.79 над бедром),
бёдра вперёд 0.45, голени вниз 0.42, руки к рулю. Это грубая оценка — окончательно посадку судим в игре.
"""
import math
import os
import sys

import bpy
from mathutils import Vector

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import er_model as M  # noqa: E402

ARGS = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
OUT = os.path.abspath(ARGS[0]) if ARGS else os.path.abspath("seat_check")
os.makedirs(OUT, exist_ok=True)
bpy.ops.wm.read_factory_settings(use_empty=True)
scene = bpy.context.scene
objs, pivots = M.build_all()
M.PALETTE["man"] = dict(color=(0.85, 0.35, 0.1), rough=0.6)

p = M.Part("dummies")


def at(H, fwd, up, side=0.0):
    dy, dz = M.crew_offset(fwd, up)
    return H + Vector((side, dy, dz))


for key, h in M.SEAT.items():
    H = Vector(h)
    N = at(H, -0.15, 0.54)
    M.tube(p, [H, N], 0.13, "man", 12, round_corners=0)
    M.lathe(p, at(H, -0.17, 0.68), (0, 0, 1), [(0.0001, -0.11), (0.08, -0.08), (0.11, 0.0), (0.08, 0.08),
                                               (0.0001, 0.11)], "man", 16, closed=False)
    for s in (1, -1):
        M.tube(p, [at(H, 0, 0, s * 0.1), at(H, 0.45, 0.05, s * 0.1), at(H, 0.55, -0.37, s * 0.1)], 0.065, "man", 10,
               round_corners=0.02)
        if key == "driver":
            S = at(H, -0.12, 0.47, s * 0.18)
            hand = Vector(M.STEER_C) + Vector((s * 0.15, 0.03, 0.02))
            M.tube(p, [S, (S + hand) / 2 + Vector((0, 0, -0.08)), hand], 0.045, "man", 8, round_corners=0.03)
dummies = p.finish()
top = max(v.co.z for v in dummies.data.vertices)
print("HEAD TOP z=%.3f  roof outer=%.3f" % (top, max(v.co.z for v in objs["body"].data.vertices)))

world = bpy.data.worlds.new("w")
bg = next(n for n in world.node_tree.nodes if n.type == "BACKGROUND")
bg.inputs["Color"].default_value = (0.62, 0.66, 0.72, 1)
scene.world = world
sun = bpy.data.objects.new("sun", bpy.data.lights.new("sun", "SUN"))
sun.data.energy = 3.0
sun.rotation_euler = (math.radians(50), 0, math.radians(-35))
scene.collection.objects.link(sun)
scene.render.engine = "CYCLES"
scene.cycles.samples = 48
scene.cycles.use_denoising = True
scene.render.resolution_x, scene.render.resolution_y = 1200, 800


def cam(name, loc, target, lens=35):
    c = bpy.data.objects.new(name, bpy.data.cameras.new(name))
    c.data.lens = lens
    scene.collection.objects.link(c)
    c.location = loc
    c.rotation_euler = (Vector(target) - Vector(loc)).to_track_quat("-Z", "Y").to_euler()
    return c


objs["door_r"].hide_render = True                  # дверь водителя (+X) снята, чтобы видеть водителя
for tag, c in (("posadka_bok", cam("s", (4.2, 0.0, 0.75), (0.3, 0.0, 0.6))),
               ("posadka_pered", cam("f", (0.0, -4.5, 1.05), (0.0, 0.0, 0.85), 40))):
    scene.camera = c
    scene.render.filepath = os.path.join(OUT, tag + ".png")
    bpy.ops.render.render(write_still=True)
print("DONE")
