# -*- coding: utf-8 -*-
"""Палитра и стражи «Древнего корня» (сервер 18, Ливония) — для генераторов текстур в этой папке.

Мод отдельный (modding/mod_vb_AncientRoot), исходники свои: VB_AncientRoot (клиент), VB_AncientRoot_Server,
VB_AncientRootSky, VB_AncientRoot_GC. Мод выделен из Изнанки 2026-11-15 и переименован целиком: классы VB_AR_*, файлы data vb_ar_*,
префиксы PBO VB_AncientRoot / VB_AncientRoot_Server / VB_AncientRoot_GC / VB_ARSky (молнии).

Генераторы (gen_ar_*.py) пишут прямо в исходники: ../VB_AncientRoot/data (или в папку из VB_AR_OUT),
молнии — в ../VB_AncientRootSky/data. Названия и описания предметов — прямо в config.cpp клиента.
"""
import os

HERE = os.path.dirname(os.path.abspath(__file__))
DEFAULT_MOD_DIR = os.path.normpath(os.path.join(HERE, "..", "VB_AncientRoot"))
SKY_DIR = os.path.normpath(os.path.join(HERE, "..", "VB_AncientRootSky"))


def theme_name():
    return "ancient_root"


def theme():
    return THEME


def mod_dir():
    """Корень клиентского мода, куда генераторы пишут data/ (по умолчанию исходники VB_AncientRoot)."""
    return os.environ.get("VB_AR_OUT") or DEFAULT_MOD_DIR


# ---------------------------------------------------------------- палитра
# BASE_PALETTE — палитра Изнанки на момент выделения (ключи по месту в генераторах), AR_PALETTE — поверх неё.

BASE_PALETTE = {
    # gen_ar_veins.make_textures: кора и жилы гнезда
    "crust_dark1": (0.045, 0.022, 0.027), "crust_dark2": (0.15, 0.06, 0.062),
    "crust_halo": (0.32, 0.04, 0.07), "crust_glow": (0.75, 0.10, 0.16),
    "crust_e_glow": (1.0, 0.22, 0.34), "crust_e_hot": (0.15, 0.3, 0.3), "crust_e_halo": (0.25, 0.02, 0.05),
    "vein_co": (0.42, 0.04, 0.08), "vein_e": (1.0, 0.25, 0.36), "vein_e_knot": (0.25, 0.3, 0.3),
    "day1": (0.20, 0.17, 0.14), "day2": (0.36, 0.31, 0.26), "day_halo": (0.13, 0.10, 0.09),
    "day_glow": (0.07, 0.05, 0.05), "vein_day": (0.24, 0.17, 0.15),
    # мох на коре (None — нет): цвет ночью, цвет днём, доля покрытия 0..1
    "moss_night": None, "moss_day": None, "moss_amount": 0.0,
    # gen_ar_veins.make_prop_textures: материалы изделий
    "p_wood": (0.36, 0.22, 0.12), "p_cloth": (0.34, 0.30, 0.24), "p_plastic": (0.30, 0.14, 0.075),
    "p_metal": (0.55, 0.55, 0.56), "p_bone": (0.84, 0.79, 0.66), "p_glass": (0.78, 0.86, 0.86),
    "p_liquid": (0.55, 0.03, 0.03), "p_steel": (0.36, 0.17, 0.09), "p_blade_vein": (0.95, 0.12, 0.10),
    "p_burlap": (0.50, 0.33, 0.17), "p_spore_glow": (1.0, 0.22, 0.16), "p_bark": (0.34, 0.19, 0.09),
    "p_bark_moss": None,
    # зомби (генератор Изнанки gen_rift_zombies.make_zombie; у «Корня» зомби нет)
    "z_cloth_mul": (0.95, 0.55, 0.55), "z_skin_mul": (0.92, 0.86, 0.90), "z_blot": (0.30, 0.07, 0.08),
    "z_halo": (0.22, 0.03, 0.05), "z_vein": (0.62, 0.06, 0.10),
    "z_e_vein": (1.0, 0.16, 0.24), "z_e_halo": (0.4, 0.02, 0.05),
    "z_ball_mul": (1.0, 0.25, 0.22), "z_ball_add": (0.18, 0.0, 0.0),
    "z_iris": (1.0, 0.18, 0.08), "z_pupil": (0.08, 0.0, 0.0), "z_e_iris": (1.0, 0.12, 0.06), "z_e_ball": (0.25, 0.0, 0.0),
    "z_moss": None,
    # gen_ar_retex: изделия на ванильных текстурах (основа, жила); плащ отдельно
    "r_base": (0.36, 0.17, 0.09), "r_vein": (0.95, 0.12, 0.10), "r_cloak_base": (0.36, 0.17, 0.09),
    "r_cloak_moss": None,
    # gen_ar_lightning: множитель цвета молнии (ночью любой ненулевой канал выгорает в полный)
    "lightning_tint": (1.0, 0.0, 0.0),
}

AR_PALETTE = dict(BASE_PALETTE)
AR_PALETTE.update({
    # тёмная кора, янтарные трещины, золотой свет
    "crust_dark1": (0.035, 0.028, 0.018), "crust_dark2": (0.13, 0.095, 0.055),
    # стенд 2026-11-15: при G 0.58–0.70 трещины ночью выгорали в оранжевое (лава) — сдвинуто в жёлтое золото
    "crust_halo": (0.36, 0.25, 0.05), "crust_glow": (0.95, 0.74, 0.22),
    "crust_e_glow": (1.0, 0.82, 0.30), "crust_e_hot": (0.25, 0.25, 0.12), "crust_e_halo": (0.28, 0.20, 0.03),
    "vein_co": (0.55, 0.40, 0.08), "vein_e": (1.0, 0.84, 0.32), "vein_e_knot": (0.3, 0.3, 0.15),
    # днём роща «спит»: кора серо-бурая, мох зелёный, трещины тёмные
    # стенд 2026-11-15: при 45% и ярком мхе корни днём походили на змей — покрытие 30%, цвет приглушён
    "day1": (0.17, 0.15, 0.11), "day2": (0.33, 0.29, 0.22), "day_halo": (0.12, 0.10, 0.06),
    "day_glow": (0.07, 0.06, 0.03), "vein_day": (0.26, 0.21, 0.12),
    "moss_night": (0.05, 0.07, 0.025), "moss_day": (0.17, 0.24, 0.09), "moss_amount": 0.30,
    # изделия: живица янтарная, свет золотой, пыльца золотая с зеленцой, пряжка деревянная
    "p_liquid": (0.78, 0.48, 0.06), "p_blade_vein": (1.0, 0.72, 0.15),
    "p_spore_glow": (1.0, 0.86, 0.30), "p_bone": (0.52, 0.36, 0.20),
    "p_bark": (0.30, 0.21, 0.11), "p_bark_moss": (0.20, 0.28, 0.08),
    # Стражи корня: одежда в тёмную оливу, кожа как кора, пятна мха, золотые прожилки и глаза
    "z_cloth_mul": (0.62, 0.62, 0.45), "z_skin_mul": (0.66, 0.52, 0.38), "z_blot": (0.16, 0.24, 0.07),
    "z_halo": (0.30, 0.18, 0.03), "z_vein": (0.85, 0.60, 0.12),
    "z_e_vein": (1.0, 0.72, 0.15), "z_e_halo": (0.4, 0.25, 0.02),
    "z_ball_mul": (1.0, 0.85, 0.40), "z_ball_add": (0.15, 0.10, 0.0),
    "z_iris": (1.0, 0.75, 0.10), "z_pupil": (0.06, 0.04, 0.0), "z_e_iris": (1.0, 0.70, 0.08), "z_e_ball": (0.20, 0.12, 0.0),
    "z_moss": (0.14, 0.22, 0.06),
    # изделия на ванильных текстурах: бурая кора и золотые жилы; Плащ лешего — во мху
    "r_base": (0.32, 0.22, 0.11), "r_vein": (1.0, 0.72, 0.15), "r_cloak_base": (0.17, 0.21, 0.09),
    "r_cloak_moss": (0.24, 0.32, 0.10),
    "lightning_tint": (1.0, 0.7, 0.0),
    # gen_ar_guards: стражи рощи (волк, Заноза, Энт) — кора, мох, золотые прожилки
    "g_body": (0.44, 0.34, 0.22), "g_keep": 0.30,
    "g_moss": (0.18, 0.26, 0.08), "g_moss_amount": 0.35,
    "g_vein": (0.95, 0.72, 0.20), "g_halo": (0.22, 0.15, 0.04),
    "g_e_vein": (1.0, 0.72, 0.15), "g_e_own": (1.0, 0.70, 0.12),
    # яркость свечения: множитель emmisive[] в материале (как у корки, 2.2) и доля прожилок в маске.
    # Стенд 11-15: при emmisive 1 и доле 0.6 прожилки волка и Занозы ночью не видны
    "g_emissive": 2.2, "g_e_gain": 1.0,
})


# Стражи рощи (2б.1, решение пользователя: звери; Заноза и Энт — наследники классов MSFZ).
# tex — исходная текстура тела (P: или исходники MSFZ), rvmat — исходный материал тела; генератор gen_ar_guards.py
# пишет data/guards/<file>_co.paa, _e_co.paa и <file>.rvmat (исходный + наша маска свечения в Stage8).
MSFZ_SRC = os.path.normpath(os.path.join(HERE, "..", "..", "mod_MSFZ_Mifical_Pack", "MSFZ_Mifical_Pack"))
AR_GUARDS = [
    {"cls": "VB_RootGuard_Wolf", "base": "Animal_CanisLupus_Grey", "file": "vb_guard_wolf",
     "name": "Лесовой волк",
     "desc": "Волк, обросший корой и мхом. Под шерстью тлеют золотые прожилки: роща сама зовёт его на защиту.",
     "tex": "P:\\DZ\\animals\\canis_lupus\\data\\wolf_grey_co.paa",
     "rvmat": "P:\\DZ\\animals\\canis_lupus\\data\\wolf_grey.rvmat",
     # вторая скрытая секция CamoHair — карточки шерсти поверх тела (fur_alpha, AlphaTest): без перекраски
     # они закрывают мох и прожилки, волк выходит просто бурым (стенд 11-15). Красим и их
     "fur": {"tex": "P:\\DZ\\animals\\canis_lupus\\data\\fur_alpha.paa",
             "rvmat": "P:\\DZ\\animals\\canis_lupus\\data\\fur_alpha.rvmat",
             "vein_keep": (0.3, 1.1)},
     "vein_keep": (0.7, 1.5),
     # у существ материал ставит только healthLevels (стенд 11-15), у ванильного волка списки пустые
     "damage_cfg": ("P:\\DZ\\animals\\canis_lupus\\config.cpp", "Animal_CanisLupus_Grey")},
    {"cls": "VB_RootGuard_Zanoza", "base": "MSFZ_Mutant_Zanoza1", "file": "vb_guard_zanoza",
     "name": "Терновая заноза",
     "desc": "Заноза, проросшая терновником. Шипы на её спине светятся золотом и рвут кожу до крови.",
     "tex": os.path.join(MSFZ_SRC, "Zanoza", "data", "msfz_zanoza_telo.paa"),
     "rvmat": os.path.join(MSFZ_SRC, "Zanoza", "data", "msfz_zanoza.rvmat"),
     # Заноза — тонкие ветки, сеть прожилок на них не читается: ещё слабое ровное свечение всего тела
     "e_base": 0.12,
     # у MSFZ в DamageSystem healthLevels свои материалы — движок ставит их поверх hiddenSelectionsMaterials
     # (стенд 11-15: наш материал не доходил). DamageSystem класса из этого конфига скопирован в config.cpp
     # с нашим материалом на здоровье 1.0 и 0.7
     "damage_cfg": (os.path.join(MSFZ_SRC, "Zanoza", "config.cpp"), "MSFZ_Mutant_Zanoza1"),
     "keep_tex": []},
    {"cls": "VB_RootGuard_Ent", "base": "MSFZ_Mutant_Ent", "file": "vb_guard_ent",
     "name": "Хранитель рощи",
     "desc": "Древний Энт, выросший из корней рощи. Сердцевина в его груди горит золотом. Выходит только к древним рощам.",
     "tex": os.path.join(MSFZ_SRC, "Ent", "data", "MSFZ_Ent_Base.paa"),
     "rvmat": os.path.join(MSFZ_SRC, "Ent", "data", "MSFZ_Ent_Base.rvmat"),
     "damage_cfg": (os.path.join(MSFZ_SRC, "Ent", "config.cpp"), "MSFZ_Mutant_Ent"),
     "keep_tex": []},
]

THEME = {
    "palette": AR_PALETTE,
    "guards": AR_GUARDS,
}
