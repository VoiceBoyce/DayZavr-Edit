# -*- coding: utf-8 -*-
"""Студийный рендер одного материала аномалии VB_AnomaliesBosses в PNG (Blender, Cycles).

Запуск:
  blender.exe -b --factory-startup --python render_mat.py -- <Ключ> [выход.png] [--size=1600] [--samples=256] [--transparent]
Геометрия — LOD0 из tools/ab_materials.py (та же, что уходит в .p3d), текстуры — TGA, распакованные
из настоящих .paa мода (ImageToPAA), материал повторяет vb_ab_mat.rvmat: _co, _nohq (DirectX, G инвертирован),
_smdi (G — блик, B — глянец), Stage8 _e_co x emmisive 2.0.
"""
import math
import os
import sys

import bpy

HERE = os.path.dirname(os.path.abspath(__file__))
TEX = os.path.join(HERE, "tex")
TOOLS = os.path.join("D:" + chr(92), "DayZavr", "DayZavr-Edit", "modding", "mod_vb_AnomaliesBosses", "tools")

args = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
opts = {a.split("=", 1)[0][2:]: (a.split("=", 1)[1] if "=" in a else True) for a in args if a.startswith("--")}
pos = [a for a in args if not a.startswith("--")]
KEY = pos[0] if pos else "Electra"
OUT = pos[1] if len(pos) > 1 else os.path.join(HERE, "vb_ab_mat_%s.png" % KEY.lower())
SIZE = int(opts.get("size", 1600))
SAMPLES = int(opts.get("samples", 256))
TRANSPARENT = bool(opts.get("transparent", False))

sys.path.insert(0, TOOLS)
import ab_materials as A  # noqa: E402

m = next(x for x in A.MATS if x["key"] == KEY)
P, T, UV = A.lod_meshes(m)[0]
Q = P.copy()
if A.RF[m["shape"]] == 17:                       # лежит плашмя, как в игре
    import numpy as np
    Q = np.column_stack([Q[:, 0], -Q[:, 2], Q[:, 1]])
Q[:, 2] -= Q[:, 2].min()
lo, hi = Q.min(0), Q.max(0)
dims = hi - lo
s = float(max(dims))
center = ((lo[0] + hi[0]) / 2, (lo[1] + hi[1]) / 2, (lo[2] + hi[2]) / 2)
print("ITEM", KEY, m["ru"], "dims, m:", [round(float(d), 4) for d in dims], "tris:", len(T))

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
            devs = [d for d in cp.devices if d.type == dt]
            if devs:
                for d in cp.devices:
                    d.use = d.type == dt
                gpu = True
                print("DEVICE", dt, [d.name for d in devs])
                break
        except Exception as e:
            print("device", dt, "fail", e)
except Exception as e:
    print("no cycles prefs", e)
scene.cycles.device = "GPU" if gpu else "CPU"
scene.cycles.samples = SAMPLES
scene.cycles.use_denoising = True
scene.cycles.max_bounces = 8
scene.render.resolution_x = scene.render.resolution_y = SIZE
scene.render.resolution_percentage = 100
scene.render.film_transparent = TRANSPARENT
scene.render.image_settings.file_format = "PNG"
scene.render.image_settings.color_mode = "RGBA" if TRANSPARENT else "RGB"
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


def img(name, noncolor=False):
    im = bpy.data.images.load(os.path.join(TEX, name + ".tga"))
    if noncolor:
        im.colorspace_settings.name = "Non-Color"
    return im


# ---------------------------------------------------------------- материал (как vb_ab_mat.rvmat)
mat = bpy.data.materials.new("vb_ab_mat")
mat.use_nodes = True
nt = mat.node_tree
N, L = nt.nodes, nt.links
bsdf = N["Principled BSDF"]
uvn = N.new("ShaderNodeUVMap")
uvn.uv_map = "UV"


def tex(name, noncolor=False):
    t = N.new("ShaderNodeTexImage")
    t.image = img(name, noncolor)
    t.interpolation = "Cubic"
    L.new(uvn.outputs["UV"], t.inputs["Vector"])
    return t


t_co = tex("vb_ab_mat_co")
L.new(t_co.outputs["Color"], bsdf.inputs["Base Color"])

t_e = tex("vb_ab_mat_e_co")
L.new(t_e.outputs["Color"], bsdf.inputs["Emission Color"])
bsdf.inputs["Emission Strength"].default_value = A.EMISSIVE * float(opts.get("glow", 1.0))

t_n = tex("vb_ab_mat_nohq", True)
sep = N.new("ShaderNodeSeparateColor")
L.new(t_n.outputs["Color"], sep.inputs["Color"])
inv = N.new("ShaderNodeMath")
inv.operation = "SUBTRACT"
inv.inputs[0].default_value = 1.0
L.new(sep.outputs["Green"], inv.inputs[1])
comb = N.new("ShaderNodeCombineColor")
L.new(sep.outputs["Red"], comb.inputs["Red"])
L.new(inv.outputs["Value"], comb.inputs["Green"])
L.new(sep.outputs["Blue"], comb.inputs["Blue"])
nm = N.new("ShaderNodeNormalMap")
nm.uv_map = "UV"
L.new(comb.outputs["Color"], nm.inputs["Color"])
L.new(nm.outputs["Normal"], bsdf.inputs["Normal"])

t_s = tex("vb_ab_mat_smdi", True)
sps = N.new("ShaderNodeSeparateColor")
L.new(t_s.outputs["Color"], sps.inputs["Color"])
rough = N.new("ShaderNodeMapRange")              # глянец B: 0 -> шероховато, 1 -> зеркально
rough.inputs["From Min"].default_value = 0.0
rough.inputs["From Max"].default_value = 1.0
rough.inputs["To Min"].default_value = 0.85
rough.inputs["To Max"].default_value = 0.08
L.new(sps.outputs["Blue"], rough.inputs["Value"])
L.new(rough.outputs["Result"], bsdf.inputs["Roughness"])
L.new(sps.outputs["Green"], bsdf.inputs["Specular IOR Level"])

# ---------------------------------------------------------------- предмет
me = bpy.data.meshes.new(KEY)
me.from_pydata(Q.tolist(), [], T.tolist())
uvl = me.uv_layers.new(name="UV")
fl = UV.copy()
fl[..., 1] = 1 - fl[..., 1]                      # в p3d v сверху вниз
uvl.data.foreach_set("uv", fl.reshape(-1).tolist())
me.polygons.foreach_set("use_smooth", [True] * len(T))
me.materials.append(mat)
ob = bpy.data.objects.new(KEY, me)
scene.collection.objects.link(ob)

# ---------------------------------------------------------------- фон-«циклорама»
bd_mat = bpy.data.materials.new("backdrop")
bd_mat.use_nodes = True
bb = bd_mat.node_tree.nodes["Principled BSDF"]
bb.inputs["Base Color"].default_value = (0.016, 0.017, 0.02, 1)
bb.inputs["Roughness"].default_value = 0.42
W, D, R, H = 14 * s, 8 * s, 3 * s, 8 * s        # ширина, глубина пола, радиус изгиба, высота стены
prof = [(-D + D * i / 11.0, 0.0) for i in range(11)]
for i in range(17):
    a = math.pi / 2 * i / 16.0
    prof.append((math.sin(a) * R, R - math.cos(a) * R))
for i in range(1, 9):
    prof.append((R, R + H * i / 8.0))
yoff = 2.5 * s                                   # стена позади предмета
verts, faces = [], []
for x in (-W / 2, W / 2):
    for (y, z) in prof:
        verts.append((x + center[0], y + yoff + center[1], z))
n = len(prof)
for i in range(n - 1):
    faces.append((i, i + 1, n + i + 1, n + i))
bm = bpy.data.meshes.new("backdrop")
bm.from_pydata(verts, [], faces)
bm.polygons.foreach_set("use_smooth", [True] * len(faces))
bm.materials.append(bd_mat)
bo = bpy.data.objects.new("backdrop", bm)
if not TRANSPARENT:                              # на прозрачном фоне — только сам предмет, без пола и тени
    scene.collection.objects.link(bo)

world = bpy.data.worlds.new("w")
world.use_nodes = True
world.node_tree.nodes["Background"].inputs["Color"].default_value = (0.006, 0.007, 0.009, 1)
world.node_tree.nodes["Background"].inputs["Strength"].default_value = 1.0
scene.world = world


# ---------------------------------------------------------------- свет
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
    return lo_


k = (s / 0.05) ** 2                              # мощность под масштаб предмета (подобрано под 5 см)
glow = tuple(m.get("glow", (0.5, 0.7, 1.0)))
area("key", (-3.0 * s, -3.2 * s, 3.6 * s), 1.1 * k * float(opts.get("key", 1.0)), 2.5 * s, (1.0, 0.95, 0.88))
area("fill", (3.6 * s, -2.4 * s, 1.4 * s), 0.18 * k, 3.0 * s, (0.85, 0.9, 1.0))
area("rim", (1.2 * s, 3.0 * s, 2.8 * s), 1.6 * k, 1.6 * s, glow)

# ---------------------------------------------------------------- камера
cam_d = bpy.data.cameras.new("cam")
cam_d.lens = 85
cam_d.sensor_width = 36
r = 0.5 * math.sqrt(float(dims[0]) ** 2 + float(dims[1]) ** 2 + float(dims[2]) ** 2)
half_fov = math.atan(18.0 / 85.0)
dist = r / math.sin(half_fov) * float(opts.get("frame", 1.25))
elev, azim = math.radians(float(opts.get("elev", 22))), math.radians(float(opts.get("azim", -28)))
cam = bpy.data.objects.new("cam", cam_d)
cam.location = (center[0] + dist * math.cos(elev) * math.sin(azim),
                center[1] - dist * math.cos(elev) * math.cos(azim),
                center[2] + dist * math.sin(elev))
tgt = bpy.data.objects.new("target", None)
tgt.location = (center[0], center[1], center[2] * 0.9)
scene.collection.objects.link(tgt)
tc = cam.constraints.new("TRACK_TO")
tc.target = tgt
tc.track_axis = "TRACK_NEGATIVE_Z"
tc.up_axis = "UP_Y"
cam_d.dof.use_dof = not TRANSPARENT
cam_d.dof.focus_object = tgt
cam_d.dof.aperture_fstop = float(opts.get("fstop", 8.0))
scene.collection.objects.link(cam)
scene.camera = cam



def add_bloom():
    """Лёгкий ореол вокруг светящихся прожилок (композитор Blender 5.x; при ошибке рендер идёт без него)."""
    ng = bpy.data.node_groups.new("comp", "CompositorNodeTree")
    ng.interface.new_socket("Image", in_out="OUTPUT", socket_type="NodeSocketColor")
    rl = ng.nodes.new("CompositorNodeRLayers")
    gl = ng.nodes.new("CompositorNodeGlare")
    out = ng.nodes.new("NodeGroupOutput")
    for name, val in (("Type", "Bloom"), ("Quality", "High")):
        if name in gl.inputs:
            gl.inputs[name].default_value = val
    for name, val in (("Threshold", 0.9), ("Strength", 0.55), ("Size", 0.55)):
        if name in gl.inputs:
            gl.inputs[name].default_value = val
    ng.links.new(rl.outputs["Image"], gl.inputs["Image"])
    ng.links.new(gl.outputs["Image"], out.inputs[0])
    scene.compositing_node_group = ng
    print("BLOOM on:", [i.name for i in gl.inputs])


if not opts.get("nobloom") and not TRANSPARENT:
    try:
        add_bloom()
    except Exception as e:
        print("BLOOM skipped:", repr(e))

bpy.ops.render.render(write_still=True)
print("RENDERED", OUT)


def crop_to_object(path, margin=0.04):
    """Прозрачный PNG: обрезать по силуэту (альфа > 0) до квадрата с отступом, предмет по центру."""
    import numpy as np
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
    print("CROPPED", path, "%dx%d" % (side, side), "object %dx%d" % (x1 - x0, y1 - y0))


if TRANSPARENT and not opts.get("nocrop"):
    crop_to_object(OUT, float(opts.get("margin", 0.04)))
