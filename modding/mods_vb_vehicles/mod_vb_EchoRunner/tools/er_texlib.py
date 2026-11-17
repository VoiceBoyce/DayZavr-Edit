# -*- coding: utf-8 -*-
r"""Генерация текстур Echo Runner (numpy; запись PNG через bpy — запускать внутри Blender).

  * бесшовный шум: спектральный синтез (FFT) — периодичен по построению, плитка без швов;
  * нормали из карты высот в соглашении DayZ _nohq (DirectX, Y вниз): n = (-dh/dcol, -dh/drow, 1)·k;
  * _smdi: R = 1, G = блик, B = глянец;
  * PNG -> PAA: ImageToPAA (формат по суффиксу имени: _co, _ca, _nohq, _smdi);
  * rvmat — шейдер Super по образцу ванильного civiliansedan.rvmat (стадии 1–7).
Массивы — (строки, столбцы, каналы), строка 0 — верх картинки, значения 0..1 (цвет — sRGB).
"""
import os
import subprocess

import numpy as np

IMAGE_TO_PAA = r"D:\SteamLibrary\steamapps\common\DayZ Tools\Bin\ImageToPAA\ImageToPAA.exe"


# ---------------------------------------------------------------- шум
def fbm(n, beta=2.0, seed=0, fmin=1.0, fmax=None):
    """Бесшовный фрактальный шум n×n, 0..1. beta — наклон спектра (больше — крупнее пятна)."""
    rng = np.random.default_rng(seed)
    fy = np.fft.fftfreq(n)[:, None] * n
    fx = np.fft.rfftfreq(n)[None, :] * n
    f = np.sqrt(fx * fx + fy * fy)
    amp = np.where(f < fmin, 0.0, 1.0 / np.maximum(f, 1e-6) ** (beta / 2.0))
    if fmax:
        amp = np.where(f > fmax, 0.0, amp)
    spec = amp * (rng.normal(size=f.shape) + 1j * rng.normal(size=f.shape))
    a = np.fft.irfft2(spec, s=(n, n))
    a -= a.min()
    return a / max(a.max(), 1e-9)


def smooth(a, lo, hi):
    t = np.clip((a - lo) / max(hi - lo, 1e-9), 0, 1)
    return t * t * (3 - 2 * t)


def blur(a, r):
    """Периодическое размытие (гаусс через FFT)."""
    n0, n1 = a.shape[:2]
    fy = np.fft.fftfreq(n0)[:, None]
    fx = np.fft.fftfreq(n1)[None, :]
    g = np.exp(-2 * (np.pi * r) ** 2 * (fx * fx + fy * fy))
    if a.ndim == 2:
        return np.real(np.fft.ifft2(np.fft.fft2(a) * g))
    return np.stack([np.real(np.fft.ifft2(np.fft.fft2(a[..., k]) * g)) for k in range(a.shape[2])], -1)


def scratches(n, count, seed, length=(0.03, 0.18), width=1.2, angle=None):
    """Карта царапин 0..1 (бесшовная: рисуем с переносом через край)."""
    rng = np.random.default_rng(seed)
    m = np.zeros((n, n), np.float32)
    for _ in range(count):
        x0, y0 = rng.uniform(0, n, 2)
        a = rng.uniform(0, np.pi) if angle is None else angle + rng.normal(0, 0.15)
        L = rng.uniform(*length) * n
        k = int(L)
        t = np.linspace(0, 1, max(k, 2))
        xs = (x0 + np.cos(a) * L * t + np.sin(t * 6 + x0) * 2).astype(int) % n
        ys = (y0 + np.sin(a) * L * t).astype(int) % n
        inten = rng.uniform(0.4, 1.0) * np.sin(np.pi * t) ** 0.5
        m[ys, xs] = np.maximum(m[ys, xs], inten)
    if width > 1:
        m = np.clip(blur(m, width * 0.5) * width * 1.6, 0, 1)
    return m


def dots(n, count, seed, rmin=1.5, rmax=4.0):
    """Россыпь круглых точек (питтинг, брызги), высота 0..1, бесшовно."""
    rng = np.random.default_rng(seed)
    m = np.zeros((n, n), np.float32)
    yy, xx = np.mgrid[-8:9, -8:9]
    for _ in range(count):
        x, y = rng.integers(0, n, 2)
        r = rng.uniform(rmin, rmax)
        d = np.sqrt(xx * xx + yy * yy)
        bump = np.clip(1 - d / r, 0, 1)
        m[np.ix_((y + yy[:, 0]) % n, (x + xx[0, :]) % n)] = np.maximum(m[np.ix_((y + yy[:, 0]) % n, (x + xx[0, :]) % n)], bump)
    return m


def lerp(a, b, t):
    t = np.asarray(t)[..., None] if np.ndim(t) == 2 and np.ndim(a) == 3 else t
    return a + (b - a) * t


def col(rgb, n):
    return np.ones((n, n, 3), np.float32) * np.array(rgb, np.float32)


# ---------------------------------------------------------------- карты
def normal_from_height(h, strength=4.0):
    """_nohq (DirectX): R = x, G = y вниз, B = z. h — бесшовная высота 0..1."""
    dx = (np.roll(h, -1, 1) - np.roll(h, 1, 1)) * 0.5
    dy = (np.roll(h, -1, 0) - np.roll(h, 1, 0)) * 0.5
    nx, ny, nz = -dx * strength, -dy * strength, np.ones_like(h)
    L = np.sqrt(nx * nx + ny * ny + nz * nz)
    return np.stack([nx / L * 0.5 + 0.5, ny / L * 0.5 + 0.5, nz / L * 0.5 + 0.5], -1)


def normal_from_height_np(h, strength=4.0, mask=None):
    """То же без переноса через край (для развёрток кузова; mask — где есть геометрия)."""
    dx = np.zeros_like(h)
    dy = np.zeros_like(h)
    dx[:, 1:-1] = (h[:, 2:] - h[:, :-2]) * 0.5
    dy[1:-1, :] = (h[2:, :] - h[:-2, :]) * 0.5
    if mask is not None:
        dx *= mask
        dy *= mask
    nx, ny, nz = -dx * strength, -dy * strength, np.ones_like(h)
    L = np.sqrt(nx * nx + ny * ny + nz * nz)
    return np.stack([nx / L * 0.5 + 0.5, ny / L * 0.5 + 0.5, nz / L * 0.5 + 0.5], -1)


def smdi(spec, gloss):
    return np.stack([np.ones_like(spec), np.clip(spec, 0, 1), np.clip(gloss, 0, 1)], -1)


# ---------------------------------------------------------------- запись
def save_png(arr, path, alpha=None, noncolor=False):
    """arr (H, W, 3) 0..1, строка 0 — верх. Запись через bpy (в Blender нет PIL)."""
    import bpy
    h, w = arr.shape[:2]
    a = np.ones((h, w), np.float32) if alpha is None else alpha.astype(np.float32)
    rgba = np.concatenate([np.clip(arr, 0, 1).astype(np.float32), a[..., None]], -1)[::-1]
    im = bpy.data.images.new(os.path.basename(path), w, h, alpha=True)
    if noncolor:
        im.colorspace_settings.name = "Non-Color"
    im.pixels.foreach_set(rgba.ravel())
    im.filepath_raw = path
    im.file_format = "PNG"
    im.save()
    bpy.data.images.remove(im)


def to_paa(png, paa):
    r = subprocess.run([IMAGE_TO_PAA, png, paa], capture_output=True, text=True)
    if not os.path.exists(paa):
        raise RuntimeError("ImageToPAA: %s %s" % (png, r.stdout[-300:] + r.stderr[-300:]))


STAGE = """class Stage{i}
{{
	texture = "{tex}";
	uvSource = "{uv}";
	class uvTransform
	{{
		aside[] = {{1,0,0}};
		up[] = {{0,1,0}};
		dir[] = {{0,0,1}};
		pos[] = {{0,0,0}};
	}};
}};
"""


def write_rvmat(path, nohq, smdi_path, specular=0.6, power=60, emissive=(0, 0, 0, 1), diffuse=(1, 1, 1, 1),
                fresnel=(0.9, 0.5), env=r"dz\data\data\env_co.paa"):
    s = ["ambient[] = {%g,%g,%g,%g};" % tuple(diffuse),
         "diffuse[] = {%g,%g,%g,%g};" % tuple(diffuse),
         "forcedDiffuse[] = {0,0,0,0};",
         "emmisive[] = {%g,%g,%g,%g};" % tuple(emissive),
         "specular[] = {%g,%g,%g,1};" % (specular, specular, specular),
         "specularPower = %g;" % power,
         'PixelShaderID = "Super";',
         'VertexShaderID = "Super";']
    st = [nohq, "#(argb,8,8,3)color(0.5,0.5,0.5,1,DT)", "#(argb,8,8,3)color(0,0,0,0,MC)",
          "#(argb,8,8,3)color(1,1,1,1,AS)", smdi_path, "#(ai,128,1,1)fresnel(%g,%g)" % fresnel, env]
    body = "\n".join(s) + "\n" + "".join(STAGE.format(i=i + 1, tex=t, uv="tex") for i, t in enumerate(st))
    with open(path, "w", encoding="ascii", newline="\r\n") as f:
        f.write(body)
