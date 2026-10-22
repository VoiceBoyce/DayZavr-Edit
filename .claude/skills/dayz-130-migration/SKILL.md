---
name: dayz-130-migration
description: "Use when: porting a DayZ mod written for 1.29 or older to 1.30, or a 1.30.164078 mod to the 1.30.164200 patch (обнови мод до 1.30 / под патч, мод сломался после обновления, проверь нужно ли обновлять мод); compile errors after an update ('marked as override, but there is no function with this name', 'Overloaded function not compatible', Can't compile World/Mission script module, Can't find variable 'Workbench', vector.Cross, GetSupportProvided/HasCrew clash); or a mod that compiles but silently stopped working (OnCEUpdate never called, headlights gone, battery override ignored, BuildPartServer hook not firing, FindFile, navmesh Invalid version). Runs scan_mod_130.py against the verified 1.29→1.30 and 164078→164200 API/config deltas, then recipes and a compile. Also refreshing the scanner for the next patch (scripts/PATCH_BUMP.md). Not for new 1.30 features — use the domain skill (dayz-vehicles, dayz-basebuilding, dayz-ui-development, enforce-script-reference)."
---

# DayZ 1.29 → 1.30 migration (+ 1.30 patch steps)

> Target: DayZ 1.30 stable (1.30.164200). Version labels: ../_shared/dayz-version-policy.md.
> Ground truth: `P:\` = unpacked 1.30.164200; `O:\DayZ 1.29 Старый распакованный клиент\` = unpacked 1.29.
> The 1.29 → 1.30 delta was generated on 2026-10-15 against 1.30.164078 (its `P:\` line refs are re-pointed to 164200);
> the 1.30.164078 → 1.30.164200 step was generated on 2026-10-21 from BI's DayZ-Script-Diff (`e3dda95…e8204c6`), the two
> dedicated-server installs and the official patch notes. Anything else says `[reported: …]`.

The scanner runs in **steps**: `1.29 → 1.30` (the big port) and `1.30.164078 → 1.30.164200` (the first 1.30 patch).
A 1.29 mod gets both; a mod already ported to 1.30.164078 runs with `--from 1.30.164078` and gets only the patch step.

This skill ports an existing mod. It does not teach the 1.30 systems themselves — once you know *what* to change,
open the domain skill for *how* (lights → `dayz-vehicles`, construction/code locks → `dayz-basebuilding`,
cursor/HUD → `dayz-ui-development`, CE/inventory/RPC → `enforce-script-reference`, storage → `dayz-persistence`).

## Files in this skill

| File | Use it for |
|---|---|
| `scripts/scan_mod_130.py` | **Run first.** Static scan of a mod folder: compile breakers, dead hooks, dead fields, removed classes/fields/enum values, obsolete classes and calls, config/layout/imageset issues, reported runtime traps. Every finding is labelled with its step. `--from {1.29,1.30.164078}`. Exit 1 if any ERROR. |
| `references/api-delta-1.30.164078-to-164200.md` | The 164200 patch: compile breakers, Workbench guard, behaviour changes, official engine fixes, generated tables with 164200 `path:line`. |
| `references/config-delta-1.30.164078-to-164200.md` | The 164200 patch on the data side (server PBOs compared entry by entry): 15 changed `config.bin` (motorbikes, Badlands/Nasdara structures and gear, camping, drinks, blunt melee), models, sounds, gui. |
| `scripts/PATCH_BUMP.md` | **Next patch:** exact commands to refresh indices, delta, scanner step and tests; with `../_shared/tools/remap_vanilla_refs.py` for the `path:line` refs in every skill. |
| `scripts/build_api_index.py`, `build_api_delta.py`, `compare_api_index.py`, `es_parse.py` | Index a vanilla scripts tree, diff two indices into a delta (`delta.json` schema + shifted enum values, `proto` flags, `shadows`, body/guard changes), compare indices. |
| `references/porting-recipes.md` | Before/after code for each common breakage (verified signatures). |
| `references/dead-hooks-1.30.md` | Every obsolete/removed method with vanilla call counts 1.29 vs 1.30 — "compiles but is never called any more". |
| `references/api-delta-1.29-to-1.30.md` | Full script API delta: removed classes, base changes, signature changes, removed methods/fields, newly `[Obsolete]`, visibility, constants, enums, globals, new-class index. |
| `references/api-additions-1.30.md` | New methods/fields on classes that already existed (what to use instead). |
| `references/config-delta-1.29-to-1.30.md` | config.cpp class/property changes (non-sound listed; sound classes counted). |
| `data/*.json` | Machine data the scanner uses: indices `api129-index.json`, `api130_164078-index.json`, `api130-index.json` (= current `P:\`, 164200); deltas `delta.json` (1.29 → 164078), `delta-1.30.164078-to-164200.json`, `cfgdelta.json`, `cfgdelta-1.30.164078-to-164200.json`; `deadhooks.json`, `obsolete_classes.json`; `vanilla-gui-files-changed-1.30.txt`. |
| `tests/` | Fixtures "1.29 mod" and "1.30.164078 mod" + regression test for the scanner (`python -I tests/test_scan_fixture.py` → `ALL PASS`). |

## Workflow

1. **Preflight.** `dayz-preflight` must report `P:\` as 1.30 (motorbike.c present, `GAME_STORAGE_VERSION = 144`).
   If `P:\` still holds 1.29 data, every check below is wrong — stop.
2. **Get source.** The scanner needs the unpacked mod (folder with `config.cpp` / `scripts\`). Only PBOs → extract with
   Mikero `ExtractPbo` (see `dayz-pbo-reverse-engineering`) into a scratch folder. Never scan or edit production
   server folders without asking.
3. **Scan.**
   ```powershell
   python -I "C:\Users\intenso2891\.claude\skills\dayz-130-migration\scripts\scan_mod_130.py" "<mod folder>" --out "<scratch>\scan_130.md" --json "<scratch>\scan_130.json"
   # mod depends on CF / Dabs / Expansion / another of your mods? add its unpacked source so overrides of ITS methods are resolved:
   #   --deps "<CF source>" "<other mod source>"
   # mod already ported to / written on 1.30.164078 (e.g. "does it need an update for the patch?"): only the patch step
   #   --from 1.30.164078
   ```
   Hidden folders (`.git`, `.dayz-mcp\corpus` copies of vanilla) are skipped.
4. **Triage in this order.**
   1. `ERROR override-signature / call-signature / override-removed / call-removed / class-removed / field-removed /
      enum-value-removed`, and from the patch step `workbench-only`, `workbench-module-typename`, `vector-cross-removed`,
      `transport-hascrew`, `shadows-new-vanilla` (vanilla added a proto/native method with the mod's method name)
      → will not compile. One compile error stops the whole module (`Can't compile "World" script module!`),
      and [reported: bzed] a 1.30 server that fails to compile scripts may segfault right after.
      `WARN transport-ismoving / shadows-new-vanilla / field-shadows-new-vanilla` → the mod's member now hides a new
      vanilla one: add `override` (+ super) or rename with your mod prefix.
   2. `WARN dead-hook` → compiles, then silently does nothing. These are the expensive ones: nobody notices until a
      player reports "lights don't work" or "raid protection doesn't trigger".
   3. `WARN cfg-* / layout-copy / imageset-*` → data that 1.30 changed under the mod.
   4. `WARN call-obsolete / override-obsolete` → works today, `SCRIPT (W)` in the log, plan the switch.
   5. `INFO runtime-* / version-define / rpc-raw-int / enum-raw-int / enum-value-shifted / vanilla-body-changed /
      vanilla-new-override / cfg-utility-build / launch-param-renamed` → needs a runtime test or a judgement call
      (`vanilla-body-changed`: the mod overrides without super a vanilla method whose body changed in the patch — re-read
      the new body; `vanilla-new-override`: the mod's super call now runs a new vanilla rule).
5. **Fix** with `references/porting-recipes.md`. For a member not covered there: find its row in
   `api-delta-1.29-to-1.30.md`, then read the 1.30 declaration in `P:\scripts` before writing the call.
6. **Version strategy.** Default: the mod becomes 1.30-only — write plain 1.30 code, no version `#ifdef`.
   Only if the same PBO must still load on a 1.29 server: old code under `#ifdef DAYZ_1_29`, 1.30 code in `#else`
   (1.29 vanilla itself uses `#ifdef DAYZ_1_29`, 1.29: `O:\…\scripts\3_game\systems\dynamicmusicplayer\dynamicmusicplayerregistry.c:116`).
   Never `#ifdef DAYZ_1_30` — it silently drops the code on 1.31.
7. **Compile on 1.30.** Build and boot the way the project does it (this user: dayz-mcp `mod_lint` → `mod_build`, then
   test server/client only through their own `.bat` launchers; see `dayz-test-ingame`). Read the script log:
   zero `SCRIPT (E)`; review every `SCRIPT (W): '…' is obsolete`. The scanner is a pre-check — the compiler is the verdict.
8. **Runtime re-test** what static analysis cannot see: dead-hook replacements actually firing, lights/horn on vehicles,
   building/dismantling hooks, file enumeration (`FindFile`), anything touching `$storage:`.
9. **Persistence.** Before the first 1.30 boot of a server with existing data: back up `storage_*`.
   `GAME_STORAGE_VERSION` 142 → 144 (`P:\scripts\3_game\global\game.c:5`); a 1.30-written storage is not loadable by
   1.29. Mods that version their own `OnStoreSave/OnStoreLoad` data keep working; see `dayz-persistence`.
   **Characters saved by 1.29 are very likely NOT loadable by 1.30** (static reading, not measured):
   - `PCOHandlerStats` registers PCO versions 100, 101 and current = **130** (`4_world\classes\playerstats\playerstatspco.c:12-14`,
     `:328-362`); `GetPCO(142)` falls back to the closest lower one = **v130**, which reads **12** stats — 1.29's v115 wrote **11**
     (`THERMAL_BIAS` is new). The stat stream is then shifted by one float.
   - `PlayerBase.OnStoreLoad` then calls `m_ThermalBiasHandler.OnStoreLoad(ctx, version)` with no version gate
     (`4_world\entities\manbase\playerbase.c:7520`, `4_world\classes\thermalbiashandler.c:72-78`); a 1.29 record has no such data,
     and a failed read returns false from `PlayerBase.OnStoreLoad`.
   Treat a 1.29 → 1.30 server upgrade as a **character wipe** unless a test boot on a copy of `storage_1` proves otherwise.
   Mods with their own PCO stats: 1.30 indices ≥ 12 (index 11 is now `THERMAL_BIAS`).

## Patch step 1.30.164078 → 1.30.164200 — the short list

Full tables: `references/api-delta-1.30.164078-to-164200.md` (scripts) and `config-delta-1.30.164078-to-164200.md` (data).
Lines are `P:\scripts` on 164200.

**Compile breakers for a mod written on 164078**

| 164078 | 164200 | Where |
|---|---|---|
| Workbench API visible to every module: `Workbench`, `WBModuleDef`, `ScriptEditor`, `ResourceBrowser`, `WorldEditor`, `WorldEditorAPI`, `WorldEditorTool`, `ButtonAttribute`, `WorkbenchPluginAttribute`, `WorkbenchToolAttribute` | only inside `#ifdef WORKBENCH` (except `ResourceName`, `WorkbenchPlugin`); `WorldEditor*`/`ResourceBrowser` removed; `Workbench.GetModule/OpenModule/CloseModule(typename)` (was string) — unguarded use: `Can't find variable 'Workbench'`, `Can't compile "Mission" script module!` [reported: Dabs Framework `gui/mvc/samplemvc.c:33`] | `1_core\workbenchapi.c:6-93`, `1_core\workbenchapi\**` |
| `vector.Cross(vector)` (new in 164078) | removed — `a * b` is the cross product (vanilla: `environment.c:696-697`) | `1_core\proto\enconvert.c` |
| `int ConstructionPart.GetSupportProvided(int)` / `GetSupportRequirement(int)` (+ `ConstructionPartTypeData.GetPartTypeSupport*`, `ConstructionPartStaticsSupportData.GetSupport*`) | `bool …(int category, out int strength)`; -1 = not configured | `4_world\classes\basebuilding\constructionpart.c`, `rebuilding\constructionstatics.c` |
| `ConstructionPart.TryHandleUtilityParts(EntityAI)`, `GetPartTypeUtilityAutobuildPartsList()`, `m_UtilityAutomaticBuild`; config `utility_build[]` | `TryQueueUtilityPartsUpdate(ConstructionBase)` → `ConstructionBase.QueueUpdateUtilityParts()`; config int `toggle_utility_part_update` | `constructionparttyped.c`, `constructionbase.c` |
| a Transport subclass declaring `bool HasCrew()` / `bool IsMoving()` | `Transport.HasCrew()` is now `proto native` (`3_game\vehicles\transport.c:175`); `Transport.IsMoving()` is a base method (`:819`, logs `Error("IsMoving() not implemented")`; only `CarScript` overrides it) — rename helpers with your prefix, or `override` IsMoving | `transport.c` |
| `NavmeshTest*` | `Land_NavmeshTest*` | `4_world\entities\navmeshtest\navmeshtest.c` |

**Compiles, behaves differently** — `NVTypes` values after `NV_OPTICS_UNDERGROUND` shifted +1/+2 (raw ints break);
server-side `CCTCursor`/`CCTCursorParent`/`CCTCursorNoRuinCheck` check is now only `distance(object centre, player) ≤
maxDist + collision radius` (no cursor hit, no LOS on the server — re-test custom actions at max reach; security checks
belong in your own server `ActionCondition`); 23 vanilla target actions no longer lock the target (`IsLockTargetOnUse`
false); `MotorbikeScript.CanReleaseAttachment` refuses ANY detach while the engine runs or someone is on the bike (a mod
slot on a bike — radio, cassette — is affected through super); motorbike light selections (`FrontOn/FrontOff`, Motorbike_02
brake index 6, `hiddenSelections[]` slots 5-7 renamed `lights_front/rear/brake`); obstruction raycasts rewritten
(`MiscGameplayFunctions.IsObjectObstructed*`, 10 m cap in `ActionTargets`); `ActionDrinkCan` registered earlier (action ids
are registration order); `ItemActionsWidget` sets the alpha of the `ia_*_mtl_up/down` arrows every rebuild;
`PluginInventoryRepair` is a release client plugin; Compass/GPS in worn clothing count as "in possession".

**Engine fixes in the official notes** `[reported: BI 1.30.164200 notes, not re-measured here]` — `FindFile` searches the
requested file system and normalises paths to `/` (DZEXP-134; it was broken on 164078, measured on this machine) → re-test
mods that enumerate `$profile:` and their workarounds (they may now see files the index fallback never knew about);
`#include` works; circular-inheritance compile error fixed; `ImageWidget.LoadImageFile` images got the SRGB flag (they were
too bright on 164078 — script-loaded coloured images now look darker/more saturated); new PBO named property
`fsShadowing=1`; launch parameter `-resolveFilePatchingUsingEnfusion` → `-loadRVFilesViaEnfusionFS`.

**Asset paths — measured, not a 164200 breaker** (retail 1.30.164200 client, packed PBO, 2026-10-21): `\\` inside a
layout / image path, mixed separators and wrong case all load (`CreateWidgets`, `LoadImageFile`, layout `image0`,
`FileExist`, `FindFile`). Only a path that **starts** with `\` or `/` fails (`Cannot open layout '\…'`,
`LoadImageFile can't load '\…'`) — scanner `WARN asset-path-leading-separator`. A community report "every `\\` path broke
after 164200, switch to `/`" is wrong as stated; a leading separator is the likely real cause (its 164078 behaviour is unknown).

**Data** — no `GAME_STORAGE_VERSION` change (still 144; storages and characters from 164078 load). Configs changed only in
15 PBOs (Badlands/Nasdara structures, gear, dog; motorbikes; camping; drinks; blunt melee; `dta\scripts`); Chernarus,
Livonia, Sakhal, firearms and wheeled cars unchanged.

**Navmesh v7 → v9 (maps!)** — every vanilla `navmesh.nm` on 164200 has header `TESM` + version **9** (uint32 at offset 4;
`P:\DZ\worlds\{chernarusplus,enoch,nasdara,sakhal}\navmesh\navmesh.nm`; 1.30.164078 = 7, 1.29 = 6). The 164200 exe compares
the version against 9 before `Navmesh could not be loaded: Invalid version` (164078 compared against 7) — derived from the
exes, not run in game. So a **custom/third-party map navmesh built for 164078 (v7) is most likely rejected on 164200**:
infected and AI stand still. Regenerate it with DayZDiag 1.30.164200 (`dayz-ai-patterns`) and check the header.
Also new: config-driven navmesh blocks on dynamic structures (`Fence` → `Navmesh/TransformParts/Gate`,
`P:\DZ\gear\camping\config.cpp:4418`; `Land_IrrigationTunnel_Entrance_01` → `Navmesh/HideParts`).

## What breaks 1.29 → 1.30 — the short list (verified on 1.30.164078, refs re-pointed to 164200)

### Compile breakers (override or call no longer matches)

| 1.29 | 1.30 | Where (P:\scripts) |
|---|---|---|
| `EntityAI/ItemBase.ProcessVariables()` | `ProcessVariables(float elapsedTime)` | `3_game\entities\entityai.c:3913`, `4_world\entities\itembase.c:4663` |
| `Construction.SetParent(BaseBuildingBase)`, ctor `Construction(BaseBuildingBase)` | `SetParent(EntityAI)`; extra ctor `Construction(EntityAI)`; `Construction : ConstructionBase : ConstructionBasic` | `4_world\classes\basebuilding\construction.c:6,44` |
| `ActionTargetsCursor.Update()` / `BuildFixedCursor()` | `Update(bool fullUpdate = true)` / `BuildFixedCursor(bool forceRebuild = true)` | `5_mission\gui\actiontargetscursor.c:310,267` |
| `CombinationLock.SetCombination(int)`, `DialNextNumber()`, `SetNextDial()`, `CheckLockedStateServer()` | `(int, bool setOtherSide = true)`, `(int lockSide = FACING_OUTSIDE)`, same, `(bool insideLock = false)`; several lock methods became `protected` | `4_world\entities\itembase\combinationlock.c:233,273,340,511` |
| `Environment.ApplyWetnessToItem(ItemBase)` / `ApplyDrynessToItemEx(ItemBase, EnvironmentDrynessData)` | extra `EnvironmentEntityData itemAttributes = null` param | `4_world\classes\environment\environment.c` |
| `HumanItemAccessor.OnItemInHandsChanged(bool)` | `(bool, bool pChangeAnimationInstance = true)` (proto) | `3_game\humanitems.c:112` |
| `BotGuardHasItemInHands.GuardCondition(HandEventBase)` | `GuardCondition(BotEventBase)` | `4_world\systems\bot\botguards.c:64` |
| `ScriptConsole.RegisterTab(handler)` | `RegisterTab(int id, handler)` | `5_mission\gui\scriptconsole.c:193` |
| `MainMenuConsole/ServerBrowserTab.OnDLCChange(EDLCId)` | `OnDLCChange()` | `5_mission\gui\newui\…` |
| `ServerBrowserMenuNew.Play()` | `Play(bool warnAboutMissingDLC = true)` | `serverbrowsermenunew.c:701` |
| `CrashDebugData.SendData(PlayerBase)` | `SendData(DayZPlayer)` | `3_game\vehicles\transport.c:1099` |
| `CarScript.ToggleHeadlights()` | removed → `Transport.LightToggle()` (proto) | `3_game\vehicles\transport.c:239` |
| `CarScript.GenerateCarHornAINoise`, `CarHorn*ActionInput` bases | `VehicleHornComponent`, `VehicleHornShort/LongActionInput`, `ActionVehicleHornShort/Long` | `4_world\…\actionvehiclehorn.c` |
| `AmmoTypesAPI.GetExplosionParticleID(ammo, surface)` | removed → `AmmoTypesAPI.GetExplosiveEffectData(ammo, surface)` (`.m_ParticleID`, sound set) | `3_game\global\ammotypes.c:90` |
| `CombinationLock.UnlockServer(…)` | removed → `UnlockOnServer(EntityAI player, EntityAI parent, bool isInsideLock = false)` | `combinationlock.c:409` |
| `CGame.VerifyWorldOwnership`, `CGame.GoBuyWorldDLC` | removed (`ContentDLC`, `ContentDLCStorePrompt`) | `3_game\services\` |
| `EDLCId.DLC_UNKNOWN / DLC_FROSTLINE` | `EDLCId.UNKNOWN / FROSTLINE / BADLANDS` | `3_game\services\contentdlc.c:3` |
| `LockAction.DIAL_NUMBER_CHANED / DIAL_INDEX_CHANGED` | `…_INSIDE / …_OUTSIDE` variants | `combinationlock.c` |
| fields `EntityAI.m_ElapsedSinceLastUpdate`, `m_LastUpdatedTime`, `ItemBase.m_ItemActionOverrides`, `CarScript.m_ForceUpdateLights` | removed (use the `elapsedTime` parameter; action overrides live on `EntityAI`) | delta §5 |
| classes `MissionBenchmark`, `BenchmarkConfig`, `BenchmarkLocation`, `Settings`, `GameSettings`, `SettingsMenu`, `UndergroundDiag*`, `DebugAgentData` | removed | delta §1 |

Adding a parameter — even with a default — breaks every `override` of the old parameter list. Calls keep compiling.

### Silent breakers — compile on 1.30, never called any more (the scanner's `dead-hook`)

| Override in an old mod | Vanilla 1.30 calls instead |
|---|---|
| `OnCEUpdate()` | `OnCEIterate(float currentTime, float elapsedTime)` (`3_game\entities\entityai.c:3904`; 1.29 called OnCEUpdate 4×, 1.30 0×) |
| `CarScript.CreateFrontLight()` / `CreateRearLight()` and all `*LightShineOn/Off`, `BrakesRearLight`, `TailLightsShineOn` … helpers | `m_LightsComponent.RegisterLight("Front"/"Rear", new VehicleLightData(new <Profile>()))` in the car constructor (`4_world\entities\vehicles\carscript.c:229-271`, e.g. `inheritedcars\civiliansedan.c:21-22`) |
| `CarScript.IsVitalCarBattery()` / `IsVitalTruckBattery()` | `Transport.NeedElectricitySourceDevice()` + `GetElectricitySourceDevice()` (`3_game\vehicles\transport.c:830-838`) |
| **A custom car that extends `CarScript` directly** (no vanilla car in between) and relied on 1.29's default battery lookup | 1.29 `CarScript.GetBattery()` found the `CarBattery`/`TruckBattery` attachment itself (1.29: `carscript.c:3015-3027`); 1.30 `GetBattery()` returns `GetElectricitySourceDevice()` — `null` in the base (`transport.c:835`) while `NeedElectricitySourceDevice()` is `true` (`:830`) → `NO_BATTERY`, **the engine never starts**. Override `GetElectricitySourceDevice()` (and `GetElectricitySourceDeviceType()`), like every vanilla car (`inheritedcars\civiliansedan.c:59-68`). Scanner: `car-no-battery-source`. CarScript-based helicopters/planes are hit too |
| `CarScript.CheckVitalItem(…)`, `CarPartsHealthCheck()` | `Transport.CheckVitalItemCallback(…)` (protected, `transport.c:423`), `PartsHealthCheck` |
| Setting `m_CarHornShortSoundName` / `m_CarHornLongSoundName` in a car ctor (fields kept, marked DEPRECATED, never read: `carscript.c:2765-2766`) | `m_HornComponent.RegisterSound(VehicleHornMode.SHORT/LONG, "<SoundSet>")` (`inheritedcars\civiliansedan.c:24-25`) — otherwise the modded car's horn is silent |
| `Construction.BuildPartServer(Man, string, int)` / `DismantlePartServer(…)` | `BuildPartServerEx(Man player, string part_name, int action_id)` / `DismantlePartServerEx(…)` (`4_world\classes\constructionbase.c:180,191`) — **raid-protection / base mods that hook building here stop working** |
| `EntityAI.CanDisplayAttachmentSlot(string slot_name)` | the `int slot_id` overload only (`entityai.c:1788`, called at `:1804`) |
| `ExplosivesBase.AddExplosionEffectForSurface` / `SetParticleExplosion` / `GetParticleExplosionID` | per-surface effects from `AmmoTypes` (`AmmoTypesAPI.GetExplosiveEffectData`) |
| `NoiseAIEvaluate.GetNoiseReduction*`, `Weather.GetNoiseReductionByWeather*` (as AI noise hooks) | native `AIParams` config dampening |
| `EmoteManager.ForceSurrenderState`, detecting surrender via `SurrenderDummyItem` (class now `[Obsolete]`, unused) | `PlayerBase.SetSurrenderState(bool)` / `IsSurrendered()` (`playerbase.c:2116`, `man.c:67`) |
| `Building.CanDoorBeOpened(int doorIndex, bool checkIfLocked)` — **door-blocking mods (raid protection, safe zones)** | `CanDoorBeOpened(notnull DoorManipulationParams params)` (`3_game\entities\building.c:141`; fields `m_DoorIndex`, `m_CheckIfLocked`, `m_Caller` at `:5-10`); the open action calls only this one (`actionopendoors.c:52,72`). `CanDoorBeClosed(int)` is still what the close action calls (`actionclosedoors.c:38,55`) |
| `IngameHudVisibility.UpdateElement(EHudElement)` / `UpdateVisibility(EHudElement, bool)` — HUD mods | `UpdateElement(EHudElement, bool ignoreWidgetVisiblityState)` / `UpdateVisibility(EHudElement, bool, bool)` (`5_mission\gui\ingamehudvisibility.c:100,128,169,177`); the old ones only forward |
| `ItemBase.GetDryingIncrement(string)` / `GetSoakingIncrement(string)` | the enum overloads `(EEnvironmentDryingCategories)` / `(EEnvironmentSoakingCategories)` — vanilla 1.29 called `GetDryingIncrement("player")`, 1.30 calls `GetDryingIncrement(EEnvironmentDryingCategories.PLAYER)` (`4_world\classes\environment\environment.c:1169`) |
| `ItemBase.GetHeatIsolationInit()` | not called any more; `[Obsolete("1.30: No replacement")]` (`itembase.c:4942`) — read the item type's `m_HeatIsolation` / `GetHeatIsolation()` path instead |
| `ExplosivesBase.SetParticleExplosion` / particle in `OnExplosionEffects` | `ExplosivesBase.OnExplosionEffects` only creates the light now (`4_world\entities\explosivesbase.c:66-70`); the particle comes from `AmmoTypesAPI` / `CfgAmmo <ammo> particle=` (`3_game\global\ammotypes.c:90-130`) |
| an item's `OnRPC` override **without** `super.OnRPC` | item sounds are a batched `ERPCs.RPC_ITEM_SOUND_PLAY` handled in `ItemBase.OnRPC` (`4_world\entities\itembase.c:2800`) — skipping super loses them (scanner: `onrpc-no-super`) |
| `World.Is3rdPersonDisabled()` (vanilla no longer asks it) | `World.GetThirdPersonViewMode()` → `ThirdPersonMode.DISABLED/ENABLED/VEHICLES_ONLY` (`3_game\global\world.c:177,301`) |
| `ActionDrinkPondContinuous.GetLiquidSource`, `CCTWaterSurface`, `ActionFillBottleBase` | `CCTLiquid` (`4_world\…\targetconditionscomponents\cctliquid.c`), `ActionObtainLiquidBase`, `Object.GetLiquidSourceObjectType()` |
| `Man/EntityAI.*TakeEntityAsAttachment*`, `*TakeEntityToCargo*` (self as target) | `*TakeEntityToTargetAttachment(Ex)(target, item[, slot])`, `*TakeEntityToTargetCargo(target, item)`, `*TakeEntityToTargetCargoEx(cargo, item, row, col)` (`3_game\entities\entityai.c:1972-2010`); GameInventory: `TakeEntityAsTargetAttachment(Ex)` (`3_game\systems\inventory\inventory.c:1100,1142`) |

Full list with call counts: `references/dead-hooks-1.30.md`.

### Data that changed under the mod

- **Enums**: `ERPCs` got `RPC_SYMPTOM_PARAM_SYNC` and other values inserted — 160 vanilla ids shifted. Code that stores or
  sends vanilla RPC ids as raw ints, or counts from a vanilla value, breaks. `EActions` (30 shifted), `DiagMenuIDs` (186)
  likewise. `CarFluid`/`BoatFluid` are now `enum CarFluid : ETransportFluid {}` — `CarFluid.FUEL` still compiles.
- **Config** (`config-delta-1.29-to-1.30.md`): e.g. `CfgVehicles/Fence` `attachments[]` gained `"Att_CodeLock"` — a mod that
  redefines the vanilla `attachments[]` hides the new code lock slot; `Inventory_Base.varWetMax` 0.249 → 0.049; ~180 CfgAmmo
  classes changed. The scanner flags redefinitions that set a changed key.
- **Retextures**: several vanilla classes gained `hiddenSelections` in 1.30 — `AK_WoodBttstck`/`AK_WoodHndgrd`/`AKM_Base`
  `{"camo"}` → `{"camo","camo2"}`, `Offroad_02` + its doors gained paint selections (`texture_roof_ext/int` …), GPSReceiver,
  Magnum/Glock parts. A 1.29 retexture that sets fewer `hiddenSelectionsTextures[]` entries leaves the new selections vanilla
  (scanner: `cfg-hiddenselections-grew`). Every vanilla `.p3d` is now ODOL v56 (1.29: v54) — tools that read vanilla
  ODOL need v56 support.
- **CfgMods script paths**: use forward slashes (`"MyMod/scripts/4_World"`). [reported: bzed] with backslashes a 1.30 Linux
  server loaded the config but compiled none of the mod's scripts, without an error; Windows not tested. Forward slashes are
  safe everywhere, so always switch.
- **GUI**: vanilla `day_z_hud.layout`, the xbox inventory layout, `main_menu_console`, `script_console`, several debug
  layouts changed; `dayz_gui` lost image `externallink`, `dayz_inventory` lost `woodcrate`
  (`data/vanilla-gui-files-changed-1.30.txt`). A mod shipping an edited copy of a vanilla layout must re-diff it.
- **Persistence**: `GAME_STORAGE_VERSION` 144; `PlayerStatsPCO_current : PlayerStatsPCO_v130`.
- **Wheels**: the ruined-wheel swap now works by class name — every wheel class `X` needs a `X_Ruined` config class and vice
  versa, otherwise `ErrorEx("No config class entry for wheel entity …")` and no swap
  (`4_world\entities\core\inherited\inventoryitemtype.c:17-45`). Vehicle `.p3d` readers must handle ODOL v56.
- **Menu ids**: vanilla took `MENU_DIGITAL_CODELOCK = 47` and `MENU_UNDERGROUND_TRIGGER_EDITOR = 48`
  (`3_game\constants.c:222-223`; 1.29 ended at 46). A mod menu registered with 47/48 now collides (scanner: `menu-id-collision`).
- **CfgAmmo rename**: `LandMineExplosion` → `LandMineExplosion_Ammo` (`P:\DZ\gear\traps\config.cpp:17`); string references to
  the old name find nothing (scanner: `cfgammo-renamed`).
- **Wetness**: `Inventory_Base` `varWetMax` default 0.249 → 0.049 (= `varWetMin`, `P:\DZ\data\config.cpp:3230-3231`). A modded
  item/garment that relied on the inherited default can no longer get wet; set `varWetMax` explicitly if it should.
- **Infected**: `m_MindState` net-sync range grew 4 → 5 (`4_world\entities\creatures\infected\zombiebase.c:54`,
  `DayZInfectedConstants.MINDSTATE_COWER`); code that switches over mind states must handle the new value.
- **Layouts with GUID headers**: 9 vanilla layouts now use `XWidgetClass "{GUID}" { Name "x" …`; regex-based layout tools that
  expect `XWidgetClass name {` miss those widgets.
- **`SlotsIcon`** now extends `SlotsIconBase`, and the widget building moved there — a 1.29 `SlotsIcon` constructor override
  that expects the old flow must be re-read against `5_mission\gui\inventorynew\containeditems\slotsicon.c`.

### Runtime traps nobody can see statically `[reported: bzed dayz-dev-plugin, Linux 1.30.164014 server]`

- `FindFile()` ignored `$profile:` / `$mission:` / `$storage:` / `$saves:` and listed the server working directory (DZEXP-134).
  **Measured on this machine's Windows 1.30.164078 server** (only the server folder and its subfolders were listed; reading a
  known file by name was fine). **Officially fixed in 1.30.164200** [reported: BI notes; not re-measured here]. A mod that
  ports from 1.29 needs no workaround on 164200; a mod that carries a 164078 workaround (`CF.FindFileEx(…)` from CF 1.5.9+,
  passing `-1`, rewriting `$profile:` to a real path, an index file as fallback) must be re-tested: keep the workaround behind
  a runtime probe so it switches itself off, and check that "now visible" files (orphans, `.tmp`, `.bak`) are filtered.
- `GetMissionFolderPath()` returns `""` on 1.30 servers. Script side verified: `SetMissionPath` splits only on `\`
  (`P:\scripts\3_game\dayzgame.c:1207-1220`, unchanged from 1.29), so a forward-slash mission path from the engine yields an
  empty folder. Derive the folder from `GetMissionPath()`.
- `$storage:` is unusable until the first `MissionServer.OnUpdate` after `OnMissionStart`.
- Experimental builds are diag builds (`DIAG_DEVELOPER`, `BUILD_EXPERIMENTAL`, `DEVELOPER` defined); stable is not. Never use
  those as a "1.30" test.

## New in 1.30 that a port may want (pointers, not a tutorial)

`CGame.IsHeadless()` / `IsHeadlessOrDedicatedServer()` (headless roboclients exist); `CreateObject(…, bool objSpawner = false)`;
construction on any `EntityAI` (`GetConstructionBasic()`), map-structure rebuilding, code locks (`CodeLock`,
`CodeLockComponent`, `cfggameplay.json` ExternalLockData); motorbikes (`dayz-motorbikes`); sandstorms (`Weather.GetSandstorm()`,
`dayz-environment-hazards`); `array.Slice`, `DoOnce`, `typename.EnumFlagsToString` (`vector.Cross` came in 164078 and was
removed again in 164200 — use `a * b`); 164200 added `Transport.HasCrew()`, `ConstructionBase.IgnoreHouseCollisions()`,
`PPERequester_CameraNV.GetUndergroundExposureCoef()`. Signatures:
`references/api-additions-1.30.md` §by class and `api-delta` §11 (new classes).

## Limits of the scanner (say them when you report)

- It parses declarations, not expressions: a call through an unrelated variable is matched by **name**, so `call-removed`
  and `call-obsolete` are `medium` confidence; `override-*` findings are resolved through the real class chain (`high`).
- Overrides of methods that are neither 1.29 nor 1.30 vanilla are reported as `INFO override-unknown` — pass the dependency's
  source with `--deps` to resolve them.
- It does not see engine-side behaviour, `.agf` graphs, models, rvmats, or anything only a run shows.
