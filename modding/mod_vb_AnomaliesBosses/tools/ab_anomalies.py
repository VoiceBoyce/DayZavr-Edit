# -*- coding: utf-8 -*-
"""Перенос аномалий OOTS в VB_AnomaliesBosses: ресурсы + сгенерированные конфиги и регистрация частиц.

Берёт таблицу tools/anomalies_spec.json (её делает ab_ootsspec.py) и для выбранных аномалий:
  - частицы .ptc -> VB_AnomaliesBosses/p/vb_ab_<имя>.ptc (пути материалов переписаны на VB_AB/p/m/...);
  - материалы .emat и текстуры .edds -> VB_AnomaliesBosses/p/m/... (атласы урезаются по мипам до MAX_DIM);
  - звуки .ogg -> VB_AnomaliesBosses/s/a/...;
  - cfg/anomalies.hpp (классы VB_AB_Anomaly_*), cfg/sounds.hpp (CfgSounds), cfg/ammo.hpp (CfgAmmo);
  - scripts/3_Game/VB_AB/VB_AB_ParticlesGen.c (регистрация частиц).
Запуск: python ab_anomalies.py [имя ...]   без имён — все 22 аномалии из ALL.
"""
import json
import os
import re
import shutil
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, ".."))
MOD = os.path.join(ROOT, "VB_AnomaliesBosses")
OOTS = os.path.join(ROOT, "исходники модов", "аномалии", "@OTTS_AnomVibrosPsi", "Addons", "OOTS_Anomaly_Client")
DMG_CFG = os.path.join(ROOT, "исходники модов", "аномалии", "аномалии на 16 сервере", "AnomalySpawner",
                       "AnomalyDmgCfg.json")
BS = chr(92)
MAX_DIM = 4096          # самая большая сторона текстуры частиц после урезания

# Все 22 аномалии (имя у нас -> класс OOTS). Генератор переписывает cfg/*.hpp целиком,
# поэтому без аргументов переносятся все; с аргументами — только названные (для проверки одной).
ALL = {
    "Electra": "Anomaly_Electra", "EMP": "Anomaly_EMP_Blast", "Groza": "Anomaly_Groza",
    "MlechniPut": "Anomaly_MlechniPut", "Zharka": "Anomaly_Zharka", "FireShtorm": "Anomaly_FireShtorm",
    "FireIskra": "Anomaly_FireIskra", "Par": "Anomaly_Par", "Snow": "Anomaly_Snow", "Steklo": "Anomaly_Steklo",
    "Zastoy": "Anomaly_Zastoy", "Voronka": "Anomaly_Voronka", "Lomka": "Anomaly_Lomka",
    "Tramplin": "Anomaly_Tramplin", "Roy": "Anomaly_Roy", "Studen": "Anomaly_Studen", "Holera": "Anomaly_Holera",
    "Flash": "Anomaly_Flash", "LinceAgr": "Anomaly_LinceAgr", "Lince": "Anomaly_Lince",
    "DeadZone": "Anomaly_DeadZone", "Atom": "Anomaly_Atom",
}

# Особое действие при срабатывании (логика в скрипте VB_AB_AnomalyBase.VB_ApplySpecial)
EFFECTS = {
    "Zharka": "heat", "Par": "heat", "Groza": "wet", "Snow": "wet", "EMP": "emp", "Atom": "push",
    "Voronka": "twophase",
}
# Радиус особого действия, м (у OOTS: жар и намокание 10, ЭМИ 7, толчок Атома 40 -> у нас 8)
EFFECT_RADIUS = {"heat": 10.0, "wet": 10.0, "emp": 7.0, "push": 8.0}
# Свет, которого у аномалии нет в OOTS, не добавляем; классы света переносятся из AnomalyLights.c
LIGHTS = {
    "Anomaly_Electra_Idle_Light": "VB_AB_Light_ElectraIdle",
    "Anomaly_Electra_Active_Light": "VB_AB_Light_ElectraActive",
    "Anomaly_Studen_Idle_Light": "VB_AB_Light_StudenIdle",
    "Anomaly_Studen_Active_Light": "VB_AB_Light_StudenActive",
    "Anomaly_Zharka_Active_Light": "VB_AB_Light_ZharkaActive",
    "Anomaly_Holera_Idle_Light": "VB_AB_Light_HoleraIdle",
    "Anomaly_Holera_Active_Light": "VB_AB_Light_HoleraActive",
}
# Вторая фаза Воронки: своя частица
EXTRA_PTC = {"Voronka": "OOTS_voronka_active_2"}

# Баланс урона (2026-11-13, пользователь: «с первого раза аномалия не должна убить игрока»).
# У игрока 100 здоровья. Аномалия бьёт в случайную зону из vbABHitZones; в общее здоровье уходит торс ×1,
# ноги ×0,25, кисти и стопы ×0,1 (DZ/characters/data/config.cpp, SurvivorBase); кровь из зон в общую не уходит;
# одежда этот урон не режет (тип "VB_AB"). Худший случай — торс: Health × DmgMax (× DamageMult из anomalies.json).
# В OOTS было до 375 (Электра 150 × 2,5) — смерть с одного раза. Ступени теперь:
#   тяжёлые 45 — два срабатывания подряд 90, третье убивает; средние 35 и 25; лёгкие до 10.
# Рядом с боссом: тяжёлая аномалия 45 + удар Энта ~20 (MeleeMul 3, без брони) + Разряд 25 ≈ 90 — ещё жив,
# но на грани; уйти из поля и подлечиться — нормальный ход боя.
# Трамплин и Атом бьют только ноги (в общее ×0,25): Трамплин до 15, Атом до 25 + толчок ~14.
AMMO_BALANCE = {          # патрон OOTS: поля DamageApplied, которые меняем
    "ElectraDmg": {"Health": 45},
    "FireShtormDmg": {"Health": 45},
    "LomkaDmg": {"Health": 45},
    "RoyDmg": {"Health": 45},
    "FlashDmg": {"Health": 45},
    "VoronkaDmg": {"Health": 45},
    "FireIskraDmg": {"Health": 35},             # Огненная искра и Млечный путь
    "ZastoyDmg": {"Health": 35, "Shock": 40},
    "StudenDmg": {"Health": 35},
    "GrozaDmg": {"Health": 25},
    "ZharkaDmg": {"Health": 25},
    "ParDmg": {"Health": 25, "Shock": 30},
    "DeadZoneDmg": {"Health": 25},
    "SnowDmg": {"Health": 10},
    "StekloDmg": {"Health": 10},
    "HoleraDmg": {"Health": 8},
    "AtomDmg": {"Health": 100},
}
RATIO_BALANCE = {         # наша аномалия: DmgMin, DmgMax — разброс урона за срабатывание
    "Electra": (0.7, 1.0),      # в OOTS 0,9..2,5
    "Voronka": (0.6, 1.0),      # в OOTS 0,1..0,85
    "Studen": (0.7, 1.0),       # в OOTS 0,5..1,3
}

MAT_RE = re.compile(r'(Material\s+")(\{[0-9A-Fa-f]+\})?([^"]+)(")')
MAP_RE = re.compile(r'(\w*Map\s+")(\{[0-9A-Fa-f]+\})?([^"]+)(")')


def oots_file(rel):
    """Путь вида OOTS_Anomaly_Client/graphics/... -> файл в исходниках."""
    parts = rel.replace(BS, "/").split("/")
    assert parts[0].lower() == "oots_anomaly_client", rel
    return os.path.join(OOTS, *parts[1:])


def our_rel(rel):
    """OOTS_Anomaly_Client/graphics/particles/materials/X -> VB_AB/p/m/X."""
    parts = rel.replace(BS, "/").split("/")
    low = [p.lower() for p in parts]
    i = low.index("materials")
    return "VB_AB/p/m/" + "/".join(parts[i + 1:])


def mod_file(rel):
    parts = rel.split("/")
    assert parts[0] == "VB_AB"
    return os.path.join(MOD, *parts[1:])


def find_ci(path):
    """Файл без учёта регистра (Windows и так не различает, но имя берём настоящее)."""
    if os.path.exists(path):
        return path
    raise FileNotFoundError(path)


# ------------------------------------------------------------------ edds
def trim_edds(data, max_dim):
    """Отбросить крупные мипы, пока сторона больше max_dim. Формат: DDS-заголовок, таблица блоков
    [тег COPY/LZ4 + размер] от меньшего мипа к большему, затем данные в том же порядке."""
    size, flags, h, w, pitch, depth, mips = struct.unpack_from("<7I", data, 4)
    pf_flags, fourcc = struct.unpack_from("<I4s", data, 80)
    hdr = 128
    if fourcc == b"DX10":
        hdr += 20
    heads = [(data[hdr + 8 * i:hdr + 8 * i + 4], struct.unpack_from("<I", data, hdr + 8 * i + 4)[0])
             for i in range(mips)]
    blocks, o = [], hdr + 8 * mips
    for tag, n in heads:
        blocks.append(data[o:o + n])
        o += n
    drop = 0
    while max(w >> drop, h >> drop) > max_dim and mips - drop > 1:
        drop += 1
    if drop == 0:
        return data, (w, h)
    nw, nh, nm = max(1, w >> drop), max(1, h >> drop), mips - drop
    if pf_flags & 0x4:      # сжатый формат: размер верхнего мипа
        bs = 8 if fourcc in (b"DXT1",) else 16
        npitch = max(1, (nw + 3) // 4) * max(1, (nh + 3) // 4) * bs
    else:
        npitch = nw * (pitch // w if w else 4)
    out = bytearray(data[:hdr])
    struct.pack_into("<7I", out, 4, size, flags, nh, nw, npitch, depth, nm)
    for tag, n in heads[:nm]:
        out += tag + struct.pack("<I", n)
    for b in blocks[:nm]:
        out += b
    return bytes(out), (nw, nh)


# ------------------------------------------------------------------ частицы и материалы
copied = {}


def port_texture(rel):
    if rel in copied:
        return copied[rel]
    src = find_ci(oots_file(rel))
    dst_rel = our_rel(rel)
    dst = mod_file(dst_rel)
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    data = open(src, "rb").read()
    if src.lower().endswith(".edds"):
        data, dim = trim_edds(data, MAX_DIM)
    with open(dst, "wb") as f:
        f.write(data)
    copied[rel] = dst_rel
    return dst_rel


def port_emat(rel):
    if rel in copied:
        return copied[rel]
    src = find_ci(oots_file(rel))
    txt = open(src, encoding="utf-8", errors="replace").read()

    def sub(m):
        path = m.group(3)
        if path.lower().startswith("oots_anomaly_client"):
            return m.group(1) + (m.group(2) or "") + port_texture(path) + m.group(4)
        is_path = "/" in path or BS in path        # не путь: UVSource... "UV set 1"
        if is_path and not os.path.exists(os.path.join("P:" + BS, *path.replace(BS, "/").split("/"))):
            # Чужой путь, которого нет ни в OOTS, ни в ванили (у Жарки — D&D_Amonaly/...SparkLongNorm.edds):
            # клиент пишет MATERIAL (E). Строку карты убираем, материал работает без неё.
            print("  убрана карта с чужим путём:", path, "в", os.path.basename(rel))
            return "\0DROP\0"
        return m.group(0)

    txt = MAP_RE.sub(sub, txt)
    txt = "\n".join(l for l in txt.split("\n") if "\0DROP\0" not in l)
    dst_rel = our_rel(rel)
    dst = mod_file(dst_rel)
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    with open(dst, "w", encoding="utf-8", newline="\n") as f:
        f.write(txt)
    copied[rel] = dst_rel
    return dst_rel


def port_ptc(oots_name, our_name):
    src_dir = os.path.join(OOTS, "graphics", "particles")
    src = None
    for fn in os.listdir(src_dir):
        if fn.lower() == oots_name.lower() + ".ptc":
            src = os.path.join(src_dir, fn)
    if not src:
        raise FileNotFoundError(oots_name)
    txt = open(src, encoding="utf-8", errors="replace").read()

    def sub(m):
        path = m.group(3)
        if path.lower().startswith("oots_anomaly_client"):
            return m.group(1) + (m.group(2) or "") + port_emat(path) + m.group(4)
        return m.group(0)

    txt = MAT_RE.sub(sub, txt)
    dst = os.path.join(MOD, "p", our_name + ".ptc")
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    with open(dst, "w", encoding="utf-8", newline="\n") as f:
        f.write(txt)
    return our_name


# ------------------------------------------------------------------ звуки
def port_sound(info):
    """info: file (\\OOTS_Anomaly_Client\\data\\sounds\\anomalies\\x), volume, pitch, distance."""
    rel = info["file"].strip(BS).replace(BS, "/")
    parts = rel.split("/")
    base = os.path.join(OOTS, *parts[1:-1])
    name = parts[-1]
    src = None
    for fn in os.listdir(base):
        stem, ext = os.path.splitext(fn)
        if stem.lower() == name.lower() and ext.lower() in (".ogg", ".wss", ".wav"):
            src = os.path.join(base, fn)
    if not src:
        raise FileNotFoundError(info["file"])
    dst = os.path.join(MOD, "s", "a", os.path.basename(src).lower())
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    shutil.copyfile(src, dst)
    return "VB_AB" + BS + "s" + BS + "a" + BS + os.path.splitext(os.path.basename(dst))[0]


# ------------------------------------------------------------------ детекторы
DET_SOUNDS = {          # имя в CfgSounds -> файл OOTS (data/sounds/detectors), громкость, дальность
    "vb_ab_detector_far": ("detector_middle", 1, 10),
    "vb_ab_detector_mid": ("detector_close", 1, 10),
    "vb_ab_detector_close": ("detector_spawn", 1, 10),
}


def port_detector_assets(sounds):
    det = os.path.join(OOTS, "data", "sounds", "detectors")
    for name, (fn, vol, dist) in DET_SOUNDS.items():
        dst = os.path.join(MOD, "s", "d", fn + ".ogg")
        os.makedirs(os.path.dirname(dst), exist_ok=True)
        shutil.copyfile(os.path.join(det, fn + ".ogg"), dst)
        sounds[name] = dict(path="VB_AB" + BS + "s" + BS + "d" + BS + fn, volume=vol, pitch=1, distance=dist)
    shutil.copyfile(os.path.join(det, "artefact_spawn.ogg"), os.path.join(MOD, "s", "d", "artefact_spawn.ogg"))
    # Отклик: горящая лампа (текстура из OOTS и материал со свечением на основе перенесённого материала лампы)
    ok = os.path.join(OOTS, "data", "objects", "Detectors", "Oklik", "date")
    out = os.path.join(MOD, "t", "dox")
    shutil.copyfile(os.path.join(ok, "Lampa_Defaultl_Blue_ca.paa"), os.path.join(out, "lampa_on_ca.paa"))
    base = [f for f in os.listdir(out) if f.startswith("oklik_lampa") and f.endswith(".rvmat") and "_on" not in f]
    if base:
        txt = open(os.path.join(out, base[0]), encoding="utf-8").read()
        txt = re.sub(r"emmisive\[\]=\{[^}]*\};", "emmisive[]={2.5,1.1,0,1};", txt)   # янтарь (ретекстур Лозы)
        with open(os.path.join(out, "oklik_lampa_on.rvmat"), "w", encoding="utf-8", newline="\r\n") as f:
            f.write(txt.replace("\r\n", "\n"))
    sys.path.insert(0, HERE)
    import ab_trim                  # лампа Отклика пришла в 2048, ужать вместе с остальными картами детекторов
    ab_trim.trim_detectors(quiet=True)


# ------------------------------------------------------------------ генерация
def main():
    spec = json.load(open(os.path.join(HERE, "anomalies_spec.json"), encoding="utf-8"))
    ratios = {}
    if os.path.exists(DMG_CFG):
        raw = open(DMG_CFG, encoding="utf-8-sig").read()
        for m in re.finditer(r'"(Anomaly_\w+)"\s*:\s*\{[^}]*?"m_DmgRatioMinMax"\s*:\s*\[([\d.]+),\s*([\d.]+)\]', raw):
            ratios[m.group(1)] = (float(m.group(2)), float(m.group(3)))
    names = sys.argv[1:] or list(ALL)
    sel = {n: ALL[n] for n in names}

    vehicles, sounds, ammo, particles = [], {}, {}, []
    for our, oots in sel.items():
        e = spec[oots]
        lo = our.lower()
        cls = []
        cls.append("\tclass VB_AB_Anomaly_%s: VB_AB_AnomalyBase" % our)
        cls.append("\t{")
        cls.append("\t\tscope = 2;")
        for st in ("idle", "active"):
            s = e[st]
            key = "Idle" if st == "idle" else "Active"
            if s.get("ptc"):
                pn = port_ptc(s["ptc"], "vb_ab_%s_%s" % (lo, st))
                particles.append(pn)
                cls.append('\t\tvbABPtc%s = "%s";' % (key, pn))
            if s.get("sound_file"):
                sname = "vb_ab_%s_%s" % (lo, st)
                sounds[sname] = dict(path=port_sound(s["sound_file"]), **{k: s["sound_file"][k] for k in ("volume", "pitch", "distance")})
                cls.append('\t\tvbABSnd%s = "%s";' % (key, sname))
                cls.append("\t\tvbABSnd%sRange = %g;" % (key, s.get("sound_range", 30)))
                cls.append("\t\tvbABSnd%sLoop = %d;" % (key, 1 if s.get("sound_loop") else 0))
            if s.get("light") and s["light"] in LIGHTS:
                cls.append('\t\tvbABLight%s = "%s";' % (key, LIGHTS[s["light"]]))
        if our in EXTRA_PTC:
            pn = port_ptc(EXTRA_PTC[our], "vb_ab_%s_active2" % lo)
            particles.append(pn)
            cls.append('\t\tvbABPtcActive2 = "%s";' % pn)
        ext = e.get("extents") or [[2, 2]]
        cls.append("\t\tvbABRadius = %g;" % ext[-1][0] if our != "Voronka" else "\t\tvbABRadius = %g;" % ext[0][0])
        cls.append("\t\tvbABHeight = %g;" % (ext[-1][1] if our != "Voronka" else ext[0][1]))
        if our == "Voronka":
            cls.append("\t\tvbABRadius2 = %g;" % ext[1][0])
            cls.append("\t\tvbABHeight2 = %g;" % ext[1][1])
        cls.append("\t\tvbABActiveTime = %g;" % e.get("active_time", 1))
        cls.append("\t\tvbABRechargeTime = %g;" % e.get("recharge", 1))
        if e["ammo"]:
            a = e["ammo"][-1]
            cls.append('\t\tvbABAmmo = "VB_AB_%s";' % a)
            d = dict(e["ammo_damage"].get(a) or {})
            d.update(AMMO_BALANCE.get(a, {}))
            ammo["VB_AB_" + a] = d
        zones = e.get("hitzones") or ["Torso"]
        cls.append("\t\tvbABHitZones[] = {%s};" % ",".join('"%s"' % z for z in zones))
        lo_r, hi_r = RATIO_BALANCE.get(our) or ratios.get(oots, (1.0, 1.0))
        cls.append("\t\tvbABDmgMin = %g;" % lo_r)
        cls.append("\t\tvbABDmgMax = %g;" % hi_r)
        eff = EFFECTS.get(our, "")
        if eff:
            cls.append('\t\tvbABEffect = "%s";' % eff)
            if eff in EFFECT_RADIUS:
                cls.append("\t\tvbABEffectRadius = %g;" % EFFECT_RADIUS[eff])
        if our == "Voronka":
            cls.append("\t\tvbABChargeTime = 2;")
        cls.append("\t};")
        vehicles.append("\n".join(cls))

    port_detector_assets(sounds)
    cfg_dir = os.path.join(MOD, "cfg")
    os.makedirs(cfg_dir, exist_ok=True)
    head = "// СГЕНЕРИРОВАНО tools/ab_anomalies.py — руками не править\n"
    with open(os.path.join(cfg_dir, "anomalies.hpp"), "w", encoding="utf-8", newline="\n") as f:
        f.write(head + "\n".join(vehicles) + "\n")
    with open(os.path.join(cfg_dir, "sounds.hpp"), "w", encoding="utf-8", newline="\n") as f:
        f.write(head)
        for n, s in sounds.items():
            f.write('\tclass %s\n\t{\n\t\tname = "%s";\n\t\tsound[] = {"%s", %g, %g, %g};\n\t\ttitles[] = {};\n\t};\n'
                    % (n, n, s["path"], s["volume"], s["pitch"], s["distance"]))
    # Те же звуки как SoundSet: способности боссов играют их в точке (SEffectManager.PlaySound(<имя>_SoundSet, pos)),
    # а CfgSounds звучит только с объекта. Громкость OOTS (до 25) ужата в 0,3–2.
    with open(os.path.join(cfg_dir, "soundshaders.hpp"), "w", encoding="utf-8", newline="\n") as f:
        f.write(head)
        for n, s in sounds.items():
            vol = max(0.3, min(2.0, s["volume"] * 0.25))
            d = max(10.0, s["distance"])
            f.write('\tclass %s_SoundShader\n\t{\n\t\tsamples[] = {{"%s", 1}};\n\t\tvolume = %g;\n\t\trange = %g;\n'
                    '\t\trangeCurve[] = {{0, 1}, {%g, 0.6}, {%g, 0}};\n\t};\n' % (n, s["path"], vol, d, d * 0.4, d))
    with open(os.path.join(cfg_dir, "soundsets.hpp"), "w", encoding="utf-8", newline="\n") as f:
        f.write(head)
        for n in sounds:
            f.write('\tclass %s_SoundSet\n\t{\n\t\tsoundShaders[] = {"%s_SoundShader"};\n\t\tspatial = 1;\n\t\tdoppler = 0;\n'
                    '\t\tloop = 0;\n\t\tvolumeCurve = "LinearCurve";\n'
                    '\t\tsound3DProcessingType = "ObjectsInner13DProcessingType";\n\t};\n' % (n, n))
    with open(os.path.join(cfg_dir, "ammo.hpp"), "w", encoding="utf-8", newline="\n") as f:
        f.write(head)
        for n, d in ammo.items():
            f.write("\tclass %s: MeleeDamage\n\t{\n\t\tclass DamageApplied\n\t\t{\n\t\t\ttype = \"VB_AB\";\n" % n)
            for k in ("Health", "Blood", "Shock"):
                f.write("\t\t\tclass %s\n\t\t\t{\n\t\t\t\tdamage = %g;\n\t\t\t};\n" % (k, d.get(k, 0)))
            f.write("\t\t};\n\t};\n")
    scr = os.path.join(MOD, "scripts", "3_Game", "VB_AB", "VB_AB_ParticlesGen.c")
    with open(scr, "w", encoding="utf-8", newline="\n") as f:
        f.write("// СГЕНЕРИРОВАНО tools/ab_anomalies.py — руками не править\n")
        f.write("// Частицы аномалий. Номер по имени: ParticleList.GetParticleIDByName(\"vb_ab_...\").\n")
        f.write("modded class ParticleList\n{\n")
        for p in particles:
            f.write('\tstatic const int %s = RegisterParticle("VB_AB/p/", "%s");\n' % (p.upper(), p))
        f.write("};\n")
    total = 0
    for dp, dn, fn in os.walk(os.path.join(MOD, "p")):
        total += sum(os.path.getsize(os.path.join(dp, x)) for x in fn)
    print("аномалий %d, частиц %d, звуков %d, патронов %d, частицы на диске %.1f МБ"
          % (len(vehicles), len(particles), len(sounds), len(ammo), total / 1048576.0))


main()
