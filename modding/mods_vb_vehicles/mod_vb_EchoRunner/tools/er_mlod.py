# -*- coding: utf-8 -*-
r"""Правка MLOD без Blender (py3d): связные куски, удаление граней, копия граней в новый LOD."""
import sys
from collections import defaultdict

sys.path.insert(0, r"d:\DayZavr\DayZavr-Edit\.claude\tools\py3d")
import py3d  # noqa: E402,F401


def is_res(lod, r):
    return abs(lod.resolution / r - 1) < 1e-3


def face_bbox(lod, faces):
    pts = [lod.points[v.point_index].coords for fa in faces for v in fa.vertices]
    return [(min(c[i] for c in pts), max(c[i] for c in pts)) for i in range(3)]


def components(lod, faces):
    """Связные куски (склейка по индексу точки и по совпадающим координатам — швы UV)."""
    parent = {}

    def find(a):
        while parent.setdefault(a, a) != a:
            parent[a] = parent[parent[a]]
            a = parent[a]
        return a
    key = [tuple(round(c, 4) for c in p.coords) for p in lod.points]
    for fa in faces:
        ks = [key[v.point_index] for v in fa.vertices]
        for k in ks[1:]:
            ra, rb = find(ks[0]), find(k)
            if ra != rb:
                parent[ra] = rb
    comp = defaultdict(list)
    for fa in faces:
        comp[find(key[fa.vertices[0].point_index])].append(fa)
    return list(comp.values())


def prune(lod, rm_faces, drop_empty=True):
    """Удалить грани (и точки, которые были только у них). Списки меняются на месте — на них ссылаются
    Vertex/Selection."""
    rm = {id(f) for f in rm_faces}
    keep = [f for f in lod.faces if id(f) not in rm]
    used_keep = {v.point_index for f in keep for v in f.vertices}
    used_rm = {v.point_index for f in rm_faces for v in f.vertices}
    drop_pts = used_rm - used_keep
    new_index, new_points = {}, []
    for i, p in enumerate(lod.points):
        if i in drop_pts:
            continue
        new_index[i] = len(new_points)
        new_points.append(p)
    for f in keep:
        for v in f.vertices:
            v.point_index = new_index[v.point_index]
    dropped = {id(lod.points[i]) for i in drop_pts}
    lod.faces[:] = keep
    lod.points[:] = new_points
    for name in list(lod.selections):
        sel = lod.selections[name]
        sel.points = {p: w for p, w in sel.points.items() if id(p) not in dropped}
        sel.faces = {f: w for f, w in sel.faces.items() if id(f) not in rm}
        if drop_empty and not sel.points and not sel.faces:
            del lod.selections[name]


def prune_selection(lod, names):
    """Удалить все грани выделений names (и сами выделения)."""
    fs = {}
    for n in names:
        if n in lod.selections:
            for f in lod.selections[n].faces:
                fs[id(f)] = f
    if fs:
        prune(lod, list(fs.values()))
    for n in names:
        lod.selections.pop(n, None)
    return len(fs)


def copy_faces_to_lod(src, faces, res=1.0):
    out = py3d.LOD()
    out.resolution = res
    pmap, nmap = {}, {}
    for fa in faces:
        nf = py3d.Face(out.points, out.facenormals)
        nf.texture, nf.material, nf.flags = fa.texture, fa.material, fa.flags
        for v in fa.vertices:
            if v.point_index not in pmap:
                p = py3d.Point()
                p.coords = src.points[v.point_index].coords
                pmap[v.point_index] = len(out.points)
                out.points.append(p)
            if v.normal_index not in nmap:
                nmap[v.normal_index] = len(out.facenormals)
                out.facenormals.append(src.facenormals[v.normal_index])
            nv = py3d.Vertex(out.points, out.facenormals)
            nv.point_index, nv.normal_index, nv.uv = pmap[v.point_index], nmap[v.normal_index], v.uv
            nf.vertices.append(nv)
        out.faces.append(nf)
    return out
