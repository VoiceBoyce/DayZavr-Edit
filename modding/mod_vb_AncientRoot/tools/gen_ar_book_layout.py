# -*- coding: utf-8 -*-
"""Разметка окна «Книги древних» (ТЗ_сервер_18.md, ветка 2в): пишет ../VB_AncientRoot/gui/layouts/vb_ar_book.layout.

  python gen_ar_book_layout.py

Единицы точные (hexact/vexact), это 1/1080 высоты экрана. Разворот 1240 x 820 по центру (чуть левее: справа
выступают закладки). Текстуры грузит скрипт (VB_AR_BookMenu), здесь только геометрия, шрифты и маски ветви.
Порядок отрисовки задаёт priority: обложка < закладки < страницы < сгиб < содержимое < стрелки.
Закладки торчат из-под обложки справа (обложка рисуется поверх их левого края). Рамка VB_AR_Book обрезает всё, что
выходит за её край (стенд 2026-11-16), поэтому она шире обложки: 1450 = 1240 + закладки; сдвиг 41 ставит обложку
на 64 левее центра экрана.
TextWidget при том же "exact text size" рисует шрифт заметно мельче RichTextWidget (стенд 2026-11-16):
текст страниц (RichText) 15-16, заголовки и подписи (TextWidget) 21-44. RichTextWidget сам уменьшает шрифт,
если текст не влезает в рамку, — поэтому размер с запасом, чтобы все страницы читались одним кеглем.
"""
import os

HERE = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.join(HERE, "..", "VB_AncientRoot", "gui", "layouts", "vb_ar_book.layout")

FONT = "gui/fonts/AmorSerifPro"
FONT_B = "gui/fonts/AmorSerifPro-Bold"
TABS = 10
PAGE_W, PAGE_H = 560, 740


def widget(cls, name, x, y, w, h, extra=None, children=None, ignore=True, prio=None, visible=1, exact=True):
    lines = ["%s %s {" % (cls, name), " visible %d" % visible]
    if ignore:
        lines.append(" ignorepointer 1")
    if prio is not None:
        lines.append(" priority %d" % prio)
    lines.append(" position %s %s" % (x, y))
    lines.append(" size %s %s" % (w, h))
    e = 1 if exact else 0
    lines += [" hexactpos 1", " vexactpos 1", " hexactsize %d" % e, " vexactsize %d" % e]
    for k in extra or []:
        lines.append(" " + k)
    if children:
        lines.append(" {")
        for c in children:
            lines += ["  " + s for s in c]
        lines.append(" }")
    lines.append("}")
    return lines


IMG = ['mode blend', '"src alpha" 1', 'stretch 1']


def image(name, x, y, w, h, prio=None, extra=None, ignore=True, visible=1):
    return widget("ImageWidgetClass", name, x, y, w, h, IMG + (extra or []), ignore=ignore, prio=prio, visible=visible)


def text(name, x, y, w, h, size, bold=False, halign="left", valign="center", prio=None):
    ex = ['font "%s"' % (FONT_B if bold else FONT), '"exact text" 1', '"exact text size" %d' % size,
          '"text halign" %s' % halign, '"text valign" %s' % valign]
    return widget("TextWidgetClass", name, x, y, w, h, ex, prio=prio)


def rich(name, x, y, w, h, size, halign="left", prio=None):
    ex = ['font "%s"' % FONT, '"exact text" 1', '"exact text size" %d' % size, 'wrap 1',
          '"text halign" %s' % halign, '"text valign" top']
    return widget("RichTextWidgetClass", name, x, y, w, h, ex, prio=prio)


def preview(name, x, y, w, h, prio=None):
    return widget("ItemPreviewWidgetClass", name, x, y, w, h, ['"force flip enable" 1'], prio=prio)


def side(s, px):
    """Содержимое одной страницы (s = L/R)."""
    ch = []
    ch.append(text("VB_AR_BookTitle" + s, 44, 40, 472, 50, 40, bold=True, halign="center"))
    ch.append(image("VB_AR_BookOrn" + s, 106, 92, 348, 22))
    ch.append(preview("VB_AR_BookPic" + s, 190, 110, 180, 170, prio=5))
    ch.append(rich("VB_AR_BookBody" + s, 52, 124, 456, 460, 16))
    ch.append(image("VB_AR_BookNoteBg" + s, 48, 588, 464, 102))
    ch.append(rich("VB_AR_BookNote" + s, 62, 596, 436, 90, 15))
    ch.append(text("VB_AR_BookNum" + s, 230, 692, 100, 28, 22, halign="center"))
    ch.append(image("VB_AR_BookRunes" + s, 60, 130, 440, 220))
    ch.append(rich("VB_AR_BookLock" + s, 64, 370, 432, 210, 16, halign="center"))
    for k in (1, 2):
        ent = [
            preview("VB_AR_BookEntPic%d%s" % (k, s), 0, 8, 150, 150, prio=5),
            text("VB_AR_BookEntName%d%s" % (k, s), 160, 0, 312, 38, 30, bold=True),
            rich("VB_AR_BookEntText%d%s" % (k, s), 160, 40, 312, 232, 16),
        ]
        ch.append(widget("FrameWidgetClass", "VB_AR_BookEnt%d%s" % (k, s), 44, 108 + (k - 1) * 288, 472, 276,
                         children=ent))
    if s == "R":
        # «Ты и лес»: руна на тёмном круге, ступень, ветвь как в худе, значение
        track = [
            image("VB_AR_BookYouBranch", 0, 0, 400, 48),
            image("VB_AR_BookYouThorn", 0, 0, 400, 48, extra=['Mask "VB_AncientRoot/gui/vb_ar_mask_left_ca.paa"',
                                                              '"Transition width" 0.04', 'Progress 0']),
            image("VB_AR_BookYouBloom", 0, 0, 400, 48, extra=['Mask "VB_AncientRoot/gui/vb_ar_mask_right_ca.paa"',
                                                              '"Transition width" 0.04', 'Progress 0']),
            image("VB_AR_BookYouBud", 189, 13, 22, 22),
        ]
        you = [
            image("VB_AR_BookYouDisc", 226, 108, 108, 108),
            image("VB_AR_BookYouRune", 238, 120, 84, 84),
            text("VB_AR_BookYouTier", 44, 220, 472, 50, 44, bold=True, halign="center"),
            widget("FrameWidgetClass", "VB_AR_BookYouTrack", 80, 272, 400, 48, children=track),
            text("VB_AR_BookYouValue", 44, 322, 472, 32, 26, halign="center"),
        ]
        ch.append(widget("FrameWidgetClass", "VB_AR_BookYou", 0, 0, PAGE_W, PAGE_H, children=you))
    return widget("FrameWidgetClass", "VB_AR_BookSide" + s, px, 40, PAGE_W, PAGE_H, children=ch, prio=4)


def main():
    book = []
    book.append(image("VB_AR_BookCover", 0, 0, 1240, 820, prio=0, ignore=False))
    for i in range(TABS):
        tab = [
            image("VB_AR_BookTabImg%d" % i, 0, 0, 200, 52),
            text("VB_AR_BookTabText%d" % i, 20, 0, 156, 52, 21, bold=True),
        ]
        book.append(widget("ButtonWidgetClass", "VB_AR_BookTab%d" % i, 1226, 66 + i * 70, 200, 52,
                           ['style Empty', '"no focus" 1'], children=tab, ignore=False, prio=1))
    book.append(image("VB_AR_BookPageL", 40, 40, PAGE_W, PAGE_H, prio=2, ignore=False))
    book.append(image("VB_AR_BookPageR", 640, 40, PAGE_W, PAGE_H, prio=2, ignore=False, extra=['"flip u" 1']))
    book.append(image("VB_AR_BookSpine", 590, 40, 60, PAGE_H, prio=3))
    book.append(side("L", 40))
    book.append(side("R", 640))
    for name, x, flip in (("VB_AR_BookPrev", 66, True), ("VB_AR_BookNext", 1090, False)):
        img = image(name + "Img", 0, 0, 84, 42, extra=['"flip u" 1'] if flip else None)
        book.append(widget("ButtonWidgetClass", name, x, 722, 84, 42, ['style Empty', '"no focus" 1'],
                           children=[img], ignore=False, prio=6))
    root_children = [
        # затемнение во весь экран (размер в долях): клик мимо книги закрывает окно
        widget("ImageWidgetClass", "VB_AR_BookDim", 0, 0, 1, 1, IMG + ['color 0 0 0 0.6'], ignore=False, prio=0,
               exact=False),
        widget("FrameWidgetClass", "VB_AR_Book", 41, 0, 1450, 820, ['halign center_ref', 'valign center_ref'],
               children=book, prio=1),
    ]
    root = widget("FrameWidgetClass", "VB_AR_BookRoot", 0, 0, 1, 1, children=root_children, exact=False)
    with open(OUT, "w", encoding="utf-8", newline="\r\n") as f:
        f.write("\n".join(root) + "\n")
    print("ok:", os.path.normpath(OUT))


if __name__ == "__main__":
    main()
