# -*- coding: utf-8 -*-
"""Общие функции генераторов текстур «Древнего корня»: TGA/PNG, PAA (ImageToPAA), шум и фильтры.

Выделены из генератора зомби Изнанки (mod_vb_Rift/tools/gen_rift_zombies.py) при выделении мода 2026-11-15.
"""
import os
import struct
import subprocess
import zlib

import numpy as np

BS = chr(92)


BS = chr(92)


IMAGE_TO_PAA = os.path.join("D:" + BS, "SteamLibrary", "steamapps", "common", "DayZ Tools",
                            "Bin", "ImageToPAA", "ImageToPAA.exe")


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
