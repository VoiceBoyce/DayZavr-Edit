# -*- coding: utf-8 -*-
r"""Рендер 27 новых ресурсов VB_MiningResources в PNG на прозрачном фоне + сводный лист (Blender, Cycles).

  blender.exe -b --factory-startup --python render_mr_models.py -- --models=<папка с vb_mr_*.p3d> --out=<папка png>
      [--only=VB_MR_Gold_Sand,VB_MR_Coal] [--size=1400] [--samples=160] [--sheet=<лист.png>] [--cell=300]

Модели — бинарные (ODOL, из собранного PBO: pbo.ps1 unpack ... -Raw) или исходные MLOD (для быстрой проверки).
Для каждого класса из data\items\mapping.json: модель формы, на секцию camo — текстура и rvmat класса
(как hiddenSelections в конфиге). Если секции camo нет или она покрывает не все грани — WARN.
Шейдер: _co — цвет, _nohq — нормали (DirectX -> инверсия G), _smdi — блик (G) и глянец (B);
слиток и самородок — металл (Metallic 1). Освещение и «небо» для отражений одинаковы для всех предметов.
Текстуры PAA -> TGA через ImageToPAA в кэш %TEMP%\vb_mr_texcache.
"""
import json
import math
import os
import subprocess
import sys
import tempfile

import bpy
import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
MOD_ROOT = os.path.dirname(HERE)
REPO = os.path.normpath(os.path.join(MOD_ROOT, "..", ".."))
RENDER_TOOLS = os.path.join(REPO, ".claude", "tools", "dayz-render")
sys.path.insert(0, RENDER_TOOLS)
sys.path.insert(0, os.path.join(REPO, ".claude", "tools", "py3d"))
import odol_read  # noqa: E402
import rvmat_read  # noqa: E402

BS = chr(92)
IMAGE_TO_PAA = os.path.join("D:" + BS, "SteamLibrary", "steamapps", "common", "DayZ Tools", "Bin", "ImageToPAA",
                            "ImageToPAA.exe")
CACHE = os.path.join(tempfile.gettempdir(), "vb_mr_texcache")
MAPPING = os.path.join(MOD_ROOT, "VB_MiningResources", "data", "items", "mapping.json")

args = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
opts = {a.split("=", 1)[0][2:]: (a.split("=", 1)[1] if "=" in a else True) for a in args if a.startswith("--")}
MODELS = opts["models"]
OUT = opts["out"]
SIZE = int(opts.get("size", 1400))
SAMPLES = int(opts.get("samples", 160))
ONLY = [o for o in str(opts.get("only", "")).split(",") if o]

RES_ORDER = ["gold", "silver", "bronze", "iron", "titanium", "coal", "rubin", "nephrite", "sulfur"]
RES_RU = {"gold": "Золото", "silver": "Серебро", "bronze": "Медь", "iron": "Железо", "titanium": "Титан",
          "coal": "Уголь", "rubin": "Рубин", "nephrite": "Нефрит", "sulfur": "Сера"}
ROW_RU = ["Песок", "Самородок / кусок", "Слиток / брикет"]
METAL_SHAPES = {"nugget", "ingot"}
VIEW = {"sand": (30, -32), "nugget": (38, -30), "chunk": (32, -34), "ingot": (30, -28), "briquette": (34, -32)}


def resolve(vpath):
    vp = vpath.replace("/", BS).lstrip(BS)
    for root in (MOD_ROOT, "P:" + BS):
        p = os.path.join(root, vp)
        if os.path.exists(p):
            return p
    return None


def paa_tga(vpath):
    src = resolve(vpath)
    if not src:
        print("WARN нет текстуры", vpath)
        return None
    os.makedirs(CACHE, exist_ok=True)
    dst = os.path.join(CACHE, vpath.replace(BS, "__").replace("/", "__").replace(":", "") + ".tga")
    if not os.path.exists(dst) or os.path.getmtime(dst) < os.path.getmtime(src):
        subprocess.run([IMAGE_TO_PAA, src, dst], capture_output=True)
    return dst if os.path.exists(dst) else None


# ---------------------------------------------------------------- чтение модели
def read_model(path):
    """Первый визуальный LOD в осях Blender: точки, грани, UV на угол, нормали на угол, грани секции camo."""
    head = open(path, "rb").read(4)
    if head == b"ODOL":
        lod = odol_read.read_visual_lod(path, 0)
        P = np.array(lod["points"], np.float64)[:, [0, 2, 1]]
        N = np.array(lod["normals"], np.float64)[:, [0, 2, 1]]
        UV = np.array(lod["uv"], np.float64)
        faces = [f for f in lod["faces"]]
        loops_uv = [[UV[v] for v in f] for f in faces]
        loops_n = [[N[v] for v in f] for f in faces]
        camo = None
        for s in lod["selections"]:
            if s["name"].lower() == "camo":
                camo = set(s["face_set"])
        return P, faces, loops_uv, loops_n, camo, "ODOL"
    import py3d
    with open(path, "rb") as f:
        m = py3d.P3D(f)
    lod = m.lods[0]
    P = np.array([p.coords for p in lod.points], np.float64)[:, [0, 2, 1]]
    FN = -np.array(lod.facenormals, np.float64)[:, [0, 2, 1]]     # MLOD: нормали внутрь
    faces, loops_uv, loops_n = [], [], []
    for fc in lod.faces:
        faces.append([v.point_index for v in fc.vertices])
        loops_uv.append([v.uv for v in fc.vertices])
        loops_n.append([FN[v.normal_index] for v in fc.vertices])
    camo = None
    sel = lod.selections.get("camo")
    if sel is not None:
        ids = {id(f) for f in sel.faces}
        camo = {i for i, f in enumerate(lod.faces) if id(f) in ids}
    return P, faces, loops_uv, loops_n, camo, "MLOD"


# ---------------------------------------------------------------- сцена
def setup_scene():
    bpy.ops.wm.read_factory_settings(use_empty=True)
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
    for vt in ("AgX", "Filmic"):
        try:
            sc.view_settings.view_transform = vt
            break
        except TypeError:
            pass
    for lk in ("AgX - Medium High Contrast", "Medium High Contrast"):
        try:
            sc.view_settings.look = lk
            break
        except TypeError:
            pass
    # «небо» для отражений металла: светлый верх, тёмный низ
    w = bpy.data.worlds.new("w")
    w.use_nodes = True
    nt = w.node_tree
    bg = nt.nodes["Background"]
    tc = nt.nodes.new("ShaderNodeTexCoord")
    sep = nt.nodes.new("ShaderNodeSeparateXYZ")
    ramp = nt.nodes.new("ShaderNodeValToRGB")
    mr = nt.nodes.new("ShaderNodeMapRange")
    mr.inputs["From Min"].default_value = -1.0
    mr.inputs["From Max"].default_value = 1.0
    nt.links.new(tc.outputs["Generated"], sep.inputs["Vector"])
    nt.links.new(sep.outputs["Z"], mr.inputs["Value"])
    nt.links.new(mr.outputs["Result"], ramp.inputs["Fac"])
    ramp.color_ramp.elements[0].position = 0.35
    ramp.color_ramp.elements[0].color = (0.035, 0.032, 0.03, 1)
    ramp.color_ramp.elements[1].position = 0.85
    ramp.color_ramp.elements[1].color = (0.58, 0.60, 0.63, 1)
    e = ramp.color_ramp.elements.new(0.55)
    e.color = (0.28, 0.27, 0.25, 1)
    nt.links.new(ramp.outputs["Color"], bg.inputs["Color"])
    bg.inputs["Strength"].default_value = 0.9
    sc.world = w
    return sc


def clear_objects():
    for ob in list(bpy.data.objects):
        bpy.data.objects.remove(ob, do_unlink=True)
    for me in list(bpy.data.meshes):
        bpy.data.meshes.remove(me)
    for ma in list(bpy.data.materials):
        bpy.data.materials.remove(ma)
    for li in list(bpy.data.lights):
        bpy.data.lights.remove(li)
    for im in list(bpy.data.images):
        bpy.data.images.remove(im)


def load_img(vpath, noncolor=False):
    p = paa_tga(vpath)
    if not p:
        return None
    im = bpy.data.images.load(p)
    if noncolor:
        im.colorspace_settings.name = "Non-Color"
    return im


def make_material(tex, rvmat_path, metal):
    mat = bpy.data.materials.new(os.path.basename(tex.replace(BS, "/")))
    mat.use_nodes = True
    nt = mat.node_tree
    Nn, L = nt.nodes, nt.links
    bsdf = Nn["Principled BSDF"]
    uvn = Nn.new("ShaderNodeUVMap")
    uvn.uv_map = "UV"

    def tex_node(im):
        t = Nn.new("ShaderNodeTexImage")
        t.image = im
        t.interpolation = "Cubic"
        L.new(uvn.outputs["UV"], t.inputs["Vector"])
        return t
    im = load_img(tex)
    if im:
        L.new(tex_node(im).outputs["Color"], bsdf.inputs["Base Color"])
    rv = {}
    rp = resolve(rvmat_path)
    if rp:
        rv = rvmat_read.read_rvmat(rp)
    else:
        print("WARN нет rvmat", rvmat_path)
    st = {k: v.get("texture", "") for k, v in rv.items() if isinstance(v, dict) and k.startswith("Stage")}
    nohq = st.get("Stage1", "")
    if nohq and not nohq.startswith("#"):
        im = load_img(nohq, True)
        if im:
            t = tex_node(im)
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
    spec = rv.get("specular") or [0.3, 0.3, 0.3, 1]
    spow = float(rv.get("specularPower", 40) or 40)
    lvl = min(1.0, float(sum(spec[:3]) / 3.0))
    gloss = min(1.0, math.log10(max(spow, 1.0)) / 2.4)
    smdi = st.get("Stage5", "")
    bsdf.inputs["Metallic"].default_value = 1.0 if metal else 0.0
    if smdi and not smdi.startswith("#"):
        im = load_img(smdi, True)
        if im:
            t = tex_node(im)
            sp = Nn.new("ShaderNodeSeparateColor")
            L.new(t.outputs["Color"], sp.inputs["Color"])
            mul = Nn.new("ShaderNodeMath")
            mul.operation = "MULTIPLY"
            mul.inputs[1].default_value = gloss
            L.new(sp.outputs["Blue"], mul.inputs[0])
            rough = Nn.new("ShaderNodeMapRange")
            rough.inputs["To Min"].default_value = 0.9
            rough.inputs["To Max"].default_value = 0.12 if metal else 0.06
            L.new(mul.outputs["Value"], rough.inputs["Value"])
            L.new(rough.outputs["Result"], bsdf.inputs["Roughness"])
            sl = Nn.new("ShaderNodeMath")
            sl.operation = "MULTIPLY"
            sl.inputs[1].default_value = lvl if metal else min(lvl, 1.0) * 0.55
            L.new(sp.outputs["Green"], sl.inputs[0])
            L.new(sl.outputs["Value"], bsdf.inputs["Specular IOR Level"])
    return mat


def build_object(P, faces, loops_uv, loops_n, mat):
    me = bpy.data.meshes.new("model")
    me.from_pydata(P.tolist(), [], faces)
    uvl = me.uv_layers.new(name="UV")
    flat = []
    for f in loops_uv:
        for uv in f:
            flat += [uv[0], 1.0 - uv[1]]
    uvl.data.foreach_set("uv", flat)
    me.materials.append(mat)
    me.polygons.foreach_set("use_smooth", [True] * len(faces))
    ln = []
    for f in loops_n:
        for n in f:
            v = np.asarray(n, float)
            ln.append(tuple(v / max(np.linalg.norm(v), 1e-9)))
    try:
        if me.validate(verbose=False) or len(me.polygons) != len(faces):
            raise RuntimeError("Blender исправил меш")
        me.normals_split_custom_set(ln)
    except Exception as e:
        print("WARN normals", e)
    me.update()
    ob = bpy.data.objects.new("model", me)
    bpy.context.scene.collection.objects.link(ob)
    return ob


def render_item(sc, cls, info):
    shape = info["shape"]
    path = os.path.join(MODELS, "vb_mr_%s.p3d" % shape)
    P, faces, luv, ln, camo, kind = read_model(path)
    if camo is None:
        print("WARN", cls, "нет секции camo в", kind)
    elif len(camo) != len(faces):
        print("WARN", cls, "camo покрывает %d из %d граней" % (len(camo), len(faces)))
    # согласовать обход граней с нормалями (наружу)
    agree = 0
    for f, nn in list(zip(faces, ln))[:3000]:
        a, b, c = P[f[0]], P[f[1]], P[f[2]]
        agree += 1 if np.dot(np.cross(b - a, c - a), np.sum(nn, axis=0)) > 0 else -1
    if agree < 0:
        faces = [f[::-1] for f in faces]
        luv = [u[::-1] for u in luv]
        ln = [n[::-1] for n in ln]
    clear_objects()
    P = P.copy()
    P[:, 2] -= P[:, 2].min()
    lo, hi = P.min(0), P.max(0)
    dims = hi - lo
    s = float(max(dims))
    center = (lo + hi) / 2
    mat = make_material(info["texture"], info["material"], shape in METAL_SHAPES)
    ob = build_object(P, faces, luv, ln, mat)
    k = (s / 0.05) ** 2

    def area(name, loc, power, size, color, glossy=True):
        ld = bpy.data.lights.new(name, "AREA")
        ld.energy = power
        ld.size = size
        ld.color = color
        o = bpy.data.objects.new(name, ld)
        o.location = tuple(center + np.array(loc))
        o.visible_glossy = glossy
        c = o.constraints.new("TRACK_TO")
        c.target = ob
        c.track_axis = "TRACK_NEGATIVE_Z"
        c.up_axis = "UP_Y"
        sc.collection.objects.link(o)
    area("key", (-3.0 * s, -3.2 * s, 3.6 * s), 1.0 * k, 2.5 * s, (1.0, 0.95, 0.88))
    # заполняющий свет только подсвечивает тени: его отражение белило целую грань у глянцевых камней
    area("fill", (3.6 * s, -2.4 * s, 1.4 * s), 0.35 * k, 3.0 * s, (0.85, 0.9, 1.0), glossy=False)
    area("rim", (1.2 * s, 3.0 * s, 2.8 * s), 1.4 * k, 1.6 * s, (0.85, 0.9, 1.0))
    cam_d = bpy.data.cameras.new("cam")
    cam_d.lens = 85
    cam_d.sensor_width = 36
    r = 0.5 * float(np.linalg.norm(dims))
    dist = r / math.sin(math.atan(18.0 / 85.0)) * 1.08
    cam_d.clip_start = dist * 0.01
    cam_d.clip_end = dist * 100
    elev, azim = (math.radians(a) for a in VIEW.get(shape, (28, -30)))
    cam = bpy.data.objects.new("cam", cam_d)
    cam.location = (center[0] + dist * math.cos(elev) * math.sin(azim), center[1] - dist * math.cos(elev) * math.cos(azim),
                    center[2] + dist * math.sin(elev))
    tgt = bpy.data.objects.new("tgt", None)
    tgt.location = tuple(center)
    sc.collection.objects.link(tgt)
    c = cam.constraints.new("TRACK_TO")
    c.target = tgt
    c.track_axis = "TRACK_NEGATIVE_Z"
    c.up_axis = "UP_Y"
    sc.collection.objects.link(cam)
    sc.camera = cam
    sc.render.resolution_x = sc.render.resolution_y = SIZE
    out = os.path.join(OUT, cls + ".png")
    sc.render.filepath = out
    bpy.ops.render.render(write_still=True)
    crop(out)
    print("RENDER %-22s %s %d граней, габарит %.1f x %.1f x %.1f см" % (cls, kind, len(faces), dims[0] * 100,
                                                                       dims[1] * 100, dims[2] * 100))
    return out


def crop(path, margin=0.05):
    im = bpy.data.images.load(path)
    w, h = im.size
    px = np.array(im.pixels[:], np.float32).reshape(h, w, 4)
    bpy.data.images.remove(im)
    ys, xs = np.nonzero(px[..., 3] > 0.003)
    x0, x1, y0, y1 = xs.min(), xs.max() + 1, ys.min(), ys.max() + 1
    side = int(max(x1 - x0, y1 - y0) * (1 + 2 * margin))
    out = np.zeros((side, side, 4), np.float32)
    ox, oy = (side - (x1 - x0)) // 2, (side - (y1 - y0)) // 2
    out[oy:oy + y1 - y0, ox:ox + x1 - x0] = px[y0:y1, x0:x1]
    save_rgba(out, path)


def save_rgba(arr, path):
    h, w = arr.shape[:2]
    new = bpy.data.images.new("tmp", w, h, alpha=True)
    new.pixels.foreach_set(np.ascontiguousarray(arr, np.float32).ravel())
    new.filepath_raw = path
    new.file_format = "PNG"
    new.save()
    bpy.data.images.remove(new)


def load_rgba(path):
    im = bpy.data.images.load(path)
    w, h = im.size
    px = np.array(im.pixels[:], np.float32).reshape(h, w, 4)
    bpy.data.images.remove(im)
    return px


def resize(px, size):
    h, w = px.shape[:2]
    ys = (np.arange(size) * h / size).astype(int)
    xs = (np.arange(size) * w / size).astype(int)
    # усреднение 2x2 субпикселей — без «лесенки»
    acc = np.zeros((size, size, 4), np.float32)
    for dy in (0.25, 0.75):
        for dx in (0.25, 0.75):
            yy = np.clip(((np.arange(size) + dy) * h / size).astype(int), 0, h - 1)
            xx = np.clip(((np.arange(size) + dx) * w / size).astype(int), 0, w - 1)
            acc += px[yy][:, xx]
    return acc / 4


def render_labels(texts, w, h, size_px):
    """Подписи белым по центру ячеек w x h (Workbench). texts: [(x_центр, y_центр, текст)] в пикселях листа."""
    sc = bpy.context.scene
    clear_objects()
    eng = sc.render.engine
    sc.render.engine = "BLENDER_WORKBENCH"
    sc.display.shading.light = "FLAT"
    sc.display.shading.color_type = "SINGLE"
    sc.display.shading.single_color = (1, 1, 1)
    sc.view_settings.view_transform = "Standard"
    sc.render.resolution_x, sc.render.resolution_y = w, h
    cam_d = bpy.data.cameras.new("lab")
    cam_d.type = "ORTHO"
    cam_d.ortho_scale = float(max(w, h))
    cam = bpy.data.objects.new("lab", cam_d)
    cam.location = (w / 2, h / 2, 10)
    sc.collection.objects.link(cam)
    sc.camera = cam
    for x, y, t in texts:
        cu = bpy.data.curves.new("t", "FONT")
        cu.body = t
        cu.size = size_px
        cu.align_x = "CENTER"
        cu.align_y = "CENTER"
        o = bpy.data.objects.new("t", cu)
        o.location = (x, h - y, 0)
        sc.collection.objects.link(o)
    path = os.path.join(tempfile.gettempdir(), "vb_mr_labels.png")
    sc.render.filepath = path
    bpy.ops.render.render(write_still=True)
    sc.render.engine = eng
    px = load_rgba(path)
    for ob in list(bpy.data.objects):
        bpy.data.objects.remove(ob, do_unlink=True)
    return px


def make_sheet(mapping, sheet_path, cell):
    rows = [["sand"], ["nugget", "chunk"], ["ingot", "briquette"]]
    lw, th = int(cell * 0.75), int(cell * 0.22)
    W, H = lw + cell * len(RES_ORDER), th + cell * len(rows)
    sheet = np.zeros((H, W, 4), np.float32)
    sheet[..., :3] = 0.30
    sheet[..., 3] = 1
    by_key = {(v["shape"], v["resource"]): k for k, v in mapping.items()}
    for ri, shapes in enumerate(rows):
        for ci, res in enumerate(RES_ORDER):
            y0 = H - th - (ri + 1) * cell                 # пиксели Blender снизу вверх
            x0 = lw + ci * cell
            sheet[y0 + 2:y0 + cell - 2, x0 + 2:x0 + cell - 2, :3] = 0.42 if (ri + ci) % 2 == 0 else 0.39
            cls = next((by_key[(s, res)] for s in shapes if (s, res) in by_key), None)
            p = os.path.join(OUT, (cls or "") + ".png")
            if not cls or not os.path.exists(p):
                continue
            px = resize(load_rgba(p), cell - 16)
            a = px[..., 3:4]
            sub = sheet[y0 + 8:y0 + cell - 8, x0 + 8:x0 + cell - 8]
            sub[..., :3] = px[..., :3] * a + sub[..., :3] * (1 - a)
    texts = [(lw + (ci + 0.5) * cell, th * 0.5, RES_RU[r]) for ci, r in enumerate(RES_ORDER)]
    texts += [(lw * 0.5, th + (ri + 0.5) * cell, t.replace(" / ", "\n/ ")) for ri, t in enumerate(ROW_RU)]
    lab = render_labels(texts, W, H, th * 0.42)
    a = lab[..., 3:4] * lab[..., :1]
    sheet[..., :3] = sheet[..., :3] * (1 - a) + 0.95 * a
    save_rgba(sheet, sheet_path)
    print("SHEET", sheet_path, "%dx%d" % (W, H))


def main():
    mapping = json.load(open(MAPPING, encoding="utf-8"))
    os.makedirs(OUT, exist_ok=True)
    sc = setup_scene()
    for cls, info in mapping.items():
        if ONLY and cls not in ONLY:
            continue
        render_item(sc, cls, info)
    if opts.get("sheet"):
        make_sheet(mapping, opts["sheet"], int(opts.get("cell", 300)))


main()
