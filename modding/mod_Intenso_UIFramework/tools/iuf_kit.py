"""INT_UIFramework design kit: tokens + layout building blocks (Python side).

One source for the look: the static design mockups (tools/mock_*.py, loaded
hot from $profile: for fast iteration) and the component layout generator use
the same tokens and blocks, so what is approved in a mockup is what ships.

Everything is authored in exact pixels at 1080p (the runtime rescales with
INT_UIF_Scale where needed). Colours are RGBA floats 0..1 (layout format).
"""

TEX = "INT_UIFramework/gui/textures/"
FONT = "gui/fonts/sdf_MetronBook24"
FONT_BIG = "gui/fonts/sdf_MetronBook72"
SET = "iuf_ui"


def hexc(h, a=1.0):
    h = h.lstrip("#")
    return (int(h[0:2], 16) / 255.0, int(h[2:4], 16) / 255.0, int(h[4:6], 16) / 255.0, a)


def alpha(c, a):
    return (c[0], c[1], c[2], a)


# --- tokens -----------------------------------------------------------------------------
C = {
    "overlay": hexc("#05070A", 0.66),
    "window": hexc("#101316", 0.985),
    "window_border": (1, 1, 1, 0.085),
    "panel": hexc("#15191D"),
    "panel_border": (1, 1, 1, 0.065),
    "tile": hexc("#1A1F24"),
    "tile_hover": hexc("#21272D"),
    "tile_border": (1, 1, 1, 0.055),
    "sheen": (1, 1, 1, 0.05),
    "text": hexc("#ECEFF2"),
    "text2": hexc("#A2AAB3"),
    "text3": hexc("#747C86"),
    "teal": hexc("#4FD8B6"),
    "amber": hexc("#F5A524"),
    "amber_text": hexc("#F8BC5A"),
    "red": hexc("#F26666"),
    "green": hexc("#62D6A0"),
    "blue": hexc("#69A9FF"),
    "violet": hexc("#A792FF"),
    "on_amber": hexc("#1C1406"),
    "shadow": (0, 0, 0, 0.62),
}


def soft(key, bg=0.12, border=0.32):
    """Tinted surface for a colour: (fill, border) pair."""
    return alpha(C[key], bg), alpha(C[key], border)


# --- DSL -----------------------------------------------------------------------------------
class px(float):
    """A pixel value: marks its axis as exact."""


def W(cls, name, pos=(0, 0), size=(1, 1), children=None, **attrs):
    return {"cls": cls, "name": name, "pos": pos, "size": size,
            "children": list(children or []), "attrs": attrs}


def fmt(v):
    v = float(v)
    if v == int(v):
        return str(int(v))
    return ("%.4f" % v).rstrip("0")


def emit(node, ind=0):
    p = " " * ind
    out = ["%s%s %s {" % (p, node["cls"], node["name"])]
    x, y = node["pos"]
    w, h = node["size"]
    out.append("%s hexactpos %d" % (p, isinstance(x, px)))
    out.append("%s vexactpos %d" % (p, isinstance(y, px)))
    out.append("%s hexactsize %d" % (p, isinstance(w, px)))
    out.append("%s vexactsize %d" % (p, isinstance(h, px)))
    out.append("%s position %s %s" % (p, fmt(x), fmt(y)))
    out.append("%s size %s %s" % (p, fmt(w), fmt(h)))
    for k, v in node["attrs"].items():
        if k == "image0" and v == "":
            continue
        key = k.replace("__", " ")
        if " " in key:
            key = '"%s"' % key
        if isinstance(v, tuple):
            v = " ".join(fmt(c) for c in v)
        elif isinstance(v, str) and not v.startswith("@"):
            v = '"%s"' % v
        elif isinstance(v, str):
            v = v[1:]
        elif isinstance(v, bool):
            v = int(v)
        out.append("%s %s %s" % (p, key, v))
    if node["children"]:
        out.append("%s {" % p)
        for c in node["children"]:
            out += emit(c, ind + 2)
        out.append("%s }" % p)
    out.append("%s}" % p)
    return out


def write_layout(node, path):
    body = "\n".join(emit(node)) + "\n"
    assert body.count("{") == body.count("}")
    with open(path, "w", encoding="utf-8", newline="\n") as f:
        f.write(body)
    return body


# --- primitives (exact px) --------------------------------------------------------------------
def P(x, y):
    return (px(x), px(y))


def frame(name, x, y, w, h, children=None, **a):
    return W("FrameWidgetClass", name, P(x, y), P(w, h), children, **a)


def shape(name, x, y, w, h, style, color, **a):
    """9-slice shape (ButtonWidget used as a drawable), decorative."""
    a.update({"text": "", "style": "@" + style, "color": color, "ignorepointer": 1})
    return W("ButtonWidgetClass", name, P(x, y), P(w, h), **a)


def image(name, x, y, w, h, img, color, **a):
    a.update({"image0": img, "mode": "@blend", "src__alpha": 1, "stretch": 1,
              "color": color, "ignorepointer": 1})
    return W("ImageWidgetClass", name, P(x, y), P(w, h), **a)


def cell(cell_name):
    return "set:%s image:%s" % (SET, cell_name)


def icon_img(name, x, y, size, icon, color):
    return image(name, x, y, size, size, cell("iuf_icon_" + icon), color)


def text(name, x, y, w, h, value, size, color, halign="left", spacing=0, font=FONT, **a):
    a.update({"text": value, "font": font, "exact__text": 1, "exact__text__size": int(size),
              "text__valign": "@center", "color": color, "ignorepointer": 1})
    if halign != "left":
        a["text__halign"] = "@" + halign
    if spacing:
        a["text__spacing"] = (spacing, 0)
    return W("TextWidgetClass", name, P(x, y), P(w, h), **a)


def mtext(name, x, y, w, h, value, size, color, **a):
    a.update({"text": value, "font": FONT, "exact__text": 1, "exact__text__size": int(size),
              "wrap": 1, "color": color, "ignorepointer": 1})
    return W("MultilineTextWidgetClass", name, P(x, y), P(w, h), **a)


def radius_style(r, outline=False):
    return ("IufO%d" if outline else "IufR%d") % r


def pill_style(h, outline=False):
    r = int(h // 2)
    for cand in (20, 18, 16, 14, 12, 10, 8, 6, 4, 3):
        if r >= cand:
            r = cand
            break
    return ("IufPO%d" if outline else "IufP%d") % r


def box(name, x, y, w, h, r, fill, border=None, children=None, sheen=False):
    """Rounded surface: fill + optional 1 px border (+ optional top sheen)."""
    kids = [shape(name + "Fill", 0, 0, w, h, radius_style(r), fill)]
    if sheen:
        kids.append(image(name + "Sheen", r, 0, w - 2 * r, 1, cell("iuf_grad_band"), C["sheen"]))
    if border:
        kids.append(shape(name + "Border", 0, 0, w, h, radius_style(r, True), border))
    kids += children or []
    return frame(name, x, y, w, h, kids)


def pill(name, x, y, w, h, fill, border=None, children=None):
    kids = [shape(name + "Fill", 0, 0, w, h, pill_style(h), fill)]
    if border:
        kids.append(shape(name + "Border", 0, 0, w, h, pill_style(h, True), border))
    kids += children or []
    return frame(name, x, y, w, h, kids)


def caption(name, x, y, value, w=300, color=None):
    return text(name, x, y, w, 14, value, 11, color or C["text3"], spacing=1)


def chip(name, x, y, value, key, w=None, h=22, icon=None):
    """Soft tinted chip: tinted fill + border + coloured text (+ icon)."""
    fill, border = soft(key, 0.13, 0.34)
    tw = w or (len(value) * 7 + 22 + (16 if icon else 0))
    kids = []
    tx = 10
    if icon:
        kids.append(icon_img(name + "Icon", 8, (h - 12) / 2.0, 12, icon, C[key]))
        tx = 24
    kids.append(text(name + "Text", tx, 0, tw - tx - 8, h, value, 11,
                     C["amber_text"] if key == "amber" else C[key]))
    return pill(name, x, y, tw, h, fill, border, kids), tw


def keycap(name, x, y, key, label):
    kw = max(26, len(key) * 8 + 14)
    cap = box(name + "Cap", 0, 0, kw, 22, 4, (1, 1, 1, 0.06), (1, 1, 1, 0.12),
              [text(name + "Key", 0, 0, kw, 22, key, 11, C["text2"], halign="center")])
    lab = text(name + "Label", kw + 8, 0, 200, 22, label, 12, C["text3"])
    return frame(name, x, y, kw + 8 + len(label) * 7 + 10, 22, [cap, lab]), kw + 8 + len(label) * 7 + 22


def avatar(name, x, y, size, letter, key):
    fill = alpha(C[key], 0.16)
    return frame(name, x, y, size, size, [
        image(name + "Bg", 0, 0, size, size, cell("iuf_circle64"), fill),
        image(name + "Ring", 0, 0, size, size, cell("iuf_ringline64"), alpha(C[key], 0.38)),
        text(name + "Letter", 0, 0, size, size, letter, int(size * 0.42),
             C["amber_text"] if key == "amber" else C[key], halign="center"),
    ])


def dot(name, x, y, size, key, glow=True):
    kids = []
    if glow:
        g = size * 3
        kids.append(image(name + "Glow", (size - g) / 2.0, (size - g) / 2.0, g, g, cell("iuf_radial"),
                          alpha(C[key], 0.45)))
    kids.append(image(name + "Core", 0, 0, size, size, cell("iuf_circle16"), C[key]))
    return frame(name, x, y, size, size, kids)


def ring(name, x, y, size, progress, key, track=0.07, mask=None):
    return frame(name, x, y, size, size, [
        image(name + "Track", 0, 0, size, size, TEX + "iuf_ring_ca.paa", (1, 1, 1, track)),
        W("ImageWidgetClass", name + "Arc", P(0, 0), P(size, size), image0=TEX + "iuf_ring_ca.paa",
          mode="@blend", src__alpha=1, stretch=1, color=C[key], ignorepointer=1,
          Mask=mask or (TEX + "iuf_ring_mask_ca.paa"), Transition__width=0.004, Progress=progress),
    ])


def caption_icon(name, x, y, value, icon, w=300):
    return frame(name, x, y, w, 14, [
        icon_img(name + "Icon", 0, 0, 13, icon, C["text3"]),
        text(name + "Text", 20, 0, w - 20, 14, value, 11, C["text3"], spacing=1),
    ])


def button(name, x, y, w, h, label, kind="secondary", icon=None, size=13):
    """kind: primary (amber fill), secondary (tile), teal/red/amber soft, ghost."""
    if kind == "primary":
        fill, border, fg = C["amber"], None, C["on_amber"]
    elif kind in ("teal", "red", "amber", "blue"):
        fill, border = soft(kind, 0.12, 0.36)
        fg = C["amber_text"] if kind == "amber" else C[kind]
    elif kind == "ghost":
        fill, border, fg = (1, 1, 1, 0.0), None, C["text2"]
    else:
        fill, border, fg = C["tile"], (1, 1, 1, 0.09), C["text"]
    kids = []
    tw = len(label) * (size * 0.55)
    iw = 14 if icon else 0
    gap = 8 if icon and label else 0
    total = iw + gap + tw
    sx = (w - total) / 2.0
    if icon:
        kids.append(icon_img(name + "Icon", sx, (h - 14) / 2.0, 14, icon, fg))
    if label:
        kids.append(text(name + "Text", sx + iw + gap, 0, tw + 6, h, label, size, fg))
    return box(name, x, y, w, h, 8, fill, border, kids, sheen=(kind == "primary"))
