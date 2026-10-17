"""Regression test: the 1.29-style fixture mod must produce the known findings.

Run: python -I tests/test_scan_fixture.py   (from the skill folder; exit 0 = pass)
"""
import json
import os
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
SKILL = os.path.dirname(HERE)
SCAN = os.path.join(SKILL, "scripts", "scan_mod_130.py")
FIX = os.path.join(HERE, "fixture_mod_129")

EXPECTED = {
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

with tempfile.TemporaryDirectory() as td:
    out = os.path.join(td, "r.json")
    rc = subprocess.run([sys.executable, "-I", SCAN, FIX, "--json", out, "--out", os.path.join(td, "r.md")]).returncode
    got = {(x["sev"], x["kind"], x["where"]) for x in json.load(open(out, encoding="utf-8"))}
missing = EXPECTED - got
extra = got - EXPECTED
print(f"exit={rc} expected={len(EXPECTED)} got={len(got)} missing={len(missing)} extra={len(extra)}")
for m in sorted(missing):
    print("MISSING", m)
for e in sorted(extra):
    print("EXTRA  ", e)
sys.exit(0 if rc == 1 and not missing and not extra else 1)
