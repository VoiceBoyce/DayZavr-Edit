# -*- coding: utf-8 -*-
"""Превью «Жил Изнанки» в Blender: та же геометрия, что в .p3d (LOD0), ночь, снег, красный свет.

  blender.exe -b --factory-startup --python preview_blender.py -- <папка_png> <итог.png> [ночь|сумерки] [ракурс]
Только для визуальной проверки формы, это не то, как DayZ рисует материал.
"""
import math
import os
import sys

import bpy

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import gen_rift_veins as G  # noqa: E402

argv = sys.argv[sys.argv.index("--") + 1:]
png_dir, out_path = argv[0], argv[1]
mode = argv[2] if len(argv) > 2 else "night"
view = int(argv[3]) if len(argv) > 3 else 0

bpy.ops.wm.read_factory_settings(use_empty=True)
scene = bpy.context.scene
for eng in ("BLENDER_EEVEE", "BLENDER_EEVEE_NEXT"):
    try:
        scene.render.engine = eng
        break
    except TypeError:
        pass
scene.render.resolution_x, scene.render.resolution_y = 1600, 900
scene.render.filepath = out_path

sc = G.build_scene()
mesh = G.mesh_lod(sc, G.LOD_PARAMS[0])
P, T, UV, M = mesh["P"], mesh["T"], mesh["UV"], mesh["M"]
me = bpy.data.meshes.new("veins")
me.from_pydata(P.tolist(), [], T.tolist())
uvl = me.uv_layers.new(name="UV")
uvl.data.foreach_set("uv", UV.reshape(-1).tolist())
me.polygons.foreach_set("material_index", [0 if m == "crust" else 1 for m in M])
me.polygons.foreach_set("use_smooth", [True] * len(M))
obj = bpy.data.objects.new("veins", me)
scene.collection.objects.link(obj)

k = 1.0 if mode == "night" else 0.6


def img(name, noncolor=False):
    im = bpy.data.images.load(os.path.join(png_dir, name + ".png"))
    if noncolor:
        im.colorspace_settings.name = "Non-Color"
    return im


def make_mat(name, co, e, nohq, strength):
    m = bpy.data.materials.new(name)
    m.use_nodes = True
    nt = m.node_tree
    bsdf = nt.nodes["Principled BSDF"]
    t_co = nt.nodes.new("ShaderNodeTexImage")
    t_co.image = img(co)
    nt.links.new(t_co.outputs["Color"], bsdf.inputs["Base Color"])
    t_e = nt.nodes.new("ShaderNodeTexImage")
    t_e.image = img(e)
    nt.links.new(t_e.outputs["Color"], bsdf.inputs["Emission Color"])
    bsdf.inputs["Emission Strength"].default_value = strength
    bsdf.inputs["Roughness"].default_value = 0.45
    if nohq:
        t_n = nt.nodes.new("ShaderNodeTexImage")
        t_n.image = img(nohq, True)
        nm = nt.nodes.new("ShaderNodeNormalMap")
        nt.links.new(t_n.outputs["Color"], nm.inputs["Color"])
        nt.links.new(nm.outputs["Normal"], bsdf.inputs["Normal"])
    return m


me.materials.append(make_mat("crust", "vb_rift_crust_co", "vb_rift_crust_e_co", "vb_rift_crust_nohq", 3.0 * k))
me.materials.append(make_mat("vein", "vb_rift_vein_co", "vb_rift_vein_e_co", None, 8.0 * k))

# снег
bpy.ops.mesh.primitive_plane_add(size=40, location=(0, 0, 0))
ground = bpy.context.active_object
gm = bpy.data.materials.new("snow")
gm.use_nodes = True
gb = gm.node_tree.nodes["Principled BSDF"]
gb.inputs["Base Color"].default_value = (0.78, 0.8, 0.85, 1)
gb.inputs["Roughness"].default_value = 0.85
ground.data.materials.append(gm)

world = bpy.data.worlds.new("w")
scene.world = world
world.use_nodes = True
bg = world.node_tree.nodes["Background"]
if mode == "night":
    bg.inputs["Color"].default_value = (0.006, 0.007, 0.012, 1)
    sun_e, sun_c = 0.05, (0.6, 0.7, 1.0)
else:
    bg.inputs["Color"].default_value = (0.05, 0.035, 0.045, 1)
    sun_e, sun_c = 0.6, (1.0, 0.6, 0.55)
bg.inputs["Strength"].default_value = 1.0

sun = bpy.data.lights.new("moon", "SUN")
sun.energy = sun_e
sun.color = sun_c
so = bpy.data.objects.new("moon", sun)
so.rotation_euler = (math.radians(55), 0, math.radians(30))
scene.collection.objects.link(so)

lt = bpy.data.lights.new("rift", "POINT")
lt.energy = 250
lt.color = (1.0, 0.12, 0.2)
lt.shadow_soft_size = 0.3
lo = bpy.data.objects.new("rift", lt)
lo.location = (0, 0, float(sc["top"][2]) + 0.45)
scene.collection.objects.link(lo)

cams = [((6.0, -7.0, 3.4), 30), ((2.2, -2.6, 1.1), 24), ((0.3, -0.4, 14.0), 35)]
pos, lens = cams[view]
cd = bpy.data.cameras.new("cam")
cd.lens = lens
co = bpy.data.objects.new("cam", cd)
co.location = pos
scene.collection.objects.link(co)
tgt = bpy.data.objects.new("tgt", None)
tgt.location = (0, 0, 0.2)
scene.collection.objects.link(tgt)
c = co.constraints.new("TRACK_TO")
c.target = tgt
c.track_axis = "TRACK_NEGATIVE_Z"
c.up_axis = "UP_Y"
scene.camera = co

bpy.ops.render.render(write_still=True)
print("PREVIEW", out_path)
