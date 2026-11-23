# -*- coding: utf-8 -*-
r"""Концепт экзоскелета Эхо (VB_EchoClothes, этап 3): весь комплект надетыми моделями (_m) в одной сцене, Blender Cycles.

  blender.exe -b --factory-startup --python ec_concept.py -- <папка вывода> [--variant=base|echo] [--views=front,back,left]
      [--size=1400] [--samples=96] [--night] [--parts=all|servo]

base — как сейчас (текстуры мода). echo — предложение: палитра Эхо (тёмный металл, ржавчина) + новые детали: две
энергоячейки на креплениях сервопривода (наклон --cell_tilt=12, отступ --cell_back=0.02), контейнер артефактов на бедре
(крепление бывшего кейса OOTS), светящиеся линзы маски, индикатор на поясе, свет по трубкам сервопривода:
--hose_debug (номера кусков), --hose_glow=0,1,16,17, --hose_cable=18,19,22,23 (гофру заменить гладкой светящейся
трубкой), --hose_hide=..., --seats (свои гнёзда ячеек вместо банок OOTS). Набор на утверждение — ТЗ_VB_EchoClothes.md
раздел 8. Новые детали — заготовка формы для модели.
Читает ODOL своей читалкой (.claude/tools/dayz-render/odol_read.py), материалы — как render_p3d.py.
"""
import math
import os
import subprocess
import sys

import bpy
import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
BS = chr(92)
EDIT = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
RENDER = os.path.join(EDIT, ".claude", "tools", "dayz-render")
sys.path.insert(0, RENDER)
import odol_read  # noqa: E402
import rvmat_read  # noqa: E402

CLOTHES = os.path.join(HERE, "..", "VB_EchoClothes")
IMAGE_TO_PAA = os.path.join("D:" + BS, "SteamLibrary", "steamapps", "common", "DayZ Tools", "Bin", "ImageToPAA",
                            "ImageToPAA.exe")
CACHE = os.path.join(RENDER, "texcache")

args = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
opts = {a.split("=", 1)[0][2:]: (a.split("=", 1)[1] if "=" in a else True) for a in args if a.startswith("--")}
OUTDIR = [a for a in args if not a.startswith("--")][0]
VARIANT = opts.get("variant", "base")
VIEWS = str(opts.get("views", "front,back,left")).split(",")
SIZE = int(opts.get("size", 1400))
SAMPLES = int(opts.get("samples", 96))
NIGHT = bool(opts.get("night", False))
PARTS = opts.get("parts", "all")
TEX_ECHO = bool(opts.get("tex_echo", False))         # брать готовые ретекстуры *_echo_co.paa (этап 3а), где есть
# --built=<папка exo_set распакованного PBO> — сервопривод, маска и шлем из собранного мода (этап 3б: исходники MLOD);
# --glow — свет собранной модели (выделение vbglow) включённым
BUILT = opts.get("built")
GLOW = bool(opts.get("glow", False))
# --helmet_concept — концепт шлема (этап 3б): свет в пазах дыхательной решётки, знак Эхо на лбу, индикаторы на
# боковых рельсах; со --glow стёкла горят (как в игре)
HELM_CONCEPT = bool(opts.get("helmet_concept", False))

X = os.path.join(CLOTHES, "exo_set")
# модель, выделение -> (текстура, материал) — как hiddenSelections конфига
SET = [
    ("helmet", os.path.join(HERE, "..", "исходники_моделей", "RoninHeavy.odol.p3d"),   # в моде с 11-23 — MLOD
     {"camoflage": ("vb_echoclothes\\exo_set\\helmet\\ronin_heavy\\data\\roninheavy_black.paa", "")}),
    ("mask", os.path.join(X, "Mask", "MaskGhost", "MaskGhost.p3d"),
     {"como": ("vb_echoclothes\\exo_set\\mask\\maskghost\\data\\maskghost_co.paa",
               "vb_echoclothes\\exo_set\\mask\\maskghost\\data\\maskghost.rvmat")}),
    ("vest", os.path.join(X, "Vest", "ExoVest", "ExoPlate_m.p3d"),
     {"como": ("vb_echoclothes\\exo_set\\vest\\exovest\\data\\exovest_co.paa",
               "vb_echoclothes\\exo_set\\vest\\exovest\\data\\exoplate.rvmat")}),
    ("servo", os.path.join(X, "Backpacks", "ExoPrivod", "ExoPrivod_m.p3d"),
     {"pr1": ("vb_echoclothes\\exo_set\\backpacks\\exoprivod\\data\\exoprivod1_co.paa",
              "vb_echoclothes\\exo_set\\backpacks\\exoprivod\\data\\exoprivod1.rvmat"),
      "pr2": ("vb_echoclothes\\exo_set\\backpacks\\exoprivod\\data\\exoprivod2_co.paa",
              "vb_echoclothes\\exo_set\\backpacks\\exoprivod\\data\\exoprivod2.rvmat")}),
    ("jacket", os.path.join(X, "Tops", "ExoJacket", "ExoJacket_m.p3d"),
     {"como": ("vb_echoclothes\\exo_set\\tops\\exojacket\\data\\exojacket_co.paa",
               "vb_echoclothes\\exo_set\\tops\\exojacket\\data\\exojacket.rvmat")}),
    ("pants", os.path.join(X, "Pants", "ExoPants", "ExoPants_m.p3d"),
     {"como": ("vb_echoclothes\\exo_set\\pants\\exopants\\data\\exopants_co.paa",
               "vb_echoclothes\\exo_set\\pants\\exopants\\data\\exopants.rvmat")}),
    ("gloves", os.path.join(X, "Gloves", "ExoGloves", "ExoGloves_m.p3d"),
     {"como": ("vb_echoclothes\\exo_set\\gloves\\exogloves\\data\\exogloves_co.paa",
               "vb_echoclothes\\exo_set\\gloves\\exogloves\\data\\exogloves.rvmat")}),
    ("boots", os.path.join(X, "Boots", "ExoBoots", "ExoBoots_m.p3d"),
     {"como": ("vb_echoclothes\\exo_set\\boots\\exoboots\\data\\exoboots_co.paa",
               "vb_echoclothes\\exo_set\\boots\\exoboots\\data\\exoboots.rvmat")}),
]
if BUILT:
    _rel = {"servo": ("Backpacks", "ExoPrivod", "ExoPrivod_m.p3d"), "mask": ("Mask", "MaskGhost", "MaskGhost.p3d"),
            "helmet": ("helmet", "ronin_heavy", "models", "RoninHeavy.p3d")}
    SET = [(n, os.path.join(BUILT, *_rel[n]) if n in _rel else p, s) for n, p, s in SET]
if PARTS != "all":
    SET = [s for s in SET if s[0] in PARTS.split("+")]


def resolve(vpath):
    vp = vpath.replace("/", BS).lstrip(BS)
    low = vp.lower()
    if low.startswith("vb_echoclothes" + BS):
        p = os.path.join(CLOTHES, vp[len("vb_echoclothes") + 1:])
        return p if os.path.exists(p) else None
    p = os.path.join("P:" + BS, vp)
    return p if os.path.exists(p) else None


def paa_png(vpath):
    src = resolve(vpath)
    if not src:
        print("WARN нет текстуры", vpath)
        return None
    os.makedirs(CACHE, exist_ok=True)
    dst = os.path.join(CACHE, vpath.replace(BS, "__").replace("/", "__").replace(":", "") + ".tga")
    if not os.path.exists(dst) or os.path.getmtime(dst) < os.path.getmtime(src):   # перегенерировать устаревший кэш
        subprocess.run([IMAGE_TO_PAA, src, dst], capture_output=True)
    return dst if os.path.exists(dst) else None


_img = {}


def load_img(vpath, noncolor=False):
    key = (vpath.lower(), noncolor)
    if key not in _img:
        path = paa_png(vpath)
        im = bpy.data.images.load(path) if path else None
        if im and noncolor:
            im.colorspace_settings.name = "Non-Color"
        _img[key] = im
    return _img[key]


def neon_mat(name, color, strength):
    m = bpy.data.materials.new(name)
    m.use_nodes = True
    b = m.node_tree.nodes["Principled BSDF"]
    b.inputs["Base Color"].default_value = (color[0] * 0.3, color[1] * 0.3, color[2] * 0.3, 1)
    b.inputs["Emission Color"].default_value = (color[0], color[1], color[2], 1)
    b.inputs["Emission Strength"].default_value = strength
    b.inputs["Roughness"].default_value = 0.3
    return m


def metal_mat(name, color, rough=0.45, metal=0.8):
    m = bpy.data.materials.new(name)
    m.use_nodes = True
    b = m.node_tree.nodes["Principled BSDF"]
    b.inputs["Base Color"].default_value = (color[0], color[1], color[2], 1)
    b.inputs["Roughness"].default_value = rough
    b.inputs["Metallic"].default_value = metal
    return m


_eye = []


def eye_mask(tex_im):
    """Маска свечения линз черепа MaskGhost: тёмные пиксели _co внутри двух овалов линз на пластине черепа
    (координаты файла 2048, сверху вниз). Строки снизу вверх, как у bpy.images."""
    if not _eye:
        n = tex_im.size[0]
        src = np.empty(n * n * 4, dtype=np.float32)
        tex_im.pixels.foreach_get(src)
        src = src.reshape(n, n, 4)
        lum = 0.3 * src[:, :, 0] + 0.59 * src[:, :, 1] + 0.11 * src[:, :, 2]
        im = bpy.data.images.new("eye_mask", n, n, alpha=False)
        yy, xx = np.mgrid[0:n, 0:n].astype(np.float64) * (2048.0 / n)
        yy = 2048.0 - yy                                 # строка снизу -> координата файла сверху
        region = np.zeros((n, n), dtype=bool)
        # ободки линз на пластине черепа + прорези глаз балаклавы под ней (их видно сквозь глазницы)
        for cx, cy, a, b, ang in ((277, 421, 125, 85, 0), (650, 420, 120, 85, 0),
                                  (1020, 1065, 80, 34, -15), (830, 1165, 80, 34, -34)):
            c_, s_ = math.cos(math.radians(ang)), math.sin(math.radians(ang))
            u = (xx - cx) * c_ + (yy - cy) * s_
            w = -(xx - cx) * s_ + (yy - cy) * c_
            region |= (u / a) ** 2 + (w / b) ** 2 < 1.0
        v = np.clip((0.2 - lum) / 0.08, 0, 1) * region
        px = np.dstack([v, v, v, np.ones_like(v)]).ravel()
        im.pixels.foreach_set(px.astype(np.float32))
        im.update()
        im.pack()
        im.colorspace_settings.name = "Non-Color"
        _eye.append(im)
    return _eye[0]


def make_material(tex, rvmat_path, echo):

    mat = bpy.data.materials.new((os.path.basename(tex.replace(BS, "/")) or "notex"))
    mat.use_nodes = True
    nt = mat.node_tree
    Nn, L = nt.nodes, nt.links
    bsdf = Nn["Principled BSDF"]
    uvn = Nn.new("ShaderNodeUVMap")
    uvn.uv_map = "UV"

    def tex_node(im):
        t = Nn.new("ShaderNodeTexImage")
        t.image = im
        L.new(uvn.outputs["UV"], t.inputs["Vector"])
        return t

    rv = {}
    if rvmat_path:
        rp = resolve(rvmat_path)
        if rp:
            try:
                rv = rvmat_read.read_rvmat(rp)
            except Exception as e:
                print("WARN rvmat", rvmat_path, e)
    stage_tex = {k: v.get("texture", "") for k, v in rv.items() if isinstance(v, dict) and k.startswith("Stage")}
    color_out = None
    glow_on = GLOW and ("vb_ec_neon_empty_co" in tex.lower()       # свет собранной модели (vbglow) — включённым
                        or ((HELM_CONCEPT or BUILT) and "glass_ca" in tex.lower()))   # стёкла шлема (как в игре)
    if glow_on:
        tex = tex.lower().replace("vb_ec_neon_empty_co", "vb_ec_neon_co")
        bsdf.inputs["Emission Color"].default_value = (1.0, 0.04, 0.05, 1)
        bsdf.inputs["Emission Strength"].default_value = 6.0 if NIGHT else 3.0
    if tex and not tex.startswith("#"):
        im = load_img(tex)
        if im:
            t = tex_node(im)
            color_out = t.outputs["Color"]
            if "_ca." in tex.lower():
                L.new(t.outputs["Alpha"], bsdf.inputs["Alpha"])
    if color_out is not None:
        if echo and "_echo_co" not in tex.lower():   # готовый ретекстур не перекрашиваем
            # палитра Эхо: темнее и холоднее, лёгкая ржавчина в средних тонах (концепт перекраски _co)
            hsv = Nn.new("ShaderNodeHueSaturation")
            hsv.inputs["Saturation"].default_value = 0.45
            hsv.inputs["Value"].default_value = 0.72
            if "maskghost" in tex.lower():                     # череп — графит, линзы светятся красным
                hsv.inputs["Saturation"].default_value = 0.2
                hsv.inputs["Value"].default_value = 0.42
                et = tex_node(eye_mask(im))
                em = Nn.new("ShaderNodeMath")
                em.operation = "MULTIPLY"
                em.inputs[1].default_value = 5.0 if NIGHT else 3.0
                L.new(et.outputs["Color"], em.inputs[0])
                bsdf.inputs["Emission Color"].default_value = (1.0, 0.08, 0.1, 1)
                L.new(em.outputs["Value"], bsdf.inputs["Emission Strength"])
            L.new(color_out, hsv.inputs["Color"])
            mix = Nn.new("ShaderNodeMix")
            mix.data_type = "RGBA"
            mix.blend_type = "OVERLAY"
            mix.inputs["Factor"].default_value = 0.35
            mix.inputs[7].default_value = (0.42, 0.18, 0.1, 1)      # ржавчина
            L.new(hsv.outputs["Color"], mix.inputs[6])
            L.new(mix.outputs[2], bsdf.inputs["Base Color"])
        else:
            L.new(color_out, bsdf.inputs["Base Color"])
    nohq = next((v for k, v in sorted(stage_tex.items()) if "_nohq" in v.lower()), None)
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
    smdi = next((v for v in stage_tex.values() if "_smdi" in v.lower()), None)
    if smdi and not smdi.startswith("#"):
        im = load_img(smdi, True)
        if im:
            t = tex_node(im)
            sps = Nn.new("ShaderNodeSeparateColor")
            L.new(t.outputs["Color"], sps.inputs["Color"])
            rough = Nn.new("ShaderNodeMapRange")
            rough.inputs["To Min"].default_value = 0.85
            rough.inputs["To Max"].default_value = 0.1
            L.new(sps.outputs["Blue"], rough.inputs["Value"])
            L.new(rough.outputs["Result"], bsdf.inputs["Roughness"])
            L.new(sps.outputs["Green"], bsdf.inputs["Specular IOR Level"])
    else:
        bsdf.inputs["Roughness"].default_value = 0.6
    return mat


# ---------------------------------------------------------------- сцена
bpy.ops.wm.read_factory_settings(use_empty=True)
scene = bpy.context.scene
scene.render.engine = "CYCLES"
try:
    cp = bpy.context.preferences.addons["cycles"].preferences
    for dt in ("OPTIX", "CUDA"):
        try:
            cp.compute_device_type = dt
            cp.get_devices()
            if any(d.type == dt for d in cp.devices):
                for d in cp.devices:
                    d.use = d.type == dt
                scene.cycles.device = "GPU"
                break
        except Exception:
            pass
except Exception:
    pass
scene.cycles.samples = SAMPLES
scene.cycles.use_denoising = True
scene.render.resolution_x = SIZE
scene.render.resolution_y = int(SIZE * 1.25)
scene.render.film_transparent = False
scene.render.image_settings.file_format = "PNG"
for vt in ("AgX", "Filmic"):
    try:
        scene.view_settings.view_transform = vt
        break
    except TypeError:
        pass

# DayZ (x, y вверх, z) -> Blender (x, y, z вверх) поворотом на 90° вокруг X: (x, -z, y) — без зеркала
def d2b_arr(a):
    a = np.array(a, dtype=np.float64)
    return np.stack([a[:, 0], -a[:, 2], a[:, 1]], axis=1)


ECHO = VARIANT == "echo"
# трубки сервопривода: --hose_debug — подсветить и пронумеровать все; --hose_glow=0,1 — светятся (неон Эхо);
# --hose_hide=4,5 — убрать
HOSE_DEBUG = bool(opts.get("hose_debug", False))
HOSE_GLOW = {int(x) for x in str(opts.get("hose_glow", "")).split(",") if x != ""}
HOSE_HIDE = {int(x) for x in str(opts.get("hose_hide", "")).split(",") if x != ""}
# --hose_cable=18,19 — шланг убрать и проложить по его маршруту гладкий светящийся силовой кабель
HOSE_CABLE = {int(x) for x in str(opts.get("hose_cable", "")).split(",") if x != ""}
HOSE_HIDE |= HOSE_CABLE
# --seats — старые банки OOTS под ячейками (сварены со спинным блоком, ниже плиты) вырезать, поставить новые гнёзда
SEATS = bool(opts.get("seats", False))


def in_old_can(c):
    """Центр грани (DayZ) внутри объёма старой банки под ячейкой."""
    return 0.04 < abs(c[0]) < 0.14 and 1.09 < c[1] < 1.175 and 0.12 < c[2] < 0.24


LABELS = []
CABLES = []                                        # (номер, осевая линия в координатах Blender)


def hose_centerline(lod, faces, nbins=12):
    """Осевая линия трубки: геодезическое расстояние по рёбрам от одного конца, точки по корзинам расстояния."""
    import heapq
    P = d2b_arr(lod["points"])
    key, wid = {}, {}
    for fi in faces:
        for v in lod["faces"][fi]:
            wid[v] = key.setdefault(tuple(np.round(P[v], 4)), len(key))
    W = np.zeros((len(key), 3))
    for v, w in wid.items():
        W[w] = P[v]
    adj = [[] for _ in range(len(key))]
    for fi in faces:
        f = [wid[v] for v in lod["faces"][fi]]
        for a, b in zip(f, f[1:] + f[:1]):
            if a != b:
                d = float(np.linalg.norm(W[a] - W[b]))
                adj[a].append((b, d))
                adj[b].append((a, d))

    def dijkstra(s):
        dist = np.full(len(key), np.inf)
        dist[s] = 0.0
        h = [(0.0, s)]
        while h:
            d, u = heapq.heappop(h)
            if d > dist[u]:
                continue
            for v, w in adj[u]:
                if d + w < dist[v]:
                    dist[v] = d + w
                    heapq.heappush(h, (d + w, v))
        return dist
    d0 = dijkstra(0)
    a = int(np.argmax(np.where(np.isfinite(d0), d0, -1)))
    da = dijkstra(a)
    ok = np.isfinite(da)
    edges = np.linspace(0, da[ok].max(), nbins + 1)
    line = []
    for i in range(nbins):
        m = ok & (da >= edges[i]) & (da <= edges[i + 1])
        if m.any():
            line.append(W[m].mean(0))
    line = np.array(line)
    for _ in range(4):                             # сгладить: гофра даёт рывки осевой линии, концы на месте
        line[1:-1] = 0.25 * line[:-2] + 0.5 * line[1:-1] + 0.25 * line[2:]
    return line


def hose_groups(lod, skip):
    """Связные куски меша (вершины склеены по позиции), длинные и тонкие — трубки. Порядок стабильный."""
    P = np.array(lod["points"], dtype=np.float64)
    key, vid = {}, np.zeros(len(P), dtype=np.int64)
    for i, p in enumerate(np.round(P, 4)):
        vid[i] = key.setdefault(tuple(p), len(key))
    par = list(range(len(key)))

    def find(a):
        while par[a] != a:
            par[a] = par[par[a]]
            a = par[a]
        return a
    fl_all = [(fi, f) for fi, f in enumerate(lod["faces"]) if fi not in skip]
    for fi, f in fl_all:
        r0 = find(vid[f[0]])
        for v in f[1:]:
            r = find(vid[v])
            if r != r0:
                par[r] = r0
    comp = {}
    for fi, f in fl_all:
        comp.setdefault(find(vid[f[0]]), []).append(fi)
    out = []
    for fl in comp.values():
        pts = P[sorted({v for fi in fl for v in lod["faces"][fi]})]
        c = pts.mean(0)
        sv = np.linalg.svd(pts - c, full_matrices=False)[1] / np.sqrt(len(pts))
        length = float(np.linalg.norm(pts.max(0) - pts.min(0)))
        if length > 0.1 and sv[1] < 0.03 and sv[1] / max(sv[0], 1e-6) < 0.4 and len(fl) >= 24:
            out.append({"faces": set(fl), "c": c, "n": len(fl)})
    out.sort(key=lambda r: (-r["n"], -round(float(r["c"][0]), 3)))
    return out


def hose_material(tex, gi):
    """Светящаяся трубка Эхо: тёмная оплётка, рёбра горят красным (свечение по яркости текстуры)."""
    mat = make_material(tex, "", True)
    nt = mat.node_tree
    bsdf = nt.nodes["Principled BSDF"]
    t = next(n for n in nt.nodes if n.type == "TEX_IMAGE")
    bw = nt.nodes.new("ShaderNodeRGBToBW")
    nt.links.new(t.outputs["Color"], bw.inputs["Color"])
    m = nt.nodes.new("ShaderNodeMath")
    m.operation = "MULTIPLY_ADD"
    m.inputs[1].default_value = 12.0 if NIGHT else 6.0
    m.inputs[2].default_value = 0.6 if NIGHT else 0.3
    nt.links.new(bw.outputs["Val"], m.inputs[0])
    bsdf.inputs["Emission Color"].default_value = (1.0, 0.03, 0.05, 1)
    nt.links.new(m.outputs["Value"], bsdf.inputs["Emission Strength"])
    return mat


def debug_material(gi):
    import colorsys
    mat = bpy.data.materials.new("hose_dbg_%d" % gi)
    mat.use_nodes = True
    b = mat.node_tree.nodes["Principled BSDF"]
    r, g, bb = colorsys.hsv_to_rgb((gi * 0.137) % 1.0, 0.9, 1.0)
    b.inputs["Base Color"].default_value = (r, g, bb, 1)
    b.inputs["Emission Color"].default_value = (r, g, bb, 1)
    b.inputs["Emission Strength"].default_value = 2.0
    return mat


LODS = {}
PROXY_FACES = set()                                # грани прокси сервопривода
USED = {}                                          # точки видимых граней (без прокси)
all_pts = []
for name, path, sels in SET:
    try:
        lod = odol_read.read_visual_lod(path, 0)
    except Exception as e:                        # шлем IMP — ODOL v53, читалка пока не берёт (этап 3)
        print("WARN модель не прочитана, пропускаю:", name, e)
        continue
    LODS[name] = lod
    by = {s["name"].lower(): s for s in lod["selections"]}
    for sn, (tx, mt) in sels.items():
        if TEX_ECHO and tx.lower().endswith(".paa"):
            base = tx[:-len("_co.paa")] if tx.lower().endswith("_co.paa") else tx[:-len(".paa")]
            tx2 = base + "_echo_co.paa"                 # у шлема исходник без _co: RoninHeavy_Black_echo_co.paa
            if resolve(tx2):
                tx = tx2
        s = by.get(sn)
        if not s:
            print("WARN нет выделения", name, sn)
            continue
        for fi in s["face_set"]:
            lod["face_tex"][fi] = tx
            if mt:
                lod["face_mat"][fi] = mt
    P = d2b_arr(lod["points"])
    N = d2b_arr(lod["normals"])
    UV = np.array(lod["uv"], dtype=np.float64)
    faces, src = [], []
    proxy_faces = set()                            # треугольники креплений (выделения proxy:*) — не рисовать
    for s in lod["selections"]:
        if s["name"].lower().startswith("proxy:"):
            proxy_faces.update(s["face_set"])
    hose_of = {}                                   # грань -> номер трубки (только сервопривод)
    if name == "servo" and (HOSE_DEBUG or HOSE_GLOW or HOSE_HIDE):
        for gi, g in enumerate(hose_groups(lod, proxy_faces)):
            for fi in g["faces"]:
                hose_of[fi] = gi
            if gi in HOSE_CABLE:
                CABLES.append((gi, hose_centerline(lod, sorted(g["faces"]))))
            if HOSE_DEBUG:
                LABELS.append((gi, d2b_arr([g["c"]])[0]))
                print("HOSE %d граней %d центр DayZ %s" % (gi, g["n"], np.round(g["c"], 3)))
    RAW = np.array(lod["points"], dtype=np.float64)
    for fi, f in enumerate(lod["faces"]):
        if fi in proxy_faces or hose_of.get(fi, -1) in HOSE_HIDE:
            continue
        if SEATS and name == "servo" and in_old_can(RAW[list(f)].mean(0)):
            continue
        if len(set(f)) == len(f) >= 3:
            faces.append(list(f))
            src.append(fi)
    agree = 0
    for f in faces[:4000]:
        a, b, c = P[f[0]], P[f[1]], P[f[2]]
        agree += 1 if np.dot(np.cross(b - a, c - a), N[f[0]] + N[f[1]] + N[f[2]]) > 0 else -1
    if agree < 0:
        faces = [f[::-1] for f in faces]
    me = bpy.data.meshes.new(name)
    me.from_pydata(P.tolist(), [], faces)
    uvl = me.uv_layers.new(name="UV")
    luv = []
    for f in faces:
        for vi in f:
            luv.extend([UV[vi, 0], 1.0 - UV[vi, 1]])
    uvl.data.foreach_set("uv", luv)
    mats, keys = {}, []
    if name == "servo":
        PROXY_FACES.update(proxy_faces)
    for k_, fi in enumerate(src):
        key = (lod["face_tex"][fi] or "", lod["face_mat"][fi] or "")
        gi = hose_of.get(fi, -1)
        if HOSE_DEBUG and gi >= 0:
            key = ("DBG", gi)
        elif gi in HOSE_GLOW:
            key = ("HOSE", key[0])
        if key not in mats:
            mats[key] = len(mats)
            if key[0] == "DBG":
                me.materials.append(debug_material(gi))
            elif key[0] == "HOSE":
                me.materials.append(hose_material(key[1], gi))
            else:
                me.materials.append(make_material(key[0], key[1], ECHO))
        keys.append(mats[key])
    me.polygons.foreach_set("material_index", keys)
    me.polygons.foreach_set("use_smooth", [True] * len(faces))
    me.update()
    ob = bpy.data.objects.new(name, me)
    scene.collection.objects.link(ob)
    used = sorted({v for f in faces for v in f})
    USED[name] = P[used]
    all_pts.append(P[used])
    print("MODEL", name, len(P), "точек", len(faces), "граней")

# ---------------------------------------------------------------- новые детали (концепт)
RED = (1.0, 0.08, 0.1)
TURQ = (0.1, 0.95, 0.85)


def add_obj(name, mesh_fn, mat, loc, rot=(0, 0, 0)):
    mesh_fn()
    ob = bpy.context.active_object
    ob.name = name
    ob.location = loc
    ob.rotation_euler = rot
    ob.data.materials.append(mat)
    return ob


def d2b(p):
    return (p[0], -p[2], p[1])


if ECHO:
    neon_r = neon_mat("neon_red", RED, 18 if NIGHT else 9)
    neon_t = neon_mat("neon_turq", TURQ, 18 if NIGHT else 9)
    dark = metal_mat("dark_metal", (0.05, 0.05, 0.055), 0.35, 0.9)
    rust = metal_mat("steel", (0.42, 0.43, 0.46), 0.35, 0.9)      # бывшая ржавчина -> сталь (без ржавчины, 11-23)
    glass = bpy.data.materials.new("glass")
    glass.use_nodes = True
    gb = glass.node_tree.nodes["Principled BSDF"]
    gb.inputs["Base Color"].default_value = (0.6, 0.1, 0.1, 1)
    gb.inputs["Transmission Weight"].default_value = 0.8
    gb.inputs["Roughness"].default_value = 0.05
    gb.inputs["Emission Color"].default_value = (RED[0], RED[1], RED[2], 1)
    gb.inputs["Emission Strength"].default_value = 3

    # 1) энергоячейки на креплениях сервопривода (позиции прокси vb_exocell_slot_1/2): Эхо и синтез.
    #    Форма — как настоящая VB_EchoCell: колпачки, 4 стойки, ядро светится (видно, есть ли заряд), кольцо уровня
    def cyl(name, r, h, mat, loc, rot=(0, 0, 0), verts=24):
        return add_obj(name, lambda: bpy.ops.mesh.primitive_cylinder_add(vertices=verts, radius=r, depth=h), mat, loc, rot)

    tier_y = neon_mat("tier_yellow", (1.0, 0.8, 0.1), 6)
    # наклон ячеек наружу (верх от центральной трубы) и отступ от спинной плиты, чтобы не входили в модель (11-23);
    # то же самое потом — поворотом матриц прокси vb_exocell_slot_1/2 в ExoPrivod_m
    TILT = math.radians(float(opts.get("cell_tilt", 12)))
    CBACK = float(opts.get("cell_back", 0.02))
    for i, (px, mat_core) in enumerate(((0.088, neon_r), (-0.089, neon_t))):
        b0 = d2b((px, 1.164, 0.176))
        th = TILT if px > 0 else -TILT

        def at(dx, dy, dz, b0=b0, th=th):
            return (b0[0] + dx * math.cos(th) + dz * math.sin(th), b0[1] - CBACK + dy,
                    b0[2] - dx * math.sin(th) + dz * math.cos(th))
        rot = (0, th, 0)
        cyl("cell_cap_lo_%d" % i, 0.036, 0.02, dark, at(0, 0, 0.01), rot)
        cyl("cell_core_%d" % i, 0.022, 0.15, mat_core, at(0, 0, 0.095), rot)
        for k_ in range(4):
            a = math.pi / 4 + k_ * math.pi / 2
            cyl("cell_bar_%d_%d" % (i, k_), 0.005, 0.15, dark, at(0.03 * math.cos(a), 0.03 * math.sin(a), 0.095), rot,
                verts=8)
        cyl("cell_cap_hi_%d" % i, 0.036, 0.02, dark, at(0, 0, 0.18), rot)
        add_obj("cell_tier_%d" % i, lambda: bpy.ops.mesh.primitive_torus_add(major_radius=0.036, minor_radius=0.004),
                tier_y if i == 0 else mat_core, at(0, 0, 0.19), rot)
        if SEATS:
            # новое гнездо (вместо ржавой банки OOTS), наклонено вместе с ячейкой: стакан с контактным кольцом
            # (светится цветом энергии, что идёт через гнездо), ржавый бандаж, две боковые направляющие,
            # верхний фиксатор с защёлкой, кронштейн к спинному блоку
            cyl("seat_cup_%d" % i, 0.044, 0.06, dark, at(0, 0, 0.0), rot, verts=32)
            add_obj("seat_band_%d" % i, lambda: bpy.ops.mesh.primitive_torus_add(major_radius=0.045, minor_radius=0.005),
                    rust, at(0, 0, -0.016), rot)
            add_obj("seat_contact_%d" % i, lambda: bpy.ops.mesh.primitive_torus_add(major_radius=0.04, minor_radius=0.003),
                    mat_core, at(0, 0, 0.031), rot)
            for sx in (-1, 1):
                add_obj("seat_rail_%d" % i, lambda: bpy.ops.mesh.primitive_cube_add(size=1), dark,
                        at(sx * 0.047, 0.0, 0.1), rot)
                bpy.context.active_object.scale = (0.008, 0.012, 0.15)
            add_obj("seat_clamp_%d" % i, lambda: bpy.ops.mesh.primitive_torus_add(major_radius=0.043, minor_radius=0.006),
                    dark, at(0, 0, 0.168), rot)
            add_obj("seat_latch_%d" % i, lambda: bpy.ops.mesh.primitive_cube_add(size=1), rust,
                    at(0, -0.046, 0.168), rot)
            bpy.context.active_object.scale = (0.022, 0.01, 0.018)
            add_obj("seat_bracket_%d" % i, lambda: bpy.ops.mesh.primitive_cube_add(size=1), dark,
                    at(0, 0.05, 0.0), rot)
            bpy.context.active_object.scale = (0.05, 0.05, 0.05)
            # опора стакана до поясного блока (верх пояса ~1,085 по высоте DayZ) и пружина-амортизатор вокруг неё
            bot = at(0, 0, -0.03)
            top_z, low_z = bot[2], 1.085
            cyl("seat_post_%d" % i, 0.011, top_z - low_z + 0.01, dark, (bot[0], bot[1], (top_z + low_z) / 2), verts=12)
            add_obj("seat_spring_%d" % i, lambda: bpy.ops.mesh.primitive_torus_add(major_radius=0.017, minor_radius=0.003),
                    rust, (bot[0], bot[1], (top_z + low_z) / 2))

    # 2) контейнер артефактов на бедре (крепление vb_ec_artcase_box): капсула-экран. Тёмный корпус, ржавые обручи,
    #    узкое окно наружу (свет артефакта внутри), крышка с неоновым кольцом, скоба к поясу
    art = d2b((-0.227, 0.984, -0.127))
    ac = (art[0] - 0.03, art[1], art[2] - 0.08)
    out_dir = (-0.8, 0.6)                                 # наружу-вперёд для левого бедра
    cyl("art_body", 0.055, 0.19, dark, ac, verts=32)
    for dz in (-0.075, 0.075):
        add_obj("art_hoop", lambda: bpy.ops.mesh.primitive_torus_add(major_radius=0.057, minor_radius=0.007),
                rust, (ac[0], ac[1], ac[2] + dz))
    add_obj("art_window", lambda: bpy.ops.mesh.primitive_cube_add(size=1), glass,
            (ac[0] + out_dir[0] * 0.05, ac[1] + out_dir[1] * 0.05, ac[2]),
            (0, 0, math.atan2(out_dir[1], out_dir[0])))
    bpy.context.active_object.scale = (0.012, 0.026, 0.11)
    cyl("art_lid", 0.06, 0.025, dark, (ac[0], ac[1], ac[2] + 0.105), verts=32)
    add_obj("art_glow_ring", lambda: bpy.ops.mesh.primitive_torus_add(major_radius=0.045, minor_radius=0.004),
            neon_r, (ac[0], ac[1], ac[2] + 0.119))
    cyl("art_cap", 0.02, 0.015, rust, (ac[0], ac[1], ac[2] + 0.125), verts=16)
    add_obj("art_bracket", lambda: bpy.ops.mesh.primitive_cube_add(size=1), dark, (ac[0] + 0.06, ac[1], ac[2] + 0.05))
    bpy.context.active_object.scale = (0.05, 0.03, 0.05)

    # 3) кольца-манжеты на руках и ногах убраны (висели криво, 11-23): свечение теперь идёт по родным трубкам
    #    сервопривода (--hose_glow), лишние трубки убираются (--hose_hide)
    servo = LODS.get("servo")
    # силовые кабели на месте убранных гофрированных шлангов: гладкая чёрная оболочка, светящаяся жила видна
    # в прорезях (чередование тёмных хомутов и света)
    cable_m = neon_mat("cable_red", (1.0, 0.03, 0.05), 7 if NIGHT else 4)
    for gi, line in CABLES:
        cu = bpy.data.curves.new("cable_%d" % gi, "CURVE")
        cu.dimensions = "3D"
        cu.bevel_depth = 0.0065
        cu.bevel_resolution = 3
        sp = cu.splines.new("NURBS")
        sp.points.add(len(line) - 1)
        for i, p in enumerate(line):
            sp.points[i].co = (p[0], p[1], p[2], 1)
        sp.use_endpoint_u = True
        sp.order_u = 3
        ob = bpy.data.objects.new("cable_%d" % gi, cu)
        cu.materials.append(cable_m)
        scene.collection.objects.link(ob)
        for k in (len(line) // 3, 2 * len(line) // 3):     # два хомута
            d = line[k + 1] - line[k - 1]
            from mathutils import Vector
            rot = Vector((0, 0, 1)).rotation_difference(Vector(tuple(d / np.linalg.norm(d)))).to_euler()
            cyl("cable_clamp", 0.0095, 0.012, dark, tuple(line[k]), tuple(rot), verts=12)
        for p in (line[0], line[-1]):                       # разъёмы на концах
            add_obj("cable_plug", lambda: bpy.ops.mesh.primitive_uv_sphere_add(radius=0.012, segments=12, ring_count=8),
                    dark, tuple(p))
        print("CABLE", gi, len(line), "точек")
    if servo:
        # индикатор заряда на поясном блоке сзади (решётка по центру пояса): горит, пока привод активен
        U = USED["servo"]
        m = (np.abs(U[:, 0]) < 0.05) & (U[:, 2] > 0.93) & (U[:, 2] < 1.06)
        if m.any():
            B = U[m]
            ymin = B[:, 1].min()
            zc = float(np.median(B[B[:, 1] < ymin + 0.012][:, 2]))
            add_obj("belt_led", lambda: bpy.ops.mesh.primitive_cube_add(size=1), neon_r, (0.0, ymin - 0.004, zc - 0.035))
            bpy.context.active_object.scale = (0.1, 0.006, 0.007)
            print("BELT", round(float(ymin), 3), round(zc, 3))

    # 4) полоса забрала шлема и знак семейства на жилете (кольцо резонатора)
    helm = LODS.get("helmet")
    if helm and not (BUILT or HELM_CONCEPT):         # старая идея; шлем по одобренному концепту — без полосы (11-23)
        HP = USED["helmet"]
        lo, hi = HP.min(0), HP.max(0)
        add_obj("visor_strip", lambda: bpy.ops.mesh.primitive_cube_add(size=1), neon_r,
                ((lo[0] + hi[0]) / 2, hi[1] + 0.004, lo[2] + (hi[2] - lo[2]) * 0.52))
        bpy.context.active_object.scale = ((hi[0] - lo[0]) * 0.55, 0.006, 0.008)
    # 5) линзы маски: глазницы черепа сквозные — вставляем свои светящиеся линзы по месту ободков (грани, чьи UV
    #    попадают в овалы линз на пластине черепа)
    mask = LODS.get("mask")
    lens_m = neon_mat("lens_red", (1.0, 0.03, 0.05), 3 if NIGHT else 2.5)
    if mask:
        MP = d2b_arr(mask["points"])
        UVm = np.array(mask["uv"], dtype=np.float64) * 2048.0
        groups = {0: [], 1: []}
        for f in mask["faces"]:
            cu, cv = UVm[list(f)].mean(0)
            for gi, (cx, cy) in enumerate(((277, 421), (650, 420))):
                if ((cu - cx) / 130) ** 2 + ((cv - cy) / 90) ** 2 < 1.0:
                    groups[gi].extend(f)
        for gi, vs in groups.items():
            if not vs:
                continue
            pts = MP[sorted(set(vs))]
            lo_, hi_ = pts.min(0), pts.max(0)
            c = (lo_ + hi_) / 2
            add_obj("mask_lens_%d" % gi, lambda: bpy.ops.mesh.primitive_uv_sphere_add(radius=1, segments=24, ring_count=12),
                    lens_m, (c[0], c[1] - 0.004, c[2]))
            bpy.context.active_object.scale = ((hi_[0] - lo_[0]) * 0.42, 0.006, (hi_[2] - lo_[2]) * 0.42)
            print("LENS", gi, np.round(c, 3), np.round(hi_ - lo_, 3))
    # знак на груди убран: метку Эхо рисовать в текстуре жилета

# ---------------------------------------------------------------- концепт шлема (этап 3б)
if HELM_CONCEPT and LODS.get("helmet"):
    HN = neon_mat("helm_neon", RED, 14 if NIGHT else 7)
    HPAINT = bpy.data.materials.new("helm_paint")
    HPAINT.use_nodes = True
    _b = HPAINT.node_tree.nodes["Principled BSDF"]
    _b.inputs["Base Color"].default_value = (0.35, 0.02, 0.03, 1)
    _b.inputs["Roughness"].default_value = 0.6
    HU = USED["helmet"]                                         # точки шлема (Blender: x, -z DayZ, y DayZ)

    def front_y(x, y):
        """Передняя поверхность шлема (Blender y — минимальный -z DayZ... лицо смотрит в +Y) в точке (x, высота)."""
        m = (np.abs(HU[:, 0] - x) < 0.006) & (np.abs(HU[:, 2] - y) < 0.01)
        return float(HU[m][:, 1].max()) if m.any() else None
    # 1) и 2) по поверхности шлема (tools/ec_helmet_geom.py — тот же код соберёт модель): свет в прорезях решётки
    #    (контуры — по текстуре, спроецированной спереди; вставка на видимой поверхности) и знак Эхо на висках
    #    (кольцо и пульс, спроецированы на боковую поверхность)
    sys.path.insert(0, HERE)
    import ec_helmet_geom as HG

    def gmesh(name, gm, mat):
        me = bpy.data.meshes.new(name)
        me.from_pydata(d2b_arr(np.array(gm.pts)).tolist(), [], [list(f) for f in gm.faces])
        me.update()
        ob = bpy.data.objects.new(name, me)
        ob.data.materials.append(mat)
        scene.collection.objects.link(ob)
        return ob
    vents, _slots = HG.grill_glow_meshes()
    for k, gm in enumerate(vents):
        gmesh("helm_vent_%d" % k, gm, HN)
    _P, _N, _T = HG.load()
    for k, gm in enumerate(HG.side_emblems(_P, _N, _T)):
        gmesh("helm_emblem_%d" % k, gm, HPAINT)
    print("HELMET прорезей", len(vents))
    # 3) индикаторы на боковых рельсах (крайние точки шлема по бокам, высота 1,66–1,69)
    for side in (-1, 1):
        m = (HU[:, 0] * side > 0.118) & (HU[:, 2] > 1.65) & (HU[:, 2] < 1.70)
        if m.any():
            c = HU[m].mean(0)
            add_obj("helm_side", lambda: bpy.ops.mesh.primitive_cube_add(size=1), HN,
                    (side * (HU[m][:, 0].max() * side + 0.002), c[1], c[2]))
            bpy.context.active_object.scale = (0.003, 0.03, 0.006)

# ---------------------------------------------------------------- свет, камера, ракурсы
ALL = np.vstack(all_pts)
lo, hi = ALL.min(0), ALL.max(0)
center = (lo + hi) / 2
world = bpy.data.worlds.new("w")
world.use_nodes = True
bg = world.node_tree.nodes["Background"]
bg.inputs["Color"].default_value = (0.012, 0.013, 0.016, 1) if NIGHT else (0.18, 0.19, 0.21, 1)
bg.inputs["Strength"].default_value = 1.0
scene.world = world
tgt = bpy.data.objects.new("tgt", None)
tgt.location = tuple(center)
scene.collection.objects.link(tgt)


def light(name, loc, power, size, color):
    ld = bpy.data.lights.new(name, "AREA")
    ld.energy = power
    ld.size = size
    ld.color = color
    o = bpy.data.objects.new(name, ld)
    o.location = loc
    c = o.constraints.new("TRACK_TO")
    c.target = tgt
    c.track_axis = "TRACK_NEGATIVE_Z"
    c.up_axis = "UP_Y"
    scene.collection.objects.link(o)


k = 0.25 if NIGHT else 1.0
light("key", (center[0] - 2.5, center[1] - 3.0, center[2] + 2.5), 900 * k, 3, (1, 0.96, 0.9))
light("fill", (center[0] + 3.0, center[1] - 1.5, center[2] + 0.8), 300 * k, 3, (0.85, 0.9, 1))
light("rim", (center[0] + 1.0, center[1] + 3.0, center[2] + 2.0), 700 * k, 2, (0.9, 0.9, 1))

cam_d = bpy.data.cameras.new("cam")
cam_d.lens = 60
cam = bpy.data.objects.new("cam", cam_d)
scene.collection.objects.link(cam)
cc = cam.constraints.new("TRACK_TO")
cc.target = tgt
cc.track_axis = "TRACK_NEGATIVE_Z"
cc.up_axis = "UP_Y"
scene.camera = cam
for gi, pos in LABELS:                             # номера трубок (--hose_debug)
    cu = bpy.data.curves.new("lbl_%d" % gi, "FONT")
    cu.body = str(gi)
    cu.size = 0.045
    cu.align_x = "CENTER"
    lo_ = bpy.data.objects.new("lbl_%d" % gi, cu)
    lo_.location = tuple(pos)
    lm = bpy.data.materials.new("lbl")
    lm.use_nodes = True
    lb = lm.node_tree.nodes["Principled BSDF"]
    lb.inputs["Emission Color"].default_value = (1, 1, 1, 1)
    lb.inputs["Emission Strength"].default_value = 8
    cu.materials.append(lm)
    tc = lo_.constraints.new("TRACK_TO")
    tc.target = cam
    tc.track_axis = "TRACK_Z"
    tc.up_axis = "UP_Y"
    lo_.show_in_front = True
    scene.collection.objects.link(lo_)
height = float(hi[2] - lo[2])
dist = height * 2.1 if PARTS == "all" else max(float(np.linalg.norm(hi - lo)) * 2.2, 1.2)
AZ = {"front": 0, "back": 180, "left": 90, "right": -90, "front34": 30, "back34": 150}
os.makedirs(OUTDIR, exist_ok=True)
for v in VIEWS:
    az = math.radians(AZ.get(v, 180))
    # спина — -Y Blender (ячейки с z DayZ = +0,176 на снимке стенда видны сзади), лицо — +Y
    cam.location = (center[0] + dist * math.sin(az), center[1] + dist * math.cos(az), center[2] + height * 0.12)
    out = os.path.join(OUTDIR, "exo_%s_%s%s.png" % (VARIANT, v, "_night" if NIGHT else ""))
    scene.render.filepath = out
    bpy.ops.render.render(write_still=True)
    print("RENDERED", out)
