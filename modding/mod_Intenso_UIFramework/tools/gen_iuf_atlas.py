"""INT_UIFramework atlas generator v2 ("modern dark" design): one white atlas +
its .imageset + .styles, plus the ring textures used by INT_UIF_Ring.

All art is WHITE on transparent: colour always comes from SetColor at runtime.
Coloured DXT art is brightened by the engine on decode (measured per-pixel in
INT_BaseProtection, SPEC GUI section 2); white art tinted by SetColor is exact.

Shapes are drawn at 4x and downsampled, so corners and 1 px borders are
anti-aliased. Every block keeps 2 px of empty padding so bilinear sampling never
pulls in a neighbour.

Outputs (PNG sources stay OUT of the addon, the packer would ship them):
  tools/art/iuf_ui_ca.png          -> gui/textures/iuf_ui_ca.paa        (asset_convert)
  tools/art/iuf_ring_ca.png        -> gui/textures/iuf_ring_ca.paa
  tools/art/iuf_ring_mask_ca.png   -> gui/textures/iuf_ring_mask_ca.paa
and, inside the addon:
  gui/imagesets/iuf_ui.imageset
  gui/styles/iuf.styles

Run:  python tools/gen_iuf_atlas.py
"""
import math
import os

from PIL import Image, ImageDraw, ImageFilter

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ADDON = os.path.join(ROOT, "INT_UIFramework")
ART = os.path.join(ROOT, "tools", "art")
PREFIX = "INT_UIFramework"
SET = "iuf_ui"
W, H = 512, 512
SS = 4  # supersampling factor
WHITE = (255, 255, 255, 255)
CLEAR = (0, 0, 0, 0)

atlas = Image.new("RGBA", (W, H), CLEAR)
cells = []  # (name, x, y, w, h)


class Packer:
    """Shelf packer: blocks left to right, a new shelf when the row is full."""

    def __init__(self, x0, y0, x1):
        self.x0, self.x, self.y, self.x1, self.row_h = x0, x0, y0, x1, 0

    def place(self, w, h, pad=2):
        if self.x + w + pad > self.x1:
            self.x = self.x0
            self.y += self.row_h + pad
            self.row_h = 0
        pos = (self.x, self.y)
        self.x += w + pad
        self.row_h = max(self.row_h, h)
        return pos

    def bottom(self):
        return self.y + self.row_h


def nine(name, ox, oy, block, corner):
    """Register the 9 parts of a square 9-slice block at (ox, oy)."""
    edge = block - 2 * corner
    parts = [("tl", 0, 0, corner, corner), ("t", corner, 0, edge, corner),
             ("tr", corner + edge, 0, corner, corner),
             ("l", 0, corner, corner, edge), ("c", corner, corner, edge, edge),
             ("r", corner + edge, corner, corner, edge),
             ("bl", 0, corner + edge, corner, corner),
             ("b", corner, corner + edge, edge, corner),
             ("br", corner + edge, corner + edge, corner, corner)]
    for pn, px, py, pw, ph in parts:
        cells.append(("%s_%s" % (name, pn), ox + px, oy + py, pw, ph))


def rounded_fill(block, radius):
    big = Image.new("RGBA", (block * SS, block * SS), CLEAR)
    ImageDraw.Draw(big).rounded_rectangle([0, 0, block * SS - 1, block * SS - 1],
                                          radius=radius * SS, fill=WHITE)
    return big.resize((block, block), Image.LANCZOS)


def rounded_outline(block, radius, width=1.0):
    big = Image.new("RGBA", (block * SS, block * SS), CLEAR)
    w = max(1, int(round(width * SS)))
    ImageDraw.Draw(big).rounded_rectangle([0, 0, block * SS - 1, block * SS - 1],
                                          radius=radius * SS, outline=WHITE, width=w)
    return big.resize((block, block), Image.LANCZOS)


pk = Packer(0, 0, W)


def add_block(name, img, block, corner):
    x, y = pk.place(block, block)
    atlas.alpha_composite(img, (x, y))
    nine(name, x, y, block, corner)


def add_cell(name, img):
    x, y = pk.place(img.width, img.height)
    atlas.alpha_composite(img, (x, y))
    cells.append((name, x, y, img.width, img.height))


# --- Rounded rectangles: fill + 1 px outline per radius ------------------------------
# Corner = radius + 1 so the curve sits inside the corner piece; 4 px of straight
# edge between the corners.
RADII = (4, 6, 8, 10, 12, 14)
for r in RADII:
    corner = r + 1
    block = 2 * corner + 4
    add_block("iuf_r%d" % r, rounded_fill(block, r), block, corner)
    add_block("iuf_o%d" % r, rounded_outline(block, r), block, corner)

# --- Pills: fully rounded ends, one per height (radius = height / 2) -----------------
PILLS = (3, 4, 6, 8, 10, 12, 14, 16, 18, 20)
for r in PILLS:
    block = 2 * r + 4
    add_block("iuf_p%d" % r, rounded_fill(block, r), block, r)
    add_block("iuf_po%d" % r, rounded_outline(block, r), block, r)

# --- Soft shadow / glow (tinted at runtime: black = shadow, accent = glow) ------------
SH_BLOCK, SH_CORNER = 128, 56
sh = Image.new("RGBA", (SH_BLOCK, SH_BLOCK), CLEAR)
ImageDraw.Draw(sh).rounded_rectangle([32, 32, SH_BLOCK - 33, SH_BLOCK - 33], radius=16,
                                     fill=(255, 255, 255, 230))
sh = sh.filter(ImageFilter.GaussianBlur(14))
add_block("iuf_shadow", sh, SH_BLOCK, SH_CORNER)

GL_BLOCK, GL_CORNER = 64, 26
gl = Image.new("RGBA", (GL_BLOCK, GL_BLOCK), CLEAR)
ImageDraw.Draw(gl).rounded_rectangle([16, 16, GL_BLOCK - 17, GL_BLOCK - 17], radius=10,
                                     fill=(255, 255, 255, 255))
gl = gl.filter(ImageFilter.GaussianBlur(6))
add_block("iuf_glow", gl, GL_BLOCK, GL_CORNER)

# --- Circles, rings, radial glow, gradients, solid ---------------------------------------
for size in (16, 32, 64):
    big = Image.new("RGBA", (size * SS, size * SS), CLEAR)
    ImageDraw.Draw(big).ellipse([0, 0, size * SS - 1, size * SS - 1], fill=WHITE)
    add_cell("iuf_circle%d" % size, big.resize((size, size), Image.LANCZOS))
    big = Image.new("RGBA", (size * SS, size * SS), CLEAR)
    ImageDraw.Draw(big).ellipse([SS, SS, size * SS - 1 - SS, size * SS - 1 - SS],
                                outline=WHITE, width=int(1.5 * SS))
    add_cell("iuf_ringline%d" % size, big.resize((size, size), Image.LANCZOS))

rg = Image.new("RGBA", (64, 64), CLEAR)
rgp = rg.load()
for yy in range(64):
    for xx in range(64):
        dd = math.hypot(xx - 31.5, yy - 31.5) / 31.5
        a = max(0.0, 1.0 - dd)
        rgp[xx, yy] = (255, 255, 255, int(255 * a * a))
add_cell("iuf_radial", rg)

gv = Image.new("RGBA", (8, 128), CLEAR)
for yy in range(128):
    a = int(255 * (1.0 - yy / 127.0))
    for xx in range(8):
        gv.putpixel((xx, yy), (255, 255, 255, a))
add_cell("iuf_grad_v", gv)

gh = Image.new("RGBA", (128, 8), CLEAR)
for xx in range(128):
    a = int(255 * (1.0 - xx / 127.0))
    for yy in range(8):
        gh.putpixel((xx, yy), (255, 255, 255, a))
add_cell("iuf_grad_h", gh)

# Centre-bright horizontal band: shimmer sweeps, divider fades.
gb = Image.new("RGBA", (128, 8), CLEAR)
for xx in range(128):
    t = abs(xx - 63.5) / 63.5
    a = int(255 * max(0.0, 1.0 - t) ** 1.5)
    for yy in range(8):
        gb.putpixel((xx, yy), (255, 255, 255, a))
add_cell("iuf_grad_band", gb)

add_cell("iuf_white", Image.new("RGBA", (8, 8), WHITE))

# --- Icons 32x32, drawn at 4x on a 128 canvas -------------------------------------------
S = 128
LW = 10  # stroke at 4x (~2.5 px at 32)


def canvas():
    img = Image.new("RGBA", (S, S), CLEAR)
    return img, ImageDraw.Draw(img)


def line(d, pts, width=LW):
    d.line(pts, fill=WHITE, width=width, joint="curve")
    r = width / 2.0
    for px, py in (pts[0], pts[-1]):
        d.ellipse([px - r, py - r, px + r, py + r], fill=WHITE)


def circle(d, cx, cy, r, width=LW):
    d.ellipse([cx - r, cy - r, cx + r, cy + r], outline=WHITE, width=width)


def dot(d, cx, cy, r):
    d.ellipse([cx - r, cy - r, cx + r, cy + r], fill=WHITE)


def rrect(d, box, radius, width=LW, fill=False):
    if fill:
        d.rounded_rectangle(box, radius=radius, fill=WHITE)
    else:
        d.rounded_rectangle(box, radius=radius, outline=WHITE, width=width)


def person(d, cx, top, scale=1.0):
    hr = 15 * scale
    circle(d, cx, top + hr, hr)
    d.arc([cx - 30 * scale, top + 2 * hr + 6 * scale, cx + 30 * scale, top + 2 * hr + 66 * scale],
          start=200, end=340, fill=WHITE, width=LW)


ICONS = {}


def icon(fn):
    ICONS[fn.__name__[2:]] = fn
    return fn


@icon
def i_check():
    img, d = canvas(); line(d, [(28, 66), (54, 90), (100, 38)]); return img


@icon
def i_close():
    img, d = canvas(); line(d, [(38, 38), (90, 90)]); line(d, [(90, 38), (38, 90)]); return img


@icon
def i_chevron_down():
    img, d = canvas(); line(d, [(36, 52), (64, 80), (92, 52)]); return img


@icon
def i_chevron_up():
    img, d = canvas(); line(d, [(36, 78), (64, 50), (92, 78)]); return img


@icon
def i_chevron_right():
    img, d = canvas(); line(d, [(52, 36), (80, 64), (52, 92)]); return img


@icon
def i_chevron_left():
    img, d = canvas(); line(d, [(76, 36), (48, 64), (76, 92)]); return img


@icon
def i_plus():
    img, d = canvas(); line(d, [(64, 30), (64, 98)]); line(d, [(30, 64), (98, 64)]); return img


@icon
def i_minus():
    img, d = canvas(); line(d, [(30, 64), (98, 64)]); return img


@icon
def i_info():
    img, d = canvas(); circle(d, 64, 64, 46)
    line(d, [(64, 58), (64, 88)]); dot(d, 64, 40, 7); return img


@icon
def i_warning():
    img, d = canvas()
    d.line([(64, 20), (110, 102), (18, 102), (64, 20)], fill=WHITE, width=LW, joint="curve")
    line(d, [(64, 52), (64, 74)]); dot(d, 64, 88, 6); return img


@icon
def i_error():
    img, d = canvas(); circle(d, 64, 64, 46)
    line(d, [(48, 48), (80, 80)]); line(d, [(80, 48), (48, 80)]); return img


@icon
def i_success():
    img, d = canvas(); circle(d, 64, 64, 46)
    line(d, [(44, 66), (58, 80), (86, 50)]); return img


@icon
def i_search():
    img, d = canvas(); circle(d, 56, 56, 32); line(d, [(80, 80), (104, 104)]); return img


@icon
def i_user():
    img, d = canvas(); person(d, 64, 18, 1.15); return img


@icon
def i_users():
    img, d = canvas()
    person(d, 50, 26, 0.95)
    d.arc([70, 26, 100, 56], start=270, end=90, fill=WHITE, width=LW)
    d.arc([76, 62, 124, 120], start=230, end=330, fill=WHITE, width=LW)
    return img


@icon
def i_user_plus():
    img, d = canvas(); person(d, 52, 22, 1.0)
    line(d, [(104, 46), (104, 78)]); line(d, [(88, 62), (120, 62)]); return img


@icon
def i_user_minus():
    img, d = canvas(); person(d, 52, 22, 1.0); line(d, [(88, 62), (120, 62)]); return img


@icon
def i_shield():
    img, d = canvas()
    d.line([(64, 16), (104, 30), (101, 70), (64, 112), (27, 70), (24, 30), (64, 16)],
           fill=WHITE, width=LW, joint="curve"); return img


@icon
def i_shield_check():
    img = ICONS["shield"](); d = ImageDraw.Draw(img)
    line(d, [(46, 62), (60, 76), (84, 50)]); return img


@icon
def i_shield_fill():
    img, d = canvas()
    d.polygon([(64, 14), (106, 29), (103, 71), (64, 114), (25, 71), (22, 29)], fill=WHITE)
    return img


@icon
def i_clock():
    img, d = canvas(); circle(d, 64, 64, 46); line(d, [(64, 38), (64, 66), (84, 78)]); return img


@icon
def i_calendar():
    img, d = canvas(); rrect(d, [20, 28, 108, 108], 14)
    line(d, [(20, 52), (108, 52)]); line(d, [(44, 16), (44, 36)]); line(d, [(84, 16), (84, 36)])
    for cx in (46, 64, 82):
        dot(d, cx, 72, 6); dot(d, cx, 90, 6)
    return img


@icon
def i_gear():
    img, d = canvas()
    pts = []
    for k in range(32):
        ang = math.pi * 2 * k / 32.0
        r = 50 if (k // 2) % 2 == 0 else 38
        pts.append((64 + r * math.cos(ang), 64 + r * math.sin(ang)))
    d.polygon(pts, fill=WHITE)
    d.ellipse([46, 46, 82, 82], fill=CLEAR); return img


@icon
def i_lock():
    img, d = canvas(); rrect(d, [28, 58, 100, 108], 12, fill=True)
    d.arc([40, 20, 88, 80], start=180, end=360, fill=WHITE, width=LW)
    line(d, [(40, 50), (40, 60)]); line(d, [(88, 50), (88, 60)]); return img


@icon
def i_flag():
    img, d = canvas(); line(d, [(34, 18), (34, 110)])
    d.polygon([(34, 22), (100, 22), (84, 44), (100, 66), (34, 66)], fill=WHITE); return img


@icon
def i_crown():
    img, d = canvas()
    d.polygon([(20, 40), (44, 66), (64, 30), (84, 66), (108, 40), (98, 96), (30, 96)], fill=WHITE)
    return img


@icon
def i_swap():
    img, d = canvas()
    line(d, [(26, 46), (98, 46)]); line(d, [(80, 28), (100, 46), (80, 64)])
    line(d, [(102, 84), (30, 84)]); line(d, [(48, 66), (28, 84), (48, 102)]); return img


@icon
def i_radar():
    img, d = canvas(); circle(d, 64, 64, 46); circle(d, 64, 64, 24); dot(d, 64, 64, 7)
    line(d, [(64, 64), (96, 32)]); return img


@icon
def i_pin():
    img, d = canvas()
    d.pieslice([30, 14, 98, 82], start=150, end=390, fill=WHITE)
    d.polygon([(34, 60), (94, 60), (64, 112)], fill=WHITE)
    d.ellipse([52, 36, 76, 60], fill=CLEAR); return img


@icon
def i_home():
    img, d = canvas()
    d.line([(18, 64), (64, 22), (110, 64)], fill=WHITE, width=LW, joint="curve")
    d.line([(32, 54), (32, 106), (96, 106), (96, 54)], fill=WHITE, width=LW, joint="curve")
    return img


@icon
def i_trash():
    img, d = canvas(); line(d, [(22, 34), (106, 34)]); line(d, [(48, 34), (52, 20), (76, 20), (80, 34)])
    d.line([(32, 34), (38, 108), (90, 108), (96, 34)], fill=WHITE, width=LW, joint="curve")
    line(d, [(56, 54), (56, 90)]); line(d, [(72, 54), (72, 90)]); return img


@icon
def i_refresh():
    img, d = canvas(); d.arc([22, 22, 106, 106], start=30, end=330, fill=WHITE, width=LW)
    d.polygon([(96, 8), (112, 46), (74, 42)], fill=WHITE); return img


@icon
def i_logout():
    img, d = canvas()
    d.line([(64, 22), (26, 22), (26, 106), (64, 106)], fill=WHITE, width=LW, joint="curve")
    line(d, [(52, 64), (108, 64)]); line(d, [(88, 44), (108, 64), (88, 84)]); return img


@icon
def i_key():
    img, d = canvas(); circle(d, 40, 64, 20); line(d, [(60, 64), (110, 64)])
    line(d, [(92, 64), (92, 84)]); line(d, [(106, 64), (106, 80)]); return img


@icon
def i_star():
    img, d = canvas(); pts = []
    for k in range(10):
        ang = -math.pi / 2 + k * math.pi / 5
        r = 50 if k % 2 == 0 else 22
        pts.append((64 + r * math.cos(ang), 66 + r * math.sin(ang)))
    d.polygon(pts, fill=WHITE); return img


@icon
def i_eye():
    img, d = canvas()
    d.arc([14, 30, 114, 130], start=200, end=340, fill=WHITE, width=LW)
    d.arc([14, -2, 114, 98], start=20, end=160, fill=WHITE, width=LW)
    dot(d, 64, 64, 16); return img


@icon
def i_list():
    img, d = canvas()
    for yy in (36, 64, 92):
        dot(d, 28, yy, 7); line(d, [(48, yy), (106, yy)])
    return img


@icon
def i_wallet():
    img, d = canvas(); rrect(d, [18, 34, 110, 104], 14)
    line(d, [(26, 34), (88, 18)]); dot(d, 88, 70, 8); return img


@icon
def i_target():
    img, d = canvas(); circle(d, 64, 64, 40); dot(d, 64, 64, 8)
    for a, b in (((64, 8), (64, 30)), ((64, 98), (64, 120)), ((8, 64), (30, 64)), ((98, 64), (120, 64))):
        line(d, [a, b])
    return img


@icon
def i_door():
    img, d = canvas(); rrect(d, [30, 16, 98, 112], 8); dot(d, 82, 66, 6); return img


@icon
def i_bolt():
    img, d = canvas()
    d.polygon([(72, 10), (30, 72), (60, 72), (52, 118), (98, 52), (68, 52)], fill=WHITE); return img


@icon
def i_dot():
    img, d = canvas(); dot(d, 64, 64, 22); return img


ICON_SIZE = 32
ip = Packer(0, pk.bottom() + 4, W)
for name, fn in ICONS.items():
    x, y = ip.place(ICON_SIZE, ICON_SIZE)
    atlas.alpha_composite(fn().resize((ICON_SIZE, ICON_SIZE), Image.LANCZOS), (x, y))
    cells.append(("iuf_icon_" + name, x, y, ICON_SIZE, ICON_SIZE))
assert ip.bottom() <= H, "atlas overflow: %d" % ip.bottom()

os.makedirs(ART, exist_ok=True)
atlas.save(os.path.join(ART, "iuf_ui_ca.png"))

# --- Ring + conic mask (INT_UIF_Ring) ----------------------------------------------------
RING = 256
big = Image.new("RGBA", (RING * SS, RING * SS), CLEAR)
stroke = 20 * SS
ImageDraw.Draw(big).ellipse([stroke // 2, stroke // 2, RING * SS - stroke // 2 - 1, RING * SS - stroke // 2 - 1],
                            outline=WHITE, width=stroke)
big.resize((RING, RING), Image.LANCZOS).save(os.path.join(ART, "iuf_ring_ca.png"))

# Thin outline circle for large hairline rings (radar zones, targets).
thin = Image.new("RGBA", (RING * SS, RING * SS), CLEAR)
ImageDraw.Draw(thin).ellipse([2 * SS, 2 * SS, RING * SS - 2 * SS - 1, RING * SS - 2 * SS - 1],
                             outline=WHITE, width=3 * SS)
thin.resize((RING, RING), Image.LANCZOS).save(os.path.join(ART, "iuf_ringthin_ca.png"))

# Mask alpha = clockwise angle from 12 o'clock / 360: SetMaskProgress(p) shows the
# arc from the top to p of a full turn (pixels with mask alpha < p are drawn).
mask = Image.new("RGBA", (RING, RING), (255, 255, 255, 0))
mp = mask.load()
cc = (RING - 1) / 2.0
for yy in range(RING):
    for xx in range(RING):
        ang = math.atan2(xx - cc, -(yy - cc))  # 0 at top, clockwise positive
        if ang < 0:
            ang += 2 * math.pi
        a = max(1, min(254, int(round(255 * ang / (2 * math.pi)))))
        # The value goes into colour AND alpha: which channel the engine reads
        # for the mask is not documented, both carry the same ramp.
        mp[xx, yy] = (a, a, a, a)
mask.save(os.path.join(ART, "iuf_ring_mask_ca.png"))

# --- Imageset ------------------------------------------------------------------------------
L = ["ImageSetClass {", ' Name "%s"' % SET, " RefSize %d %d" % (W, H), " Textures {",
     "  ImageSetTextureClass {", "   mpix 1",
     '   path "%s/gui/textures/iuf_ui_ca.paa"' % PREFIX, "  }", " }", " Images {"]
for name, x, y, w, h in cells:
    L += ["  ImageSetDefClass %s {" % name, '   Name "%s"' % name,
          "   Pos %d %d" % (x, y), "   Size %d %d" % (w, h), "   Flags 0", "  }"]
L += [" }", " Groups {", " }", "}"]
os.makedirs(os.path.join(ADDON, "gui", "imagesets"), exist_ok=True)
with open(os.path.join(ADDON, "gui", "imagesets", "iuf_ui.imageset"), "w",
          encoding="utf-8", newline="\n") as f:
    f.write("\n".join(L) + "\n")

# --- Styles --------------------------------------------------------------------------------
# Every state shows the same white shape: hover/press colours are animated in
# script (SetColor), the style only supplies the geometry.
ITEM = [("LeftTop", "tl"), ("Top", "t"), ("RightTop", "tr"), ("Left", "l"),
        ("Center", "c"), ("Right", "r"), ("LeftBottom", "bl"), ("Bottom", "b"),
        ("RightBottom", "br")]
BUTTON_STATES = ("Normal", "Highlight", "Pushed", "Focus", "Disabled")
EDIT_STATES = ("Normal", "Focus", "Disabled")
FONT = "gui/fonts/sdf_MetronBook24"


def style_block(style, base, states):
    out = ['        <Style Name="%s" Font="%s" ImageSet="%s" Color="4294967295">'
           % (style, FONT, SET)]
    for state in states:
        out.append('            <State Name="%s">' % state)
        for item, part in ITEM:
            img = "" if base is None else "%s_%s" % (base, part)
            out.append('                <Item Name="%s" Image="%s" />' % (item, img))
        out.append("            </State>")
    out.append("        </Style>")
    return out


STYLES = []
S_OUT = ["<WidgetStyles>", '    <Widget Name="ButtonWidget">']
for r in RADII:
    S_OUT += style_block("IufR%d" % r, "iuf_r%d" % r, BUTTON_STATES)
    S_OUT += style_block("IufO%d" % r, "iuf_o%d" % r, BUTTON_STATES)
    STYLES += ["IufR%d" % r, "IufO%d" % r]
for r in PILLS:
    S_OUT += style_block("IufP%d" % r, "iuf_p%d" % r, BUTTON_STATES)
    S_OUT += style_block("IufPO%d" % r, "iuf_po%d" % r, BUTTON_STATES)
    STYLES += ["IufP%d" % r, "IufPO%d" % r]
S_OUT += style_block("IufShadow", "iuf_shadow", BUTTON_STATES)
S_OUT += style_block("IufGlow", "iuf_glow", BUTTON_STATES)
S_OUT += style_block("IufEmpty", None, BUTTON_STATES)
STYLES += ["IufShadow", "IufGlow", "IufEmpty"]
S_OUT += ["    </Widget>", '    <Widget Name="EditBoxWidget">']
S_OUT += style_block("IufEdit", None, EDIT_STATES)
S_OUT += ["    </Widget>", "</WidgetStyles>"]
os.makedirs(os.path.join(ADDON, "gui", "styles"), exist_ok=True)
with open(os.path.join(ADDON, "gui", "styles", "iuf.styles"), "w", encoding="utf-8",
          newline="\n") as f:
    f.write("\n".join(S_OUT) + "\n")

# Preview for humans (not shipped): atlas on dark grey, 2x.
prev = Image.new("RGBA", (W, H), (30, 33, 37, 255))
prev.alpha_composite(atlas)
prev.resize((W * 2, H * 2), Image.NEAREST).save(os.path.join(ART, "atlas_preview.png"))
print("atlas v2: %d cells, %d icons, %d styles, used height %d/%d"
      % (len(cells), len(ICONS), len(STYLES), ip.bottom(), H))
