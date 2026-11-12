# -*- coding: utf-8 -*-
"""Перенос моделей мутантов SFP/MSFZ в VB_AnomaliesBosses.

Модели бинарные (ODOL v54/v56). Пути к текстурам и материалам зашиты внутри файла строками с нулём в конце
(в списке текстур каждого LOD и во встроенных материалах). Скрипт копирует модель и заменяет каждый путь
SFP/MSFZ на путь той же длины внутри нашего PBO (префикс vb_ab), поэтому длина файла и все смещения
остаются прежними (тот же приём, что у молний VB_RiftSky, mod_vb_Rift/tools/gen_rift_lightning.py).

Раскладка в PBO (папка VB_AnomaliesBosses = префикс vb_ab):
  vb_ab\\m\\vb_<модель>_<скин>.p3d     копия модели для скина
  vb_ab\\t\\<модель><скин>\\...         цветовые текстуры (_co/_ca/_em и без суффикса) и .rvmat скина
  vb_ab\\t\\<модель>\\...               общие карты модели (_nohq, _smdi, _as ...)
Имя файла внутри папки = исходное имя, дополненное «_» перед расширением до нужной длины.

Запуск (нужен numpy только для перекраски):
  python ab_models.py list                    пути внутри исходных моделей
  python ab_models.py build <модель> <скин> [<скин> ...]
"""
import os
import re
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, ".."))
MOD = os.path.join(ROOT, "VB_AnomaliesBosses")
SRC = os.path.join(ROOT, "исходники модов")
BS = chr(92)
PREFIX = "vb_ab"
IMAGE_TO_PAA = os.path.join("D:" + BS, "SteamLibrary", "steamapps", "common", "DayZ Tools",
                            "Bin", "ImageToPAA", "ImageToPAA.exe")

_SFP = os.path.join(SRC, "sfp", "@Server5_SFP", "Addons")
# виртуальный корень исходного PBO -> папка распакованных исходников
ROOTS = {
    "sfp": os.path.join(_SFP, "SFP"),
    "sfp2": os.path.join(_SFP, "SFP2"),
    "sfp3": os.path.join(_SFP, "SFP3"),
    "msfz_mifical_pack": os.path.join(SRC, "msfz", "@MSFZ_Mifical_Pack", "Addons", "MSFZ_Mifical_Pack"),
    "oots_anomaly_client": os.path.join(SRC, "аномалии", "@OTTS_AnomVibrosPsi", "Addons", "OOTS_Anomaly_Client"),
}
_DET = ["oots_anomaly_client", "data", "objects", "detectors"]

# код модели -> имя и исходная модель (виртуальный путь)
MODELS = {
    "en": ("ent", ["msfz_mifical_pack", "ent", "msfz_ent.p3d"]),
    "ri": ("ricar", ["msfz_mifical_pack", "dark_ricar", "msfz_ricar.p3d"]),
    "mo": ("mozgotras", ["sfp3", "1_ai", "mozgot", "mozgot.p3d"]),
    "mr": ("mozgotras_red", ["sfp3", "1_ai", "mozgot", "mozgot_red.p3d"]),
    "te": ("tenevnik", ["sfp", "1_ai", "tenevnik", "tenevnik_original.p3d"]),
    "sh": ("shum", ["sfp3", "1_ai", "shum_female", "shum_plash.p3d"]),
    "wo": ("mutawolf", ["sfp", "1_ai", "mutawolf", "mutawolf.p3d"]),
    # детекторы OOTS (скин всегда x)
    "do": ("detector_otklik", _DET + ["oklik", "detectoroklik.p3d"]),
    "dk": ("detector_oko", _DET + ["oko", "detectoroko.p3d"]),
    "dm": ("detector_medved", _DET + ["medved", "detectormedved.p3d"]),
    "ds": ("detector_svarog", _DET + ["svarog", "detectorsvarog.p3d"]),
    "dv": ("detector_veles", _DET + ["veles", "detectorveles.p3d"]),
}

# суффиксы карт, которые не перекрашиваем (общие для всех скинов модели)
SHARED_SUFFIXES = ("_nohq", "_smdi", "_as", "_mc", "_dt", "_sm", "_no", "_ads", "_mask")

STR_RE = re.compile(b"[" + bytes([0x21]) + b"-" + bytes([0x7E]) + b"]{4,250}" + bytes([0]))


def vpath(parts):
    return BS.join(parts)


def to_disk(vp):
    """Виртуальный путь исходного мода -> файл в распакованных исходниках (регистр не важен)."""
    parts = vp.split(BS)
    base = ROOTS.get(parts[0].lower())
    if base is None:
        return None
    return os.path.join(base, *parts[1:])


def is_ours(vp):
    return vp.split(BS)[0].lower() in ROOTS


def is_shared(vp):
    stem, ext = os.path.splitext(vp.split(BS)[-1].lower())
    return ext == ".paa" and stem.endswith(SHARED_SUFFIXES)


def odol_paths(data):
    """Все пути SFP/MSFZ внутри ODOL: {строка: [смещения]}."""
    found = {}
    for m in STR_RE.finditer(data):
        s = m.group(0)[:-1].decode("latin1")
        if BS in s and is_ours(s):
            found.setdefault(s, []).append(m.start())
    return found


def new_path(old, folder):
    """Путь той же длины: vb_ab\\t\\<folder>\\<имя, дополненное «_» перед расширением>."""
    head = BS.join([PREFIX, "t", folder]) + BS
    name = old.split(BS)[-1].lower()
    stem, ext = os.path.splitext(name)
    budget = len(old) - len(head)
    if budget < len(ext) + 3:
        raise RuntimeError("путь слишком короткий для замены: " + old)
    if len(name) > budget:
        stem = stem[:budget - len(ext)]
    else:
        stem = stem + "_" * (budget - len(name))
    out = head + stem + ext
    assert len(out) == len(old), (old, out)
    return out


def to_mod_file(vp):
    """vb_ab\\t\\... -> файл в папке исходников мода."""
    parts = vp.split(BS)
    assert parts[0] == PREFIX
    return os.path.join(MOD, *parts[1:])


# ------------------------------------------------------------------ перекраска
def paa_to_tga(src, tmp):
    os.makedirs(tmp, exist_ok=True)
    dst = os.path.join(tmp, os.path.basename(src) + ".tga")
    if not os.path.exists(dst):
        subprocess.run([IMAGE_TO_PAA, src, dst], capture_output=True, text=True)
        if not os.path.exists(dst):
            raise RuntimeError("ImageToPAA не смог прочитать " + src)
    return dst


def png_to_paa(png, dst):
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    r = subprocess.run([IMAGE_TO_PAA, png, dst], capture_output=True, text=True)
    if r.returncode != 0 or not os.path.exists(dst):
        raise RuntimeError("ImageToPAA: " + png + " " + r.stdout[-300:] + r.stderr[-300:])


def recolor_copy(src, dst, skin, tmp):
    """Цветовая текстура скина. Пока только «как есть» и пробный синий оттенок (этап 0)."""
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    if skin in ("x", "r"):
        shutil.copyfile(src, dst)
        return
    import ab_tex  # noqa: E402  (numpy нужен только здесь)
    ab_tex.recolor(src, dst, skin, tmp, paa_to_tga, png_to_paa)


# ------------------------------------------------------------------ материалы
TEX_RE = re.compile(r'(texture\s*=\s*")([^"]*)(")', re.I)
EMIS_RE = re.compile(r"emmisive\[\]\s*=\s*\{[^}]*\};", re.I)


CFGCONVERT = os.path.join("D:" + BS, "SteamLibrary", "steamapps", "common", "DayZ Tools", "Bin",
                          "CfgConvert", "CfgConvert.exe")


def read_rvmat_text(path):
    """Текст материала. Бинарный (rapified, начинается с 0 r a P) переводится CfgConvert'ом."""
    raw = open(path, "rb").read()
    if raw[:4] == bytes([0]) + b"raP":
        tmp = os.path.join(os.environ.get("TEMP", HERE), "vb_ab_rvmat")
        os.makedirs(tmp, exist_ok=True)
        dst = os.path.join(tmp, "derap.rvmat")
        subprocess.run([CFGCONVERT, "-txt", "-dst", dst, path], capture_output=True)
        raw = open(dst, "rb").read()
    return raw.decode("utf-8", errors="replace")


def write_rvmat(src_vp, dst_vp, mapping, emissive=None):
    src = to_disk(src_vp)
    txt = read_rvmat_text(src)
    low = {k.lower(): v for k, v in mapping.items()}

    def sub(m):
        t = m.group(2).lstrip(BS)
        return m.group(1) + low.get(t.lower(), m.group(2)) + m.group(3)

    txt = TEX_RE.sub(sub, txt)
    if emissive is not None:
        txt = EMIS_RE.sub("emmisive[]={%g,%g,%g,1};" % emissive, txt)
    dst = to_mod_file(dst_vp)
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    with open(dst, "w", encoding="utf-8", newline="\r\n") as f:
        f.write(txt.replace("\r\n", "\n"))
    missing = [m.group(2) for m in TEX_RE.finditer(txt)
               if is_ours(m.group(2).lstrip(BS))]
    return missing


# ------------------------------------------------------------------ сборка скина
def build_skin(code, skin, tmp, emissive=None):
    name, parts = MODELS[code]
    src_vp = vpath(parts)
    data = bytearray(open(to_disk(src_vp), "rb").read())
    paths = odol_paths(bytes(data))
    mapping = {}
    for old in paths:
        shared = is_shared(old)
        folder = code if shared else code + skin
        mapping[old] = new_path(old, folder)
    # уникальность новых имён
    seen = {}
    for old, new in mapping.items():
        if new in seen and seen[new].lower() != old.lower():
            raise RuntimeError("совпали новые пути: %s и %s -> %s" % (seen[new], old, new))
        seen[new] = old
    for old, offs in paths.items():
        nb = mapping[old].encode("latin1")
        for o in offs:
            data[o:o + len(nb)] = nb
    out_p3d = os.path.join(MOD, "m", "vb_%s_%s.p3d" % (name, skin))
    os.makedirs(os.path.dirname(out_p3d), exist_ok=True)
    with open(out_p3d, "wb") as f:
        f.write(bytes(data))

    problems = []
    for old, new in mapping.items():
        src = to_disk(old)
        dst = to_mod_file(new)
        ext = os.path.splitext(old)[1].lower()
        if not os.path.exists(src):
            problems.append("нет исходника: " + old)
            continue
        if ext == ".rvmat":
            for t in write_rvmat(old, new, mapping, emissive):
                problems.append("материал %s ссылается на %s, которого нет в модели" % (old, t))
        elif is_shared(old):
            if not os.path.exists(dst):
                os.makedirs(os.path.dirname(dst), exist_ok=True)
                shutil.copyfile(src, dst)
        else:
            recolor_copy(src, dst, skin, tmp)
    print("МОДЕЛЬ %s (%s, скин %s): путей %d, файл %s" % (name, code, skin, len(mapping), out_p3d))
    for p in problems:
        print("   !", p)
    return out_p3d, problems


# ------------------------------------------------------------------ трофеи (ТЗ 2.7)
# код модели босса -> имя и исходная модель трофея. Копия трофея кладёт текстуры в те же папки, что скин босса
# (t\<код><скин>\ и t\<код>\), поэтому ухо, глаз, челюсть и голова Энта ссылаются на уже перекрашенные
# текстуры скина — новые пути совпадают с путями скина байт в байт (new_path зависит только от исходного пути).
TROPHIES = {
    "en": ("ent_head", ["msfz_mifical_pack", "ent", "skining", "msfz_ent_head.p3d"]),
    "ri": ("ricar_head", ["msfz_mifical_pack", "dark_ricar", "skining", "msfz_ricar_head.p3d"]),
    "mo": ("mozgotras_ear", ["sfp3", "1_ai", "mozgot", "srez", "mzt_ear.p3d"]),
    "mr": ("mozgotras_red_ear", ["sfp3", "1_ai", "mozgot", "srez", "mzt_ear_red.p3d"]),
    "te": ("tenevnik_eye", ["sfp", "1_ai", "tenevnik", "srez", "tenevnik_eye.p3d"]),
    "sh": ("shum_jaw", ["sfp3", "1_ai", "shum_female", "srez", "shum_jaw.p3d"]),
    # у Мутаволка своего трофея в SFP нет — коготь медведя-зомби (своя текстура bear_zmb, ТЗ 2.7)
    "wo": ("mutawolf_claw", ["sfp3", "1_ai", "zmb_bear", "srez", "kogot_bear.p3d"]),
}
# общие карты (нормали, блики), которые нужны только трофею, ужимаются до этого размера отбрасыванием
# верхних мипмапов (данные не пережимаются): у когтя это карты всего медведя 2048
TROPHY_SHARED_MAX = {"wo": 1024}


def paa_trim(src, dst, max_dim):
    """Копия PAA без верхних мипмапов, пока сторона больше max_dim. Сжатые данные мипов копируются как есть,
    в заголовке пересчитывается тег OFFS (смещения мипмапов)."""
    d = open(src, "rb").read()
    i = 2
    tags = []
    while d[i:i + 4] == b"GGAT":
        ln = int.from_bytes(d[i + 8:i + 12], "little")
        tags.append((d[i + 4:i + 8], d[i + 12:i + 12 + ln]))
        i += 12 + ln
    npal = int.from_bytes(d[i:i + 2], "little")
    pal = d[i:i + 2 + npal * 3]
    i += 2 + npal * 3
    mips = []
    while i + 7 <= len(d):
        w = int.from_bytes(d[i:i + 2], "little")
        h = int.from_bytes(d[i + 2:i + 4], "little")
        if w == 0 and h == 0:
            break
        size = int.from_bytes(d[i + 4:i + 7], "little")
        mips.append(((w & 0x7FFF), h, d[i:i + 7 + size]))
        i += 7 + size
    tail = d[i:]
    while len(mips) > 1 and max(mips[0][0], mips[0][1]) > max_dim:
        mips.pop(0)
    head_len = 2 + sum(12 + len(t[1]) for t in tags) + len(pal)
    offs, pos = [], head_len
    for _, _, raw in mips:
        offs.append(pos)
        pos += len(raw)
    out = bytearray(d[:2])
    for name, data in tags:
        if name == b"SFFO":     # OFFS (имя тега в файле задом наперёд)
            data = b"".join(o.to_bytes(4, "little") for o in (offs + [0] * 16)[:len(data) // 4])
        out += b"GGAT" + name + len(data).to_bytes(4, "little") + data
    out += pal
    for _, _, raw in mips:
        out += raw
    out += tail
    os.makedirs(os.path.dirname(os.path.abspath(dst)), exist_ok=True)
    with open(dst, "wb") as f:
        f.write(bytes(out))
    return mips[0][0], mips[0][1]


def build_trophy(code, skin, tmp):
    """Копия модели трофея для скина: m\\vb_<трофей>_<скин>.p3d. Текстуры, которые уже сделал build_skin,
    не трогает; недостающие (только у трофея) перекрашивает или копирует сам."""
    name, parts = TROPHIES[code]
    data = bytearray(open(to_disk(vpath(parts)), "rb").read())
    paths = odol_paths(bytes(data))
    skin_parts = MODELS[code][1]
    skin_paths = {p.lower() for p in odol_paths(open(to_disk(vpath(skin_parts)), "rb").read())}
    mapping = {}
    for old in paths:
        mapping[old] = new_path(old, code if is_shared(old) else code + skin)
    seen = {}
    for old, new in mapping.items():
        if new in seen and seen[new].lower() != old.lower():
            raise RuntimeError("совпали новые пути: %s и %s -> %s" % (seen[new], old, new))
        seen[new] = old
    for old, offs in paths.items():
        nb = mapping[old].encode("latin1")
        for o in offs:
            data[o:o + len(nb)] = nb
    out_p3d = os.path.join(MOD, "m", "vb_%s_%s.p3d" % (name, skin))
    os.makedirs(os.path.dirname(out_p3d), exist_ok=True)
    with open(out_p3d, "wb") as f:
        f.write(bytes(data))

    problems = []
    own = []
    for old, new in mapping.items():
        src = to_disk(old)
        dst = to_mod_file(new)
        ext = os.path.splitext(old)[1].lower()
        from_skin = old.lower() in skin_paths and os.path.exists(dst)
        if not os.path.exists(src):
            problems.append("нет исходника: " + old)
            continue
        if from_skin:
            continue
        own.append(new)
        if ext == ".rvmat":
            for t in write_rvmat(old, new, mapping):
                problems.append("материал %s ссылается на %s, которого нет в модели" % (old, t))
        elif is_shared(old):
            if code in TROPHY_SHARED_MAX:
                paa_trim(src, dst, TROPHY_SHARED_MAX[code])
            else:
                os.makedirs(os.path.dirname(dst), exist_ok=True)
                shutil.copyfile(src, dst)
        else:
            recolor_copy(src, dst, skin, tmp)
    print("ТРОФЕЙ %s (%s, скин %s): путей %d, своих файлов %d, файл %s"
          % (name, code, skin, len(mapping), len(own), out_p3d))
    for p in problems:
        print("   !", p)
    return out_p3d, problems, own


def cmd_list():
    for code, (name, parts) in MODELS.items():
        data = open(to_disk(vpath(parts)), "rb").read()
        print("==", code, name, "ODOL v%d" % int.from_bytes(data[4:8], "little"))
        for old, offs in odol_paths(data).items():
            print("   %2d  %-75s %s" % (len(offs), old, new_path(old, code + ("" if is_shared(old) else "x"))))


def main():
    a = sys.argv[1:]
    if not a or a[0] == "list":
        cmd_list()
        return
    if a[0] == "build":
        code = a[1]
        tmp = os.path.join(os.environ.get("TEMP", HERE), "vb_ab_tex")
        bad = 0
        for skin in a[2:]:
            emissive = (3.0, 0.0, 0.0) if skin == "r" else None
            _, problems = build_skin(code, skin, tmp, emissive)
            bad += len(problems)
        sys.exit(1 if bad else 0)
    raise SystemExit("неизвестная команда: " + a[0])


if __name__ == "__main__":
    main()
