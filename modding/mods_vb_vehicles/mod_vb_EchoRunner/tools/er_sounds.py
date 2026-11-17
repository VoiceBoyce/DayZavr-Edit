# -*- coding: utf-8 -*-
"""Звуки антиграв-модулей Echo Runner (этап 8), моно OGG Vorbis, бесшовные петли по 4 с.

  blender.exe -b --factory-startup --python er_sounds.py -- [папка_для_wav]
  выход: VB_EchoRunner/sounds/vb_er_grav_idle.ogg (гул на холостых), vb_er_grav_fly.ogg (полёт)

Скрипт машины играет обе петли и смешивает их громкостью по скорости роторов (частоту звука скрипт менять не может).
Петля бесшовная: все частоты кратны 1/DUR (целое число периодов), шум — обратное БПФ (периодичен по длине).
Запись OGG — встроенный в Blender модуль aud (как tools/gen_rift_sounds.py в VB_Rift).
"""
import os
import sys
import wave

import aud
import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
OUT_DIR = os.path.normpath(os.path.join(HERE, "..", "VB_EchoRunner", "sounds"))
RATE = 44100
DUR = 4.0
N = int(RATE * DUR)
T = np.arange(N) / RATE


def q(f):
    """Частота, округлённая до целого числа периодов в петле."""
    return round(f * DUR) / DUR


def tone(f, amp=1.0, ph=0.0):
    return amp * np.sin(2 * np.pi * q(f) * T + ph)


def band_noise(lo, hi, seed, tilt=0.0):
    """Периодический шум в полосе lo..hi Гц (обратное БПФ случайного спектра)."""
    rng = np.random.default_rng(seed)
    f = np.fft.rfftfreq(N, 1.0 / RATE)
    mag = ((f >= lo) & (f <= hi)).astype(float)
    mag *= np.where(f > 0, (np.maximum(f, 1.0) / max(lo, 1.0)) ** -tilt, 0.0)
    spec = mag * np.exp(1j * rng.uniform(0, 2 * np.pi, len(f)))
    x = np.fft.irfft(spec, N)
    return x / (np.max(np.abs(x)) + 1e-9)


def am(rate, depth, ph=0.0):
    """Амплитудная модуляция с целым числом циклов в петле."""
    return 1.0 - depth * 0.5 * (1 + np.cos(2 * np.pi * q(rate) * T + ph))


def make_idle():
    # низкий трансформаторный гул + «дыхание» резонатора (вращение ~1.5 об/с) + мягкий шелест поля
    s = tone(46, 1.0) + tone(92, 0.45, 0.7) + tone(138, 0.22, 1.9) + tone(230, 0.08, 0.3)
    s *= am(0.5, 0.18)
    whoosh = band_noise(180, 900, 11, tilt=1.2) * am(1.5, 0.85) * 0.35
    hiss = band_noise(2500, 6000, 12, tilt=0.5) * 0.04
    s = s * 0.55 + whoosh + hiss
    return s / np.max(np.abs(s)) * 0.85


def make_fly():
    # напряжённый гул выше тоном, вой резонатора с вибрато, частое «шуршание» лопаток, треск поля
    base = tone(62, 1.0) + tone(124, 0.55, 0.4) + tone(186, 0.3, 1.1) + tone(310, 0.12, 2.0)
    ph = 2 * np.pi * q(520) * T + 0.6 * np.sin(2 * np.pi * q(2.0) * T)     # вой с вибрато (фазовая модуляция)
    whine = (np.sin(ph) + 0.3 * np.sin(2 * ph + 0.5)) * 0.16 * am(0.75, 0.4)
    blades = band_noise(250, 1600, 21, tilt=1.0) * am(6.0, 0.9) * 0.45
    hiss = band_noise(3000, 8000, 22, tilt=0.4) * 0.07
    rng = np.random.default_rng(23)
    crackle = np.zeros(N)
    for _ in range(26):                                           # редкие щелчки поля
        i0 = rng.integers(0, N - 2000)
        k = np.arange(1200)
        crackle[i0:i0 + 1200] += rng.standard_normal(1200) * np.exp(-k / 90.0) * rng.uniform(0.2, 0.5)
    crackle = np.convolve(crackle, np.ones(3) / 3, mode="same")
    s = base * 0.5 * am(0.5, 0.12) + whine + blades + hiss + crackle * 0.25
    return s / np.max(np.abs(s)) * 0.85


def make_thrust():
    # тяга под газом: низкий рёв поля, плотный «реактивный» шум с пульсацией, звенящий обертон резонатора
    rumble = tone(38, 1.0) + tone(57, 0.6, 0.8) + tone(76, 0.4, 1.7)
    rumble *= am(3.0, 0.35)
    roar = band_noise(120, 2400, 31, tilt=0.8) * am(8.0, 0.5) * 0.9
    hiss = band_noise(2500, 9000, 32, tilt=0.3) * am(4.0, 0.3) * 0.18
    ph = 2 * np.pi * q(330) * T + 1.2 * np.sin(2 * np.pi * q(1.5) * T)
    ring = (np.sin(ph) + 0.4 * np.sin(2 * ph + 0.3)) * 0.12 * am(1.0, 0.5)
    s = rumble * 0.45 + roar + hiss + ring
    return s / np.max(np.abs(s)) * 0.9


def write_wav(path, data):
    pcm = (np.clip(data, -1, 1) * 32767).astype("<i2")
    with wave.open(path, "wb") as w:
        w.setnchannels(1)
        w.setsampwidth(2)
        w.setframerate(RATE)
        w.writeframes(pcm.tobytes())


def main():
    argv = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
    wav_dir = argv[0] if argv else os.path.join(HERE, "_wav")
    os.makedirs(wav_dir, exist_ok=True)
    os.makedirs(OUT_DIR, exist_ok=True)
    for name, fn in (("vb_er_grav_idle", make_idle), ("vb_er_grav_fly", make_fly), ("vb_er_grav_thrust", make_thrust)):
        data = fn().astype(np.float32)
        jump = abs(float(data[0] - data[-1]))
        wav = os.path.join(wav_dir, name + ".wav")
        ogg = os.path.join(OUT_DIR, name + ".ogg")
        write_wav(wav, data)
        aud.Sound(wav).write(ogg, RATE, aud.CHANNELS_MONO, aud.FORMAT_S16, aud.CONTAINER_OGG,
                             aud.CODEC_VORBIS, 96000, 1024)
        rms = float(np.sqrt(np.mean(data ** 2)))
        print("SOUND %-16s %.2f с  rms %.3f  стык %.4f  %d байт" % (name, len(data) / RATE, rms, jump, os.path.getsize(ogg)))


main()
