"""INT_UIFramework layout generator v2 - the single source of every .layout.

Two kinds of layouts:
  * primitives (px_*, surface_*): one small tree each, created by INT_UIF_Draw
    and placed in exact pixels by script. v2 components are composed from them,
    the same way tools/mock_*.py compose the approved design mockups.
  * overlay roots (menu_frame, dialog, toast, list): the few named widgets the
    overlays look up; everything inside them is composed by script.

Why generated: dayz-ui-development Rule 3 wants all four unit flags declared on
every widget and Rule 4 wants balanced braces; a generator guarantees both, and
keeps widget names (the FindAnyWidget contract) in one place. It also writes
INT_UIF_LayoutTable.c: CreateWidgets must get LITERAL paths (engine bug, see
the table header and memory note dayz-createwidgets-literal-paths).

Run:  python tools/gen_layouts.py
"""
import os

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(ROOT, "INT_UIFramework", "gui", "layouts")
TEX = "INT_UIFramework/gui/textures/"
WHITE = TEX + "white.paa"
FONT = "gui/fonts/sdf_MetronBook24"
FONT_BIG = "gui/fonts/sdf_MetronBook72"
RADII = (4, 6, 8, 10, 12, 14)
PILLS = (3, 4, 6, 8, 10, 12, 14, 16, 18, 20)


class px(float):
    """A pixel value: marks its axis as exact."""


def W(cls, name, pos=(0, 0), size=(1, 1), children=None, **attrs):
    return {"cls": cls, "name": name, "pos": pos, "size": size,
            "children": children or [], "attrs": attrs}


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
            continue  # set at runtime via LoadImageFile; an empty path only logs an error
        key = k.replace("__", " ")
        if " " in key:
            key = '"%s"' % key
        if isinstance(v, str) and not v.startswith("@"):
            v = '"%s"' % v
        elif isinstance(v, str):
            v = v[1:]  # bare word, e.g. @center -> center
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


# --- attribute bundles ---------------------------------------------------------
def deco(**a):
    """Decorative widget: never takes the mouse."""
    a["ignorepointer"] = 1
    return a


def image(**a):
    a.update({"image0": WHITE, "mode": "@blend", "src__alpha": 1, "stretch": 1})
    return deco(**a)


def icon(**a):
    a.update({"mode": "@blend", "src__alpha": 1, "stretch": 1})
    return deco(**a)


def styled(style, pointer=False, **a):
    a.update({"text": "", "style": "@" + style})
    if not pointer:
        a["ignorepointer"] = 1
    return a


def text(halign="left", prop=None, exact=False, font=FONT, **a):
    a.update({"text": "", "font": font, "text__valign": "@center"})
    if halign != "left":
        a["text__halign"] = "@" + halign
    if exact:
        a["exact__text"] = 1
    elif prop is not None:
        a["text_proportion"] = prop
    return deco(**a)


CENTER = {"halign": "@center_ref", "valign": "@center_ref"}
VCENTER = {"valign": "@center_ref"}
PX0 = (px(0), px(0))
PX10 = (px(10), px(10))

L = {}

# --- primitives (INT_UIF_Draw) ------------------------------------------------------
# Primitive roots ignore the pointer: only px_hit / buttons take the mouse,
# so a decorative frame (avatar, badge, actions strip) never steals hover or
# clicks from the row/button hit under it. Children still get input.
# Containers set clipchildren 0: the engine default is 1 (vanilla writes
# "clipchildren 0" 1506 times), which cut every glow and shadow a component
# draws around itself at its root's edge.
NOCLIP = {"clipchildren": 0}
L["px_frame"] = W("FrameWidgetClass", "Frame", PX0, PX10, ignorepointer=1, **NOCLIP)
# Clipping viewport (dropdown rows scrolled by script).
L["px_clip"] = W("FrameWidgetClass", "Clip", PX0, PX10, ignorepointer=1, clipchildren=1)
L["px_image"] = W("ImageWidgetClass", "Image", PX0, PX10, image0="", **icon())
for suffix, halign in (("l", "left"), ("c", "center"), ("r", "right")):
    L["px_text_" + suffix] = W("TextWidgetClass", "Text", PX0, PX10, **text(halign=halign, exact=True))
    L["px_big_" + suffix] = W("TextWidgetClass", "Text", PX0, PX10,
                              **text(halign=halign, exact=True, font=FONT_BIG))
    L["px_rich_" + suffix] = W("RichTextWidgetClass", "Rich", PX0, PX10, **text(halign=halign, exact=True))
L["px_mtext"] = W("MultilineTextWidgetClass", "MText", PX0, PX10, text="", font=FONT,
                  exact__text=1, wrap=1, ignorepointer=1)
L["px_hit"] = W("ButtonWidgetClass", "Hit", PX0, PX10, **styled("IufEmpty", pointer=True))
# Edit box: a plain EditBox sizes its glyphs to its own HEIGHT. Measured
# in-game: neither "exact text size" (vanilla uses it only on
# MultilineEditBox) nor text_proportion changes it; vanilla dialog_input_text
# gets 20 px text from a 20 px tall box. INT_UIF_Draw.Edit therefore makes
# the box exactly `size` tall, centred in its slot.
L["px_edit"] = W("EditBoxWidgetClass", "Edit", PX0, PX10, style="@IufEdit", font=FONT,
                 text="", exact__text=0)
# Plain scroll area with the native scrollbar (consumers); the content frame
# grows in exact px. The dropdown scrolls its rows itself in a px_clip.
L["px_scroll"] = W("ScrollWidgetClass", "Scroll", PX0, PX10, style="@blank", Scrollbar__V=1,
                   clipchildren=1, children=[W("FrameWidgetClass", "Content", (0, px(0)), (1, px(10)),
                                               ignorepointer=1)])
L["px_ring"] = W("FrameWidgetClass", "Ring", PX0, PX10, ignorepointer=1, **NOCLIP, children=[
    W("ImageWidgetClass", "Track", image0=TEX + "iuf_ring_ca.paa", **icon()),
    W("ImageWidgetClass", "Arc", image0=TEX + "iuf_ring_ca.paa", Mask=TEX + "iuf_ring_mask_ca.paa",
      Transition__width=0.004, Progress=0, **icon()),
])
for r in RADII:
    L["surface_r%d" % r] = W("FrameWidgetClass", "Surface", PX0, PX10, ignorepointer=1, children=[
        W("ButtonWidgetClass", "Fill", **styled("IufR%d" % r)),
        W("ButtonWidgetClass", "Border", **styled("IufO%d" % r)),
    ])
for r in PILLS:
    L["surface_p%d" % r] = W("FrameWidgetClass", "Surface", PX0, PX10, ignorepointer=1, children=[
        W("ButtonWidgetClass", "Fill", **styled("IufP%d" % r)),
        W("ButtonWidgetClass", "Border", **styled("IufPO%d" % r)),
    ])
L["surface_shadow"] = W("FrameWidgetClass", "Surface", PX0, PX10, ignorepointer=1, children=[
    W("ButtonWidgetClass", "Fill", **styled("IufShadow"))])
L["surface_glow"] = W("FrameWidgetClass", "Surface", PX0, PX10, ignorepointer=1, children=[
    W("ButtonWidgetClass", "Fill", **styled("IufGlow"))])

# --- generic -----------------------------------------------------------------------
L["slot"] = W("FrameWidgetClass", "Slot")
L["label"] = W("FrameWidgetClass", "Label", children=[
    W("TextWidgetClass", "Text", **text(exact=True))])

# --- overlays: roots only, composed by script ------------------------------------------
L["menu_frame"] = W("FrameWidgetClass", "IufMenu", children=[
    W("ImageWidgetClass", "Dim", **image()),
    W("FrameWidgetClass", "Window", PX0, (px(960), px(640)), **CENTER, **NOCLIP),
    W("FrameWidgetClass", "ModalLayer", visible=0),
])
L["dialog"] = W("FrameWidgetClass", "IufDialog", children=[
    W("ImageWidgetClass", "Shade", **image()),
    W("ButtonWidgetClass", "Blocker", **styled("IufEmpty", pointer=True)),
    W("FrameWidgetClass", "Panel", PX0, (px(520), px(240)), **CENTER, **NOCLIP),
])
L["toast"] = W("FrameWidgetClass", "IufToast", PX0, (px(384), px(76)), **NOCLIP)
L["list"] = W("FrameWidgetClass", "IufList", children=[
    W("ScrollWidgetClass", "Scroll", style="@blank", Scrollbar__V=1, clipchildren=1, children=[
        W("FrameWidgetClass", "Rows", (0, px(0)), (1, px(10))),
    ]),
    W("TextWidgetClass", "Empty", **text(halign="center", exact=True)),
])

# --- write layouts (and drop files of layouts that no longer exist) -----------------
os.makedirs(OUT, exist_ok=True)
for fn in os.listdir(OUT):
    if fn.endswith(".layout") and fn[:-7] not in L:
        os.remove(os.path.join(OUT, fn))
for name, node in L.items():
    body = "\n".join(emit(node)) + "\n"
    assert body.count("{") == body.count("}"), name
    with open(os.path.join(OUT, name + ".layout"), "w", encoding="utf-8", newline="\n") as f:
        f.write(body)
print("layouts: %d -> %s" % (len(L), OUT))

# --- Enforce table: one LITERAL path per layout -------------------------------------
# Engine workaround, measured in-game by INT_UIF_SelfTest (2026-10-12): a path
# built at runtime ("INT_UIFramework/gui/layouts/" + name) made CreateWidgets
# return the tree of a DIFFERENT layout (progress->card, label->checkbox,
# tabs->menu_frame), depending on what had been loaded before. Vanilla only ever
# passes literals. Generated so the table can never drift from the files.
TABLE = os.path.join(ROOT, "INT_UIFramework", "scripts", "3_Game", "Core", "INT_UIF_LayoutTable.c")
roots = {name: emit(node)[0].split()[1] for name, node in L.items()}
T = ["// GENERATED by tools/gen_layouts.py -- do not edit by hand.",
     "// One literal CreateWidgets path per framework layout (see INT_UIF_Widgets.Create).",
     "class INT_UIF_LayoutTable", "{",
     "\tstatic int Count()", "\t{", "\t\treturn %d;" % len(L), "\t}", "",
     "\tstatic string NameAt(int i)", "\t{"]
for i, name in enumerate(L):
    T += ["\t\tif (i == %d)" % i, '\t\t\treturn "%s.layout";' % name]
T += ['\t\treturn "";', "\t}", "",
      "\t// Root widget name each layout must produce.",
      "\tstatic string RootName(string name)", "\t{"]
for name in L:
    T += ['\t\tif (name == "%s.layout")' % name, '\t\t\treturn "%s";' % roots[name]]
T += ['\t\treturn "";', "\t}", "",
      "\tstatic bool IsKnown(string name)", "\t{", '\t\treturn RootName(name) != "";', "\t}", "",
      "\t// Literal returned through a function (self-test comparison only).",
      "\tstatic string LiteralPath(string name)", "\t{"]
for name in L:
    T += ['\t\tif (name == "%s.layout")' % name,
          '\t\t\treturn "INT_UIFramework/gui/layouts/%s.layout";' % name]
T += ['\t\treturn "";', "\t}", "",
      "\t// The path every framework layout is created through: a literal in the call.",
      "\tstatic Widget Create(string name, Widget parent)", "\t{",
      "\t\tWorkspaceWidget ws = GetGame().GetWorkspace();",
      "\t\tif (!ws)", "\t\t\treturn null;"]
for name in L:
    T += ['\t\tif (name == "%s.layout")' % name,
          '\t\t\treturn ws.CreateWidgets("INT_UIFramework/gui/layouts/%s.layout", parent);' % name]
T += ["\t\treturn null;", "\t}", "}"]
with open(TABLE, "w", encoding="utf-8", newline="\n") as f:
    f.write("\n".join(T) + "\n")
print("layout table: %d entries -> %s" % (len(L), TABLE))
