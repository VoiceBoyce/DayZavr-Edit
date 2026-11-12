# -*- coding: utf-8 -*-
"""Детекторы Око, Медведь, Сварог и Велес для VB_AnomaliesBosses: модели, текстуры, материалы и cfg/detectors.hpp.

Что делает:
  1) модели OOTS через ab_models.build_skin (скин x): m/vb_detector_<имя>_x.p3d, t/<код>/ (общие карты),
     t/<код>x/ (цвет и материалы, бинарные .rvmat переводятся в текст);
  2) то, чего нет внутри моделей, но нужно скриптам (VB_AB_Detectors2.c):
     - Медведь: экраны «пусто / слабый / средний / сильный» (t/dmx/ekran_*.paa) и негорящий материал экрана
       medved_ekranoff.rvmat (в OOTS он указан в конфиге, но файла нет);
     - Сварог, Велес: материал «индикаторы горят» для секции emiss (t/dsx/svarog_emiss_on.rvmat,
       t/dvx/veles_emiss_on.rvmat; в OOTS SvarogEmiss/VelesEmiss указаны, но файлов нет);
  3) cfg/detectors.hpp — классы VB_AB_Detector_Oko/_Medved/_Svarog/_Veles (наследники VB_AB_DetectorBase).

Какая секция модели чем покрыта, выяснено разбором секций ODOL (раздел «Секции» ниже). Скрипт читает
из конфига поля vbAB<часть>Sel / vbAB<часть><состояние>Tex|Mat, поэтому пути живут только здесь.

Запуск: python -I tools/ab_detectors.py
"""
import os
import re
import shutil
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import ab_models as am  # noqa: E402

BS = chr(92)
MOD = am.MOD
OOTS_DET = os.path.join(am.ROOTS["oots_anomaly_client"], "data", "objects", "Detectors")
OUT_HPP = os.path.join(MOD, "cfg", "detectors.hpp")
DESTRUCT = "DZ" + BS + "data" + BS + "data" + BS + "default_destruct.rvmat"

# Секции моделей (разбор ODOL v56, LOD 0):
#   Око:     lampa = okoon_co + oko_lampalight (горит), ecran = okooff_co + oko, корпус okoon_co + oko,
#            стекло oko_ca + oko_glass;
#   Медведь: ekran = ekranoff + medved_ekranon (крутится анимацией ekran 0..180°), display = ekranoff без
#            материала, ekrangloss = eckangloss_ca + medved_adef, стекло glass_ca + medved_glass;
#   Сварог:  ekran = svadog_ekranon.paa + svadog_ekranon.rvmat (экран включён), emiss = svarog_default_co +
#            svarogmat, метки art1..4 = зелёный цвет + svadog_light;
#   Велес:   ekran = veles_ekranon.paa + velesmat, emiss = veles_default_co + velesmat,
#            метки art1..4 = veles_ekrangreen + veles_light.
# Анимации меток Сварога/Велеса: art*_rotate -90..+90°, art*_translate 0..1, art*_hide (1 — скрыта).

CODES = ["dk", "dm", "ds", "dv"]


def mapping_for(code):
    """Старый путь -> новый путь (как в ab_models.build_skin, скин x)."""
    name, parts = am.MODELS[code]
    data = open(am.to_disk(am.vpath(parts)), "rb").read()
    res = {}
    for old in am.odol_paths(data):
        folder = code if am.is_shared(old) else code + "x"
        res[old] = am.new_path(old, folder)
    return res


def cfg_path(vp):
    """vb_ab\\t\\... -> VB_AB\\t\\... (так пути записаны у Отклика)."""
    return "VB_AB" + vp[len(am.PREFIX):]


class Model:
    def __init__(self, code):
        self.code = code
        self.map = mapping_for(code)

    def p(self, filename):
        """Новый путь файла модели по имени исходного файла (регистр не важен)."""
        fn = filename.lower()
        hits = [new for old, new in self.map.items() if old.lower().split(BS)[-1] == fn]
        if len(hits) != 1:
            raise SystemExit("%s: файл %s в модели найден %d раз" % (self.code, filename, len(hits)))
        return cfg_path(hits[0])

    def extra(self, filename):
        """Свой файл в папке t\\<код>x (не из модели)."""
        return "VB_AB" + BS + "t" + BS + self.code + "x" + BS + filename


def mod_file(cfgp):
    parts = cfgp.split(BS)
    return os.path.join(MOD, *parts[1:])


# ------------------------------------------------------------------ дополнительные ресурсы
def copy_paa(src_rel, dst_cfg):
    src = os.path.join(OOTS_DET, *src_rel.split("/"))
    dst = mod_file(dst_cfg)
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    shutil.copyfile(src, dst)


def rvmat_variant(src_cfg, dst_cfg, values):
    """Копия текстового материала с заменой параметров освещения: values = {"emmisive": "0,0,0,1", ...}."""
    txt = open(mod_file(src_cfg), encoding="utf-8").read()
    for key, val in values.items():
        txt, n = re.subn(r"(?m)^%s\[\]\s*=\s*\{[^}]*\};" % re.escape(key), "%s[]={%s};" % (key, val), txt)
        if n != 1:
            raise SystemExit("в %s нет строки %s[]" % (src_cfg, key))
    with open(mod_file(dst_cfg), "w", encoding="utf-8", newline="\r\n") as f:
        f.write(txt.replace("\r\n", "\n"))


# ------------------------------------------------------------------ конфиг
def arr(items):
    return "{" + ", ".join('"%s"' % x for x in items) + "}"


def health(mats):
    rows = []
    for lvl in ("1.0", "0.7", "0.5", "0.3"):
        rows.append("\t\t\t\t\t\t{%s, %s}" % (lvl, arr(mats)))
    rows.append("\t\t\t\t\t\t{0.0, %s}" % arr([DESTRUCT] * len(mats)))
    return [
        "\t\tclass DamageSystem",
        "\t\t{",
        "\t\t\tclass GlobalHealth",
        "\t\t\t{",
        "\t\t\t\tclass Health",
        "\t\t\t\t{",
        "\t\t\t\t\thitpoints = 150;",
        "\t\t\t\t\thealthLevels[] =",
        "\t\t\t\t\t{",
        ",\n".join(rows),
        "\t\t\t\t\t};",
        "\t\t\t\t};",
        "\t\t\t};",
        "\t\t};",
    ]


def anim_sources(extra):
    out = ["\t\tclass AnimationSources: AnimationSources", "\t\t{"]
    for name, period, init in extra:
        out += ["\t\t\tclass %s" % name, "\t\t\t{", '\t\t\t\tsource = "user";',
                "\t\t\t\tanimPeriod = %g;" % period, "\t\t\t\tinitPhase = %g;" % init, "\t\t\t};"]
    out.append("\t\t};")
    return out


def det_class(cls, comment, model, sels, texs, mats, fields, health_mats, anims=None):
    out = ["\t// " + comment,
           "\tclass VB_AB_Detector_%s: VB_AB_DetectorBase" % cls,
           "\t{",
           "\t\tscope = 2;",
           '\t\tdisplayName = "$STR_VB_AB_Detector_%s";' % cls,
           '\t\tdescriptionShort = "$STR_VB_AB_Detector_%s_Desc";' % cls,
           '\t\tmodel = "%s";' % model,
           "\t\thiddenSelections[] = %s;" % arr(sels),
           "\t\thiddenSelectionsTextures[] =",
           "\t\t{",
           ",\n".join("\t\t\t\"%s\"" % t for t in texs),
           "\t\t};",
           "\t\thiddenSelectionsMaterials[] =",
           "\t\t{",
           ",\n".join("\t\t\t\"%s\"" % m for m in mats),
           "\t\t};"]
    for key, val in fields:
        if isinstance(val, int):
            out.append("\t\t%s = %d;" % (key, val))
        elif isinstance(val, list):
            out.append("\t\t%s[] =" % key)
            out.append("\t\t{")
            out.append(",\n".join("\t\t\t\"%s\"" % v for v in val))
            out.append("\t\t};")
        else:
            out.append('\t\t%s = "%s";' % (key, val))
    if anims:
        out += anim_sources(anims)
    out += health(health_mats)
    out.append("\t};")
    return out


def main():
    tmp = os.path.join(os.environ.get("TEMP", HERE), "vb_ab_tex")
    bad = 0
    for code in CODES:
        _, problems = am.build_skin(code, "x", tmp)
        bad += len(problems)
    if bad:
        raise SystemExit("модели перенесены с ошибками, cfg/detectors.hpp не записан")

    ok, md, sv, vl = (Model(c) for c in CODES)

    # Медведь: экраны по силе сигнала и негорящий материал экрана
    ekr = {}
    for state, src in (("empty", "EkranEmpty"), ("low", "EkranLow"), ("mid", "EkranMid"), ("high", "EkranHigh")):
        ekr[state] = md.extra("ekran_%s.paa" % state)
        copy_paa("Medved/date/%s.paa" % src, ekr[state])
    md_on = md.p("medved_ekranon.rvmat")
    md_off = md.extra("medved_ekranoff.rvmat")
    rvmat_variant(md_on, md_off, {"ambient": "1,1,1,1", "diffuse": "1,1,1,1",
                                  "forcedDiffuse": "0,0,0,1", "emmisive": "0,0,0,1"})
    # Сварог, Велес: индикаторы emiss горят, пока детектор работает
    sv_emiss = sv.extra("svarog_emiss_on.rvmat")
    rvmat_variant(sv.p("svarogmat.rvmat"), sv_emiss, {"emmisive": "0.3,1.2,0.3,1"})
    vl_emiss = vl.extra("veles_emiss_on.rvmat")
    rvmat_variant(vl.p("velesmat.rvmat"), vl_emiss, {"emmisive": "1.2,0.7,0.15,1"})

    L = ["// СГЕНЕРИРОВАНО tools/ab_detectors.py — руками не править",
         "// Детекторы Око, Медведь, Сварог, Велес (ТЗ 2.2). Модели OOTS с заменёнными путями, логика в",
         "// scripts/4_World/VB_AB/VB_AB_Detectors2.c. Поля vbAB<часть>Sel — индекс секции в hiddenSelections,",
         "// vbAB<часть><On|Off>Tex/Mat — что кладёт на неё скрипт во включённом и выключенном состоянии."]

    # Око: экран горит, пока детектор включён; лампа мигает тем чаще, чем ближе находка
    L += det_class(
        "Oko", "Око: как Отклик (только расстояние, только впереди), лампа мигает, экран горит",
        "VB_AB" + BS + "m" + BS + "vb_detector_oko_x.p3d",
        ["Lampa", "Ecran"],
        [ok.p("okoon_co.paa"), ok.p("okooff_co.paa")],
        [ok.p("oko.rvmat"), ok.p("oko.rvmat")],
        [("vbABLampSel", 0), ("vbABLampOnMat", ok.p("oko_lampalight.rvmat")), ("vbABLampOffMat", ok.p("oko.rvmat")),
         ("vbABScreenSel", 1),
         ("vbABScreenOnTex", ok.p("okoon_co.paa")), ("vbABScreenOnMat", ok.p("oko_lampalight.rvmat")),
         ("vbABScreenOffTex", ok.p("okooff_co.paa")), ("vbABScreenOffMat", ok.p("oko.rvmat"))],
        [ok.p("oko.rvmat"), ok.p("oko_glass.rvmat"), ok.p("oko_lampalight.rvmat")])
    L.append("")

    # Медведь: экран по силе сигнала, сектор направления — поворотом экрана (анимация ekran)
    L += det_class(
        "Medved", "Медведь: расстояние (картинка экрана) и направление по секторам 15° (поворот экрана)",
        "VB_AB" + BS + "m" + BS + "vb_detector_medved_x.p3d",
        ["zbytek", "glass", "ekrangloss", "ekran", "display"],
        [md.p("medved_default_co.paa"), md.p("glass_defaultmaterial_ca.paa"),
         md.p("medved_defaulteckangloss_ca.paa"), md.p("ekranoff.paa"), md.p("ekranoff.paa")],
        [md.p("medved_adef.rvmat"), md.p("medved_glass.rvmat"), md.p("medved_adef.rvmat"), md_off],
        [("vbABScreenSel", 3), ("vbABScreenOnMat", md_on),
         ("vbABScreenOffTex", md.p("ekranoff.paa")), ("vbABScreenOffMat", md_off),
         ("vbABDisplaySel", 4),
         ("vbABScreenRangeTex", [ekr["empty"], ekr["low"], ekr["mid"], ekr["high"]])],
        [md.p("medved_adef.rvmat"), md.p("medved_glass.rvmat"), md_on, md_off],
        [("ekran", 0.65, 0.5)])
    L.append("")

    multi_anims = []
    for i in range(1, 5):
        multi_anims += [("art%d_translate" % i, 1, 0), ("art%d_rotate" % i, 1, 0.5), ("art%d_hide" % i, 0.1, 1)]

    # Сварог и Велес: до 4 меток на экране
    L += det_class(
        "Svarog", "Сварог: до 4 находок сразу, расстояние и направление к каждой — метками на экране",
        "VB_AB" + BS + "m" + BS + "vb_detector_svarog_x.p3d",
        ["zbytek", "glass", "emiss", "ekran"],
        [sv.p("svarog_default_co.paa"), sv.p("glass_defaultmaterial_ca.paa"), sv.p("svarog_default_co.paa"),
         sv.p("svarog_default_co.paa")],
        [sv.p("svarogmat.rvmat"), sv.p("svarogglass.rvmat"), sv.p("svarogmat.rvmat"), sv.p("svarogmat.rvmat")],
        [("vbABEmissSel", 2), ("vbABEmissOnMat", sv_emiss), ("vbABEmissOffMat", sv.p("svarogmat.rvmat")),
         ("vbABScreenSel", 3),
         ("vbABScreenOnTex", sv.p("svadog_ekranon.paa")), ("vbABScreenOnMat", sv.p("svadog_ekranon.rvmat")),
         ("vbABScreenOffTex", sv.p("svarog_default_co.paa")), ("vbABScreenOffMat", sv.p("svarogmat.rvmat"))],
        [sv.p("svarogmat.rvmat"), sv.p("svarogglass.rvmat"), sv.p("svadog_ekranon.rvmat"),
         sv.p("svadog_light.rvmat"), sv_emiss],
        multi_anims)
    L.append("")

    L += det_class(
        "Veles", "Велес: то же, что Сварог, другая модель (метки оранжевые)",
        "VB_AB" + BS + "m" + BS + "vb_detector_veles_x.p3d",
        ["zbytek", "glass", "emiss", "ekran"],
        [vl.p("veles_default_co.paa"), vl.p("glass_defaultmaterial_ca.paa"), vl.p("veles_default_co.paa"),
         vl.p("veles_default_co.paa")],
        [vl.p("velesmat.rvmat"), vl.p("svarogglass.rvmat"), vl.p("velesmat.rvmat"), vl.p("velesmat.rvmat")],
        [("vbABEmissSel", 2), ("vbABEmissOnMat", vl_emiss), ("vbABEmissOffMat", vl.p("velesmat.rvmat")),
         ("vbABScreenSel", 3),
         ("vbABScreenOnTex", vl.p("veles_ekranon.paa")), ("vbABScreenOnMat", vl.p("veles_light.rvmat")),
         ("vbABScreenOffTex", vl.p("veles_default_co.paa")), ("vbABScreenOffMat", vl.p("velesmat.rvmat"))],
        [vl.p("velesmat.rvmat"), vl.p("svarogglass.rvmat"), vl.p("veles_light.rvmat"), vl_emiss],
        multi_anims)

    with open(OUT_HPP, "w", encoding="utf-8", newline="\r\n") as f:
        f.write("\n".join(L) + "\n")
    print("записан", OUT_HPP)
    import ab_trim                  # карты 2048 -> 1024: детектору в руках хватает
    ab_trim.trim_detectors()


if __name__ == "__main__":
    main()
