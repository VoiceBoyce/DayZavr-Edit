# -*- coding: utf-8 -*-
r"""Данные ALEX под нашими путями: vb_echorunner\data\<rel> <- Interceptor_byALEX\data\<rel> (без учёта регистра).

ensure(rels): текстуры копируются, только если файла ещё нет (перекрашенные этапом 5 не затираются);
бинарные rvmat ALEX переписываются текстом с нашими путями стадий (и их текстуры тоже обеспечиваются).
"""
import os
import shutil
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
MOD = os.path.dirname(HERE)
ALEX_DATA = os.path.join(os.path.dirname(MOD), "купленные модели", "Interceptor", "Interceptor_byALEX", "data")
OUR_DATA = os.path.join(MOD, "VB_EchoRunner", "data")
SRC_PFX, DST_PFX = "interceptor_byalex\\data\\", "vb_echorunner\\data\\"


def alex_file(rel):
    d, name = os.path.split(os.path.join(ALEX_DATA, rel))
    if not os.path.isdir(d):
        return None
    for f in os.listdir(d):
        if f.lower() == name.lower():
            return os.path.join(d, f)
    return None


def _rvmat_text(src, todo):
    sys.path.insert(0, r"d:\DayZavr\DayZavr-Edit\.claude\tools\dayz-render")
    import rvmat_read
    rv = rvmat_read.read_rvmat(src)
    lines = []
    for k in ("ambient", "diffuse", "forcedDiffuse", "emmisive", "specular"):
        if k in rv:
            lines.append("%s[] = {%s};" % (k, ",".join("%g" % x for x in rv[k])))
    lines.append("specularPower = %g;" % rv.get("specularPower", 40))
    lines.append('PixelShaderID = "%s";' % rv.get("PixelShaderID", "Super"))
    lines.append('VertexShaderID = "%s";' % rv.get("VertexShaderID", "Super"))
    for st in sorted((k for k in rv if k.startswith("Stage")), key=lambda k: int(k[5:])):
        t = rv[st].get("texture", "")
        if t.lower().startswith(SRC_PFX):
            todo.add(t.lower()[len(SRC_PFX):])
            t = DST_PFX + t.lower()[len(SRC_PFX):]
        lines.append('class %s\n{\n\ttexture = "%s";\n\tuvSource = "%s";\n\tclass uvTransform\n\t{\n'
                     '\t\taside[] = {1,0,0};\n\t\tup[] = {0,1,0};\n\t\tdir[] = {0,0,1};\n\t\tpos[] = {0,0,0};\n'
                     '\t};\n};' % (st, t, rv[st].get("uvSource", "tex")))
    return "\n".join(lines) + "\n"


def ensure(rels):
    """rels — пути относительно data (нижний регистр), например 'body_co.paa', 'base\\body_damage.rvmat'."""
    todo = set(r.lower() for r in rels)
    done = set()
    copied = 0
    while todo - done:
        rel = sorted(todo - done)[0]
        done.add(rel)
        dst = os.path.join(OUR_DATA, rel)
        src = alex_file(rel)
        if rel.endswith(".rvmat"):
            if src and not os.path.exists(dst):
                os.makedirs(os.path.dirname(dst), exist_ok=True)
                with open(dst, "w", encoding="ascii", newline="\r\n") as f:
                    f.write(_rvmat_text(src, todo))
                copied += 1
            elif src:
                _rvmat_text(src, todo)                       # стадии всё равно обеспечить
            continue
        if os.path.exists(dst):
            continue
        if src:
            os.makedirs(os.path.dirname(dst), exist_ok=True)
            shutil.copy2(src, dst)
            copied += 1
        else:
            print("  WARN нет у ALEX:", rel)
    return copied
