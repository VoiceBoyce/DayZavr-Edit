# -*- coding: utf-8 -*-
"""Звуки Изнанки, моно OGG Vorbis: сердцебиение (3), гул-петля у сердца, скрипы корней (3), шёпот (3), хлопок лопнувшего зомби (2).

  blender.exe -b --factory-startup --python gen_rift_sounds.py -- <папка_для_wav> [имена...]
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


def fftconv(a, b):
    n = len(a) + len(b) - 1
    m = 1 << (n - 1).bit_length()
    return np.fft.irfft(np.fft.rfft(a, m) * np.fft.rfft(b, m), m)[:n]


def band_noise(rng, n, lo, hi, periodic=False):
    """Шум в полосе lo..hi Гц (через FFT; periodic — бесшовно зациклен на длине n)."""
    F = np.fft.rfft(rng.standard_normal(n))
    f = np.fft.rfftfreq(n, 1.0 / RATE)
    edge = 0.15
    mask = np.clip((f - lo * (1 - edge)) / (lo * edge * 2 + 1e-9), 0, 1) * np.clip((hi * (1 + edge) - f) / (hi * edge * 2), 0, 1)
    x = np.fft.irfft(F * mask, n)
    return x / (x.std() + 1e-9)


def smooth_curve(rng, n, knots):
    """Плавная случайная кривая 0..1 по knots опорным точкам."""
    k = rng.uniform(0, 1, knots)
    return np.interp(np.linspace(0, knots - 1, n), np.arange(knots), k)


def room(rng, s, length, decay, mix):
    """Короткий отзвук: свёртка с затухающим шумом."""
    ir_t = np.arange(int(RATE * length)) / RATE
    ir = rng.standard_normal(len(ir_t)) * np.exp(-ir_t / decay)
    ir *= mix / (np.sqrt(np.sum(ir ** 2)) + 1e-9)
    ir[0] = 1.0
    return fftconv(s, ir)[: len(s)]


def normalize(s, peak=0.9):
    return (s / (np.max(np.abs(s)) + 1e-9) * peak).astype(np.float32)


def make_hum():
    """Гул у сердца: бесшовная петля 8 с. Все частоты и модуляции кратны 1/8 Гц — стык не слышен."""
    rng = np.random.default_rng(2001)
    dur = 8.0
    n = int(RATE * dur)
    t = np.arange(n) / RATE
    tw = 2 * np.pi * t
    s = 0.55 * np.sin(tw * 38.0) + 0.45 * np.sin(tw * 38.25 + 1.1)      # биения 0.25 Гц
    s += 0.30 * np.sin(tw * 76.375 + 0.4) + 0.18 * np.sin(tw * 114.5 + 2.0) + 0.08 * np.sin(tw * 152.75 + 0.7)
    s *= 0.75 + 0.25 * np.sin(tw * 0.125)
    breath = band_noise(rng, n, 150, 700, periodic=True) * 0.20
    breath *= (0.5 + 0.5 * np.sin(tw * 0.25 - np.pi / 2)) ** 2             # «вдох» раз в 4 с
    ring = 0.035 * np.sin(tw * 412.5) * (0.5 + 0.5 * np.sin(tw * 0.375))   # тонкий звон, мерцает
    return normalize(s + breath + ring, 0.8)


def make_creak(seed):
    """Скрип корня: срывы трения с плавающей частотой через деревянные резонансы и низкий стон."""
    rng = np.random.default_rng(seed)
    dur = rng.uniform(0.9, 1.6)
    n = int(RATE * dur)
    t = np.arange(n) / RATE
    rate = 22 + 70 * smooth_curve(rng, n, 5)
    ph = np.cumsum(rate) / RATE
    idx = np.nonzero(np.diff(np.floor(ph)) > 0)[0]
    imp = np.zeros(n)
    imp[idx] = rng.uniform(0.4, 1.0, len(idx))
    env = np.sin(np.pi * np.clip(t / dur, 0, 1)) ** 0.7
    imp *= env
    ir_t = np.arange(int(RATE * 0.09)) / RATE
    ir = np.zeros_like(ir_t)
    for f, d, a in ((190, 0.030, 1.0), (430, 0.018, 0.7), (980, 0.010, 0.45), (2150, 0.006, 0.25)):
        f *= rng.uniform(0.85, 1.15)
        ir += a * np.exp(-ir_t / d) * np.sin(2 * np.pi * f * ir_t)
    s = fftconv(imp, ir)[:n]
    s /= np.max(np.abs(s)) + 1e-9
    groan = band_noise(rng, n, 60, 220) * env * 0.25
    s = room(rng, s + groan, 0.4, 0.09, 0.5)
    fade = np.clip((dur - t) / 0.05, 0, 1)
    return normalize(s * fade, 0.85)


# гласные шёпота: форманты F1, F2, F3, Гц
VOWELS = [(700, 1220, 2600), (390, 2300, 3000), (300, 870, 2240), (500, 1000, 2500), (400, 1900, 2550)]


def make_whisper(seed):
    """Неразборчивый шёпот: шум через плавающие форманты гласных, слоги и шипящие, отзвук."""
    rng = np.random.default_rng(seed)
    dur = rng.uniform(1.6, 2.6)
    n = int(RATE * dur)
    win, hop = 1024, 256
    frames = (n - win) // hop + 1
    w = np.hanning(win)
    noise = rng.standard_normal(n)
    f = np.fft.rfftfreq(win, 1.0 / RATE)
    out = np.zeros(n)
    norm = np.zeros(n)
    count = max(3, int(dur * rng.uniform(3.0, 4.2)))
    centers = np.sort(rng.uniform(0.2, dur - 0.25, count))
    vowels = [VOWELS[rng.integers(len(VOWELS))] for _ in range(count)]
    sib = rng.uniform(0, 1, count) < 0.45                # перед слогом шипящий «с/ш»
    sib_band = np.exp(-((f - rng.uniform(4500, 6500)) / 1200.0) ** 2)
    for i in range(frames):
        a = i * hop
        tc = (a + win / 2) / RATE
        d = tc - centers
        g = np.exp(-(d / 0.075) ** 2)
        amp = float(np.sum(g))
        wsum = np.sum(g) + 1e-9
        F = [float(np.sum(g * np.array([v[k] for v in vowels])) / wsum) for k in range(3)]
        spec = np.exp(-((f - F[0]) / 130.0) ** 2) + 0.7 * np.exp(-((f - F[1]) / 170.0) ** 2) \
            + 0.35 * np.exp(-((f - F[2]) / 220.0) ** 2)
        s_amp = float(np.sum(sib * np.exp(-((d + 0.09) / 0.035) ** 2)))
        seg = np.fft.rfft(noise[a:a + win] * w)
        frame = np.fft.irfft(seg * (spec * amp + sib_band * s_amp * 0.35), win) * w
        out[a:a + win] += frame
        norm[a:a + win] += w * w
    out /= np.maximum(norm, 1e-3)
    out = room(rng, out, 0.7, 0.18, 0.6)
    t = np.arange(n) / RATE
    fade = np.clip(t / 0.05, 0, 1) * np.clip((dur - t) / 0.1, 0, 1)
    return normalize(out * fade, 0.8)


def make_burst(seed):
    """Зомби Изнанки лопается: глухой мокрый хлопок, бульканье и шипение выходящих спор."""
    rng = np.random.default_rng(seed)
    dur = 1.5
    n = int(RATE * dur)
    t = np.arange(n) / RATE
    s = thump(t, 0.01, rng.uniform(58, 66), 34, 0.09, 1.0, rng)
    x = np.maximum(t - 0.012, 0)
    s += band_noise(rng, n, 350, 2400) * np.exp(-x / 0.045) * (t >= 0.012) * 0.55
    for _ in range(rng.integers(6, 11)):
        t0 = rng.uniform(0.04, 0.4)
        xb = np.maximum(t - t0, 0)
        fb = rng.uniform(280, 520) * (1 + 1.6 * xb / 0.02)
        pop = np.sin(2 * np.pi * np.cumsum(fb) / RATE) * np.exp(-xb / 0.012) * (t >= t0)
        s += pop * rng.uniform(0.08, 0.2)
    xh = np.maximum(t - 0.05, 0)
    hiss = band_noise(rng, n, 2000, 8000) * (1 - np.exp(-xh / 0.03)) * np.exp(-xh / 0.45) * (t >= 0.05)
    s += hiss * 0.22
    s = room(rng, s, 0.3, 0.06, 0.35)
    return normalize(s, 0.9)


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
    jobs = [("vb_rift_heartbeat_%d" % i, make_beat, 1000 + i) for i in range(1, 4)]
    jobs += [("vb_rift_hum", lambda _s: make_hum(), 0)]
    jobs += [("vb_rift_creak_%d" % i, make_creak, 3000 + i) for i in range(1, 4)]
    jobs += [("vb_rift_whisper_%d" % i, make_whisper, 4000 + i) for i in range(1, 4)]
    jobs += [("vb_rift_burst_%d" % i, make_burst, 5000 + i) for i in range(1, 3)]
    only = [a for a in argv[1:]]          # имена через пробел: собрать только их
    for name, fn, seed in jobs:
        if only and not any(o in name for o in only):
            continue
        data = fn(seed)
        wav = os.path.join(wav_dir, name + ".wav")
        write_wav(wav, data)
        ogg = os.path.join(OUT_DIR, name + ".ogg")
        aud.Sound(wav).write(ogg, RATE, aud.CHANNELS_MONO, aud.FORMAT_S16, aud.CONTAINER_OGG,
                             aud.CODEC_VORBIS, 96000, 1024)
        rms = float(np.sqrt(np.mean(data ** 2)))
        print("SOUND %-22s %.2f с  пик %.2f  rms %.3f  %d байт" % (name, len(data) / RATE, float(np.max(np.abs(data))), rms, os.path.getsize(ogg)))


main()
