# -*- coding: utf-8 -*-
"""Ауры боссов VB_AnomaliesBosses: частицы палитр, их регистрация и классы света (ТЗ 3 п.3, 2.7, 8 п.4).

Пишет (всё целиком, руками не править):
  VB_AnomaliesBosses/p/vb_ab_aura_<палитра>.ptc            аура живого босса (петля, крепится к кости spine2);
  VB_AnomaliesBosses/p/vb_ab_aura_<палитра>_dissolve.ptc   распад тела после разделки (один раз, ~2,5 с);
  scripts/3_Game/VB_AB/VB_AB_AuraParticlesGen.c             modded ParticleList: регистрация 14 частиц;
  scripts/4_World/VB_AB/VB_AB_AuraLightsGen.c               VB_AB_Light_AuraBase и VB_AB_Light_Aura<Палитра>.
Логика (что, когда и к чему крепится) — руками в scripts/4_World/VB_AB/VB_AB_Aura.c.

Новых текстур и материалов нет: только ванильные материалы частиц (graphics/particles/materials/...)
и уже перенесённая снежинка аномалии Снег (VB_AB/p/m/snow/snowflake.emat). Поэтому вес — только .ptc.

Формы излучателей — шар (Sphere) и разлёт во все стороны: оси кости, к которой крепится аура, не важны.
Цвета держим строгими каналами (E45): лишний канал = 0, иначе ночью ярко-эмиссивное выжигается в белый.
Целочисленные ключи .ptc пишутся целыми и сверяются с ванилью на P: (E47: дробное значение обрывает разбор).

Запуск: python ab_aura.py           (любой Python 3; P: нужен только для проверки ключей и ванильных материалов)
"""
import os
import re
import sys
from collections import defaultdict

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, ".."))
MOD = os.path.join(ROOT, "VB_AnomaliesBosses")
VANILLA_PTC = os.path.join("P:" + os.sep, "graphics", "particles")
HEAD = "СГЕНЕРИРОВАНО tools/ab_aura.py — руками не править"

# палитры скинов (ТЗ 4.6); тот же список, что у боссов в tools/ab_skins.py (проверяется в main)
PALETTES = ["fire", "storm", "ice", "gravity", "acid", "psi", "rad"]

# ------------------------------------------------------------------ материалы (только готовые)
MAT = {
    "glow": "{CB7AF4FD8ACBFDFC}Graphics/Particles/materials/glow/glow1.emat",          # мягкая точка
    "spark": "{009C2EBAACD2D72A}Graphics/Particles/materials/fire/sparks1.emat",       # яркая искра
    "electric": "{00D2D6B87126B8FA}Graphics/Particles/materials/flash/electric.emat",  # вспышка-разряд
    "dust": "{68A8BAEC9498D2CB}Graphics/Particles/materials/smoke/smoke_dust_01_em3.emat",  # светящаяся дымка
    "mist": "{D29B53088A5A3911}Graphics/Particles/materials/smoke/smoke_dust_01_W.emat",    # белый туман
    "ring": "{3E3B5AA16932182E}Graphics/Particles/materials/enviroment/bubbles_01_em.emat",  # кольцо
    "flake": "{E93A41A920705F9C}VB_AB/p/m/snow/snowflake.emat",                         # снежинка (из Снега)
}

# ------------------------------------------------------------------ свет (VB_AB_AuraLightsGen.c)
# цвет — строгими каналами (E45); radius/bright — для босса масштаба 1 (у Энта радиус × vbABScale в скрипте);
# flicker — (амплитуда 0..1, скорость), 0 — ровный свет; haze — марево над огнём.
LIGHT = {
    "fire": dict(rgb=(1.0, 0.32, 0.0), radius=8, bright=1.3, flicker=(0.35, 3.0), haze=True),
    "storm": dict(rgb=(0.0, 0.45, 1.0), radius=8, bright=1.3, flicker=(0.6, 9.0)),
    "ice": dict(rgb=(0.4, 0.75, 1.0), radius=8, bright=1.0, flicker=(0, 0)),
    "gravity": dict(rgb=(0.55, 0.0, 1.0), radius=8, bright=1.2, flicker=(0.3, 1.2)),
    "acid": dict(rgb=(0.25, 1.0, 0.0), radius=8, bright=1.0, flicker=(0.2, 2.0)),
    "psi": dict(rgb=(0.8, 0.75, 1.0), radius=8, bright=0.9, flicker=(0.25, 0.8)),
    "rad": dict(rgb=(0.65, 1.0, 0.0), radius=8, bright=1.0, flicker=(0.3, 2.5)),
}
DISSOLVE_LIGHT_MULT = 2.0     # вспышка света при распаде: яркость × (свет живого босса)
DISSOLVE_LIGHT_SEC = 2.5      # и гаснет за столько секунд

# ------------------------------------------------------------------ формат .ptc
# порядок ключей как в ванильных файлах
ORDER = ["ShapeType", "ShapeSize", "ConeAngle", "MaxNum", "BirthRate", "BirthRateRND", "BirthRateVel", "Offset",
         "LocalTransform", "Material", "CenterY", "SizeMultiplier", "SizeRND", "BillboardingType", "RandomRotDir",
         "RotMultiplier", "StretchMultiplier", "VelAngle", "RandomAngle", "Velocity", "VelocityRND", "AirResistance",
         "AirResistanceRND", "GravityMultiply", "GravityMultiplyRND", "Spring", "Wind", "AnimFPS", "RandomFrame",
         "LifeTime", "LifeTimeRND", "Color", "Alpha", "Size", "EffectTime", "Repeat", "BRateMast"]
CURVES = {"Color", "Alpha", "Size", "BRateMast"}
# целые в ванили (E47); ConeAngle — вектор из целых
INT_KEYS = {"MaxNum", "Repeat", "VelAngle", "RandomAngle", "Wind", "RandomRotDir", "RandomFrame", "LocalTransform",
            "AnimFPS", "RotMultiplier"}
INT_VEC_KEYS = {"ConeAngle"}

BASE = dict(ShapeType="Sphere", ConeAngle=(360, 360, 0), Offset=(0, 0, 0), BillboardingType="Full", RandomAngle=1,
            AirResistance=0, GravityMultiply=0, Wind=0)


def num(v):
    """Число без экспоненты и лишних нулей."""
    if isinstance(v, bool):
        v = int(v)
    if isinstance(v, int):
        return str(v)
    s = ("%.4f" % v).rstrip("0").rstrip(".")
    return "0" if s in ("-0", "") else s


def val(key, v):
    if key in INT_KEYS:
        if float(v) != int(v):
            raise SystemExit("E47: ключ %s должен быть целым, а не %r" % (key, v))
        return str(int(v))
    if key in INT_VEC_KEYS:
        if any(float(x) != int(x) for x in v):
            raise SystemExit("E47: ключ %s должен быть из целых, а не %r" % (key, v))
        return " ".join(str(int(x)) for x in v)
    if key == "Material":
        return '"%s"' % MAT[v]
    if isinstance(v, str):
        return v
    if isinstance(v, (tuple, list)):
        return " ".join(num(x) for x in v)
    return num(v)


def sphere(r):
    return (r, r, r)


def emitter(name, **kw):
    e = dict(BASE)
    e.update(kw)
    unknown = [k for k in e if k not in ORDER]
    if unknown:
        raise SystemExit("неизвестные ключи %s в %s" % (unknown, name))
    L = ["  EmitorDef %s {" % name]
    for k in ORDER:
        if k not in e or e[k] is None:
            continue
        v = e[k]
        if k in CURVES:
            L.append("   %s {" % k)
            L.append("    " + " ".join(num(x) for x in v))
            L.append("   }")
        else:
            L.append("   %s %s" % (k, val(k, v)))
    L.append("  }")
    return "\n".join(L)


def effect(emitters):
    return "EffectDef {\n Emitors {\n" + "\n".join(emitters) + "\n }\n}\n"


def col(*stops):
    """Кривая цвета: (t, (r, g, b)), ... -> плоский список."""
    out = []
    for t, rgb in stops:
        out += [t] + list(rgb)
    return out


# ------------------------------------------------------------------ аура живого босса (Repeat 1)
# Мировые частицы (LocalTransform 0) остаются шлейфом, когда босс бежит; «прилипшие» (1) едут на теле.
LOOP = dict(EffectTime=5, Repeat=1)
# распад: излучение 1,8 с с нарастанием к ~1,5 с (тело удаляется через 1,5 с после разделки), частицы живут ещё ~1 с
BURST = dict(EffectTime=1.8, Repeat=0, BRateMast=[0, 0.35, 0.6, 0.7, 0.85, 1, 1, 0.3])
BODY = dict(ShapeType="Box", ShapeSize=(1.3, 0.3, 1.3))   # лежащее тело (в мире, без поворота)


def aura_fire():
    return [
        emitter("embers", ShapeSize=sphere(0.4), MaxNum=40, BirthRate=10, BirthRateRND=6, BirthRateVel=3,
                LocalTransform=0, Material="spark", SizeMultiplier=0.03, SizeRND=0.02, Velocity=0.3,
                VelocityRND=0.3, AirResistance=0.8, AirResistanceRND=0.4, GravityMultiply=-0.12,
                GravityMultiplyRND=0.06, Wind=1, LifeTime=1.4, LifeTimeRND=0.8,
                Color=col((0, (1, 0.55, 0)), (0.4, (1, 0.28, 0)), (1, (0.7, 0.06, 0))),
                Alpha=[0, 0, 0.08, 1, 0.3, 0.6, 0.45, 1, 0.7, 0.5, 1, 0], Size=[0, 1, 1, 0.4], **LOOP),
        emitter("smolder", ShapeSize=sphere(0.3), MaxNum=8, BirthRate=4, BirthRateRND=2, LocalTransform=1,
                Material="glow", SizeMultiplier=0.3, SizeRND=0.12, Velocity=0, LifeTime=0.9, LifeTimeRND=0.5,
                Color=col((0, (1, 0.3, 0)), (1, (0.8, 0.1, 0))), Alpha=[0, 0, 0.3, 0.3, 1, 0], **LOOP),
    ]


def aura_storm():
    return [
        emitter("sparks", ShapeSize=sphere(0.4), MaxNum=30, BirthRate=6, BirthRateRND=12, BirthRateVel=2,
                LocalTransform=0, Material="spark", SizeMultiplier=0.015, SizeRND=0.01, StretchMultiplier=1.02,
                VelAngle=1, Velocity=1.6, VelocityRND=1.2, AirResistance=3, AirResistanceRND=1, GravityMultiply=0.3,
                LifeTime=0.3, LifeTimeRND=0.25,
                Color=col((0, (0, 0.75, 1)), (0.4, (0, 0.45, 1)), (1, (0, 0.2, 0.9))),
                Alpha=[0, 1, 0.7, 0.8, 1, 0], Size=[0, 1, 1, 0.5], **LOOP),
        emitter("arcs", ShapeSize=sphere(0.35), MaxNum=3, BirthRate=1.5, BirthRateRND=3, LocalTransform=1,
                Material="electric", SizeMultiplier=0.28, SizeRND=0.12, Velocity=0, LifeTime=0.09, LifeTimeRND=0.06,
                Color=col((0, (0, 0.7, 1)), (1, (0, 0.45, 1))), Alpha=[0, 1, 1, 0], **LOOP),
        emitter("glint", ShapeSize=sphere(0.3), MaxNum=4, BirthRate=3, BirthRateRND=2, LocalTransform=1,
                Material="glow", SizeMultiplier=0.3, SizeRND=0.1, Velocity=0, LifeTime=0.2, LifeTimeRND=0.15,
                Color=col((0, (0, 0.5, 1)), (1, (0, 0.3, 1))), Alpha=[0, 0.3, 1, 0], **LOOP),
    ]


def aura_ice():
    return [
        emitter("flakes", ShapeSize=sphere(0.45), MaxNum=30, BirthRate=7, BirthRateRND=3, BirthRateVel=2,
                LocalTransform=0, Material="flake", SizeMultiplier=0.035, SizeRND=0.015, RandomRotDir=1,
                RotMultiplier=40, Velocity=0.2, VelocityRND=0.15, AirResistance=1.2, GravityMultiply=0.04,
                GravityMultiplyRND=0.03, Wind=1, AnimFPS=0, RandomFrame=1, LifeTime=1.8, LifeTimeRND=0.8,
                Color=col((0, (0.6, 0.85, 1)), (1, (0.4, 0.7, 1))), Alpha=[0, 0, 0.15, 0.9, 0.7, 0.7, 1, 0], **LOOP),
        emitter("frost", ShapeSize=sphere(0.35), MaxNum=10, BirthRate=5, BirthRateRND=3, LocalTransform=1,
                Material="glow", SizeMultiplier=0.06, SizeRND=0.03, Velocity=0, LifeTime=0.4, LifeTimeRND=0.3,
                Color=col((0, (0.55, 0.85, 1)), (1, (0.35, 0.7, 1))), Alpha=[0, 0, 0.4, 1, 1, 0], **LOOP),
        emitter("mist", ShapeSize=sphere(0.45), MaxNum=6, BirthRate=1.2, BirthRateRND=0.6, LocalTransform=0,
                Material="mist", SizeMultiplier=0.7, SizeRND=0.3, RandomRotDir=1, RotMultiplier=10, Velocity=0.05,
                AirResistance=1, GravityMultiply=0.03, Wind=1, LifeTime=2.5, LifeTimeRND=1,
                Color=col((0, (0.7, 0.85, 1)), (1, (0.6, 0.8, 1))), Alpha=[0, 0, 0.3, 0.22, 1, 0],
                Size=[0, 0.6, 1, 1.3], **LOOP),
    ]


def aura_gravity():
    # «втягивание по спирали»: случайная начальная скорость + Spring к центру излучателя + сопротивление воздуха —
    # частица идёт по сужающейся петле; излучатель «прилипший» (LocalTransform 1), центр едет с боссом
    return [
        emitter("dust", ShapeSize=sphere(1.1), MaxNum=24, BirthRate=7, BirthRateRND=3, LocalTransform=1,
                Material="dust", SizeMultiplier=0.12, SizeRND=0.05, RandomRotDir=1, RotMultiplier=90,
                Velocity=0.6, VelocityRND=0.3, AirResistance=0.35, Spring=1.2, LifeTime=2.2, LifeTimeRND=0.8,
                Color=col((0, (0.45, 0, 0.9)), (1, (0.3, 0, 0.75))), Alpha=[0, 0, 0.2, 0.45, 0.8, 0.35, 1, 0],
                Size=[0, 1.2, 1, 0.3], **LOOP),
        emitter("motes", ShapeSize=sphere(1.3), MaxNum=30, BirthRate=12, BirthRateRND=4, LocalTransform=1,
                Material="spark", SizeMultiplier=0.018, SizeRND=0.008, Velocity=0.8, VelocityRND=0.4,
                AirResistance=0.5, Spring=2, LifeTime=1.6, LifeTimeRND=0.6,
                Color=col((0, (0.7, 0, 1)), (1, (0.4, 0, 1))), Alpha=[0, 0, 0.15, 1, 0.85, 0.8, 1, 0],
                Size=[0, 1, 1, 0.3], **LOOP),
        emitter("ring", ShapeType="Point", ShapeSize=(0, 0, 0), ConeAngle=(0, 0, 0), MaxNum=2, BirthRate=0.6,
                LocalTransform=1, Material="ring", SizeMultiplier=1.4, RandomAngle=1, Velocity=0, LifeTime=1.6,
                Color=col((0, (0.5, 0, 1)), (1, (0.3, 0, 0.9))), Alpha=[0, 0, 0.2, 0.35, 1, 0],
                Size=[0, 1, 1, 0.1], **LOOP),
    ]


def aura_acid():
    return [
        emitter("drops", ShapeSize=sphere(0.4), MaxNum=24, BirthRate=6, BirthRateRND=4, BirthRateVel=2,
                LocalTransform=0, Material="spark", SizeMultiplier=0.022, SizeRND=0.01, StretchMultiplier=1.015,
                VelAngle=1, Velocity=0.15, VelocityRND=0.15, AirResistance=0.3, GravityMultiply=0.7,
                GravityMultiplyRND=0.2, LifeTime=0.9, LifeTimeRND=0.4,
                Color=col((0, (0.35, 1, 0)), (1, (0.15, 0.8, 0))), Alpha=[0, 0, 0.1, 1, 0.8, 0.9, 1, 0],
                Size=[0, 1, 1, 0.7], **LOOP),
        emitter("vapor", ShapeSize=sphere(0.4), MaxNum=10, BirthRate=2.5, BirthRateRND=1, LocalTransform=0,
                Material="dust", SizeMultiplier=0.3, SizeRND=0.12, RandomRotDir=1, RotMultiplier=15, Velocity=0.15,
                AirResistance=1, GravityMultiply=-0.04, Wind=1, LifeTime=2, LifeTimeRND=0.8,
                Color=col((0, (0.25, 0.9, 0)), (1, (0.1, 0.5, 0))), Alpha=[0, 0, 0.25, 0.3, 1, 0],
                Size=[0, 0.5, 1, 1.4], **LOOP),
        emitter("bubbles", ShapeSize=sphere(0.35), MaxNum=6, BirthRate=3, BirthRateRND=2, LocalTransform=1,
                Material="ring", SizeMultiplier=0.05, SizeRND=0.02, Velocity=0, LifeTime=0.5, LifeTimeRND=0.3,
                Color=col((0, (0.35, 1, 0)), (1, (0.2, 0.9, 0))), Alpha=[0, 0.8, 1, 0], Size=[0, 0.3, 1, 1],
                **LOOP),
    ]


def aura_psi():
    pearl = col((0, (0.9, 0.8, 1)), (0.5, (0.7, 0.9, 1)), (1, (1, 0.85, 1)))
    return [
        emitter("haze", ShapeSize=sphere(0.4), MaxNum=8, BirthRate=2.5, BirthRateRND=1, LocalTransform=1,
                Material="glow", SizeMultiplier=0.55, SizeRND=0.2, Velocity=0.05, LifeTime=2.2, LifeTimeRND=0.8,
                Color=col((0, (0.8, 0.75, 1)), (0.5, (0.65, 0.85, 1)), (1, (0.9, 0.7, 1))),
                Alpha=[0, 0, 0.35, 0.16, 0.7, 0.12, 1, 0], Size=[0, 0.7, 1, 1.2], **LOOP),
        emitter("motes", ShapeSize=sphere(0.5), MaxNum=24, BirthRate=6, BirthRateRND=3, BirthRateVel=2,
                LocalTransform=0, Material="glow", SizeMultiplier=0.035, SizeRND=0.015, Velocity=0.15,
                VelocityRND=0.1, AirResistance=0.5, GravityMultiply=-0.03, LifeTime=2, LifeTimeRND=1,
                Color=pearl, Alpha=[0, 0, 0.2, 0.9, 0.7, 0.6, 1, 0], **LOOP),
    ]


def aura_rad():
    return [
        emitter("sparks", ShapeSize=sphere(0.4), MaxNum=30, BirthRate=8, BirthRateRND=6, BirthRateVel=3,
                LocalTransform=0, Material="spark", SizeMultiplier=0.025, SizeRND=0.015, Velocity=0.35,
                VelocityRND=0.3, AirResistance=0.8, GravityMultiply=-0.06, GravityMultiplyRND=0.04, Wind=1,
                LifeTime=1.3, LifeTimeRND=0.7,
                Color=col((0, (0.75, 1, 0)), (0.5, (0.6, 1, 0)), (1, (0.35, 0.8, 0))),
                Alpha=[0, 0, 0.1, 1, 0.25, 0.3, 0.4, 1, 0.55, 0.4, 0.7, 0.9, 1, 0], **LOOP),
        emitter("sores", ShapeSize=sphere(0.3), MaxNum=8, BirthRate=4, BirthRateRND=2, LocalTransform=1,
                Material="glow", SizeMultiplier=0.2, SizeRND=0.08, Velocity=0, LifeTime=1, LifeTimeRND=0.5,
                Color=col((0, (0.65, 1, 0)), (1, (0.4, 0.85, 0))), Alpha=[0, 0, 0.4, 0.35, 1, 0], **LOOP),
    ]


# ------------------------------------------------------------------ распад тела (Repeat 0, PlayInWorld)
def flash(rgb):
    """Мягкий шар света над телом: разгорается к моменту исчезновения тела и гаснет."""
    return emitter("flash", ShapeType="Point", ShapeSize=(0, 0, 0), ConeAngle=(0, 0, 0), MaxNum=1, BirthRate=999,
                   LocalTransform=0, Material="glow", SizeMultiplier=2.2, Velocity=0, LifeTime=2.2,
                   Color=col((0, rgb), (1, rgb)), Alpha=[0, 0, 0.55, 0.45, 0.75, 0.5, 1, 0], Size=[0, 0.5, 0.7, 1.2, 1, 1.5],
                   EffectTime=0.1, Repeat=0)


def puff(rgb_from, rgb_to, mat="dust", alpha=0.4, rise=-0.03):
    return emitter("puff", MaxNum=14, BirthRate=8, LocalTransform=0, Material=mat, SizeMultiplier=0.8, SizeRND=0.3,
                   RandomRotDir=1, RotMultiplier=15, Velocity=0.3, VelocityRND=0.2, AirResistance=1,
                   GravityMultiply=rise, Wind=1, LifeTime=2.2, LifeTimeRND=0.6, Color=col((0, rgb_from), (1, rgb_to)),
                   Alpha=[0, 0, 0.25, alpha, 1, 0], Size=[0, 0.4, 1, 1.5], **dict(BODY, **BURST))


def crumble(mat, colors, **kw):
    """Главный распад: частицы палитры с тела, нарастают к ~1,5 с."""
    e = dict(MaxNum=140, BirthRate=80, BirthRateRND=20, LocalTransform=0, Material=mat, SizeMultiplier=0.03,
             SizeRND=0.015, Velocity=0.6, VelocityRND=0.5, AirResistance=1.2, LifeTime=1.2, LifeTimeRND=0.6,
             Color=colors, Alpha=[0, 0, 0.08, 1, 0.7, 0.8, 1, 0], Size=[0, 1, 1, 0.3])
    e.update(BODY)
    e.update(BURST)
    e.update(kw)
    return emitter("crumble", **e)


def dissolve(pal):
    rgb = LIGHT[pal]["rgb"]
    if pal == "fire":
        return [crumble("spark", col((0, (1, 0.55, 0)), (0.5, (1, 0.25, 0)), (1, (0.6, 0.05, 0))), GravityMultiply=-0.2,
                        GravityMultiplyRND=0.1, Wind=1),
                puff((0.9, 0.3, 0), (0.3, 0.05, 0), alpha=0.35, rise=-0.06), flash(rgb)]
    if pal == "storm":
        arcs = emitter("arcs", MaxNum=6, BirthRate=7, BirthRateRND=5, LocalTransform=0, Material="electric",
                       SizeMultiplier=0.45, SizeRND=0.2, Velocity=0, LifeTime=0.1, LifeTimeRND=0.08,
                       Color=col((0, (0, 0.7, 1)), (1, (0, 0.45, 1))), Alpha=[0, 1, 1, 0], **dict(BODY, **BURST))
        return [crumble("spark", col((0, (0, 0.8, 1)), (0.5, (0, 0.45, 1)), (1, (0, 0.2, 0.9))), SizeMultiplier=0.018,
                        StretchMultiplier=1.02, VelAngle=1, Velocity=2, VelocityRND=1.2, AirResistance=2.5,
                        LifeTime=0.6, LifeTimeRND=0.4), arcs, flash(rgb)]
    if pal == "ice":
        return [crumble("flake", col((0, (0.65, 0.88, 1)), (1, (0.4, 0.7, 1))), SizeMultiplier=0.045, SizeRND=0.02,
                        RandomRotDir=1, RotMultiplier=60, GravityMultiply=0.12, GravityMultiplyRND=0.06, Wind=1,
                        AnimFPS=0, RandomFrame=1, LifeTime=1.6, LifeTimeRND=0.6),
                puff((0.7, 0.85, 1), (0.6, 0.8, 1), mat="mist", alpha=0.3, rise=0.02), flash(rgb)]
    if pal == "gravity":
        # тело «схлопывается»: пыль летит в центр (Spring) и гаснет
        ring = emitter("ring", ShapeType="Point", ShapeSize=(0, 0, 0), ConeAngle=(0, 0, 0), MaxNum=3, BirthRate=1.8,
                       LocalTransform=0, Material="ring", SizeMultiplier=2.4, Velocity=0, LifeTime=1.2,
                       Color=col((0, (0.55, 0, 1)), (1, (0.3, 0, 0.9))), Alpha=[0, 0, 0.2, 0.5, 1, 0],
                       Size=[0, 1, 1, 0.05], **BURST)
        return [crumble("dust", col((0, (0.5, 0, 1)), (1, (0.3, 0, 0.8))), MaxNum=60, BirthRate=35, BirthRateRND=10,
                        SizeMultiplier=0.15, SizeRND=0.06, RandomRotDir=1, RotMultiplier=120, Velocity=0.9,
                        VelocityRND=0.4, AirResistance=0.4, Spring=2.5, LifeTime=1.5, LifeTimeRND=0.5,
                        Alpha=[0, 0, 0.15, 0.6, 0.8, 0.4, 1, 0]),
                emitter("motes", MaxNum=80, BirthRate=50, BirthRateRND=10, LocalTransform=0, Material="spark",
                        SizeMultiplier=0.02, SizeRND=0.01, Velocity=1, VelocityRND=0.5, AirResistance=0.4, Spring=3,
                        LifeTime=1.2, LifeTimeRND=0.5, Color=col((0, (0.7, 0, 1)), (1, (0.4, 0, 1))),
                        Alpha=[0, 0, 0.1, 1, 0.8, 0.8, 1, 0], Size=[0, 1, 1, 0.2], **dict(BODY, **BURST)),
                ring, flash(rgb)]
    if pal == "acid":
        return [crumble("spark", col((0, (0.35, 1, 0)), (1, (0.15, 0.75, 0))), SizeMultiplier=0.025,
                        StretchMultiplier=1.015, VelAngle=1, Velocity=0.9, GravityMultiply=0.6, GravityMultiplyRND=0.2,
                        AirResistance=0.6),
                puff((0.25, 0.9, 0), (0.1, 0.45, 0), alpha=0.35, rise=-0.05), flash(rgb)]
    if pal == "psi":
        return [crumble("glow", col((0, (0.9, 0.8, 1)), (0.5, (0.7, 0.9, 1)), (1, (1, 0.85, 1))), SizeMultiplier=0.05,
                        SizeRND=0.02, Velocity=0.4, VelocityRND=0.3, AirResistance=0.8, GravityMultiply=-0.05,
                        LifeTime=1.8, LifeTimeRND=0.6),
                puff((0.8, 0.75, 1), (0.7, 0.85, 1), mat="glow", alpha=0.2, rise=-0.02), flash(rgb)]
    if pal == "rad":
        return [crumble("spark", col((0, (0.75, 1, 0)), (0.5, (0.6, 1, 0)), (1, (0.3, 0.75, 0))), GravityMultiply=-0.08,
                        GravityMultiplyRND=0.05, Wind=1, LifeTime=1.4),
                puff((0.6, 1, 0), (0.25, 0.5, 0), alpha=0.3, rise=-0.04), flash(rgb)]
    raise SystemExit("нет распада для палитры " + pal)


AURA = {"fire": aura_fire, "storm": aura_storm, "ice": aura_ice, "gravity": aura_gravity, "acid": aura_acid,
        "psi": aura_psi, "rad": aura_rad}


# ------------------------------------------------------------------ проверки
def vanilla_int_keys():
    """Ключи, у которых в ванили только целые значения (E47)."""
    kinds = defaultdict(set)
    for d, _, fs in os.walk(VANILLA_PTC):
        for f in fs:
            if not f.lower().endswith(".ptc"):
                continue
            with open(os.path.join(d, f), encoding="utf-8", errors="replace") as fh:
                for line in fh:
                    s = line.split()
                    if len(s) >= 2 and re.match(r"^[A-Z]\w+$", s[0]) and s[1] != "{":
                        kinds[s[0]].add("int" if re.fullmatch(r"-?\d+", s[1]) else "other")
    return {k for k, v in kinds.items() if v == {"int"}}


def check_ptc(path, int_keys):
    bad = []
    with open(path, encoding="utf-8") as fh:
        for n, line in enumerate(fh, 1):
            s = line.split()
            if len(s) >= 2 and s[0] in int_keys and not all(re.fullmatch(r"-?\d+", x) for x in s[1:]):
                bad.append("%s:%d: %s" % (os.path.basename(path), n, line.strip()))
    return bad


def check_materials():
    bad = []
    for k, m in MAT.items():
        path = m.split("}", 1)[1]
        if path.lower().startswith("vb_ab/"):
            disk = os.path.join(MOD, *path.split("/")[1:])
        else:
            disk = os.path.join("P:" + os.sep, *path.split("/"))
            if not os.path.isdir("P:" + os.sep):
                continue
        if not os.path.exists(disk):
            bad.append("нет материала %s: %s" % (k, disk))
    return bad


def check_palettes():
    try:
        sys.dont_write_bytecode = True      # не оставлять __pycache__ от импорта в репозитории
        sys.path.insert(0, HERE)
        import ab_skins
        have = sorted({pal for _, _, pal in ab_skins.SKINS})
    except Exception as ex:          # ab_skins без numpy тоже грузится, но на всякий случай не падаем
        print("   ! палитры из ab_skins не прочитаны:", ex)
        return []
    miss = [p for p in have if p not in PALETTES]
    return ["у скинов есть палитра %s, а ауры для неё нет" % p for p in miss]


# ------------------------------------------------------------------ скрипты
def lights_c():
    L = ["// " + HEAD,
         "// Свет аур боссов (VB_AB_Aura.c). Только ночью (днём точечный свет не нужен), без теней.",
         "// Цвет — строгими каналами (E45). Радиус и яркость — для масштаба 1, VB_SetScale умножает радиус.",
         "",
         "class VB_AB_Light_AuraBase extends PointLightBase",
         "{",
         "\tprotected float m_VB_BaseRadius = 8;",
         "\tprotected float m_VB_BaseBright = 1;",
         "",
         "\tvoid VB_AB_Light_AuraBase()",
         "\t{",
         "\t\tSetVisibleDuringDaylight(false);",
         "\t\tSetCastShadow(false);",
         "\t\tSetFlareVisible(false);",
         "\t\tSetFadeOutTime(%s);" % num(DISSOLVE_LIGHT_SEC),
         "\t}",
         "",
         "\tprotected void VB_InitAura(float r, float g, float b, float radius, float bright)",
         "\t{",
         "\t\tm_VB_BaseRadius = radius;",
         "\t\tm_VB_BaseBright = bright;",
         "\t\tSetDiffuseColor(r, g, b);",
         "\t\tSetAmbientColor(r, g, b);",
         "\t\tSetRadiusTo(radius);",
         "\t\tSetBrightnessTo(bright);",
         "\t}",
         "",
         "\t// масштаб босса (Энт 1,6): шире круг света, яркость та же",
         "\tvoid VB_SetScale(float scale)",
         "\t{",
         "\t\tSetRadiusTo(m_VB_BaseRadius * Math.Max(1, scale));",
         "\t}",
         "",
         "\t// вспышка при распаде тела: ярче, гаснет за %s с и удаляется сама" % num(DISSOLVE_LIGHT_SEC),
         "\tvoid VB_Flash(float scale)",
         "\t{",
         "\t\tSetRadiusTo(m_VB_BaseRadius * Math.Max(1, scale));",
         "\t\tSetBrightnessTo(m_VB_BaseBright * %s);" % num(DISSOLVE_LIGHT_MULT),
         "\t\tFadeOut(%s);" % num(DISSOLVE_LIGHT_SEC),
         "\t}",
         "};"]
    for pal in PALETTES:
        lt = LIGHT[pal]
        cls = "VB_AB_Light_Aura" + pal.capitalize()
        L += ["",
              "class %s extends VB_AB_Light_AuraBase" % cls,
              "{",
              "\tvoid %s()" % cls,
              "\t{",
              "\t\tVB_InitAura(%s, %s, %s, %s, %s);" % (tuple(num(x) for x in lt["rgb"]) + (num(lt["radius"]), num(lt["bright"])))]
        amp, spd = lt["flicker"]
        if amp > 0:
            L += ["\t\tSetFlickerAmplitude(%s);" % num(amp), "\t\tSetFlickerSpeed(%s);" % num(spd)]
        if lt.get("haze"):
            L += ["\t\tEnableHeatHaze(true);", "\t\tSetHeatHazeRadius(1.2);", "\t\tSetHeatHazePower(0.006);"]
        L += ["\t}", "};"]
    return "\n".join(L) + "\n"


def particles_c(names):
    L = ["// " + HEAD,
         "// Частицы аур и распада тела боссов (VB_AB_Aura.c). Номер по имени: ParticleList.GetParticleIDByName(\"vb_ab_aura_...\").",
         "// Частицы аномалий — отдельно, в VB_AB_ParticlesGen.c (его пишет ab_anomalies.py).",
         "modded class ParticleList",
         "{"]
    for n in names:
        L.append('\tstatic const int %s = RegisterParticle("VB_AB/p/", "%s");' % (n.upper(), n))
    L.append("};")
    return "\n".join(L) + "\n"


def write(path, text):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "w", encoding="utf-8", newline="\n") as f:
        f.write(text)


def main():
    problems = check_materials() + check_palettes()
    names, files = [], []
    for pal in PALETTES:
        for suffix, emitters in (("", AURA[pal]()), ("_dissolve", dissolve(pal))):
            name = "vb_ab_aura_%s%s" % (pal, suffix)
            path = os.path.join(MOD, "p", name + ".ptc")
            write(path, effect(emitters))
            names.append(name)
            files.append(path)
    scr3 = os.path.join(MOD, "scripts", "3_Game", "VB_AB", "VB_AB_AuraParticlesGen.c")
    scr4 = os.path.join(MOD, "scripts", "4_World", "VB_AB", "VB_AB_AuraLightsGen.c")
    write(scr3, particles_c(names))
    write(scr4, lights_c())

    if os.path.isdir(VANILLA_PTC):
        ik = vanilla_int_keys() | INT_KEYS | INT_VEC_KEYS
        for f in files:
            problems += check_ptc(f, ik)
    else:
        print("   ! нет %s — ключи с ванилью не сверены (subst P: G:\\DayZ_P)" % VANILLA_PTC)

    total = sum(os.path.getsize(f) for f in files)
    print("частиц %d (%s), %.1f КБ; новых текстур и материалов нет" % (len(files), ", ".join(names[:2]) + ", ...", total / 1024.0))
    print("скрипты:\n   %s\n   %s" % (scr3, scr4))
    for p in problems:
        print("   ! " + p)
    sys.exit(1 if problems else 0)


if __name__ == "__main__":
    main()
