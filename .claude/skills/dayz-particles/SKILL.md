---
name: dayz-particles
description: "Use when: particles, .ptc, .emat, ParticleSource, SEffectManager, PlayOnObject, ParticleList, cfgeffectarea, smoke/sparks without Workbench, explosion particles (CfgAmmo particle, AmmoTypesAPI), SurfaceInfo particle IDs, sandstorm, ceiling dust, ParticleParamsOverrideData, VehicleVFX, IsHeadlessOrDedicatedServer, porting 1.29 particle code to DayZ 1.30. Client-only .ptc/.emat. Not sound: dayz-sound-system."
---

# DayZ Particle Effects — Complete Modding Reference

> Target: DayZ 1.30 stable (1.30.164200). Version labels: ../_shared/dayz-version-policy.md. Porting a ≤1.29 mod: dayz-130-migration skill.

## CRITICAL: .ptc AND .emat ARE PLAIN TEXT

DayZ particle files (.ptc) and material files (.emat) are plain text configs,
not binary. Custom particles can be created programmatically without Workbench.

| Layer | Format | Creatable? | Purpose |
|---|---|---|---|
| `.ptc` | Plain text config | YES | Particle effect (emitters, physics, curves) |
| `.emat` | Plain text config | YES | Material (texture ref, blend mode, color) |
| `.edds` | Standard DDS renamed | YES | Billboard sprite texture |
| `.meta` | Plain text (GUIDs) | Workbench only | NOT needed for mod runtime |

All particles are CLIENT-ONLY. Server controls state via SyncVars; client
creates/destroys particles in response.
The client gate is `!g_Game.IsHeadlessOrDedicatedServer()` (`P:\scripts\3_game\global\game.c:1136`):
`ParticleManager.GetInstance()` returns null on dedicated servers **and** headless roboclients
(`P:\scripts\3_game\particles\particlemanager\particlemanager.c:65`), and FireplaceBase uses the same gate
(`P:\scripts\4_world\entities\itembase\fireplacebase.c:1112`). Synced effecters are the exception: the server
creates an effecter object with `SEffectManager.CreateParticleServer` (trees, bushes, fishing, traps and, new in
1.30, player ceiling dust) and each client renders its particle. Details: `references/dayz-1-30-particles.md`.
Before 1.30: vanilla gated with `!g_Game.IsDedicatedServer()` (1.29: `wreck_mi8.c:6`, `fireplacebase.c:1096`).

## EFFECT-AREA PREFLIGHT (added 2026-08-31)

For `cfgeffectarea.json`, run **Effect areas: emitter budget and vertical band** plus its correction
below **before** choosing a radius. Compute the exact per-ring upper budget, keep it below 1,000 with
margin, measure the full elevation/underground band, and compare every global `SafePositions` entry
against every area.

---

## .ptc FILE FORMAT (verified from 9 vanilla files)

One EffectDef with one or more EmitorDef blocks. Each emitter is an independent
particle stream. Simplest particle (debug_dot): 1 emitter, 478 bytes. Complex
fire (fire_small_camp_01): 6 emitters (glow + flames + burst + short + sparks + smoke).
1.30 ships 314 `.ptc` under `P:\graphics\particles\`; 5 new ones spell the block `EmitterDef`
(`building_destruction_wood/brick/metal.ptc`, `explosion_grenade_sand.ptc`, `explosion_grenade_desert_sand.ptc`),
the rest use `EmitorDef` (one, `weapon_shot_flame_3d_4star.ptc`, is empty). Write `EmitorDef` in new files.

```
EffectDef {
 Emitors {
  EmitorDef <name> {
   ShapeType Point
   MaxNum 10
   BirthRate 5
   Material "{GUID}path/to/material.emat"
   SizeMultiplier 0.5
   Velocity 2
   LifeTime 1.5
   Color {
    0 1 0.8 0.3 1 0.7 0.5 0.25
   }
   Alpha {
    0 0 0.2 1 1 0
   }
   Repeat 1
  }
 }
}
```

### Property categories (56 total — full list in references/ptc-format-reference.md)

**Shape & emission:** ShapeType (Point/Box/Ellipse/Cone/Sphere/OnSphere — all six occur in
`P:\graphics\particles\**\*.ptc`), ShapeSize (x y z),
ConeAngle (yaw pitch roll deg), MaxNum, BirthRate, BirthRateRND, Offset (x y z)

**Visual:** Material ("{GUID}path.emat"), SizeMultiplier, SizeRND,
BillboardingType (Full), CenterY (-1=bottom, 0=center), StretchMultiplier

**Motion:** Velocity, VelocityRND, AirResistance, AirResistanceRND,
GravityMultiply, GravityMultiplyRND, Wind (0-1), Spring, Restitution,
VelAngle (0/1), VelAffect (0-1)
1.30: `EffectParticle.ParticleParamsOverride(notnull ParticleParamsOverrideData)` is an empty virtual hook
(`P:\scripts\3_game\effects\effectparticle.c:619`); subclasses implement it and call `SetParticleParam` /
`ScaleParticleParamFromOriginal` on the live particle (vehicle dust, wheel contact, exhaust). It is script, not a
`.ptc` key. See `references/dayz-1-30-particles.md`.

**Lifetime:** LifeTime (seconds), LifeTimeRND, EffectTime (cycle duration),
Repeat (0=one-shot auto-deletes, 1=loop MUST stop manually)

**Animation:** AnimOnce, AnimFPS, LifetimeByAnim, RandomFrame

**Rotation:** RandomRotDir, RotMultiplier, RandomAngle

**Curves (keyframe pairs: time value time value ...):**
Color { t r g b t r g b }, Alpha { t a t a },
Size { t s t s }, RotationSpeed { t s t s }

**Master curves (modulate over EffectTime):**
ColorMast, AlphaMast, SizeMast, BRateMast, VelocityMast, AirResistanceMast

---

## .emat MATERIAL FORMAT (verified from 8 vanilla materials)

### Shader: `Particle` (fire, sparks, water, most particles)
```
Particle {
 AlbedoMap "{GUID}Graphics/Particles/sprites/texture.edds"
 Color 0 0 0 1
 Emissive 1 1 1 0
 BlendMode Additive_AlphaModulate
 Sort translucent
 Softness 2
 TileU 4
 TileV 2
}
```

Particle materials use the `Particle` or `ParticleSprite` shader. 1.30 also ships weather **postprocess**
`.emat` files that are not particle materials: `P:\graphics\materials\postprocess\sandstorm.emat:1` is shader
`SnowEffect`, `rainnasdara.emat:1` is `RainEffect`. Never put them on a `.ptc` `Material` line. The sandstorm
`.ptc` itself uses ordinary `Particle` materials from `Graphics/Particles/materials/sandstorm/`.

### Shader: `ParticleSprite` (steam, flares, glow billboards)
```
ParticleSprite {
 AlbedoMap "{GUID}Graphics/Particles/sprites/texture.edds"
 Color 0.95 0.95 0.95 1
 Diffuse 0.95 0.95 0.95 1
 Ambient 0.59 0.59 0.59 1
 Softness 2
}
```

### All .emat properties (12):
AlbedoMap (texture GUID+path), Color (RGBA 0-1), Diffuse (RGBA),
Ambient (RGBA), Emissive (RGBA, self-glow), BlendMode (Additive_AlphaModulate
or numeric), Sort (translucent/overlay/0), Softness (0-2, edge blending),
TileU/TileV (sprite sheet cols/rows), CameraBlendFar (distance fade),
BidirLightScale (bidirectional lighting)

Full reference with examples: `references/emat-format-reference.md`

---

## GUID REFERENCE SYSTEM

Resources reference each other via `{HEXGUID}relative/path.ext`.

- **Vanilla→vanilla**: GUIDs embedded, engine resolves by GUID first
- **Mod .ptc→vanilla .emat**: USE vanilla GUID+path (mandatory, verified)
- **Mod .emat→vanilla .edds**: USE vanilla GUID+path (safe)
- **Mod .ptc→mod .emat**: path-only may work (needs in-game testing)
- **.meta files NOT needed** for mod runtime loading

---

## VANILLA MATERIAL LIBRARY (11 GUIDs mapped)

Reference these in custom .ptc to reuse vanilla visuals:

| GUID | Short name | Visual type |
|---|---|---|
| `{CB7AF4FD8ACBFDFC}` | glow/glow1.emat | Soft glow dot |
| `{009C2EBAACD2D72A}` | fire/sparks1.emat | Bright spark point |
| `{0B829A150C6A13E5}` | fire/sparks_06.emat | Spark strip (8-tile) |
| `{853257EDA4B1E35D}` | fire/fire_flame_01.emat | Fire flame billboard |
| `{3E6CE4D61F8AC71F}` | fire/fire_medium_camp_02.emat | Medium fire anim |
| `{83BCB6516091DB01}` | fire/fire_medium_camp_04.emat | Fire burst anim |
| `{62F13B8B540EBC80}` | fire/fireball_1.emat | Fireball |
| `{00BEA27443975BEB}` | smoke/smoke_anim_02.emat | Animated smoke puff |
| `{008E25684854DAB8}` | smoke/steam_small_cook_01.emat | Steam/vapor cloud |
| `{8FE2383D6913E098}` | enviroment/water_splash_01_NoEm.emat | Water splash |
| `{D29B53088A5A3911}` | smoke/smoke_dust_01_W.emat | White dust/mist |

All paths prefixed with `Graphics/Particles/materials/`.

---

## THREE SCRIPT API LAYERS

### 1. Legacy `Particle` (scripts/3_Game/Particles/Particle.c)
Creates `#particlesourceenf` child entity. EOnFrame lifetime tracking.
Auto-deletes non-looping particles. Used by FireplaceBase, torches, cooking.

### 2. `ParticleSource` extends Particle (ParticleSource.c)
Native C++ backed. No EOnFrame. Auto-destroy flags: ON_END, ON_STOP, ALL, NONE.
StopParticleFlags: `NONE = 0` (no-op; default `StopParticle()` already does gradual fade), IMMEDIATE, PAUSE (freeze visible).

### 3. `ParticleManager` pool (ParticleManager.c)
Global instance via `ParticleManager.GetInstance()` — 10000 pool (`particlemanager.c:25`), client only,
returns null when `g_Game.IsHeadlessOrDedicatedServer()` — dedicated servers and headless roboclients
(`P:\scripts\3_game\particles\particlemanager\particlemanager.c:65`). Pre-allocates+reuses ParticleSource objects.
Before 1.30: it returned null only on `IsDedicatedServer()`, so an old mod gated with `!IsDedicatedServer()` that
does not null-check `GetInstance()` dereferences null on a 1.30 headless client.
Mods can also `new ParticleManager(settings)` to instantiate their own pools
(the test framework does this); GetInstance() is the global, not a hard singleton.

### Wrapper: `SEffectManager`
PowerGenerator pattern: `SEffectManager.PlayOnObject(effect, parent, pos, ori)`.
Cleanup: `SEffectManager.DestroyEffect(m_Smoke)`.
Synced spawn: `SEffectManager.CreateParticleServer(pos, EffecterParameters)` (`P:\scripts\3_game\effectmanager.c:586`;
existed in 1.29). 1.30 adds `SEffectManager.GetEffecterByID(int)` (`:649`) and the protected hook
`ParticleEffecter.GetEffectTypename()` (`:857`) so an effecter can spawn its own `EffectParticleGeneral` subclass —
used by player ceiling dust (`DustEffectCeilingHandlerSynced`, `P:\scripts\4_world\classes\dust\dusteffectceilinghandler.c:92`).

Full API signatures: `references/script-api-reference.md`

---

## API QUICK REFERENCE

```
// Static create+play
Particle p = Particle.PlayOnObject(ID, parent, localPos, localOri, forceWorldRot);
Particle p = Particle.PlayInWorld(ID, worldPos);
// ParticleManager.GetInstance() returns null on dedicated server AND headless client — always guard.
// Gate VFX code with !g_Game.IsHeadlessOrDedicatedServer() (Before 1.30: !g_Game.IsDedicatedServer()).
ParticleManager pm = ParticleManager.GetInstance();
ParticleSource ps;
if (pm) ps = pm.PlayOnObject(ID, parent, localPos);

// Instance control
p.PlayParticle(optionalNewId);
p.StopParticle(flags);  // StopParticleFlags: NONE, IMMEDIATE, PAUSE
p.Stop();               // legacy alias
p.IsParticlePlaying();
p.ResetParticle();      // ParticleSource only
p.RestartParticle();    // reset+play
p.DisableAutoDestroy(); // ParticleSource only

// Parameter tuning (-1 = all emitters)
p.SetParameter(emitter, EmitorParam.AIR_RESISTANCE, 3.0);
p.ScaleParticleParamFromOriginal(EmitorParam.SIZE, 0.5);

// Wiggle
p.SetWiggle(randomAngle, randomInterval);
p.StopWiggle();
```

---

## REGISTERING CUSTOM PARTICLES

### Mod file structure
```
LFPowerGrid/
  data/particles/
    lfpg_sprinkler_spray.ptc
    lfpg_overload_sparks.ptc
  data/particles/materials/         ← optional custom materials
    lfpg_blue_water.emat
  scripts/3_Game/
    LFPG_ParticleList.c
```

### Registration (3_Game layer)
```
modded class ParticleList
{
    static const int LFPG_SPRINKLER_SPRAY = RegisterParticle(
        "LFPowerGrid/data/particles/", "lfpg_sprinkler_spray");
    static const int LFPG_OVERLOAD_SPARKS = RegisterParticle(
        "LFPowerGrid/data/particles/", "lfpg_overload_sparks");
};
```

1.30 `RegisterParticle(root, file)` lowercases both arguments before storing the path, and keys the
name map by the last path segment only (`P:\scripts\3_game\particles\particlelist.c:455-462,485`);
`GetParticleID` / `GetParticleIDByName` lowercase their argument (`:543,554`). So `GetParticlePath(id)`
now returns a lowercase path, and a file name that matches another registered particle's file name
(any folder) logs `Duplicate particle filename` and `GetParticleIDByName` returns the first one — which
matters because explosion particles from CfgAmmo `particle=` are resolved by name (see 1.30 changes below).
Keep the `lfpg_`-style unique prefix. Before 1.30: case was kept and the name key was the `file` argument as passed.

### AddonBuilder filter (include .ptc .emat .edds)
```
*.emat;*.edds;*.meta;*.ptc;*.c;*.imageset;*.layout;*.ogg;*.json;*.xml;*.paa;*.rvmat
```

---

## LFPG INTEGRATION PATTERNS (summary)

Full code: `references/lfpg-integration-patterns.md`

**Pattern A** — FireplaceBase helpers: `LFPG_PlayParticle(out Particle, id, pos)` /
`LFPG_StopParticle(out Particle)` with null-safety and server guard.

**Pattern B** — SyncVar toggle: server sets state→SetSynchDirty, client
creates/destroys in LFPG_OnVarSync.

**Pattern C** — One-shot sparks: non-looping (Repeat 0) particles auto-delete,
no cleanup needed. Use for overload events.

**Pattern D** — SEffectManager wrapper: for combining particle+sound. Cleanup
ONLY via SEffectManager.DestroyEffect().

**Pattern E** — Runtime parameter tuning: SetParameter for AIR_RESISTANCE,
ScaleParticleParamFromOriginal for SIZE.
1.30: vanilla vehicle VFX put this tuning inside an `EffectParticle` subclass's `ParticleParamsOverride(ParticleParamsOverrideData)`,
driven by `VehicleVFXComponent` (`P:\scripts\4_world\entities\vehicles\components\vehiclevfxcomponent.c:195,218`).
Plain `SetParameter` after Play still works for your own particles.

**Cleanup contract**: EVERY device with looping particles MUST clean up in
EEDelete + LFPG_OnWiresCut + destructor.

---

## PITFALLS AND HARD RULES

1. **Particles are CLIENT-ONLY.** Wrap in `#ifndef SERVER` and gate at runtime with
   `!g_Game.IsHeadlessOrDedicatedServer()` so headless roboclients also skip VFX. Server-spawned effecters use
   `CreateParticleServer`, not PlayOnObject. Before 1.30: the runtime gate was `!IsDedicatedServer()`. Not every
   1.30 vanilla file switched (`wreck_chinook.c:6`, `wreck_mi24_tk.c:5`, `effectarea.c:206` still use
   `IsDedicatedServer`) — do not copy those.
2. **Looping particles (Repeat 1) NEVER end.** MUST Stop() + null ref.
3. **Stop() is gradual.** Use StopParticleFlags.IMMEDIATE for instant removal.
4. **ObjectDelete(particle)** valid for ParticleSource with DisableAutoDestroy().
5. **Render distance ~200m.** Engine limit, no workaround.
6. **Duplicate filenames** across mods: use unique prefixes (`lfpg_`).
7. **ParticleManager.GetInstance()** returns null on server and on headless clients
   (`particlemanager.c:65`). Always null-check.
   Guards 1 and 7 are complementary, not redundant: `#ifndef SERVER` (pitfall 1) is
   compile-time exclusion for whole client-only blocks; the null-check (this pitfall)
   is runtime safety for code compiled on both sides (a diag/SP client-server build has
   no SERVER define but still runs server-side). Side checks: `enforce-script-reference`
   hard rules 19-20.
8. **SEffectManager.DestroyEffect()** is the ONLY safe Effect cleanup.
9. **AIR_RESISTANCE** controls smoke rise. FireplaceBase adjusts for ceilings.
10. **force_world_rotation=true** for smoke/steam that should always rise up.
11. **.meta files NOT needed** for mod runtime.
12. **GUIDs mandatory** when referencing vanilla .emat/.edds from mod .ptc.
13. **.ptc is PLAIN TEXT.** Can be generated by Python scripts, no Workbench needed.
14. **EffectTime + Repeat** interact: EffectTime is the master curve cycle, Repeat
    loops the entire effect. One-shot (Repeat 0) plays once for EffectTime duration.

---

## VANILLA PARTICLE CATALOG — KEY CATEGORIES

Full catalog: `references/vanilla-particle-catalog.md` — all 304 `ParticleList` constants of 1.30.164078
(278 on 1.29; none removed or renamed, 26 added: tandoor fireplace, sand/desert-sand grenade explosions,
desert-sand and mud-brick impacts, ceiling dust, building destruction, soil/desert wheels, dust, oil-pit fire —
`P:\scripts\3_game\particles\particlelist.c`; listed in the catalog's 1.30 section).

### Electricity
`POWER_GENERATOR_SMOKE` (1 emitter, loop), `BARBED_WIRE_SPARKS` (1 emitter, one-shot)

### Water
`WATER_JET` (2 emitters, loop), `WATER_JET_WEAK` (2 emitters, loop),
`WATER_SPILLING`, `DROWNING_BUBBLES`

### Fire
`CAMP_SMALL_FIRE` (6 emitters), `CAMP_STOVE_FIRE`, `BONFIRE_FIRE`

### Smoke/Steam
`CAMP_SMALL_SMOKE` (1 emitter), `CAMP_STEAM_2END`, `EVAPORATION`, `SPOOKY_MIST`

### Environment
`ENV_SWARMING_FLIES`, `SMOKING_HELI_WRECK`, `HOTPSRING_WATERVAPOR`,
`GEYSER_NORMAL`, `GEYSER_STRONG`, `VOLCANO`
1.30: `OIL_FIRE1`/`OIL_FIRE2`, `DUST_GROUND`/`DUST_CLOUD`/`DUST_VEHICLE`/`DUST_VEHICLE_SOFT`, `CEILING_WOOD`.
The weather sandstorm `.ptc` (`P:\graphics\particles\sandstorm\sandstorm.ptc`) is not a `ParticleList` constant;
config `CfgWorlds/CAWorld/Weather/Sandstorm particlePath` points at it (`P:\DZ\data\config.cpp:1540`).

---

## REFERENCE FILES

- `references/standalone-particle-reference.md` — standalone predecessor reference: multi-emitter composition (PLANES_EX/IMPACT/BOOM), animated + Mast curves, CfgCloudlets conversion. [merged 2026-06-05]

- `references/vanilla-particle-catalog.md` — all 304 vanilla 1.30 particles by category (1.30 additions tagged)
- `references/lfpg-integration-patterns.md` — 5 LFPG code patterns
- `references/ptc-format-reference.md` — 56-property .ptc reference with vanilla examples
- `references/emat-format-reference.md` — .emat format, both shader types, all 12 properties
- `references/script-api-reference.md` — Particle/ParticleSource/ParticleManager signatures
- `references/answeroverflow-2026-05-17.md` — community snippets verified vs vanilla (PlayOnObject pattern with client guard)
- `references/dayz-1-30-particles.md` — DayZ 1.30 (164078): headless gate, explosion particles via AmmoTypesAPI/CfgAmmo, ParticleList registration changes, dust, VehicleVFX, ParticleParamsOverrideData, sandstorm, oil pits, SurfaceInfo IDs, porting notes

## Effect areas (`cfgeffectarea.json`): emitter budget and vertical band (added 2026-08-31)

Run these gates **before** choosing a large contaminated-area radius.

### Hard particle budget

Each effect area has a hard cap of 1,000 emitters: `const int PARTICLES_MAX = 1000`
(`P:\scripts\4_world\classes\contaminatedarea\effectarea.c:85`), enforced while
`SpawnParticles` fills the area (`:425`). Exceeding it does not shrink the damage radius. The area
can remain lethal beyond the visible particle core and logs
`Not enough particles in pool for EffectArea` (`:444`). A valid JSON and a booting server do not
prove that the perimeter is visible. (1.29 lines were `:84`, `:421`, `:440`; the budget code did not change.)

For the static `FillWithParticles` route, budget first with:

```text
N = ceil((Radius + OuterOffset - InnerPartDist/2) / InnerPartDist)
emitters ~= 6.283 * N^2 * (VerticalLayers + 1)
```

Vanilla computes `circumference = 2 * Math.PI2 * ringRadius` (`effectarea.c:390`) while
`Math.PI2` is already 6.28318530717958 (`P:\scripts\1_core\proto\enmath.c:13`). That quirk
uses approximately `4*pi*r`, twice the ordinary circumference. Budgeting with `2*pi*r` therefore
under-counts. Increasing `InnerPartDist` lowers the ring count `N`; it does not reduce the emitter
count within an existing ring.

For static areas, only `InnerPartDist` and `OuterOffset` feed this route
(`contaminatedarea.c:70`). `InnerRingCount`, `OuterPartDist`, and `OuterRingToggle` belong to
`PlaceParticles` and are inert here. Do not tune them to solve a static-area density problem.

### Damage and visibility are vertically bounded

The trigger is a finite cylinder, built by
`SetCollisionCylinderTwoWay(radius, -(NegHeight + c), PosHeight - c)` with
`c = (PosHeight - NegHeight) * 0.5` (`effectarea.c:499-501`). Particle placement is culled against
the same band after the height snap (`:414-417`, cull `:425`). Choose `PosHeight` and `NegHeight` from the
actual elevation range inside the radius, not from a small vanilla zone.

1.30 height snap: each emitter starts at the area centre's Y; `SpawnParticles` lowers it to
`g_Game.SurfaceRoadY(x, z)` only when it is **above** that surface (`effectarea.c:414-417`), so gas sits on
roads/bridge decks, and an emitter whose start height is already below the surface keeps that height (it is
not lifted to terrain). An area centred underground therefore places its emitters at centre height; whether
they are visible inside the bunker was not measured on 1.30. Static check: compare the centre Y with
`SurfaceRoadY` over the whole radius. `ContaminatedArea_Local` snaps unconditionally to `SurfaceRoadY` and
falls back to the centre height when that leaves the band (`contaminatedarea_local.c:48-60`, same logic in 1.29).
Before 1.30: `partPos[1] = g_Game.SurfaceY(...)` always (1.29: `effectarea.c:413`) — emitters were never below
terrain, so an underground area could be lethal while showing no gas.
Tick hook on 1.30 is `override void OnCEIterate(float currentTime, float elapsedTime)` (`effectarea.c:191`);
`OnCEUpdate` is an empty `[Obsolete]` stub (`P:\scripts\3_game\entities\entityai.c:4924-4925`) that nothing calls
[reported: bzed guide — 0 calls in 2 min on a Linux 1.30.164014 server].

### Safe positions and schema

`SafePositions` is global, not attached to one area. `GetClosestSafePos` searches the whole array in
2D (`P:\scripts\4_world\static\miscgameplayfunctions.c:1769,1788`) during the early-presence
rescue path (`:1763`; caller `P:\scripts\4_world\classes\playermodifiers\modifiers\conditions\areaexposure.c:44`;
1.29 lines were `:1698,1717,1692`). After any radius change, measure every safe
position against every area; a rescue point inside another cylinder can teleport a reconnecting
player from gas to gas.

Treat `JsonDataContaminatedArea` / `JsonDataAreaData` as the schema authority
(`P:\scripts\4_world\classes\contaminatedarea\jsondatacontaminatedarea.c:17-34`, unchanged from 1.29). Community
fields named `InnerRingRatio` and `OuterRingRatio` are not in that schema. Large-radius height values
remain `[VERIFY IN GAME]` until the real terrain and underground depth are measured.

## Correction: effect-area budget and pool diagnostics (added 2026-08-31)

This section supersedes the approximate `6.283 * N^2` budget and the RPT claim above. For the normal
large-area branch (`R >= 1.25 * s`), use the same rounded ring spacing and per-ring floors as
`FillWithParticles`:

```text
R = Radius + OuterOffset
s = max(InnerPartDist, 1)
N = ceil((R - s/2) / s)
d = (R - s/2) / N
XZ = 1 + sum(floor(2 * PI2 * d * k / s), k = 1..N)
budget_max = XZ * (VerticalLayers + 1)
```

`PI2` is `2*pi`, so the source's `2 * PI2 * ringRadius` remains the intentional doubled
circumference. The center emitter is the leading `1`. Small areas take separate one-ring/center
branches in `effectarea.c:361-375` (1.30); use those branches directly rather than this large-area formula.
As a check, `Radius=500`, `OuterOffset=20`, `InnerPartDist=70`, and `VerticalLayers=1` give
`N=7`, `d=69.286`, `XZ=346`, and `budget_max=692`.

Reaching `PARTICLES_MAX` truncates additional visual emitters in `SpawnParticles`
(`effectarea.c:412-431`, 1.30) but does **not** itself log `Not enough particles in pool`. That error is in
`InsertParticles` and means `ParticleManager` returned fewer objects than the count requested
(`:433-451`). Do not use the RPT message as the cap gate. Vertical culling can reduce the actual
count, but an approximation that under-counts cannot authorize a radius; calculate the upper budget
and confirm the visible perimeter in game.

## DayZ 1.30 changes

Static facts below were checked on 1.30.164078 (`P:\`). Full detail and evidence:
`references/dayz-1-30-particles.md`.

### What changes
- Client VFX gate is `!g_Game.IsHeadlessOrDedicatedServer()`; `ParticleManager.GetInstance()` is null on
  headless clients too (`particlemanager.c:65`).
- **Explosion particles moved to the ammo.** `DayZGame.ExplosionEffects` plays
  `AmmoTypesAPI.GetExplosiveEffectData(ammoType, surface).m_ParticleID` in world at the explosion position for
  every explosion (`P:\scripts\3_game\dayzgame.c:3623-3639`); `ExplosivesBase.OnExplosionEffects` no longer plays
  a particle (`P:\scripts\4_world\entities\explosivesbase.c:66-70`). Data source: CfgAmmo `particle = "<.ptc file
  name>"` resolved with `ParticleList.GetParticleIDByName` (`P:\scripts\3_game\global\ammotypes.c:108-112,121-125`),
  plus per-surface overrides `AmmoTypesAPI.AddExplosionParticleEffect(string ammo, string surface, int particleID,
  string soundSetName = "")` (`ammotypes.c:78`; vanilla list `:52-72`). `AmmoTypesAPI.GetExplosionParticleID` is gone.
- Synced ceiling dust for players: `SEffectManager.CreateParticleServer` +
  `ParticleEffecterParameters("CeilingDustEffecter", 2.0, id)` (`dusteffectceilinghandler.c:98`); animals and
  infected use the local handler.
- `EffectParticle` gained virtuals `SelectFromSurface(string)`, `SelectFromSurface(SurfaceInfo)`,
  `SetParticleState(int)`, `ParticleParamsOverride(notnull ParticleParamsOverrideData)` (`effectparticle.c:611-619`),
  `SetEffectSound(EffectSound)` (`:599`) and `DisableParticleInUI(bool)` (`:569`), which sets the new
  `ParticlePropertiesFlags.DISABLED_IN_UI` ("Not created in player review": no particle in the UI player
  preview; `particlesource.c:63`; vanilla user `EffCloudDust`, `effclouddust.c:5`).
- Vehicle VFX: `VehicleVFXComponent` (`CarScript.InitializeParticles`, `carscript.c:343-382`) drives
  `EffVehicleDust`, `EffWheelContact`, `EffWheelSmoke`, `EffExhaustSmoke`, `EffEngineSmoke`, `EffCoolantSteam`
  via `ParticleParamsOverrideData.m_FloatModifiers`.
- Surface particle IDs: `SurfaceInfo.GetStepParticleId` / `GetWheelParticleId` / `GetWheelContactParticleId` /
  `GetVehicleDustParticleId` / `GetCloudEffectParticleId` / `GetGroundEffectParticleId` / `GetCeilingDustParticleId`
  (`P:\scripts\3_game\surfaceinfo.c:47-70`); `Surface.GetStepsParticleID` / `GetWheelParticleID` are `[Obsolete]`
  wrappers (`P:\scripts\4_world\static\surface.c:64,70`).
- Effect areas: `OnCEIterate` replaces `OnCEUpdate`; `SpawnParticles` lowers emitters to `SurfaceRoadY`
  (`effectarea.c:191,414-417`). New `EEffectAreaType.OIL_PIT = 8` (`effectarea.c:15`).
- `ParticleList`: 26 new constants, none removed (304 total); `RegisterParticle` lowercases paths and keys
  names by the last path segment (`particlelist.c:455-462`).
- Bullet impacts: new `Hit_Desert_Sand` / `Hit_Mud_Brick` effect classes
  (`P:\scripts\3_game\effects\effectparticle\bulletimpactbase\hit_desert_sand.c:1`, `hit_mud_brick.c:1`);
  `ImpactMaterials.MUD_BRICK` is new (`P:\scripts\3_game\impacteffects.c:45`), `DESERT_SAND` already existed (`:44`).
- Weather sandstorm: postprocess `sandstorm.emat` (shader `SnowEffect`) plus
  `particlePath = "Graphics/Particles/sandstorm/sandstorm"` (`P:\DZ\data\config.cpp:1540`).

### Custom explosion particle on 1.30
1. Register the `.ptc` in a `modded class ParticleList` (unique file name — lookup is by file name).
2. On the explosive's CfgAmmo class: `particle = "my_blast";` (file name only, no folder, no `.ptc`).
3. Optional surface variants/sound: call `AmmoTypesAPI.AddExplosionParticleEffect("MyBlast_Ammo", "Hit_Snow",
   ParticleList.MY_BLAST_SNOW, "MySoundSet")` after `AmmoTypesAPI.Init()` (`P:\scripts\5_mission\mission\missionbase.c:40`)
   and only under `#ifndef NO_GUI` — the map is created only there (`ammotypes.c:47-50`). Where to hook this in a mod
   was not tested here.
Before 1.30: explosives set the ID in script (`SetParticleExplosion(ParticleList.X)` in the constructor;
surface variants via an `InitSpecificsExplosionEffectForSurface` override) and `ExplosivesBase.OnExplosionEffects`
played it with `PlayOnObject` at `m_ParticlePosition` (1.29: `explosivesbase.c:64-85`).

### What breaks (1.29 mods)
| Symptom | Severity | Change |
|---|---|---|
| Script null pointer in particle code on headless roboclients | MEDIUM | `!IsDedicatedServer()` passes on headless but `GetInstance()` is null there. Gate with `!IsHeadlessOrDedicatedServer()` and null-check. |
| `AmmoTypesAPI.GetExplosionParticleID` | compile error | Use `GetExplosiveEffectData(ammo, surface).m_ParticleID` (`ammotypes.c:90`). |
| Custom explosive shows no blast particle, or the one its CfgAmmo inherits from a vanilla parent | HIGH | `SetParticleExplosion`, `InitSpecificsExplosionEffectForSurface`, `AddExplosionEffectForSurface`, `GetParticleExplosionID` are `[Obsolete]` and nothing plays their ID (`explosivesbase.c:306-438`). Add CfgAmmo `particle=`. |
| Null pointer when a mod calls `AddExplosionEffectForSurface` | MEDIUM | The obsolete static maps (`explosivesbase.c:41,43`) are never allocated on 1.30. Remove the call. |
| Explosion offset/orientation ignored | LOW | `SetParticlePosition` / `SetParticleOrientation` still store values (`explosivesbase.c:313-331`) but the explosion particle no longer uses them (only smoke grenades still read `m_ParticlePosition`, `smokegrenadebase.c:76`). |
| Compile error reading `EffectParticle.m_ForceRotationRelativeToWorld` | compile error | Field removed; use `ForceParticleRotationRelativeToWorld(bool)` / `IsParticleRotationRelativeToWorld()` (`effectparticle.c:542-563`). |
| Own `EffectParticle` subclass declares `SetParticleState` / `SelectFromSurface` / `ParticleParamsOverride` | LOW | These now exist on the base; vanilla added `override` (`vehiclesmoke.c:3`, `wheelsmoke.c:19`, `boatwatereffects.c:292`). Add it too (error vs warning not tested). |
| Modded car tweaks `m_exhaustFx` / `m_engineFx` / `m_coolantFx` / `m_WheelSmokeFx` and nothing happens | MEDIUM | Still declared under `//! old particles` (`carscript.c:2776-2781`) but never assigned. Use `m_VFXComponent`. |
| `new EffExhaustSmoke()` (or other `EffVehicleSmoke`) plays nothing | LOW | The 1.29 constructor that called `SetParticleStateLight()` is gone; call `SetParticleState(id)` yourself. |
| `GetParticleIDByName("impacts/hit_wood_ent_01")` returns `NONE` | LOW | Name key is the last segment on 1.30: pass `"hit_wood_ent_01"`. |
| Wheel/step particle lookups warn as obsolete | LOW | Use `SurfaceInfo.GetByName(name).GetStepParticleId()` / `GetWheelParticleId()` (wrappers still work). |
| `EffWheelSmoke.SetSurface(string)` warns as obsolete | LOW | Use `SelectFromSurface(SurfaceInfo)`. |
| Custom `EffectArea` or CE-ticked item never ticks | HIGH | `OnCEUpdate` is no longer called [reported: bzed, Linux 1.30.164014]. Override `OnCEIterate(float currentTime, float elapsedTime)`. |

### 1.30 migration checklist
- [ ] Search the mod for `IsDedicatedServer()` on particle spawn; switch VFX to `IsHeadlessOrDedicatedServer()` and null-check `ParticleManager.GetInstance()`.
- [ ] Explosives: grep `SetParticleExplosion|InitSpecificsExplosionEffectForSurface|AddExplosionEffectForSurface|GetExplosionParticleID`; move to CfgAmmo `particle=` + `AmmoTypesAPI.AddExplosionParticleEffect`.
- [ ] Replace `Surface.GetStepsParticleID` / `GetWheelParticleID` with `SurfaceInfo.GetByName(...).GetStepParticleId()` / `GetWheelParticleId()`.
- [ ] Replace `EffWheelSmoke.SetSurface` with `SelectFromSurface`.
- [ ] `EffectParticle` subclasses: add `override` to `SetParticleState` / `SelectFromSurface` / `ParticleParamsOverride`; replace `m_ForceRotationRelativeToWorld`.
- [ ] Custom EffectArea / CE items: `OnCEIterate(float currentTime, float elapsedTime)`; re-check emitter heights against `SurfaceRoadY`.
- [ ] Vehicle dust/exhaust: register through `VehicleVFXComponent` and override `ParticleParamsOverride`; stop using `m_exhaustFx` & co.
- [ ] `GetParticleIDByName` callers: pass the bare file name.
- [ ] Do not copy `sandstorm.emat` as a particle material — shader is `SnowEffect`.

### DayZ 1.30.164200 changes (patch of 2026-10-21)
Static reading of `P:\scripts` 1.30.164200; nothing here was measured in game.
- **Server particle + sound in one effecter.** Fishing now spawns its synced splash with a custom effecter,
  `ParticleEffecterParameters("FishingSplashEffecter", EffecterBase.NOT_DEFINED_LIFESPAN, particleId)`
  (`P:\scripts\4_world\classes\useractionscomponent\actions\continuous\actionfishingnew.c:326,330`). The new classes
  (`:462-486`): `FishingSplashEffecter : ParticleEffecter` overrides `GetEffectTypename()` to return
  `FishingSplashEffect : EffectParticleGeneral`, whose constructor hooks `Event_OnEffectStarted` and plays
  `SEffectManager.PlaySound("Fishing_splash_SoundSet", m_Position).SetAutodestroy(true)`. Reusable recipe (sound on every
  client, no RPC): `references/dayz-1-30-particles.md` §Server particle with a sound. Before 1.30.164200 fishing used
  the plain `"ParticleEffecter"` (no sound).
- **Vehicle wheel VFX cooldown.** `RegisterEffectGroup(WHEEL_SMOKE | WHEEL_CONTACT, …, new VehicleVFXDataParams(pos, dir, 2.0))`
  in `CarScript` and `MotorbikeScript` (`carscript.c:381-382`, `motorbikescript.c:203-204`): the third argument is
  `m_CooldownSec`, the minimum seconds between two (re)starts of one wheel's effect (`vehiclevfxcomponent.c:82-93`,
  guard `VehicleVFXData.CanPlay()` `:43-50`). It went 5.0 → 2.0.
- No `ParticleList`, `.ptc` or `EffectParticle` API changed in this patch.

### 1.30 references in this skill
- [DayZ 1.30 particles](references/dayz-1-30-particles.md)
- [Script API 1.30](references/script-api-reference.md#dayz-130-surfaceinfo-vehicle-vfx-synced-spawn-explosions)
- [Vanilla catalog 1.30 constants](references/vanilla-particle-catalog.md#dayz-130-new-particlelist-constants)
- [PlayOnObject client gate](references/answeroverflow-2026-05-17.md)
