# -*- coding: utf-8 -*-
"""Сбор данных аномалий OOTS из исходников в одну таблицу (tools/anomalies_spec.json).

Источники (исходники модов/аномалии):
  клиент  OOTS_Anomaly_Scripts/scripts/4_World/Anomalies.c   — свет, звуки, частицы простоя и срабатывания
          OOTS_Anomaly_Scripts/scripts/3_Game/ParticleList_Modded.c — имена .ptc
          OOTS_Anomaly_Client/config.bin (CfgSounds)          — файлы звуков, громкость, дальность
  сервер  OOTS_Anomaly_Server/scripts/4_World/Anomalies_Server.c — тайминги, зона урона, патрон урона, зоны тела
          OOTS_Anomaly_Server/config.bin (CfgAmmo)            — урон патронов
Запуск: python ab_ootsspec.py
"""
import json
import os
import re
import subprocess

HERE = os.path.dirname(os.path.abspath(__file__))
A = os.path.join(HERE, "..", "исходники модов", "аномалии")
CLIENT = os.path.join(A, "@OTTS_AnomVibrosPsi", "Addons")
SERVER = os.path.join(A, "OOTS_Anomaly_Server")
CFGCONVERT = os.path.join("D:" + chr(92), "SteamLibrary", "steamapps", "common", "DayZ Tools", "Bin",
                          "CfgConvert", "CfgConvert.exe")
TMP = os.path.join(os.environ.get("TEMP", HERE), "vb_ab_spec")


def derap(src, name):
    os.makedirs(TMP, exist_ok=True)
    dst = os.path.join(TMP, name + ".cpp")
    subprocess.run([CFGCONVERT, "-txt", "-dst", dst, src], capture_output=True)
    with open(dst, encoding="utf-8", errors="replace") as f:
        return f.read()


def class_bodies(text, header_re):
    """{имя класса: тело} для классов верхнего уровня файла скриптов."""
    out = {}
    for m in re.finditer(header_re, text, re.M):
        name = m.group(1)
        i = text.find("{", m.end() - 1)
        d, j = 0, i
        while True:
            c = text[j]
            d += (c == "{") - (c == "}")
            j += 1
            if d == 0:
                break
        out.setdefault(name, "")
        out[name] += text[i:j]
    return out


def method(body, name):
    m = re.search(r"\b" + name + r"\s*\([^)]*\)\s*\{", body)
    if not m:
        return ""
    i = m.end() - 1
    d, j = 0, i
    while True:
        c = body[j]
        d += (c == "{") - (c == "}")
        j += 1
        if d == 0:
            break
    return body[i:j]


def parse_state(code):
    s = {}
    m = re.search(r"CreateLight\((\w+)", code)
    if m:
        s["light"] = m.group(1)
    m = re.search(r"PlaySound(Loop)?\(\s*\"([^\"]+)\"\s*,\s*([\d.]+)", code)
    if m:
        s["sound"] = m.group(2)
        s["sound_loop"] = bool(m.group(1))
        s["sound_range"] = float(m.group(3))
    m = re.search(r"ParticleList\.(\w+)", code)
    if m:
        s["particle"] = m.group(1)
    return s


def main():
    cl = open(os.path.join(CLIENT, "OOTS_Anomaly_Scripts", "scripts", "4_World", "Anomalies.c"),
              encoding="utf-8", errors="replace").read()
    pl = open(os.path.join(CLIENT, "OOTS_Anomaly_Scripts", "scripts", "3_Game", "ParticleList_Modded.c"),
              encoding="utf-8", errors="replace").read()
    sv = open(os.path.join(SERVER, "scripts", "4_World", "Anomalies_Server.c"),
              encoding="utf-8", errors="replace").read()
    particles = {m.group(1): m.group(3) for m in re.finditer(
        r"static const int (\w+)\s*=\s*RegisterParticle\(\s*\"([^\"]*)\"\s*,\s*\"([^\"]+)\"", pl)}

    ccfg = derap(os.path.join(CLIENT, "OOTS_Anomaly_Client", "config.bin"), "client")
    sounds = {}
    for m in re.finditer(r"class (\w+): default\s*\{\s*sound\[\]=\s*\{\s*\"([^\"]+)\",\s*([\d.e-]+),\s*([\d.e-]+),\s*([\d.e-]+)", ccfg):
        sounds[m.group(1).lower()] = dict(file=m.group(2), volume=float(m.group(3)), pitch=float(m.group(4)),
                                          distance=float(m.group(5)))
    scfg = derap(os.path.join(SERVER, "config.bin"), "server")
    ammo = {}
    for m in re.finditer(r"\n\tclass (\w+Dmg)\b[^\n]*\n\t\{", scfg):
        i = m.end()
        d, j = 1, i
        while d:
            d += (scfg[j] == "{") - (scfg[j] == "}")
            j += 1
        body = scfg[i:j]
        dmg = {k: float(v) for k, v in re.findall(r"class (Health|Blood|Shock)\s*\{\s*damage=([\d.]+);", body)}
        ammo[m.group(1)] = dmg

    cbodies = class_bodies(cl, r"^class (Anomaly_\w+)\s*:\s*OOTS_AnomalyBase")
    sbodies = class_bodies(sv, r"^modded class (Anomaly_\w+)")
    spec = {}
    for name, body in cbodies.items():
        if name.startswith("Anomaly_Teleport") or name == "Anomaly_BlackHole":
            continue
        e = {"idle": parse_state(method(body, "SetIdle")), "active": parse_state(method(body, "SetActive"))}
        for st in ("idle", "active"):
            p = e[st].get("particle")
            if p:
                e[st]["ptc"] = particles.get(p)
            snd = e[st].get("sound")
            if snd:
                e[st]["sound_file"] = sounds.get(snd.lower())
        sb = sbodies.get(name, "")
        for key, rx in (("recharge", r"m_RechargeTime\s*=\s*([\d.]+)"), ("active_time", r"m_ActiveTime\s*=\s*([\d.]+)")):
            m = re.search(rx, sb)
            if m:
                e[key] = float(m.group(1))
        ams = re.findall(r"SetAmmoName\(\"(\w*)\"\)", sb)
        e["ammo"] = [a for a in ams if a and a != "ZeroFakeDmg"]
        e["ammo_damage"] = {a: ammo.get(a) for a in e["ammo"]}
        e["extents"] = [[float(x), float(y)] for x, y in re.findall(r"SetCylinderExtents\(([\d.]+),\s*([\d.]+)\)", sb)]
        zones = re.findall(r"SetHitZones\(\{([^}]*)\}\)", sb)
        e["hitzones"] = [z.strip().strip('"') for z in zones[0].split(",")] if zones else []
        e["server_extra"] = sorted(set(re.findall(
            r"(AddToEnvironmentTemperature|SetWetMax|SetEnergy|dBodyApplyImpulse|ProcessDirectDamage|SetHealth|AddHealth)", sb)))
        spec[name] = e
    out = os.path.join(HERE, "anomalies_spec.json")
    with open(out, "w", encoding="utf-8") as f:
        json.dump(spec, f, ensure_ascii=False, indent=1)
    print("аномалий:", len(spec), "->", out)


main()
