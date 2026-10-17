"""Static design mockup of the base-protection terminal (sample data).

Written into the stand client's profile and loaded hot with
ui_load("$profile:iuf_mock_terminal.layout") -- seconds per iteration, no repack.
Run:  python tools/mock_terminal.py [profile_dir]
"""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from iuf_kit import (C, W, P, alpha, avatar, box, button, caption_icon, chip, dot, frame,  # noqa: E402
                     icon_img, image, cell, keycap, mtext, ring, shape, soft, text, write_layout,
                     FONT_BIG)

PROFILE = sys.argv[1] if len(sys.argv) > 1 else r"G:\DayZServers_TEST\DS-Server MCP Stand\clientprofile"
RING_MASK = "$profile:iuf_ring_mask_ca.paa"   # hot test of the mask texture

WIN_W, WIN_H = 1480, 820
HEADER_H = 76
PAD = 16
GAP = 14
COL_L, COL_C = 384, 596
COL_R = WIN_W - 2 * PAD - COL_L - COL_C - 2 * GAP
BODY_Y = HEADER_H + 14
BODY_H = WIN_H - BODY_Y - PAD

# --- sample data -------------------------------------------------------------------------
OWNER = "Intenso"
RADIUS, TELEPORT = 80, 150
MEMBERS = [("Intenso", "владелец терминала · вы", "Владелец", "amber"),
           ("Serega", "администратор", "Админ", "teal"),
           ("Kovboy", "есть доступ к терминалу", "Доступ", "blue")]
NEARBY = ["Bars", "Sobr"]
DAYS = [("Пн", 18 * 60, 23 * 60 + 59, True), ("Вт", 18 * 60, 23 * 60 + 59, True),
        ("Ср", 18 * 60, 23 * 60 + 59, True), ("Чт", 18 * 60, 23 * 60 + 59, True),
        ("Пт", 16 * 60, 23 * 60 + 59, True), ("Сб", 0, 23 * 60 + 59, True), ("Вс", 0, 0, False)]


def clock(m):
    return "%02d:%02d" % (m // 60, m % 60)


def panel(name, x, y, w, h, kids):
    return box(name, x, y, w, h, 12, C["panel"], C["panel_border"], kids, sheen=True)


def hline(name, x, y, w, a=0.06):
    return image(name, x, y, w, 1, cell("iuf_white"), (1, 1, 1, a))


# --- header ---------------------------------------------------------------------------------
def header():
    kids = []
    kids.append(image("AppGlow", 0, -2, 80, 80, cell("iuf_radial"), alpha(C["amber"], 0.32)))
    kids.append(box("AppTile", 20, 18, 40, 40, 10, C["amber"], None, [
        image("AppSheen", 0, 0, 40, 20, cell("iuf_grad_v"), (1, 1, 1, 0.24)),
        icon_img("AppGlyph", 10, 10, 20, "shield_fill", C["on_amber"]),
    ]))
    kids.append(text("Title", 74, 16, 520, 26, "Защита территории", 20, C["text"]))
    kids.append(text("Subtitle", 74, 42, 520, 16, "ТЕРМИНАЛ  ·  ВЛАДЕЛЕЦ " + OWNER.upper(), 11, C["text3"], spacing=1))

    fill, border = soft("teal", 0.10, 0.30)
    sw = 178
    kids.append(box("Status", (WIN_W - sw) / 2.0, 22, sw, 32, 10, fill, border, [
        dot("StatusDot", 16, 12, 8, "teal"),
        text("StatusText", 34, 0, sw - 44, 32, "Защита активна", 13, C["teal"]),
    ]))

    role, rw = chip("Role", 0, 0, "Владелец", "amber", h=32, icon="crown", w=118)
    role["pos"] = P(WIN_W - 20 - 32 - 10 - 118, 22)
    kids.append(role)
    kids.append(box("Close", WIN_W - 20 - 32, 22, 32, 32, 10, C["tile"], (1, 1, 1, 0.08), [
        icon_img("CloseIcon", 10, 10, 12, "close", C["text2"])]))
    kids.append(hline("HeaderLine", 0, HEADER_H - 1, WIN_W))
    return frame("Header", 0, 0, WIN_W, HEADER_H, kids)


# --- left: status + zone -----------------------------------------------------------------------
def radar_card(x, y, w, h):
    R = 150  # diameter of the teleport circle
    cx, cy = 20, (h - R) / 2.0
    inner = R * RADIUS / float(TELEPORT)
    io = (R - inner) / 2.0
    kids = [
        # grid cross
        image("RGridH", cx - 6, cy + R / 2.0, R + 12, 1, cell("iuf_white"), (1, 1, 1, 0.05)),
        image("RGridV", cx + R / 2.0, cy - 6, 1, R + 12, cell("iuf_white"), (1, 1, 1, 0.05)),
        # teleport circle (amber)
        image("RTeleFill", cx, cy, R, R, cell("iuf_circle64"), alpha(C["amber"], 0.035)),
        image("RTeleRing", cx, cy, R, R, cell("iuf_ringline64"), alpha(C["amber"], 0.55)),
        # protection zone (teal)
        image("RZoneGlow", cx + io - 14, cy + io - 14, inner + 28, inner + 28, cell("iuf_radial"), alpha(C["teal"], 0.22)),
        image("RZoneFill", cx + io, cy + io, inner, inner, cell("iuf_circle64"), alpha(C["teal"], 0.14)),
        image("RZoneRing", cx + io, cy + io, inner, inner, cell("iuf_ringline64"), alpha(C["teal"], 0.85)),
        # terminal at the centre
        box("RCore", cx + R / 2.0 - 13, cy + R / 2.0 - 13, 26, 26, 8, C["teal"], None, [
            icon_img("RCoreIcon", 6, 6, 14, "shield_fill", C["on_amber"])]),
    ]
    lx = cx + R + 22
    kids += [
        text("LZoneCap", lx, 34, 200, 14, "ЗОНА ЗАЩИТЫ", 10, C["text3"], spacing=1),
        dot("LZoneDot", lx, 58, 8, "teal", glow=False),
        text("LZoneVal", lx + 16, 46, 120, 32, "%d м" % RADIUS, 24, C["teal"], font=FONT_BIG),
        text("LTeleCap", lx, 96, 200, 14, "ПЕРЕНОС ПРИ ВХОДЕ", 10, C["text3"], spacing=1),
        dot("LTeleDot", lx, 120, 8, "amber", glow=False),
        text("LTeleVal", lx + 16, 108, 120, 32, "%d м" % TELEPORT, 24, C["amber_text"], font=FONT_BIG),
    ]
    return box("Radar", x, y, w, h, 10, C["tile"], C["tile_border"], kids)


def left_panel():
    w, h = COL_L, BODY_H
    kids = [caption_icon("Cap", 20, 18, "СТАТУС ЗАЩИТЫ", "shield_check")]
    covered = sum((t - f + 1) for _, f, t, on in DAYS if on)
    cover = covered / float(7 * 24 * 60)
    kids.append(image("RingGlow", 2, 26, 148, 148, cell("iuf_radial"), alpha(C["teal"], 0.12)))
    kids.append(ring("Ring", 20, 44, 112, cover, "teal", mask=RING_MASK))
    kids.append(text("RingValue", 20, 76, 112, 30, "%d%%" % round(cover * 100), 26, C["text"],
                     halign="center", font=FONT_BIG))
    kids.append(text("RingSub", 20, 104, 112, 16, "недели", 11, C["text3"], halign="center"))
    kids.append(text("StateTitle", 152, 62, 220, 24, "Защита активна", 18, C["teal"]))
    kids.append(text("StateSub", 152, 88, 220, 18, "работает по расписанию", 12, C["text2"]))
    kids.append(text("StateSub2", 152, 106, 220, 18, "%d ч из 168 в неделю" % round(covered / 60.0), 12, C["text3"]))

    kids.append(radar_card(20, 172, w - 40, 192))

    rows = [("blue", "С доступом", "3 игрока"), ("violet", "Рядом с терминалом", "2 игрока")]
    y = 376
    for i, (key, label, value) in enumerate(rows):
        kids.append(frame("Stat%d" % i, 20, y, w - 40, 34, [
            dot("Stat%dDot" % i, 2, 13, 8, key, glow=False),
            text("Stat%dLabel" % i, 20, 0, 200, 34, label, 13, C["text2"]),
            text("Stat%dValue" % i, 0, 0, w - 40, 34, value, 14, C["text"], halign="right"),
        ]))
        y += 36

    fill, border = soft("amber", 0.07, 0.26)
    kids.append(box("Warn", 20, 462, w - 40, 86, 10, fill, border, [
        icon_img("WarnIcon", 14, 14, 16, "warning", C["amber_text"]),
        text("WarnTitle", 40, 10, 280, 22, "Чужих переместят из зоны", 13, C["amber_text"]),
        mtext("WarnText", 40, 34, w - 40 - 54, 46,
              "Игроки без доступа, вошедшие в игру внутри зоны, окажутся за её пределами.",
              12, C["text2"]),
    ]))

    kids.append(box("Owner", 20, h - 20 - 68, w - 40, 68, 10, C["tile"], C["tile_border"], [
        image("OwnerGlow", -8, 0, 68, 68, cell("iuf_radial"), alpha(C["amber"], 0.22)),
        box("OwnerIcon", 14, 16, 36, 36, 10, alpha(C["amber"], 0.14), alpha(C["amber"], 0.32), [
            icon_img("OwnerGlyph", 10, 10, 16, "crown", C["amber_text"])]),
        text("OwnerTitle", 62, 14, 260, 22, "Вы владелец терминала", 14, C["text"]),
        text("OwnerSub", 62, 34, 260, 18, "только вы управляете доступом", 12, C["text3"]),
    ]))
    return panel("Left", PAD, BODY_Y, w, h, kids)


# --- centre: members ------------------------------------------------------------------------
def member_row(i, y, rw, name, sub, role, key, hovered=False):
    fill = C["tile_hover"] if hovered else C["tile"]
    border = alpha(C["teal"], 0.30) if hovered else C["tile_border"]
    kids = [avatar("M%dAv" % i, 12, 10, 36, name[0], key),
            text("M%dName" % i, 60, 9, 260, 20, name, 14, C["text"]),
            text("M%dSub" % i, 60, 29, 300, 18, sub, 12, C["text3"])]
    ch, cw = chip("M%dRole" % i, 0, 0, role, key, h=22)
    right = rw - 14 - cw
    if hovered:
        kids.append(box("M%dActT" % i, rw - 14 - 30 - 38, 13, 30, 30, 8, alpha(C["amber"], 0.10), alpha(C["amber"], 0.32),
                        [icon_img("M%dActTI" % i, 8, 8, 14, "crown", C["amber_text"])]))
        kids.append(box("M%dActR" % i, rw - 14 - 30, 13, 30, 30, 8, alpha(C["red"], 0.10), alpha(C["red"], 0.32),
                        [icon_img("M%dActRI" % i, 8, 8, 14, "user_minus", C["red"])]))
        right = rw - 14 - 30 - 38 - 10 - cw
    ch["pos"] = P(right, 17)
    kids.append(ch)
    row = box("M%d" % i, 20, y, rw, 56, 10, fill, border, kids)
    if hovered:
        row["children"].insert(0, image("M%dAccent" % i, 0, 14, 3, 28, cell("iuf_white"), C["teal"]))
    return row


def centre_panel():
    w, h = COL_C, BODY_H
    rw = w - 40
    kids = [caption_icon("Cap", 20, 18, "ДОСТУП К ТЕРМИНАЛУ", "users"),
            text("Title", 20, 36, 200, 26, "Участники", 18, C["text"])]
    cnt, _ = chip("Count", 122, 40, "3", "blue", w=26, h=20)
    kids.append(cnt)
    y = 76
    for i, (name, sub, role, key) in enumerate(MEMBERS):
        kids.append(member_row(i, y, rw, name, sub, role, key, hovered=(i == 1)))
        y += 64
    y += 14
    kids.append(caption_icon("NearCap", 20, y, "РЯДОМ С ТЕРМИНАЛОМ  ·  5 М", "radar"))
    y += 26
    for j, name in enumerate(NEARBY):
        kids.append(box("N%d" % j, 20, y, rw, 56, 10, C["tile"], C["tile_border"], [
            avatar("N%dAv" % j, 12, 10, 36, name[0], "violet"),
            text("N%dName" % j, 60, 9, 260, 20, name, 14, C["text"]),
            text("N%dSub" % j, 60, 29, 260, 18, "нет доступа", 12, C["text3"]),
            button("N%dAdd" % j, rw - 14 - 124, 12, 124, 32, "Выдать доступ", "teal", icon="user_plus", size=12),
        ]))
        y += 64
    kids.append(box("Tip", 20, h - 20 - 44, rw, 44, 10, (1, 1, 1, 0.025), (1, 1, 1, 0.05), [
        icon_img("TipIcon", 14, 14, 16, "info", C["text3"]),
        text("TipText", 40, 0, rw - 60, 44, "Наведите на участника, чтобы передать владение или забрать доступ", 12, C["text3"]),
    ]))
    return panel("Centre", PAD + COL_L + GAP, BODY_Y, w, h, kids)


# --- right: schedule ------------------------------------------------------------------------
def right_panel():
    w, h = COL_R, BODY_H
    kids = [caption_icon("Cap", 20, 18, "РАСПИСАНИЕ", "calendar"),
            text("Title", 20, 36, 300, 26, "Когда работает защита", 18, C["text"])]
    x0, x1 = 60, w - 20 - 104
    track_w = x1 - x0
    y = 84
    for t, label in enumerate(("0", "6", "12", "18", "24")):
        tx = x0 + track_w * t / 4.0
        kids.append(text("Tick%d" % t, tx - 20, y, 40, 14, label, 10, C["text3"], halign="center"))
        kids.append(image("TickL%d" % t, tx, y + 20, 1, 7 * 50 - 14, cell("iuf_white"), (1, 1, 1, 0.04)))
    y += 22
    for d, (day, f, to, on) in enumerate(DAYS):
        kids.append(text("D%dName" % d, 20, y, 40, 40, day, 13, C["text"] if on else C["text3"]))
        kids.append(shape("D%dTrack" % d, x0, y + 16, track_w, 8, "IufP4", (1, 1, 1, 0.06)))
        if on:
            sx = x0 + track_w * f / 1440.0
            ex = x0 + track_w * (to + 1) / 1440.0
            kids.append(shape("D%dBar" % d, sx, y + 16, ex - sx, 8, "IufP4", C["teal"]))
            kids.append(image("D%dShine" % d, sx + 2, y + 17, ex - sx - 4, 3, cell("iuf_grad_band"), (1, 1, 1, 0.30)))
            kids.append(text("D%dTime" % d, x1 + 10, y, 94, 40, "%s–%s" % (clock(f), clock(to)), 12, C["text2"], halign="right"))
        else:
            kids.append(text("D%dTime" % d, x1 + 10, y, 94, 40, "выключена", 12, C["text3"], halign="right"))
        y += 50
    y += 4
    kids.append(hline("Div", 20, y, w - 40))
    y += 16
    tw = (w - 40 - 12) / 2.0
    covered = sum((t - f + 1) for _, f, t, on in DAYS if on)
    for k, (label, value, key) in enumerate((("Часов в неделю", "%d ч" % round(covered / 60.0), "teal"),
                                             ("Дней без защиты", "%d" % sum(1 for dd in DAYS if not dd[3]), "red"))):
        kids.append(box("Tile%d" % k, 20 + k * (tw + 12), y, tw, 76, 10, C["tile"], C["tile_border"], [
            text("Tile%dLabel" % k, 14, 12, tw - 28, 18, label, 12, C["text3"]),
            text("Tile%dValue" % k, 14, 32, tw - 28, 34, value, 26, C[key], font=FONT_BIG),
        ]))
    kids.append(box("Note", 20, h - 20 - 44, w - 40, 44, 10, (1, 1, 1, 0.025), (1, 1, 1, 0.05), [
        icon_img("NoteIcon", 14, 14, 16, "clock", C["text3"]),
        text("NoteText", 40, 0, w - 80, 44, "Время указано по часам сервера", 12, C["text3"]),
    ]))
    return panel("Right", PAD + COL_L + GAP + COL_C + GAP, BODY_Y, w, h, kids)


def hints():
    items = [("Esc", "закрыть"), ("2×ЛКМ", "выдать доступ игроку рядом")]
    kids, x = [], 0
    for i, (k, lbl) in enumerate(items):
        node, adv = keycap("H%d" % i, x, 0, k, lbl)
        kids.append(node)
        x += adv
    return frame("Hints", 4, WIN_H + 14, x, 22, kids)


def build():
    win = frame("Window", 0, 0, WIN_W, WIN_H, [
        shape("Shadow", -56, -44, WIN_W + 112, WIN_H + 112, "IufShadow", C["shadow"]),
        shape("WindowBg", 0, 0, WIN_W, WIN_H, "IufR14", C["window"]),
        image("WindowSheen", 0, 0, WIN_W, 160, cell("iuf_grad_v"), (1, 1, 1, 0.025)),
        image("TopEdge", 40, 0, WIN_W - 80, 1, cell("iuf_grad_band"), (1, 1, 1, 0.16)),
        shape("WindowBorder", 0, 0, WIN_W, WIN_H, "IufO14", C["window_border"]),
        header(), left_panel(), centre_panel(), right_panel(), hints(),
    ], halign="@center_ref", valign="@center_ref")
    return W("FrameWidgetClass", "MockTerminal", (0, 0), (1, 1), [
        W("ImageWidgetClass", "Dim", (0, 0), (1, 1), image0=cell("iuf_white"), mode="@blend",
          src__alpha=1, stretch=1, color=C["overlay"], ignorepointer=1),
        win])


if __name__ == "__main__":
    out = os.path.join(PROFILE, "iuf_mock_terminal.layout")
    body = write_layout(build(), out)
    print("mockup -> %s (%d lines)" % (out, body.count("\n")))
