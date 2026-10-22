"""Regression test: the fixture mods must produce exactly the known findings.

Run: python -I tests/test_scan_fixture.py   (from the skill folder; exit 0 = pass)

fixture_mod_129          a 1.29-style mod, scanned with the default --from 1.29 (both steps run; the hotfix step must stay silent)
fixture_mod_130_164078   a mod written for 1.30.164078, scanned with --from 1.30.164078 (hotfix step only)
"""
import json
import os
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
SKILL = os.path.dirname(HERE)
SCAN = os.path.join(SKILL, "scripts", "scan_mod_130.py")
STEP_129 = "1.29 -> 1.30"
STEP_HF = "1.30.164078 -> 1.30.164200"

EXPECTED_129 = {
    ("ERROR", "override-signature", "scripts/4_World/fixture.c:8"),      # ProcessVariables()
    ("ERROR", "override-signature", "scripts/4_World/fixture.c:40"),     # Construction.SetParent(BaseBuildingBase)
    ("ERROR", "override-signature", "scripts/5_Mission/fixture_ui.c:3"), # ActionTargetsCursor.Update()
    ("ERROR", "call-removed", "scripts/4_World/fixture.c:30"),           # ToggleHeadlights
    ("ERROR", "call-removed", "scripts/4_World/fixture.c:50"),           # UnlockServer
    ("ERROR", "class-removed", "scripts/5_Mission/fixture_ui.c:8"),      # MissionBenchmark
    ("ERROR", "enum-value-removed", "scripts/4_World/fixture.c:59"),     # EDLCId.DLC_FROSTLINE
    ("ERROR", "field-removed", "scripts/4_World/fixture.c:6"),           # m_ElapsedSinceLastUpdate
    ("WARN", "dead-hook", "scripts/4_World/fixture.c:3"),                # OnCEUpdate
    ("WARN", "dead-hook", "scripts/4_World/fixture.c:12"),               # CanDisplayAttachmentSlot(string)
    ("WARN", "dead-hook", "scripts/4_World/fixture.c:20"),               # CreateFrontLight
    ("WARN", "dead-hook", "scripts/4_World/fixture.c:24"),               # IsVitalCarBattery
    ("WARN", "dead-hook", "scripts/4_World/fixture.c:36"),               # BuildPartServer
    ("WARN", "cfgmods-backslash", "config.cpp:18"),
    ("WARN", "cfg-overrides-changed-vanilla", "config.cpp:26"),          # Fence attachments[] lacks Att_CodeLock
    ("WARN", "layout-copy", "gui/layouts/day_z_hud.layout"),
    ("WARN", "runtime-findfile", "scripts/4_World/fixture.c:65"),
    ("WARN", "dead-field", "scripts/4_World/fixture.c:73"),              # m_CarHornShortSoundName
    ("WARN", "class-obsolete", "scripts/4_World/fixture.c:22"),          # OffroadHatchbackFrontLight
    ("WARN", "cfg-hiddenselections-grew", "config.cpp:43"),              # AK_WoodBttstck camo -> camo, camo2
    ("WARN", "dead-hook", "scripts/5_Mission/fixture_ui.c:17"),          # IngameHudVisibility.UpdateElement(EHudElement)
    ("WARN", "dead-hook", "scripts/4_World/fixture.c:79"),               # GetDryingIncrement(string)
    ("WARN", "menu-id-collision", "scripts/5_Mission/fixture_ui.c:23"),  # MENU_FIXTURE = 47
    ("WARN", "onrpc-no-super", "scripts/4_World/fixture.c:87"),          # ItemBase.OnRPC without super
    ("WARN", "cfgammo-renamed", "scripts/4_World/fixture.c:93"),         # LandMineExplosion -> _Ammo
    ("WARN", "dead-hook", "scripts/4_World/fixture.c:99"),               # Building.CanDoorBeOpened(int, bool)
    ("WARN", "car-no-battery-source", "scripts/4_World/fixture.c:105"),  # FixtureCar : CarScript, no battery source
    ("WARN", "dead-hook", "scripts/4_World/fixture.c:107"),              # IsVitalCarBattery on FixtureCar
    ("INFO", "version-define", "scripts/5_Mission/fixture_ui.c:11"),
}

F = "scripts/4_World/fixture164078.c"
WB = "scripts/5_Mission/fixture_wb.c"
EXPECTED_164078 = {
    ("ERROR", "transport-hascrew", f"{F}:4"),               # modded CarScript: bool HasCrew()
    ("WARN", "transport-ismoving", f"{F}:8"),               # modded CarScript: bool IsMoving() without override
    ("ERROR", "transport-hascrew", f"{F}:16"),              # modded MotorbikeScript: bool HasCrew()
    ("WARN", "transport-ismoving", f"{F}:20"),              # modded MotorbikeScript: bool IsMoving() without override
    ("ERROR", "transport-ismoving", f"{F}:28"),             # BoatScript subclass: int IsMoving(float)
    ("ERROR", "vector-cross-removed", f"{F}:38"),           # v1.Cross(v2)
    ("ERROR", "vector-cross-removed", f"{F}:42"),           # o.GetDirection().Cross(x)
    ("ERROR", "override-signature", f"{F}:48"),             # override int GetSupportProvided(int)
    ("ERROR", "call-signature", f"{F}:50"),                 # super.GetSupportProvided(category) - 1 argument
    ("ERROR", "override-removed", f"{F}:52"),               # override TryHandleUtilityParts(EntityAI)
    ("ERROR", "call-removed", f"{F}:54"),                   # super.TryHandleUtilityParts(parent)
    ("WARN", "shadows-new-vanilla", f"{F}:56"),             # bool InvertSupportBehavior() without override
    ("ERROR", "call-removed", f"{F}:66"),                   # GetPartTypeUtilityAutobuildPartsList()
    ("ERROR", "field-removed", f"{F}:67"),                  # m_UtilityAutomaticBuild
    ("WARN", "enum-raw-int", f"{F}:73"),                    # int m_SavedNV = NVTypes.NV_GOGGLES_DAY
    ("WARN", "enum-raw-int", f"{F}:77"),                    # m_SavedNV == 15
    ("INFO", "enum-value-shifted", f"{F}:82"),              # NVTypes.NV_OPTICS_SANDSTORM_DAY 12 -> 13
    ("WARN", "field-shadows-new-vanilla", f"{F}:88"),       # CCTCursor subclass: float m_MaximalActionDistance
    ("INFO", "vanilla-body-changed", f"{F}:89"),            # CCTCursor.Can override without super
    ("ERROR", "class-removed", f"{F}:95"),                  # class FixtureNav : NavmeshTest
    ("WARN", "navmeshtest-renamed", f"{F}:99"),             # CreateObject("NavmeshTest_A")
    ("ERROR", "workbench-only", f"{WB}:1"),                 # [WorkbenchPluginAttribute(...)] outside #ifdef WORKBENCH
    ("ERROR", "workbench-only", f"{WB}:6"),                 # ScriptEditor / Workbench outside #ifdef WORKBENCH
    ("ERROR", "workbench-module-typename", f"{WB}:6"),      # Workbench.GetModule("ScriptEditor")
    ("WARN", "cfg-utility-build", "config.cpp:36"),         # utility_build[] without toggle_utility_part_update
    ("INFO", "cfg-utility-build", "config.cpp:43"),         # utility_build[] next to toggle_utility_part_update
    ("WARN", "navmeshtest-renamed", "config.cpp:49"),       # class Fixture_Nav: NavmeshTest_A
    ("WARN", "launch-param-renamed", "start_client.bat:3"), # -resolveFilePatchingUsingEnfusion
    ("ERROR", "navmesh-version", "navmesh/navmesh.nm"),     # TESM v7 header (164200 needs v9)
    ("WARN", "asset-path-leading-separator", "scripts/5_Mission/fixture_paths.c:3"),  # "\Mod\..." leading separator; inner backslashes on :4 must NOT be flagged
}
# (kind, where, text the message must contain)
MESSAGES_164078 = [
    ("vector-cross-removed", f"{F}:38", "(a * b)"),
    ("override-signature", f"{F}:48", "bool GetSupportProvided(int, out int)"),
    ("call-signature", f"{F}:50", "bool GetSupportProvided(int, out int)"),
    ("override-removed", f"{F}:52", "TryQueueUtilityPartsUpdate(ConstructionBase con)"),
    ("transport-hascrew", f"{F}:4", "proto native bool Transport.HasCrew()"),
    ("transport-ismoving", f"{F}:20", "override bool IsMoving()"),
    ("workbench-only", f"{WB}:6", "#ifdef WORKBENCH"),
    ("enum-value-shifted", f"{F}:82", "13"),
    ("navmesh-version", "navmesh/navmesh.nm", "needs 9"),
]


def scan(fixture, *extra):
    with tempfile.TemporaryDirectory() as td:
        out = os.path.join(td, "r.json")
        rc = subprocess.run([sys.executable, "-I", SCAN, os.path.join(HERE, fixture), "--json", out,
                             "--out", os.path.join(td, "r.md"), *extra]).returncode
        return rc, json.load(open(out, encoding="utf-8"))


ok = True


def check(name, rc, items, expected, step_filter=None):
    global ok
    got = {(x["sev"], x["kind"], x["where"]) for x in items if step_filter is None or x["step"] == step_filter}
    missing, extra = expected - got, got - expected
    print(f"[{name}] exit={rc} expected={len(expected)} got={len(got)} missing={len(missing)} extra={len(extra)}")
    for m in sorted(missing):
        print("  MISSING", m)
    for e in sorted(extra):
        print("  EXTRA  ", e)
    passed = rc == 1 and not missing and not extra
    print(f"[{name}] {'PASS' if passed else 'FAIL'}")
    ok = ok and passed


# 1) 1.29 fixture, all steps: the 1.29 step must give exactly EXPECTED_129, the hotfix step nothing
rc, items = scan("fixture_mod_129")
check("fixture_mod_129 / step 1.29", rc, items, EXPECTED_129, STEP_129)
hf = [x for x in items if x["step"] == STEP_HF]
print(f"[fixture_mod_129 / step hotfix] findings={len(hf)} {'PASS' if not hf else 'FAIL'}")
ok = ok and not hf

# 2) 164078 fixture, hotfix step only
rc, items = scan("fixture_mod_130_164078", "--from", "1.30.164078")
check("fixture_mod_130_164078 / --from 1.30.164078", rc, items, EXPECTED_164078)
wrong_step = [x for x in items if x["step"] != STEP_HF]
bad_msgs = [(k, w, t) for k, w, t in MESSAGES_164078
            if not any(x["kind"] == k and x["where"] == w and t in x["msg"] for x in items)]
for b in bad_msgs:
    print("  MESSAGE MISSING TEXT", b)
print(f"[fixture_mod_130_164078 / labels+messages] wrong_step={len(wrong_step)} bad_messages={len(bad_msgs)} "
      f"{'PASS' if not wrong_step and not bad_msgs else 'FAIL'}")
ok = ok and not wrong_step and not bad_msgs

# 3) 164078 fixture with the default --from 1.29: hotfix findings identical, 1.29 step silent
rc2, items2 = scan("fixture_mod_130_164078")
same = {(x["sev"], x["kind"], x["where"]) for x in items2 if x["step"] == STEP_HF} == {(x["sev"], x["kind"], x["where"]) for x in items}
silent = not [x for x in items2 if x["step"] == STEP_129]
print(f"[fixture_mod_130_164078 / --from 1.29] exit={rc2} hotfix_same={same} step129_silent={silent} "
      f"{'PASS' if same and silent and rc2 == 1 else 'FAIL'}")
ok = ok and same and silent and rc2 == 1

print("ALL PASS" if ok else "SOME FAILED")
sys.exit(0 if ok else 1)
