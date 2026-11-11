# -*- coding: utf-8 -*-
"""Зомби Изнанки: перекрас ванильных инфицированных (текстуры, маски свечения, материалы).

Запуск (Python из Blender, в нём есть numpy):
  "G:\\SteamLibrary\\steamapps\\common\\Blender\\5.2\\python\\bin\\python.exe" gen_rift_zombies.py <папка_для_png> [--preview]

Что делает для каждой базы из ZOMBIES:
  - берёт ванильную _co с P: (ImageToPAA -> несжатый TGA, читается numpy);
  - уводит одежду в тёмный бордовый, кожу делает мертвенно-серой;
  - по коже рисует сетку жил тем же приёмом, что у корней (линии нулей полосового шума);
  - глазные яблоки перекрашивает: кровавый белок, светящаяся красная радужка, тёмный зрачок;
  - маска свечения (Stage8, как у ванильного Spooky_Goat_e): радужка ярко, жилы слабо;
  - пишет VB_Rift/data/zombies/<имя>_co.paa, <имя>_e_co.paa и <имя>.rvmat.
Остальные стадии материала (нормали, smdi) — ванильные, UV у модели те же.
С --preview пишет только картинки проверки: маска кожи и найденные зрачки поверх текстуры.
"""
import os
import struct
import subprocess
import sys
import zlib

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
BS = chr(92)
OUT_DIR = os.path.normpath(os.path.join(HERE, "..", "VB_Rift", "data", "zombies"))
PREFIX = "VB_Rift" + BS + "data" + BS + "zombies" + BS
SRC = "P:" + BS + "DZ" + BS + "characters" + BS + "zombies" + BS + "data" + BS
SRC_CFG = BS.join(["dz", "characters", "zombies", "data"]) + BS
IMAGE_TO_PAA = os.path.join("D:" + BS, "SteamLibrary", "steamapps", "common", "DayZ Tools",
                            "Bin", "ImageToPAA", "ImageToPAA.exe")
SEED = 4242

# база: ванильный класс, текстура, материал (nohq/smdi берутся из него), окна глазных яблок
# (x0, y0, x1, y1 в пикселях 2048x2048, верх-лево) — сняты глазами по превью текстур
ZOMBIES = [
    dict(name="vb_rift_zmb_villager_m", tex="villager_old_m_CO", rvmat="villager_old_m",
         eyes=[(875, 12, 1015, 62), (868, 78, 1008, 126)], pupils=[(945, 36), (947, 95)],
         no_skin=[(640, 0, 1015, 215)]),
    dict(name="vb_rift_zmb_villager_f", tex="villager_old_f_red_CO", rvmat="villager_old_f",
         eyes=[(878, 4, 1025, 70), (855, 70, 1022, 145)], pupils=[(945, 32), (944, 97)],
         no_skin=[(640, 0, 1030, 215)]),
    dict(name="vb_rift_zmb_prisoner", tex="prisoner_skinny_m_co", rvmat="prisoner_skinny_m",
         eyes=[(885, 8, 1028, 62), (868, 72, 1012, 128)],
         no_skin=[(640, 0, 1030, 215)]),
    dict(name="vb_rift_zmb_jacket", tex="jacket_grey_co", rvmat="jacket",
         eyes=[(825, 38, 992, 102), (820, 100, 962, 172)],
         no_skin=[(640, 0, 1000, 215)]),
]

EMISSIVE = 3.0   # emmisive[] материала; яркость каждого пикселя задаёт маска


# ---------------------------------------------------------------- картинки
def read_tga(path):
    with open(path, "rb") as f:
        d = f.read()
    idlen, cmap, itype = d[0], d[1], d[2]
    w, h, bpp, desc = struct.unpack("<HHBB", d[12:18])
    if itype != 2 or cmap != 0 or bpp not in (24, 32):
        raise RuntimeError("TGA не того формата: " + path)
    c = bpp // 8
    px = np.frombuffer(d, np.uint8, w * h * c, 18 + idlen).reshape(h, w, c)
    if not desc & 0x20:
        px = px[::-1]
    rgb = px[..., 2::-1].astype(np.float32) / 255.0
    a = px[..., 3].astype(np.float32) / 255.0 if c == 4 else np.ones((h, w), np.float32)
    return rgb, a


def write_png(path, arr):
    arr = np.ascontiguousarray(arr)
    h, w, c = arr.shape
    ct = 2 if c == 3 else 6
    raw = b"".join(b"\x00" + arr[y].tobytes() for y in range(h))

    def chunk(t, dd):
        return struct.pack(">I", len(dd)) + t + dd + struct.pack(">I", zlib.crc32(t + dd) & 0xFFFFFFFF)

    data = b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", w, h, 8, ct, 0, 0, 0)) \
        + chunk(b"IDAT", zlib.compress(raw, 6)) + chunk(b"IEND", b"")
    with open(path, "wb") as f:
        f.write(data)


def to_u8(x):
    return (np.clip(x, 0, 1) * 255 + 0.5).astype(np.uint8)


def paa_to_tga(name, tmp_dir):
    dst = os.path.join(tmp_dir, name + "_src.tga")
    if not os.path.exists(dst):
        r = subprocess.run([IMAGE_TO_PAA, SRC + name + ".paa", dst], capture_output=True, text=True)
        if not os.path.exists(dst):
            raise RuntimeError("ImageToPAA: " + name + " " + r.stdout[-300:] + r.stderr[-300:])
    return dst


def png_to_paa(png, dst):
    r = subprocess.run([IMAGE_TO_PAA, png, dst], capture_output=True, text=True)
    if r.returncode != 0 or not os.path.exists(dst):
        raise RuntimeError("ImageToPAA: " + png + " " + r.stdout[-300:] + r.stderr[-300:])


# ---------------------------------------------------------------- шум и фильтры
def smoothstep(e0, e1, x):
    t = np.clip((np.asarray(x, np.float32) - e0) / (e1 - e0), 0, 1)
    return t * t * (3 - 2 * t)


def spectral(n, rng, f0=None, bw=None, beta=None, fmin=1.0):
    w = rng.standard_normal((n, n)).astype(np.float32)
    F = np.fft.rfft2(w)
    fy = np.fft.fftfreq(n)[:, None] * n
    fx = np.fft.rfftfreq(n)[None, :] * n
    f = np.hypot(fy, fx)
    if beta is not None:
        filt = np.where(f >= fmin, np.maximum(f, 1e-9) ** (-beta), 0.0)
    else:
        filt = np.exp(-((f - f0) / bw) ** 2)
        filt[0, 0] = 0
    out = np.fft.irfft2(F * filt, s=(n, n)).astype(np.float32)
    return out / out.std()


def blur(img, sigma):
    n0, n1 = img.shape
    fy = np.fft.fftfreq(n0)[:, None]
    fx = np.fft.rfftfreq(n1)[None, :]
    g = np.exp(-2 * (np.pi * sigma) ** 2 * (fy ** 2 + fx ** 2))
    return np.fft.irfft2(np.fft.rfft2(img) * g, s=img.shape).astype(np.float32)


def box_mean(img, r):
    """Среднее в квадрате (2r+1)^2 через суммы по префиксам."""
    p = np.pad(img, r + 1, mode="edge").astype(np.float64)
    c = p.cumsum(0).cumsum(1)
    k = 2 * r + 1
    s = c[k:, k:] - c[:-k, k:] - c[k:, :-k] + c[:-k, :-k]
    return (s / (k * k))[: img.shape[0], : img.shape[1]].astype(np.float32)


def rgb_to_hsv(rgb):
    r, g, b = rgb[..., 0], rgb[..., 1], rgb[..., 2]
    mx = rgb.max(-1)
    mn = rgb.min(-1)
    d = mx - mn
    s = np.where(mx > 1e-6, d / np.maximum(mx, 1e-6), 0)
    dd = np.maximum(d, 1e-6)
    h = np.where(mx == r, ((g - b) / dd) % 6, np.where(mx == g, (b - r) / dd + 2, (r - g) / dd + 4)) / 6.0
    h = np.where(d < 1e-6, 0, h)
    return h, s, mx


# ---------------------------------------------------------------- маски
def skin_mask(rgb, alpha):
    """Кожа ванильных зомби: тёплый бежево-розовый, умеренная насыщенность."""
    h, s, v = rgb_to_hsv(rgb)
    warm = np.where(h > 0.5, h - 1.0, h)   # красные оттенки около 0 с обеих сторон
    m = smoothstep(0.012, 0.035, warm) * (1 - smoothstep(0.10, 0.14, warm))   # красная ткань краснее кожи
    m *= smoothstep(0.10, 0.18, s) * (1 - smoothstep(0.52, 0.62, s))
    m *= smoothstep(0.22, 0.32, v) * (1 - smoothstep(0.92, 0.98, v))
    m *= (rgb[..., 0] - rgb[..., 2] > 0.05)
    m *= smoothstep(0.4, 0.8, alpha)
    m = box_mean(m.astype(np.float32), 10)           # убрать соль-перец и мелкие пятна грязи
    return smoothstep(0.45, 0.8, m)


def find_pupil(rgb, win):
    """Зрачок: тёмное пятно в светлом кольце радужки/белка — максимум разности крупного и мелкого среднего."""
    x0, y0, x1, y1 = win
    lum = rgb[..., 0] * 0.3 + rgb[..., 1] * 0.59 + rgb[..., 2] * 0.11
    big = box_mean(lum, 16)
    small = box_mean(lum, 4)
    score = (big - small)[y0:y1, x0:x1]
    score *= (small[y0:y1, x0:x1] < 0.35)
    iy, ix = np.unravel_index(np.argmax(score), score.shape)
    return x0 + ix, y0 + iy, float(score[iy, ix])


# ---------------------------------------------------------------- перекрас
def make_zombie(z, tmp_dir, preview):
    rgb, alpha = read_tga(paa_to_tga(z["tex"], tmp_dir))
    h, w, _ = rgb.shape
    skin = skin_mask(rgb, alpha)
    for x0, y0, x1, y1 in z.get("no_skin", []):     # рот, зубы, глаза: без жил
        skin[y0:y1, x0:x1] = 0
    if "pupils" in z:
        pupils = [(px, py, 0.0) for px, py in z["pupils"]]
    else:
        pupils = [find_pupil(rgb, win) for win in z["eyes"]]
    if preview:
        pv = rgb * (1 - 0.6 * skin[..., None]) + np.array([0, 1, 0.3]) * 0.6 * skin[..., None]
        yy, xx = np.mgrid[0:h, 0:w]
        for (x0, y0, x1, y1), (px, py, sc) in zip(z["eyes"], pupils):
            pv[y0:y1, x0] = pv[y0:y1, x1 - 1] = (0, 1, 1)
            pv[y0, x0:x1] = pv[y1 - 1, x0:x1] = (0, 1, 1)
            ring = np.abs(np.hypot(xx - px, yy - py) - 14) < 1.5
            pv[ring] = (1, 0, 1)
            print("  зрачок", z["name"], (px, py), "оценка %.3f" % sc)
        write_png(os.path.join(tmp_dir, z["name"] + "_preview.png"), to_u8(pv))
        return None

    rng = np.random.default_rng(SEED + sum(map(ord, z["name"])))
    n = max(h, w)
    # сетка жил: линии нулей полосового шума, толщина плавает, местами пропадает
    n1 = spectral(n, rng, f0=26, bw=8.0)
    n2 = spectral(n, rng, f0=70, bw=20.0)
    keep = smoothstep(-0.6, 0.4, spectral(n, rng, beta=1.6))
    width = 0.10 + 0.10 * smoothstep(-1, 1, spectral(n, rng, beta=1.5))
    veins = np.exp(-(n1 / width) ** 2) * keep + 0.55 * np.exp(-(n2 / 0.12) ** 2) * keep * (1 - keep * 0.4)
    veins = np.clip(veins[:h, :w], 0, 1) * skin
    halo = np.clip(blur(veins, 4.0) * 2.0, 0, 1) * skin
    grime = spectral(n, rng, beta=1.3)[:h, :w]

    co = rgb.copy()
    lum = co @ np.array([0.3, 0.59, 0.11], np.float32)
    # одежда: в тёмный бордовый, без кислотных цветов
    cloth = 1 - skin
    dull = lum[..., None] * np.array([0.95, 0.55, 0.55]) * 0.85
    co = co * (1 - 0.65 * cloth[..., None]) + dull * 0.65 * cloth[..., None]
    # кожа: мертвенно-серая с синевой, пятнами краснее
    dead = lum[..., None] * np.array([0.92, 0.86, 0.90])
    blot = smoothstep(0.3, 1.6, grime)[..., None]
    dead = dead * (1 - 0.35 * blot) + np.array([0.30, 0.07, 0.08]) * 0.35 * blot
    co = co * (1 - 0.75 * skin[..., None]) + dead * 0.75 * skin[..., None]
    # жилы под кожей: тёмный ореол и красная нить
    co = co * (1 - 0.45 * halo[..., None]) + np.array([0.22, 0.03, 0.05]) * 0.45 * halo[..., None]
    co = co * (1 - veins[..., None]) + np.array([0.62, 0.06, 0.10]) * veins[..., None]

    e = np.array([1.0, 0.16, 0.24]) * (veins * 0.35)[..., None] + np.array([0.4, 0.02, 0.05]) * (halo * 0.08)[..., None]

    # глаза: кровавый белок по всему окну, светящаяся радужка вокруг найденного зрачка
    yy, xx = np.mgrid[0:h, 0:w].astype(np.float32)
    for (x0, y0, x1, y1), (px, py, _) in zip(z["eyes"], pupils):
        cx, cy = (x0 + x1) * 0.5, (y0 + y1) * 0.5
        rx, ry = (x1 - x0) * 0.5, (y1 - y0) * 0.5
        ball = 1 - smoothstep(0.8, 1.0, np.hypot((xx - cx) / rx, (yy - cy) / ry))
        co = co * (1 - 0.8 * ball[..., None]) + co * np.array([1.0, 0.25, 0.22]) * 0.8 * ball[..., None] \
            + np.array([0.18, 0.0, 0.0]) * ball[..., None]
        R = min(rx, ry) * 0.85
        d = np.hypot(xx - px, yy - py) / R
        iris = (1 - smoothstep(0.85, 1.0, d)) * ball
        pupil = 1 - smoothstep(0.22, 0.32, d)
        # рисунок исходной радужки сохраняем яркостью, цвет — красный
        detail = np.clip(0.55 + 1.1 * (lum - 0.25), 0.45, 1.25)
        iris_col = np.array([1.0, 0.18, 0.08]) * ((0.75 + 0.25 * (1 - d)) * detail)[..., None]
        co = co * (1 - iris[..., None]) + iris_col * iris[..., None]
        co = co * (1 - pupil[..., None]) + np.array([0.08, 0.0, 0.0]) * pupil[..., None]
        glow = np.clip(iris * (1 - 0.8 * pupil), 0, 1)
        e = e * (1 - glow[..., None]) + np.array([1.0, 0.12, 0.06]) * glow[..., None]
        e += np.array([0.25, 0.0, 0.0]) * (ball * (1 - iris) * 0.4)[..., None]

    co_rgba = np.concatenate([to_u8(co), to_u8(alpha)[..., None]], -1)
    os.makedirs(OUT_DIR, exist_ok=True)
    out = {}
    for suffix, arr in (("_co", co_rgba), ("_e_co", to_u8(e))):
        png = os.path.join(tmp_dir, z["name"] + suffix + ".png")
        write_png(png, arr)
        dst = os.path.join(OUT_DIR, z["name"] + suffix + ".paa")
        png_to_paa(png, dst)
        out[suffix] = png
    write_rvmat(z)
    return out


# ---------------------------------------------------------------- материал
def vanilla_stage(rvmat_name, stage):
    """Текстура стадии из ванильного материала зомби (распакован на P: в текст)."""
    path = SRC + rvmat_name + ".rvmat"
    with open(path, encoding="utf-8", errors="replace") as f:
        txt = f.read()
    i = txt.find("class Stage%d" % stage)
    j = txt.find("texture", i)
    k1 = txt.find('"', j)
    k2 = txt.find('"', k1 + 1)
    return txt[k1 + 1:k2]


def write_rvmat(z):
    nohq = vanilla_stage(z["rvmat"], 1)
    smdi = vanilla_stage(z["rvmat"], 5)
    tg = ("class TexGen0\n{\n\tuvSource = \"tex\";\n\tclass uvTransform\n\t{\n"
          "\t\taside[] = {1,0,0};\n\t\tup[] = {0,1,0};\n\t\tdir[] = {0,0,1};\n\t\tpos[] = {0,0,0};\n\t};\n};\n")
    tg += "".join("class TexGen%d: TexGen0{};\n" % i for i in range(1, 8))
    stages = [
        (0, PREFIX + z["name"] + "_co.paa", 0),
        (1, nohq, 1), (2, "#(argb,8,8,3)color(0.5,0.5,0.5,1,DT)", 2), (3, "#(argb,8,8,3)color(0,0,0,0,MC)", 3),
        (4, "#(argb,8,8,3)color(1,1,1,1,AS)", 4), (5, smdi, 5), (6, "#(ai,64,64,1)fresnel(1,0.7)", 6),
        (7, "dz" + BS + "data" + BS + "data" + BS + "env_land_co.paa", 7),
        (8, PREFIX + z["name"] + "_e_co.paa", 0),
    ]
    s = "ambient[] = {2,2,2,1};\ndiffuse[] = {1,1,1,1};\nforcedDiffuse[] = {0,0,0,0};\n"
    s += "emmisive[] = {%.2f,%.2f,%.2f,1};\n" % (EMISSIVE, EMISSIVE, EMISSIVE)
    s += "specular[] = {0.2,0.2,0.2,1};\nspecularPower = 80;\n"
    s += "PixelShaderID = \"Super\";\nVertexShaderID = \"Super\";\n" + tg
    for i, tx, gen in stages:
        s += "class Stage%d\n{\n\ttexture = \"%s\";\n\ttexGen = \"%d\";\n};\n" % (i, tx, gen)
    with open(os.path.join(OUT_DIR, z["name"] + ".rvmat"), "w", encoding="utf-8", newline="\n") as f:
        f.write(s)


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    preview = "--preview" in sys.argv
    tmp_dir = args[0] if args else os.path.join(HERE, "_zombies")
    os.makedirs(tmp_dir, exist_ok=True)
    only = args[1] if len(args) > 1 else None
    for z in ZOMBIES:
        if only and only not in z["name"]:
            continue
        print(z["name"])
        make_zombie(z, tmp_dir, preview)
    print("готово")


if __name__ == "__main__":
    main()
