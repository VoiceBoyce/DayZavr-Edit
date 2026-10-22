"""Static porting scan for a DayZ mod (Enforce Script + config.cpp + layouts): 1.29 -> 1.30 and the 1.30 hotfix.

Usage:
    python -I scan_mod_130.py <mod_source_dir> [--deps <dir> ...] [--from {1.29,1.30.164078}] [--out report.md] [--json report.json]

<mod_source_dir> is the unpacked mod (the folder with config.cpp / scripts/). --deps adds the source of
mods this one depends on (CF, Dabs, Expansion ...) so overrides of *their* methods are not reported.
--from says which build the mod was written for:
  1.29 (default)   run every step: "1.29 -> 1.30" (against the newest 1.30 index) + "1.30.164078 -> 1.30.164200"
  1.30.164078      the mod is already ported to 1.30.164078: run only the hotfix step
Every finding carries its step ("step" in --json, a section per step in --out).

Step 1.29 -> 1.30 finds (each finding says how sure it is):
  ERROR  override of a vanilla method whose parameter list changed in 1.30 (does not compile)
  ERROR  override / call of a vanilla method or use of a class/field/enum value that 1.30 removed
  WARN   override of a hook vanilla 1.30 never calls any more (compiles, silently does nothing)
  WARN   call of a method that is [Obsolete] in 1.30 (compiles with SCRIPT (W))
  WARN   config: CfgMods script paths with backslashes; class overriding a vanilla config class that 1.30 changed
  WARN   copies of vanilla layouts that 1.30 changed; images removed from vanilla imagesets
  INFO   runtime traps reported for 1.30 (FindFile + $profile:, $storage:, GetMissionFolderPath, version defines)
Step 1.30.164078 -> 1.30.164200 (data/delta-1.30.164078-to-164200.json) finds:
  ERROR  vector.Cross(...) calls (removed; `a * b` is the cross product), base-building support API signature changes
         (GetSupportProvided/Requirement, GetPartTypeSupport*, TryHandleUtilityParts, GetPartTypeUtilityAutobuildPartsList,
         m_UtilityAutomaticBuild), Transport subclasses declaring HasCrew (now proto native), Workbench API used outside
         #ifdef WORKBENCH, removed classes/methods/fields, overrides/calls that no longer match
  WARN   mod methods that now shadow a method vanilla 164200 added to an ancestor (IsMoving without override ...),
         raw ints of enums whose values shifted (NVTypes), config utility_build[] (no longer read), NavmeshTest* names,
         launch files with -resolveFilePatchingUsingEnfusion
  INFO   uses of shifted enum values, overrides of vanilla methods whose body changed in 164200 (override without super)
  + config checks against data/cfgdelta-1.30.164078-to-164200.json when that file exists (cfg-overrides-changed-vanilla,
    cfg-hiddenselections-grew, cfg-base-removed, cfgammo-renamed, cfg-ref-removed).
Hotfix steps are data-driven (HOTFIX_STEPS below): each has an old index, a new index and a delta built by
build_api_index.py / build_api_delta.py; rules that name a specific API fire only when that step's delta contains it.
Adding the next patch: scripts/PATCH_BUMP.md.
It is a static pre-check. The final word is a compile on the target build (DayZDiag / server script log).
Exit code: 1 if any ERROR, else 0.
"""
import argparse
import bisect
import json
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
DATA = os.path.join(os.path.dirname(HERE), "data")
sys.path.insert(0, HERE)
from es_parse import Parser, clean, preprocess, split_top  # noqa: E402

STEP_129 = "1.29 -> 1.30"
SCRIPTS = "P:\\scripts\\"
# Hotfix steps after the first 1.30 build, oldest first. Files live in data/; "cfgdelta" is optional.
# "new_index" of the LAST step must be api130-index.json (P:\scripts). scripts/PATCH_BUMP.md says how to add a step.
HOTFIX_STEPS = [
    {"from": "1.30.164078", "to": "1.30.164200", "old_index": "api130_164078-index.json", "new_index": "api130-index.json",
     "delta": "delta-1.30.164078-to-164200.json", "cfgdelta": "cfgdelta-1.30.164078-to-164200.json"},
]
# navmesh.nm format version the engine accepts per build (uint32 at offset 4; vanilla P:\DZ\worlds\*\navmesh\navmesh.nm)
NAVMESH_VERSION = {"1.30.164200": 9}


def _load(name, optional=False):
    p = os.path.join(DATA, name)
    if optional and not os.path.isfile(p):
        return None
    with open(p, encoding="utf-8") as fh:
        return json.load(fh)


# ---------------------------------------------------------------- data
API = _load("api130-index.json")              # newest 1.30 build (P:\scripts, 1.30.164200)
OLD = _load("api129-index.json")              # 1.29
DELTA = _load("delta.json")                   # 1.29 -> 1.30.164078
DEAD = _load("deadhooks.json")
CFG = _load("cfgdelta.json")
OBSOLETE_CLASSES = _load("obsolete_classes.json")


def removed_ammo(cfg):
    if not cfg:
        return {}
    added = {k.split("/")[1].lower(): k.split("/")[1] for k in cfg["added"] if k.count("/") == 1 and k.split("/")[0].lower() == "cfgammo"}
    return {k.split("/")[1]: added.get(k.split("/")[1].lower() + "_ammo")
            for k in cfg["removed"] if k.count("/") == 1 and k.split("/")[0].lower() == "cfgammo"}


REMOVED_AMMO = removed_ammo(CFG)
GUI =[ln.split(None, 1) for ln in open(os.path.join(DATA, "vanilla-gui-files-changed-1.30.txt"), encoding="utf-8")
       if ln[:1] != "#" and ln.strip()]

# Hooks vanilla 1.30 no longer calls (verified by call-site count 1.29 vs 1.30, plus moved Construction ones)
EXTRA_DEAD = {
    ("ConstructionBase", "BuildPartServer"): "vanilla calls BuildPartServerEx(Man, string, int) (P:\\scripts\\4_world\\classes\\constructionbase.c:180)",
    ("ConstructionBase", "DismantlePartServer"): "vanilla calls DismantlePartServerEx(Man, string, int) (constructionbase.c:191)",
    ("Construction", "BuildPartServer"): "vanilla calls BuildPartServerEx(Man, string, int) (P:\\scripts\\4_world\\classes\\constructionbase.c:180)",
    ("Construction", "DismantlePartServer"): "vanilla calls DismantlePartServerEx(Man, string, int) (constructionbase.c:191)",
}
REPLACEMENT_HINTS = {
    "OnCEUpdate": "override OnCEIterate(float currentTime, float elapsedTime) instead (P:\\scripts\\3_game\\entities\\entityai.c)",
    "CreateFrontLight": "register lights: m_LightsComponent.RegisterLight(\"Front\", new VehicleLightData(new <YourProfile> : VehicleLightProfileFront)) in the car ctor",
    "CreateRearLight": "register lights: m_LightsComponent.RegisterLight(\"Rear\", new VehicleLightData(new <YourProfile> : VehicleLightProfileBrakeAndReverse))",
    "IsVitalCarBattery": "override bool NeedElectricitySourceDevice() + EntityAI GetElectricitySourceDevice() (Transport, P:\\scripts\\3_game\\vehicles\\transport.c:830) instead",
    "IsVitalTruckBattery": "override bool NeedElectricitySourceDevice() + EntityAI GetElectricitySourceDevice() (Transport, P:\\scripts\\3_game\\vehicles\\transport.c:830) instead",
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
# P:\scripts 1.30.164078; those files are unchanged in 1.30.164200): vanilla calls the overload 1.30 added instead.
BYPASSED_OVERLOADS = {
    ("IngameHudVisibility", "UpdateElement", 1): "vanilla calls UpdateElement(EHudElement, bool ignoreWidgetVisiblityState) (5_mission\\gui\\ingamehudvisibility.c:100,128)",
    ("IngameHudVisibility", "UpdateVisibility", 2): "vanilla calls UpdateVisibility(EHudElement, bool, bool ignoreWidgetVisiblityState) (ingamehudvisibility.c:169,177)",
    ("Building", "CanDoorBeOpened", 2): "the open-door action calls CanDoorBeOpened(DoorManipulationParams) (actionopendoors.c:52,72; building.c:141) — "
                                         "door-blocking logic (raid protection, safe zones, locks) in the (int, bool) override no longer runs. "
                                         "Override the params overload; doorIndex is params.m_DoorIndex",
    ("ExplosivesBase", "AddExplosionEffectForSurface", 2): "per-surface explosion effects come from AmmoTypesAPI (3_game\\global\\ammotypes.c:78); the old ExplosivesBase map is never allocated",
}
DEAD_SET |= set(EXTRA_DEAD)

# ---------------------------------------------------------------- hotfix steps (data-driven, see HOTFIX_STEPS)
# variable-name pattern that marks an int holding a value of that enum (for literal-comparison checks)
ENUM_VAR_HINT = {"NVTypes": re.compile(r"nv", re.I)}


def p_loc(rel):
    """'4_world/x.c:12' -> 'P:\\scripts\\4_world\\x.c:12'"""
    return SCRIPTS + rel.replace("/", "\\") if rel else ""


def _arity(idx, cname, name):
    out = []
    for r in idx["classes"].get(cname, {}).get("m", {}).get(name, []):
        out.append((len(r["p"]) - r["d"], len(r["p"])))
    return out


class HotfixStep:
    """Everything one hotfix step (old build -> new build) needs, derived from its two indices and its delta."""

    def __init__(self, spec):
        self.frm, self.to = spec["from"], spec["to"]
        self.label = f"{self.frm} -> {self.to}"
        self.OLD = _load(spec["old_index"])
        self.NEW = API if spec["new_index"] == "api130-index.json" else _load(spec["new_index"])
        self.D = _load(spec["delta"])
        self.CFG = _load(spec["cfgdelta"], optional=True) if spec.get("cfgdelta") else None
        self.cfg_name = spec.get("cfgdelta")
        self.removed_ammo = removed_ammo(self.CFG)
        # top-level config classes the step removed (and did not re-add elsewhere): name -> "CfgX/Name"
        self.removed_cfg = {}
        if self.CFG:
            readded = {k.split("/")[1].lower() for k in self.CFG["added"] if k.count("/") == 1}
            self.removed_cfg = {k.split("/")[1]: k for k in self.CFG["removed"]
                                if k.count("/") == 1 and k.split("/")[1].lower() not in readded and k.split("/")[0].lower() != "cfgammo"}
        new_m, new_f = (NEW_METHODS, NEW_FIELDS) if self.NEW is API else (all_method_names(self.NEW), all_field_names(self.NEW))
        self.removed_methods = {n for n in all_method_names(self.OLD) - new_m if len(n) > 3 and not n.startswith("~")}
        self.removed_fields = {n for n in all_field_names(self.OLD) - new_f
                               if n.startswith("m_") or (len(n) > 5 and any(ch.isupper() for ch in n))}
        D = self.D
        # name-based call/field checks only for members removed from classes that still exist; members of removed
        # classes (164200: WorldEditorAPI.CreateEntity, m_API ...) are covered by class-removed and would only add noise
        self.removed_methods &= {r["name"] for ch in D["class_changes"].values() for r in ch.get("methods_removed", [])}
        self.removed_fields &= {r["name"] for ch in D["class_changes"].values() for r in ch.get("fields_removed", [])}
        self.removed_classes = {r["class"]: r for r in D["classes_removed"]}
        added = {r["class"] for r in D["classes_added"]}
        self.renamed = {c: "Land_" + c for c in self.removed_classes if "Land_" + c in added}
        # classes that moved under a build-flavour guard (164200: the Workbench API went under #ifdef WORKBENCH)
        self.guarded = {r["class"]: r for r in D.get("guard_changed", []) if r["new"] and not r["old"] and not r["new"].startswith("!")}
        self.sig = {}            # name -> [(class, old sigs, new sigs, line)]
        self.new = {}            # (class, name) -> methods_added record whose name is new in the chain
        self.new_override = {}   # (class, name) -> methods_added record that overrides an older ancestor method
        self.body = {}           # (class, name, sig) -> line of the new body
        self.new_fields = {}     # (class, field) -> fields_added record
        for c, ch in D["class_changes"].items():
            for r in ch.get("sig_changed", []):
                self.sig.setdefault(r["name"], []).append((c, r["old"], r["new"], r["line"]))
            for r in ch.get("methods_added", []):
                (self.new_override if r.get("shadows") else self.new)[(c, r["name"])] = r
            for r in ch.get("body_changed", []):
                self.body[(c, r["name"], r["sig"])] = r["line"]
            for r in ch.get("fields_added", []):
                self.new_fields[(c, r["name"])] = r
        self.enum_shift = {e: {s["name"]: (s["old"], s["new"]) for s in r.get("shifted", r["shifted_sample"])}
                           for e, r in D["enum_changes"].items() if r["shifted_count"]}
        self.enum_removed = {e: set(r["removed"]) for e, r in D["enum_changes"].items() if r["removed"]}
        self.enum_line = {e: r.get("line") or r["file"] for e, r in D["enum_changes"].items()}
        self.arity = {}  # name -> [(class, old arities, new arities, new sigs, line)]
        for n, lst in self.sig.items():
            for c, _old, new, line in lst:
                self.arity.setdefault(n, []).append((c, _arity(self.OLD, c, n), _arity(self.NEW, c, n), new, line))
        self.hints = STEP_HINTS.get(self.to, {})

_BB_SUPPORT = ("The base-building support API changed in 1.30.164200 (Rebuilding / Takistan rebuildable houses): it returns bool "
               "(false = category not configured; 164078 returned the int and 0) and hands the strength back through an out int.")
HF_HINTS = {
    "Cross": "vector.Cross(vector) was removed in 1.30.164200 (it existed only in 164078, 1_core\\proto\\enconvert.c:239). The `*` operator "
             "of two vectors is the cross product: write `(a * b)` for `a.Cross(b)` (vanilla 164200: P:\\scripts\\4_world\\classes\\environment\\environment.c:696-697).",
    "GetSupportProvided": _BB_SUPPORT + " New: `bool GetSupportProvided(int category, out int providedStrength)` "
                          "(ConstructionPart: P:\\scripts\\4_world\\classes\\basebuilding\\constructionpart.c:371; "
                          "ConstructionPartStaticsSupportData: P:\\scripts\\4_world\\classes\\rebuilding\\constructionstatics.c:80).",
    "GetSupportRequirement": _BB_SUPPORT + " New: `bool GetSupportRequirement(int category, out int requiredStrength)` "
                             "(constructionpart.c:376; constructionstatics.c:89).",
    "GetPartTypeSupportProvided": _BB_SUPPORT + " New: `bool GetPartTypeSupportProvided(int category, out int providedStrength)` "
                                  "(P:\\scripts\\4_world\\classes\\basebuilding\\typeconstructiondata\\constructionparttyped.c:633).",
    "GetPartTypeSupportRequirement": _BB_SUPPORT + " New: `bool GetPartTypeSupportRequirement(int category, out int requiredStrength)` "
                                     "(constructionparttyped.c:638).",
    "TryHandleUtilityParts": "ConstructionPart.TryHandleUtilityParts(EntityAI parent) was removed in 1.30.164200; vanilla now calls "
                             "`void TryQueueUtilityPartsUpdate(ConstructionBase con)` (P:\\scripts\\4_world\\classes\\basebuilding\\constructionpart.c:491, "
                             "ConstructionPartRebuild override :885), which queues ConstructionBase.QueueUpdateUtilityParts() / UpdateUtilityParts() "
                             "(P:\\scripts\\4_world\\classes\\constructionbase.c:626,636).",
    "GetPartTypeUtilityAutobuildPartsList": "ConstructionPartTypeData.GetPartTypeUtilityAutobuildPartsList() and field m_UtilityAutomaticBuild were "
                                            "removed in 1.30.164200 (config key utility_build[] is no longer read). A part now only says whether it "
                                            "triggers the utility-part update: `bool TriggersUtilityPartsUpdate()` "
                                            "(P:\\scripts\\4_world\\classes\\basebuilding\\typeconstructiondata\\constructionparttyped.c:478; config "
                                            "`toggle_utility_part_update = 1;`, read in InitUtilityBuildData :267).",
}
HF_HINTS["m_UtilityAutomaticBuild"] = HF_HINTS["GetPartTypeUtilityAutobuildPartsList"]
STEP_HINTS = {"1.30.164200": HF_HINTS}   # hand-written fix hints per target build (keyed by HOTFIX_STEPS "to")

HSTEPS = [HotfixStep(s) for s in HOTFIX_STEPS]
assert HSTEPS[-1].NEW is API, "the last hotfix step must end at api130-index.json"
STEP_HF = HSTEPS[0].label          # first hotfix step (kept as a name for tests / callers)
STEPS_FROM = {"1.29": [STEP_129] + [s.label for s in HSTEPS]}
for _i, _s in enumerate(HSTEPS):
    STEPS_FROM[_s.frm] = [s.label for s in HSTEPS[_i:]]


# ---------------------------------------------------------------- findings
class Report:
    def __init__(self):
        self.items = []
        self.step = STEP_129

    def add(self, sev, kind, where, msg, sure="high", step=None):
        self.items.append({"sev": sev, "kind": kind, "where": where, "msg": msg, "confidence": sure, "step": step or self.step})


class Model:
    """The parsed mod (+ dependencies), shared by every step."""

    def __init__(self, mod_dirs, dep_dirs):
        self.mod = Parser("")
        self.files = []
        for root in mod_dirs + dep_dirs:
            for dp, dn, fn in walk(root):
                for f in fn:
                    if f.lower().endswith(".c"):
                        self.files.append((root, os.path.join(dp, f), root in mod_dirs))
        self.mod_only = Parser("")
        for root, p, is_mod in self.files:
            self.mod.root = root
            self.mod.parse_file(p)
            if is_mod:
                self.mod_only.root = root
                self.mod_only.parse_file(p)
        self.tdefs = dict(API["typedefs"])
        self.tdefs.update({k: v["target"] for k, v in self.mod.typedefs.items()})
        # compact mod classes (all, incl. deps) for chain resolution
        self.mclasses = {}
        for cname, c in self.mod.classes.items():
            base = next((d["base"] for d in c["decls"] if d["base"]), None)
            self.mclasses[cname] = {"b": re.sub(r"<.*", "", base).strip() if base else None,
                                    "m": {n: [{"p": [p["type"] for p in r["params"]]} for r in recs] for n, recs in c["methods"].items()},
                                    "F": list(c["fields"].keys())}
        self.method_names = set()
        for c in self.mod.classes.values():
            self.method_names.update(c["methods"].keys())
        self.method_names.update(self.mod.globals.keys())
        self.field_names = set()
        for c in self.mod.classes.values():
            self.field_names.update(c["fields"].keys())
        self.full_path = {os.path.relpath(p, root).replace("\\", "/"): p for root, p, is_mod in self.files if is_mod}
        # declaration lines of mod methods: (rel file, line) -> {names}
        self.decl_lines = {}
        for c in self.mod_only.classes.values():
            for n, recs in c["methods"].items():
                for r in recs:
                    self.decl_lines.setdefault((r["file"], r["line"]), set()).add(n)
        for n, recs in self.mod_only.globals.items():
            for r in recs:
                self.decl_lines.setdefault((r["file"], r["line"]), set()).add(n)
        self._text = {}

    def mod_files(self):
        for root, p, is_mod in self.files:
            if is_mod:
                yield root, p, os.path.relpath(p, root).replace("\\", "/")

    def text(self, p):
        """(raw, cleaned text, cleaned lines, #ifdef condition per line, newline offsets)"""
        if p not in self._text:
            raw = open(p, encoding="utf-8", errors="replace").read()
            text = clean(raw)
            lines = text.split("\n")
            _, cond = preprocess(lines)
            nl = [i for i, ch in enumerate(text) if ch == "\n"]
            self._text[p] = (raw, text, lines, cond, nl)
        return self._text[p]

    def method_body(self, rel_file, line):
        p = self.full_path.get(rel_file)
        if not p:
            return ""
        t = self.text(p)[1]
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


def scan_scripts(M, rep):
    """Step 1.29 -> 1.30."""
    mod, mod_only, tdefs, mclasses = M.mod, M.mod_only, M.tdefs, M.mclasses
    mod_method_names, mod_field_names = M.method_names, M.field_names
    method_body = M.method_body

    # 0) custom cars: 1.30 CarScript.GetBattery() = GetElectricitySourceDevice(), which is null in Transport
    #    (P:\scripts\3_game\vehicles\transport.c:835, carscript.c GetBattery); 1.29 found CarBattery/TruckBattery itself.
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
                    "base (transport.c:835) while NeedElectricitySourceDevice() is true (:830) → state NO_BATTERY, the engine never starts. "
                    "1.29 found the CarBattery/TruckBattery slot by itself. Add: override EntityAI GetElectricitySourceDevice() "
                    "{ return GetInventory().FindAttachment(CarBattery.SLOT_ID); } (vanilla: inheritedcars\\civiliansedan.c:59-69) "
                    "or override NeedElectricitySourceDevice() { return false; }.")

    # 1) overrides
    for cname, c in mod_only.classes.items():
        for mname, recs in c["methods"].items():
            for r in recs:
                if "override" not in r["mods"]:
                    continue
                where = f"{r['file']}:{r['line']}"
                if mname == "OnRPC" and any(anc == "ItemBase" for anc, _, _ in ancestors(API, cname, mclasses)) \
                        and "super.OnRPC" not in method_body(r["file"], r["line"]):
                    rep.add("WARN", "onrpc-no-super", where,
                            f"`{cname}.OnRPC` does not call super.OnRPC. On 1.30 item sounds arrive as a batched RPC handled in "
                            "ItemBase.OnRPC (case ERPCs.RPC_ITEM_SOUND_PLAY, P:\\scripts\\4_world\\entities\\itembase.c:2800) — "
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
                        if any(src == "vanilla" and mname in rec["m"] and mine in [[norm_type(t, tdefs) for t in ov["p"]] for ov in rec["m"][mname]]
                               for _, rec, src in ancestors(HSTEPS[0].OLD, cname, mclasses)):
                            continue  # matched 1.30.164078: a hotfix change, reported by the hotfix step
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
                        if any(src == "vanilla" and mname in rec["m"] for _, rec, src in ancestors(HSTEPS[0].OLD, cname, mclasses)):
                            continue  # existed in 1.30.164078 and is gone in 164200: the hotfix step reports it
                        rep.add("INFO", "override-unknown", where,
                                f"`{cname}.{mname}` overrides something that is neither 1.29 nor 1.30 vanilla — a dependency mod? "
                                f"Pass its source with --deps to check it.", "low")

    # 2) calls / identifiers in bodies
    call_rx = re.compile(r"(?<![\w.~])(?:\.\s*)?([A-Za-z_]\w*)\s*\(")
    dotted_rx = re.compile(r"\b([A-Z]\w*)\.([A-Za-z_]\w*)\b")
    word_rx = re.compile(r"\b([A-Za-z_]\w*)\b")
    for root, p, rel in M.mod_files():
        raw, text = M.text(p)[:2]
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
                rep.add("WARN", "runtime-findfile", where, "[DZEXP-134] On 1.30.164078 FindFile ignored $profile:/$mission:/$storage:/$saves: and "
                        "listed only the server working folder (measured on this machine). Officially fixed in 1.30.164200 (searches the "
                        "requested file system, normalises paths to '/'), not re-measured here. Re-test the enumeration on 164200, including "
                        "any workaround written for 164078 (CF.FindFileEx, absolute or rewritten $profile: paths).")
            if "$storage:" in ln:
                rep.add("INFO", "runtime-storage", where, "[reported: bzed] $storage: is not usable before the first MissionServer.OnUpdate after OnMissionStart.")
            if "GetMissionFolderPath" in ln:
                rep.add("WARN", "runtime-missionfolder", where, "[reported: bzed] GetMissionFolderPath() returns \"\" on 1.30 servers (engine passes a "
                        "forward-slash mission path); derive the folder from GetMissionPath().")
            if re.search(r"\bERPCs\.\w+\s*[+\-]\s*\d|\bcase\s+\d+\s*:", ln) and "RPC" in raw:
                rep.add("INFO", "rpc-raw-int", where, "raw RPC integer / arithmetic on ERPCs: 1.30 inserted values into ERPCs (every id after "
                        "RPC_PLAYER_SYMPTOM_OFF shifted). Use enum names and your own id range.", "low")
    return mod_only


# ---------------------------------------------------------------- hotfix step: script checks
def _vanilla_chain(idx, cname, mclasses):
    return [(anc, rec) for anc, rec, src in ancestors(idx, cname, mclasses) if src == "vanilla"]


def _guarded(cond, define):
    return any(part.strip().strip("()") == define for part in (cond or "").split("&&"))


def _call_args(text, open_paren):
    """Number of top-level arguments of the call whose '(' is at text[open_paren]; None if unbalanced."""
    depth, j, start = 0, open_paren, open_paren + 1
    while j < len(text):
        ch = text[j]
        if ch in "([{":
            depth += 1
        elif ch in ")]}":
            depth -= 1
            if depth == 0:
                inner = text[start:j]
                return len(split_top(inner)) if inner.strip() else 0
        j += 1
    return None


def _in_string(line, col):
    for m in re.finditer(r'"[^"\n]*"', line):
        if m.start() <= col < m.end():
            return True
    return False


def scan_scripts_hotfix(M, rep, st):
    """One hotfix step (st = HotfixStep). Rules that name a specific API only fire when the step's delta contains it."""
    mod, mod_only, tdefs, mclasses = M.mod, M.mod_only, M.tdefs, M.mclasses
    API, HOLD = st.NEW, st.OLD  # noqa: N806 - the step's new / old index
    HF_NEW, HF_NEW_OVERRIDE, HF_BODY, HF_SIG, HF_NEW_FIELDS = st.new, st.new_override, st.body, st.sig, st.new_fields  # noqa: N806
    HF_REMOVED_METHOD_NAMES, HF_REMOVED_FIELD_NAMES, HF_REMOVED_CLASSES = st.removed_methods, st.removed_fields, st.removed_classes  # noqa: N806
    HF_RENAMED, HF_GUARDED, HF_ARITY, HF_HINTS = st.renamed, st.guarded, st.arity, st.hints  # noqa: N806
    HF_ENUM_SHIFT, HF_ENUM_REMOVED, HF_ENUM_LINE = st.enum_shift, st.enum_removed, st.enum_line  # noqa: N806
    FRM, TO = st.frm, st.to  # noqa: N806
    hascrew_new = ("Transport", "HasCrew") in HF_NEW
    wb_typename = any(c == "Workbench" for c, *_ in HF_SIG.get("GetModule", []))
    ismoving_new = ("Transport", "IsMoving") in HF_NEW

    # 1) declarations in mod classes
    for cname, c in mod_only.classes.items():
        new_chain = _vanilla_chain(API, cname, mclasses)
        old_chain = _vanilla_chain(HOLD, cname, mclasses)
        new_names = [a for a, _ in new_chain]
        is_transport = "Transport" in new_names
        old_has = lambda name: any(name in rec["m"] for _, rec in old_chain)  # noqa: E731
        for mname, recs in c["methods"].items():
            for r in recs:
                where = f"{r['file']}:{r['line']}"
                has_override = "override" in r["mods"]
                ret = re.sub(r"\b(ref|owned|const|static|private|protected)\b", "", r["ret"]).strip()
                # Transport.HasCrew: new proto native
                if is_transport and hascrew_new and mname == "HasCrew":
                    rep.add("ERROR", "transport-hascrew", where,
                            f"`{cname}.HasCrew` collides with `proto native bool Transport.HasCrew()`, new in 1.30.164200 "
                            "(P:\\scripts\\3_game\\vehicles\\transport.c:175; vanilla MotorbikeScript.CanReleaseAttachment now calls it). "
                            "A script method cannot replace a native one: delete yours and use the native HasCrew(), or rename it "
                            "(and its callers). [compile behaviour not measured: expect an error or a hidden native]")
                    continue
                if is_transport and ismoving_new and mname == "IsMoving":
                    if r["params"] or ret != "bool":
                        rep.add("ERROR", "transport-ismoving", where,
                                f"`{cname}.{mname}({', '.join(p['type'] for p in r['params'])})` returns `{ret}` — 1.30.164200 added "
                                "`bool Transport.IsMoving()` (P:\\scripts\\3_game\\vehicles\\transport.c:819; CarScript's is now `override`, "
                                "carscript.c:2221). A different signature on a Transport subclass no longer compiles: make it "
                                "`override bool IsMoving()` or rename it.")
                    elif not has_override:
                        under_car = "CarScript" in new_names
                        rep.add("WARN", "transport-ismoving", where,
                                f"`{cname}.IsMoving()` is declared without `override`. 1.30.164200 added `bool Transport.IsMoving()` "
                                "(P:\\scripts\\3_game\\vehicles\\transport.c:819, logs Error(\"IsMoving() not implemented\") and returns false) and "
                                "made CarScript.IsMoving an override (carscript.c:2221). Declare it `override bool IsMoving()`"
                                + (" (it already shadowed CarScript.IsMoving on 164078)." if under_car else
                                   "; MotorbikeScript/BoatScript have no vanilla implementation, so yours is the one callers need."))
                    continue
                # generic: mod method with the name of a method 164200 added to a vanilla ancestor
                if not has_override and not old_has(mname):
                    hit = next(((anc, HF_NEW[(anc, mname)]) for anc, _ in new_chain if (anc, mname) in HF_NEW), None)
                    if hit:
                        anc, nr = hit
                        proto = nr.get("proto")
                        rep.add("ERROR" if proto else "WARN", "shadows-new-vanilla", where,
                                f"`{cname}.{mname}` is declared without `override` and now shadows vanilla `{anc}.{mname}` "
                                f"(`{'; '.join(nr['new'])}`), new in {TO} ({p_loc(nr['line'])})."
                                + (" A script method cannot replace a proto/native one — rename yours." if proto else
                                   " Add `override` (matching the signature) or rename it, otherwise vanilla callers and yours disagree."))
                if not has_override:
                    continue
                mine = [norm_type(p["type"], tdefs) for p in r["params"]]
                # a. override of a method whose signature changed
                done = False
                for anc, rec in new_chain:
                    if mname not in rec["m"]:
                        continue
                    new_ps = [[norm_type(t, tdefs) for t in ov["p"]] for ov in rec["m"][mname]]
                    if mine in new_ps:
                        # matches 164200; body changed / new intermediate override?
                        ov = rec["m"][mname][new_ps.index(mine)]
                        body_line = HF_BODY.get((anc, mname, ov["s"]))
                        nr = HF_NEW_OVERRIDE.get((anc, mname))
                        calls_super = re.search(r"\bsuper\s*\.\s*" + re.escape(mname) + r"\s*\(", M.method_body(r["file"], r["line"]) or "")
                        if nr:
                            rep.add("INFO", "vanilla-new-override", where,
                                    f"{TO} added `{anc}.{mname}` ({p_loc(nr['line'])}), overriding `{nr['shadows']['class']}.{mname}`. "
                                    + ("Your super call now runs the new vanilla code — check the combined behaviour."
                                       if calls_super else "Your override does not call super, so it bypasses the new vanilla logic."), "low")
                        elif body_line and not calls_super:
                            rep.add("INFO", "vanilla-body-changed", where,
                                    f"`{cname}.{mname}` replaces `{anc}.{mname}` without calling super, and vanilla {TO} changed that "
                                    f"method's body ({p_loc(body_line)}). If yours is a copy of vanilla code, re-diff it.", "low")
                        done = True
                        break
                    sc = [x for x in HF_SIG.get(mname, []) if x[0] == anc]
                    if sc:
                        hold_ovs = HOLD["classes"].get(anc, {}).get("m", {}).get(mname, [])
                        if mine in [[norm_type(t, tdefs) for t in ov["p"]] for ov in hold_ovs]:
                            rep.add("ERROR", "override-signature", where,
                                    f"`{cname}.{mname}({', '.join(p['type'] for p in r['params'])})` matched {FRM} `{anc}` but {TO} "
                                    f"changed it to `{'; '.join(sc[0][2])}` ({p_loc(sc[0][3])}). {HF_HINTS.get(mname, '')}".rstrip())
                            done = True
                            break
                    break  # nearest vanilla declaration decides
                if done:
                    continue
                # b. override of a method 164200 removed
                if not any(mname in rec["m"] for _, rec in new_chain):
                    anc_old = next((anc for anc, rec in old_chain if mname in rec["m"]), None)
                    if anc_old:
                        rep.add("ERROR", "override-removed", where,
                                f"`{cname}.{mname}` overrides `{anc_old}.{mname}`, which {TO} removed. {HF_HINTS.get(mname, '')}".rstrip())
        # fields that collide with fields 164200 added to a vanilla ancestor
        for fname, f in c["fields"].items():
            for anc, rec in new_chain:
                if (anc, fname) in HF_NEW_FIELDS and not any(fname in orec["F"] for _, orec in old_chain) and anc != cname:
                    rep.add("WARN", "field-shadows-new-vanilla", f"{f['file']}:{f['line']}",
                            f"`{cname}.{fname}` has the name of a member {TO} added to `{anc}` "
                            f"({HF_NEW_FIELDS[(anc, fname)]['type']} {fname}). Rename yours (duplicate member names in a class chain do not compile "
                            "or hide the vanilla one).", "medium")
                    break

    # 2) per-file checks
    word_rx = re.compile(r"\b([A-Za-z_]\w*)\b")
    dotted_rx = re.compile(r"\b([A-Z]\w*)\.([A-Za-z_]\w*)\b")
    cross_rx = re.compile(r"\.\s*Cross\s*\(") if "Cross" in HF_REMOVED_METHOD_NAMES else None
    names_for_calls = (HF_REMOVED_METHOD_NAMES - {"Cross"}) | set(HF_ARITY)
    call_rx = re.compile(r"(?<![\w~])(" + "|".join(sorted(map(re.escape, names_for_calls))) + r")\s*\(") if names_for_calls else None
    mod_cross = {cn for cn, c in mod.classes.items() if "Cross" in c["methods"]}
    for root, p, rel in M.mod_files():
        raw, text, lines, cond, nl = M.text(p)

        def line_of(off):
            return bisect.bisect_left(nl, off) + 1

        # a. vector.Cross
        for m in (cross_rx.finditer(text) if cross_rx else ()):
            i = line_of(m.start())
            recv = re.search(r"([A-Za-z_]\w*)\s*$", text[max(0, m.start() - 80):m.start()])
            if recv and recv.group(1) in mod_cross:
                continue
            if _in_string(lines[i - 1], m.start() - (nl[i - 2] + 1 if i > 1 else 0)):
                continue
            rep.add("ERROR", "vector-cross-removed", f"{rel}:{i}",
                    f"`.Cross(...)` — {HF_HINTS.get('Cross', 'vector.Cross was removed in ' + TO + '.')}" + (" (the mod declares its own Cross(): ignore this if the receiver is that class)"
                                                              if mod_cross else ""), "medium" if mod_cross else "high")
        # b. calls of removed methods / calls whose arity no longer fits
        if call_rx:
            for m in call_rx.finditer(text):
                n = m.group(1)
                i = line_of(m.start())
                if n in M.decl_lines.get((rel, i), ()):
                    continue  # the declaration itself
                if _in_string(lines[i - 1], m.start() - (nl[i - 2] + 1 if i > 1 else 0)):
                    continue
                via_super = re.search(r"\bsuper\s*\.\s*$", text[max(0, m.start() - 20):m.start()]) is not None
                if n in HF_REMOVED_METHOD_NAMES:
                    if n in M.method_names and not via_super:
                        continue
                    rep.add("ERROR", "call-removed", f"{rel}:{i}",
                            f"`{n}(...)` no longer exists in {TO} vanilla. {HF_HINTS.get(n, '')}".rstrip(), "medium")
                    continue
                nargs = _call_args(text, m.end() - 1)
                if nargs is None:
                    continue
                for cls, old_ar, new_ar, new_sigs, line in HF_ARITY[n]:
                    fits = lambda ar: any(lo <= nargs <= hi for lo, hi in ar)  # noqa: E731
                    if old_ar and fits(old_ar) and not fits(new_ar):
                        mine = [] if via_super else [rr for cc in mod.classes.values() for rr in cc["methods"].get(n, [])]
                        if any(sum(1 for x in rr["params"] if x["default"] is None) <= nargs <= len(rr["params"]) for rr in mine):
                            break  # the mod's own method with this name takes this many arguments
                        rep.add("ERROR", "call-signature", f"{rel}:{i}",
                                f"`{n}(...)` with {nargs} argument(s) matches {FRM} `{cls}.{n}` but {TO} declares "
                                f"`{'; '.join(new_sigs)}` ({p_loc(line)}). {HF_HINTS.get(n, '')}".rstrip(), "medium")
                        break
        # c. identifiers: removed fields/classes, Workbench guard, enums
        file_mentions = {e for e in HF_ENUM_SHIFT if re.search(r"\b" + re.escape(e) + r"\b", text)}
        switch_nv = set()
        for i, ln in enumerate(lines, 1):
            if not ln.strip() or ln.lstrip().startswith("#"):
                continue
            where = f"{rel}:{i}"
            lc = cond[i - 1] if i - 1 < len(cond) else ""
            for m in word_rx.finditer(ln):
                w = m.group(1)
                if w in mod.classes and w not in API["classes"]:
                    continue
                in_str = _in_string(ln, m.start())
                if w in HF_REMOVED_CLASSES and w not in mod.classes:
                    if w in HF_RENAMED:
                        rep.add("WARN" if in_str else "ERROR", "navmeshtest-renamed" if in_str else "class-removed", where,
                                f"`{w}` was renamed `{HF_RENAMED[w]}` in {TO} "
                                f"({p_loc(HF_REMOVED_CLASSES[w]['file'])}).")
                    elif not in_str:
                        rep.add("ERROR", "class-removed", where,
                                f"class `{w}` was removed in {TO} ({p_loc(HF_REMOVED_CLASSES[w]['file'])} in {FRM})"
                                + (" — the Workbench API lost ResourceBrowser / WorldEditor / WorldEditorAPI / WorldEditorTool." if
                                   HF_REMOVED_CLASSES[w]["file"].startswith("1_core/workbenchapi") else "."))
                elif w in HF_GUARDED and not in_str and w not in mod.classes:
                    g = HF_GUARDED[w]
                    if not _guarded(lc, g["new"]):
                        rep.add("ERROR", "workbench-only", where,
                                f"`{w}` exists only under `#ifdef {g['new']}` since {TO} ({p_loc(g['line'])}); the game/server "
                                f"compile no longer sees it (`Can't find variable '{w}'`, then `Can't compile \"…\" script module!` — "
                                f"reported on Dabs Framework). Wrap this code (class, attribute, call) in `#ifdef {g['new']}` … `#endif`.")
                elif w in HF_REMOVED_FIELD_NAMES and w not in M.field_names and not in_str:
                    rep.add("ERROR", "field-removed", where,
                            f"field `{w}` no longer exists in {TO} vanilla. {HF_HINTS.get(w, '')}".rstrip(), "medium")
            for m in re.finditer(r'"([A-Za-z_]\w*)"', ln):
                if m.group(1) in st.removed_cfg:
                    rep.add("WARN", "cfg-ref-removed", where,
                            f"\"{m.group(1)}\" is no longer a config class in {TO} (removed {st.removed_cfg[m.group(1)]}).", "medium")
            m = re.search(r"\b(GetModule|OpenModule|CloseModule)\s*\(\s*\"", ln) if wb_typename else None
            if m:
                rep.add("ERROR", "workbench-module-typename", where,
                        f"`Workbench.{m.group(1)}(\"…\")` — {TO} takes the module class: `{m.group(1)}(typename)`, e.g. "
                        "Workbench.GetModule(ScriptEditor) (P:\\scripts\\1_core\\workbenchapi\\workbench.c:10-12).")
            for m in dotted_rx.finditer(ln):
                e, v = m.group(1), m.group(2)
                if e in HF_ENUM_REMOVED and v in HF_ENUM_REMOVED[e]:
                    rep.add("ERROR", "enum-value-removed", where, f"`{e}.{v}` was removed in {TO} ({p_loc(HF_ENUM_LINE[e])}).")
                if e in HF_ENUM_SHIFT:
                    raw_int = re.search(r"\bint\s+\w+\s*=|\(\s*int\s*\)|\.Write\s*\(|Param\d*\s*<[^>]*\bint\b|\bint\s+\w+\s*\[", ln)
                    if raw_int:
                        rep.add("WARN", "enum-raw-int", where,
                                f"`{e}` value handled as a raw int here. {TO} inserted values into `{e}` ({p_loc(HF_ENUM_LINE[e])}); "
                                f"{len(HF_ENUM_SHIFT[e])} values shifted (e.g. " + ", ".join(f"{k} {a}→{b}" for k, (a, b) in list(HF_ENUM_SHIFT[e].items())[:3])
                                + f"). Ints stored/sent by a {FRM} build decode to a different value: store the name "
                                "(typename.EnumToString) or version your data.", "medium")
                    elif v in HF_ENUM_SHIFT[e]:
                        a, b = HF_ENUM_SHIFT[e][v]
                        rep.add("INFO", "enum-value-shifted", where,
                                f"`{e}.{v}` is {b} in {TO} (was {a}); fine by name, wrong if compared/stored as a number.", "low")
            for e in file_mentions:
                hint = ENUM_VAR_HINT.get(e)
                if not hint:
                    continue
                sm = re.search(r"\bswitch\s*\(\s*([^)]*)\)", ln)
                if sm and hint.search(sm.group(1)):
                    switch_nv.add(e)
                cm = re.search(r"\b(\w+)\s*(?:==|!=|<=|>=|<|>)\s*\d+\b", ln)
                if (cm and hint.search(cm.group(1))) or (e in switch_nv and re.search(r"\bcase\s+\d+\s*:", ln)):
                    rep.add("WARN", "enum-raw-int", where,
                            f"numeric literal compared with what looks like a `{e}` value. {len(HF_ENUM_SHIFT[e])} `{e}` values shifted in "
                            f"{TO} ({p_loc(HF_ENUM_LINE[e])}); compare with `{e}.<NAME>` instead.", "medium")


# ---------------------------------------------------------------- config.cpp
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


def config_files(mod_dirs):
    for root in mod_dirs:
        for dp, dn, fn in walk(root):
            for f in fn:
                if f.lower() == "config.cpp":
                    p = os.path.join(dp, f)
                    yield os.path.relpath(p, root).replace("\\", "/"), open(p, encoding="utf-8", errors="replace").read()


def scan_configs(mod_dirs, rep, cfg=None, ammo=None, old_v="1.29", new_v="1.30", cfgmods_check=True):
    """Config checks against a config delta (default: cfgdelta.json, 1.29 -> 1.30)."""
    cfg = cfg if cfg is not None else CFG
    ammo = ammo if ammo is not None else REMOVED_AMMO
    vchanged = {k.lower(): (k, v) for k, v in cfg["changed"].items()}
    removed = {k.split("/")[-1].lower(): k for k in cfg["removed"] if k.count("/") == 1}

    def elems(val):
        return [x.strip() for x in str(val).strip().strip("{}").split(",") if x.strip()]

    # vanilla classes whose hiddenSelections[] list changed / appeared
    hs_changed = {}
    for k, v in cfg["changed"].items():
        if k.count("/") != 1:
            continue
        for kind in ("props_changed", "props_added"):
            for pk, val in v.get(kind, {}).items():
                if pk.lower() == "hiddenselections[]":
                    old = elems(val[0]) if kind == "props_changed" else []
                    new = elems(val[1]) if kind == "props_changed" else elems(val)
                    hs_changed[k.split("/")[1].lower()] = (k, old, new)
    for rel, txt in config_files(mod_dirs):
        for path, base, ln, props in cfg_classes(txt):
            for key, (val, pl) in props.items():
                for m in re.finditer(r'"([A-Za-z_]\w*)"', val):
                    if m.group(1) in ammo:
                        new = ammo[m.group(1)]
                        rep.add("WARN", "cfgammo-renamed", f"{rel}:{pl}", f"`{key}` references CfgAmmo \"{m.group(1)}\", which {new_v} "
                                "removed" + (f" (renamed \"{new}\")." if new else "."))
            if cfgmods_check and path and path[0].lower() == "cfgmods":
                for key, (val, pl) in props.items():
                    if key.lower() in ("files[]", "value", "dir", "files[] +=") and "\\" in val:
                        rep.add("WARN", "cfgmods-backslash", f"{rel}:{pl}",
                                f"CfgMods `{key}` uses backslashes ({val[:80]}). Measured here: a Windows 1.30 server silently skipped a mod's "
                                "scripts with backslash CfgMods paths ([reported: bzed] the same on Linux), no error. Forward slashes work "
                                "everywhere: \"MyMod/scripts/4_World\".")
            if base and len(path) == 2 and base.lower() in hs_changed:
                vk, old, new = hs_changed[base.lower()]
                mine = {k.lower(): v for k, v in props.items()}
                for tk in ("hiddenselectionstextures[]", "hiddenselectionsmaterials[]"):
                    if tk in mine and "hiddenselections[]" not in mine:
                        n = len(elems(mine[tk][0]))
                        if n < len(new):
                            added = [x for x in new if x not in old]
                            rep.add("WARN", "cfg-hiddenselections-grew", f"{rel}:{mine[tk][1]}",
                                    f"`{path[-1]}` inherits `{base}`, whose hiddenSelections[] grew in {new_v} to {len(new)} entries "
                                    f"(new: {', '.join(added[:6]) or '-'}); this {tk} has {n}. The new selections keep the "
                                    "vanilla texture/material — extend the array (see config-delta).", "medium")
            if base and len(path) == 2 and base.lower() in removed:
                rep.add("WARN", "cfg-base-removed", f"{rel}:{ln}", f"`{path[-1]}` inherits `{base}`, a config class {new_v} removed ({removed[base.lower()]}).")
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
                            touched.append(f"{pk}: {new_v} vanilla added {', '.join(add[:8]) or '-'}; removed {', '.join(rem[:8]) or '-'}")
                        else:
                            touched.append(f"{pk}: {old_v} `{str(o)[:70]}` → {new_v} `{str(nv)[:70]}`")
                for pk in v.get("props_added", {}):
                    if pk.lower() in mine:
                        touched.append(f"{pk} (new in {new_v} vanilla)")
                if touched:
                    rep.add("WARN", "cfg-overrides-changed-vanilla", f"{rel}:{ln}",
                            f"`{'/'.join(path)}` sets keys that vanilla {new_v} changed — your value replaces the new vanilla one: "
                            + "; ".join(touched[:6]), "medium")


def scan_configs_hotfix(mod_dirs, rep, st):
    util_gone = "m_UtilityAutomaticBuild" in st.removed_fields
    for rel, txt in config_files(mod_dirs):
        for path, base, ln, props in cfg_classes(txt):
            low = {k.lower(): (k, v) for k, v in props.items()}
            if util_gone and "utility_build[]" in low:
                k, (val, pl) = low["utility_build[]"]
                has_toggle = "toggle_utility_part_update" in low
                rep.add("INFO" if has_toggle else "WARN", "cfg-utility-build", f"{rel}:{pl}",
                        f"`{'/'.join(path)}` sets `utility_build[]`, which 1.30.164200 no longer reads (ConstructionPartTypeData."
                        "InitUtilityBuildData reads the int `toggle_utility_part_update`, "
                        "P:\\scripts\\4_world\\classes\\basebuilding\\typeconstructiondata\\constructionparttyped.c:267-273)."
                        + (" This class already sets toggle_utility_part_update; the old key is dead weight (vanilla 164200 dropped 43 of "
                           "its 45 utility_build[] arrays; the 2 left at P:\\DZ\\structures_takistan\\Industrial\\config.cpp:402,462 are unread)."
                           if has_toggle else " Add `toggle_utility_part_update = 1;` to parts whose state change should re-run the utility "
                                              "parts (vanilla: P:\\DZ\\structures_takistan\\Industrial\\config.cpp)."))
            for nm in [path[-1], base]:
                if nm and nm in st.renamed:
                    rep.add("WARN", "navmeshtest-renamed", f"{rel}:{ln}",
                            f"`{nm}`: script class renamed `{st.renamed[nm]}` in {st.to} ({p_loc(st.removed_classes[nm]['file'])}).")
            for key, (val, pl) in props.items():
                for m in re.finditer(r'"([A-Za-z_]\w*)"', val):
                    if m.group(1) in st.removed_cfg:
                        rep.add("WARN", "cfg-ref-removed", f"{rel}:{pl}",
                                f"`{key}` references \"{m.group(1)}\", a config class {st.to} removed ({st.removed_cfg[m.group(1)]}).")
                    if m.group(1) not in st.renamed:
                        continue
                    rep.add("WARN", "navmeshtest-renamed", f"{rel}:{pl}",
                            f"`{key}` references \"{m.group(1)}\", renamed \"{st.renamed[m.group(1)]}\" in {st.to}.")
    if st.CFG:
        scan_configs(mod_dirs, rep, st.CFG, st.removed_ammo, st.frm, st.to, cfgmods_check=False)


LAUNCH_EXT = (".bat", ".cmd", ".ps1", ".sh", ".cfg")


def scan_launch_files(mod_dirs, rep, st):
    if st.to != "1.30.164200":
        return  # official 164200 notes: launch parameter renamed
    for root in mod_dirs:
        for dp, dn, fn in walk(root):
            for f in fn:
                if not f.lower().endswith(LAUNCH_EXT):
                    continue
                p = os.path.join(dp, f)
                rel = os.path.relpath(p, root).replace("\\", "/")
                try:
                    lines = open(p, encoding="utf-8", errors="replace").read().split("\n")
                except OSError:
                    continue
                for i, ln in enumerate(lines, 1):
                    if re.search(r"resolveFilePatchingUsingEnfusion", ln, re.I):
                        rep.add("WARN", "launch-param-renamed", f"{rel}:{i}",
                                "`-resolveFilePatchingUsingEnfusion` was renamed `-loadRVFilesViaEnfusionFS` in 1.30.164200 (official notes, Modding).")


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
                    # 1.30.164078 also renamed dayz_inventory `woodcrate` -> `handsactive`; 1.30.164200 named it `woodcrate` again
                    # (P:\gui\imagesets\dayz_inventory.imageset:683), so only `externallink` is still gone
                    for img in ("externallink",):
                        if re.search(r"image\s*:\s*" + img + r"\b", txt, re.I):
                            rep.add("WARN", "imageset-image-removed", rel, f"references image `{img}`, removed from the vanilla imageset in 1.30.")
                    if re.search(r"image\s*:\s*handsactive\b", txt, re.I):
                        rep.add("WARN", "imageset-image-removed", rel, "references image `handsactive` (dayz_inventory, 1.30.164078 only); "
                                "1.30.164200 renamed it back to `woodcrate` (P:\\gui\\imagesets\\dayz_inventory.imageset:683).")


ASSET_LEAD_RX = re.compile(r'"((?:\\\\|\\|/)[A-Za-z_][^"\n]*\.(?:layout|edds|paa|imageset|styles))"', re.I)


def scan_asset_paths(mod_dirs, rep, st):
    """Script/layout asset paths that START with a separator. Measured on 1.30.164200 (retail client, packed pbo, 2026-10-21):
    CreateWidgets / LoadImageFile / layout image0 / FileExist fail for "\\Mod\\x" and "/Mod/x" (RPT: Cannot open layout,
    LoadImageFile can't load); backslashes INSIDE the path, mixed separators and wrong case all load fine."""
    if st.to != "1.30.164200":
        return
    for root in mod_dirs:
        for dp, dn, fn in walk(root):
            for f in fn:
                low = f.lower()
                if not low.endswith((".c", ".layout", ".styles", ".imageset")):
                    continue
                p = os.path.join(dp, f)
                rel = os.path.relpath(p, root).replace("\\", "/")
                try:
                    lines = open(p, encoding="utf-8", errors="replace").read().split("\n")
                except OSError:
                    continue
                for i, ln in enumerate(lines, 1):
                    if ln.lstrip().startswith("//"):
                        continue
                    for m in ASSET_LEAD_RX.finditer(ln):
                        rep.add("WARN", "asset-path-leading-separator", f"{rel}:{i}",
                                f"`{m.group(1)}` starts with a path separator: on 1.30.164200 CreateWidgets returns null / "
                                "LoadImageFile and layout image0 load nothing for such paths (measured; RPT `Cannot open layout '\\...'`, "
                                "`LoadImageFile can't load '\\...'`). Drop the leading separator; backslashes inside the path are fine.",
                                sure="medium")


def scan_navmesh(mod_dirs, rep, st):
    """Map mods: navmesh.nm header = b'TESM' + uint32 version at offset 4 (1.29: 6, 1.30.164078: 7, 1.30.164200: 9)."""
    want = NAVMESH_VERSION.get(st.to)
    if not want:
        return
    for root in mod_dirs:
        for dp, dn, fn in walk(root):
            for f in fn:
                if not f.lower().endswith(".nm"):
                    continue
                p = os.path.join(dp, f)
                rel = os.path.relpath(p, root).replace("\\", "/")
                try:
                    with open(p, "rb") as fh:
                        head = fh.read(8)
                except OSError:
                    continue
                if len(head) < 8 or head[:4] != b"TESM":
                    rep.add("INFO", "navmesh-header", rel, "not a TESM navmesh header; cannot check the navmesh version.")
                    continue
                ver = int.from_bytes(head[4:8], "little")
                if ver != want:
                    rep.add("ERROR", "navmesh-version", rel,
                            f"navmesh version {ver}, {st.to} needs {want} (every vanilla P:\\DZ\\worlds\\*\\navmesh\\navmesh.nm is v{want}; "
                            "the exe compares the version before 'Navmesh could not be loaded: Invalid version' — derived from the exe, "
                            "not run in game). Infected/AI will not move: regenerate with DayZDiag " + st.to + " (dayz-ai-patterns).")


def main():
    ap = argparse.ArgumentParser(description="DayZ 1.29 -> 1.30 (+ 1.30 hotfix steps: " + ", ".join(h.label for h in HSTEPS) + ") static porting scan")
    ap.add_argument("mod")
    ap.add_argument("--deps", nargs="*", default=[])
    ap.add_argument("--from", dest="frm", choices=sorted(STEPS_FROM), default="1.29",
                    help="build the mod was written for (default 1.29 = run every step)")
    ap.add_argument("--out")
    ap.add_argument("--json")
    a = ap.parse_args()
    steps = STEPS_FROM[a.frm]
    rep = Report()
    M = Model([a.mod], a.deps)
    if STEP_129 in steps:
        rep.step = STEP_129
        scan_scripts(M, rep)
        scan_configs([a.mod], rep)
        scan_gui([a.mod], rep)
    for st in HSTEPS:
        if st.label not in steps:
            continue
        rep.step = st.label
        scan_scripts_hotfix(M, rep, st)
        scan_configs_hotfix([a.mod], rep, st)
        scan_launch_files([a.mod], rep, st)
        scan_navmesh([a.mod], rep, st)
        scan_asset_paths([a.mod], rep, st)
    # de-duplicate
    seen, items = set(), []
    for it in rep.items:
        key = (it["step"], it["sev"], it["kind"], it["where"], it["msg"])
        if key not in seen:
            seen.add(key)
            items.append(it)
    order = {"ERROR": 0, "WARN": 1, "INFO": 2}
    sorder = {s: i for i, s in enumerate([STEP_129] + [h.label for h in HSTEPS])}
    items.sort(key=lambda x: (sorder[x["step"]], order[x["sev"]], x["kind"], x["where"]))
    counts = {s: sum(1 for x in items if x["sev"] == s) for s in order}
    lines = [f"# 1.30 porting scan: `{a.mod}`", "",
             f"Steps: {' + '.join(steps)} (--from {a.frm})" + "".join(f"; {h.cfg_name} not available, config checks of {h.label} limited"
                                                             for h in HSTEPS if h.label in steps and h.cfg_name and not h.CFG), "",
             f"ERROR {counts['ERROR']} · WARN {counts['WARN']} · INFO {counts['INFO']}  "
             "(static pre-check; confirm with a 1.30 compile — script log `(E)` lines are the truth)", ""]
    for step in steps:
        sitems = [x for x in items if x["step"] == step]
        sc = {s: sum(1 for x in sitems if x["sev"] == s) for s in order}
        lines += ["", f"# Step {step}: ERROR {sc['ERROR']} · WARN {sc['WARN']} · INFO {sc['INFO']}"]
        cur = None
        for it in sitems:
            if it["sev"] != cur:
                cur = it["sev"]
                lines += ["", f"## {cur} ({step})", "", "| Where | Kind | Finding | Confidence |", "|---|---|---|---|"]
            lines.append(f"| `{it['where']}` | {it['kind']} | {it['msg'].replace('|', '/')} | {it['confidence']} |")
    md = "\n".join(lines) + "\n"
    if a.out:
        open(a.out, "w", encoding="utf-8").write(md)
    if a.json:
        json.dump(items, open(a.json, "w", encoding="utf-8"), indent=1, ensure_ascii=False)
    sys.stdout.reconfigure(encoding="utf-8")
    per = "; ".join(f"[{s}] E{sum(1 for x in items if x['step'] == s and x['sev'] == 'ERROR')} "
                    f"W{sum(1 for x in items if x['step'] == s and x['sev'] == 'WARN')} "
                    f"I{sum(1 for x in items if x['step'] == s and x['sev'] == 'INFO')}" for s in steps)
    print(md if not a.out else f"ERROR {counts['ERROR']} WARN {counts['WARN']} INFO {counts['INFO']} ({per}) -> {a.out}")
    sys.exit(1 if counts["ERROR"] else 0)


if __name__ == "__main__":
    main()
