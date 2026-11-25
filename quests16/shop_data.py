# -*- coding: utf-8 -*-
"""Лавка лагеря в крафте DRL сервера 16 (ТЗ: Квесты_16.md, разделы 4.8 и 2а «Экономика монет»). Генератор: make_shop16.py.

Экономика v2.0 (2026-11-14): курс 10:1 — 1 з = 10 с = 100 м. Монета DayZavr — отдельный предмет на клетку (не
стопка), поэтому курс 100:1 заставлял носить сотни монет. При 10:1 любая сумма = N золотых + до 9 серебряных + до
9 медных, размен выдаёт не больше 10 монет.
Цены ниже — В ЗОЛОТЕ с десятыми и сотыми: 1.5 = 1 з 5 с, 0.02 = 2 м. Это старые цены магазина квестов ÷ 10 (точно).
Сеты одежды разбиты на вещи так, что вещи сета в сумме дают его цену (общие вещи сетов стоят одинаково).
Одинаковые вещи разных цветов по одной цене — один рецепт с выбором варианта (ResultClasses).
Оружие: рецепт на ствол; магазины и обвес набора — навесные рецепты (AttachedRecipeIds); цена ствола = цена
набора минус обвес, поэтому полный набор стоит как старый ÷ 10.
"""

WB, SEW = "DRL_Craft_Workbench", "DRL_Craft_Sewing"
OLD_DIV = 10            # старая цена в золоте / OLD_DIV = новая цена в золоте
RATE = 10               # 1 з = RATE с, 1 с = RATE м

CATEGORIES = {  # ключ: (название, иконка, звук). Одежда — по сетам, как категории сетов крафта (решение 2026-11-14)
    "ghost": ("Лавка: сет GHOST", "DRL_Core/Icon/drl_ghost.edds", "DRLCraft_Paper_SoundSet"),
    "swat": ("Лавка: сет SWAT", "DRL_Core/Icon/drl_chest-armor.edds", "DRLCraft_Paper_SoundSet"),
    "merc": ("Лавка: сет наёмника", "DRL_Core/Icon/drl_polo-shirt.edds", "DRLCraft_Paper_SoundSet"),
    "palace": ("Лавка: сет PALACE", "DRL_Core/Icon/drl_shirt.edds", "DRLCraft_Paper_SoundSet"),
    "raijin": ("Лавка: сет RAIJIN", "DRL_Core/Icon/drl_fur-shirt.edds", "DRLCraft_Paper_SoundSet"),
    "bags": ("Лавка: рюкзаки и ремни", "DRL_Core/Icon/drl_backpack.edds", "DRLCraft_Paper_SoundSet"),
    "weapons": ("Лавка: оружие", "DRL_Core/Icon/drl_rifle.edds", "DRLCraft_Metal_SoundSet"),
    "addons": ("Лавка: обвесы и магазины", "DRL_Core/Icon/drl_machine-gun-magazine.edds", "DRLCraft_Metal_SoundSet"),
    "base": ("Лавка: для базы", "DRL_Core/Icon/drl_paint-roller.edds", "DRLCraft_Generic1_SoundSet"),
}

# Обмен монет — у Босса квестами 16001–16004 (решение пользователя 2026-11-14): в крафте его нет.
# Рецепты 5000–5003 и категория «Обмен монет» снимаются с сервера при выкладке.
EXCHANGE = []
# 2026-11-25 (решение пользователя): мод OOTS уходит, его вещи на 16 — только в скупке у Коллекционера
# (Trader/TraderConfig5_Collector.txt). Экзоскелет OOTS (5110–5116), сервопривод (5200) и модули (5210–5215) сняты.
OBSOLETE_CATEGORIES = {"Обмен монет", "Лавка: одежда", "Лавка: снаряжение", "Лавка: экзоскелет", "Лавка: модули OOTS"}


def clothes_category(rid):
    """Категория лавки для одежды и снаряжения по Id рецепта."""
    if 5100 <= rid <= 5106:
        return "ghost"
    if rid == 5114 or 5120 <= rid <= 5122:
        return "bags"
    if 5130 <= rid <= 5135:
        return "merc"
    if 5136 <= rid <= 5138:
        return "palace"
    if 5139 <= rid <= 5140:
        return "raijin"
    if 5150 <= rid <= 5157:
        return "swat"
    raise ValueError("нет категории для %d" % rid)

# Одежда (швейная машинка — у неё нет хранилища, монеты при себе): Id, название, варианты, цена в золоте
CLOTHES = [
    (5100, "Куртка GHOST", ["Candy_Set_Ghost_Ghosted_Jacket_100"], 11),
    (5101, "Штаны GHOST", ["Candy_Set_Ghost_Ghosted_Pants_100"], 9),
    (5102, "Шлем «Вулкан»", ["IMP_Helmet_Vulkan_Green"], 7),
    (5103, "Маска GHOST", ["Candy_Set_Ghost_Ghosted_Maskv2"], 5),
    (5104, "Ботинки GHOST", ["Candy_Set_Ghost_Ghosted_Boots"], 3),
    (5105, "Перчатки GHOST", ["Candy_Set_Ghost_Ghosted_Gloves"], 3),
    (5106, "Подсумок GHOST", ["Candy_Set_Ghost_Ghosted_Pouch1"], 2),
    (5114, "Кобура на бронежилет", ["PlateCarrierHolster_Black"], 0.7),
    (5120, "Рюкзак 6Ш118", ["FOG_Bag_6ShBagT_Green", "FOG_Bag_6ShBagT_BlackEMR", "FOG_Bag_6ShBagT_Black",
                            "FOG_Bag_6ShBagT_RG", "FOG_Bag_6ShBagT_CB", "FOG_Bag_6ShBagT_White",
                            "FOG_Bag_6ShBagT_EMR_White"], 22),
    (5121, "Тяжёлая ременная система", ["IMP_Belt_BeltSystem_Large_80_Black", "IMP_Belt_BeltSystem_Large_80_NWU",
                                        "IMP_Belt_BeltSystem_Large_80_EMR", "IMP_Belt_BeltSystem_Large_80_MARPAT",
                                        "IMP_Belt_BeltSystem_Large_80_UCP"], 3.5),
    (5122, "Оружейный ремень", ["IMP_Sling_Back", "IMP_Sling_Front"], 15),
    (5130, "Кепка наёмника", ["kr_mercguard_cap_jingle", "kr_mercguard_cap_darkny", "kr_mercguard_cap_creamy"], 1.5),
    (5131, "Маска наёмника", ["kr_mercguard_mask_jingle", "kr_mercguard_mask_darkny", "kr_mercguard_mask_creamy"], 2),
    (5132, "Худи наёмника", ["kr_mercguard_hoodie_jingle", "kr_mercguard_hoodie_darkny",
                             "kr_mercguard_hoodie_creamy"], 4.5),
    (5133, "Жилет наёмника", ["kr_mercguard_vest_jingle", "kr_mercguard_vest_darkny", "kr_mercguard_vest_creamy"], 5),
    (5134, "Штаны наёмника", ["kr_mercguard_pants_jingle", "kr_mercguard_pants_darkny",
                              "kr_mercguard_pants_creamy"], 3.5),
    (5135, "Ботинки наёмника", ["kr_mercguard_boots_jingle", "kr_mercguard_boots_darkny",
                                "kr_mercguard_boots_creamy"], 2),
    (5136, "Штаны PALACE", ["kr_mercguard_pants_palace"], 3),
    (5137, "Худи PALACE", ["kr_mercguard_hoodie_palace"], 4),
    (5138, "Маска PALACE", ["kr_mercguard_mask_palace"], 1.5),
    (5139, "Штаны RAIJIN", ["kr_mercguard_pants_japBlcGrn"], 4),
    (5140, "Худи RAIJIN", ["kr_mercguard_hoodie_japBlcGrn"], 4.5),
    (5150, "Шлем SWAT", ["kr_swatsuit_helmet_black"], 2.5),
    (5151, "Маска SWAT", ["kr_swatsuit_mask_black"], 1.5),
    (5152, "Противогаз SWAT", ["kr_swatsuit_gasmask"], 2),
    (5153, "Жилет SWAT", ["kr_swatsuit_vest", "kr_swatsuit_vest_black"], 3),
    (5154, "Боевая рубашка SWAT", ["kr_swatsuit_combatShirt_black", "kr_swatsuit_combatShirt_green",
                                   "kr_swatsuit_combatShirt_tan", "kr_swatsuit_combatShirt_multicam",
                                   "kr_swatsuit_combatShirt_nwu", "kr_swatsuit_combatShirt",
                                   "kr_swatsuit_combatShirt_surpat"], 2),
    (5155, "Штаны SWAT", ["kr_swatsuit_pants_black", "kr_swatsuit_pants_green", "kr_swatsuit_pants_tan",
                          "kr_swatsuit_pants_multicam", "kr_swatsuit_pants_nwu", "kr_swatsuit_pants_ucp",
                          "kr_swatsuit_pants_surpat"], 2),
    (5156, "Ботинки SWAT", ["kr_swatsuit_boots_black"], 1),
    (5157, "Перчатки SWAT", ["kr_Gloves_SWAT_Gloves_black", "kr_swatsuit_gloves_green", "kr_swatsuit_gloves",
                             "kr_swatsuit_gloves_Multicam", "kr_swatsuit_gloves_NWU", "kr_swatsuit_gloves_UCP",
                             "kr_swatsuit_gloves_Surpat"], 1),
]
# Класс, исправленный при переносе (в старом квесте 18031 был с лишней точкой). В чужом моде не проверяем —
# генератор предупреждает, пользователь проверяет в игре.
UNVERIFIED = {"kr_swatsuit_combatShirt_multicam"}

# Снаряжение (верстак, в нём хранилище на 100 клеток): Id, название, класс, цена в золоте
# 2026-11-25: сервопривод OOTS (5200) и модули OOTS (5210–5215) сняты — вещи OOTS только в скупке у Коллекционера.
GEAR = []
# Описания рецептов снаряжения (по умолчанию пусто — окно покажет описание предмета; слова «Лавка лагеря» убраны 2026-11-21)
GEAR_DESC = {}
# Убраны из лавки по решению пользователя 2026-11-14: отпугиватель мутантов (5220), граната РГД-5 (5221),
# контейнер для артефактов (5201), батарея экзоскелета (5202), зарядка батареи экзоскелета (5203).
# 2026-11-25: экзоскелет OOTS (5110–5116), сервопривод (5200), модули OOTS (5210–5215).
RECHARGE = None

# Оружие (верстак): старые квесты 18301–18335 по порядку, Id 5300 + (номер - 18301); название ствола
WEAPON_NAMES = {
    "SP_Gauss": "Gauss Rifle", "DZVR_PTRS41": "ПТРС-41", "DZVR_OSV96": "ОСВ-96", "DZVR_M87R": "McMillan M87R",
    "DZVR_SuperSniper": "WC Super Sniper", "DZVR_M96": "M96 Windrunner", "DZVR_MAR10": "MAR-10",
    "DZVR_WA2000": "WA2000", "DZVR_Tac21": "MDT TAC 21", "DZVR_T5000": "T5000", "DZVR_Delta5": "DELTA 5 Pro",
    "DZVR_SAKOTRG_338": "SAKO TRG M10", "BO_M134": "M134", "BO_MG42_308": "MG-3", "BO_M240B": "M240B",
    "DZVR_RPD": "РПДН", "DZVR_M249": "M249", "DZVR_SR25": "SR-25", "DZVR_HKG28": "HK G28", "DZVR_FN2000": "FN2000",
    "DZVR_M4_Tact": "M4 Tactical V2", "DZVR_M4_Sling": "M4 Tactical V1", "DZVR_MCX308_Tan": "MCX Spear 308",
    "DZVR_L85_RIS": "L85", "DZVR_OC14Groza_762": "ОЦ-14 «Гроза-1»", "DZVR_PPSKN": "ППС-42", "DZVR_AA12": "AA-12",
    "KOD_Raptr": "RAPTR 12", "DZVR_SPAS12_SemiAuto": "SPAS-12", "DZVR_BenelliM4": "Benelli M4", "DZVR_UZI": "UZI",
    "DZVR_Sten": "STEN Mk III", "DZVR_Glock18C": "Glock 18C", "DZVR_APS": "АПС", "DZVR_ChiappaRhino": "Chiappa Rhino",
}
# Обвес: класс -> (название, цена в золоте). Магазины считаются отдельно (MAG_*).
ADDONS = {
    "AD_LeupoldMk4": ("Прицел Leupold Mk4", 1.5), "AD_NFATACR": ("Прицел NF ATACR", 1.8),
    "AD_NFATACR_1_8": ("Прицел NF ATACR 1-8", 1.4),
    "AD_NFATACR_1_8_MosinMount": ("Прицел NF ATACR 1-8 (крепление ПТРС)", 1.4),
    "AD_SpecterDR": ("Прицел Specter DR", 1.0), "AD_ACOG_RMR": ("Прицел ACOG с RMR", 1.2),
    "AD_XPS34": ("Коллиматор XPS3", 0.6), "AD_XPS34_HHS": ("Коллиматор XPS3 с магнификатором", 0.9),
    "AD_MRS_B13": ("Коллиматор MRS на B-13", 0.6), "AD_RMR": ("Коллиматор RMR", 0.4),
    "AD_RMR_LM": ("Коллиматор RMR (крепление)", 0.5), "AD_Pilad": ("Прицел Pilad", 0.8),
    "DZVR_Suppressor338": ("Глушитель .338", 1.2), "DZVR_Suppressor308": ("Глушитель .308", 1.0),
    "DZVR_Suppressor556": ("Глушитель 5.56", 0.8), "DZVR_Suppressor545_762": ("Глушитель 5.45/7.62", 0.8),
    "DZVR_Suppressor12ga": ("Глушитель 12 калибра", 0.6), "DZVR_SuppressorPistol": ("Пистолетный глушитель", 0.4),
    "KOD_M4Suppressor_Black": ("Глушитель M4 (чёрный)", 0.8), "DZVR_MCX308_Suppressor": ("Глушитель MCX", 1.0),
    "UniversalLight": ("Тактический фонарь", 0.2), "TLRLight": ("Пистолетный фонарь", 0.2),
    "DZVR_LaserSight": ("Лазерный целеуказатель", 0.6), "DZVR_UBR_Stock": ("Приклад UBR", 0.4),
    "DZVR_MCX308_Bttstck_Tan": ("Приклад MCX (песочный)", 0.4), "KOD_RIS_Bipod": ("Сошки RIS", 0.3),
    # батарейки — расходники за медь
    "FC_Battery9V_X5": ("Батарейки 9 В ×5", 0.08), "FC_Battery9V_X25": ("Батарейки 9 В ×25", 0.3),
    "Battery9V": ("Батарейка 9 В", 0.02),
}
MAG_PRICE, MAG_BIG_PRICE, MAG_BIG_ROUNDS = 0.3, 0.4, 50   # магазин; большой (от 50 патронов), в золоте
MAG_OVERRIDE = {"SP_Mag_Gauss_15rnd": 0.5, "DZVR_Mag_D60_60rnd": 0.5}
# Патроны (пачкой): класс -> (название, цена за штуку в золоте)
AMMO = {"SP_Ammo_Gauss_500dmg": ("Заряд Gauss 500", 0.1), "SP_Ammo_Gauss_20kdmg": ("Заряд Gauss 20k", 0.2)}
WEAPON_MIN_SHARE = 0.5   # ствол не дешевле половины набора

# Для базы (верстак): группы по началу класса; цена за штуку — старая ÷ OLD_DIV
BASE_GROUPS = [  # Id, название, функция-условие
    (5500, "Шпатлёвка для крыши", lambda c: "plaster" in c.lower()),
    (5501, "Обои: аниме", lambda c: c.startswith("wallpaperBBP_anime_")),
    (5502, "Обои: 18+", lambda c: c.startswith("wallpaper_2024_Erotic_")),
    (5503, "Обои: двери", lambda c: c.startswith("wallpaper_Door")),
    (5504, "Обои: DayZavr", lambda c: c.startswith("wallpaper_Dayzavr")),
    (5505, "Обои: 2024", lambda c: c.startswith("wallpaper_2024_")),
    (5506, "Обои: разные", lambda c: "wallpaper" in c.lower()),
    (5507, "Ковры", lambda c: "carpet" in c.lower()),
    (5508, "Сундук для монет", lambda c: c == "DZVR_CoinsChest"),
]


# ---------------------------------------------------------------------------- кроме монет (решение 2026-11-14)
# Чтобы запасы монет не покупали топ сами по себе: дорогие вещи требуют то, что добывается только игрой —
# EFT-детали и ткани (лут, конвой, аирдроп, бандиты, тайники), материалы аномалий (поля боссов), жилу Изнанки,
# трофей любого босса. Ступени по цене вещи:
#   дешевле 1 з — только монеты (расходники, магазины, коллиматоры, обои);
#   1–15 з     — + 1–2 обычные EFT-детали или ткани;
#   15–25 з    — + редкие EFT-детали / материалы аномалий;
#   от 25 з    — + трофей любого босса.
# Не больше 8 ингредиентов в рецепте вместе с монетами (правило пользователя для DRL). Разбор ничего не возвращает.
TROPHY = "TROPHY"   # трофей любого из 14 боссов (Classnames — все трофеи, как в рецепте детектора «Невод»)
TROPHY_CLASSES = ["VB_AB_Trophy_Ricar_Acid", "VB_AB_Trophy_Ricar_Rad", "VB_AB_Trophy_Tenevnik_Psi",
                  "VB_AB_Trophy_Tenevnik_Rad", "VB_AB_Trophy_Shum_Ice", "VB_AB_Trophy_Shum_Gravity",
                  "VB_AB_Trophy_MutaWolf_Ice", "VB_AB_Trophy_MutaWolf_Fire", "VB_AB_Trophy_MozgoTras_Psi",
                  "VB_AB_Trophy_MozgoTras_Storm", "VB_AB_Trophy_MozgoTrasRed_Fire", "VB_AB_Trophy_MozgoTrasRed_Gravity",
                  "VB_AB_Trophy_Ent_Storm", "VB_AB_Trophy_Ent_Acid"]
EXTRA_RU = {
    TROPHY: "трофей любого босса",
    "EFT_Cloth_paracord": "паракорд", "EFT_Cloth_fleece": "флис", "EFT_Cloth_cordura": "кордура",
    "EFT_Cloth_ripstop": "рипстоп", "EFT_Cloth_aramid": "арамидная ткань", "EFT_Filtr": "фильтр противогаза",
    "EFT_Bolts": "болты", "EFT_model_nuts": "гайки", "EFT_MetalParts": "металлические детали", "EFT_Klean": "смазка FireClean",
    "EFT_Poxearm": "холодная сварка", "EFT_Lens": "фотообъектив", "EFT_OrgGlass": "оргстекло",
    "EFT_Capacitors": "конденсаторы", "EFT_Rele": "реле", "EFT_Military_cable": "военный кабель", "EFT_Engine": "электродвигатель",
    "EFT_Magnet": "электромагнит", "EFT_Modem": "сетевой модем", "EFT_Bloodset": "пакет крови",
    "Paper": "бумага", "Rag": "тряпки",
    "OOTS_Exo_Servoprivod_black": "обычный сервопривод экзоскелета", "VB_RiftVein": "жила Изнанки", "VB_AB_Mat_Snow": "вечный лёд", "VB_AB_Mat_Studen": "студенистая слизь",
    "VB_AB_Mat_EMP": "ионная пыль", "VB_AB_Mat_Voronka": "сжатый камень", "VB_AB_Mat_Tramplin": "гравитационная галька",
    "VB_AB_Mat_Groza": "грозовой кристалл", "VB_AB_Mat_Flash": "световая пыль",
}
# Одежда: Id -> [(класс, сколько)]
CLOTHES_EXTRA = {
    5100: [("EFT_Cloth_ripstop", 2), ("EFT_Cloth_aramid", 1)],            # куртка GHOST
    5101: [("EFT_Cloth_ripstop", 2), ("EFT_Cloth_aramid", 1)],            # штаны GHOST
    5102: [("EFT_Cloth_aramid", 1), ("EFT_Cloth_cordura", 1)],            # шлем «Вулкан»
    5103: [("EFT_Cloth_fleece", 1), ("EFT_Cloth_cordura", 1)],            # маска GHOST
    5104: [("EFT_Cloth_paracord", 1), ("EFT_Cloth_cordura", 1)],          # ботинки GHOST
    5105: [("EFT_Cloth_fleece", 1), ("EFT_Cloth_cordura", 1)],            # перчатки GHOST
    5106: [("EFT_Cloth_cordura", 1), ("EFT_Cloth_paracord", 1)],          # подсумок GHOST
    5120: [("EFT_Cloth_cordura", 3), ("EFT_Cloth_paracord", 2), ("EFT_Cloth_aramid", 1)],           # рюкзак 6Ш118
    5121: [("EFT_Cloth_cordura", 1), ("EFT_Cloth_paracord", 1)],          # ременная система
    5122: [("EFT_Cloth_paracord", 2), ("EFT_Cloth_cordura", 1), ("EFT_Cloth_aramid", 1)],           # оружейный ремень
    5130: [("EFT_Cloth_fleece", 1)], 5131: [("EFT_Cloth_fleece", 1)], 5132: [("EFT_Cloth_fleece", 2)],
    5133: [("EFT_Cloth_cordura", 1), ("EFT_Cloth_paracord", 1)], 5134: [("EFT_Cloth_ripstop", 1)],
    5135: [("EFT_Cloth_paracord", 1)],
    5136: [("EFT_Cloth_ripstop", 1)], 5137: [("EFT_Cloth_fleece", 2)], 5138: [("EFT_Cloth_fleece", 1)],
    5139: [("EFT_Cloth_ripstop", 1)], 5140: [("EFT_Cloth_fleece", 2)],
    5150: [("EFT_Cloth_cordura", 1)], 5151: [("EFT_Cloth_fleece", 1)], 5152: [("EFT_Filtr", 1)],
    5153: [("EFT_Cloth_cordura", 1), ("EFT_Cloth_paracord", 1)], 5154: [("EFT_Cloth_ripstop", 1)],
    5155: [("EFT_Cloth_ripstop", 1)], 5156: [("EFT_Cloth_paracord", 1)], 5157: [("EFT_Cloth_fleece", 1)],
}
# Снаряжение: Id -> [(класс, сколько)]. Модули OOTS — по стихии (материалы аномалий) + электроника.
GEAR_EXTRA = {
    5201: [("EFT_MetalParts", 1), ("EFT_OrgGlass", 1)],                                           # контейнер артефактов
    5220: [(TROPHY, 2), ("EFT_Modem", 1), ("EFT_Capacitors", 2), ("VB_AB_Mat_Flash", 2)],         # отпугиватель
}
# Оружие (ствол) по цене всего набора в золоте: (от, ингредиенты)
WEAPON_EXTRA_TIERS = [
    (25, [("EFT_MetalParts", 2), ("EFT_Klean", 1), (TROPHY, 1)]),
    (15, [("EFT_MetalParts", 2), ("EFT_Klean", 1), ("EFT_Poxearm", 1)]),
    (5, [("EFT_MetalParts", 1), ("EFT_Klean", 1)]),
    (0, [("EFT_Bolts", 1), ("EFT_model_nuts", 1)]),
]
WEAPON_EXTRA_OVERRIDE = {  # Gauss — электромагнитная винтовка
    "SP_Gauss": [("EFT_Magnet", 2), ("EFT_Capacitors", 2), ("VB_AB_Mat_Groza", 2), (TROPHY, 1)],
}
# Обвес от 1 з: прицелы — объектив, глушители — металлические детали
ADDON_EXTRA = {"optic": [("EFT_Lens", 1)], "suppressor": [("EFT_MetalParts", 1)]}
# Магазины (2026-11-19, как на 18, но без руды — мода добычи на 16 нет): металл из бартера EFT. (от патронов, ингредиенты)
MAG_EXTRA_TIERS = [
    (101, [("EFT_MetalParts", 2)]),                          # пулемётный короб 200–250
    (50, [("EFT_MetalParts", 1), ("EFT_Bolts", 1)]),         # ёмкий 50–100 (порог как у цены, MAG_BIG_ROUNDS)
    (0, [("EFT_Bolts", 1)]),                                 # обычный, до 50
]
# Для базы: обои печатают на бумаге, ковры ткут из тряпок; шпатлёвка и сундук — только монеты
BASE_EXTRA = {gid: [("Paper", 2)] for gid in (5501, 5502, 5503, 5504, 5505, 5506)}
BASE_EXTRA[5507] = [("Rag", 3)]


def mag_extra(rounds):
    for floor, extra in MAG_EXTRA_TIERS:
        if rounds >= floor:
            return extra
    return []
MAX_INGREDIENTS = 8


def weapon_extra(cls, kit_gold):
    if cls in WEAPON_EXTRA_OVERRIDE:
        return WEAPON_EXTRA_OVERRIDE[cls]
    for floor, extra in WEAPON_EXTRA_TIERS:
        if kit_gold >= floor:
            return extra
    return []


def addon_extra(cls, gold):
    if gold < 1:
        return []
    if "Suppressor" in cls:
        return ADDON_EXTRA["suppressor"]
    if cls.startswith("AD_"):
        return ADDON_EXTRA["optic"]
    return []


def to_copper(gold):
    """Цена в золоте -> в медных (1 з = 100 м при курсе 10:1)."""
    return int(round(gold * RATE * RATE))


def nice(gold):
    """Цена до медной монеты (при 10:1 грубое округление больше не нужно)."""
    return to_copper(gold) / float(RATE * RATE)


def from_old_gold(g):
    """Старая цена магазина квестов (золото) -> новая цена в золоте."""
    return nice(g / float(OLD_DIV))
