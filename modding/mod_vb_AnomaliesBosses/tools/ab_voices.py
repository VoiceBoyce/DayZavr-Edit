# -*- coding: utf-8 -*-
"""Голоса боссов VB_AnomaliesBosses: звуки мутантов-доноров SFP и MSFZ (ТЗ 2.3).

Что делает:
  - копирует выбранные .ogg доноров в VB_AnomaliesBosses\\s\\v\\<код модели>\\ (код — как в ab_models.MODELS);
  - пишет cfg\\voices_shaders.hpp (внутрь class CfgSoundShaders) и cfg\\voices_sets.hpp (внутрь class CfgSoundSets).
Имена: VB_AB_Voice_<Модель>_<Событие>_SoundShader / _SoundSet, <Модель> — как в именах классов боссов
(VB_AB_<Модель>_<Палитра>, tools\\ab_skins.py), <Событие> — EVENTS ниже. Скрипт VB_AB_Voice.c собирает имя набора так же.
Если у модели нет набора для события, VB_AB_Voice молчит (без предупреждений в логе).

События (кто их вызывает — scripts\\4_World\\VB_AB\\VB_AB_Voice.c):
  Idle    спокойный (MINDSTATE_CALM), повтор раз в 9–16 с после конца звука
  Alert   встревожен (DISTURBED / ALERTED), повтор 5–9 с
  Chase   погоня и бой (CHASE / FIGHT), повтор 3,5–6 с
  Attack  удар (анимационное событие SoundVoice Attack id 5, прыжок id 10)
  Scream  крик (анимация «зов» CallToArmsShort id 20 при входе в погоню; способности — VB_AB_Voice.ServerSay)
  Pain    попадание (сервер EEHitBy -> RPC, не чаще раза в 2 с)
  Death   смерть (клиент OnDamageDestroyed)

Громкость и дальность. Набор — как у ванильного голоса зомби (Zmb_VoiceFX_Base_SoundSet), но кривая
infectedShoutingAttenuationCurve (у ванили — «крик» CallToArmsShort): она нормирована на range шейдера,
громкость ~0,58 на 0,22·range, ~0,3 на 0,325·range, ~0,04 на 0,45·range. При RANGE ниже:
Idle слышно ~35–50 м, Alert/Attack/Pain ~50–70 м, Chase ~55–77 м, Death ~65–90 м, Scream ~80–110 м
(ванильный зомби: range 60 -> 20–27 м). volume шейдера — от донора (MSFZ-файлы тихие, донор ставил 1,8–4,5),
но не больше VOLUME_MAX.

Запуск:  python ab_voices.py          копирует звуки и пишет оба .hpp (файл переписывается, только если изменился)
         python ab_voices.py list     таблица: модель, событие, файлы, длительность, размер
"""
import hashlib
import os
import shutil
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, ".."))
MOD = os.path.join(ROOT, "VB_AnomaliesBosses")
CFG = os.path.join(MOD, "cfg")
SRC = os.path.join(ROOT, "исходники модов")
BS = chr(92)
PREFIX = "VB_AB"

_SFP = os.path.join(SRC, "sfp", "@Server5_SFP", "Addons")
# корень исходного PBO -> папка распакованных исходников
ROOTS = {
    "msfz": os.path.join(SRC, "msfz", "@MSFZ_Mifical_Pack", "Addons", "MSFZ_Mifical_Pack"),
    "sfp": os.path.join(_SFP, "SFP"),
    "sfp2": os.path.join(_SFP, "SFP2"),
    "sfp3": os.path.join(_SFP, "SFP3"),
}

EVENTS = ["Idle", "Alert", "Chase", "Attack", "Scream", "Pain", "Death"]
RANGE = {"Idle": 110, "Alert": 150, "Chase": 170, "Attack": 150, "Scream": 240, "Pain": 150, "Death": 200}
VOLUME_MAX = 3.0

# папки звуков доноров (корень:путь), чтобы таблица ниже была короче
ENT = "msfz:Ent\\sound\\"
RIC = "msfz:Dark_ricar\\sound\\"
MOZ = "sfp3:1_AI\\MozgoT\\sound\\"
DIK = "sfp3:1_AI\\Dikoy\\sounds\\man01\\"          # «дикий» MAN01: атака и боль Мозготряса (SFP_MozgoTras_Base)
ZOM = "sfp2:1_AI\\zombirovan\\sounds\\"             # зомбированный: «зов» Мозготряса и смерть его базы
PIY = "sfp2:1_AI\\NoLor_Piyavka\\sound\\humanimal_v2_"  # «пиявка»: весь голос Теневика и Шум
WLF = "sfp:1_AI\\MutaWolf\\sounds\\"
KRO = "sfp2:1_AI\\Krovosisa\\sounds\\"              # кровососка: попадание Мутаволка (SFP_KrovososHit_Soundset)


def _n(prefix, names):
    return [prefix + n for n in names]


# код модели -> имя в классах, высота тона (frequencyFactor), события: (volume, [файлы]).
# Файл: "<корень>:<путь без .ogg>"; «путь=>имя» — своё имя файла в s\v\<код>\ (иначе имя донора в нижнем регистре).
# same — модель без своих файлов: те же наборы и файлы, что у указанной, но своя высота тона.
VOICES = {
    # Энт (MSFZ_Mutant_Ent). Донор: SoundVoice LightHit/HeavyHit = EntHit, Attack = EntAttack, CallToArmsShort = EntRun;
    # CfgAIBehaviours: спокойный — EntWalk (psi_scan), вход в тревогу — EntPrepare, во время — EntControl, EntPsyHit.
    # Ent_die_* у донора лежат без конфига — взяты на смерть. psy_aura_l/r = psi_scan_4 (тот же файл).
    "en": dict(cls="Ent", freq=1.0, events={
        "Idle": (3.0, _n(ENT, ["Ent_psi_scan_1", "Ent_psi_scan_2", "Ent_psi_scan_3", "Ent_psi_scan_4"])),
        "Alert": (2.05, _n(ENT, ["Ent_control_1", "Ent_control_2"])),
        "Chase": (3.0, _n(ENT, ["Ent_psy_hit_l", "Ent_psy_hit_r"])),
        "Attack": (1.78, _n(ENT, ["Ent_attack_0", "Ent_attack_1"])),
        "Scream": (2.05, _n(ENT, ["Ent_prepare_1", "Ent_run_0"])),
        "Pain": (1.78, _n(ENT, ["Ent_hit_0", "Ent_hit_1", "Ent_hit_2"])),
        "Death": (1.78, _n(ENT, ["Ent_die_0", "Ent_die_1"])),
    }),
    # Ричар (MSFZ_Ricar). Донор: Attack = Ricar_attack_0 (attack_1 и first_hit_0 — тот же файл), «Hit»/«Run» и три
    # «Scan» — один и тот же файл 8,9 с (ricar_scan_1), поэтому боль — RicarHit2 (hit_l = hit_r), смерть — die_0/1.
    "ri": dict(cls="Ricar", freq=1.0, events={
        "Idle": (3.0, _n(RIC, ["Ricar_scan_1", "Ricar_scan_2"])),
        "Alert": (3.0, _n(RIC, ["Ricar_aura_l", "Ricar_aura_r"])),
        "Chase": (2.05, _n(RIC, ["Ricar_control_1", "Ricar_control_2"])),
        "Attack": (1.78, _n(RIC, ["Ricar_attack_0"])),
        "Scream": (2.05, _n(RIC, ["Ricar_prepare_1", "Ricar_final_hit_0"])),
        "Pain": (3.0, _n(RIC, ["Ricar_hit_l"])),
        "Death": (1.78, _n(RIC, ["Ricar_die_0", "Ricar_die_1"])),
    }),
    # Мозготряс (SFP_MozgoTras_Base: SFP_Zombirovan_Base). Донор: Attack = Dikoy MAN01 attack, LightHit/HeavyHit/Agre =
    # Dikoy MAN01 pain, CallToArmsShort = Zombirovan idle; скрипт MozgoDav.c: спокойный — петля PsiAura (psi_aura_*),
    # агр — PsiAuraGuaa (controller_presence_l/r). Своей смерти нет (bodyfall игрока) — взята смерть базы
    # SFP_Zombirovan_Base (SFP_Zombirovan_Dead_Table: zombie_die_*). Из длинных наборов — часть файлов.
    "mo": dict(cls="MozgoTras", freq=1.0, events={
        "Idle": (1.5, _n(MOZ, ["psi_aura_01", "psi_aura_06"])),
        "Alert": (1.0, _n(ZOM, ["zombie_idle_2", "zombie_idle_3", "zombie_idle_5", "zombie_idle_10", "zombie_idle_12"])),
        "Chase": (1.5, _n(MOZ, ["controller_presence_l", "controller_presence_r"])),
        "Attack": (1.0, _n(DIK, ["humanimal_attack_1", "humanimal_attack_2", "humanimal_attack_3",
                                 "humanimal_attack_5", "humanimal_attack_7", "humanimal_attack_9"])),
        "Scream": (1.2, _n(ZOM, ["zombie_idle_7", "zombie_idle_13"])),
        "Pain": (1.0, _n(DIK, ["humanimal_pain_1", "humanimal_pain_2", "humanimal_pain_4",
                               "humanimal_pain_6", "humanimal_pain_9"])),
        "Death": (1.0, _n(ZOM, ["zombie_die_2", "zombie_die_5", "zombie_die_9"])),
    }),
    # Мозготряс красный — тот же класс-донор SFP_MozgoTras_Base, те же звуки; ниже тоном (берсерк).
    "mr": dict(cls="MozgoTrasRed", freq=0.88, same="mo"),
    # Теневик (SFP_Tenevnik: SFP_Zombie_Base). Донор: весь голос — наборы «пиявки» (SFP_Piyavka*: idle, Breath,
    # Alarm = CallToArmsShort/Jump, Attack, Hit), смерть — SFP_Piyavka_Dead_Table (death_*). danger_idle_* лежат
    # у донора без конфига — взяты на погоню. Тот же банк у Шум, поэтому файлы поделены и тон ниже (0,85).
    "te": dict(cls="Tenevnik", freq=0.85, events={
        "Idle": (1.0, _n(PIY, ["idle_01", "idle_02"])),
        "Alert": (1.2, _n(PIY, ["breath_single_01", "breath_single_03", "breath_single_05", "breath_single_07",
                                "breath_single_09", "breath_single_11", "breath_single_13"])),
        "Chase": (1.2, _n(PIY, ["danger_idle_1", "danger_idle_2"])),
        "Attack": (1.4, _n(PIY, ["attack_01", "attack_03", "attack_05", "attack_07", "attack_09"])),
        "Scream": (1.5, _n(PIY, ["alert_reaction_01", "alert_reaction_03", "alert_reaction_05"])),
        "Pain": (1.3, _n(PIY, ["pain_01", "pain_03", "pain_05", "pain_07", "pain_09"])),
        "Death": (1.0, _n(PIY, ["death_1", "death_3", "death_5", "death_7"])),
    }),
    # Шум (SFP_Shum_female: SFP_Zombie_Base). Донор: те же наборы «пиявки», что у Теневика (свои у Шум только
    # звуки телекинеза в shum_female\sound — это способности, не голос). Вторая половина банка, тон выше (1,12).
    "sh": dict(cls="Shum", freq=1.12, events={
        "Idle": (1.2, _n(PIY, ["breath_single_02", "breath_single_04", "breath_single_06", "breath_single_08",
                               "breath_single_10", "breath_single_12", "breath_single_14"])),
        "Alert": (1.2, _n(PIY, ["danger_idle_3"])),
        "Chase": (1.5, _n(PIY, ["alert_reaction_02", "alert_reaction_04"])),
        "Attack": (1.4, _n(PIY, ["attack_02", "attack_04", "attack_06", "attack_08", "attack_10"])),
        "Scream": (1.5, _n(PIY, ["alert_reaction_01", "alert_reaction_02", "alert_reaction_03",
                                 "alert_reaction_04", "alert_reaction_05"])),
        "Pain": (1.3, _n(PIY, ["pain_02", "pain_04", "pain_06", "pain_08"])),
        "Death": (1.0, _n(PIY, ["death_2", "death_4", "death_6"])),
    }),
    # Мутаволк (SFP_MutaWolf_Base). Донор: idle = mw_idle + breach_05/06, Breath = breach_01–04, Jump/CallToArmsShort =
    # AGRE (mw_agre*), Attack = mw_attack*, LightHit/HeavyHit = SFP_KrovososHit (кровососка hit_0–2),
    # смерть — SFP_MW_Dead_Table (mw_dead*), вой mw_summoner_wolf* (summonerWolf, range 150) — на крик.
    "wo": dict(cls="MutaWolf", freq=1.0, events={
        "Idle": (1.0, _n(WLF, ["idle\\mw_idle_01", "idle\\mw_idle_02", "breach\\breach_05", "breach\\breach_06"])),
        "Alert": (1.0, _n(WLF, ["breach\\breach_01", "breach\\breach_02", "breach\\breach_03", "breach\\breach_04"])),
        "Chase": (1.4, _n(WLF, ["mw_agre", "mw_agre_02", "mw_agre_03"])),
        "Attack": (1.0, _n(WLF, ["attack\\mw_attack_01", "attack\\mw_attack_02", "attack\\mw_attack_03",
                                 "attack\\mw_attack_04", "attack\\mw_attack_05", "attack\\mw_attack_06"])),
        "Scream": (1.4, _n(WLF, ["mw_summoner_wolf", "mw_summoner_wolf_02", "mw_summoner_wolf_03"])),
        "Pain": (1.0, [KRO + "hit_0=>krovosos_hit_0", KRO + "hit_1=>krovosos_hit_1", KRO + "hit_2=>krovosos_hit_2"]),
        "Death": (1.0, _n(WLF, ["mw_dead01", "mw_dead02"])),
    }),
}

HEADER = "// СГЕНЕРИРОВАНО tools/ab_voices.py — руками не править\n"


def src_file(spec):
    """«корень:путь[=>имя]» -> (файл донора, имя файла у нас без расширения)."""
    path, _, dst = spec.partition("=>")
    root, _, rel = path.partition(":")
    base = ROOTS.get(root)
    if base is None:
        raise SystemExit("неизвестный корень донора: " + spec)
    full = os.path.join(base, *rel.split(BS)) + ".ogg"
    if not dst:
        dst = rel.split(BS)[-1]
    return full, dst.lower()


def ogg_info(path):
    """Длительность (с) по granule последней страницы Ogg и частоте из заголовка Vorbis."""
    data = open(path, "rb").read()
    rate = 0
    i = data.find(b"\x01vorbis")
    if i >= 0:
        rate = struct.unpack("<I", data[i + 12:i + 16])[0]
    j = data.rfind(b"OggS")
    gran = struct.unpack("<q", data[j + 6:j + 14])[0] if j >= 0 else 0
    return (gran / rate if rate else 0.0), len(data)


def resolve():
    """[(код, класс, freq, папка, {событие: (volume, [(файл донора, имя)])})] — с разворотом same."""
    out = []
    for code, v in VOICES.items():
        if "same" in v:
            base = VOICES[v["same"]]
            folder, events = v["same"], base["events"]
        else:
            folder, events = code, v["events"]
        ev = {}
        for name in EVENTS:
            if name in events:
                vol, files = events[name]
                ev[name] = (vol, [src_file(f) for f in files])
        out.append((code, v["cls"], v["freq"], folder, ev))
    return out


def fmt(x):
    s = ("%.3f" % x).rstrip("0").rstrip(".")
    return s or "0"


def build_hpp(models):
    sh = [HEADER, "// Голоса боссов: шейдеры (внутри class CfgSoundShaders в config.cpp)\n"]
    ss = [HEADER, "// Голоса боссов: наборы (внутри class CfgSoundSets в config.cpp). Звук — только у клиента, VB_AB_Voice.c\n",
          "\t// база: как ванильный Zmb_VoiceFX_Base_SoundSet, но кривая громкости «крик» (слышно в ~2,5 раза дальше зомби)\n",
          "\tclass VB_AB_Voice_Base_SoundSet\n\t{\n"
          "\t\tsound3DProcessingType = \"infected3DProcessingType\";\n"
          "\t\tdistanceFilter = \"infectedDistanceFreqAttenuationFilter\";\n"
          "\t\tvolumeCurve = \"infectedShoutingAttenuationCurve\";\n"
          "\t\tspatial = 1;\n\t\tdoppler = 0;\n\t\tloop = 0;\n"
          "\t\tocclusionFactor = 1;\n\t\tobstructionFactor = 1;\n"
          "\t\tvolumeFactor = 1;\n\t\tfrequencyFactor = 1;\n\t};\n"]
    for code, cls, freq, folder, ev in models:
        for name in EVENTS:
            if name not in ev:
                continue
            vol, files = ev[name]
            base = "VB_AB_Voice_%s_%s" % (cls, name)
            samples = ", ".join('{"%s", 1}' % BS.join([PREFIX, "s", "v", folder, dst]) for _, dst in files)
            sh.append("\tclass %s_SoundShader\n\t{\n\t\tsamples[] = {%s};\n\t\tvolume = %s;\n\t\trange = %d;\n\t};\n"
                      % (base, samples, fmt(min(vol, VOLUME_MAX)), RANGE[name]))
            ss.append("\tclass %s_SoundSet: VB_AB_Voice_Base_SoundSet\n\t{\n\t\tsoundShaders[] = {\"%s_SoundShader\"};\n"
                      "\t\tfrequencyFactor = %s;\n\t};\n" % (base, base, fmt(freq)))
    return "".join(sh), "".join(ss)


def write_if_changed(path, text):
    old = None
    if os.path.isfile(path):
        with open(path, encoding="utf-8") as f:
            old = f.read()
    if old == text:
        return False
    with open(path, "w", encoding="utf-8", newline="\n") as f:
        f.write(text)
    return True


def md5(path):
    return hashlib.md5(open(path, "rb").read()).hexdigest()


def cmd_build():
    models = resolve()
    copied = total = 0
    used = {}
    problems = []
    for code, cls, freq, folder, ev in models:
        for name, (vol, files) in ev.items():
            for src, dst in files:
                if not os.path.isfile(src):
                    problems.append("нет файла донора: " + src)
                    continue
                out = os.path.join(MOD, "s", "v", folder, dst + ".ogg")
                prev = used.get(out)
                if prev and prev != src and md5(prev) != md5(src):
                    problems.append("два разных файла под одним именем: %s и %s" % (prev, src))
                used[out] = src
    if problems:
        for p in problems:
            print("!", p)
        sys.exit(1)
    for out, src in sorted(used.items()):
        os.makedirs(os.path.dirname(out), exist_ok=True)
        if not os.path.isfile(out) or md5(out) != md5(src):
            shutil.copyfile(src, out)
            copied += 1
        total += os.path.getsize(out)
    # лишние .ogg в s\v (убрали из таблицы) — удалить, чтобы не попали в PBO
    vroot = os.path.join(MOD, "s", "v")
    removed = 0
    for d, _, names in os.walk(vroot):
        for n in names:
            p = os.path.join(d, n)
            if n.lower().endswith(".ogg") and p not in used:
                os.remove(p)
                removed += 1
    shaders, sets = build_hpp(models)
    a = write_if_changed(os.path.join(CFG, "voices_shaders.hpp"), shaders)
    b = write_if_changed(os.path.join(CFG, "voices_sets.hpp"), sets)
    nsets = sum(len(ev) for *_, ev in models)
    print("звуков: %d (скопировано %d, удалено лишних %d), %.2f МБ; наборов: %d; voices_shaders.hpp %s, voices_sets.hpp %s"
          % (len(used), copied, removed, total / 1048576.0, nsets,
             "обновлён" if a else "без изменений", "обновлён" if b else "без изменений"))


def cmd_list():
    total = 0
    seen = set()
    for code, cls, freq, folder, ev in resolve():
        print("== %s %s (папка s\\v\\%s, тон %s)" % (code, cls, folder, fmt(freq)))
        for name in EVENTS:
            if name not in ev:
                print("   %-7s —" % name)
                continue
            vol, files = ev[name]
            parts = []
            for src, dst in files:
                dur, size = ogg_info(src)
                key = (folder, dst)
                if key not in seen:
                    seen.add(key)
                    total += size
                parts.append("%s %.1fс" % (dst, dur))
            print("   %-7s vol %-4s range %3d: %s" % (name, fmt(min(vol, VOLUME_MAX)), RANGE[name], ", ".join(parts)))
    print("всего файлов %d, %.2f МБ" % (len(seen), total / 1048576.0))


def main():
    a = sys.argv[1:]
    if a and a[0] == "list":
        cmd_list()
        return
    if a:
        raise SystemExit("неизвестная команда: " + a[0])
    cmd_build()


if __name__ == "__main__":
    main()
