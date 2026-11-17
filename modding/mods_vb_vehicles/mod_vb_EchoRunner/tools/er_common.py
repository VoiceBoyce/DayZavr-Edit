# -*- coding: utf-8 -*-
r"""Общее для скриптов Echo Runner в Blender: чтение MLOD (py3d), меши из LOD, материалы из текстур DayZ.

Оси: MLOD DayZ (x, y вверх, z; нос в -z) -> Blender (x, z, y): нос в -Y, верх +Z, водитель (+x) = левый борт.
Порядок вершин MLOD (векторное произведение внутрь) после отражения осей смотрит наружу — порядок не меняем,
нормали MLOD меняют знак. UV: v -> 1 - v.
Прокси: py3d.get_proxies() даёт «сырую» рамку (x, y, z); строки матрицы ODOL = (-x, z, y)
(сверено конвертером odol2mlod), точка детали c -> anchor + c0*(-x) + c1*z + c2*y.
"""
import os
import subprocess
import sys

import bpy
import numpy as np

TOOLS = r"d:\DayZavr\DayZavr-Edit\.claude\tools"
sys.path.insert(0, os.path.join(TOOLS, "py3d"))
sys.path.insert(0, os.path.join(TOOLS, "dayz-render"))
import py3d  # noqa: E402
import rvmat_read  # noqa: E402

BS = chr(92)
IMAGE_TO_PAA = r"D:\SteamLibrary\steamapps\common\DayZ Tools\Bin\ImageToPAA\ImageToPAA.exe"
CACHE = os.path.join(TOOLS, "dayz-render", "texcache")
ALEX_ROOT = r"d:\DayZavr\DayZavr-Edit\modding\mods_vb_vehicles\купленные модели\Interceptor"
ROOTS = [ALEX_ROOT, "P:" + BS]
MAXEDGE = 1.5


def set_roots(roots):
    ROOTS[:] = [r for r in roots if r] + ["P:" + BS]


def load_p3d(path):
    with open(path, "rb") as f:
        return py3d.P3D(f)


def is_res(lod, r):
    return abs(lod.resolution / r - 1) < 1e-3


def lod_of(m, res):
    return min(m.lods, key=lambda l: abs(l.resolution - res) / max(res, 1e-9))


def proxy_xform(pr):
    """(anchor, A0, A1, A2) в осях DayZ: точка детали c -> anchor + c0*A0 + c1*A1 + c2*A2."""
    x, y, z = pr["frame"]
    return (tuple(pr["anchor"]), tuple(-c for c in x), tuple(z), tuple(y))


def proxy_base(pr):
    path = pr.get("path") or pr["name"][6:].rsplit(".", 1)[0]
    return os.path.basename(path.replace(BS, "/")).lower()


def to_blender(P):
    return P[:, [0, 2, 1]]


# ---------------------------------------------------------------- текстуры и материалы
def resolve(vpath):
    vp = vpath.replace("/", BS).lstrip(BS)
    for r in ROOTS:
        p = os.path.join(r, vp)
        if os.path.exists(p):
            return p
    return None


def paa_png(vpath):
    src = resolve(vpath)
    if not src:
        print("WARN нет текстуры", vpath)
        return None
    os.makedirs(CACHE, exist_ok=True)
    dst = os.path.join(CACHE, vpath.lower().replace(BS, "__").replace("/", "__").replace(":", "") + ".tga")
    if not os.path.exists(dst) or os.path.getmtime(dst) < os.path.getmtime(src):
        subprocess.run([IMAGE_TO_PAA, src, dst], capture_output=True)
    return dst if os.path.exists(dst) else None


def proc_color(s):
    try:
        inside = s[s.index("color(") + 6:s.rindex(")")]
        v = [float(x) for x in inside.split(",")[:4]]
        return tuple(v + [1.0] * (4 - len(v)))
    except Exception:
        return None


_img = {}


def load_img(vpath, noncolor=False):
    key = (vpath.lower(), noncolor)
    if key not in _img:
        p = paa_png(vpath)
        im = bpy.data.images.load(p) if p else None
        if im and noncolor:
            im.colorspace_settings.name = "Non-Color"
        _img[key] = im
    return _img[key]


_mats = {}
# процедурные материалы деталей-черновиков: «#(argb,...)color(r,g,b,a,co)» + подсказка в rvmat-поле
EMISSIVE_HINT = "emit"


def material(tex, rvmat):
    key = (tex.lower(), (rvmat or "").lower())
    if key in _mats:
        return _mats[key]
    mat = bpy.data.materials.new(os.path.basename(tex.replace(BS, "/")) + "|" + os.path.basename((rvmat or "").replace(BS, "/")))
    mat.use_nodes = True
    nt = mat.node_tree
    Nn, L = nt.nodes, nt.links
    bsdf = Nn["Principled BSDF"]
    uvn = Nn.new("ShaderNodeUVMap")
    uvn.uv_map = "UV"

    def tnode(im):
        t = Nn.new("ShaderNodeTexImage")
        t.image = im
        L.new(uvn.outputs["UV"], t.inputs["Vector"])
        return t

    rv = {}
    if rvmat and rvmat.lower().endswith(".rvmat"):
        rp = resolve(rvmat)
        if rp:
            try:
                rv = rvmat_read.read_rvmat(rp)
            except Exception as e:
                print("WARN rvmat", rvmat, e)
    stages = {k: v.get("texture", "") for k, v in rv.items() if isinstance(v, dict) and k.startswith("Stage")}
    alpha = False
    if tex.startswith("#"):
        c = proc_color(tex) or (0.5, 0.5, 0.5, 1)
        bsdf.inputs["Base Color"].default_value = c
        if rvmat == EMISSIVE_HINT:                  # неон: тёмно-красное стекло + насыщенное красное свечение
            bsdf.inputs["Base Color"].default_value = (c[0] * 0.30, c[1] * 0.30, c[2] * 0.30, 1)
            bsdf.inputs["Emission Color"].default_value = (1.0, 0.025, 0.01, 1)
            bsdf.inputs["Emission Strength"].default_value = 1.5
            bsdf.inputs["Roughness"].default_value = 0.25
        if rvmat == "glass":                        # стекло колбы
            bsdf.inputs["Transmission Weight"].default_value = 0.85
            bsdf.inputs["Roughness"].default_value = 0.05
        if rvmat == "metal":
            bsdf.inputs["Metallic"].default_value = 0.8
            bsdf.inputs["Roughness"].default_value = 0.45
    elif tex:
        im = load_img(tex)
        if im:
            t = tnode(im)
            L.new(t.outputs["Color"], bsdf.inputs["Base Color"])
            if "_ca." in tex.lower():
                L.new(t.outputs["Alpha"], bsdf.inputs["Alpha"])
                alpha = True
        else:
            bsdf.inputs["Base Color"].default_value = (0.5, 0.5, 0.5, 1)
    else:
        d = rv.get("diffuse") or [0.5, 0.5, 0.5, 1]
        bsdf.inputs["Base Color"].default_value = tuple(list(d[:3]) + [1])
    nohq = next((v for k, v in sorted(stages.items()) if "_nohq" in v.lower()), None)
    if nohq and not nohq.startswith("#"):
        im = load_img(nohq, True)
        if im:
            t = tnode(im)
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
    smdi = next((v for v in stages.values() if "_smdi" in v.lower()), None)
    if smdi and not smdi.startswith("#") and load_img(smdi, True):
        t = tnode(load_img(smdi, True))
        sps = Nn.new("ShaderNodeSeparateColor")
        L.new(t.outputs["Color"], sps.inputs["Color"])
        rough = Nn.new("ShaderNodeMapRange")
        rough.inputs["To Min"].default_value = 0.85
        rough.inputs["To Max"].default_value = 0.12
        L.new(sps.outputs["Blue"], rough.inputs["Value"])
        L.new(rough.outputs["Result"], bsdf.inputs["Roughness"])
        L.new(sps.outputs["Green"], bsdf.inputs["Specular IOR Level"])
    elif not tex.startswith("#"):
        bsdf.inputs["Roughness"].default_value = 0.6
        bsdf.inputs["Specular IOR Level"].default_value = 0.3
    em = rv.get("emmisive") or [0, 0, 0, 1]
    if max(em[:3]) > 0.001:
        bsdf.inputs["Emission Color"].default_value = (min(1, em[0]), min(1, em[1]), min(1, em[2]), 1)
        bsdf.inputs["Emission Strength"].default_value = max(1.0, max(em[:3]))
    if alpha:
        try:
            mat.surface_render_method = "BLENDED"
        except Exception:
            pass
    _mats[key] = mat
    return mat


# ---------------------------------------------------------------- меши
def mesh_from_lod(lod, name, xform=None, with_mats=True, skip_sel=None):
    """xform: (anchor, A0, A1, A2) — c -> anchor + c0*A0 + c1*A1 + c2*A2 (оси DayZ).
    skip_sel: имена выделений, чьи грани не берём (кроме прокси, они не берутся всегда)."""
    skip = set()
    for sname, sel in lod.selections.items():
        if sname.startswith("proxy:") or (skip_sel and sname in skip_sel):
            skip.update(id(f) for f in sel.faces)
    P = np.array([p.coords for p in lod.points], dtype=np.float64) if lod.points else np.zeros((0, 3))
    Nrm = np.array(lod.facenormals, dtype=np.float64) if lod.facenormals else np.zeros((0, 3))
    if xform is not None:
        a, A0, A1, A2 = (np.array(v, dtype=np.float64) for v in xform)
        M = np.stack([A0, A1, A2])
        P = a + P @ M
        if len(Nrm):
            Nrm = Nrm @ M
    PB = to_blender(P)
    NB = -Nrm[:, [0, 2, 1]] if len(Nrm) else Nrm
    faces, uvs, nrms, keys, mats = [], [], [], [], {}
    for fa in lod.faces:
        if id(fa) in skip:
            continue
        idx = [v.point_index for v in fa.vertices]
        if len(set(idx)) != len(idx):
            continue
        if MAXEDGE and max(np.linalg.norm(P[idx[k]] - P[idx[(k + 1) % len(idx)]]) for k in range(len(idx))) > MAXEDGE:
            continue
        faces.append(idx)
        for v in fa.vertices:
            uvs.extend([v.uv[0], 1.0 - v.uv[1]])
            nrms.append(NB[v.normal_index] if len(NB) else (0, 0, 1))
        k = (fa.texture or "", fa.material or "")
        if k not in mats:
            mats[k] = len(mats)
        keys.append(mats[k])
    me = bpy.data.meshes.new(name)
    me.from_pydata(PB.tolist(), [], faces)
    if with_mats:
        uvl = me.uv_layers.new(name="UV")
        uvl.data.foreach_set("uv", uvs)
        for k in sorted(mats, key=lambda k: mats[k]):
            me.materials.append(material(*k))
        me.polygons.foreach_set("material_index", keys)
        me.polygons.foreach_set("use_smooth", [True] * len(faces))
        try:
            nn = np.array(nrms, dtype=np.float64)
            nn /= np.maximum(np.linalg.norm(nn, axis=1, keepdims=True), 1e-9)
            if not me.validate(verbose=False) and len(me.loops) == len(nn):
                me.normals_split_custom_set(nn.tolist())
        except Exception as e:
            print("WARN normals", name, e)
    me.update()
    ob = bpy.data.objects.new(name, me)
    bpy.context.scene.collection.objects.link(ob)
    return ob


def load_car(car_path, proxydir, res=1.0, with_mats=True, extra=()):
    """Кузов + детали-прокси (по имени файла в proxydir). Возвращает {имя: объект}, {имя прокси: xform}."""
    car = load_p3d(car_path)
    lod = lod_of(car, res)
    objs = {"body": mesh_from_lod(lod, "body", with_mats=with_mats)}
    xforms = {}
    for pr in lod.get_proxies():
        base = proxy_base(pr)
        src = os.path.join(proxydir, base + ".p3d") if proxydir else ""
        if not (src and os.path.exists(src)):
            continue
        xf = proxy_xform(pr)
        nm = base + "_" + pr["name"].rsplit(".", 1)[1]
        xforms[nm] = xf
        objs[nm] = mesh_from_lod(lod_of(load_p3d(src), res), nm, xf, with_mats=with_mats)
    for ex in extra:
        objs[os.path.basename(ex)] = mesh_from_lod(lod_of(load_p3d(ex), res), os.path.basename(ex), with_mats=with_mats)
    return objs, xforms
