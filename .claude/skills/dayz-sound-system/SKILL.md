---
name: dayz-sound-system
description: "Use when: sonido DayZ, CfgSoundShaders, SoundSet, sound not playing, no sound on dedicated server, PlaySoundSet, StartItemSoundServer, ItemSoundHandler, RPC_ITEM_SOUND_PLAY, NoiseSystem, AIParams noise, bunker radio, HEAVY_BREATHING, GetEnvironmentNoiseReduction, IsHeadlessOrDedicatedServer, VehicleHornComponent, VegetationSounds. DayZ 1.30 (164078); porting 1.29 audio/noise code. Audio is client-only. Not particles: dayz-particles; not UI: dayz-ui-development."
---

# DayZ Sound System (Enforce Script + config)

> Target: DayZ 1.30 stable (1.30.164078). Version labels: ../_shared/dayz-version-policy.md. Porting a ≤1.29 mod: dayz-130-migration skill.

Verified reference for mod audio. Vanilla citations are `path:line` re-checked against DayZ 1.30.164078
(short paths are relative to `P:\scripts\`; config paths to `P:\DZ\`) unless written `1.29:`. Two real
mods are used as prior art (IMPWMODPart2 weapons, DoorLockSystem; their citations are unchanged). The
deep-dive with every signature lives in `references/sonido-deep-dive.md` — load it for AbstractWave
events, DynamicMusicPlayer internals, or PlayerSoundManager handlers. `[EXACT]` 1.30 source blocks:
`references/dayz-1-30-sound.md`.

## The one rule that explains most bugs

**Audio is client-only.** `SEffectManager.Init()` (sound maps) is called only when `!IsDedicatedServer()`
(`3_game/global/game.c:56-69`; the dedicated server gets `InitServer()` = particle effecters only,
`3_game/effectmanager.c:507-521`) and `Object.PlaySoundSet` is guarded by
`if ( g_Game && !g_Game.IsDedicatedServer() )` (`3_game/entities/object.c:1249`). Any `Play*` call on a
dedicated server is a silent no-op. To make a sound happen "from the server", you send an event or
synchronize a state change and let each client play it: the official vehicle for item sounds is
`StartItemSoundServer` (below — an RPC on 1.30), or your own SyncVar + `OnVariablesSynchronized`.
1.30: `CGame.IsHeadlessOrDedicatedServer()` is new (`3_game/global/game.c:1132-1136`, next to the new
`IsHeadless()` at `:1130`); several sound paths use it so headless clients are skipped too —
`ItemSoundHandler.StopItemSoundClient` (`4_world/classes/soundhandlers/itemsoundhandler.c:121`),
`PlayerSoundManagerClient` (`4_world/classes/soundhandlers/playersoundmanager.c:105`),
`4_world/entities/manbase/dayzplayer/dayzplayercfgsounds.c:335`, `SEffectManager.Cleanup`
(`3_game/effectmanager.c:536`). `Object.PlaySoundSet`/`StopSoundSet` did **not** migrate (still
`IsDedicatedServer()`, `object.c:1249,1316`). Before 1.30: only `IsDedicatedServer()` existed.

## Decision table — which API when

| Situation | Use |
|---|---|
| One-shot 3D sound at a position (client code) | `SEffectManager.PlaySound("Set", pos)` (`3_game/effectmanager.c:178`) |
| Sound that follows an object (client code) | `SEffectManager.PlaySoundOnObject("Set", obj)` (`effectmanager.c:256`) or `obj.PlaySoundSet(snd, "Set", fadeIn, fadeOut, loop)` (`3_game/entities/object.c:1247-1325`) |
| Server-driven item sound, sent to all clients | `StartItemSoundServer(id)` / `StopItemSoundServer(id)` (`4_world/entities/itembase.c:4436-4503`; 1.30: batched RPC, see below) |
| Persistent server state with a sound (horn, alarm on/off) | Own SyncVar + play in `OnVariablesSynchronized` — vanilla 1.30 example `VehicleHornComponent` (below) |
| Sound at a memory point of the model | `PlaySoundSetAtMemoryPoint(...)` (`object.c:1294`) |
| Animation-bound audio (footsteps, gestures) | AnimEvents in the anim config — engine plays them client-side automatically |
| Building door/alarm (legacy) | `obj.PlaySound(name, range, create_local = true)` / `PlaySoundLoop(...)` — script wrappers on `Object` over `g_Game.CreateSoundOnObject`, return `SoundOnVehicle` (`object.c:1234-1244`; prior art DoorLockSystem `ActionUnlockDLSDoor.c:63,73`) |
| Make AI/infected hear something | `g_Game.GetNoiseSystem().AddNoise/AddNoisePos/AddNoiseTarget` (`3_game/noise.c:1-14`) — server side |
| Ambient music zone | `DynamicMusicPlayer.RegisterDynamicLocation` (`3_game/systems/dynamicmusicplayer/dynamicmusicplayer.c:289`) |
| Per-world static music boxes | Subclass `DynamicMusicPlayerRegistry`, call `RegisterTrackLocationStaticMultiRectangle` (`dynamicmusicplayerregistry.c:224-244`); vanilla constructs the registry in `MissionBase.InitialiseWorldData` (`5_mission/mission/missionbase.c:107-152`). Mechanism predates 1.30; 1.30 adds `DynamicMusicPlayerRegistryNasdara` (`dynamicmusicplayerregistrynasdara.c:39-67`, wired `missionbase.c:130-133`) |
| 1.30: Radio tuned to bunker frequency | Set `SOUND_BUNKER_STATIC_NOISE` on the transmitter; `IsTunedToBunkerFrequency()` (`4_world/entities/itembase/transmitterbase.c:6,135-138`, proto on `ItemTransmitter` in `4_world/entities/core/inherited/inventoryitem.c:25`) |
| 1.30: Per-bush / static vegetation footsteps | `CfgNonAIVehicles <class> VegetationSounds` via `StaticObjectType` (`3_game/entities/staticobjecttype.c:11-50`); macro `CUSTOM_PLAYER_MOVEMENT_SOUNDSETS` (`P:\DZ\data\basicdefines.hpp:310-338`) |

Cleanup matters: every `SEffectManager.Play*` registers a ref; call `SEffectManager.DestroyEffect(eff)`
or set autodestroy, or you leak (`effectmanager.c:40-42`). 1.30: `SEffectManager.PlayOnObject`
no longer re-registers an Effect that is already registered (`effectmanager.c:70-84`).

## Config: CfgSoundShaders + CfgSoundSets

Files: `.ogg` (universal in mods) or `.wav`; referenced **without extension**, path relative to the
mod root (real example: `samples[] = {{"IMPWMODPart2\Weapons\Automatic\Sounds\Rifle1Mid", 1}}`).

```cpp
// config.cpp — minimal working pair (pattern from real mod, IMPWMODPart2 ...\Sounds\config.cpp:27-36)
class CfgSoundShaders
{
    class MyMod_Roll_SoundShader
    {
        samples[]    = {{"MyMod\Sounds\roll_loop", 1}};   // {path-no-extension, random weight}
        volume       = 1.0;
        range        = 40;                                 // meters
        rangeCurve[] = {{0, 1}, {20, 0.8}, {40, 0}};       // inline attenuation curve
        // or: rangeCurve = "closeShotCurve";              // reference a vanilla CfgSoundCurves name
    };
};
class CfgSoundSets
{
    class MyMod_Roll_SoundSet
    {
        soundShaders[] = {"MyMod_Roll_SoundShader"};
    };
};
```

Cleaner for variants: inherit a vanilla base via forward declaration (real example,
`IMPWMODPart2\Weapons\Automatic\F2000\Sounds\config.cpp:22-45`):
```cpp
class base_closeShot_SoundShader;                      // forward-declare vanilla base
class My_closeShot_SoundShader : base_closeShot_SoundShader
{
    samples[] = {{"MyMod\Sounds\my_close", 1}};
    volume    = 0.8;
};
class CfgSoundSets
{
    class Rifle_Shot_Base_SoundSet;                    // forward-declare vanilla base
    class My_Shot_SoundSet : Rifle_Shot_Base_SoundSet
    {
        soundShaders[] = {"My_closeShot_SoundShader", "My_midShot_SoundShader", "My_distShot_SoundShader"};
    };
};
```
Both vanilla bases above still exist on 1.30 (`P:\DZ\sounds\hpp\config.cpp:17117` `base_closeShot_SoundShader`
= `volume=1; range=3000; rangeCurve="closeShotAttenuationCurve"`; `:323687` `Rifle_Shot_Base_SoundSet`);
`closeShotCurve` is a `CfgSoundCurves` class (`:59`, `:930`). The only sound classes 1.30 removed are
`IZH43*` tail shaders, `Music_Menu_R2B_*` and `runProne_water_Zmb_Soundset` (config delta) — none is a base
this skill recommends.
Extra SoundSet properties, as set on vanilla bases (`P:\DZ\sounds\hpp\config.cpp:323687-323700`
`Rifle_Shot_Base_SoundSet`; `:318381-318389` `baseCharacter_SoundSet`): `volumeFactor`, `volumeCurve`
(a `CfgSoundCurves` name), `sound3DProcessingType` (a class in `CfgSound3DProcessors`, `:2587`),
`distanceFilter` (`CfgDistanceFilters`, `:2422`), `occlusionFactor`, `obstructionFactor`, `spatial`,
`doppler`, `loop`, `soundShadersLimit`, `soundObjectsLimit`. Inherit a vanilla set (e.g.
`baseCharacter_SoundSet` for item/character sounds) rather than hand-rolling them.

## Script API core (`3_game/sound.c`, `3_game/effects/effectsound.c`)

`3_game/sound.c` is byte-identical between 1.29 and 1.30, so the `sound.c` line numbers below hold on both.

- `SoundParams("SetName")` → validates/loads a SoundSet (:137-144).
- `SoundObjectBuilder(params)` → `AddEnvSoundVariables(pos)` / `AddVariable(name, val)` →
  `BuildSoundObject()` (:82-108).
- `SoundObject.SetParent(entity)` / `SetPosition(pos)` / `SetKind(WaveKind.X)` (:111-134).
- Play: `g_Game.GetSoundScene().Play3D(so, sob)` / `Play2D(...)` (:53-79; access `3_game/global/game.c:735`).
- `AbstractWave` handle: `Play/Stop/Loop(bool)/SetVolumeRelative(0..1)/SetFadeInFactor/
  SetFadeOutFactor/SetDoppler/GetLength/GetCurrPosition` + `GetEvents()` ScriptInvokers
  (Started/Stopped/Loaded/HeaderLoaded/Ended) (:146-230).
- `EffectSound` (high-level wrapper): `SetSoundSet/SetSoundLoop/SetSoundFadeIn/Out/SetSoundVolume/
  SoundPlay/SoundStop/IsSoundPlaying` + its own invokers — what `PlaySoundSet` returns.
  1.30: if `m_SoundFadeInDuration > 0`, `Event_OnSoundWaveStarted` calls `SetSoundVolume(0)` so the
  wave starts at volume 0 before the fade-in (`3_game/effects/effectsound.c:549-555`). Before 1.30:
  no zeroing — the first frame could play at full volume (1.29: `effectsound.c:549-556`).
- `WaveKind` enum: WAVEEFFECT, WAVEEFFECTEX, WAVESPEECH, WAVEMUSIC, WAVEENVIRONMENT, WAVEWEAPONS,
  WAVEATTALWAYS, WAVEUI... (:1-14) — controls mixer category (and thus user volume sliders).
- Global category volumes only: `AbstractSoundScene.SetSoundVolume(vol, time)/SetMusicVolume/...`
  (:64-75) — there is no per-instance "master volume" beyond `SetVolumeRelative`.

## Server→client item sound (the official pattern)

1. Register sets in `InitItemSounds()` (`4_world/entities/itembase.c:4416-4433`; vanilla overrides it
   as `override void InitItemSounds()`, e.g. `4_world/entities/itembase/combinationlock.c:808-819`):
```c
override void InitItemSounds()
{
    super.InitItemSounds();
    ItemSoundHandler handler = GetItemSoundHandler();
    handler.AddSound(SoundConstants.ITEM_DEPLOY, "MyMod_Deploy_SoundSet");
    SoundParameters params = new SoundParameters();
    params.m_Loop = true;
    handler.AddSound(SoundConstants.ITEM_DEPLOY_LOOP, "MyMod_Loop_SoundSet", params);
}
```
2. Fire from server: `StartItemSoundServer(SoundConstants.ITEM_DEPLOY)` (or `(id, slotId)`) /
   `StopItemSoundServer(SoundConstants.ITEM_DEPLOY_LOOP)` (`itembase.c:4436-4454`, `:4495-4503`).
3. Client side is automatic. 1.30 transport is an **RPC**, not sync vars:
   - `StartItemSoundServer` returns unless `GetGame().IsServer()` and `id > 0`, queues an `ItemSoundEvent`
     and flushes the queue once per frame (`CallLater(FlushItemSoundQueue)`) as ONE
     `ERPCs.RPC_ITEM_SOUND_PLAY` RPC (count + id/slot pairs) to all clients (`itembase.c:4436-4479`;
     `ItemSoundEvent` at `:9-19`; enum `3_game/enums/erpcs.c:114-115`).
   - `StopItemSoundServer` sends `RPC_ITEM_SOUND_STOP` immediately (`itembase.c:4495-4503`).
   - The client handles both in `ItemBase.OnRPC` → `ItemSoundHandler.PlayItemSoundClient/StopItemSoundClient`
     (`itembase.c:2796-2825`, inside `#ifndef SERVER` / `#ifndef NO_GUI`). **If your item overrides
     `OnRPC`, call `super.OnRPC(...)`** or its handler sounds never play.

Limits (1.30): several `StartItemSoundServer` calls in the same frame are batched into one RPC, so the
old "one play + one stop per sync" limit is gone (the header comment at
`4_world/classes/soundhandlers/itemsoundhandler.c:19-20` still describes the old sync-var limit and is
stale). The client ignores a play for an id it still tracks in `m_PlayingSounds` (`itemsoundhandler.c:67-68`).
Vanilla now uses ids 1-7, 20-35, 40-46, 50-51, 60-61 (`3_game/constants.c:423-464`) — pick custom ids
outside those. `ITEM_SOUNDS_MAX = 63` (`itembase.c:116`) only bounds the legacy sync vars
(`:208-213`); the RPC writes full ints. `[DESIGN]` keep ids ≤ 63 anyway so the legacy path stays usable.
An RPC is a one-off event: a client that streams the item in later does not hear a loop that is
already running — for a persistent loop keep a state SyncVar (next paragraph).
Also new in 1.30: `StartItemSoundClient(id, slotId)` plays locally on a client (`itembase.c:4456-4462`);
`CanPlayAttachSound()/CanPlayDetachSound()` gate the vanilla attach/detach sounds (`:1335,1346`, `:4853-4861`);
`ItemSoundHandler` falls back to the registered `ITEM_ATTACH/ITEM_DETACH` set when the slot has none
(`itemsoundhandler.c:94-109`). Not verified here: whether the RPC reaches the local client in offline
single-player (1.29 had an explicit `|| !g_Game.IsMultiplayer()` branch; 1.30 checks only `IsServer()`).

Before 1.30: `StartItemSoundServer` set the sync vars `m_SoundSyncPlay`/`m_SoundSyncSlotID` +
`SetSynchDirty()` and cleared them 100 ms later via `ClearStartItemSoundServer`; the client dispatched
them in `ItemBase.OnVariablesSynchronized` (1.29: `itembase.c:4468-4509`, `:3319-3332`), so only ONE
play + ONE stop per sync tick. Porting: calls to `StartItemSoundServer`/`StopItemSoundServer` compile and
work unchanged; but `ClearStartItemSoundServer()` is `[Obsolete("1.30: No replacement")]`
(`itembase.c:4944-4949`), vanilla no longer writes `m_SoundSyncPlay` (a mod that reads it in its own
`OnVariablesSynchronized` sees nothing), and the old behaviour survives only as
`LegacyStopItemSoundServer(id)` + calling `LegacyItemSoundSync()` from your `OnVariablesSynchronized`
override (`itembase.c:4481-4493`, `:3291-3310`). Vanilla migration example: `CombinationLock` registers
its sets in `InitItemSounds` and fires `StartItemSoundServer(ITEM_COMBINATION_LOCK_*)`
(`combinationlock.c:808-819`, `:625-643`); its old `SoundLockOpen/Close/ChangeNumber/ChangeDial` are
`[Obsolete("1.30: Use ItemSoundHandler instead")]` (`:822-845`).

For non-item state sounds, the generic pattern is unchanged: server sets a SyncVar → client plays in
`OnVariablesSynchronized` (remote clients never see one-frame server states otherwise). 1.30 vanilla
example: `VehicleHornComponent` — `CarScript` registers `RegisterNetSyncVariableInt("m_HornComponent.m_Mode", …)`
(`4_world/entities/vehicles/carscript.c:283`), the server sets it in `SetMode` (+`SetSynchDirty`,
`4_world/entities/vehicles/components/vehiclehorncomponent.c:41-53`), and `CarScript.OnVariablesSynchronized`
→ `HandleCarHornSound` → `HandleSound()` plays the registered set with `PlaySoundSet`
(`carscript.c:435-449`, `:1548-1551`; `vehiclehorncomponent.c:55-71`).

## NoiseSystem (AI hearing)

```c
// 3_game/noise.c:1-26 — server side
g_Game.GetNoiseSystem().AddNoise(entity, noiseParams, multiplier);      // at entity
g_Game.GetNoiseSystem().AddNoisePos(entity, pos, noiseParams, mult);    // at position
g_Game.GetNoiseSystem().AddNoiseTarget(pos, lifetimeSec, noiseParams);  // positional DECOY with duration
g_Game.GetNoiseSystem().GetEnvironmentNoiseReduction(pos);              // 1.30, read-only (see below)
NoiseParams np = new NoiseParams();
np.LoadFromPath("CfgVehicles SurvivorBase NoiseActionDefault");          // config path (vanilla actionopendoors.c:83)
// or np.Load("ZombieRoarNoise");                                       // a class name in CfgNoises (P:\DZ\AI\config.cpp:6903-6941)
```
1.30: weather dampening of what AI hears is **native**. It is tuned in config `class AIParams`
(`P:\DZ\data\aiconfigs\config.cpp:12-23`: `rainToNoiseMultiplier`, `seaToNoiseMultiplier` and the new
`snowfallToNoiseMultiplier`, `fogToNoiseMultiplier`, `windToNoiseMultiplier`, `sandstormToNoiseMultiplier`);
no script class reads `AIParams`. Vanilla passes no weather factor any more — player steps use only
`NoiseAIEvaluate.GetNoiseMultiplier(this)` (`4_world/entities/dayzplayerimplement.c:3472-3474`, `AddNoise`
helper `:3258-3262`), infected `AddNoise(this, sound_event.m_NoiseParams)` (`4_world/entities/creatures/infected/zombiebase.c:597-600`),
and the horn / trap-catch code says "weather dampening is now done engine-side"
(`vehiclehorncomponent.c:105-107`, `4_world/entities/itembase/trapspawnbase.c:648`).
The only script-visible value is `NoiseSystem.GetEnvironmentNoiseReduction(vector pos)` (`3_game/noise.c:12-13`):
an *absolute* noise-strength reduction (weather/sandstorm/sea) at `pos`, not a 0..1 multiplier; vanilla
reads it only for the debug presence HUD (`4_world/plugins/pluginbase/pluginpresencenotifier.c:153-159`).
**Do not multiply `AddNoise`/`AddNoiseTarget` by it or by any weather factor** — the engine already applies it.
Obsolete in 1.30 (no vanilla caller left in `P:\scripts`):
- `NoiseAIEvaluate.GetNoiseReduction(Weather)` → `[Obsolete("Use GetNoiseReductionEx instead!")]` (`4_world/static/sensesaievaluate.c:92-98`);
- `Weather.GetNoiseReductionByWeather()` → `[Obsolete("Use Weather.GetNoiseReductionByWeatherEx instead!")]` (`3_game/weather.c:454-467`);
- but the suggested `NoiseAIEvaluate.GetNoiseReductionEx(notnull Object)` (`sensesaievaluate.c:18-25`) and
  `Weather.GetNoiseReductionByWeatherEx(notnull Object)` (`weather.c:407-420`, rain/snowfall/sandstorm → `1 − max`)
  are themselves `[Obsolete("1.30: AI noise dampening is now done natively via AIParams config parameters. Kept for HUD and old mods.")]`.
  Use neither in new code; drop the factor.
- `MiscGameplayFunctions.GenerateAINoiseAtPosition(vector, float, NoiseParams)` →
  `[Obsolete("Use MiscGameplayFunctions.GenerateAINoiseAtPositionEx instead!")]` (`4_world/static/miscgameplayfunctions.c:2072-2083`);
  new `static void GenerateAINoiseAtPositionEx(notnull Object source, float lifeTime, NoiseParams noiseParams)`
  calls `AddNoiseTarget(source.GetPosition(), lifeTime, noiseParams)` on the server (`:1886-1896`; used by `flaresimulation.c:138`).

Before 1.30: vanilla multiplied noise by `NoiseAIEvaluate.GetNoiseReduction(g_Game.GetWeather())`
(1.29: `dayzplayerimplement.c:3344`, `zombiebase.c:594`, `miscgameplayfunctions.c:1760`). Porting: remove that
factor (it would now double-dampen), and know that a mod that *overrode* those functions to change AI hearing
has no effect any more, because nothing calls them — balance through `AIParams` (`[reported: bzed]` same
conclusion; the in-game AI reaction was not re-measured on this machine).
Weapon shots define
`class NoiseShoot { strength = 82; type = "shot"; }` in the weapon config (real example
`IMPWMODPart2\Weapons\Automatic\MCXSpear\config.cpp:73-77`). `AddNoiseTarget` is the tool for
distraction devices: infected investigate a position that has no entity.

## Environment sound controllers

`SetSoundControllerOverride(name, value, action)` / `MuteAllSoundControllers()` /
`ResetAllSoundControllers()` (`3_game/sound.c:38-48`). Controller names (from the proto docs): rain,
night, meadow, trees, hills, houses, windy, deadBody, sea, forest, altitudeGround, altitudeSea,
daytime, shooting, coast, waterDepth, overcast, fog, snowfall, caveSmall, caveBig.

## What does NOT exist / classic confabulations

| Confabulation | Reality |
|---|---|
| `SEffectManager` works on server | Client-only; `InitServer()` exists for particles, not sound (`3_game/global/game.c:56-69`, `effectmanager.c:507-521`) |
| `PlaySoundSet` audible on dedicated server | Guarded no-op (`object.c:1249`) — send an event / sync state instead |
| `GetGame().CreateSoundOnObject(...)` returns EffectSound | Returns `SoundOnVehicle` entity (`game.c:692`; class `3_game/entities/soundonvehicle.c:1-4`) — legacy, prefer SEffectManager |
| `DynamicMusicPlayer.PlayTrack("...")` | Private; public API is `SetCategory(...)` + `RegisterDynamicLocation` (`dynamicmusicplayer.c:511,272,289`) |
| `OnSoundEvent` override on ItemBase | Does not exist — use ItemSoundHandler or PlaySoundSet manually |
| Only one synced play per tick via StartItemSoundServer | Before 1.30 only. 1.30 batches all plays of a frame into one RPC (`itembase.c:4436-4479`) |
| Item sounds arrive in `OnVariablesSynchronized` / via `m_SoundSyncPlay` | Before 1.30 only. 1.30: `ItemBase.OnRPC` `RPC_ITEM_SOUND_PLAY/STOP` (`itembase.c:2796-2825`) |
| `GetNoiseReductionEx` / `GetNoiseReductionByWeatherEx` are the replacement | Both are `[Obsolete]` too (`sensesaievaluate.c:18`, `weather.c:407`); dampening is native (`AIParams`) |
| Multiply `AddNoise` by `GetEnvironmentNoiseReduction(pos)` | It is an absolute reduction the engine already applies (`noise.c:12-13`) — do not feed it back |
| Per-sound master volume API | Only `SetVolumeRelative(0..1)` per wave; shader `volume` is fixed config |
| samples[] with file extension | Engine infers `.ogg` — paths go extension-less (all prior-art configs) |
| `SurfaceInfo.GetSoundHitType()` / `SoundHitType` | `[CHANGELOG 1.30]` names them (T189758) but no such identifier exists anywhere in `P:\scripts` on 164078 — do not call |

1.30: the `PlaySoundSet` row is still true for `Object.PlaySoundSet` (`IsDedicatedServer()` guard). Other
sound paths additionally skip headless clients via `IsHeadlessOrDedicatedServer()` (see "The one rule").

## Recipes

**A. Local loop tied to movement (e.g. rolling object) — pure client**
```c
EffectSound m_RollSound;
void UpdateRollAudio()                          // call from client-side update
{
    bool moving = GetVelocity(this).Length() > 0.5;
    if (moving && !m_RollSound)
        PlaySoundSet(m_RollSound, "MyMod_Roll_SoundSet", 0.1, 0.5, true);   // loop
    else if (!moving && m_RollSound)
        StopSoundSet(m_RollSound);              // respects fade-out
}
```
Position is already replicated to clients, so no server involvement is needed for the audio itself.

**B. Server-triggered impact sound** — register a set in `InitItemSounds()`, call
`StartItemSoundServer(id)` from the server event (e.g. `EOnContact`), done. On 1.30 the clients get it
through `ItemBase.OnRPC` — keep `super.OnRPC` in any `OnRPC` override.

**C. Distraction noise (server)**
```c
NoiseParams np = new NoiseParams();
np.LoadFromPath("cfgWeapons Flaregun_Base NoiseFlare"); // vanilla: flaresimulation.c:55; config P:\DZ\weapons\pistols\flaregun\config.cpp:81-85
g_Game.GetNoiseSystem().AddNoiseTarget(pos, 30, np, 2.0);
```
1.30: do not multiply that `2.0` by a weather reduction you computed in script — dampening is native.
(For an entity source, `MiscGameplayFunctions.GenerateAINoiseAtPositionEx(obj, lifeTime, np)` does the
same at `obj.GetPosition()`, `miscgameplayfunctions.c:1886-1896`.)

**D. Music zone** — `g_Game.GetMission().GetDynamicMusicPlayer()` (`3_game/gameplay.c:752`) →
`RegisterDynamicLocation(this, DynamicMusicLocationTypes.X, radius)` on spawn,
`UnregisterDynamicLocation(this)` on delete (vanilla: `4_world/classes/contaminatedarea/contaminatedarea.c:21,29`);
track = SoundSet with `WaveKind.WAVEMUSIC`. A custom world's static/time music lives in a
`DynamicMusicPlayerRegistry` subclass constructed in `MissionBase.InitialiseWorldData`
(`missionbase.c:107-152`; unknown worlds get the plain `DynamicMusicPlayerRegistry`, `:147-150`).
This predates 1.30; 1.30 only adds `DynamicMusicPlayerRegistryNasdara` as another example.

## Cross-skill pointers

- `enforce-script-reference` — SyncVars/OnVariablesSynchronized mechanics, CfgSoundSets params table
  (Llama extraction section), config.cpp rules.
- `dayz-physics-engine` — contact events that typically drive impact sounds.
- `dayz-particles` — SEffectManager also owns particles; same lifecycle/leak rules.
- `dayz-ai-patterns` — how the eAI/infected consume NoiseSystem events.

## DayZ 1.30 changes

What changed from 1.29 to 1.30.164078, what breaks, and the porting checklist. Every item was re-checked
in `P:\scripts` / `P:\DZ`; the body above already states the 1.30 behaviour. `[EXACT]` blocks:
`references/dayz-1-30-sound.md`. Shared AI noise: skill `dayz-ai-patterns` (its 1.30 noise section).

**What changes**

- Item sounds: `StartItemSoundServer`/`StopItemSoundServer` now send `ERPCs.RPC_ITEM_SOUND_PLAY/STOP` (play batched per frame) handled in `ItemBase.OnRPC`; the sync vars are legacy (`itembase.c:4436-4503`, `:2796-2825`).
- Native AI-noise attenuation; `AIParams` adds snowfall/fog/wind/sandstorm multipliers (`P:\DZ\data\aiconfigs\config.cpp:16-20`); read-only `NoiseSystem.GetEnvironmentNoiseReduction(pos)` (`3_game/noise.c:12-13`); all script weather-reduction helpers and `GenerateAINoiseAtPosition` obsolete (`…Ex` replacement takes an `Object`).
- `IsHeadless()` / `IsHeadlessOrDedicatedServer()` (`game.c:1130,1136`) on several sound paths; `Object.PlaySoundSet` still uses `IsDedicatedServer()`.
- `EffectSound` zeros volume at wave start when fade-in is set (`effectsound.c:549-555`).
- `EPlayerSoundEventID.HEAVY_BREATHING` / `HeavyBreathEvent1` (anim 907); silicosis requests it (`4_world/classes/playermodifiers/modifiers/diseases/silicosis.c:105`).
- `DynamicMusicPlayerTimeOfDay` gains `Translate` (MORNING/NOON/AFTERNOON→DAY, EVENING→NIGHT) and `ToString`, and the player translates the current daytime with it (`dynamicmusicplayer.c:733,737`, `:1053-1096`); new `DynamicMusicPlayerRegistryNasdara` (the per-world registry mechanism itself predates 1.30).
- Transmitters: `SOUND_BUNKER_STATIC_NOISE` / `IsTunedToBunkerFrequency()`.
- `StaticObjectType` + `CUSTOM_PLAYER_MOVEMENT_SOUNDSETS` for vegetation step sounds.
- Vehicles: horn moved to `VehicleHornComponent` on `Transport` (`GetVehicleHornComponent()`, `3_game/vehicles/transport.c:842`); horn sets are registered with `m_HornComponent.RegisterSound(VehicleHornMode.SHORT/LONG, "…_SoundSet")` (e.g. `inheritedcars/civiliansedan.c:24-25`); `CarScript.GenerateCarHornAINoise` is gone; `CarHorn*ActionInput` → `VehicleHorn*ActionInput`; crash-sound members moved from `CarScript` to `Transport` (api delta). `CarWheel.m_AttachSound/m_DetachSound` are gone — `CarWheel` registers `ITEM_ATTACH/DETACH` and `CarWheel_Ruined` `ITEM_CARWHEEL_EXPLOSION_*` with the item sound handler (`4_world/entities/core/inherited/inventoryitem.c:255-256,374-375`).
- `CombinationLock` sounds go through `ItemSoundHandler` (new `SoundConstants.ITEM_COMBINATION_LOCK_*` 30-35, `ITEM_DIGITAL_LOCK_*` 40-46, `ITEM_CARWHEEL_EXPLOSION_*` 50-51, `ITEM_ARMY_FLASHLIGHT_*` 60-61, `constants.c:444-463`).
- `SurfaceInfo` exposes sound-set names: `GetStepSoundSetName`, `GetWheelSoundSetName`, `GetWheelContactSoundSetName`, `GetVehicleDustSoundSetName`, `GetCloudEffectSoundSetName`, `GetGroundEffectSoundSetName` (`3_game/surfaceinfo.c:50-67`).
- `ItemBase.m_LockSoundSet` field is gone; the value lives in `ItemBaseType.m_LockSoundSet` and `ItemBase.GetLockSoundSet()` still returns it (`itembase.c:3822-3825`, `itembasetype.c:49,97`).
- `[CHANGELOG 1.30]` `SoundHitType` / `GetSoundHitType()` (T189758): **not** present anywhere in `P:\scripts` on 164078 (`3_game/surfaceinfo.c` has no such member). Do not call them.

**What breaks for a 1.29 audio / stealth mod**

1. An item that overrides `OnRPC` without `super.OnRPC(...)` loses all its handler sounds; code that read `m_SoundSyncPlay` in `OnVariablesSynchronized` sees nothing; `ClearStartItemSoundServer()` is obsolete with no replacement.
2. `[Obsolete]` warnings on `Weather.GetNoiseReductionByWeather`, `NoiseAIEvaluate.GetNoiseReduction`, `MiscGameplayFunctions.GenerateAINoiseAtPosition` (the `…Ex` noise-reduction "replacements" are obsolete too).
3. Double-damp if you still pass a weather multiplier into `AddNoise`/`AddNoiseTarget`; overriding the weather-reduction helpers no longer changes what AI hears.
4. A car mod that only sets `m_CarHornShortSoundName` / `m_CarHornLongSoundName` gets no custom horn: those fields are marked deprecated and the 1.30 `HandleCarHornSound` only calls `m_HornComponent.HandleSound()`, which plays what was passed to `RegisterSound` (`carscript.c:1548-1551,2765-2766`; `vehiclehorncomponent.c:36-71`). A subclass of a vanilla car keeps the vanilla horn.
5. Custom radios that ignore bunker frequency never play `bunkerbroadcast_staticnoise_SoundSet`.
6. `~InfectedSoundEventBase` no longer exists (1.29: `infectedsoundeventbase.c:15-18`); the base `~SoundEventBase` now does `if (g_Game) Stop();` (`4_world/classes/soundevents/soundevents.c:12-16`). `[DESIGN]` If you add a destructor to a sound-event subclass, keep the same `g_Game` guard.
7. Looped sounds with fade-in now start at volume 0 — before 1.30 the first frame played at full volume, so levels can sound different.
8. Custom item sound ids in the new vanilla ranges (30-35, 40-46, 50-51, 60-61) clash with vanilla registrations if your item derives from `CombinationLock`, `CodeLock`, `CarWheel` or `CarWheel_Ruined` (`combinationlock.c:808-819`, `4_world/entities/itembase/codelock.c:551-557`, `4_world/entities/core/inherited/inventoryitem.c:255-256,374-375`) — move them to free ids. (60-61 are declared but no vanilla script uses them.)

**Porting checklist**

- [ ] Item sounds: keep `StartItemSoundServer`/`StopItemSoundServer`; make every `OnRPC` override call `super`; remove `ClearStartItemSoundServer` and any `m_SoundSyncPlay` reading (or call `LegacyItemSoundSync()` deliberately).
- [ ] AI noise: delete script weather math around `AddNoise`; balance via `AIParams`; use `GetEnvironmentNoiseReduction(pos)` only to display/debug.
- [ ] `GenerateAINoiseAtPosition(pos, …)` → `GenerateAINoiseAtPositionEx(sourceObject, …)` (or call `AddNoiseTarget(pos, …)` directly if you have no object).
- [ ] New client-only play/stop guards: `IsHeadlessOrDedicatedServer()`.
- [ ] Custom car: register the horn with `m_HornComponent.RegisterSound(VehicleHornMode.SHORT/LONG, …)` in the constructor. The horn's AI noise is loaded from `cfgVehicles <type> NoiseCarHorn` (`vehiclehorncomponent.c:86`), inherited from `CarScript` (`P:\DZ\vehicles\wheeled\config.cpp:431`) — override that class only to change horn loudness for AI.
- [ ] Custom transmitter: set `SOUND_BUNKER_STATIC_NOISE`; vanilla `SetNextFrequency` already calls `SetSynchDirty()` (`transmitterbase.c:45-49`).
- [ ] Custom world music: subclass `DynamicMusicPlayerRegistry`, register boxes with `RegisterTrackLocationStaticMultiRectangle`, construct it for your world in `MissionBase.InitialiseWorldData`.
- [ ] Custom bushes: `VegetationSounds` on the `CfgNonAIVehicles` class, or the `CUSTOM_PLAYER_MOVEMENT_SOUNDSETS` macro.
- [ ] Do not call `SurfaceInfo.GetSoundHitType`.
- [ ] Load `references/sonido-deep-dive.md` for player-event and NoiseSystem signature tables.
