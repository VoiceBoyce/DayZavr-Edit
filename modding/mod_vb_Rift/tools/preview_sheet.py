# -*- coding: utf-8 -*-
"""Обзор всех вариантов VB_Rift одной картинкой (LOD0, ночь, снег).

  blender.exe -b --factory-startup --python preview_sheet.py -- <папка_png> <итог.png>
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
name_filter = argv[2] if len(argv) > 2 else ""      # подстрока имени: показать только часть вариантов
variants = [v for v in G.VARIANTS if name_filter in v[0]]

bpy.ops.wm.read_factory_settings(use_empty=True)
scene = bpy.context.scene
for eng in ("BLENDER_EEVEE", "BLENDER_EEVEE_NEXT"):
    try:
        scene.render.engine = eng
        break
    except TypeError:
        pass
scene.render.resolution_x, scene.render.resolution_y = 2000, 1500
scene.render.filepath = out_path


def img(name, noncolor=False):
    im = bpy.data.images.load(os.path.join(png_dir, name + ".png"))
    if noncolor:
        im.colorspace_settings.name = "Non-Color"
    return im


def make_mat(name, co, e, strength):
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
    return m


m_crust = make_mat("crust", "vb_rift_crust_co", "vb_rift_crust_e_co", 3.0)
m_vein = make_mat("vein", "vb_rift_vein_co", "vb_rift_vein_e_co", 8.0)

COLS, STEP = 4, 17.0
if name_filter:
    STEP = 8.0
bark = bpy.data.materials.new("bark")
bark.use_nodes = True
bark.node_tree.nodes["Principled BSDF"].inputs["Base Color"].default_value = (0.35, 0.33, 0.3, 1)
for idx, (name, kind, seed) in enumerate(variants):
    sc = G.variant_scene(kind, seed)
    mesh = G.mesh_lod(sc, G.LOD_PARAMS[0])
    P, T, UV, M = mesh["P"], mesh["T"], mesh["UV"], mesh["M"]
    me = bpy.data.meshes.new(name)
    me.from_pydata(P.tolist(), [], T.tolist())
    uvl = me.uv_layers.new(name="UV")
    uvl.data.foreach_set("uv", UV.reshape(-1).tolist())
    me.polygons.foreach_set("material_index", [0 if m == "crust" else 1 for m in M])
    me.polygons.foreach_set("use_smooth", [True] * len(M))
    me.materials.append(m_crust)
    me.materials.append(m_vein)
    ob = bpy.data.objects.new(name, me)
    col, row = idx % COLS, idx // COLS
    ob.location = (col * STEP, -row * STEP, 0)
    scene.collection.objects.link(ob)
    if kind.startswith("wrap:"):        # ствол для наглядности
        R = float(kind.split(":")[1])
        bpy.ops.mesh.primitive_cylinder_add(radius=R, depth=5.0, location=(col * STEP, -row * STEP, 2.5))
        bpy.context.active_object.data.materials.append(bark)
    if kind.startswith("hang:"):        # поднять, чтобы щупальца висели над землёй
        ob.location = (col * STEP, -row * STEP, 5.5)
    # подпись
    cu = bpy.data.curves.new(name + "_t", "FONT")
    cu.body = name.replace("vb_rift_", "")
    cu.size = 1.1
    to = bpy.data.objects.new(name + "_t", cu)
    to.location = (col * STEP - 6.5, -row * STEP - 7.6, 0.05)
    tm = bpy.data.materials.new(name + "_tm")
    tm.use_nodes = True
    tm.node_tree.nodes["Principled BSDF"].inputs["Emission Color"].default_value = (1, 1, 1, 1)
    tm.node_tree.nodes["Principled BSDF"].inputs["Emission Strength"].default_value = 1.5
    cu.materials.append(tm)
    scene.collection.objects.link(to)

cx = (COLS - 1) * STEP / 2
rows = (len(variants) - 1) // COLS + 1
cy = -(rows - 1) * STEP / 2
bpy.ops.mesh.primitive_plane_add(size=120, location=(cx, cy, 0))
gm = bpy.data.materials.new("snow")
gm.use_nodes = True
gb = gm.node_tree.nodes["Principled BSDF"]
gb.inputs["Base Color"].default_value = (0.75, 0.78, 0.83, 1)
gb.inputs["Roughness"].default_value = 0.85
bpy.context.active_object.data.materials.append(gm)

world = bpy.data.worlds.new("w")
scene.world = world
world.use_nodes = True
world.node_tree.nodes["Background"].inputs["Color"].default_value = (0.01, 0.012, 0.02, 1)
sun = bpy.data.lights.new("moon", "SUN")
sun.energy = 0.25
sun.color = (0.6, 0.7, 1.0)
so = bpy.data.objects.new("moon", sun)
so.rotation_euler = (math.radians(40), 0, math.radians(30))
scene.collection.objects.link(so)

cd = bpy.data.cameras.new("cam")
cd.type = "ORTHO"
cd.ortho_scale = max(COLS * STEP, rows * STEP * 4 / 3) + 2
co = bpy.data.objects.new("cam", cd)
if name_filter:     # сбоку под углом, чтобы были видны высота обвивок и длина щупалец
    co.location = (cx, cy - 60, 30)
    co.rotation_euler = (math.radians(62), 0, 0)
else:
    co.location = (cx, cy - 22, 60)
    co.rotation_euler = (math.radians(20), 0, 0)
scene.collection.objects.link(co)
scene.camera = co
bpy.ops.render.render(write_still=True)
print("SHEET", out_path)
