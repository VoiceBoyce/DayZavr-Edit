# Рендер детекторов VB_AnomaliesBosses: модель и hiddenSelections* из конфигов мода (текстом)
import json
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
MOD = r"D:\DayZavr\DayZavr-Edit\modding\mod_vb_AnomaliesBosses\VB_AnomaliesBosses"
BLENDER = r"G:\SteamLibrary\steamapps\common\Blender\blender.exe"
SCRIPT = r"D:\DayZavr\DayZavr-Edit\.claude\tools\dayz-render\render_p3d.py"
OUT = os.path.join(HERE, "out")
os.makedirs(OUT, exist_ok=True)
CLASSES = ["VB_AB_Detector_Loza", "VB_AB_Detector_Ogonek", "VB_AB_Detector_Peleng", "VB_AB_Detector_Sito",
           "VB_AB_Detector_Nevod"]
only = [a for a in sys.argv[1:] if not a.startswith("--")]
VIEW = [a for a in sys.argv[1:] if a.startswith("--")]   # общие опции рендера (ракурс, размер)
if VIEW and any(a.startswith("--out=") for a in VIEW):
    OUT = os.path.join(HERE, [a.split("=", 1)[1] for a in VIEW if a.startswith("--out=")][0])
    VIEW = [a for a in VIEW if not a.startswith("--out=")]
    os.makedirs(OUT, exist_ok=True)
extra = {}                                       # ракурсы по классам, если понадобятся

text = ""
for f in [os.path.join(MOD, "config.cpp")] + [os.path.join(MOD, "cfg", n) for n in os.listdir(os.path.join(MOD, "cfg"))]:
    text += open(f, encoding="utf-8", errors="replace").read() + "\n"


def body(name):
    m = re.search(r"class\s+%s\s*:\s*\w+\s*\{" % name, text)
    i, d = m.end() - 1, 0
    for j in range(i, len(text)):
        d += {"{": 1, "}": -1}.get(text[j], 0)
        if d == 0:
            return text[i:j + 1]


def arr(b, key):
    m = re.search(r"\b%s\[\]\s*=\s*\{(.*?)\};" % key, b, re.S)
    return re.findall(r'"([^"]*)"', m.group(1)) if m else []


for c in CLASSES:
    if only and c not in only:
        continue
    b = body(c)
    model = re.search(r'model\s*=\s*"([^"]+)"', b).group(1)
    sel = {"selections": arr(b, "hiddenSelections"), "textures": arr(b, "hiddenSelectionsTextures"),
           "materials": arr(b, "hiddenSelectionsMaterials")}
    sp = os.path.join(HERE, c + ".json")
    json.dump(sel, open(sp, "w", encoding="utf-8"), ensure_ascii=False, indent=1)
    mp = os.path.join(MOD, model.split("\\", 1)[1])          # VB_AB\m\... -> папка мода
    cmd = [BLENDER, "-b", "--factory-startup", "--python", SCRIPT, "--", mp, os.path.join(OUT, c + ".png"),
           "--prefix=vb_ab=" + MOD, "--sel=" + sp, "--size=2000", "--samples=256"] + extra.get(c, []) + VIEW
    r = subprocess.run(cmd, capture_output=True, text=True, encoding="utf-8", errors="replace")
    keep = [l for l in (r.stdout + r.stderr).splitlines()
            if l.startswith(("GEOM", "WARN", "SEL", "MAT", "CROPPED", "Traceback")) or "Error" in l and "HIP" not in l]
    print("=== " + c + "\n  " + "\n  ".join(keep), flush=True)
print("DONE")
