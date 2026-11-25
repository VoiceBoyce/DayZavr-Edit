# -*- coding: utf-8 -*-
"""Крафт DRL сервера 16 для семейства Эхо (таблица утверждена пользователем 2026-11-25).

  python make_echo_craft16.py

Мод универсальный, а рецепты — под 16: кроме VB_EM, EFT и ванили в них материалы аномалий VB_AB_Mat_*, трофеи боссов
VB_AB_Trophy_*, жилы Изнанки VB_RiftVein и артефакт OOTS «Батарейка».

  20–26      «Материалы»       — верстак: 7 крафтовых материалов VB_EM_* (общий раздел 16, Баги №73);
  30–34      «Энергия Эхо»     — верстак: энергоячейки Mk I–IV (цепочкой, выходят пустыми) и аккумулятор Эхо (2026-11-25);
  3201–3208  «Экзоскелет Эхо»  — швейная: 8 вещей, расцветки — варианты результата (ResultClasses);
  604–606    «Обмундирование»  — верстак: пояса для артефактов S/M/L (Баги №67: со швейной на верстак, раздел общий с 600–602);
  4301–4305  «Echo Runner»     — подъёмник: кузов + капот, двери, колесо (AttachedRecipeIds), разбор кузова выключен;
  4306–4310  «Echo Runner»     — подъёмник: антиграв-модуль, бак Эхо, трубопроводы Изнанки (Баги №68, отдельные рецепты).
Бак и аккумулятор холодного синтеза — донат, не крафтятся. Наборы базы, ячейки, аккумуляторы Эхо — пока не крафтятся.
Свои рецепты узнаются по результату (VB_EM_*, VB_EC_*, VB_EchoRunner*): при перезапуске старые снимаются целиком,
поэтому смена номеров (пояса были 3209–3211) безопасна.

Пишет рядом:
  echo_recipes_16.json, echo_categories_16.json — только своё;
  CraftConfig_16.json, CraftCategories_16.json  — полные файлы 16-го со своим (готовы к выкладке);
  echo_craft_16.md                              — таблица на проверку.
Основа — текущие живые файлы сервера 16; свои ID и разделы заменяются, остальное не трогается. Живые файлы не пишет.
"""
import csv
import io
import json
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
MODDING = os.path.normpath(os.path.join(HERE, "..", ".."))
SRV16 = r"D:\DayZavr\DayZavr-Servers\DayZServer16\Instance_1\DRL_CraftMenu"
MAX_CATS = 24          # DRL_CraftWindow.c: разделов на станцию, лишние молча отрезаются

BENCH = ["DRL_Craft_Workbench"]
SEWING = ["DRL_Craft_Sewing"]
LIFT = ["DRL_Craft_Truck_Lift"]

TROPHIES = ["VB_AB_Trophy_Ricar_Acid", "VB_AB_Trophy_Ricar_Rad", "VB_AB_Trophy_Tenevnik_Psi", "VB_AB_Trophy_Tenevnik_Rad",
            "VB_AB_Trophy_Shum_Ice", "VB_AB_Trophy_Shum_Gravity", "VB_AB_Trophy_MutaWolf_Ice", "VB_AB_Trophy_MutaWolf_Fire",
            "VB_AB_Trophy_MozgoTras_Psi", "VB_AB_Trophy_MozgoTras_Storm", "VB_AB_Trophy_MozgoTrasRed_Fire",
            "VB_AB_Trophy_MozgoTrasRed_Gravity", "VB_AB_Trophy_Ent_Storm", "VB_AB_Trophy_Ent_Acid"]
VEIN = "VB_RiftVein"
PALETTES = ["Ranger", "Coyote", "Arctic"]
MAT_CATEGORY = "Материалы"        # общий раздел верстака 16 (1, 14, 16–19), туда же материалы Эхо (Баги №73)
BELT_CATEGORY = "Обмундирование"   # общий раздел верстака 16 (600–602), туда же пояса для артефактов
DESC_MAX = 170         # в шапке окна DRL две строки (описания артефактов 16 тоже до 170)


def comp(names, amount=1, recraft=0.5, att=0):
    names = [names] if isinstance(names, str) else list(names)
    return {"Classname": names[0], "Classnames": names, "EnableVariationSelection": 1, "MinHealthFrac": 3,
            "Amount": amount, "QuantityFrac": 0.0, "Delete": 1, "ApplyDamageFrac": 0.0,
            "RecraftDropChance": recraft, "RecraftAmount": amount, "RecraftQuantityFrac": 0.0,
            "UseInheritance": 0, "AttachmentsEx": att, "RecraftNeedAttachments": 0}


def tool(names, dmg=0.02, att=2):
    """Инструмент: не расходуется, изнашивается на dmg. att: 1 — навес станка, 2 — навес или инвентарь."""
    c = comp(names, 1, 0.0, att)
    c["Delete"] = 0
    c["ApplyDamageFrac"] = dmg
    return c


def trophy():
    """Трофей любого из 14 боссов; при разборе не возвращается."""
    return comp(TROPHIES, 1, 0.0)


def recipe(rid, name, result, comps, desc, time, bench, count=1, variants=None, attached=None):
    r = {"RecipeId": rid, "ShowInFreeMenu": 0, "WorkbenchClasses": list(bench), "RecipeName": name,
         "Result": result, "ResultClasses": list(variants or []), "PreviewClassname": "", "CraftIcon": "",
         "Description": desc, "CraftTimeSec": time, "RecraftTimeSec": time, "ResultCount": count,
         "ResultQuantityFrac": 0.0, "ResultDamageFrac": 0.0, "CraftComponents": comps}
    if attached:
        r["AttachedRecipeIds"] = list(attached)
    return r


def m(cls, amount=1, recraft=0.0):
    """Ингредиент материалов верстака: при разборе материала сырьё не возвращается (без петель)."""
    return comp(cls, amount, recraft)


def ab(key, amount):
    return "VB_AB_Mat_" + key, amount


# ------------------------------------------------------------------ 1. материалы (верстак)
def materials():
    """Материалы Эхо (Баги №80, 2026-11-25 — сложнее, время прежнее). Ревизия 11-25 (Баги №81): проводник и катушка
    входят почти везде — без лишних деталей, иначе на летающую машину уходило 37 меди, 37 грозы, ~97 сварки."""
    R = []
    R.append(recipe(20, "Демпферная вставка", "VB_EM_DamperPad",
                    [m("EFT_Cloth_fleece", 3), m("EFT_Cloth_aramid", 2), m("EFT_Cloth_ripstop"), m("EFT_Mounting_foam", 2),
                     m(*ab("Studen", 2)), m("EFT_Poxearm")],
                    "Стёгаешь флис с арамидом в чехле из рипстопа и проливаешь пеной пополам со студенистой слизью. "
                    "Слизь застывает гелем и гасит удар.", 6, BENCH, count=2))
    R.append(recipe(21, "Резонансная катушка", "VB_EM_ResonanceCoil",
                    [m("VB_EM_CopperSpool"), m("EFT_Magnet"), m("EFT_Insulatingtape"), m(*ab("Groza", 2)),
                     m("EFT_Capacitors"), m("EFT_Wires")] + ELEC_TOOLS,
                    "Мотаешь медь на сердечник электромагнита, вплавляешь грозовые кристаллы и выводишь концы через "
                    "конденсатор. Кристаллы держат заряд.", 8, BENCH))
    R.append(recipe(22, "Блок управления экзоскелетом", "VB_EM_ExoController",
                    [m("VB_EM_ECU"), m("EFT_Capacitors", 3), m("EFT_Rele", 2), m(*ab("Electra", 2)), m("EFT_Military_cable"),
                     m("EFT_Wires", 2), m("EFT_Insulatingtape")] + ELEC_TOOLS,
                    "Перепаиваешь ЭБУ под экзоскелет: конденсаторы и реле на питание, военный кабель к сервомоторам, "
                    "разрядный кварц — стабилизатор.", 10, BENCH))
    R.append(recipe(23, "Проводник Эхо", "VB_EM_EchoConduit",
                    [m("EFT_SiliconTube"), m("VB_EM_CopperSpool"), m(VEIN, 2), m("EFT_Insulatingtape")],
                    "Протягиваешь жилы Изнанки с медью в силиконовой трубке и заматываешь концы изолентой. По "
                    "проводнику энергия Эхо идёт в броне и машине.", 6, BENCH, count=2))
    R.append(recipe(24, "Силовая рама", "VB_EM_PowerFrame",
                    [m("Pipe", 4), m("MetalPlate", 3), m("EFT_Bolts", 3), m("EFT_model_nuts", 3), m("EFT_MetalParts", 2),
                     m("MetalWire", 2),
                     tool("Blowtorch", 0.05), tool(["Wrench", "EFT_Advwrench", "EFT_Ratchetwrench", "LugWrench"], 0.03)],
                    "Свариваешь трубы и листы в секцию каркаса, ставишь косынки и стягиваешь болтами и проволокой. "
                    "Основа кузова машины.", 12, BENCH))
    R.append(recipe(25, "Кузовная панель", "VB_EM_BodyPanel",
                    [m("MetalPlate", 4), m("EFT_Screw_pack", 2), m("EFT_Poxearm"), m("EFT_MetalParts"),
                     m("EFT_Mounting_foam"), m("EFT_Sheloch"),
                     tool("Hammer", 0.03), tool("EFT_MetalScissors", 0.03)],
                    "Выколачиваешь лист, травишь щёлочью, обрезаешь по шаблону, проклеиваешь рёбра холодной сваркой "
                    "и пеной изнутри. Идёт на двери и капоты.", 10, BENCH))
    R.append(recipe(26, "Бронестекло", "VB_EM_ArmorGlass",
                    [m("EFT_OrgGlass", 3), m(*ab("FireShtorm", 2)), m("EFT_Poxearm"), m(*ab("LinceAgr", 1)),
                     m("EFT_Cloth_aramid"), m("EFT_Klean")],
                    "Склеиваешь оргстекло слоями пепельного и линзового стекла с арамидной сеткой. Бронестекло "
                    "для визоров шлемов и стёкол машин.", 8, BENCH))
    return R


# ------------------------------------------------------------------ 1б. энергия (верстак), просьба пользователя 2026-11-25
ELEC_TOOLS = [tool(["Pliers", "EFT_kusajki", "EFT_Cablecutter"], 0.02),
              tool(["Screwdriver", "EFT_Screwdriver", "EFT_Screwdriver_krest"], 0.02)]


def energy():
    """Энергоячейки Mk I–IV (цепочкой: ячейка уровня ниже + детали; ячейки выходят пустыми) и аккумулятор Эхо.
    Энергия — грозовые материалы аномалий (Ионная пыль → Грозовой кристалл → Разрядный кварц), у Mk IV — жилы Изнанки.
    Ячейка и аккумулятор синтеза — донат, не крафтятся."""
    R = []
    R.append(recipe(30, "Энергоячейка Эхо Mk I", "VB_EchoCell_Mk1",
                    [m("EFT_GreenBat"), m("VB_EM_EchoConduit"), m("EFT_Wires"), m("EFT_Insulatingtape"),
                     m(*ab("EMP", 1))] + ELEC_TOOLS,
                    "Литиевый аккумулятор, пропитанный ионной пылью, в обмотке проводника Эхо. 2048 kE, выходит пустой — "
                    "заряжается на станции Эхо.", 8, BENCH))
    R.append(recipe(31, "Энергоячейка Эхо Mk II", "VB_EchoCell_Mk2",
                    [m("VB_EchoCell_Mk1"), m("EFT_GreenBat"), m("EFT_Capacitors"), m(*ab("EMP", 2))] + ELEC_TOOLS,
                    "Ячейка Mk I со вторым аккумулятором и конденсаторами. 4096 kE, выходит пустой: заряд старой ячейки "
                    "не сохраняется.", 10, BENCH))
    R.append(recipe(32, "Энергоячейка Эхо Mk III", "VB_EchoCell_Mk3",
                    [m("VB_EchoCell_Mk2"), m("VB_EM_ResonanceCoil"), m("EFT_Rele"), m(*ab("Groza", 2))] + ELEC_TOOLS,
                    "Ячейка Mk II на резонансной катушке с грозовыми кристаллами. 10 000 kE, выходит пустой: заряд старой "
                    "ячейки не сохраняется.", 12, BENCH))
    R.append(recipe(33, "Энергоячейка Эхо Mk IV", "VB_EchoCell_Mk4",
                    [m("VB_EchoCell_Mk3"), m("VB_EM_ResonanceCoil"), m("EFT_Military_cable"), m(*ab("Electra", 2)),
                     m(VEIN, 2)] + ELEC_TOOLS,
                    "Ячейка Mk III, прошитая жилами Изнанки и разрядным кварцем. 20 000 kE, выходит пустой: заряд старой "
                    "ячейки не сохраняется.", 15, BENCH))
    R.append(recipe(34, "Аккумулятор Эхо", "VB_EchoBattery",
                    [m("EFT_Carbattery"), m("VB_EM_ResonanceCoil"), m("VB_EM_EchoConduit", 2), m("EFT_Capacitors", 2),
                     m(*ab("Groza", 2)), m("EFT_Insulatingtape")] + ELEC_TOOLS,
                    "Автомобильный аккумулятор с резонатором Эхо, 20 000 kE. Ставится в любую машину; держит подушку "
                    "Echo Runner на стоянке, подзаряжается на станции Эхо.", 15, BENCH))
    return R


# ------------------------------------------------------------------ 2. экзоскелет (швейная)
SEW_TOOLS = [tool("EFT_Sewing", 0.05, 2), tool("DRL_Craft_Sewing_scissors", 0.0, 1),
             tool("DRL_Craft_Metalic_thimble", 0.0, 1)]
METAL_SCISSORS = tool("EFT_MetalScissors", 0.05, 2)


def e(cls, amount=1):
    """Ингредиент брони: при разборе вещи вернётся с шансом 0,5, как у сетов 16."""
    return comp(cls, amount, 0.5)


def colors(base):
    return [base] + [base + "_" + p for p in PALETTES]


def exo():
    """Экзоскелет Эхо. Баги №72: трофей любого босса в шлеме, жилете, куртке, штанах и сервоприводе. Баги №79: у каждой
    вещи 9 ингредиентов (маска 6) — ровно 3 ряда в окне «Раскрой», ткань и кожа ×3. Баги №78: время прежнее (не трогаем)."""
    R = []
    R.append(recipe(3201, "Шлем экзоскелета Эхо", "VB_EC_Exo_Helmet",
                    [e("VB_EM_ArmorPlate", 2), e("VB_EM_ArmorGlass"), e("VB_EM_DamperPad"), e("VB_EM_EchoConduit"),
                     e("VB_AB_Mat_LinceAgr", 2), e("EFT_Carbonecase"), e("EFT_Cloth_aramid", 3), e("EFT_Wires", 2),
                     trophy()] + SEW_TOOLS + [METAL_SCISSORS],
                    "Две бронепластины в углепластиковой основе на арамидном подбое, визор из бронестекла и линзового "
                    "стекла, замок — трофей босса. Носится с маской.", 15, SEWING))
    R.append(recipe(3202, "Маска экзоскелета Эхо", "VB_EC_Exo_Mask",
                    [e("EFT_Filtr"), e("VB_AB_Mat_LinceAgr", 2), e("EFT_Cloth_fleece", 6), e("MetalPlate"),
                     e("VB_EM_DamperPad"), e("EFT_SiliconTube")] + SEW_TOOLS + [METAL_SCISSORS],
                    "Фильтр с клапаном и линзы из линзового стекла под флисовой балаклавой. Защищает от газа, носится "
                    "со шлемом, входит в комплект экзо.", 10, SEWING, variants=colors("VB_EC_Exo_Mask")))
    R.append(recipe(3203, "Бронежилет экзоскелета Эхо", "VB_EC_Exo_Vest",
                    [e("VB_EM_ArmorPlate", 4), e("VB_AB_Mat_Voronka", 3), e("VB_EM_DamperPad", 2),
                     e("EFT_Cloth_aramid", 9), e("EFT_Cloth_cordura", 6), e("EFT_MetalParts"), e("EFT_Carbonecase"),
                     e("VB_EM_EchoConduit"), trophy()] + SEW_TOOLS + [METAL_SCISSORS],
                    "Четыре бронепластины, уплотнённые сжатым камнем, на арамиде и кордуре, замок плит — трофей босса. "
                    "Карго 80, входит в комплект экзо.", 15, SEWING, variants=colors("VB_EC_Exo_Vest")))
    R.append(recipe(3204, "Куртка экзоскелета Эхо", "VB_EC_Exo_Jacket",
                    [e("VB_EM_CarbonProfile", 3), e("VB_EM_ServoMotor", 3), e("VB_EM_EchoConduit"), e(VEIN, 3),
                     e("EFT_Cloth_ripstop", 9), e("EFT_Cloth_aramid", 6), e("EFT_Military_cable"), e("VB_EM_DamperPad", 2),
                     trophy()] + SEW_TOOLS,
                    "Карбоновый каркас рук с сервомоторами на военном кабеле, демпферы в локтях, по рукавам — жилы "
                    "Изнанки, замок — трофей босса. Карго 100.", 15, SEWING, variants=colors("VB_EC_Exo_Jacket")))
    R.append(recipe(3205, "Штаны экзоскелета Эхо", "VB_EC_Exo_Pants",
                    [e("VB_EM_CarbonProfile", 3), e("VB_EM_HydraulicCylinder", 3), e("VB_EM_DamperPad", 2), e(VEIN, 3),
                     e("EFT_Cloth_ripstop", 9), e("EFT_Cloth_aramid", 6), e("EFT_Military_cable"), e("VB_EM_EchoConduit"),
                     trophy()] + SEW_TOOLS,
                    "Карбоновый каркас ног с гидроцилиндрами, демпферы в коленях, жилы Изнанки, замок — трофей босса. "
                    "Карго 90, входит в комплект экзо.", 15, SEWING, variants=colors("VB_EC_Exo_Pants")))
    R.append(recipe(3206, "Перчатки экзоскелета Эхо", "VB_EC_Exo_Gloves",
                    [e("TannedLeather", 6), e("EFT_Cloth_aramid", 6), e("VB_EM_DamperPad"), e("EFT_Insulatingtape"),
                     e("EFT_MetalParts"), e("VB_EM_ServoMotor"), e("VB_EM_CarbonProfile"), e("VB_EM_EchoConduit"),
                     e("EFT_Wires")] + SEW_TOOLS,
                    "Кожа и арамид на карбоновых фалангах, проводка к сервомотору усиливает хват, демпферы на "
                    "костяшках. Входят в комплект экзо.", 8, SEWING, variants=colors("VB_EC_Exo_Gloves")))
    R.append(recipe(3207, "Ботинки экзоскелета Эхо", "VB_EC_Exo_Boots",
                    [e("TannedLeather", 9), e("VB_EM_ArmorPlate", 2), e("VB_EM_DamperPad", 2), e("VB_AB_Mat_Tramplin", 3),
                     e("EFT_Cloth_cordura", 3), e("EFT_MetalParts"), e("VB_EM_HydraulicCylinder"),
                     e("VB_EM_CarbonProfile", 2), e("VB_EM_EchoConduit")] + SEW_TOOLS + [METAL_SCISSORS],
                    "Кожа, бронепластины и гидроцилиндр на карбоновой голени, в подошве гравитационная галька: урон от "
                    "падения вдвое меньше.", 12, SEWING, variants=colors("VB_EC_Exo_Boots")))
    R.append(recipe(3208, "Сервопривод экзоскелета Эхо", "VB_EC_Exo_Servo",
                    [e("VB_EM_ExoController"), e("VB_EM_ResonanceCoil", 2), e("VB_EM_ServoMotor", 2),
                     e("VB_EM_HydraulicCylinder", 2), e("VB_EM_CarbonProfile", 2), e("VB_EM_EchoConduit", 2),
                     comp("OOTS_ART_Battery", 1, 0.0), e("EFT_Cloth_cordura", 3), trophy()] + SEW_TOOLS + [METAL_SCISSORS],
                    "Сердце привода — артефакт «Батарейка» на кордурных ремнях, замок — трофей босса. От ячеек Эхо несёт "
                    "50 кг (с полным экзо 85), бег быстрее.", 15, SEWING))
    return R


def belts():
    """Пояса для артефактов — верстак, раздел «Обмундирование» (Баги №67), цепочкой малый → средний → большой (Баги №74), время прежнее 8/10/12 с (Баги №78).
    Пояс с артефактами в баночках DRL в крафт не берёт (защита в DRL_CraftComponentResolver: расходник с навесным/грузом
    пропускается) — описание просит вынуть артефакты."""
    R = []
    R.append(recipe(604, "Малый пояс для артефактов Эхо", "VB_EC_ArtBelt_Small",
                    [e("TannedLeather", 3), e("EFT_Cloth_cordura", 2), e("EFT_OrgGlass", 2), e("VB_EM_EchoConduit"),
                     e("EFT_MetalParts"), tool("EFT_Sewing", 0.05)],
                    "Кожаный пояс на кордуре с 2 баночками из оргстекла, вешается на сервопривод. Артефакты в "
                    "баночках действуют.",
                    8, BENCH))
    R.append(recipe(605, "Средний пояс для артефактов Эхо", "VB_EC_ArtBelt_Medium",
                    [e("VB_EC_ArtBelt_Small"), e("TannedLeather", 2), e("EFT_OrgGlass", 2), e("VB_EM_EchoConduit"),
                     e("VB_AB_Mat_Lince", 2), tool("EFT_Sewing", 0.05)],
                    "Малый пояс + ещё 2 баночки из пустого стекла — 4 артефакта. Перед крафтом выньте артефакты из "
                    "баночек: пояс с ними не берётся.", 10, BENCH))
    R.append(recipe(606, "Большой пояс для артефактов Эхо", "VB_EC_ArtBelt_Large",
                    [e("VB_EC_ArtBelt_Medium"), e("TannedLeather", 2), e("EFT_OrgGlass", 2), e("VB_EM_EchoConduit"),
                     e("VB_AB_Mat_Zastoy", 2), tool("EFT_Sewing", 0.05)],
                    "Средний пояс + 2 баночки, где застывший пузырь держит артефакт в покое — 6 артефактов. Перед крафтом "
                    "выньте артефакты из баночек.", 12, BENCH))
    return R


# ------------------------------------------------------------------ 3. Echo Runner (подъёмник)
LIFT_TOOLS = [tool("EFT_Advwrench", 0.10), tool("EFT_Ratchetwrench", 0.10), tool("EFT_Screwdriver_krest", 0.05),
              tool("EFT_Electric_Drill", 0.10), tool("Blowtorch", 0.10), tool("LugWrench", 0.05),
              tool("DRL_Craft_Car_Jack", 0.0, 1)]


def car():
    """Echo Runner (Баги №77, 2026-11-25 — сложнее): кузов = всё, что идёт на Interceptor (на нём модель), + детали Эхо;
    детали и части для полёта — больше материалов. Время прежнее (Баги №78)."""
    R = []
    b = lambda cls, n: comp(cls, n, 0.0)                                        # noqa: E731 — разбор кузова выключен
    body = [
        # обычная машина — как Interceptor 4001
        b("MetalPlate", 30), b("EFT_MetalParts", 15), b("EFT_Bolts", 15), b("EFT_model_nuts", 15),
        b("EFT_Screw_pack", 10), b("EFT_Poxearm", 5), b("EFT_Wires", 10), b("EFT_Military_cable", 5),
        b("EFT_Insulatingtape", 10), b("EFT_Rele", 5), b("EFT_Capacitors", 4), b("EFT_Magnet", 2),
        b("EFT_Hose_military", 5), b("EFT_SiliconTube", 5), b("EFT_PressureGauge", 1), b("EngineOil", 5),
        b("SparkPlug", 8), b("EFT_Gasolin", 1), b("TannedLeather", 10), b("EFT_Mounting_foam", 5),
        b("CarRadiator", 1), b("EFT_Engine", 1), b("HeadlightH7", 2),
        # Эхо
        b("VB_EM_PowerFrame", 6), b("VB_EM_BodyPanel", 4), b("VB_EM_ArmorGlass", 3), b("VB_EM_ECU", 2),
        b("VB_EM_EchoConduit", 4), b("VB_EM_ResonanceCoil", 2), b(VEIN, 8),
        # трофей один: список ×2 при выборе варианта DRL (CloneWithVariation) требует ДВА ОДИНАКОВЫХ трофея
        trophy()] + LIFT_TOOLS
    R.append(recipe(4301, "Echo Runner", "VB_EchoRunner", body,
                    "Летающая машина: на 4 антиграв-модулях с трубопроводами Изнанки и баками Эхо поднимается в воздух. "
                    "Капот, двери и колёса — в доп. деталях.", 300, LIFT, attached=[4302, 4303, 4304, 4305]))
    part = lambda cls, n: comp(cls, n, 0.3)                                     # noqa: E731 — как у деталей Interceptor
    R.append(recipe(4302, "Капот Echo Runner", "VB_EchoRunner_Hood",
                    [part("VB_EM_BodyPanel", 3), part("MetalPlate", 4), part("EFT_MetalParts", 2), part("EFT_Bolts", 4),
                     part("EFT_Poxearm", 2)],
                    "Три кузовные панели на стальном каркасе, на петлях и болтах, швы проклеены холодной сваркой.", 8, LIFT))
    for rid, cls, who in ((4303, "VB_EchoRunner_Door_Driver", "водителя"),
                          (4304, "VB_EchoRunner_Door_CoDriver", "пассажира")):
        R.append(recipe(rid, "Дверь %s Echo Runner" % who, cls,
                        [part("VB_EM_BodyPanel", 2), part("VB_EM_ArmorGlass", 1), part("MetalPlate", 4),
                         part("EFT_MetalParts", 2), part("EFT_Screw_pack", 2), part("EFT_Wires", 2),
                         part("TannedLeather", 2)],
                        "Две кузовные панели с бронестеклом, проводка стеклоподъёмника, кожаная обивка, замок и петли.",
                        10, LIFT))
    R.append(recipe(4305, "Колесо Echo Runner", "VB_EchoRunner_Wheel",
                    [part("VB_EM_ShockAbsorber", 1), part("TireRepairKit", 4), part("MetalPlate", 4), part("EFT_Bolts", 4),
                     part("EFT_model_nuts", 4), part("EFT_SiliconTube", 1)],
                    "Шина на диске с амортизатором ступицы. Машине нужно 4 колеса.", 6, LIFT))
    # энергетические части (Баги №68): отдельные рецепты раздела, не доп. детали кузова — ставятся позже, для полёта
    R.append(recipe(4306, "Антиграв-модуль ДВ-4", "VB_EchoRunner_GravModule",
                    [part("VB_EM_ResonanceCoil", 1), part("VB_AB_Mat_Voronka", 2), part("VB_AB_Mat_Tramplin", 3),
                     part("VB_EM_EchoConduit", 2), part("VB_EM_ServoMotor", 1), part("VB_EM_BodyPanel", 1),
                     part("EFT_MetalParts", 2), part("EFT_Military_cable", 1)],
                    "Резонансная катушка раскручивает гравитационную гальку и сжатый камень, сервомотор наклоняет "
                    "модуль. Для полёта нужно 4 модуля.", 12, LIFT))
    R.append(recipe(4307, "Бак Эхо", "VB_EchoRunner_Tank",
                    [part("VB_EM_BodyPanel", 2), part("VB_EM_ArmorGlass", 2), part("VB_EM_EchoConduit", 2),
                     part("EFT_PressureGauge", 1), part(VEIN, 3), part("EFT_Poxearm", 2), part("EFT_Capacitors", 2),
                     part("VB_EM_ResonanceCoil", 1)],
                    "Бронированная колба с катушкой-накопителем для энергии Эхо. Выходит пустой — заряжается на станции "
                    "Эхо. Машина берёт два бака.", 10, LIFT))
    for rid, cls, side in ((4308, "VB_EchoRunner_Pipes_L", "левый"), (4309, "VB_EchoRunner_Pipes_R", "правый")):
        R.append(recipe(rid, "Трубопровод Изнанки (%s)" % side, cls,
                        [part("VB_EM_EchoConduit", 3), part("EFT_SiliconTube", 3), part(VEIN, 3),
                         part("EFT_Hose_military", 1), part("EFT_MetalParts", 1)],
                        "Светящиеся трубки вдоль борта: по ним энергия Эхо идёт от баков к антиграв-модулям. "
                        "Нужен для полёта.", 8, LIFT))
    R.append(recipe(4310, "Трубопровод Изнанки (мотор)", "VB_EchoRunner_Pipes_Engine",
                    [part("VB_EM_EchoConduit", 4), part("EFT_SiliconTube", 2), part(VEIN, 3),
                     part("EFT_Hose_military", 2), part("EFT_PressureGauge", 1), part("EFT_MetalParts", 1)],
                    "Узел трубок над мотором: связывает баки Эхо с бортовыми трубопроводами. Нужен для полёта.",
                    10, LIFT))
    return R


# ------------------------------------------------------------------ разделы
def category(name, icon, ids, sound, fon):
    return {"Name": name, "Icon": icon, "ShowAll": 0, "UseList": 1, "ShowOnlyAvailableCraft": 0,
            "ShowOnlyAvailableStudy": 0, "CategoryFonImagePath": "set:drl_craft_work image:work",
            "CategoryCraftFonImagePath": fon, "CategoryCraftSoundSet": sound, "List": list(ids)}


def known_classes(cfg16):
    """Классы, которые точно есть на 16: свои моды (config.cpp/.hpp) + всё, что уже ходит в крафте 16."""
    have = set()
    for rel in (r"mod_vb_Echo\VB_EchoMaterials", r"mod_vb_Echo\VB_EchoClothes", r"mod_vb_Echo\VB_EchoCore",
                r"mods_vb_vehicles\mod_vb_EchoRunner\VB_EchoRunner",
                r"mods_vb_vehicles\mod_vb_EchoVehicleParts\VB_EchoVehicleParts", r"mod_vb_AnomaliesBosses\VB_AnomaliesBosses",
                r"mod_vb_Rift"):
        for root, _, files in os.walk(os.path.join(MODDING, rel)):
            for fn in files:
                if fn.lower().endswith((".cpp", ".hpp")):
                    txt = io.open(os.path.join(root, fn), encoding="utf-8", errors="replace").read()
                    have |= set(re.findall(r"^\s*class\s+(\w+)\s*:", txt, re.M))
    for r in cfg16:
        have.add(r["Result"])
        have |= set(r.get("ResultClasses") or [])
        for c in r["CraftComponents"]:
            have |= set(c["Classnames"])
    return have


def ru_names():
    out = {}
    for rel in (r"mod_vb_Echo\VB_EchoMaterials\stringtable.csv", r"mod_vb_Echo\VB_EchoClothes\stringtable.csv",
                r"mod_vb_AnomaliesBosses\VB_AnomaliesBosses\stringtable.csv",
                r"mods_vb_vehicles\mod_vb_EchoRunner\VB_EchoRunner\stringtable.csv"):
        rows = list(csv.reader(io.open(os.path.join(MODDING, rel), encoding="utf-8")))
        ri = [h.lower() for h in rows[0]].index("russian")
        for r in rows[1:]:
            if r:
                out[r[0]] = r[ri]
    return out


def load(path):
    raw = io.open(path, "rb").read()
    return json.loads(raw.decode("utf-8-sig")), raw.startswith(b"\xef\xbb\xbf")


def dump(obj, path, bom=False):
    with io.open(path, "w", encoding="utf-8-sig" if bom else "utf-8", newline="\r\n") as f:
        f.write(json.dumps(obj, ensure_ascii=False, indent=4))


def per_station(cfg, cats):
    """Сколько разделов увидит каждая станция (раздел со своими рецептами, «Все рецепты» скрыт)."""
    st = {r["RecipeId"]: r["WorkbenchClasses"] for r in cfg}
    out = {}
    for c in cats:
        if c.get("ShowAll"):
            continue
        for s in {s for i in c.get("List", []) for s in st.get(i, [])}:
            out[s] = out.get(s, 0) + 1
    return out


def main():
    cfg16, cfg_bom = load(os.path.join(SRV16, "CraftConfig.json"))
    cat16, cat_bom = load(os.path.join(SRV16, "CraftCategories.json"))

    mats, en, ex, bl, cr = materials(), energy(), exo(), belts(), car()
    new = mats + en + ex + bl + cr
    ids = {r["RecipeId"] for r in new}
    # свои прошлые рецепты (по результату) снимаются целиком — и старые номера, если номер сменился
    own = lambda r: r["Result"].startswith(("VB_EM_", "VB_EC_", "VB_EchoRunner", "VB_EchoCell_", "VB_EchoBattery"))  # noqa: E731
    n_before = len(cfg16)
    old_own = {r["RecipeId"] for r in cfg16 if own(r)}
    cfg16 = [r for r in cfg16 if not own(r)]
    clash = [r["RecipeId"] for r in cfg16 if r["RecipeId"] in ids]
    assert not clash, ("номера заняты чужими рецептами", clash)
    for c in cat16:
        if "List" in c:
            c["List"] = [i for i in c["List"] if i not in old_own and i not in ids]
    gear = next(c for c in cat16 if c["Name"] == BELT_CATEGORY)
    gear["List"] += [r["RecipeId"] for r in bl]
    mcat = next(c for c in cat16 if c["Name"] == MAT_CATEGORY)
    mcat["List"] += [r["RecipeId"] for r in mats]                 # Баги №73: в общий раздел

    cats = [
        ("Материалы", category("Энергия Эхо", "DRL_Core/Icon/drl_battery-pack.edds", [r["RecipeId"] for r in en],
                                   "DRLCraft_Generic1_SoundSet", "set:drl_craft_recipemanu_part image:recipemanu_part")),
        ("Альфа", category("Экзоскелет Эхо", "DRL_Core/Icon/drl_battle-mech.edds", [r["RecipeId"] for r in ex],
                           "DRLCraft_Paper_SoundSet", "set:drl_craft_recipemanu_armor image:recipemanu_armor")),
        ("Mustang", category("Echo Runner", "DRL_Core/Icon/drl_f1-car.edds", [r["RecipeId"] for r in cr],
                             "DRLCraft_Metal_SoundSet", "set:drl_craft_recipemanu_armor image:recipemanu_armor")),
    ]

    # проверки: классы существуют, описания влезают, у вариантов есть база
    have = known_classes([r for r in cfg16 if r["RecipeId"] not in ids])
    miss = sorted({n for r in new for n in [r["Result"]] + r["ResultClasses"] + [x for c in r["CraftComponents"]
                                                                                  for x in c["Classnames"]]} - have)
    assert not miss, ("классов нет ни в наших модах, ни в крафте 16", miss)
    long_desc = [(r["RecipeId"], len(r["Description"])) for r in new if len(r["Description"]) > DESC_MAX]
    assert not long_desc, ("описание длиннее %d" % DESC_MAX, long_desc)
    for r in new:
        assert not r["ResultClasses"] or r["ResultClasses"][0] == r["Result"], r["RecipeId"]
        n_ing = len([c for c in r["CraftComponents"] if c["Delete"] == 1])
        # швейная: «Раскрой» кладёт ингредиенты по 3 в ряд, 9 = 3 ряда (больше — лист вылезет вниз, Баги №79);
        # верстак: материалы 4–8 (Баги №80); подъёмник — список без предела (у машин 16 до 32)
        cap = {"DRL_Craft_Sewing": 9, "DRL_Craft_Workbench": 8}.get(r["WorkbenchClasses"][0], 40)
        assert n_ing <= cap, ("ингредиентов больше %d" % cap, r["RecipeId"], n_ing)

    # сборка: свои рецепты заменяются, файл остаётся по порядку ID; свои разделы — после соседа по смыслу
    full = sorted([r for r in cfg16 if r["RecipeId"] not in ids] + new, key=lambda r: r["RecipeId"])
    names = {c["Name"] for _, c in cats} | {"Материалы Эхо"}       # «Материалы Эхо» снят (Баги №73)
    fullcat = [c for c in cat16 if c["Name"] not in names]
    for after, c in cats:
        pos = next(i for i, x in enumerate(fullcat) if x["Name"] == after)
        fullcat.insert(pos + 1, c)

    st = per_station(full, fullcat)
    over = {s: n for s, n in st.items() if n > MAX_CATS}
    assert not over, ("разделов на станции больше %d" % MAX_CATS, over)

    dump(new, os.path.join(HERE, "echo_recipes_16.json"))
    dump([c for _, c in cats], os.path.join(HERE, "echo_categories_16.json"))
    dump(full, os.path.join(HERE, "CraftConfig_16.json"), cfg_bom)
    dump(fullcat, os.path.join(HERE, "CraftCategories_16.json"), cat_bom)

    ru = ru_names()
    disp = {}
    for k, v in ru.items():
        disp[k.replace("STR_", "")] = v

    def nm(cls):
        if cls in TROPHIES:
            return "трофей босса"
        for key in (cls, cls.replace("VB_EchoRunner", "VB_ER")):
            if key in disp:
                return disp[key]
        return {"VB_RiftVein": "жилы Изнанки", "OOTS_ART_Battery": "артефакт «Батарейка»",
                "VB_EchoCell_Mk1": "энергоячейка Mk I", "VB_EchoCell_Mk2": "энергоячейка Mk II",
                "VB_EchoCell_Mk3": "энергоячейка Mk III"}.get(cls, cls)

    lines = ["# Крафт Эхо на сервере 16 — таблица на проверку", "",
             "Сгенерировано `make_echo_craft16.py` (утверждено 2026-11-25, правки по Баги.md №67–80). ★ — есть только "
             "на 16 (аномалии, боссы, Изнанка, артефакты).", "Инструменты не расходуются; на швейной ещё нитки "
             "«Красавчик», ножницы и напёрсток станка.", ""]
    star = ("VB_AB_", "VB_Rift", "OOTS_ART_")
    for title, rs in (("Материалы Эхо — верстак, раздел «Материалы»", mats), ("Энергия Эхо — верстак", en), ("Экзоскелет Эхо — швейная", ex),
                      ("Пояса для артефактов — верстак, раздел «Обмундирование»", bl),
                      ("Echo Runner — подъёмник", cr)):
        lines += ["## " + title, "", "| ID | Результат | Ингредиенты | Инструменты | Время, с |", "|---|---|---|---|---|"]
        for r in rs:
            ing = ", ".join(("★" if c["Classname"].startswith(star) else "") + "%s ×%d" % (nm(c["Classname"]), c["Amount"])
                            for c in r["CraftComponents"] if c["Delete"] == 1)
            sew = ("EFT_Sewing", "DRL_Craft_Sewing_scissors", "DRL_Craft_Metalic_thimble") if r["WorkbenchClasses"] == SEWING else ()
            tl = ", ".join(c["Classname"] for c in r["CraftComponents"] if c["Delete"] == 0 and c["Classname"] not in sew)
            res = r["RecipeName"] + (" ×%d" % r["ResultCount"] if r["ResultCount"] > 1 else "")
            if r["ResultClasses"]:
                res += " (4 расцветки)"
            lines.append("| %d | %s | %s | %s | %d |" % (r["RecipeId"], res, ing, tl or "—", r["CraftTimeSec"]))
        lines.append("")
    with io.open(os.path.join(HERE, "echo_craft_16.md"), "w", encoding="utf-8", newline="\r\n") as f:
        f.write("\n".join(lines) + "\n")

    print("рецептов своих: %d, всего на 16: %d (было %d); разделов: %d" % (len(new), len(full), n_before, len(fullcat)))
    print("разделов на станцию:", ", ".join("%s %d" % (s.replace("DRL_Craft_", ""), n) for s, n in sorted(st.items())))


if __name__ == "__main__":
    sys.stdout.reconfigure(encoding="utf-8")
    main()
