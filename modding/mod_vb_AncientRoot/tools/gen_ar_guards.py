# -*- coding: utf-8 -*-
"""Стражи рощи «Древнего корня» (ТЗ 2б.1): перекраска волка (ваниль), Занозы и Энта (MSFZ) в тему.

  "G:\\SteamLibrary\\steamapps\\common\\Blender\\5.2\\python\\bin\\python.exe" gen_ar_guards.py <папка_для_png>

Для каждого стража из ar_theme.AR_GUARDS:
  - исходная текстура тела (ImageToPAA -> TGA): шерсть/тело уводится в бурую кору, пятна мха,
    сеть золотых прожилок (тот же приём, что у зомби Изнанки: линии нулей полосового шума);
  - маска свечения: прожилки; у Энта ещё его собственная маска свечения (_em), перекрашенная в золото;
  - материал: исходный .rvmat (шейдер Super) с нашей маской в Stage8 и emmisive из палитры (g_emissive);
  - у волка ещё карточки шерсти (вторая скрытая секция CamoHair, fur_alpha): та же перекраска, альфа исходная.
Пишет ../VB_AncientRoot/data/guards/<file>_co.paa, <file>_e_co.paa, <file>.rvmat (у волка ещё
<file>_fur_ca/_fur_e_co/_fur.rvmat). Классы стражей — в config.cpp клиента (вместе с копиями DamageSystem: у существ
материал ставит только healthLevels, см. E77); при смене числа секций или материалов править и их.
"""
import os
import re
import subprocess
import sys

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import ar_theme as RT  # noqa: E402  палитра и стражи «Древнего корня», папка вывода
from ar_texlib import IMAGE_TO_PAA, read_tga, write_png, png_to_paa, spectral, smoothstep, blur, to_u8  # noqa: E402

T = RT.theme()
PAL = T["palette"]
OUT_DIR = os.path.join(RT.mod_dir(), "data", "guards")
BS = chr(92)
PREFIX = "VB_AncientRoot" + BS + "data" + BS + "guards" + BS
SEED = 5151


def C(key):
    return np.asarray(PAL[key], float)


def paa_rgba(src, tmp, tag):
    tga = os.path.join(tmp, tag + "_src.tga")
    r = subprocess.run([IMAGE_TO_PAA, src, tga], capture_output=True, text=True)
    if r.returncode != 0 or not os.path.exists(tga):
        raise RuntimeError("ImageToPAA: " + src + " " + r.stdout[-300:] + r.stderr[-300:])
    return read_tga(tga)


def own_emissive(g, tmp, h, w):
    """Собственная маска свечения модели (у Энта _em рядом с текстурой) -> яркость, подогнанная под размер."""
    em = g["tex"][:-4] + "_em.paa"
    if not os.path.exists(em):
        return None
    rgb, _ = paa_rgba(em, tmp, g["file"] + "_em")
    lum = rgb @ np.array([0.3, 0.59, 0.11], np.float32)
    if lum.shape != (h, w):
        ys = (np.arange(h) * lum.shape[0] / h).astype(int)
        xs = (np.arange(w) * lum.shape[1] / w).astype(int)
        lum = lum[ys][:, xs]
    return lum


def recolor(rgb, rng, vein_keep, own=None, e_base=0.0, own_veins=False, moss_amount=None):
    """Текстура -> (цвет, маска свечения): бурая кора, пятна мха, сеть золотых прожилок.
    own_veins — прожилки не из шума, а из собственной маски модели own (Энт: борозды коры вдоль волокон)."""
    h, w, _ = rgb.shape
    n = max(h, w)
    lum = rgb @ np.array([0.3, 0.59, 0.11], np.float32)

    # тело -> бурая кора, часть исходного рисунка сохраняем
    base = (0.25 + 0.95 * lum)[..., None] * C("g_body")
    base = base * (1 - PAL["g_keep"]) + rgb * PAL["g_keep"]
    # пятна мха
    thr = 1.0 - 2.0 * (PAL["g_moss_amount"] if moss_amount is None else moss_amount)
    mm = smoothstep(thr, thr + 0.7, spectral(n, rng, beta=1.5)[:h, :w]) * 0.85
    mtone = (0.35 + 0.9 * lum)[..., None] * (0.85 + 0.25 * spectral(n, rng, beta=0.8)[:h, :w, None].clip(-1, 1))
    base = base * (1 - mm[..., None]) + C("g_moss") * mtone * mm[..., None]
    if own_veins and own is not None:
        # Дерево (баг-просьба 2026-11-20: «жилки волнами — нелогично»): светятся борозды самой коры — собственная
        # маска модели идёт вдоль волокон. Крошку сводим в линии (лёгкое размытие + порог), силу слегка меняем
        # крупными пятнами, чтобы ствол не горел ровно. Шумовой сети нет.
        o = blur(own.astype(np.float32), 0.8)
        veins = smoothstep(0.20, 0.40, o)
        veins *= (0.55 + 0.45 * smoothstep(-0.8, 1.2, spectral(n, rng, beta=1.8)))[:h, :w]
        halo = np.clip(blur(veins, 2.0) * 1.2, 0, 1)
        co = base * (1 - 0.35 * halo[..., None]) + C("g_halo") * 0.35 * halo[..., None]
        co = co * (1 - 0.85 * veins[..., None]) + C("g_vein") * 0.85 * veins[..., None]
        e = C("g_e_vein") * np.clip(veins * PAL["g_e_gain"] + halo * 0.08, 0, 1)[..., None]
        return co, e
    # золотые прожилки: редкая сеть, местами пропадает
    n1 = spectral(n, rng, f0=18, bw=6.0)
    k0, k1 = vein_keep                              # выше порог — реже прожилки (у волка шерсть, им мало места)
    keep = smoothstep(k0, k1, spectral(n, rng, beta=1.6))
    width = 0.08 + 0.08 * smoothstep(-1, 1, spectral(n, rng, beta=1.5))
    veins = np.clip(np.exp(-(n1 / width) ** 2) * keep, 0, 1)[:h, :w]
    halo = np.clip(blur(veins, 3.0) * 1.8, 0, 1)
    co = base * (1 - 0.5 * halo[..., None]) + C("g_halo") * 0.5 * halo[..., None]
    co = co * (1 - veins[..., None]) + C("g_vein") * veins[..., None]
    e = C("g_e_vein") * np.clip(veins * PAL["g_e_gain"] + halo * 0.15, 0, 1)[..., None]
    if e_base:
        # ровное слабое свечение всего тела (тонкие модели, на которых сеть не читается)
        e = np.maximum(e, C("g_e_vein") * (e_base * (0.5 + lum))[..., None])
    if own is not None:
        e = np.maximum(e, C("g_e_own") * own[..., None])
    return co, e


def write_tex(tmp, name, arr):
    png = os.path.join(tmp, name + ".png")
    write_png(png, arr)
    png_to_paa(png, os.path.join(OUT_DIR, name + ".paa"))


def make_guard(g, tmp):
    os.makedirs(OUT_DIR, exist_ok=True)
    rgb, alpha = paa_rgba(g["tex"], tmp, g["file"])
    h, w, _ = rgb.shape
    rng = np.random.default_rng(SEED + sum(map(ord, g["file"])))
    own = own_emissive(g, tmp, h, w)
    co, e = recolor(rgb, rng, g.get("vein_keep", (-0.2, 0.7)), own, g.get("e_base", 0.0),
                    g.get("own_veins", False), g.get("moss_amount"))
    has_alpha = float(alpha.min()) < 0.999
    co_arr = np.concatenate([to_u8(co), to_u8(alpha)[..., None]], -1) if has_alpha else to_u8(co)
    write_tex(tmp, g["file"] + "_co", co_arr)
    write_tex(tmp, g["file"] + "_e_co", to_u8(e))
    write_rvmat(g["rvmat"], g["file"], g.get("emissive"))

    fur = g.get("fur")
    if fur:
        # карточки шерсти: альфа (AlphaTest) остаётся исходной, иначе шерсть станет сплошной
        frgb, falpha = paa_rgba(fur["tex"], tmp, g["file"] + "_fur")
        frng = np.random.default_rng(SEED + 7 + sum(map(ord, g["file"])))
        fco, fe = recolor(frgb, frng, fur.get("vein_keep", (0.9, 1.7)), None, fur.get("e_base", 0.0))
        write_tex(tmp, g["file"] + "_fur_ca", np.concatenate([to_u8(fco), to_u8(falpha)[..., None]], -1))
        write_tex(tmp, g["file"] + "_fur_e_co", to_u8(fe))
        write_rvmat(fur["rvmat"], g["file"] + "_fur", g.get("emissive"))
    print("страж", g["cls"], w, h, "своё свечение" if own is not None else "", "шерсть" if fur else "")


def stage_block(txt, n):
    m = re.search(r"class\s+Stage%d\s*\{" % n, txt)
    if not m:
        return None
    depth, i = 1, m.end()
    while depth:
        depth += txt[i] == "{"
        depth -= txt[i] == "}"
        i += 1
    end = i
    if end < len(txt) and txt[end] == ";":
        end += 1
    return m.start(), end


STAGE_DEFAULT = {
    1: "#(argb,8,8,3)color(0.5,0.5,1,1,NOHQ)", 2: "#(argb,8,8,3)color(0.5,0.5,0.5,1,DT)",
    3: "#(argb,8,8,3)color(0,0,0,0,MC)", 4: "#(argb,8,8,3)color(1,1,1,1,AS)",
    5: "#(argb,8,8,3)color(0,0,0,1,SMDI)", 6: "#(ai,64,64,1)fresnel(1,0.7)", 7: "dz\\data\\data\\env_land_co.paa",
}


def rebuild_texgen(txt, e_tex):
    """Материал в стиле uvSource -> стиль texGen (как у рабочих зомби Изнанки): шапка (цвета, шейдер, renderFlags)
    как была, текстуры Stage1..7 из исходника, все стадии на TexGen0..7, Stage8 — маска свечения на TexGen0."""
    first = re.search(r"^\s*class\s+", txt, re.M)
    head = txt[:first.start()] if first else txt
    tex = {}
    for n in range(1, 8):
        s = stage_block(txt, n)
        m = re.search(r'texture\s*=\s*"([^"]*)"', txt[s[0]:s[1]]) if s else None
        tex[n] = m.group(1) if m else STAGE_DEFAULT[n]
    out = [head.rstrip("\n"),
           "class TexGen0\n{\n\tuvSource = \"tex\";\n\tclass uvTransform\n\t{\n\t\taside[] = {1,0,0};\n"
           "\t\tup[] = {0,1,0};\n\t\tdir[] = {0,0,1};\n\t\tpos[] = {0,0,0};\n\t};\n};"]
    out += ["class TexGen%d: TexGen0{};" % n for n in range(1, 8)]
    out += ['class Stage%d\n{\n\ttexture = "%s";\n\ttexGen = "%d";\n};' % (n, tex[n], n) for n in range(1, 8)]
    out.append('class Stage8\n{\n\ttexture = "%s";\n\ttexGen = "0";\n};' % e_tex)
    return "\n".join(out) + "\n"


def write_rvmat(src, name, k=None):
    """Исходный материал + наша маска свечения в Stage8 (заменить или добавить по образцу Stage7).
    k — сила свечения стража (поле emissive в AR_GUARDS), иначе общая g_emissive палитры."""
    with open(src, encoding="utf-8", errors="replace") as f:
        txt = f.read()
    e_tex = PREFIX + name + "_e_co.paa"
    if k is None:
        k = PAL.get("g_emissive", 1.0)
    emis = "emmisive[] = {%g,%g,%g,1};" % (k, k, k)
    txt, n = re.subn(r"emmisive\[\]\s*=\s*\{[^}]*\}\s*;", emis, txt, count=1)
    if not n:
        txt = emis + "\n" + txt
    if not re.search(r"class\s+TexGen0\b", txt):
        # Стенд 11-15: Stage8 в стиле uvSource (волк, Заноза) движок не светит, а в стиле texGen (Энт, корка,
        # зомби Изнанки) — светит. Такой материал пересобираем в стиль texGen: TexGen0..7 + Stage1..7 + Stage8
        txt = rebuild_texgen(txt, e_tex)
    s8 = stage_block(txt, 8)
    if s8:
        blk = txt[s8[0]:s8[1]]
        blk = re.sub(r'texture\s*=\s*"[^"]*"', 'texture = "%s"' % e_tex.replace(BS, BS + BS), blk, count=1)
        txt = txt[:s8[0]] + blk + txt[s8[1]:]
    else:
        # своя стадия свечения после Stage7, в том же стиле координат, что у файла (texGen или uvSource)
        s7 = stage_block(txt, 7)
        if not s7:
            raise RuntimeError("в %s нет Stage7 — некуда добавить Stage8" % src)
        if re.search(r"texGen\s*=", txt):
            blk = 'class Stage8\n{\n\ttexture = "%s";\n\ttexGen = "0";\n};' % e_tex
        else:
            blk = ('class Stage8\n{\n\ttexture = "%s";\n\tuvSource = "tex";\n\tclass uvTransform\n\t{\n'
                   '\t\taside[] = {1,0,0};\n\t\tup[] = {0,1,0};\n\t\tdir[] = {0,0,0};\n\t\tpos[] = {0,0,0};\n\t};\n};' % e_tex)
        txt = txt[:s7[1]] + "\n" + blk + txt[s7[1]:]
    # шапка DeRap (распакованные ванильные материалы) не нужна
    txt = "\n".join(l for l in txt.split("\n") if not l.startswith("////") and not l.startswith("//DeRap")
                    and not l.startswith("//Produced") and not l.startswith("//https") and not l.startswith("//'now'")
                    and l.strip() != "#define _ARMA_")
    txt = "// VB_AncientRoot «Древний корень»: страж рощи — копия " + os.path.basename(src) + " + золотое свечение (Stage8)\n" + txt
    with open(os.path.join(OUT_DIR, name + ".rvmat"), "w", encoding="utf-8", newline="\n") as f:
        f.write(txt)


def main():
    if "guards" not in T:
        print("в теме %s стражей нет" % RT.theme_name())
        return
    tmp = sys.argv[1] if len(sys.argv) > 1 else os.path.join(HERE, "_guards")
    os.makedirs(tmp, exist_ok=True)
    for g in T["guards"]:
        make_guard(g, tmp)
    print("готово")


if __name__ == "__main__":
    main()
