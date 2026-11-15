# -*- coding: utf-8 -*-
"""Янтарь «Древнего корня» (ТЗ 2б.5): текстуры и материалы для предметов VB_AR_Amber и VB_AR_AmberInclusion.

  "G:\\SteamLibrary\\steamapps\\common\\Blender\\5.2\\python\\bin\\python.exe" gen_ar_amber.py <папка_для_png>

Модель — кусок сердцевины (vb_ar_heart_chunk.p3d, скрытые секции crust и veins), здесь только перекраска:
  vb_ar_amber_co.paa       — тело: тёплый янтарь, медовые разводы и светлые пузырьки;
  vb_ar_amber_incl_co.paa  — то же с тёмными вкраплениями (семечко, мошка, осколок руны внутри);
  vb_ar_amber_vein_co.paa  — секция прожилок: светлые золотые потёки;
  vb_ar_amber.rvmat        — глянцевый материал (Super, texGen), слабое тёплое свечение изнутри.
Пишет в ../VB_AncientRoot/data/items.
"""
import os
import sys

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import ar_theme as RT  # noqa: E402
from ar_texlib import write_png, png_to_paa, spectral, smoothstep, blur, to_u8  # noqa: E402

OUT_DIR = os.path.join(RT.mod_dir(), "data", "items")
BS = chr(92)
PREFIX = "VB_AncientRoot" + BS + "data" + BS + "items" + BS
N = 512
SEED = 7331

DARK = np.array([0.42, 0.17, 0.02])     # густой тёмный мёд
MID = np.array([0.80, 0.45, 0.06])      # янтарь
LIGHT = np.array([0.98, 0.72, 0.22])    # светлые разводы
VEIN = np.array([1.00, 0.82, 0.38])


def amber_body(rng, inclusions):
    swirl = spectral(N, rng, beta=2.2)                       # крупные разводы
    fine = spectral(N, rng, beta=1.2)                         # мелкая неоднородность
    t = np.clip(0.5 + 0.55 * swirl + 0.12 * fine, 0, 1)
    col = DARK * (1 - t)[..., None] + MID * t[..., None]
    lt = smoothstep(0.55, 0.95, t)
    col = col * (1 - 0.6 * lt[..., None]) + LIGHT * (0.6 * lt)[..., None]
    # пузырьки: редкие светлые точки разного размера
    yy, xx = np.mgrid[0:N, 0:N]
    bub = np.zeros((N, N), np.float32)
    for _ in range(90):
        cx, cy = rng.uniform(0, N, 2)
        r = rng.uniform(0.8, 3.0)
        d2 = (xx - cx) ** 2 + (yy - cy) ** 2
        bub = np.maximum(bub, np.exp(-d2 / (r * r)))
    col = col * (1 - 0.45 * bub[..., None]) + np.array([1.0, 0.9, 0.6]) * (0.45 * bub)[..., None]
    if inclusions:
        # тёмные включения: несколько пятен неровной формы с мягким краем
        yy, xx = np.mgrid[0:N, 0:N] / N
        mask = np.zeros((N, N), np.float32)
        for _ in range(5):
            cx, cy = rng.uniform(0.15, 0.85, 2)
            r = rng.uniform(0.03, 0.07)
            d = np.sqrt((xx - cx) ** 2 + ((yy - cy) * rng.uniform(0.6, 1.4)) ** 2)
            mask = np.maximum(mask, 1 - smoothstep(r * 0.6, r, d))
        rough = 0.75 + 0.25 * spectral(N, rng, beta=1.0)
        mask = np.clip(mask * rough, 0, 1)
        mask = np.maximum(mask, 0.35 * blur(mask, 6.0))
        col = col * (1 - 0.85 * mask[..., None]) + np.array([0.08, 0.04, 0.01]) * (0.85 * mask)[..., None]
    return col


def vein_tex(rng):
    t = np.clip(0.5 + 0.6 * spectral(N, rng, beta=1.8), 0, 1)
    return MID * (1 - t)[..., None] + VEIN * t[..., None]


RVMAT = """ambient[] = {1,1,1,1};
diffuse[] = {1,1,1,1};
forcedDiffuse[] = {0,0,0,0};
emmisive[] = {0.12,0.12,0.12,1};
specular[] = {0.65,0.55,0.35,1};
specularPower = 90;
PixelShaderID = "Super";
VertexShaderID = "Super";
class TexGen0
{
	uvSource = "tex";
	class uvTransform
	{
		aside[] = {1,0,0};
		up[] = {0,1,0};
		dir[] = {0,0,1};
		pos[] = {0,0,0};
	};
};
class TexGen1: TexGen0{};
class TexGen2: TexGen0{};
class TexGen3: TexGen0{};
class TexGen4: TexGen0{};
class TexGen5: TexGen0{};
class TexGen6: TexGen0{};
class TexGen7: TexGen0{};
class Stage1
{
	texture = "#(argb,8,8,3)color(0.5,0.5,1,1,NOHQ)";
	texGen = "1";
};
class Stage2
{
	texture = "#(argb,8,8,3)color(0.5,0.5,0.5,1,DT)";
	texGen = "2";
};
class Stage3
{
	texture = "#(argb,8,8,3)color(0,0,0,0,MC)";
	texGen = "3";
};
class Stage4
{
	texture = "#(argb,8,8,3)color(1,1,1,1,AS)";
	texGen = "4";
};
class Stage5
{
	texture = "#(argb,8,8,3)color(1,0.85,0.9,1,SMDI)";
	texGen = "5";
};
class Stage6
{
	texture = "#(ai,64,64,1)fresnel(1.5,0.5)";
	texGen = "6";
};
class Stage7
{
	texture = "#(argb,8,8,3)color(0,0,0,1,CO)";
	texGen = "7";
};
class Stage8
{
	texture = "#(argb,8,8,3)color(1,0.55,0.12,1,CO)";
	texGen = "0";
};
"""


def main():
    tmp = sys.argv[1] if len(sys.argv) > 1 else os.path.join(HERE, "_amber")
    os.makedirs(tmp, exist_ok=True)
    os.makedirs(OUT_DIR, exist_ok=True)
    rng = np.random.default_rng(SEED)
    for name, arr in (("vb_ar_amber_co", amber_body(rng, False)),
                      ("vb_ar_amber_incl_co", amber_body(rng, True)),
                      ("vb_ar_amber_vein_co", vein_tex(rng))):
        png = os.path.join(tmp, name + ".png")
        write_png(png, to_u8(arr))
        png_to_paa(png, os.path.join(OUT_DIR, name + ".paa"))
        print("TEX", name)
    with open(os.path.join(OUT_DIR, "vb_ar_amber.rvmat"), "w", encoding="utf-8", newline="\n") as f:
        f.write("// «Древний корень»: янтарь — глянец и слабое тёплое свечение изнутри (gen_ar_amber.py)\n" + RVMAT)
    print("готово:", OUT_DIR)


if __name__ == "__main__":
    main()
