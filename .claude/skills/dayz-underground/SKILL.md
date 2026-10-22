---
name: dayz-underground
description: >
  Use when: terrain holes, terrain hole, SurfaceIsHole, CfgWorlds Holes, tiles[],
  holes.cfg, hole in the terrain, agujero o hueco en el terreno, underground area,
  zona subterránea, bunker, búnker, tunnel, túnel, trench, trinchera, cave,
  irrigation tunnel, cfgundergroundtriggers.json, UndergroundTrigger, breadcrumbs,
  EyeAccommodation, EUndergroundPresence, INPUT_UDT_UNDERGROUND_SYNC, underground
  trigger editor, disallowedTypesInUnderground, bunker broadcast,
  cfgbunkerbroadcast.json, Buldozer MarkUnderground or CopyTileCoord, GetUndergroundPresence,
  underground night vision, porting a 1.29 underground mod. DayZ 1.30 stable
  (1.30.164200). Not construction parts, rebuilding or code locks: dayz-basebuilding.
  Not the bunker-broadcast storage file: dayz-persistence. Not sandstorm or heat:
  dayz-environment-hazards.
---

# DayZ Underground (1.30)

Underground areas in DayZ 1.30: the hole in the terrain (*terrain holes*, new in 1.30),
the underground triggers that darken and change the environment, the player's
"underground" state, and what Badlands builds on top (radio-announced bunkers,
irrigation tunnel entrance).

> Target: DayZ 1.30 stable (1.30.164200). Version labels: ../_shared/dayz-version-policy.md. Porting a ≤1.29 mod: dayz-130-migration skill.

Script citations are `P:\scripts\…` (unpacked 1.30.164200), config/data citations
`P:\DZ\…` and `P:\bin\…`; `1.29: O:\…` points at the unpacked 1.29 client
(`O:\DayZ 1.29 Старый распакованный клиент\`). The skill was first written against
1.30 Exp 164014; every `source_verified` claim was re-opened on stable 164078
(2026-10-15). **Nothing in this skill has been tested in game yet**: §Verification
status states the level of each claim and §Before designing with holes, how to raise
it. Porting checklist for old underground code: §DayZ 1.30 changes.

## The model: three pieces

| Piece | What it is | Where defined | Who uses it |
|---|---|---|---|
| Hole | heightmap cells without terrain | `CfgWorlds >> <world> >> Holes` (map config) | the engine, upon loading the world |
| Geometry | tunnel or bunker placed below the surface | `.wrp` (Terrain Builder), Object Spawner or script | the engine |
| Triggers | boxes that darken, alter sound, and flag "underground" | `cfgundergroundtriggers.json` | the server creates them, the client applies the effects |

The hole only removes terrain: it does not create any space. What happens if something falls through a
hole without geometry underneath is not measured; design as if there were no ground. Without
triggers, it does not darken inside.

## Terrain holes

### Format

Among readable vanilla maps in 1.30, only Livonia has holes (Chernarus does not; the
Sakhal and Nasdara world configs ship encrypted as `worlds_sakhal.ebo` /
`worlds_nasdara.ebo`): seven cells in two groups, next to the two entrances of the
Dambog bunker. Before 1.30: the same map config has no `Holes`
(1.29: `O:\…\DZ\worlds\enoch\world\config.cpp`).

```cpp
// [EXACT][CLAIM-UG-HOLES-SCHEMA-130] P:\DZ\worlds\enoch\world\config.cpp:303-318 > CfgWorlds > Enoch (1.30.164078)
class Holes
{
	class Dambog            // one group = one subclass; the name is arbitrary
	{
		tiles[]=
		{
			{118,195},{118,196},{118,197},
			{94,180},{95,180},{94,181},{95,181}
		};
	};
};
```

- Each entry in `tiles[]` is `{x, z}`: heightmap cell indices, not meters.
- The engine validates the range: code reading `tiles` uses the messages
  `x (%d) out of range <0, %d)` and `y (%d) …` (strings present in the stable
  `DayZDiag_x64.exe` 1.30.164.078; details in `references/terrain-holes-evidence.md`).
- Each group is a subclass, so a mod could add its own without overriding Bohemia's.
  **Unverified**: that a mod config patch on
  `CfgWorlds >> ChernarusPlus` opens holes is the first hypothesis that must be measured.

### Unit: heightmap cell

In Livonia the grid is 2048 cells of 6.25 m (12,800 m). With that size, both
groups fall over the vanilla underground triggers of Dambog; with 5 m or 10 m neither
falls within range (calculations in the reference):

- `{118, 195–197}` → X 737.5–743.75, Z 1218.75–1237.5: main entrance.
- `{94–95, 180–181}` → 12.5 m square at X 587.5–600, Z 1125–1137.5: second entrance.

It is `cross_checked` by that fit: the cell size was not read in the header of the
`.wrp` (OPRW v32) and the other maps are not measured. Always calculate
`cell = floor(coordinate / cell_size)` with the size of your terrain.

### Script API

```c
// [EXACT][CLAIM-UG-SURFACEISHOLE-130] P:\scripts\3_game\global\game.c:1203-1204
//! Returns whether tile at provided world coordinates is a hole.
proto native bool		SurfaceIsHole(float x, float z);
```

- It is the only new holes API and is read-only: there is no way to open or close
  holes from script at runtime. Not present in 1.29 (`1.29: O:\…\scripts\3_game\global\game.c`).
- No vanilla 1.30 script calls it (grep of `P:\scripts`, 2026-10-15).
- Useful to avoid placing anything where there is no terrain: custom loot, spawn points,
  custom holograms.

### Where data lives

- `enoch.wrp` and `chernarusplus.wrp` go from OPRW v29 to v32 in 1.30 and both grow by 5%
  (sizes re-checked on `P:\DZ\worlds\<world>\world\*.wrp`, 164078); Chernarus has no
  `Holes`. That change is format-related, not hole-related: holes reside in the world
  config (`cross_checked`).
- `P:\scripts\4_world\classes\hologram.c` is identical in 1.29 and 1.30 (diff): no
  kit placement check is aware of holes.

## Authoring workflow

- Buldozer 1.30 brings four new inputs: `UABuldLinkCamToTerrain`,
  `UABuldCopyTileCoord`, `UABuldMarkUnderground`, and `UABuldRemoveUnderground`
  (`P:\bin\constants.xml:282-285`; absent from `1.29: O:\…\bin\`; all four strings are in
  the stable `DayZDiag_x64.exe` 1.30.164.078). Default keys in the mouse-keyboard presets
  (`P:\bin\preset_x1mousekey.xml:1058-1075`): `8` link/unlink camera to terrain,
  `LCtrl+T` copy tile coord, `LCtrl+U` mark underground, `LCtrl+B` remove underground.
- [CHANGELOG 1.30], Terrain Builder section: the Buldozer camera can be unlinked from
  terrain height to edit below the surface (key 8 — confirmed by the preset above), and
  each object has an "underground" flag preventing a static object below terrain from
  being hidden by occlusion when it should be visible.
- That flag belongs to Terrain Builder. No script API sets an "underground" flag by name.
  1.30 does add an Object-Spawner marker whose engine effect is **unverified** (it may or
  may not touch occlusion):
  - `ECE_OBJECT_SPAWNER = 256` (`P:\scripts\3_game\ce\centraleconomy.c:15`, new in 1.30),
    set by Object Spawner on class-name entries (`P:\scripts\3_game\objectspawner.c:48`);
  - a trailing `bool objSpawner = false` on `CGame.CreateObject`
    (`P:\scripts\3_game\global\game.c:691`) and `CreateStaticObjectUsingP3D` (`:680`).
    Old calls still compile; no vanilla script passes `objSpawner = true`.
  - Object Spawner `.p3d` entries are limited to `DZ\plants*`/`DZ\rocks*`
    (`objectspawner.c:3-8`), so bunker geometry goes through class names. On 1.30 those
    `.p3d` entries pass `true` as the 5th argument, which is `createLocal`, not
    `objSpawner` (`objectspawner.c:44`; 1.29 passed nothing); runtime effect not measured.
  - Whether spawner/script objects under terrain get occluded is unmeasured.
- DayZ Tools on this machine, updated 2026-10-15 (Steam build 25460346): `terrainBuilder.exe`
  contains the per-object flag ("Compute objects that are fully underground", "This file
  seems to be missing the underground flag on line %d. The importing will still happen,
  but please re-export…") and **no** hole strings. So the flag is a Terrain Builder
  object-import column (a missing column is a warning, not an error), while holes are not
  authored in Terrain Builder. The April 2026 Tools had neither (measured 2026-09-24).
- Third-party tool [reported: FTT guide]: Flynn's Terrain Tools (FTT, released 2026-09-19).
  - Marks cells and writes `holes.cfg` next to `layers.cfg`, where according to its guide
    Terrain Builder and Buldozer read it.
  - Adds the `#include` to the terrain's `config.cpp`.
  - Buldozer re-reads it on each alt-tab.
  - Matches the `\holes.cfg` string that the engine uses in its world-loading function
    (present in stable `DayZDiag_x64.exe` 1.30.164.078), but the exact path and reload
    behavior are not verified here.

## Underground triggers

- File: `$mission:cfgundergroundtriggers.json`; if not present,
  `dz/worlds/<world>/ce/cfgundergroundtriggers.json`
  (`P:\scripts\3_game\undergroundarealoader.c:105-125`). Only one file is read.
- Server creates boxes (`UndergroundTriggerCarrier`) in `MissionServer.OnInit`
  (`P:\scripts\5_mission\mission\missionserver.c:91`) and sends the entire JSON to each
  client upon connecting (`undergroundarealoader.c:174-177`; `missionserver.c:342,363`). A
  custom mission JSON works without the client having the file. The carrier is the
  networked entity; the real `UndergroundTrigger` is created with `ECE_LOCAL` from the
  carrier's `OnVariablesSynchronized`
  (`P:\scripts\4_world\entities\scriptedentities\triggers\undergroundtrigger.c:23-32,44-69`),
  i.e. on clients only: a dedicated server has no trigger to test players against.
- Trigger type (`undergroundtrigger.c:100-118`):
  - with `Breadcrumbs` it is `TRANSITIONING`; more than 32 only logs `ErrorEx` (`:102-105`)
    and the client handler ignores crumbs past index 32
    (`P:\scripts\4_world\classes\undergroundhandlerclient.c:105`);
  - without them, `EyeAccommodation == 1.0` yields `OUTER`;
  - any other value yields `INNER`.
- [EXACT][CLAIM-UG-TRIGGER-LIMIT-130] Cap of 4096 triggers per JSON: `m_TriggerIndex` is
  synchronized in `-1..4095` (`undergroundtrigger.c:10`). Before 1.30: `-1..255`
  (1.29: `O:\…\scripts\4_world\entities\scriptedentities\triggers\undergroundtrigger.c:10`).
  Nasdara's vanilla JSON alone has 1076 triggers (`P:\DZ\worlds\nasdara\ce\cfgundergroundtriggers.json`,
  from `nasdara\addons\worlds_nasdara_ce.pbo`), so a 1.29-sized index
  would not fit it.
- Triggers bound to an object (`CustomSpawn`): via `ParentNetworkId` for map
  objects, or via `Tag`. Both fields already existed in 1.29.
  - Code-supported pattern for a structure placed by Object Spawner: its spawner entry
    contains `"customString": "undergroundTriggerTag=TAG"` (only a code comment shows it,
    `P:\scripts\4_world\entities\building\underground\land_warheadstorage_bunker_facility.c:76`;
    no vanilla map JSON uses `Tag` — Sakhal uses `ParentNetworkId`).
  - The class, in `OnSpawnByObjectSpawner`, creates triggers whose `Tag` matches
    (`land_warheadstorage_bunker_facility.c:62-93`;
    `P:\scripts\3_game\objectspawner.c:63,107`).
  - That logic lives in Sakhal classes, not in a common base: a mod class
    must implement it.
- In-game editor, new in 1.30: `LCTRL+/`, only in DayZDiag (`#ifdef DIAG_DEVELOPER`,
  `P:\scripts\4_world\plugins\pluginbase\pluginundergroundtriggermanager.c:1`) and only
  offline (`EditorToggle` returns in multiplayer, `:358-361`). The Exp client was a diag
  build, so there it also opened in the normal client [reported: bzed]; on stable use DayZDiag.
  - Double-click creates a trigger; edits boxes and breadcrumbs
    (`P:\scripts\4_world\plugins\pluginbase\pluginkeybinding.c:55,109-112`).
  - Exports to `$mission:cfgundergroundtriggers.json` (`:821`). The
    `…json.backup-<UTC date>` copy is made **after** the save (`OnExport`, `:830`,
    `:915-923`), so it is a copy of the new file; the previous JSON is overwritten with no
    backup. Copy your JSON yourself before exporting.
  - Menu `MENU_UNDERGROUND_TRIGGER_EDITOR = 48` (`P:\scripts\3_game\constants.c:223`),
    `UndergroundTriggerEditorMenu` (`P:\scripts\5_mission\gui\undergroundtriggereditormenu.c`).
    More details in `dayz-mod-workflow`.
  - Before 1.30: no editor; DayZDiag only had Diag menu > Underground > "Enable debug"
    (`UndergroundDiag`, `DiagMenuIDs.UNDERGROUND_DEBUG`), both removed in 1.30 (§DayZ 1.30 changes).
- Full JSON schema and trigger recipe with `Tag`: `references/underground-triggers.md`.

## Underground presence

- `EUndergroundPresence`: `NONE`, `OUTER`, `TRANSITIONING`, `FULL`
  (`P:\scripts\4_world\classes\undergroundhandlerclient.c:1-7`). Calculated by the client
  from the trigger it is in (`:464-475`), via the new
  `UndergroundTrigger.TranslateTriggerTypeToPresence()` (`undergroundtrigger.c:190-203`).
- [EXACT][CLAIM-UG-PRESENCE-SYNC-130] The client sends it to the server.
  - `SetUnderground` sends `INPUT_UDT_UNDERGROUND_SYNC` (= 17,
    `P:\scripts\3_game\tools\component\_constants.c:20`) in multiplayer
    (`P:\scripts\4_world\entities\manbase\playerbase.c:2853-2867`).
  - The server accepts it in `OnInputUserDataProcess` with only a range check
    (`:6531`, `:6557-6567`).
  - Read it with `GetUndergroundPresence()` (`:2848-2851`, new in 1.30); the field
    `m_UndergroundPresence` (`:75`) is still public.
  - Before 1.30: `SetUnderground` only set the field (1.29: `O:\…\playerbase.c:2678-2681`),
    so the value only existed on the client and every server-side read saw `NONE`.
- Server-side readers on 1.30:
  - `CanPlaceItem` (`playerbase.c:2834-2846`, called from
    `P:\scripts\4_world\classes\hologram.c:438`): inside a trigger it blocks types from
    `disallowedTypesInUnderground`.
    - By default: `FenceKit`, `TerritoryFlagKit`, and `WatchtowerKit`
      (`P:\scripts\3_game\cfggameplaydatajson.c:229-232`).
    - Configured in `cfggameplay.json` > `BaseBuildingData` > `HologramData`
      (vanilla `P:\DZ\worlds\chernarusplus\ce\cfggameplay.json:77`).
  - Sandstorm exposure: `GetIntensityForPlayer` → `ApplyUndergroundModifier` treats
    `TRANSITIONING`/`FULL` as sheltered
    (`P:\scripts\4_world\systems\sandstorm\scriptedsandstormcontroller.c:180-197`),
    used by the `SandstormExposure` modifier
    (`P:\scripts\4_world\classes\playermodifiers\modifiers\conditions\sandstormexposure.c:103-116`).
- Client-side readers: night-vision variant (below), dust effects
  (`P:\scripts\4_world\classes\dust\dusteffectsmanager.c:164`).
- The value is decided by the client: the server does not recalculate it. Treat it as
  untrusted for anything that matters (a modified client can report any value in range).

### Night vision underground (new in 1.30)

- The camera now re-resolves the NV type from the environment in `UpdateCameraNV`
  (`P:\scripts\4_world\entities\manbase\dayzplayer\dayzplayercamera_base.c:419-438`):
  `ResolveNVType` (`:664-724`) maps a device that is on (`NV_OPTICS_ON`, the Kazuar/Starlight
  `*_DAY`/`*_NIGHT` modes, `NV_GOGGLES`/`NV_GOGGLES_2D`, `NV_PUMPKIN`) to its `*_UNDERGROUND`,
  `*_SANDSTORM` or `*_UNDERGROUND_SANDSTORM` variant (`NVTypes` `:54-89`, condition variants `:69-86`)
  when `IsPlayerUnderground` (`:728-737`, `GetUndergroundPresence() != NONE`) or a sandstorm is
  active (`IsSandstormActiveForPlayer`, intensity > `SANDSTORM_NVG_THRESHOLD` = 0.15, `:741-755`).
- **Weapon optics keep their day/night mode underground.** A night mode (`NV_OPTICS_ON`,
  `NV_OPTICS_KAZUAR_NIGHT`, `NV_OPTICS_STARLIGHT_NIGHT`) resolves to `NV_OPTICS_UNDERGROUND` /
  `NV_OPTICS_UNDERGROUND_SANDSTORM`; a day mode (`OPTICS_STATE_DAY` → `NV_OPTICS_KAZUAR_DAY` /
  `NV_OPTICS_STARLIGHT_DAY`, `P:\scripts\4_world\entities\itembase\inventory_base\optics.c:74,147`)
  resolves to `NV_OPTICS_UNDERGROUND_DAY` / `NV_OPTICS_UNDERGROUND_SANDSTORM_DAY` (`:689-702`).
  "Day" is the optic's mode, not the time of day. Goggles and pumpkin have no day split underground.
- `SetCameraPP` passes the **resolved** type to `SetNVPostprocess` (`:441-463`); the ironsights and
  optics cameras do the same (`dayzplayercameraironsights.c:274`, `:305`, `:485`, `:530`, `:572`), except a
  1x non-NV optic used with NV goggles on, which still gets the raw type (`:538`). The PPE
  profiles are the `PPERequester_CameraNV` constants
  (`P:\scripts\3_game\ppemanager\requesters\ppercameranv.c:3-29`): `NV_UNDERGROUND_OPTICS = 6`,
  `NV_UNDERGROUND_SANDSTORM_OPTICS = 8`, `NV_UNDERGROUND_GLASSES = 10`, `NV_UNDERGROUND_PUMPKIN = 14`, …, and
  for optics in day mode `NV_UNDERGROUND_DAYTIME_OPTICS = 17` (`ApplyOpticsProfile(2.0, 1.0)`) and
  `NV_UNDERGROUND_SANDSTORM_DAYTIME_OPTICS = 18` (`ApplyOpticsProfile(1.0, 1.0)`) (`:28-29`, `:103-110`).
  `UndergroundHandlerClient` scales the underground exposure through
  `SetUndergroundExposureCoef` (`ppercameranv.c:146`, called at `undergroundhandlerclient.c:404`);
  `GetUndergroundExposureCoef()` (`ppercameranv.c:82`) reads it back.
- Do not store or send `NVTypes` as raw ints. 1.30.164200 inserted `NV_OPTICS_UNDERGROUND_DAY` and
  `NV_OPTICS_UNDERGROUND_SANDSTORM_DAY`, so every later value moved by +1/+2 (e.g. `NV_GOGGLES_DAY` 15 → 17)
  and `MAX` is now 25.
- Before 1.30.164200 (1.30.164078): any optic underground resolved to the night variant
  `NV_OPTICS_UNDERGROUND(_SANDSTORM)` whatever its mode, the ironsights camera passed the raw type on its
  second branch, and `NVTypes.MAX` was 23.
- Before 1.30: `NVTypes` ended at `NV_OPTICS_STARLIGHT_NIGHT`, `MAX` was 11, and
  `SetNVPostprocess` received the raw type (1.29: `O:\…\dayzplayercamera_base.c:442`).

## Badlands in 1.30 scripts

- **Radio-announced bunkers.**
  - Classes `Land_Bunker_Basement_*` and `Land_Bunker_Shelter_*` on `BroadcastedBunker`
    (`P:\scripts\4_world\entities\building\bunker.c:170,331-336`), with code lock (`:8`).
  - The radio broadcasts coordinates and code in Morse.
  - Configured in `cfgbunkerbroadcast.json`, in `$mission:` or in
    `dz/worlds/<world>/ce/` (`P:\scripts\3_game\cfgbunkerbroadcasthandler.c:5,23-27`), plus
    `cfggameplay.json` > `WorldsData` > `bunkerBroadcastEnabled`
    (`P:\scripts\3_game\cfggameplaydatajson.c:172,193`). When it is false, `LoadData`
    returns early (`cfgbunkerbroadcasthandler.c:15-16`). The vanilla Chernarus, Livonia and
    Sakhal `cfggameplay.json` omit the key (so `false`); Nasdara's sets it `true`
    (`worlds_nasdara_ce.pbo > cfggameplay.json`, stable install).
  - State file is covered by `dayz-persistence`.
- **Irrigation tunnel.** `Land_IrrigationTunnel_Entrance_01` is in
  `P:\scripts\4_world\entities\building\rebuildable\irrigationtunnel_entrance.c:1-11`: the
  entrance is rebuildable, and its debug spawn populates it with sticks and rope.
  Rebuilding is covered by `dayz-basebuilding`. Since 1.30.164200 its config carries
  `class Navmesh { generateWithRoadway=1; generateWithProxies=1; class HideParts {…
  hideAnimationSource="proxyroadway_block_01"; } }` and its ladder part sets
  `toggle_utility_part_update=1;` (`P:\DZ\structures_takistan\Industrial\config.cpp:20,25-35,461`).
  The tunnel water no longer gives cholera (§DayZ 1.30.164200 changes).
- Nasdara ships in the stable install under `<DayZ>\nasdara\addons\` (structures, CE,
  navmesh); its world config is `worlds_nasdara.ebo` (encrypted, so its `Holes` cannot be
  read). Since the 1.30.164200 unpack the rest is on `P:\`: `P:\DZ\worlds\nasdara\ce\` holds
  `cfgundergroundtriggers.json` (1076 triggers, each `CustomSpawn: 0`, no `Tag` or `ParentNetworkId`)
  and `cfgbunkerbroadcast.json`, plus `data\` and `navmesh\`. (On 1.30 Exp 164014 only Nasdara's
  scripts shipped.)
- Bohemia (Dev Blog Recap 2, Steam, 2026-09-22): Badlands underground facilities
  use the terrain holes system, which will be available for other maps and
  for the modding community.

## DayZ 1.30 changes (porting from ≤1.29)

What an old underground mod meets on 1.30. Full API delta: `dayz-130-migration`.

| 1.29 code | 1.30 | Breaks how |
|---|---|---|
| Own `INPUT_UDT_*` constant `= 17` | vanilla `INPUT_UDT_UNDERGROUND_SYNC = 17` (`P:\scripts\3_game\tools\component\_constants.c:20`), consumed in `PlayerBase.OnInputUserDataProcess` (`playerbase.c:6557-6567`) | compiles; a modded override that calls `super` first never sees its 17 (vanilla returns `true`) and the server reads your payload as presence. Pick another id. |
| `modded PlayerBase.SetUnderground` without `super` | vanilla `SetUnderground` now sends the sync (`playerbase.c:2853-2867`) | compiles; server presence stays `NONE`, so server-side readers (`CanPlaceItem`, sandstorm shelter) behave as if above ground |
| Reading `player.m_UndergroundPresence` on the server | now synced; use `GetUndergroundPresence()` (`playerbase.c:2848`) | behaviour change: server code that assumed `NONE` now sees real values |
| Mapping trigger type → presence inside `UndergroundHandlerClient.SetUndergroundPresence` | moved to `UndergroundTrigger.TranslateTriggerTypeToPresence()` (`undergroundtrigger.c:190-203`), still called from `undergroundhandlerclient.c:470` | an override of `SetUndergroundPresence` still runs; to change the mapping, override `TranslateTriggerTypeToPresence` |
| > 256 triggers in one JSON | cap 4096 (`undergroundtrigger.c:10`) | 1.29 synced the index only in `-1..255` (what happened above that was not measured); 1.30 syncs up to 4095 |
| `modded class UndergroundDiag`, `PluginDiagMenuClient.m_UndergroundDiag`, `DiagMenuIDs.UNDERGROUND_DEBUG` | removed; `undergrounddiag.c` now holds the editor gizmos (`BoxDraw(vector position, vector orientation = "0 0 0", vector size = "1 1 1")`, `P:\scripts\4_world\plugins\pluginbase\plugindiagmenu\undergrounddiag.c:276`) | compile error: the classes were `#ifdef DEVELOPER` (1.29: `O:\…\undergrounddiag.c:1`), so DayZDiag only; the `DiagMenuIDs` value is unguarded, so every build |
| `modded DayZPlayerCameraBase.SetNVPostprocess` switching on `NV_GOGGLES`/`NV_OPTICS_ON`… | receives the resolved `*_UNDERGROUND*`/`*_SANDSTORM*` type underground or in a sandstorm (`dayzplayercamera_base.c:441-463`) | compiles; your branch is silently skipped underground. Arrays sized by `NVTypes.MAX` grow from 11 to 25 (23 on 1.30.164078) |
| `modded UndergroundStash.PlaceOnGround()` | `[Obsolete("1.30: No replacement")]` (`P:\scripts\4_world\entities\undergroundstash.c:51`) and no longer called by `ActionDigInStash` (stash spawned with `ECE_PLACE_ON_SURFACE`, `P:\scripts\4_world\classes\useractionscomponent\actions\continuous\actiondiginstash.c:161`; 1.29 called it at `O:\…\actiondiginstash.c:165`) | compiles; the override is silently dead. (Buried stash, not this skill's trigger system.) |
| `CreateObject(type, pos, local, ai, physics)` / `CreateStaticObjectUsingP3D(p3d, pos, ori, scale, local)` | new trailing `bool objSpawner = false` (`P:\scripts\3_game\global\game.c:691`, `:680`) | existing calls compile unchanged |
| `cfgundergroundtriggers.json` from 1.29 | loads unchanged; 1.30 only adds optional `Comment` on triggers and breadcrumbs (`undergroundarealoader.c:48,71`) | no change needed |

Unchanged between 1.29 and 1.30 (diff): `hologram.c`, `objectspawner.c` apart from line 44/48
(see §Authoring workflow), `land_warheadstorage_bunker_facility.c`, `triggercarrierbase.c`,
`pperundergroundacco.c`, `undergroundbunkerhandlerclient.c`, `undergroundbunkertrigger.c`.
`Land_Underground_Entrance`/`_Panel`/`_WaterReservoir` and `Land_WarheadStorage_Main` only
gained `#ifndef NO_GUI` guards around client-only bodies.

### DayZ 1.30.164200 changes

What the 1.30.164200 patch (2026-10-21) changes for an underground mod written on 1.30.164078. Full
delta: `dayz-130-migration` (`api-delta-1.30.164078-to-164200.md`, `config-delta-1.30.164078-to-164200.md`).

- **NV types.** `NVTypes` gained `NV_OPTICS_UNDERGROUND_DAY` and `NV_OPTICS_UNDERGROUND_SANDSTORM_DAY`;
  later values moved by +1/+2 and `MAX` is 25. `ResolveNVType` keeps the optic's day/night mode
  underground; two new PPE profiles `NV_UNDERGROUND_DAYTIME_OPTICS = 17` and
  `NV_UNDERGROUND_SANDSTORM_DAYTIME_OPTICS = 18`. A `modded SetNVPostprocess` that switches on the
  underground optics types needs the two `*_DAY` cases. Detail: §Night vision underground.
- **Irrigation tunnel water.** [reported: BI 1.30.164200 notes] "Irrigation tunnel water gave character
  cholera" is fixed. The data change behind it (config-delta §2.8): 38 irrigation-tunnel and water-stream
  `.p3d` now use the roadway surface `clean_water_int` instead of `still_water_int`. In
  `P:\DZ\surfaces\config.cpp` `still_water_int` is `liquidType=512` (`LIQUID_WATER`, `:1775-1781`) and
  `clean_water_int` is `liquidType=4194304` (`LIQUID_CLEANWATER`, `:1803-1809`). Drinking a
  `LIQUID_CLEANWATER` surface counts as `ENVIRO_WELL`, anything else as `ENVIRO_POND`
  (`actiondrinkpondcontinuous.c:78-86`), and only `ENVIRO_POND` adds `CHOLERA` (`playerbase.c:7696-7704`).
  For water inside your own tunnel model, pick the surface material by the liquid you want.
- **Navmesh.** All four vanilla `navmesh.nm` (`P:\DZ\worlds\*\navmesh\`) are now format v9 (1.29: v6,
  1.30.164078: v7; header `TESM`, uint32 version at offset 4). The 164200 server compares the version
  strictly (`cmp r9d,9` before the `Navmesh could not be loaded: Invalid version` string, derived from exe
  analysis in config-delta §2.9, not run in game), so a custom map's v7 navmesh most likely no longer loads
  and its infected/animals stand still. Regenerate it with DayZ Tools 1.30.164200 (`dayz-ai-patterns`
  §Navmesh). [reported: BI 1.30.164200 notes] "Infected and animals had numerous issues navigating
  rebuildable structures" is fixed; config-wise the tunnel entrance and the `Fence` gate got `Navmesh`
  blocks (above; `P:\DZ\gear\camping\config.cpp:4418`).
- **Known issue** [reported: BI 1.30.164200 notes]: "Incorrect interior/exterior detection in some
  buildings". The note does not say which test; the trigger-driven `EUndergroundPresence` is script and did
  not change, so the likely candidates are the engine's inside-building checks (e.g. `IsSoundInsideBuilding`,
  used by the sandstorm shelter, `dayz-environment-hazards`) [inference, not measured].
- No underground trigger, breadcrumb, `UndergroundHandlerClient` or bunker-broadcast script changed in
  164200 (script delta file list).

## Verification status

| Claim | Level |
|---|---|
| Format `Holes`/`tiles[]`, `SurfaceIsHole`, presence synchronization, trigger cap, JSON schema, NV variants, editor export order | `source_verified` (re-opened on 1.30.164078; NV variants re-read on 1.30.164200) |
| One entry in `tiles[]` is a heightmap cell; 6.25 m in Livonia | `cross_checked` (fit with triggers) |
| Holes reside in config, not in `.wrp` | `cross_checked` (`.wrp` grows identically with and without holes) |
| Per-object "underground" flag is a Terrain Builder import column | `cross_checked` (strings in DayZ Tools of 2026-10-15) |
| How a hole renders and collides, what happens when falling through it, AI and navmesh | `unverified` |
| A config-only mod opens holes in a vanilla map | `unverified` (hypothesis) |
| `holes.cfg` next to `layers.cfg` and reload on alt-tab | `unverified` (third parties: FTT) |
| Effect of `ECE_OBJECT_SPAWNER` / `objSpawner`, and of `createLocal = true` on spawner `.p3d` entries | `unverified` |

## Before designing with holes

Each test advances one row of §Verification status. Together they fit in a single session of
DayZDiag 1.30 (`dayz-test-ingame`, `dayz-mcp-verify`):

1. **Livonia vanilla.** `g_Game.SurfaceIsHole(740, 1225)` must return `true` (cell
   `{118,196}`) and `g_Game.SurfaceIsHole(1000, 1000)` `false`. One screenshot from outside and
   another from inside the hole.
2. **Config-only mod.** Add a custom group in `CfgWorlds >> ChernarusPlus >> Holes`
   and repeat the check on its cell.
3. **Mod geometry.** Place an object with Object Spawner under that hole and
   verify that it is visible from inside and outside, and that it collides.
4. **Falling.** Drop an object and enter with a character into a hole without geometry.

## Delegations

| Topic | Skill |
|---|---|
| Base building parts, reconstruction, locks | `dayz-basebuilding` |
| Bunker broadcast state file | `dayz-persistence` |
| Trigger editor within general mod workflow | `dayz-mod-workflow` |
| Buldozer inputs and Enforce 1.30 API | `enforce-script-reference` |
| Launch game and verify with screenshots | `dayz-test-ingame` + `dayz-mcp-verify` |
| Sandstorm and heat of Nasdara | `dayz-environment-hazards` |
| Whole-mod port 1.29 → 1.30 (API/config delta, scanner) | `dayz-130-migration` |

## References

- `references/terrain-holes-evidence.md`: new engine strings and their references in
  code, Livonia config diff, cell size calculations, `.wrp` v29→v32, and official
  and third-party sources.
- `references/underground-triggers.md`: full schema of `cfgundergroundtriggers.json`,
  trigger recipe with `Tag` and underground presence.

## WHAT THIS SKILL COULD NOT VERIFY

- Any in-game behavior: rendering, collision, occlusion, falling, AI, and navmesh.
- Cell size of any map other than Livonia, and Livonia's read from the
  header of `.wrp`.
- Sakhal and Nasdara: their world configs are encrypted `.ebo` files and could not be read
  (so whether they use `Holes` is unknown; Bohemia says Badlands does).
- Exactly where the engine looks for `holes.cfg` and whether a config-only mod opens holes.
- What the engine does with `ECE_OBJECT_SPAWNER` / `objSpawner`, and whether
  `createLocal = true` on Object Spawner `.p3d` entries (`objectspawner.c:44`) is intended.
