# -*- coding: utf-8 -*-
r"""Правки model.cfg машины поверх перевода ALEX (Баги №60, №61, 2026-11-25). Без зависимостей.

  python er_modelcfg_fix.py            — применить к VB_EchoRunner/model.cfg (повторный запуск ничего не меняет)
  er_finalize.py вызывает apply() при сборке model.cfg.

№60 «заднее колесо утоплено, перед выше». Подвеска (config.cpp, от ALEX): перед ход 0.15/0.15, зад 0.09/0.09,
жёсткость 60 000. По замеру скилла dayz-vehicles (LL-512) пружина свободна при полном отбое, фаза damper линейна:
0 — полный отбой (−travelMaxDown), 1 — полное сжатие (+travelMaxUp). Анимации ALEX рисовали колесо не там, где оно
в физике: перед (перенос по оси 0.244 м) зажат с фазы 0.3, а в покое фаза ~0.25 — колесо рисовалось на ~6 см выше
физического, висело над землёй; зад (качание моста) — на ~2 см ниже. Теперь видимое колесо = физическое + 3 см
(видимый радиус колеса 0.46, в конфиге 0.42: шина «проминается» на 1 см). Модуль (видимый низ = 0.42) висит над
землёй на 3 см — подушка.
  перед: перенос в долях длины оси, offset(p) = (u(p) + 0.03) / L, u = −0.15 + 0.30·p; L лев 0.244, прав 0.246;
  зад: качание моста, колесо поднимается на −1.264·угол (плечи 1.430 и 0.166 от осей качания, знак — по правилу
  правой руки, проверен по дверям), угол(p) = −(u(p) + 0.03) / 1.264, u = −0.09 + 0.18·p.

№61 на костях колёс машины ОТМЕНЁН 11-25: свои user-источники машины ломали двери (фаза уходила на двери).
№61 на модуле: в скелет модуля корневая кость hub (родитель pitch), анимация unspin (источник модуля) вокруг hub_axis.
"""
import io
import os
import re

S = 0.03          # видимое колесо выше физического центра (радиусы 0.46 и 0.42, минус 1 см «проминания»)
K = 1.264         # подъём заднего колеса на радиан качания моста
# Замер на стенде 2026-11-25 (VB_SuspDiag: касание WheelGetContactPosition, фаза damper, кость колеса): физическое
# колесо = ось модели + D − travelMaxDown + (up+down)·фаза. Наклон совпал с ходом (перед 0.30, зад 0.18), а покой
# физики — на D выше нашей оси: движок держит высоту оси ALEX до подъёма er_build (0.446 / 0.394 против 0.382 / 0.330).
D = 0.0642


def _num(v):
    return ("%.6f" % v).rstrip("0").rstrip(".")


def _set(src, cls, fields):
    m = re.search(r"(\t+class %s\n\t+\{\n)(.*?)(\n\t+\};)" % re.escape(cls), src, re.S)
    assert m, cls
    body = m.group(2)
    for k, v in fields.items():
        body, n = re.subn(r"(\b%s\s*=\s*)[-0-9.e]+;" % k, r"\g<1>%s;" % v, body)
        assert n == 1, (cls, k)
    return src[:m.start(2)] + body + src[m.end(2):]


def apply(src):
    src = src.replace("\r\n", "\n")
    for cls, L in (("suspension_damper_1_1", 0.244), ("suspension_damper_2_1", 0.246)):
        src = _set(src, cls, {"minValue": "0", "maxValue": "1",
                              "offset0": _num((-0.15 + D + S) / L), "offset1": _num((0.15 + D + S) / L)})
    a0, a1 = -(-0.09 + D + S) / K, -(0.09 + D + S) / K
    src = _set(src, "suspension_axis_1_2", {"angle0": _num(a0), "angle1": _num(a1)})
    src = _set(src, "suspension_axis_2_2", {"angle0": _num(-a0), "angle1": _num(-a1)})
    # №61 на модуле: корневая кость hub (пустая, er_gravmodule_hub.py) над pitch и её поворот вокруг оси колеса
    if '"hub", ""' not in src:
        old = '\t\t\t"pitch", "",\n'
        assert src.count(old) == 1
        src = src.replace(old, '\t\t\t"hub", "",\n\t\t\t"pitch", "hub",\n')
    if "class unspin" not in src:
        i = src.index("class vb_er_gravmodule: Default")
        m = re.compile(r"\t+class tilt_z\n\t+\{\n.*?\n\t+\};\n", re.S).search(src, i)
        assert m
        add = ("\t\t\tclass unspin\n\t\t\t{\n\t\t\t\ttype = \"rotation\";\n\t\t\t\tsource = \"unspin\";\n"
               "\t\t\t\tselection = \"hub\";\n\t\t\t\taxis = \"hub_axis\";\n\t\t\t\tmemory = 1;\n"
               "\t\t\t\tsourceAddress = \"loop\";\n\t\t\t\tminValue = 0;\n\t\t\t\tmaxValue = 1;\n"
               "\t\t\t\tangle0 = 0;\n\t\t\t\tangle1 = 6.2831855;\n\t\t\t};\n")
        src = src[:m.end()] + add + src[m.end():]
    return src


if __name__ == "__main__":
    p = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "VB_EchoRunner", "model.cfg")
    old = io.open(p, encoding="utf-8").read()
    new = apply(old)
    io.open(p, "w", encoding="utf-8", newline="\r\n").write(new)
    print("model.cfg:", "изменён" if new != old.replace("\r\n", "\n") else "без изменений")
