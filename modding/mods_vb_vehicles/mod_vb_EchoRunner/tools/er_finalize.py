# -*- coding: utf-8 -*-
r"""Итоговые модели мода VB_EchoRunner из work/<вариант>/full (этап 6).

  blender.exe -b --factory-startup --python er_finalize.py -- <вариант>
  выход: VB_EchoRunner/vb_echorunner.p3d, VB_EchoRunner/proxy/vb_er_*.p3d, VB_EchoRunner/model.cfg

  * прокси ALEX -> наши пути (\vb_echorunner\proxy\vb_er_wheel, vb_er_door_driver, vb_er_door_codriver, vb_er_hood);
  * Geometry / Fire / View Geometry: выпуклые коробки расширителей (по 3 на арку) и люстры — новые компоненты;
  * массы деталей (перевод ALEX раскладывал 1800 кг на каждую деталь — ошибка, см. скилл dayzavr-pbo);
  * model.cfg: скелет и анимации из перевода ALEX (класс vb_echorunner), секции + neon, screen; классы деталей.
"""
import os
import re
import shutil
import sys

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import er_common as C  # noqa: E402
from er_geom import Part, V, box, part_data, append  # noqa: E402

py3d = C.py3d
args = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
VAR = [a for a in args if not a.startswith("--")][0]
MOD = os.path.dirname(HERE)
FULL = os.path.join(MOD, "work", VAR, "full")
WD = os.path.join(MOD, "work", VAR)
SRC = os.path.join(MOD, "VB_EchoRunner")
PROXY = r"\vb_echorunner\proxy" + "\\"
RENAME = {   # прокси ALEX -> наши файлы
    r"\interceptor_byalex\proxy\interceptor_wheel": PROXY + "vb_er_wheel",
    r"\interceptor_byalex\proxy\interceptor_doors_driver": PROXY + "vb_er_door_driver",
    r"\interceptor_byalex\proxy\interceptor_doors_codriver": PROXY + "vb_er_door_codriver",
    r"\interceptor_byalex\proxy\interceptor_doors_hood": PROXY + "vb_er_hood",
}
PARTS = {    # файл в full/proxy (или work) -> (имя в моде, масса кг)
    "interceptor_wheel": ("vb_er_wheel", 30.0),
    "interceptor_doors_driver": ("vb_er_door_driver", 20.0),
    "interceptor_doors_codriver": ("vb_er_door_codriver", 20.0),
    "interceptor_doors_hood": ("vb_er_hood", 15.0),
    "vb_er_tank": ("vb_er_tank", None),
    "vb_er_tank_1": ("vb_er_tank_1", None),        # модели прокси баков — копии бака (er_attach.py)
    "vb_er_tank_2": ("vb_er_tank_2", None),
    "vb_er_pipes_l": ("vb_er_pipes_l", None),
    "vb_er_pipes_r": ("vb_er_pipes_r", None),
    "vb_er_pipes_engine": ("vb_er_pipes_engine", None),
}


def rename_proxies(m):
    n = 0
    for lod in m.lods:
        new = type(lod.selections)()
        for name, sel in lod.selections.items():
            if name.startswith("proxy:"):
                path, idx = name[6:].rsplit(".", 1)
                key = path.lower()
                if key in RENAME:
                    name = "proxy:%s.%s" % (RENAME[key], idx)
                    n += 1
            new[name] = sel
        lod.selections = new
    return n


def set_mass(m, kg):
    for lod in m.lods:
        if C.is_res(lod, 1e13) and lod.points:
            for p in lod.points:
                p.mass = kg / len(lod.points)


def next_component(lod):
    k = 0
    for name in lod.selections:
        mm = re.match(r"(?i)component(\d+)$", name)
        if mm:
            k = max(k, int(mm.group(1)))
    return k + 1


def collision_boxes(car):
    """Коробки (Blender: центр, полуразмеры) по точкам выделений er_flares (3 на арку) и er_lightbar в LOD 1."""
    lod = C.lod_of(car, 1.0)
    out = []
    pts = np.array([p.coords for p in lod.selections["er_flares"].points])      # оси DayZ
    for sx in (1, -1):
        for sz in (1, -1):
            g = pts[(np.sign(pts[:, 0]) == sx) & (np.sign(pts[:, 2]) == sz)]
            if len(g) == 0:
                continue
            zs = np.linspace(g[:, 2].min(), g[:, 2].max(), 4)
            for a, b in zip(zs, zs[1:]):
                s = g[(g[:, 2] >= a - 1e-6) & (g[:, 2] <= b + 1e-6)]
                if len(s) < 4:
                    continue
                lo, hi = s.min(0), s.max(0)
                out.append((lo, hi))
    lb = np.array([p.coords for p in lod.selections["er_lightbar"].points])
    out.append((lb.min(0), lb.max(0)))
    return out


def add_collision(car, boxes, mass_each=6.0):
    for lod in car.lods:
        if not any(C.is_res(lod, r) for r in (1e13, 6e15, 7e15)):
            continue
        k = next_component(lod)
        for lo, hi in boxes:
            c = (lo + hi) / 2
            h = np.maximum((hi - lo) / 2, 0.015)
            name = "Component%02d" % k
            k += 1
            gp = Part("geo", "", [], [name])
            box(gp, V(c[0], c[2], c[1]), V(1, 0, 0), V(0, 1, 0), V(0, 0, 1), h[0], h[2], h[1], "steel")
            d = part_data(gp)
            d = (d[0], [(f[0], f[1], f[2], ("", "")) for f in d[1]])
            base = len(lod.points)
            append(lod, d, gp.sels)
            if C.is_res(lod, 1e13):
                for p in lod.points[base:]:
                    p.mass = mass_each / 8.0
        print("  коллизия: LOD %g + %d компонентов" % (lod.resolution, len(boxes)))


def model_cfg():
    src = open(os.path.join(MOD, "основа_ALEX_mlod", "model.cfg"), encoding="utf-8").read()
    src = src.replace("Interceptor_skeleton", "VB_EchoRunner_skeleton")
    src = re.sub(r"class interceptor: Default", "class vb_echorunner: Default", src)
    src = src.replace('"dmgzone_lights_2_1", "pl2"};', '"dmgzone_lights_2_1", "pl2", "neon", "screen"};')
    assert '"neon", "screen"' in src
    head = "// VB_EchoRunner: скелет и анимации — из перевода Interceptor ALEX (odol2mlod.py), этап 6 (er_finalize.py)\n"
    # антиграв-модуль (этап 8): наклон по ходу полёта и два встречных ротора; фазы задаёт скрипт (источники user)
    skel = """	class VB_ER_GravModule_skeleton: Default
	{
		isDiscrete = 1;
		skeletonInherit = "";
		skeletonBones[] =
		{
			"pitch", "",
			"tilt", "pitch",
			"rotor", "tilt",
			"rotor_b", "tilt",
			"spin", "tilt"
		};
	};
"""
    i = src.index("class CfgModels")
    j = src.rfind("};", 0, i)
    src = src[:j] + skel + src[j:]
    extra = """
	class vb_er_gravmodule: Default
	{
		skeletonName = "VB_ER_GravModule_skeleton";
		sections[] = {"neon"};
		class Animations
		{
			class rotor
			{
				type = "rotation";
				source = "rotor";
				selection = "rotor";
				axis = "rotor_axis";
				memory = 1;
				sourceAddress = "loop";
				minValue = 0;
				maxValue = 1;
				angle0 = 0;
				angle1 = 6.2831855;
			};
			class rotor_b
			{
				type = "rotation";
				source = "rotor";
				selection = "rotor_b";
				axis = "rotor_axis";
				memory = 1;
				sourceAddress = "loop";
				minValue = 0;
				maxValue = 1;
				angle0 = 0;
				angle1 = -12.566371;
			};
			class spin
			{
				type = "rotation";
				source = "spin";
				selection = "spin";
				axis = "rotor_axis";
				memory = 1;
				sourceAddress = "loop";
				minValue = 0;
				maxValue = 1;
				angle0 = 0;
				angle1 = 6.2831855;
			};
			class tilt_x
			{
				type = "rotation";
				source = "tilt_x";
				selection = "pitch";
				axis = "tilt_axis_x";
				memory = 1;
				minValue = 0;
				maxValue = 1;
				angle0 = -0.4363323;
				angle1 = 0.4363323;
			};
			class tilt_z
			{
				type = "rotation";
				source = "tilt_z";
				selection = "tilt";
				axis = "tilt_axis_z";
				memory = 1;
				minValue = 0;
				maxValue = 1;
				angle0 = -0.4363323;
				angle1 = 0.4363323;
			};
		};
	};
	class vb_er_wheel: Default { sections[] = {}; };
	class vb_er_wheel_destroyed: Default { sections[] = {}; };
	class vb_er_door_driver: Default { sections[] = {"camo1", "neon"}; };
	class vb_er_door_codriver: Default { sections[] = {"camo1", "neon"}; };
	class vb_er_hood: Default { sections[] = {"camo1"}; };
	class vb_er_tank: Default { sections[] = {"neon"}; };
	class vb_er_tank_1: Default { sections[] = {"neon"}; };
	class vb_er_tank_2: Default { sections[] = {"neon"}; };
	class vb_er_pipes_l: Default { sections[] = {"neon"}; };
	class vb_er_pipes_r: Default { sections[] = {"neon"}; };
	class vb_er_pipes_engine: Default { sections[] = {"neon"}; };
};"""
    i = src.rstrip().rfind("};")
    src = src[:i].rstrip() + "\n" + extra + "\n"
    return head + src.split("\n", 1)[1] if src.startswith("//") else head + src


def main():
    os.makedirs(os.path.join(SRC, "proxy"), exist_ok=True)
    car = C.load_p3d(os.path.join(FULL, "interceptor.p3d"))
    print("прокси переименовано:", rename_proxies(car))
    add_collision(car, collision_boxes(car))
    car.save(os.path.join(SRC, "vb_echorunner.p3d"))
    for src, (dst, kg) in PARTS.items():
        m = C.load_p3d(os.path.join(FULL, "proxy", src + ".p3d"))
        if kg:
            set_mass(m, kg)
        m.save(os.path.join(SRC, "proxy", dst + ".p3d"))
    d = C.load_p3d(os.path.join(WD, "proxy", "vb_er_wheel_destroyed.p3d"))
    set_mass(d, 25.0)
    d.save(os.path.join(SRC, "proxy", "vb_er_wheel_destroyed.p3d"))
    gm = os.path.join(WD, "proxy", "vb_er_gravmodule.p3d")          # tools/er_gravmodule.py (этап 8)
    if os.path.isfile(gm):
        shutil.copy2(gm, os.path.join(SRC, "proxy", "vb_er_gravmodule.p3d"))
    with open(os.path.join(SRC, "model.cfg"), "w", encoding="utf-8", newline="\r\n") as f:
        f.write(model_cfg())
    print("OK", SRC)


main()
