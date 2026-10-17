"""Static 1.29 -> 1.30 porting scan for a DayZ mod (Enforce Script + config.cpp + layouts).

Usage:
    python -I scan_mod_130.py <mod_source_dir> [--deps <dir> ...] [--out report.md] [--json report.json]

<mod_source_dir> is the unpacked mod (the folder with config.cpp / scripts/). --deps adds the source of
mods this one depends on (CF, Dabs, Expansion ...) so overrides of *their* methods are not reported.

What it finds (each finding says how sure it is):
  ERROR  override of a vanilla method whose parameter list changed in 1.30 (does not compile)
  ERROR  override / call of a vanilla method or use of a class/field/enum value that 1.30 removed
  WARN   override of a hook vanilla 1.30 never calls any more (compiles, silently does nothing)
  WARN   call of a method that is [Obsolete] in 1.30 (compiles with SCRIPT (W))
  WARN   config: CfgMods script paths with backslashes; class overriding a vanilla config class that 1.30 changed
  WARN   copies of vanilla layouts that 1.30 changed; images removed from vanilla imagesets
  INFO   runtime traps reported for 1.30 (FindFile + $profile:, $storage:, GetMissionFolderPath, version defines)
It is a static pre-check. The final word is a compile on 1.30 (DayZDiag / server script log).
Exit code: 1 if any ERROR, else 0.
"""
import argparse
import json
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
DATA = os.path.join(os.path.dirname(HERE), "data")
sys.path.insert(0, HERE)
from es_parse import Parser, clean  # noqa: E402

# ---------------------------------------------------------------- data
API = json.load(open(os.path.join(DATA, "api130-index.json"), encoding="utf-8"))
OLD = json.load(open(os.path.join(DATA, "api129-index.json"), encoding="utf-8"))
DELTA = json.load(open(os.path.join(DATA, "delta.json"), encoding="utf-8"))
DEAD = json.load(open(os.path.join(DATA, "deadhooks.json"), encoding="utf-8"))
CFG = json.load(open(os.path.join(DATA, "cfgdelta.json"), encoding="utf-8"))
OBSOLETE_CLASSES = json.load(open(os.path.join(DATA, "obsolete_classes.json"), encoding="utf-8"))
_added_ammo = {k.split("/")[1].lower(): k.split("/")[1] for k in CFG["added"] if k.count("/") == 1 and k.split("/")[0].lower() == "cfgammo"}
REMOVED_AMMO = {k.split("/")[1]: _added_ammo.get(k.split("/")[1].lower() + "_ammo")
                for k in CFG["removed"] if k.count("/") == 1 and k.split("/")[0].lower() == "cfgammo"}
GUI = [ln.split(None, 1) for ln in open(os.path.join(DATA, "vanilla-gui-files-changed-1.30.txt"), encoding="utf-8")
       if ln[:1] != "#" and ln.strip()]

# Hooks vanilla 1.30 no longer calls (verified by call-site count 1.29 vs 1.30, plus moved Construction ones)
EXTRA_DEAD = {
    ("ConstructionBase", "BuildPartServer"): "vanilla calls BuildPartServerEx(Man, string, int) (P:\\scripts\\4_world\\classes\\constructionbase.c:179)",
    ("ConstructionBase", "DismantlePartServer"): "vanilla calls DismantlePartServerEx(Man, string, int) (constructionbase.c:190)",
    ("Construction", "BuildPartServer"): "vanilla calls BuildPartServerEx(Man, string, int) (P:\\scripts\\4_world\\classes\\constructionbase.c:179)",
    ("Construction", "DismantlePartServer"): "vanilla calls DismantlePartServerEx(Man, string, int) (constructionbase.c:190)",
}
REPLACEMENT_HINTS = {
    "OnCEUpdate": "override OnCEIterate(float currentTime, float elapsedTime) instead (P:\\scripts\\3_game\\entities\\entityai.c)",
    "CreateFrontLight": "register lights: m_LightsComponent.RegisterLight(\"Front\", new VehicleLightData(new <YourProfile> : VehicleLightProfileFront)) in the car ctor",
    "CreateRearLight": "register lights: m_LightsComponent.RegisterLight(\"Rear\", new VehicleLightData(new <YourProfile> : VehicleLightProfileBrakeAndReverse))",
    "IsVitalCarBattery": "override bool NeedElectricitySourceDevice() + EntityAI GetElectricitySourceDevice() (Transport, P:\\scripts\\3_game\\vehicles\\transport.c:821) instead",
    "IsVitalTruckBattery": "override bool NeedElectricitySourceDevice() + EntityAI GetElectricitySourceDevice() (Transport, P:\\scripts\\3_game\\vehicles\\transport.c:821) instead",
    "ToggleHeadlights": "use LightToggle() (Transport)",
    "CanDisplayAttachmentSlot": "override the int overload CanDisplayAttachmentSlot(int slot_id); vanilla never calls the string one",
    "ProcessVariables": "1.30 signature is ProcessVariables(float elapsedTime)",
    "GetNoiseReduction": "AI noise dampening is native (AIParams config) on 1.30",
    "GetNoiseReductionByWeather": "AI noise dampening is native (AIParams config) on 1.30",
    "AddExplosionEffectForSurface": "per-surface explosion effects live in AmmoTypes (AmmoTypesAPI.GetExplosiveEffectData)",
    "GetExplosionParticleID": "AmmoTypesAPI.GetExplosiveEffectData(ammo, surface).m_ParticleID",
    "UnlockServer": "CombinationLock.UnlockOnServer(...)",
    "Is3rdPersonDisabled": "World.GetThirdPersonViewMode() (ThirdPersonMode.DISABLED / ENABLED / VEHICLES_ONLY)",
    "BuildPartServer": "BuildPartServerEx(Man player, string part_name, int action_id)",
    "DismantlePartServer": "DismantlePartServerEx(Man player, string part_name, int action_id)",
    "IsColliding": "IsCollidingEx(CollisionCheckData)",
    "ForceSurrenderState": "PlayerBase.SetSurrenderState(bool)",
}

SKIP_DIRS = {"node_modules", "__pycache__", "corpus", "backup", "backups"}


def walk(root):
    """os.walk that skips hidden folders (.git, .dayz-mcp corpus copies of vanilla) and obvious non-source dirs."""
    for dp, dn, fn in os.walk(root):
        dn[:] = [d for d in dn if not d.startswith(".") and d.lower() not in SKIP_DIRS]
        yield dp, dn, fn


TYPE_STRIP = re.compile(r"\b(notnull|const|ref|autoptr|owned|local|reference)\b")


def norm_type(t, tdefs):
    t = TYPE_STRIP.sub("", t).strip()
    mod = ""
    m = re.match(r"^(out|inout)\s+", t)
    if m:
        mod, t = m.group(1) + " ", t[m.end():]
    t = t.replace(" ", "")
    seen = set()
    while t in tdefs and t not in seen:
        seen.add(t)
        t = TYPE_STRIP.sub("", tdefs[t]).replace(" ", "")
    return mod + t.lower()


def ancestors(idx, cname, extra=None):
    """Yield (class, record) up the chain; `extra` = mod classes {name: {"b": base, "m": {...}}} searched first."""
    seen = set()
    c = cname
    while c and c not in seen:
        seen.add(c)
        rec = (extra or {}).get(c)
        if rec:
            yield c, rec, "mod"
            nxt = rec.get("b")
            if c in idx["classes"]:
                yield c, idx["classes"][c], "vanilla"
                nxt = nxt or idx["classes"][c]["b"]
            c = nxt
            continue
        rec = idx["classes"].get(c)
        if not rec:
            td = idx["typedefs"].get(c)
            if td:
                c = re.sub(r"<.*", "", td).strip()
                continue
            return
        yield c, rec, "vanilla"
        c = re.sub(r"<.*", "", rec["b"]).strip() if rec["b"] else None


# names that exist anywhere in an index
def all_method_names(idx):
    s = set()
    for c in idx["classes"].values():
        s.update(c["m"].keys())
    s.update(idx["globals"])
    return s


def all_field_names(idx):
    s = set()
    for c in idx["classes"].values():
        s.update(c["F"])
    return s


NEW_METHODS, OLD_METHODS = all_method_names(API), all_method_names(OLD)
NEW_FIELDS, OLD_FIELDS = all_field_names(API), all_field_names(OLD)
REMOVED_METHOD_NAMES = {n for n in OLD_METHODS - NEW_METHODS if len(n) > 3 and not n.startswith("~")}
REMOVED_FIELD_NAMES = {n for n in OLD_FIELDS - NEW_FIELDS if n.startswith("m_") or (len(n) > 5 and any(ch.isupper() for ch in n))}
REMOVED_CLASSES = {c["class"] for c in DELTA["classes_removed"]}

# methods whose every 1.30 declaration is [Obsolete]
OBSOLETE_ONLY = {}
for cname, c in API["classes"].items():
    for n, recs in c["m"].items():
        if recs and all(r["o"] for r in recs):
            OBSOLETE_ONLY.setdefault(n, []).append((cname, recs[0]["o"]))
for n, recs in list(OBSOLETE_ONLY.items()):
    # keep only names that are obsolete everywhere they exist
    for cname, c in API["classes"].items():
        if n in c["m"] and not all(r["o"] for r in c["m"][n]):
            OBSOLETE_ONLY.pop(n, None)
            break

DEAD_SET = {(r["class"], r["method"]) for r in DEAD if r["calls_129"] > 0 and r["calls_130"] == 0}
# obsolete overloads vanilla no longer calls although another overload of the same name is still called
DEAD_OVERLOADS = {("EntityAI", "CanDisplayAttachmentSlot"), ("ItemBase", "GetDryingIncrement"), ("ItemBase", "GetSoakingIncrement")}
# (class, method, param count) of 1.29 overloads that still exist but vanilla 1.30 no longer calls (call-arity check,
# P:\scripts 1.30.164078): vanilla calls the overload 1.30 added instead.
BYPASSED_OVERLOADS = {
    ("IngameHudVisibility", "UpdateElement", 1): "vanilla calls UpdateElement(EHudElement, bool ignoreWidgetVisiblityState) (5_mission\\gui\\ingamehudvisibility.c:100,128)",
    ("IngameHudVisibility", "UpdateVisibility", 2): "vanilla calls UpdateVisibility(EHudElement, bool, bool ignoreWidgetVisiblityState) (ingamehudvisibility.c:169,177)",
    ("Building", "CanDoorBeOpened", 2): "the open-door action calls CanDoorBeOpened(DoorManipulationParams) (actionopendoors.c:52,72; building.c:141) — "
                                         "door-blocking logic (raid protection, safe zones, locks) in the (int, bool) override no longer runs. "
                                         "Override the params overload; doorIndex is params.m_DoorIndex",
    ("ExplosivesBase", "AddExplosionEffectForSurface", 2): "per-surface explosion effects come from AmmoTypesAPI (3_game\\global\\ammotypes.c:78); the old ExplosivesBase map is never allocated",
}
DEAD_SET |= set(EXTRA_DEAD)


# ---------------------------------------------------------------- findings
class Report:
    def __init__(self):
        self.items = []

    def add(self, sev, kind, where, msg, sure="high"):
        self.items.append({"sev": sev, "kind": kind, "where": where, "msg": msg, "confidence": sure})


def scan_scripts(mod_dirs, dep_dirs, rep):
    mod = Parser("")
    files = []
    for root in mod_dirs + dep_dirs:
        for dp, dn, fn in walk(root):
            for f in fn:
                if f.lower().endswith(".c"):
                    files.append((root, os.path.join(dp, f), root in mod_dirs))
    mod_only = Parser("")
    for root, p, is_mod in files:
        mod.root = root
        mod.parse_file(p)
        if is_mod:
            mod_only.root = root
            mod_only.parse_file(p)
    tdefs = dict(API["typedefs"])
    tdefs.update({k: v["target"] for k, v in mod.typedefs.items()})
    # compact mod classes (all, incl. deps) for chain resolution
    mclasses = {}
    for cname, c in mod.classes.items():
        base = next((d["base"] for d in c["decls"] if d["base"]), None)
        mclasses[cname] = {"b": re.sub(r"<.*", "", base).strip() if base else None,
                           "m": {n: [{"p": [p["type"] for p in r["params"]]} for r in recs] for n, recs in c["methods"].items()}}
    mod_method_names = set()
    for c in mod.classes.values():
        mod_method_names.update(c["methods"].keys())
    mod_method_names.update(mod.globals.keys())
    mod_field_names = set()
    for c in mod.classes.values():
        mod_field_names.update(c["fields"].keys())

    full_path = {os.path.relpath(p, root).replace("\\", "/"): p for root, p, is_mod in files if is_mod}

    def method_body(rel_file, line):
        p = full_path.get(rel_file)
        if not p:
            return ""
        t = clean(open(p, encoding="utf-8", errors="replace").read())
        off = sum(len(x) + 1 for x in t.split("\n")[:line - 1])
        i = t.find("{", off)
        if i < 0:
            return ""
        depth, j = 0, i
        while j < len(t):
            if t[j] == "{":
                depth += 1
            elif t[j] == "}":
                depth -= 1
                if depth == 0:
                    return t[i:j + 1]
            j += 1
        return t[i:]

    # 0) custom cars: 1.30 CarScript.GetBattery() = GetElectricitySourceDevice(), which is null in Transport
    #    (P:\scripts\3_game\vehicles\transport.c:826, carscript.c GetBattery); 1.29 found CarBattery/TruckBattery itself.
    for cname, c in mod_only.classes.items():
        if all(d["modded"] for d in c["decls"]):
            continue
        chain = list(ancestors(API, cname, mclasses))
        names = [a for a, _, _ in chain]
        if "CarScript" not in names:
            continue
        below = []
        for anc, rec, src in chain:
            if anc == "CarScript":
                break
            below.append(rec)
        if not any(("GetElectricitySourceDevice" in rec["m"] or "NeedElectricitySourceDevice" in rec["m"]) for rec in below):
            d0 = next(d for d in c["decls"] if not d["modded"])
            rep.add("WARN", "car-no-battery-source", f"{d0['file']}:{d0['line']}",
                    f"`{cname}` extends CarScript without a vanilla car in between and overrides neither GetElectricitySourceDevice() nor "
                    "NeedElectricitySourceDevice(). On 1.30 CarScript.GetBattery() returns GetElectricitySourceDevice(), which is null in the "
                    "base (transport.c:826) while NeedElectricitySourceDevice() is true (:821) → state NO_BATTERY, the engine never starts. "
                    "1.29 found the CarBattery/TruckBattery slot by itself. Add: override EntityAI GetElectricitySourceDevice() "
                    "{ return GetInventory().FindAttachment(CarBattery.SLOT_ID); } (vanilla: inheritedcars\\civiliansedan.c:59-68) "
                    "or override NeedElectricitySourceDevice() { return false; }.")

    # 1) overrides
    for cname, c in mod_only.classes.items():
        decl_files = {d["file"] for d in c["decls"]}
        for mname, recs in c["methods"].items():
            for r in recs:
                if "override" not in r["mods"]:
                    continue
                where = f"{r['file']}:{r['line']}"
                if mname == "OnRPC" and any(anc == "ItemBase" for anc, _, _ in ancestors(API, cname, mclasses)) \
                        and "super.OnRPC" not in method_body(r["file"], r["line"]):
                    rep.add("WARN", "onrpc-no-super", where,
                            f"`{cname}.OnRPC` does not call super.OnRPC. On 1.30 item sounds arrive as a batched RPC handled in "
                            "ItemBase.OnRPC (case ERPCs.RPC_ITEM_SOUND_PLAY, P:\\scripts\\4_world\\entities\\itembase.c:2796) — "
                            "without super this item's sounds are lost on clients.", "medium")
                mine = [norm_type(p["type"], tdefs) for p in r["params"]]
                # search: vanilla chain of this class (for modded: the vanilla class itself), skipping this mod's own decl
                found_v, found_m, match = [], False, False
                for anc, rec, src in ancestors(API, cname, mclasses):
                    if src == "mod":
                        if anc != cname and mname in rec["m"]:
                            found_m = True
                        continue
                    if mname in rec["m"]:
                        found_v.append((anc, rec["m"][mname]))
                if found_v:
                    for anc, ovs in found_v:
                        for ov in ovs:
                            theirs = [norm_type(t, tdefs) for t in ov["p"]]
                            if theirs == mine:
                                match = True
                                byp = BYPASSED_OVERLOADS.get((anc, mname, len(ov["p"])))
                                if byp or (((anc, mname) in DEAD_SET or (cname, mname) in DEAD_SET) and (ov.get("o") or (anc, mname) in EXTRA_DEAD)) \
                                        or ((anc, mname) in DEAD_OVERLOADS and ov.get("o")):
                                    hint = byp or EXTRA_DEAD.get((anc, mname)) or REPLACEMENT_HINTS.get(mname, ov.get("o") or "")
                                    rep.add("WARN", "dead-hook", where,
                                            f"`{cname}.{mname}` overrides `{anc}.{mname}`, which vanilla 1.30 never calls any more "
                                            f"(compiles, does nothing). {hint}")
                                elif ov.get("o"):
                                    rep.add("WARN", "override-obsolete", where,
                                            f"`{cname}.{mname}` overrides `{anc}.{mname}` — {ov['o']}. Check vanilla still calls it.")
                                break
                        if match:
                            break
                    if not match and not found_m:
                        sigs = "; ".join(ov["s"] for anc, ovs in found_v[:1] for ov in ovs)
                        old = [ov for anc, rec, src in ancestors(OLD, cname) if src == "vanilla" and mname in rec["m"] for ov in rec["m"][mname]]
                        was_ok = any([norm_type(t, tdefs) for t in ov["p"]] == mine for ov in old)
                        hint = REPLACEMENT_HINTS.get(mname, "")
                        rep.add("ERROR" if was_ok else "WARN", "override-signature", where,
                                f"`{cname}.{mname}({', '.join(p['type'] for p in r['params'])})` does not match 1.30 `{found_v[0][0]}`: `{sigs}`"
                                + (" — this matched 1.29, so it is a 1.30 change." if was_ok else " — it did not match 1.29 either (dependency overload?).")
                                + (f" {hint}" if hint else ""), "high" if was_ok else "medium")
                elif not found_m:
                    old_hit = [(anc, rec) for anc, rec, src in ancestors(OLD, cname) if src == "vanilla" and mname in rec["m"]]
                    if old_hit:
                        hint = REPLACEMENT_HINTS.get(mname, "")
                        rep.add("ERROR", "override-removed", where,
                                f"`{cname}.{mname}` overrides `{old_hit[0][0]}.{mname}`, which 1.30 removed. {hint}")
                    elif cname in API["classes"] or any(src == "vanilla" for _, _, src in ancestors(API, cname, mclasses)):
                        rep.add("INFO", "override-unknown", where,
                                f"`{cname}.{mname}` overrides something that is neither 1.29 nor 1.30 vanilla — a dependency mod? "
                                f"Pass its source with --deps to check it.", "low")

    # 2) calls / identifiers in bodies
    call_rx = re.compile(r"(?<![\w.~])(?:\.\s*)?([A-Za-z_]\w*)\s*\(")
    dotted_rx = re.compile(r"\b([A-Z]\w*)\.([A-Za-z_]\w*)\b")
    word_rx = re.compile(r"\b([A-Za-z_]\w*)\b")
    for root, p, is_mod in files:
        if not is_mod:
            continue
        rel = os.path.relpath(p, root).replace("\\", "/")
        raw = open(p, encoding="utf-8", errors="replace").read()
        text = clean(raw)
        for i, ln in enumerate(text.split("\n"), 1):
            where = f"{rel}:{i}"
            if ln.lstrip().startswith("#"):
                if re.search(r"DAYZ_1_(2[0-9])\b", ln):
                    rep.add("INFO", "version-define", where, f"version gate `{ln.strip()}` — on 1.30 only DAYZ_1_30 is defined; "
                            "keep old code under #ifdef DAYZ_1_29 and 1.30 code in #else (never #ifdef DAYZ_1_30).")
                if re.search(r"\b(BUILD_EXPERIMENTAL|DIAG_DEVELOPER)\b", ln):
                    rep.add("INFO", "build-define", where, f"`{ln.strip()}` is a build flavour (Diag/Experimental), not a version test.")
                continue
            for m in call_rx.finditer(ln):
                n = m.group(1)
                if n in mod_method_names:
                    continue
                if n in REMOVED_METHOD_NAMES:
                    rep.add("ERROR", "call-removed", where, f"`{n}(...)` no longer exists in 1.30 vanilla. {REPLACEMENT_HINTS.get(n, '')}", "medium")
                elif n in OBSOLETE_ONLY:
                    cls, msg = OBSOLETE_ONLY[n][0]
                    rep.add("WARN", "call-obsolete", where, f"`{cls}.{n}` is {msg}. {REPLACEMENT_HINTS.get(n, '')}", "medium")
            for m in dotted_rx.finditer(ln):
                e, v = m.group(1), m.group(2)
                if e in API["enums"] or e in OLD["enums"]:
                    vals = set()
                    en = e
                    seen = set()
                    while en and en in API["enums"] and en not in seen:
                        seen.add(en)
                        vals.update(API["enums"][en]["v"])
                        en = API["enums"][en]["b"]
                    if e not in API["enums"]:
                        rep.add("ERROR", "enum-removed", where, f"enum `{e}` was removed in 1.30.")
                    elif v not in vals and not v[0].islower():
                        oldv = set(OLD["enums"].get(e, {}).get("v", []))
                        if v in oldv:
                            rep.add("ERROR", "enum-value-removed", where, f"`{e}.{v}` does not exist in 1.30 (renamed/removed). 1.30 values include: "
                                    + ", ".join(sorted(vals)[:12]))
            for m in word_rx.finditer(ln):
                w = m.group(1)
                if w in REMOVED_CLASSES and w not in mod.classes:
                    rep.add("ERROR", "class-removed", where, f"class `{w}` was removed in 1.30.")
                elif w in OBSOLETE_CLASSES and w not in mod.classes:
                    rep.add("WARN", "class-obsolete", where, f"class `{w}` is {OBSOLETE_CLASSES[w]['msg']} "
                            f"(P:\\scripts\\{OBSOLETE_CLASSES[w]['at'].replace('/', chr(92))}).")
                elif w in REMOVED_FIELD_NAMES and w not in mod_field_names:
                    rep.add("ERROR", "field-removed", where, f"field `{w}` no longer exists in 1.30 vanilla.", "medium")
            m = re.search(r"\b(m_CarHornShortSoundName|m_CarHornLongSoundName)\s*=(?!=)", ln)
            if m:
                mode = "SHORT" if "Short" in m.group(1) else "LONG"
                rep.add("WARN", "dead-field", where,
                        f"`{m.group(1)}` is a DEPRECATED field 1.30 never reads (P:\\scripts\\4_world\\entities\\vehicles\\carscript.c:2765) — "
                        f"the horn is silent. Register it: m_HornComponent.RegisterSound(VehicleHornMode.{mode}, \"<SoundSet>\") in the car ctor "
                        "(vanilla: inheritedcars\\civiliansedan.c:24-25).")
            m = re.search(r"\b(?:const\s+)?int\s+(\w*MENU\w*)\s*=\s*(47|48)\b", ln)
            if m:
                rep.add("WARN", "menu-id-collision", where,
                        f"`{m.group(1)} = {m.group(2)}` collides with vanilla 1.30 MENU_DIGITAL_CODELOCK = 47 / "
                        "MENU_UNDERGROUND_TRIGGER_EDITOR = 48 (P:\\scripts\\3_game\\constants.c:222-223). Pick an id well above vanilla.")
            for m in re.finditer(r'"([A-Za-z_]\w*)"', ln):
                if m.group(1) in REMOVED_AMMO:
                    rep.add("WARN", "cfgammo-renamed", where, f"\"{m.group(1)}\" is no longer a CfgAmmo class in 1.30"
                            + (f" — renamed to \"{REMOVED_AMMO[m.group(1)]}\"." if REMOVED_AMMO[m.group(1)] else "."))
            if "FindFile" in ln and re.search(r"\bFindFile\s*\(", ln):
                rep.add("WARN", "runtime-findfile", where, "[reported: bzed, DZEXP-134] on 1.30 FindFile ignores $profile:/$mission:/$storage:/$saves: and "
                        "lists the server working dir instead. Use CF.FindFileEx (CF 1.5.9+) or a resolved, forward-slash path. Re-test on your server.")
            if "$storage:" in ln:
                rep.add("INFO", "runtime-storage", where, "[reported: bzed] $storage: is not usable before the first MissionServer.OnUpdate after OnMissionStart.")
            if "GetMissionFolderPath" in ln:
                rep.add("WARN", "runtime-missionfolder", where, "[reported: bzed] GetMissionFolderPath() returns \"\" on 1.30 servers (engine passes a "
                        "forward-slash mission path); derive the folder from GetMissionPath().")
            if re.search(r"\bERPCs\.\w+\s*[+\-]\s*\d|\bcase\s+\d+\s*:", ln) and "RPC" in raw:
                rep.add("INFO", "rpc-raw-int", where, "raw RPC integer / arithmetic on ERPCs: 1.30 inserted values into ERPCs (every id after "
                        "RPC_PLAYER_SYMPTOM_OFF shifted). Use enum names and your own id range.", "low")
    return mod_only


CFG_TOK = re.compile(r'"(?:[^"\n]|"")*"|//[^\n]*|/\*.*?\*/|[A-Za-z_0-9.\-]+(?:\[\])?|\+=|[{}=;:,\[\]\n]|\S', re.S)


def cfg_classes(txt):
    """Yield (path_tuple, base, line, props{key: (value_text, line)}) for every class body in a config.cpp."""
    toks, line = [], 1
    for m in CFG_TOK.finditer(txt):
        t = m.group(0)
        if t == "\n":
            line += 1
            continue
        if t.startswith("/*"):
            line += t.count("\n")
            continue
        if t.startswith("//") or t.startswith("#"):
            continue
        toks.append((t, line))
    stack, out, i = [], [], 0
    while i < len(toks):
        t, ln = toks[i]
        if t == "class" and i + 1 < len(toks):
            name = toks[i + 1][0]
            j, base = i + 2, None
            if j < len(toks) and toks[j][0] == ":":
                base, j = toks[j + 1][0], j + 2
            if j < len(toks) and toks[j][0] == "{":
                rec = (tuple(s[0] for s in stack) + (name,), base, ln, {})
                out.append(rec)
                stack.append((name, rec))
                i = j + 1
                continue
            i = j + 1
            continue
        if t == "}":
            if stack:
                stack.pop()
            i += 1
            continue
        if i + 1 < len(toks) and toks[i + 1][0] in ("=", "+=") and stack:
            j, val = i + 2, []
            depth = 0
            while j < len(toks):
                if toks[j][0] == "{":
                    depth += 1
                elif toks[j][0] == "}":
                    depth -= 1
                    if depth < 0:
                        break
                elif toks[j][0] == ";" and depth == 0:
                    break
                val.append(toks[j][0])
                j += 1
            stack[-1][1][3][t] = (" ".join(val), ln)
            i = j + 1
            continue
        i += 1
    return out


def scan_configs(mod_dirs, rep):
    vchanged = {k.lower(): (k, v) for k, v in CFG["changed"].items()}
    removed = {k.split("/")[-1].lower(): k for k in CFG["removed"] if k.count("/") == 1}

    def elems(val):
        return [x.strip() for x in str(val).strip().strip("{}").split(",") if x.strip()]

    # vanilla classes whose hiddenSelections[] list changed / appeared in 1.30
    hs_changed = {}
    for k, v in CFG["changed"].items():
        if k.count("/") != 1:
            continue
        for kind in ("props_changed", "props_added"):
            for pk, val in v.get(kind, {}).items():
                if pk.lower() == "hiddenselections[]":
                    old = elems(val[0]) if kind == "props_changed" else []
                    new = elems(val[1]) if kind == "props_changed" else elems(val)
                    hs_changed[k.split("/")[1].lower()] = (k, old, new)
    for root in mod_dirs:
        for dp, dn, fn in walk(root):
            for f in fn:
                if f.lower() != "config.cpp":
                    continue
                p = os.path.join(dp, f)
                rel = os.path.relpath(p, root).replace("\\", "/")
                txt = open(p, encoding="utf-8", errors="replace").read()
                for path, base, ln, props in cfg_classes(txt):
                    for key, (val, pl) in props.items():
                        for m in re.finditer(r'"([A-Za-z_]\w*)"', val):
                            if m.group(1) in REMOVED_AMMO:
                                new = REMOVED_AMMO[m.group(1)]
                                rep.add("WARN", "cfgammo-renamed", f"{rel}:{pl}", f"`{key}` references CfgAmmo \"{m.group(1)}\", which 1.30 "
                                        "removed" + (f" (renamed \"{new}\")." if new else "."))
                    if path and path[0].lower() == "cfgmods":
                        for key, (val, pl) in props.items():
                            if key.lower() in ("files[]", "value", "dir", "files[] +=") and "\\" in val:
                                rep.add("WARN", "cfgmods-backslash", f"{rel}:{pl}",
                                        f"CfgMods `{key}` uses backslashes ({val[:80]}). [reported: bzed] a 1.30 Linux server compiled none of a mod's "
                                        "scripts with backslash paths (no error). Forward slashes work everywhere: \"MyMod/scripts/4_World\".")
                    if base and len(path) == 2 and base.lower() in hs_changed:
                        vk, old, new = hs_changed[base.lower()]
                        mine = {k.lower(): v for k, v in props.items()}
                        for tk in ("hiddenselectionstextures[]", "hiddenselectionsmaterials[]"):
                            if tk in mine and "hiddenselections[]" not in mine:
                                n = len(elems(mine[tk][0]))
                                if n < len(new):
                                    added = [x for x in new if x not in old]
                                    rep.add("WARN", "cfg-hiddenselections-grew", f"{rel}:{mine[tk][1]}",
                                            f"`{path[-1]}` inherits `{base}`, whose hiddenSelections[] grew in 1.30 to {len(new)} entries "
                                            f"(new: {', '.join(added[:6]) or '-'}); this {tk} has {n}. The new selections keep the "
                                            "vanilla texture/material — extend the array (see config-delta).", "medium")
                    if base and len(path) == 2 and base.lower() in removed:
                        rep.add("WARN", "cfg-base-removed", f"{rel}:{ln}", f"`{path[-1]}` inherits `{base}`, a config class 1.30 removed ({removed[base.lower()]}).")
                    key = "/".join(path).lower()
                    if key in vchanged and props:
                        vk, v = vchanged[key]
                        mine = {k.lower() for k in props}
                        touched = []
                        for pk, (o, nv) in v.get("props_changed", {}).items():
                            if pk.lower() in mine:
                                if str(o).startswith("{") and str(nv).startswith("{"):
                                    oe = [x.strip() for x in str(o).strip("{}").split(",") if x.strip()]
                                    ne = [x.strip() for x in str(nv).strip("{}").split(",") if x.strip()]
                                    add = [x for x in ne if x not in oe]
                                    rem = [x for x in oe if x not in ne]
                                    touched.append(f"{pk}: 1.30 vanilla added {', '.join(add[:8]) or '-'}; removed {', '.join(rem[:8]) or '-'}")
                                else:
                                    touched.append(f"{pk}: 1.29 `{str(o)[:70]}` → 1.30 `{str(nv)[:70]}`")
                        for pk in v.get("props_added", {}):
                            if pk.lower() in mine:
                                touched.append(f"{pk} (new in 1.30 vanilla)")
                        if touched:
                            rep.add("WARN", "cfg-overrides-changed-vanilla", f"{rel}:{ln}",
                                    f"`{'/'.join(path)}` sets keys that vanilla 1.30 changed — your value replaces the new vanilla one: "
                                    + "; ".join(touched[:6]), "medium")


def scan_gui(mod_dirs, rep):
    changed = {os.path.basename(p.strip()).lower(): p.strip() for kind, p in GUI if kind == "CHANGED" and p.strip().endswith(".layout")}
    for root in mod_dirs:
        for dp, dn, fn in walk(root):
            for f in fn:
                low = f.lower()
                p = os.path.join(dp, f)
                rel = os.path.relpath(p, root).replace("\\", "/")
                if low.endswith(".layout") and low in changed:
                    rep.add("WARN", "layout-copy", rel, f"same name as vanilla `{changed[low]}`, which 1.30 changed — if this is a copy, re-diff it against P:\\.")
                if low.endswith((".layout", ".c", ".imageset", ".styles")):
                    txt = open(p, encoding="utf-8", errors="replace").read()
                    for img in ("externallink", "woodcrate"):
                        if re.search(r"image\s*:\s*" + img + r"\b", txt, re.I):
                            rep.add("WARN", "imageset-image-removed", rel, f"references image `{img}`, removed from the vanilla imageset in 1.30.")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("mod")
    ap.add_argument("--deps", nargs="*", default=[])
    ap.add_argument("--out")
    ap.add_argument("--json")
    a = ap.parse_args()
    rep = Report()
    scan_scripts([a.mod], a.deps, rep)
    scan_configs([a.mod], rep)
    scan_gui([a.mod], rep)
    # de-duplicate
    seen, items = set(), []
    for it in rep.items:
        key = (it["sev"], it["kind"], it["where"], it["msg"])
        if key not in seen:
            seen.add(key)
            items.append(it)
    order = {"ERROR": 0, "WARN": 1, "INFO": 2}
    items.sort(key=lambda x: (order[x["sev"]], x["kind"], x["where"]))
    counts = {s: sum(1 for x in items if x["sev"] == s) for s in order}
    lines = [f"# 1.30 porting scan: `{a.mod}`", "",
             f"ERROR {counts['ERROR']} · WARN {counts['WARN']} · INFO {counts['INFO']}  "
             "(static pre-check; confirm with a 1.30 compile — script log `(E)` lines are the truth)", ""]
    cur = None
    for it in items:
        if it["sev"] != cur:
            cur = it["sev"]
            lines += ["", f"## {cur}", "", "| Where | Kind | Finding | Confidence |", "|---|---|---|---|"]
        lines.append(f"| `{it['where']}` | {it['kind']} | {it['msg'].replace('|', '/')} | {it['confidence']} |")
    md = "\n".join(lines) + "\n"
    if a.out:
        open(a.out, "w", encoding="utf-8").write(md)
    if a.json:
        json.dump(items, open(a.json, "w", encoding="utf-8"), indent=1, ensure_ascii=False)
    sys.stdout.reconfigure(encoding="utf-8")
    print(md if not a.out else f"ERROR {counts['ERROR']} WARN {counts['WARN']} INFO {counts['INFO']} -> {a.out}")
    sys.exit(1 if counts["ERROR"] else 0)


if __name__ == "__main__":
    main()
