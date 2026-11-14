# -*- coding: utf-8 -*-
"""Описания артефактов в крафте 16 (рецепты 800–857): что артефакт делает с игроком.

  python art_desc16.py            — таблица artifacts_effects_16.md на проверку
  python art_desc16.py --deploy   — заменить только поле Description у рецептов 800–857 на живом 16 (с бэкапом)

Значения берутся из исходников серверного мода артефактов (ArtifactScriptsServer, modded class … Init):
меняешь баланс там — перезапускаешь этот скрипт, описания пересобираются.

Как работает мод (по коду ArtifactScriptsServer и ванили 1.30):
- эффект срабатывает раз в 3 с (кровотечения — раз в 10 с), пока артефакт где угодно у игрока,
  кроме контейнера OOTS_ART_CASE_BASE (на 16: OOTS_ART_CASE, CASEBig, CASELow);
- значения одного стата от разных артефактов складываются, множители (m_Relatives) перемножаются
  и действуют на сумму. Множитель без «абсолютного» значения стата сам ничего не даёт;
- Bleeds > 0 — шанс раз в 10 с ЗАКРЫТЬ кровотечение, < 0 — ОТКРЫТЬ (комментарий в исходниках врёт);
- Stamina — доля от ТЕКУЩЕЙ выносливости за тик;
- Radiation в серверном моде пустой (Process закомментирован) — не описываем;
- шок: ваниль восстанавливает 5/с в сознании и 1/с без сознания (cfggameplay 16 — ванильные 5 и 1),
  отключка при шоке ≤ 25, очнуться можно при ≥ 50. Поэтому −20 и сильнее за 3 с вырубают сами,
  а любой заметный минус не даёт очнуться после нокаута.
Классы: OOTS_ART_X наследует DD_Artifact_Y в config.bin (DD_Artifacts\\model\\Artifacts); у номерных версий
(Plamya2, Glaz3…) свой скриптовый класс от DD_ArtifactBase — у них только свои значения.
"""
import datetime
import io
import json
import math
import os
import re
import shutil
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ARTS = os.path.join(HERE, "..", "..", "mod_OTTS_Artifacts", "то что можно редачить, серверный мод артефактов",
                    "ArtifactScriptsServer", "scripts", "4_World", "ArtifactScriptsServer", "entities", "Arts")
LIVE = r"D:\DayZavr\DayZavr-Servers\DayZServer16\Instance_1\DRL_CraftMenu"
BACKUP = r"G:\DayZ_Test\backup"
ID_MIN, ID_MAX = 800, 857

# OOTS_ART_<ключ> -> скриптовый класс (config.bin: class OOTS_ART_X: DD_Artifact_Y)
MAP = {
    "svet": "DD_Artifact_Lens", "Crustal": "DD_Artifact_Crustal", "Drop": "DD_Artifact_Drop",
    "Plamya": "DD_Artifact_Flame", "Glaz": "DD_Artifact_Eye", "kameniy_cvetok": "DD_Artifact_StoneFlower",
    "Medusa": "DD_Artifact_JellyFish", "SeaUrchin": "DD_Artifact_SeaUrchin", "myaso": "DD_Artifact_BreakingMeet",
    "kolobok": "DD_Artifact_Colobok", "Battery": "DD_Artifact_Battery", "Light": "DD_Artifact_Light",
    "Klyaksa": "DD_Artifact_Klyaksa", "Mohovik": "DD_Artifact_Mohovik", "Angel": "DD_Artifact_Angel",
    "AngelFool": "DD_Artifact_AngelFool", "vspishka": "DD_Artifact_Flash", "Plasma": "DD_Artifact_Plasma",
    "Sparkler": "DD_Artifact_Sparkler", "snejinka": "DD_Artifact_SnowFlake", "Gravi": "DD_Artifact_Gravy",
    "vivert": "DD_Artifact_Twist", "NightStar": "DD_Artifact_NightStar", "Thorn": "DD_Artifact_Thorn",
    "Mikrob": "DD_Artifact_Mikrob", "Flower": "DD_Artifact_Flower", "krov_kamnya": "DD_Artifact_BloodOfStone",
    "FireBall": "DD_Artifact_FireBall", "Burn": "DD_Artifact_Burn", "izolator": "DD_Artifact_Insulator",
    "DemonEye": "DD_Artifact_DemonEye", "puzir": "DD_Artifact_Babble", "BloodStone_New": "DD_Artifact_BloodStone_New",
    "ShineOfTheForest": "DD_Artifact_ShineOfTheForest", "CrustalThorn": "DD_Artifact_CrustalThorn",
    "ribka": "DD_Artifact_GoldenFish", "MoonLight": "DD_Artifact_MoonLight", "Compas": "DD_Artifact_Compass",
    "DevelHert": "DD_Artifact_DevelHert", "DarkSoul": "DD_Artifact_DarkSoul", "shturval": "DD_Artifact_SteeringWheel",
    "NanoCell": "DD_Artifact_NanoCell", "Monolit": "DD_Artifact_Monolit", "Oasis": "DD_Artifact_Oasis",
    "Spring": "DD_Artifact_Spring", "Braen": "DD_Artifact_Braen",
}
# свой скриптовый класс (class OOTS_ART_X extends DD_ArtifactBase в клиентских скриптах)
OWN = {"Battery3", "Crustal2", "Crustal3", "Drop2", "Drop3", "Glaz2", "Glaz3", "Plamya2", "Plamya3", "busy2",
       "busy3", "kameniy_cvetok2", "kameniy_cvetok3"}

TICK = 3.0           # с, тик здоровья/крови/воды/еды/шока/выносливости
BLEED_TICK = 10.0    # с, тик кровотечений
SHOCK_REGEN = 5.0    # в секунду, в сознании (cfggameplay 16)
SHOCK_REGEN_UNCON = 1.0
STAT_RU = {"Health": "здоровье", "Blood": "кровь", "Water": "вода", "Energy": "сытость", "Stamina": "выносливость"}


def parse_scripts():
    """{класс: (abs, rel)} из modded class … Init() серверного мода."""
    out = {}
    for fn in sorted(os.listdir(ARTS)):
        if not fn.endswith(".c"):
            continue
        txt = io.open(os.path.join(ARTS, fn), encoding="utf-8-sig").read()
        txt = re.sub(r"/\*.*?\*/", "", txt, flags=re.S)
        txt = re.sub(r"//[^\n]*", "", txt)
        parts = re.split(r"modded\s+class\s+(\w+)", txt)
        for name, body in zip(parts[1::2], parts[2::2]):
            ab, rel = {}, {}
            for kind, stat, val in re.findall(r'm_(Absolutes|Relatives)\.Set\(\s*"(\w+)"\s*,\s*(-?[\d.]+)\s*\)', body):
                (ab if kind == "Absolutes" else rel)[stat] = float(val)
            out[name] = (ab, rel)
    return out


def script_class(key):
    return "OOTS_ART_" + key if key in OWN else MAP.get(key)


def num(x):
    x = round(x, 2)
    s = ("%d" % x) if x == int(x) else ("%g" % x)
    return s.replace(".", ",").replace("-", "−")


def signed(x):
    return ("+" if x > 0 else "") + num(x)


def dur_min(minutes):
    """«~30 с», «~2,5 мин», «~7 мин», «~2 ч»."""
    sec = minutes * 60
    if sec < 60:
        return "~%d с" % max(5, int(round(sec / 5.0)) * 5)
    if minutes < 3:
        return "~%s мин" % num(round(minutes * 2) / 2.0)
    if minutes < 90:
        return "~%d мин" % round(minutes)
    return "~%s ч" % num(round(minutes / 60.0 * 2) / 2.0)


def dur_sec(sec):
    if sec < 20:
        return "~%d с" % round(sec)
    if sec < 120:
        return "~%d с" % (int(round(sec / 5.0)) * 5)
    return dur_min(sec / 60.0)


def times(n):
    """«вдвое», «в 2,5 раза», «в 4 раза», «в 10 раз»."""
    n = round(n, 2)
    if n == 2:
        return "вдвое"
    if n != int(n) or int(n) in (3, 4):
        return "в %s раза" % num(n)
    return "в %s раз" % num(n)


def mult_words(r):
    if r < 0:
        return "×(%s) — переворачивает: трата становится прибавкой и наоборот" % num(r)
    if r < 1:
        return "×%s — %s слабее" % (num(r), times(1 / r))
    if r < 1.5:
        return "×%s — на %d%% сильнее" % (num(r), round((r - 1) * 100))
    return "×%s — %s сильнее" % (num(r), times(r))


def describe(key):
    """Текст описания для OOTS_ART_<key>. Возвращает (текст, сырые значения для таблицы)."""
    cls = script_class(key)
    data = SCRIPTS.get(cls)
    if data is None:
        return "Эффектов на игрока нет.", (cls, {}, {})
    ab, rel = data
    own = {s: ab[s] * rel.get(s, 1.0) for s in ab}           # сам по себе: свой множитель тоже действует
    plus, minus, danger = [], [], []

    # здоровье, кровь, вода, сытость — в минуту
    per_min = {s: own[s] * 60.0 / TICK for s in ("Health", "Blood", "Water", "Energy") if own.get(s)}
    h = per_min.get("Health")
    if h:
        txt = "здоровье %s/мин (из 100)" % signed(h)
        (plus if h > 0 else minus).append(txt)
        if h <= -10:
            danger.append("убьёт за %s, если не перекрыть лечением" % dur_min(100.0 / -h))
    b = per_min.get("Blood")
    if b:
        txt = "кровь %s/мин (из 5000)" % signed(b)
        if b >= 1000:
            txt += " — восполняет за минуты"
        (plus if b > 0 else minus).append(txt)
        if b <= -250:
            danger.append("кровь упадёт до смертельной за %s" % dur_min(2500.0 / -b))
    w, e = per_min.get("Water"), per_min.get("Energy")
    food = []
    if w is not None and e is not None and w == e:
        food.append(("вода и сытость", w))
    else:
        if w is not None:
            food.append(("вода", w))
        if e is not None:
            food.append(("сытость", e))
    for name, v in food:
        txt = "%s %s/мин (запас 5000)" % (name, signed(v))
        if v <= -150:
            txt += ", досуха за %s" % dur_min(5000.0 / -v)
        elif v >= 1000:
            txt += " — восполняет за минуты"
        (plus if v > 0 else minus).append(txt)

    # кровотечения
    p = own.get("Bleeds")
    if p:
        every = BLEED_TICK / abs(p)
        if p > 0:
            plus.append("закрывает кровотечения (%s)" % ("по одному каждые 10 с" if p >= 1 else
                                                       "в среднем одно за %s" % dur_sec(every)))
        else:
            minus.append("открывает кровотечения (в среднем одно за %s)" % dur_sec(every))

    # выносливость (доля от текущей за тик)
    st = own.get("Stamina")
    if st:
        txt = "выносливость %s%% от текущей каждые 3 с" % signed(st * 100)
        (plus if st > 0 else minus).append(txt)

    # шок
    s = own.get("Shock")
    if s:
        if s > 0:
            plus.append("быстрее приходишь в себя после нокаута (+%s к шоку за 3 с)" % num(s))
        else:
            net = s + SHOCK_REGEN * TICK
            if net < 0:
                n = math.ceil((75.0 + s) / -net) + 1          # 100 + s + (n-1)*net <= 25
                danger.append("сам вырубает примерно через %s и не даёт очнуться, пока он при тебе (%s шока за 3 с)"
                              % (dur_sec(n * TICK), num(s)))
            elif s + SHOCK_REGEN_UNCON * TICK < 0:
                danger.append("если тебя вырубят, не очнёшься, пока он при тебе (%s шока за 3 с)" % num(s))

    # множители для других артефактов
    mults = []
    groups = {}
    for stat, r in rel.items():
        if stat in STAT_RU:
            groups.setdefault(r, []).append(STAT_RU[stat])
    for r, stats in sorted(groups.items(), key=lambda x: -abs(x[0])):
        mults.append("%s %s" % (" и ".join(stats), mult_words(r)))

    parts = []
    if plus and minus:
        parts.append("Плюсы: " + ", ".join(plus) + ".")
        parts.append("Минусы: " + ", ".join(minus) + ".")
    elif plus:
        parts.append("Только плюсы: " + ", ".join(plus) + ".")
    elif minus:
        parts.append("Только минусы: " + ", ".join(minus) + ".")
    if danger:
        parts.append("Опасно: " + "; ".join(danger) + ".")
    if mults:
        alone = not plus and not minus and not danger
        parts.append(("Сам ничего не даёт, меняет эффекты других артефактов (и плюсы, и минусы): " if alone else
                      "Эффекты других артефактов: ") + "; ".join(mults) + ".")
        if alone:
            parts.append("На обычный голод и жажду не влияет.")
    if not parts:
        parts.append("Эффектов на игрока нет.")
    return " ".join(parts), (cls, ab, rel)


SCRIPTS = parse_scripts()


def full_description(key, elem_ru, tier_roman):
    text, _ = describe(key)
    return "%s Стихия «%s», тир %s. Действует из инвентаря, в контейнере для артефактов выключен." % (
        text, elem_ru, tier_roman)


def dump(obj, path):
    with io.open(path, "w", encoding="utf-8", newline="\r\n") as f:
        json.dump(obj, f, ensure_ascii=False, indent=4)


def main():
    sys.path.insert(0, HERE)
    import make_ab_craft16 as AB                      # стихии и тиры — оттуда же, что и рецепты

    path = os.path.join(LIVE, "CraftConfig.json")
    raw = io.open(path, encoding="utf-8-sig").read()
    cfg = json.loads(raw)
    rows, changed = [], 0
    for r in cfg:
        if not (ID_MIN <= r["RecipeId"] <= ID_MAX and r["Result"].startswith("OOTS_ART_")):
            continue
        key = r["Result"][len("OOTS_ART_"):]
        assert key in AB.ART, key
        assert key in OWN or key in MAP, ("нет класса для", key)
        cls = script_class(key)
        assert key not in OWN or cls in SCRIPTS, ("номерной без значений в серверном моде", key)
        e, t = AB.ART[key]
        desc = full_description(key, AB.RU[e], AB.ROMAN[t])
        _, (c, ab, rel) = describe(key)
        if r.get("Description") != desc:
            changed += 1
        r["Description"] = desc
        vals = ", ".join("%s %s" % (k, num(v)) for k, v in ab.items() if k != "Radiation")
        vals += "".join("; ×%s %s" % (num(v), k) for k, v in rel.items())
        rows.append((r["RecipeId"], r["RecipeName"].replace("Артефакт - ", ""), key, c, vals or "—", desc))

    lines = ["# Описания артефактов в крафте 16", "",
             "Сгенерировано `art_desc16.py` из исходников серверного мода артефактов (ArtifactScriptsServer). "
             "Значения — за тик 3 с (Bleeds — шанс за 10 с: плюс закрывает, минус открывает; Stamina — доля от текущей), "
             "радиация в моде не работает и не показана.", "",
             "| ID | Артефакт | Класс | Скрипт | Значения | Описание |", "|---|---|---|---|---|---|"]
    lines += ["| %d | %s | `%s` | `%s` | %s | %s |" % x for x in rows]
    with io.open(os.path.join(HERE, "artifacts_effects_16.md"), "w", encoding="utf-8", newline="\n") as f:
        f.write("\n".join(lines) + "\n")
    longest = max(len(x[5]) for x in rows)
    print("артефактов %d, изменится описаний %d, самое длинное %d символов" % (len(rows), changed, longest))

    if "--deploy" in sys.argv[1:]:
        # формат файла не меняем: сверяем, что повторная запись без правок даёт тот же текст
        probe = json.dumps(json.loads(raw), ensure_ascii=False, indent=4)
        if probe.replace("\n", "\r\n") != raw.lstrip("\ufeff").rstrip("\r\n") and probe != raw.lstrip("\ufeff").rstrip("\r\n"):
            print("ВНИМАНИЕ: живой файл записан в другом формате — сравни diff после выкладки")
        stamp = datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
        bdir = os.path.join(BACKUP, stamp + "_server16_art_desc")
        os.makedirs(bdir)
        shutil.copy2(path, os.path.join(bdir, "CraftConfig.json"))
        dump(cfg, path)
        print("выложено на 16; бэкап:", bdir)


if __name__ == "__main__":
    main()
