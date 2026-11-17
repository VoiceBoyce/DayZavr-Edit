# -*- coding: utf-8 -*-
r"""Запекание карт положения/нормали/покрытия в развёртку текстур ALEX (для перекраски er_tex_paint.py).

  blender.exe -b --factory-startup --python er_tex_bake.py -- <вариант> [--only=body,salon]
  вход: work/<вариант>/full (машина + детали-прокси в своих местах, LOD 1)
  выход: work/<вариант>/tex/<имя>_pos.npy (x, y, z Blender, м), _nrm.npy (нормаль), _mask.npy (покрытие 0/1);
         строка 0 — верх картинки (как в PAA/PNG, v DayZ вниз).
Положение и нормаль — в осях машины (нос -Y, верх +Z, водитель +X); двери и капот — в закрытом положении.
"""
import os
import sys

import bpy
import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import er_common as C  # noqa: E402

args = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
opts = {a.split("=", 1)[0][2:]: (a.split("=", 1)[1] if "=" in a else True) for a in args if a.startswith("--")}
VAR = [a for a in args if not a.startswith("--")][0]
MOD = os.path.dirname(HERE)
FULL = os.path.join(MOD, "work", VAR, "full")
OUT = os.path.join(MOD, "work", VAR, "tex")
os.makedirs(OUT, exist_ok=True)
SETS = {   # имя: (ширина, высота) итоговой текстуры
    "body": (4096, 2048),
    "chassis": (2048, 1024),
    "salon": (2048, 2048),
    "engine": (1024, 1024),
}
ONLY = set(str(opts.get("only", "")).split(",")) - {""}
OFF = 4.0          # кодирование положения в 0..1: (p + OFF) / (2*OFF)

bpy.ops.wm.read_factory_settings(use_empty=True)
scene = bpy.context.scene
scene.render.engine = "CYCLES"
scene.cycles.samples = 1
scene.cycles.device = "CPU"


def gather(texname):
    """Меш из граней с текстурой vb_echorunner\\data\\<texname>_co.paa — по машине и деталям-прокси (LOD 1)."""
    want = ("vb_echorunner\\data\\%s_co.paa" % texname).lower()
    car = C.load_p3d(os.path.join(FULL, "interceptor.p3d"))
    lod = C.lod_of(car, 1.0)
    pieces = [(lod, None)]
    seen = set()
    for pr in lod.get_proxies():
        base = C.proxy_base(pr)
        src = os.path.join(FULL, "proxy", base + ".p3d")
        if not os.path.exists(src) or base in seen:          # одна копия детали: развёртка у копий общая
            continue
        seen.add(base)
        pieces.append((C.lod_of(C.load_p3d(src), 1.0), C.proxy_xform(pr)))
    verts, faces, uvs = [], [], []
    for l, xf in pieces:
        P = np.array([p.coords for p in l.points], dtype=np.float64)
        if xf is not None:
            a, A0, A1, A2 = (np.array(v, dtype=np.float64) for v in xf)
            P = a + P @ np.stack([A0, A1, A2])
        PB = C.to_blender(P)
        for fa in l.faces:
            if fa.texture.lower() != want:
                continue
            idx = [v.point_index for v in fa.vertices]
            if len(set(idx)) != len(idx):
                continue
            base_i = len(verts)
            verts.extend(PB[idx].tolist())
            faces.append(list(range(base_i, base_i + len(idx))))
            uvs.extend([(v.uv[0], 1.0 - v.uv[1]) for v in fa.vertices])
    me = bpy.data.meshes.new(texname)
    me.from_pydata(verts, [], faces)
    uvl = me.uv_layers.new(name="UV")
    for li, uv in enumerate(uvs):
        uvl.data[li].uv = uv
    me.update()
    ob = bpy.data.objects.new(texname, me)
    scene.collection.objects.link(ob)
    print("SET", texname, "граней", len(faces))
    return ob


def bake(ob, w, h, kind):
    img = bpy.data.images.new("%s_%s" % (ob.name, kind), w, h, alpha=True, float_buffer=True)
    img.colorspace_settings.name = "Non-Color"
    mat = bpy.data.materials.new("bake_" + kind)
    mat.use_nodes = True
    nt = mat.node_tree
    nt.nodes.clear()
    out = nt.nodes.new("ShaderNodeOutputMaterial")
    em = nt.nodes.new("ShaderNodeEmission")
    geo = nt.nodes.new("ShaderNodeNewGeometry")
    nt.links.new(em.outputs["Emission"], out.inputs["Surface"])
    if kind == "pos":
        vm = nt.nodes.new("ShaderNodeVectorMath")
        vm.operation = "MULTIPLY_ADD"
        vm.inputs[1].default_value = (1 / (2 * OFF),) * 3
        vm.inputs[2].default_value = (0.5,) * 3
        nt.links.new(geo.outputs["Position"], vm.inputs[0])
        nt.links.new(vm.outputs["Vector"], em.inputs["Color"])
    elif kind == "nrm":
        vm = nt.nodes.new("ShaderNodeVectorMath")
        vm.operation = "MULTIPLY_ADD"
        vm.inputs[1].default_value = (0.5, 0.5, 0.5)
        vm.inputs[2].default_value = (0.5, 0.5, 0.5)
        nt.links.new(geo.outputs["True Normal"], vm.inputs[0])
        nt.links.new(vm.outputs["Vector"], em.inputs["Color"])
    else:
        em.inputs["Color"].default_value = (1, 1, 1, 1)
    tex = nt.nodes.new("ShaderNodeTexImage")
    tex.image = img
    nt.nodes.active = tex
    ob.data.materials.clear()
    ob.data.materials.append(mat)
    for o in scene.objects:
        o.select_set(o == ob)
    bpy.context.view_layer.objects.active = ob
    scene.render.bake.margin = 0 if kind == "mask" else 16
    scene.render.bake.use_clear = True
    bpy.ops.object.bake(type="EMIT")
    px = np.array(img.pixels[:], dtype=np.float32).reshape(h, w, 4)[::-1]       # строка 0 — верх
    return px


for name, (w, h) in SETS.items():
    if ONLY and name not in ONLY:
        continue
    ob = gather(name)
    pos = bake(ob, w, h, "pos")
    nrm = bake(ob, w, h, "nrm")
    mask = bake(ob, w, h, "mask")
    np.save(os.path.join(OUT, name + "_pos.npy"), ((pos[..., :3] - 0.5) * 2 * OFF).astype(np.float16))
    np.save(os.path.join(OUT, name + "_nrm.npy"), ((nrm[..., :3] - 0.5) * 2).astype(np.float16))
    np.save(os.path.join(OUT, name + "_mask.npy"), (mask[..., 0] > 0.5))
    print("BAKED", name, w, h, "покрытие %.1f%%" % (100 * (mask[..., 0] > 0.5).mean()))
    bpy.data.objects.remove(ob)
print("OK", OUT)
