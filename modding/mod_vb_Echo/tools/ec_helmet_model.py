# -*- coding: utf-8 -*-
r"""Сборка модели шлема экзо Эхо (IMP RoninHeavy, мужская и женская) — этап 3б, по одобренному концепту 11-23.

  python ec_helmet_model.py            (Python из Blender 5.3: нужен numpy)

Вход — оригиналы ODOL IMP (modding/mod_vb_Echo/исходники_моделей/RoninHeavy(_f).odol.p3d, в мод не входят).
Выход — MLOD VB_EchoClothes/exo_set/helmet/ronin_heavy/models/RoninHeavy(_f).p3d + model.cfg; бинаризует
pbo.ps1 -Binarize (RoninHeavy_g.p3d — модель на земле — остаётся ODOL как есть).

* Модель защищена: таблицы адресов LOD нет, читается только видимый LOD (odol_read.scan_lod_starts). Остальные два —
  1100 (вид от первого лица) и геометрия — у ванильных шлемов на голове (Mich, хоккейный, мото) ПУСТЫЕ (геометрия —
  только свойство autocenter=0): такими их здесь и создаём (проверено по P:\DZ\characters\headgear 11-23).
* Свет (выделение vbglow, секция model.cfg и hiddenSelections конфига; скрипт VB_EC_Exo_Helmet_Base зажигает его
  вместе со стёклами от сервопривода): ленты в прорезях дыхательной решётки и индикаторы на боковых рельсах —
  ec_helmet_geom (тот же код строил концепт). Веса — кость head от ближайших вершин.
* Знак Эхо на висках — краской в текстуре (ec_retex.py helmet), модель для него не трогается.
"""
import os
import shutil
import sys
import tempfile

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.stdout.reconfigure(encoding="utf-8")
import ec_servo_model as S  # noqa: E402  (общие пути, LodEditor, odol2mlod, py3d, текстуры света)
import ec_helmet_geom as HG  # noqa: E402

O, M2, py3d = S.O, S.M2, S.py3d
SRC_DIR = os.path.join(HERE, "..", "исходники_моделей")
OUTDIR = os.path.join(HERE, "..", "VB_EchoClothes", "exo_set", "helmet", "ronin_heavy", "models")
CFG = os.path.join(OUTDIR, "model.cfg")
MODELS = ["RoninHeavy", "RoninHeavy_f"]
SECTIONS = ["camoflage", "glass", "vbglow"]


def build(name, tex):
    src = os.path.join(SRC_DIR, name + ".odol.p3d")
    data = open(src, "rb").read()
    ver, n, res, p = O.header(data)
    skel = M2.parse_skeleton(data, p)
    bones = skel["bones"]
    od = O.read_visual_lod(src, 0)
    assert len(od["proxies"]) == 2, "прокси фонаря и ПНВ не прочитаны"
    W = O.read_bone_weights(data, od)
    assert W, "у видимого LOD нет весов костей"
    lod, nf = M2.lod_to_mlod(od, bones, None, W)
    ed = S.LodEditor(lod, od, W, bones)
    P = ed.P
    allv = np.arange(len(P))
    used = sorted({v for fi in ed.mlod_face for v in od["faces"][fi]})     # без точек треугольников прокси (у
    Pu = P[used]                                                           # фонаря есть точка на x = 1,09)
    # обход новых граней — по геометрии оболочки: нормали лицевой пластины IMP смотрят внутрь, по ним обход выходил
    # наоборот, и плоские ленты прорезей игра не рисовала (11-24, стенд)
    w_norm = ed.winding
    ed.winding, share = ed.winding_outward(
        (Pu.min(0) + Pu.max(0)) / 2,
        pick=lambda c: abs(c[0]) < 0.06 and 1.53 < c[1] < 1.64 and c[2] < -0.10,       # лицевая пластина, решётка
        direction=(0.0, 0.0, -1.0))                                                    # смотрит к лицу (-z)
    print("   обход: по нормалям %+d, по геометрии %+d (%.0f%% граней пластины), берём по геометрии" % (
        w_norm, ed.winding, share * 100))
    assert share > 0.65, "обход лицевой пластины не определён"

    def near(mesh, r=0.06):
        c = np.mean(mesh.pts, axis=0)
        return set(allv[np.linalg.norm(P - c, axis=1) < r].tolist()) or set(allv.tolist())
    vents, slots = HG.grill_glow_meshes(src, tex)
    nv = sum(ed.add_mesh(m, S.TEX_GLOW, S.MAT_GLOW, "vbglow", near(m)) for m in vents)
    rails = HG.rail_indicators(Pu)
    nr = sum(ed.add_mesh(m, S.TEX_GLOW, S.MAT_GLOW, "vbglow", near(m)) for m in rails)
    m = py3d.P3D()
    m.lods.append(lod)
    view = py3d.LOD()                                  # 1100 «вид от первого лица» — пустой, как у ванили
    view.resolution = 1100.0
    m.lods.append(view)
    geo = py3d.LOD()                                   # геометрия — пустая, как у ванили (только autocenter)
    geo.resolution = 1e13
    geo.properties["autocenter"] = "0"
    m.lods.append(geo)
    dst = os.path.join(OUTDIR, name + ".p3d")
    with tempfile.TemporaryDirectory() as td:          # сохранение целиком, потом замена (не оставить половину файла)
        tmp = os.path.join(td, name + ".p3d")
        m.save(tmp)
        shutil.move(tmp, dst)                          # TEMP на C:, мод на D: — os.replace между дисками не умеет
    print("%s: LOD %g, граней %d + прорези %d (%d граней) + индикаторы %d (%d граней), прокси %s -> %s" % (
        name, od["res"], nf, len(vents), nv, len(rails), nr, [px["name"].split(chr(92))[-1] for px in od["proxies"]],
        dst))
    return skel


def write_cfg(skel):
    L = ["// Шлем экзо Эхо: собрано tools/ec_helmet_model.py из ODOL IMP (odol2mlod с весами костей)",
         "class CfgSkeletons\n{\n\tclass Default\n\t{\n\t\tisDiscrete = 1;\n\t\tskeletonInherit = \"\";"
         "\n\t\tskeletonBones[] = {};\n\t};",
         "\tclass %s: Default\n\t{\n\t\tisDiscrete = %d;\n\t\tskeletonBones[] =\n\t\t{" % (skel["name"],
                                                                                     skel["discrete"]),
         ",\n".join("\t\t\t\"%s\", \"%s\"" % bp for bp in skel["bones"]),
         "\t\t};\n\t};\n};",
         "class CfgModels\n{\n\tclass Default\n\t{\n\t\tsectionsInherit = \"\";\n\t\tsections[] = {};"
         "\n\t\tskeletonName = \"\";\n\t};",
         "\tclass RoninHeavy: Default\n\t{\n\t\tskeletonName = \"%s\";\n\t\tsections[] = {%s};\n\t};" % (
             skel["name"], ", ".join("\"%s\"" % s for s in SECTIONS)),
         "\tclass RoninHeavy_f: RoninHeavy {};",
         "};"]
    with open(CFG, "w", encoding="utf-8") as f:
        f.write("\n".join(L) + "\n")
    print("model.cfg ->", CFG)


def main():
    tex = HG.helmet_texture()
    skel = None
    for name in MODELS:
        skel = build(name, tex)
    write_cfg(skel)


if __name__ == "__main__":
    main()
