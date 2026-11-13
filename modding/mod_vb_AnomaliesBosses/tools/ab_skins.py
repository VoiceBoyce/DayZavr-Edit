# -*- coding: utf-8 -*-
"""Скины боссов и трофеи VB_AnomaliesBosses (ТЗ 4.4, 4.5, 4.6, 2.7).

Для каждого из 14 боссов стартового набора (модель x палитра):
  - ab_models.build_skin: копия модели m\\vb_<модель>_<палитра>.p3d, перекрашенные текстуры в t\\<код><палитра>\\
    (перекраска — ab_tex.recolor), общие карты нормалей и бликов — t\\<код>\\;
  - у Ричара — маска свечения прожилок и свой материал (шейдер Super, маска в Stage8) для секции camo;
  - ab_models.build_trophy: копия модели трофея на тех же текстурах скина m\\vb_<трофей>_<палитра>.p3d;
  - cfg\\bosses.hpp (классы VB_AB_<Модель>_<Палитра>) и cfg\\trophies.hpp (VB_AB_Trophy_<Модель>_<Палитра>);
  - превью в %TEMP%\\vb_ab_skins\\preview\\: <NN>_<класс>.png (все текстуры скина и трофея)
    и <NN>_<класс>_crop.png (фрагмент 1:1: слева исходник, справа перекраска).
Строки (имена, описания) — tools\\strings\\skins_trophies.json, в stringtable их собирает ab_strings.py.

Запуск (нужен numpy — Python из Blender):
  "G:\\SteamLibrary\\steamapps\\common\\Blender\\5.2\\python\\bin\\python.exe" ab_skins.py            все 14
  ... ab_skins.py ent_storm ricar_acid     только эти (конфиги всё равно пишутся целиком)
  ... ab_skins.py --cfg                    только cfg\\bosses.hpp и cfg\\trophies.hpp, без текстур и моделей
Конфиг переписывается, только если его содержимое изменилось (иначе файл не трогается).
"""
import json
import os
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)

import ab_models as M  # noqa: E402

BS = chr(92)
TMP = os.path.join(os.environ.get("TEMP", HERE), "vb_ab_skins")
TEX_TMP = os.path.join(TMP, "tex")
PREVIEW = os.path.join(TMP, "preview")
CFG = os.path.join(M.MOD, "cfg")
STRINGS = os.path.join(HERE, "strings", "skins_trophies.json")

# код модели -> класс в именах, значения по умолчанию (ТЗ 4.4), главная текстура (для превью), трофей (ТЗ 2.7)
MODELS = {
    "en": dict(cls="Ent", scale=1.6, health=8000, main="msfz_ent_base",
               weight=1200, size=(2, 2), stack=0),
    "ri": dict(cls="Ricar", scale=1.0, health=2500, main="msfz_ricar_body",
               weight=1200, size=(2, 2), stack=0),
    "mo": dict(cls="MozgoTras", scale=1.1, health=4500, main="mozgot_ca",
               weight=93, size=(1, 1), stack=10, rot=63),
    "mr": dict(cls="MozgoTrasRed", scale=1.1, health=5000, main="mozgot_red_ca",
               weight=93, size=(1, 1), stack=10, rot=63),
    "te": dict(cls="Tenevnik", scale=1.0, health=3000, main="zmb_z_e_co",
               weight=11, size=(1, 1), stack=10),
    "sh": dict(cls="Shum", scale=1.0, health=3500, main="shum_f_co",
               weight=11, size=(1, 1), stack=10),
    "wo": dict(cls="MutaWolf", scale=1.1, health=3500, main="mutawolf_co",
               weight=113, size=(1, 1), stack=10, rot=63),
}

# стартовый набор (ТЗ 4.5): номер, код модели, палитра (она же имя скина в путях, до 8 латинских букв)
SKINS = [
    (1, "en", "storm"),
    (2, "en", "acid"),
    (3, "mr", "fire"),
    (4, "mr", "gravity"),
    (5, "mo", "psi"),
    (6, "mo", "storm"),
    (7, "wo", "ice"),
    (8, "wo", "fire"),
    (9, "sh", "gravity"),
    (10, "sh", "ice"),
    (11, "te", "psi"),
    (12, "te", "rad"),
    (13, "ri", "acid"),
    (14, "ri", "rad"),
]

# Ричар: свечение прожилок через секцию camo (покрывает тело) и свой материал
RICAR_BODY = BS.join(["msfz_mifical_pack", "dark_ricar", "data", "msfz_ricar_body.paa"])
RICAR_BODY_RVMAT = BS.join(["msfz_mifical_pack", "dark_ricar", "data", "msfz_ricar_body.rvmat"])
RICAR_EMISSIVE = 1.6     # emmisive[] материала; яркость каждого пикселя задаёт маска


def skin_id(code, pal):
    return MODELS[code]["cls"] + "_" + pal.capitalize()


def cfg_path(vp):
    """vb_ab\\t\\... -> VB_AB\\t\\... для config.cpp."""
    return "VB_AB" + vp[len(M.PREFIX):]


def ricar_paths(pal):
    folder = "ri" + pal
    return dict(
        tex=M.new_path(RICAR_BODY, folder),
        body_rvmat=M.new_path(RICAR_BODY_RVMAT, folder),
        glow=BS.join([M.PREFIX, "t", folder, "vb_glow_e_co.paa"]),
        rvmat=BS.join([M.PREFIX, "t", folder, "vb_glow.rvmat"]),
    )


def build_ricar_glow(pal):
    """Маска свечения тела Ричара (те же прожилки, что на цветовой текстуре) и материал со Stage8."""
    import ab_tex
    p = ricar_paths(pal)
    ab_tex.glow_mask(M.to_disk(RICAR_BODY), M.to_mod_file(p["glow"]), pal, TEX_TMP,
                     M.paa_to_tga, M.png_to_paa, size=1024)
    # материал тела скина (его уже написал build_skin: нормали и блики ведут в t\ri\) + свечение
    with open(M.to_mod_file(p["body_rvmat"]), encoding="utf-8") as f:
        txt = f.read()
    txt = M.EMIS_RE.sub("emmisive[]={%g,%g,%g,1};" % ((RICAR_EMISSIVE,) * 3), txt)
    txt = txt.rstrip() + "\n" + "\n".join([
        "class Stage8",
        "{",
        '\ttexture="%s";' % p["glow"],
        '\tuvSource="tex";',
        "\tclass uvTransform",
        "\t{",
        "\t\taside[]={1,0,0};",
        "\t\tup[]={0,1,0};",
        "\t\tdir[]={0,0,0};",
        "\t\tpos[]={0,0,0};",
        "\t};",
        "};",
    ]) + "\n"
    with open(M.to_mod_file(p["rvmat"]), "w", encoding="utf-8", newline="\r\n") as f:
        f.write(txt)


# ------------------------------------------------------------------ конфиги
def write_if_changed(path, text):
    """Файл переписывается, только если содержимое другое (иначе не трогаем и дату изменения)."""
    data = text.replace("\r\n", "\n").replace("\n", "\r\n").encode("utf-8")
    if os.path.exists(path):
        with open(path, "rb") as f:
            if f.read() == data:
                print("   без изменений:", os.path.basename(path))
                return False
    with open(path, "wb") as f:
        f.write(data)
    print("   записан:", os.path.basename(path))
    return True


def write_bosses():
    L = ["// Скины боссов (ТЗ 4.5): модель x палитра. Генерирует tools/ab_skins.py, руками не править.",
         "// vbABScale и vbABHealth — значения модели по умолчанию (ТЗ 4.4), bosses.json их переопределяет.",
         "// vbABPalette — палитра скина: по ней клиент выбирает ауру (scripts/4_World/VB_AB/VB_AB_Aura.c)."]
    for n, code, pal in SKINS:
        m = MODELS[code]
        sid = skin_id(code, pal)
        L += ["class VB_AB_%s: VB_AB_BossBase" % sid,
              "{",
              "\tscope = 2;",
              '\tdisplayName = "$STR_VB_AB_Boss_%s";' % sid,
              '\tmodel = "VB_AB\\m\\vb_%s_%s.p3d";' % (M.MODELS[code][0], pal),
              "\tvbABScale = %g;" % m["scale"],
              "\tvbABHealth = %d;" % m["health"],
              '\tvbABPalette = "%s";' % pal]
        if code == "ri":
            p = ricar_paths(pal)
            L += ["\t// секция camo покрывает тело: свой материал со свечением прожилок (Stage8)",
                  '\thiddenSelections[] = {"camo"};',
                  '\thiddenSelectionsTextures[] = {"%s"};' % cfg_path(p["tex"]),
                  '\thiddenSelectionsMaterials[] = {"%s"};' % cfg_path(p["rvmat"])]
        L += ["};"]
    write_if_changed(os.path.join(CFG, "bosses.hpp"), "\n".join(L) + "\n")


def write_trophies():
    L = ["// Трофеи боссов (ТЗ 2.7): копии трофеев SFP/MSFZ на текстурах скина босса.",
         "// Генерирует tools/ab_skins.py, руками не править. Не еда, не портятся; мелкие — в стопку до 10.",
         "class VB_AB_TrophyBase: Inventory_Base",
         "{",
         "\tscope = 0;",
         "\tclass DamageSystem",
         "\t{",
         "\t\tclass GlobalHealth",
         "\t\t{",
         "\t\t\tclass Health",
         "\t\t\t{",
         "\t\t\t\thitpoints = 100;",
         "\t\t\t\thealthLevels[] = {{1.0,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.0,{}}};",
         "\t\t\t};",
         "\t\t};",
         "\t};",
         "};",
         "// стопка как у ванильного BurlapStrip/Bone: количество = число штук, вес = weight x количество",
         "class VB_AB_TrophyStackBase: VB_AB_TrophyBase",
         "{",
         "\tscope = 0;",
         "\tcanBeSplit = 1;",
         "\tvarQuantityInit = 1;",
         "\tvarQuantityMin = 0;",
         "\tvarQuantityMax = 10;",
         "\tvarQuantityDestroyOnMin = 1;",
         "};"]
    for n, code, pal in SKINS:
        m = MODELS[code]
        sid = skin_id(code, pal)
        base = "VB_AB_TrophyStackBase" if m["stack"] else "VB_AB_TrophyBase"
        L += ["class VB_AB_Trophy_%s: %s" % (sid, base),
              "{",
              "\tscope = 2;",
              '\tdisplayName = "$STR_VB_AB_Trophy_%s";' % sid,
              '\tdescriptionShort = "$STR_VB_AB_Trophy_%s_Desc";' % sid,
              '\tmodel = "VB_AB\\m\\vb_%s_%s.p3d";' % (M.TROPHIES[code][0], pal),
              "\tweight = %d;" % m["weight"],
              "\titemSize[] = {%d,%d};" % m["size"]]
        if m["stack"] and m["stack"] != 10:
            L += ["\tvarQuantityMax = %d;" % m["stack"]]
        if m.get("rot"):
            L += ["\trotationFlags = %d;" % m["rot"]]
        L += ["};"]
    write_if_changed(os.path.join(CFG, "trophies.hpp"), "\n".join(L) + "\n")


def check_strings():
    with open(STRINGS, encoding="utf-8") as f:
        have = json.load(f)
    need = []
    for n, code, pal in SKINS:
        sid = skin_id(code, pal)
        need += ["STR_VB_AB_Boss_" + sid, "STR_VB_AB_Trophy_" + sid, "STR_VB_AB_Trophy_%s_Desc" % sid]
    miss = [k for k in need if k not in have]
    for k in miss:
        print("   ! нет строки", k)
    return not miss


# ------------------------------------------------------------------ превью и размер
def _fit(img, cell):
    import numpy as np
    h, w = img.shape[:2]
    k = max(h, w) / float(cell)
    ys = (np.arange(int(h / k)) * k).astype(int)
    xs = (np.arange(int(w / k)) * k).astype(int)
    return img[ys][:, xs]


def write_preview(n, code, pal, dsts):
    import numpy as np
    import ab_tex
    os.makedirs(PREVIEW, exist_ok=True)
    items = [ab_tex.RESULTS[d] for d in dsts if d in ab_tex.RESULTS]
    if not items:
        return []
    cell, cols = 384, 4
    rows = (len(items) + cols - 1) // cols
    sheet = np.full((rows * cell, cols * cell, 3), 32, np.uint8)
    for k, it in enumerate(items):
        im = _fit(it["thumb"][..., :3], cell)
        r, c = divmod(k, cols)
        sheet[r * cell:r * cell + im.shape[0], c * cell:c * cell + im.shape[1]] = im
    base = os.path.join(PREVIEW, "%02d_%s" % (n, skin_id(code, pal)))
    ab_tex.write_png(base + ".png", sheet)
    out = [base + ".png"]
    main = [it for it in items if it["stem"] == MODELS[code]["main"]]
    if main and main[0].get("crop") is not None:
        a, b = main[0]["orig_crop"], main[0]["crop"]
        gap = np.full((a.shape[0], 8, 3), 32, np.uint8)
        ab_tex.write_png(base + "_crop.png", np.concatenate([a, gap, b], 1))
        out.append(base + "_crop.png")
    return out


def skin_files(code, pal):
    """Файлы скина и трофея: модели, папка t\\<код><палитра>\\ и общие карты, нужные только трофею."""
    files = [os.path.join(M.MOD, "m", "vb_%s_%s.p3d" % (M.MODELS[code][0], pal)),
             os.path.join(M.MOD, "m", "vb_%s_%s.p3d" % (M.TROPHIES[code][0], pal))]
    folder = os.path.join(M.MOD, "t", code + pal)
    if os.path.isdir(folder):
        files += [os.path.join(folder, f) for f in os.listdir(folder)]
    return [f for f in files if os.path.exists(f)]


def size_report(trophy_shared):
    total = 0
    print("\nРАЗМЕР (модели + цветовые текстуры скина и трофея):")
    for n, code, pal in SKINS:
        mb = sum(os.path.getsize(f) for f in skin_files(code, pal)) / 1048576.0
        total += mb
        print("   %2d  %-24s %6.2f МБ" % (n, skin_id(code, pal), mb))
    extra = sum(os.path.getsize(f) for f in set(trophy_shared) if os.path.exists(f)) / 1048576.0
    total += extra
    print("   общие карты, нужные только трофеям: %.2f МБ" % extra)
    print("   ВСЕГО: %.1f МБ" % total)
    return total


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    only_cfg = "--cfg" in sys.argv
    want = {a.lower() for a in args}
    todo = [s for s in SKINS if not want or skin_id(s[1], s[2]).lower() in want
            or "%s_%s" % (MODELS[s[1]]["cls"].lower(), s[2]) in want or str(s[0]) in want]
    for _, code, pal in SKINS:
        if len(pal) > 8 or not pal.isalpha() or not pal.isascii():
            raise SystemExit("имя скина должно быть до 8 латинских букв: " + pal)
    bad = 0
    trophy_shared = []
    previews = []
    if not only_cfg:
        os.makedirs(TEX_TMP, exist_ok=True)
        for n, code, pal in todo:
            t0 = time.time()
            print("\n== %2d %s" % (n, skin_id(code, pal)))
            _, problems = M.build_skin(code, pal, TEX_TMP)
            bad += len(problems)
            if code == "ri":
                build_ricar_glow(pal)
            _, problems, own = M.build_trophy(code, pal, TEX_TMP)
            bad += len(problems)
            trophy_shared += [M.to_mod_file(v) for v in own if v.split(BS)[2] == code]
            folder = os.path.join(M.MOD, "t", code + pal)
            dsts = sorted(os.path.join(folder, f) for f in os.listdir(folder))
            previews += write_preview(n, code, pal, dsts)
            print("   готово за %.0f с" % (time.time() - t0))
    write_bosses()
    write_trophies()
    print("\ncfg: %s, %s" % (os.path.join(CFG, "bosses.hpp"), os.path.join(CFG, "trophies.hpp")))
    if not check_strings():
        bad += 1
    if not only_cfg:
        # общие карты когтя Мутаволка (t\wo\bear_*) нужны только трофею
        wo = os.path.join(M.MOD, "t", "wo")
        trophy_shared += [os.path.join(wo, f) for f in os.listdir(wo) if f.startswith("bear_zmb")]
        size_report(trophy_shared)
        print("\nПРЕВЬЮ:")
        for p in previews:
            print("   " + p)
    if bad:
        print("\nЕСТЬ ПРОБЛЕМЫ: %d (см. «!» выше)" % bad)
    sys.exit(1 if bad else 0)


if __name__ == "__main__":
    main()
