# -*- coding: utf-8 -*-
"""Звуки Изнанки: низкое глухое «тук-тук» сердца (3 варианта), моно OGG Vorbis.

  blender.exe -b --factory-startup --python gen_rift_sounds.py -- <папка_для_wav>
Синтез в numpy, запись OGG встроенным в Blender модулем aud. Итог: VB_Rift/sounds/vb_rift_heartbeat_N.ogg.
Первый удар начинается через FIRST_AT секунд от начала файла (скрипт запускает звук с этим упреждением).
"""
import os
import sys
import wave

import aud
import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
OUT_DIR = os.path.normpath(os.path.join(HERE, "..", "VB_Rift", "sounds"))
RATE = 44100
FIRST_AT = 0.03       # с: где в файле первый удар (держать в согласии с VB_RiftPulse.SOUND_LEAD)
SECOND_AT = 0.48      # с: второй удар, как BEAT_SECOND - BEAT_FIRST в скрипте (0.6 - 0.15 = 0.45) + небольшой разброс


def thump(t, t0, f0, f1, decay, amp, rng):
    """Один глухой удар: низкий тон с падением частоты, призвук тела и мягкий щелчок."""
    x = t - t0
    on = x >= 0
    x = np.where(on, x, 0.0)
    env = np.where(on, (1 - np.exp(-x / 0.006)) * np.exp(-x / decay), 0.0)
    freq = f1 + (f0 - f1) * np.exp(-x / 0.05)
    phase = 2 * np.pi * np.cumsum(freq) / RATE
    body = np.sin(phase) + 0.35 * np.sin(2.02 * phase + 0.6)
    noise = rng.standard_normal(len(t))
    k = int(RATE * 0.004)
    noise = np.convolve(noise, np.ones(k) / k, mode="same")          # грубый ФНЧ: глухой «мясной» шорох
    click = noise * np.where(on, np.exp(-x / 0.02), 0.0) * 0.6
    return amp * env * (body + click)


def make_beat(seed):
    rng = np.random.default_rng(seed)
    dur = 1.6
    t = np.arange(int(RATE * dur)) / RATE
    j = rng.uniform(-0.02, 0.02)
    s = thump(t, FIRST_AT, rng.uniform(70, 80), rng.uniform(40, 46), 0.16, 1.0, rng)
    s += thump(t, SECOND_AT + j, rng.uniform(62, 72), rng.uniform(38, 42), 0.2, rng.uniform(0.6, 0.72), rng)
    # тихий подземный гул-хвост
    rumble = rng.standard_normal(len(t))
    k = int(RATE * 0.02)
    rumble = np.convolve(rumble, np.ones(k) / k, mode="same")
    rumble *= np.exp(-np.maximum(t - FIRST_AT, 0) / 0.5) * (t >= FIRST_AT) * 0.5
    s += rumble
    # короткий «подвальный» отзвук
    ir_t = np.arange(int(RATE * 0.35)) / RATE
    ir = rng.standard_normal(len(ir_t)) * np.exp(-ir_t / 0.08) * 0.04
    ir[0] = 1.0
    s = np.convolve(s, ir)[: len(t)]
    fade = np.clip((dur - t) / 0.15, 0, 1)
    s *= fade
    s /= np.max(np.abs(s)) + 1e-9
    return (s * 0.92).astype(np.float32)


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
    for i in range(1, 4):
        data = make_beat(1000 + i)
        wav = os.path.join(wav_dir, "vb_rift_heartbeat_%d.wav" % i)
        write_wav(wav, data)
        ogg = os.path.join(OUT_DIR, "vb_rift_heartbeat_%d.ogg" % i)
        aud.Sound(wav).write(ogg, RATE, aud.CHANNELS_MONO, aud.FORMAT_S16, aud.CONTAINER_OGG,
                             aud.CODEC_VORBIS, 96000, 1024)
        print("SOUND", ogg, os.path.getsize(ogg))


main()
