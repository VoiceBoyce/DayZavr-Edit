# 02 — DayZ sound system (Enfusion/Enforce Script)
> First written against vanilla v1.24 scripts + real prior art (2026-06-06). Vanilla citations re-checked
> against DayZ 1.30 stable 1.30.164078 on 2026-10-15: `scripts\…` = `P:\scripts\…`, config = `P:\DZ\…`;
> `1.29:` marks a legacy cite into `O:\DayZ 1.29 Старый распакованный клиент\`. Prior-art mod cites are unchanged.
> Labels: `../../_shared/dayz-version-policy.md`. `[EXACT]` 1.30 blocks: `references/dayz-1-30-sound.md`.

---

## Resumen ejecutivo

DayZ Enfusion sound system has two cleanly separated layers:

1. **Config (no-script):** `CfgSoundShaders` + `CfgSoundSets` in `config.cpp`. Defines which audio file plays, at what volume, and how it attenuates with distance. Engine resolves everything automatically in 3D.
2. **Script API:** `EffectSound` / `SEffectManager` (client-only). Server CANNOT play sounds directly; it sends an event (1.30 item sounds: RPC, §4) or sets network variables that clients read in `OnVariablesSynchronized`.

For a rolling stone (LF_RollingStone): simplest approach is defining a SoundSet in config, and triggering it from client (`PlaySoundSet` / `SEffectManager.PlaySoundOnObject`) within already synchronized events (for example `OnVariablesSynchronized`, or upon receiving local contact).

---

## 1. Audio pipeline

### Formatos
- **Supported format:** `.ogg` (Vorbis) and `.wav`. In practice **all mods use `.ogg`**; IMP and armory files go as `.ogg` without extension in config paths (engine infers it).
- **WSS:** proprietary Bohemia compressed sound format; in DayZ Standalone mods use `.ogg` directly.
- Samples are referenced **without extension** in `CfgSoundShaders`, e.g.: `"IMPWMODPart2\Weapons\Automatic\F2000\Sounds\F2000_close"` — [IMPWMODPart2\Weapons\Automatic\F2000\Sounds\config.cpp:24].

### Where files live in a mod
```
MiMod/
├── Sounds/           ← archivos .ogg
├── config.cpp        ← CfgSoundShaders, CfgSoundSets
└── scripts/          ← lógica Enforce Script
```
Paths in `samples[]` are relative to mod root (same as model paths).

---

## 2. Config verificada

### CfgSoundShaders

Defines a sound "shader": which audio file(s), volume, range, and attenuation curve.

**Propiedades verificadas** [IMPWMODPart2\Weapons\Automatic\Sounds\config.cpp]:

| Property | Type | Description |
|---|---|---|
| `samples[]` | array of `{path, weight}` pairs | List of samples with randomization weight |
| `volume` | float | Base volume (0–1 or more) |
| `range` | float | Maximum distance in meters where audible |
| `rangeCurve[]` | array of `{distance, factor}` pairs | Distance attenuation curve |
| `rangeCurve` | string | Predefined curve name (e.g. `"closeShotCurve"`) |

**Real example** (shader without inheritance, with inline curve):
```cpp
class IMP_SoundShaderMid
{
    samples[] = {{"IMPWMODPart2\Weapons\Automatic\Sounds\Rifle1Mid", 1}};
    volume = 0.56234133;
    range = 1500;
    rangeCurve[] =
    {
        {0,   0.2},
        {50,  1},
        {300, 0},
        {1500,0}
    };
};
```
[IMPWMODPart2\Weapons\Automatic\Sounds\config.cpp:27-36]

**Example with vanilla base inheritance** (cleanest way for mods):
```cpp
class base_closeShot_SoundShader;   // forward-declare la base vanilla
class IMP_F2000_closeShot_SoundShader: base_closeShot_SoundShader
{
    samples[] = {{"IMPWMODPart2\Weapons\Automatic\F2000\Sounds\F2000_close", 1}};
    volume = 0.8;
};
```
[IMPWMODPart2\Weapons\Automatic\F2000\Sounds\config.cpp:22-26]

### CfgSoundSets

Groups shaders into a "set" that script references by string name.

**Minimum verified properties:**

| Property | Description |
|---|---|
| `soundShaders[]` | Array of CfgSoundShaders names to combine |

**Additional properties** (as set on vanilla bases; `Rifle_Shot_Base_SoundSet` at `P:\DZ\sounds\hpp\config.cpp:324357-324370`, `baseCharacter_SoundSet` at `:319051-319059`): `volumeFactor`, `volumeCurve`, `sound3DProcessingType`, `distanceFilter`, `occlusionFactor`, `obstructionFactor`, `spatial`, `doppler`, `loop`, `soundShadersLimit`, `soundObjectsLimit`. Both bases recommended here still exist on 1.30 (`base_closeShot_SoundShader` at `:17117`).

**Ejemplo real** (heredando de base vanilla):
```cpp
class CfgSoundSets
{
    class Rifle_Shot_Base_SoundSet;   // forward-declare base vanilla
    class IMP_F2000_Shot_SoundSet: Rifle_Shot_Base_SoundSet
    {
        soundShaders[] = {
            "IMP_F2000_closeShot_SoundShader",
            "IMP_F2000_midShot_SoundShader",
            "IMP_F2000_distShot_SoundShader"
        };
    };
};
```
[IMPWMODPart2\Weapons\Automatic\F2000\Sounds\config.cpp:39-45]

**Simple SoundSet without inheritance** (for items, alarms, etc.):
```cpp
class CfgSoundSets
{
    class LFRS_Roll_SoundSet
    {
        soundShaders[] = {"LFRS_Roll_SoundShader"};
    };
};
```
[UNVERIFIED — pattern deduced from prior art; one shader suffices for simple effects]

### CfgSoundCurves / CfgSound3DProcessingTypes

- `CfgSoundCurves`: defines volume/range curves by name (e.g. `closeShotCurve` at `P:\DZ\sounds\hpp\config.cpp:930`, `closeShotAttenuationCurve` at `:160`; root class at `:59`). Mods reference them by name.
- The 3D-processing classes live in **`CfgSound3DProcessors`** (`P:\DZ\sounds\hpp\config.cpp:2587`, e.g. `character3DProcessingType` at `:2602`), not `CfgSound3DProcessingTypes`; distance filters in `CfgDistanceFilters` (`:2422`). Same root classes on 1.29. What each processor parameter does was not checked.

---

## 3. Script API verificada

### Tipos core (scripts/3_game/sound.c)

`scripts\3_game\sound.c` is byte-identical on 1.29 and 1.30 — all `sound.c` line cites in this section hold on both.

```
enum WaveKind
{
    WAVEEFFECT, WAVEEFFECTEX, WAVESPEECH, WAVEMUSIC,
    WAVESPEECHEX, WAVEENVIRONMENT, WAVEENVIRONMENTEX,
    WAVEWEAPONS, WAVEWEAPONSEX, WAVEATTALWAYS, WAVEUI
}
```
[scripts\3_game\sound.c:1-14]

**`SoundParams`** — loads and validates a SoundSet by name:
```
class SoundParams
{
    void SoundParams(string name);
    proto native bool Load(string name);
    proto native bool IsValid();
    proto string GetName();
}
```
[scripts\3_game\sound.c:137-144]

**`SoundObjectBuilder`** — builds a SoundObject with environment variables:
```
class SoundObjectBuilder
{
    void SoundObjectBuilder(SoundParams soundParams);
    SoundObject BuildSoundObject();                        // llama g_Game.GetSoundScene().BuildSoundObject
    proto native void AddEnvSoundVariables(vector position);
    proto native void AddVariable(string name, float value);
}
```
[scripts\3_game\sound.c:82-108]

**`SoundObject`** — representa la fuente 3D:
```
class SoundObject
{
    proto native void SetParent(IEntity parent, int pivot = -1);
    proto native void SetPosition(vector position);
    proto native void SetKind(WaveKind kind);
    proto native void SetOcclusionObstruction(float occlusion, float obstruction);
}
```
[scripts\3_game\sound.c:111-134]

**`AbstractWave`** — handle to playing sound:
```
class AbstractWave
{
    proto void Play();
    proto void Stop();
    proto void Loop(bool setLoop);
    proto void SetVolumeRelative(float value);    // 0.0–1.0 relative to shader max
    proto void SetFadeInFactor(float volume);
    proto void SetFadeOutFactor(float volume);
    proto void SetDoppler(bool setDoppler);
    proto float GetLength();
    proto float GetCurrPosition();
    proto bool IsHeaderLoaded();
    AbstractWaveEvents GetEvents();               // accede a ScriptInvokers de eventos
}
```
[scripts\3_game\sound.c:155-230]

**`AbstractWaveEvents`** — lifecycle ScriptInvokers:
```
class AbstractWaveEvents
{
    ref ScriptInvoker Event_OnSoundWaveStarted;
    ref ScriptInvoker Event_OnSoundWaveStopped;
    ref ScriptInvoker Event_OnSoundWaveLoaded;
    ref ScriptInvoker Event_OnSoundWaveHeaderLoaded;
    ref ScriptInvoker Event_OnSoundWaveEnded;
}
```
[scripts\3_game\sound.c:146-153]

**`AbstractSoundScene`** — global sound engine access:
```
proto native AbstractWave Play2D(SoundObject soundObject, SoundObjectBuilder soundBuilder);
proto native AbstractWave Play3D(SoundObject soundObject, SoundObjectBuilder soundBuilder);
proto native float GetSoundVolume();
proto native void SetSoundVolume(float vol, float time);    // controla volumen GLOBAL
proto native float GetMusicVolume();
proto native void SetMusicVolume(float vol, float time);
// also: GetRadioVolume/SetRadioVolume, GetSpeechExVolume/SetSpeechExVolume
```
[scripts\3_game\sound.c:53-79]
Acceso: `g_Game.GetSoundScene()` [scripts\3_game\global\game.c:735]

### SEffectManager (scripts/3_game/effectmanager.c)

Static manager of Effects (sounds + particles). **Sound side only exists on client** — `CGame` calls `SEffectManager.Init()` (sound maps) only when `!IsDedicatedServer()`, the dedicated server gets `InitServer()` (particle effecters) [scripts\3_game\global\game.c:56-69; scripts\3_game\effectmanager.c:507-521].

**Verified sound methods:**

```
// Create + play at position
static EffectSound PlaySound(string sound_set, vector position,
    float play_fade_in = 0, float stop_fade_out = 0, bool loop = false);

// Create + play parented to an Object (follows object)
static EffectSound PlaySoundOnObject(string sound_set, Object parent_object,
    float play_fade_in = 0, float stop_fade_out = 0, bool loop = false);

// Create + play with pre-built SoundParams (more efficient if reused)
static EffectSound PlaySoundParams(notnull SoundParams params, vector position,
    float play_fade_in = 0, float stop_fade_out = 0, bool loop = false);

// With SoundParams cache (avoids recreating SoundParams object on each call)
static EffectSound PlaySoundCachedParams(string sound_set, vector position, ...);

// With environment variables (reverb, etc.)
static EffectSound PlaySoundEnviroment(string sound_set, vector position, ...);

// Cleanup seguro (respeta fade-out en curso)
static void DestroyEffect(Effect effect);
static bool DestroySound(EffectSound sound_effect);   // legacy alias
```
[scripts\3_game\effectmanager.c:178-265; `DestroyEffect` :280, `DestroySound` :441] (signatures unchanged from 1.29; 1.29 lines were 169-256)

**WARNING:** All `Play*` methods register Effect in SEffectManager (retains `ref`). If `DestroyEffect` or `SetAutodestroy(true)` is not called, a memory leak occurs. [scripts\3_game\effectmanager.c:40-42]

### EffectSound (scripts/3_game/effects/effectsound.c)

High-level wrapper over AbstractWave:

```
class EffectSound : Effect
{
    void SetSoundSet(string snd);
    void SetSoundLoop(bool loop);
    void SetSoundFadeIn(float fade_in);
    void SetSoundFadeOut(float fade_out);
    void SetSoundVolume(float volume);         // relativo 0-1
    void SetSoundMaxVolume(float volume);      // max para fade-in
    void SetSoundWaveKind(WaveKind wave_kind);
    void SetDoppler(bool setDoppler);
    void SetEnviromentVariables(bool setEnvVariables);
    bool SoundPlay();
    void SoundStop();                          // respects fade-out if configured
    bool IsSoundPlaying();
    float GetSoundWaveLength();

    // ScriptInvokers para eventos propios:
    ref ScriptInvoker Event_OnSoundWaveStarted;
    ref ScriptInvoker Event_OnSoundWaveEnded;
    ref ScriptInvoker Event_OnSoundFadeInStopped;
    ref ScriptInvoker Event_OnSoundFadeOutStarted;
}
```
[scripts\3_game\effects\effectsound.c — completo]

1.30: if `m_SoundFadeInDuration > 0`, `Event_OnSoundWaveStarted` calls `SetSoundVolume(0)` to prevent a one-frame spike at full volume [scripts\3_game\effects\effectsound.c:549-555]. Before 1.30: no zeroing (1.29: `effectsound.c:549-556`). `[EXACT]` in `references/dayz-1-30-sound.md`.

### Object.PlaySoundSet (scripts/3_game/entities/object.c)

Convenience method on any Object to play parented:

```
bool PlaySoundSet(out EffectSound sound, string sound_set, float fade_in, float fade_out, bool loop = false);
bool PlaySoundSetLoop(out EffectSound sound, string sound_set, float fade_in, float fade_out);
bool PlaySoundSetAtMemoryPoint(out EffectSound sound, string soundSet, string memoryPoint,
    bool looped = false, float play_fade_in = 0, float stop_fade_out = 0);
bool StopSoundSet(out EffectSound sound);
```
[scripts\3_game\entities\object.c:1247-1325] (1.29: 1243-1321)

**IMPORTANT:** `PlaySoundSet` and `StopSoundSet` have guard `if (g_Game && !g_Game.IsDedicatedServer())` — **play only on client**. [scripts\3_game\entities\object.c:1249, :1316]

1.30: other sound paths use the new `!g_Game.IsHeadlessOrDedicatedServer()` so headless clients skip them too — `ItemSoundHandler.StopItemSoundClient` [scripts\4_world\classes\soundhandlers\itemsoundhandler.c:121], `PlayerSoundManagerClient` [scripts\4_world\classes\soundhandlers\playersoundmanager.c:105], [scripts\4_world\entities\manbase\dayzplayer\dayzplayercfgsounds.c:335]. Proto [scripts\3_game\global\game.c:1132-1136]. `PlaySoundSet` on `Object` did **not** migrate. Before 1.30: only `IsDedicatedServer()` existed.

Also on `Object`: `SoundOnVehicle PlaySound(string sound_name, float range, bool create_local = true)` / `PlaySoundLoop(...)` — script wrappers over `g_Game.CreateSoundOnObject` [scripts\3_game\entities\object.c:1234-1244] (no `Building` class in `P:\scripts` overrides them, so a `building.PlaySound/PlaySoundLoop` call such as DoorLockSystem's resolves to these `Object` methods — not a native Building API).

### g_Game.CreateSoundOnObject

```
proto native SoundOnVehicle CreateSoundOnObject(Object source, string sound_name, float distance, bool looped, bool create_local = false);
proto native SoundWaveOnVehicle CreateSoundWaveOnObject(Object source, SoundObject soundObject, AbstractWave soundWave);
```
[scripts\3_game\global\game.c:692-693]

`SoundOnVehicle` is an Entity class (not EffectSound):
```
class SoundOnVehicle extends Entity
{
    proto native float GetSoundLength();
};
```
[scripts\3_game\entities\soundonvehicle.c:1-4]

This method exists and differs from SEffectManager — used internally for vehicles. For item mods, `SEffectManager.PlaySoundOnObject` is preferred.

### Script category volume control

`AbstractSoundScene` exposes setters for global volume by category (Sound, Music, Radio, SpeechEx, VOIP). **There is NO per-sound-instance API beyond `SetVolumeRelative` on AbstractWave or `SetSoundVolume` on EffectSound.** [scripts\3_game\sound.c:64-75]

---

## 4. Sounds on Items (ItemBase)

### ItemSoundHandler — server→client pattern

Official mechanism for triggering sounds from server and delivering them to clients:

1. **Declare IDs** in `SoundConstants` (or custom added ones):
   ```
   class SoundConstants
   {
       const int ITEM_PLACE        = 1;
       const int ITEM_DEPLOY_LOOP  = 2;
       const int ITEM_DEPLOY       = 3;
       const int ITEM_FOLD_LOOP    = 4;
       const int ITEM_FOLD         = 5;
       const int ITEM_ATTACH       = 6;
       const int ITEM_DETACH       = 7;
       // 20-29 barrel/tent/explosive/key; 1.30 adds 30-35 combination lock, 40-46 digital lock,
       // 50-51 car-wheel explosion, 60-61 army flashlight
   }
   ```
   [scripts\3_game\constants.c:422-464] — `ITEM_SOUNDS_MAX = 63` [scripts\4_world\entities\itembase.c:116] bounds only the legacy sync vars on 1.30 (the RPC writes full ints).

2. **Registrar soundsets** en `InitItemSounds()`:
   ```
   override void InitItemSounds()
   {
       super.InitItemSounds();
       ItemSoundHandler handler = GetItemSoundHandler();
       handler.AddSound(SoundConstants.ITEM_DEPLOY, "MiMod_Deploy_SoundSet");
       // para loop:
       SoundParameters params = new SoundParameters();
       params.m_Loop = true;
       handler.AddSound(SoundConstants.ITEM_DEPLOY_LOOP, "MiMod_DeployLoop_SoundSet", params);
   }
   ```
   [scripts\4_world\entities\itembase.c:4420-4437]

3. **Fire from the server**:
   ```
   StartItemSoundServer(SoundConstants.ITEM_DEPLOY);           // one-shot (or (id, slotId))
   StopItemSoundServer(SoundConstants.ITEM_DEPLOY_LOOP);       // para loop
   ```
   [scripts\4_world\entities\itembase.c:4440-4458, :4499-4507]
   1.30: `StartItemSoundServer` returns unless `GetGame().IsServer()` and `id > 0`, queues an `ItemSoundEvent` and flushes the queue once per frame as one `ERPCs.RPC_ITEM_SOUND_PLAY` RPC (count + id/slot pairs) [itembase.c:4440-4483]; `StopItemSoundServer` sends `RPC_ITEM_SOUND_STOP` at once [:4499-4507].

4. **Client reception** (1.30) via `ItemBase.OnRPC`:
   ```
   case ERPCs.RPC_ITEM_SOUND_PLAY:   // read count, then id/slot pairs
       m_ItemSoundHandler.PlayItemSoundClient(soundIdPlay, slotIdPlay);
   case ERPCs.RPC_ITEM_SOUND_STOP:
       m_ItemSoundHandler.StopItemSoundClient(soundIdStop);
   ```
   [scripts\4_world\entities\itembase.c:2800-2829], inside `#ifndef SERVER` / `#ifndef NO_GUI` [:2767-2768]. An item that overrides `OnRPC` must call `super.OnRPC(...)`.

**Before 1.30 (legacy):** `StartItemSoundServer` set the network variables `m_SoundSyncPlay`, `m_SoundSyncSlotID` (and `StopItemSoundServer` `m_SoundSyncStop`) + `SetSynchDirty()`, cleared 100 ms later via `CallLater(ClearStartItemSoundServer/ClearStopItemSoundServer)`; `ItemBase.OnVariablesSynchronized` dispatched them to `PlayItemSoundClient/StopItemSoundClient` (1.29: `itembase.c:4468-4509`, `:3319-3332`). So only one play and one stop could be in flight per sync. On 1.30 the vars are still registered [itembase.c:208-213] but vanilla no longer sets `m_SoundSyncPlay`; `ClearStartItemSoundServer` is `[Obsolete("1.30: No replacement")]` [:4948-4953]; old-style delivery needs `LegacyStopItemSoundServer(id)` + `LegacyItemSoundSync()` called from your `OnVariablesSynchronized` [:4485-4497, :3295-3314].

**Limitation (1.30):** plays queued in the same frame are batched into one RPC, so several sounds can start together. The header comment at [scripts\4_world\classes\soundhandlers\itemsoundhandler.c:19-20] still describes the old single-variable limit — stale. The client still ignores a play for an id it already tracks in `m_PlayingSounds` [itemsoundhandler.c:67-68]. An RPC is not replayed to clients that stream the item in later; a persistent loop needs a state SyncVar.

### PlaySoundSet directly on client

For objects already on client (e.g. local action feedback):
```
EffectSound m_RollSound;
PlaySoundSet(m_RollSound, "LFRS_Roll_SoundSet", 0, 0.5, true);  // loop con fade-out 0.5s
// ...
StopSoundSet(m_RollSound);
```
[scripts\3_game\entities\object.c:1247-1325]

---

## 5. Synchronization patterns (Multiplayer Gotcha)

**Audio IS client-only.** Server does not have `SEffectManager` or `Play*`. Patterns to synchronize:

| Pattern | Mechanism | Typical usage |
|---|---|---|
| `StartItemSoundServer` | 1.30: batched RPC (`RPC_ITEM_SOUND_PLAY/STOP`) handled in `ItemBase.OnRPC`. Before 1.30: network variable + OnVariablesSynchronized | Items: deploy, place, looped deploy, lock clicks |
| Own SyncVar + `OnVariablesSynchronized` | State replicated to every client, incl. late joiners | Persistent loops / on-off states (1.30 vanilla: `VehicleHornComponent`, `carscript.c:283`) |
| `PlaySoundSet` in `OnVariablesSynchronized` | Existing network variable | Sound upon item state change |
| AnimEvent (bind in anim config) | Engine processes automatically on client | Footsteps, player actions |
| Direct `SEffectManager.PlaySound*` | Only in `#ifndef SERVER` or explicit guard | Local effects, impacts |

**For LF_RollingStone:** rolling sound can be triggered on client upon detecting object motion (speed > threshold), as `OnVariablesSynchronized` already receives synchronized position. No server needed for audio.

---

## 6. Player sounds

### PlayerSoundManagerBase

```
const float SOUNDS_HEARING_DISTANCE = 50;    // constante global

enum eSoundHandlers { STAMINA, HUNGER, INJURY, THIRST, COUNT }

class PlayerSoundManagerBase
{
    ref SoundHandlerBase m_Handlers[MAX_HANDLERS_COUNT];
    void RegisterHandler(SoundHandlerBase handler);
    SoundHandlerBase GetHandler(eSoundHandlers id);
}
```
[scripts\4_world\classes\soundhandlers\playersoundmanager.c:2-55]

Handlers separados: `StaminaSoundHandler`, `HungerSoundHandler`, `InjurySoundHandler`, `ThirstSoundHandler`, `FreezingSoundHandler`. Cada uno extiende `SoundHandlerBase`.

### Player footsteps

Handled in `DayZPlayerImplement.OnStepEvent`:
- Reads `DayZPlayerType.GetStepSoundLookupTable()` → surface table → soundset
- Builds `SoundObjectBuilder` with environment variables
- Calls `g_Game.GetSoundScene().Play3D(so, sob)` directly
- **Executes only on client** (`#ifndef SERVER` / `#ifndef NO_GUI`) [scripts\4_world\entities\dayzplayerimplement.c:3269 `OnStepEvent`, client block from :3326]
- 1.30: when the player brushes a `WoodBase` plant with `HasPlayerCollisionSound()`, its class's own `VegetationSounds` (`StaticObjectType.GetVegetationSoundBuilderFromEventID`) is tried first, then the generic `DayZPlayerType.GetVegetationSounds()` [scripts\4_world\entities\dayzplayerimplement.c:3394-3425]; see §12.

### Archivos relevantes
- `scripts\4_world\entities\manbase\dayzplayer\dayzplayercfgsounds.c` — exists. It registers step/sound/voice events and, only when `!g_Game.IsHeadlessOrDedicatedServer()` (1.30), the particle event and the step/attachment sound lookup tables [:322-345, guard :335]; `SoundAttachment` events are skipped only on `IsDedicatedServer()` [:328].
- Voice/VON sounds: `AbstractSoundScene.SetSpeechExVolume` + WaveKind.WAVESPEECHEX

### HEAVY_BREATHING (1.30)

New `EPlayerSoundEventID.HEAVY_BREATHING` [scripts\4_world\classes\soundevents\playersoundevents\playersoundeventhandler.c:38], registered with `RegisterState(new HeavyBreathEvent1())` [:99]. Class `HeavyBreathEvent1` in [scripts\4_world\classes\soundevents\playersoundevents\events\heavybreathevents.c:21-27] with `m_SoundVoiceAnimEventClassID = 907`. Vanilla fires it from silicosis [scripts\4_world\classes\playermodifiers\modifiers\diseases\silicosis.c:105]. `[EXACT]`: `references/dayz-1-30-sound.md`.

---

## 7. Music: DynamicMusicPlayer

Complete ambient/contextual music system on client:

```
class DynamicMusicPlayer
{
    void SetCategory(DynamicMusicPlayerCategoryPlaybackData playbackData);
    void RegisterDynamicLocation(notnull Entity caller, int locationType, float locationSize);
    void UnregisterDynamicLocation(notnull Entity caller);
}

enum EDynamicMusicPlayerCategory
{
    NONE, MENU, CREDITS, TIME,
    LOCATION_STATIC, LOCATION_STATIC_PRIORITY, LOCATION_DYNAMIC
}
```
[scripts\3_game\systems\dynamicmusicplayer\dynamicmusicplayer.c:81-303]

**Playback interno:** usa `SoundObjectBuilder` + `g_Game.GetSoundScene().Play2D(soundObject, soundBuilder)` con `WaveKind.WAVEMUSIC`. [scripts\3_game\systems\dynamicmusicplayer\dynamicmusicplayer.c:511-538]

**Fade-out:** implemented via `AbstractWave.SetFadeOutFactor(volume)` on 0.2s tick. [scripts\3_game\systems\dynamicmusicplayer\dynamicmusicplayer.c:574-580]

**For mods:** a dynamic location can be registered (e.g. event zone) with `RegisterDynamicLocation`, playing `LOCATION_DYNAMIC` tracks when player enters. Track is a normal SoundSet with `WaveKind.WAVEMUSIC`.

`dynamicmusicplayer.c` is unchanged from 1.29 up to line 732, so the cites above hold on 1.30. Access: `g_Game.GetMission().GetDynamicMusicPlayer()` [scripts\3_game\gameplay.c:752; vanilla use scripts\4_world\classes\contaminatedarea\contaminatedarea.c:21,29]. Location types: `DynamicMusicLocationTypes.CONTAMINATED_ZONE = 0`, `UNDERGROUND = 1` [dynamicmusicplayer.c:4-9].

Each world instantiates its `DynamicMusicPlayerRegistry*` in `MissionBase.InitialiseWorldData` [scripts\5_mission\mission\missionbase.c:107-152] — this already existed on 1.29 for Chernarus/Enoch/Sakhal (1.29: `missionbase.c:113-137`). Static rectangular zones: `RegisterTrackLocationStaticMultiRectangle` [scripts\3_game\systems\dynamicmusicplayer\dynamicmusicplayerregistry.c:224-244]. 1.30 adds `DynamicMusicPlayerRegistryNasdara` [missionbase.c:130-133; calls in dynamicmusicplayerregistrynasdara.c:39-67; 15 DAY/DUSK/DAWN/NIGHT time tracks, not hourly, :71-93] and `DynamicMusicPlayerTimeOfDay.Translate` (MORNING/NOON/AFTERNOON→DAY, EVENING→NIGHT) plus `ToString` [dynamicmusicplayer.c:1053-1096], applied to the current daytime at [:733, :737]. Before 1.30: `DynamicMusicPlayerTimeOfDay` was an empty subclass of `WorldDataDaytime` (1.29: `dynamicmusicplayer.c:1053-1054`).

---

## 8. NoiseSystem

```
class NoiseSystem
{
    proto void AddNoise(EntityAI source_entity, NoiseParams noise_params,
        float external_strenght_multiplier = 1.0);
    proto void AddNoisePos(EntityAI source_entity, vector pos, NoiseParams noise_params,
        float external_strenght_multiplier = 1.0);
    proto void AddNoiseTarget(vector pos, float lifetime, NoiseParams noise_params,
        float external_strength_multiplier = 1.0);
    // 1.30: "Absolute noise-strength reduction the environment (weather/sandstorm/sea) applies at 'pos'. Single source of truth used by AI."
    proto native float GetEnvironmentNoiseReduction(vector pos);
}

class NoiseParams
{
    proto native void Load(string noise_name);        // loads from CfgNoises by name
    proto native void LoadFromPath(string noise_path); // full config path, e.g. "CfgVehicles SurvivorBase NoiseActionDefault"
}
```
[scripts\3_game\noise.c:1-26] (1.29: `noise.c:1-23`, no `GetEnvironmentNoiseReduction`). Vanilla `Load(name)`: `scripts\3_game\dayzanimevents.c:182`, `scripts\4_world\entities\itembase\trapspawnbase.c:646`; `LoadFromPath`: `scripts\4_world\classes\useractionscomponent\actions\interact\actionopendoors.c:83`, `scripts\4_world\entities\firearms\flaresimulation.c:55`. `CfgNoises` (by-name classes) is in `P:\DZ\AI\config.cpp:6903-6941` (DeerStepNoise, DeerRoarNoise, WolfStepNoise, WolfRoarNoise, ZombieStepNoise, ZombieRoarNoise).

Acceso: `g_Game.GetNoiseSystem()` [scripts\3_game\global\game.c:738]

### How it feeds AI

En `DayZPlayerImplement.AddNoise`:
```
void AddNoise(NoiseParams noisePar, float noiseMultiplier = 1.0)
{
    if (noisePar != null)
        g_Game.GetNoiseSystem().AddNoise(this, noisePar, noiseMultiplier);
}
```
[scripts\4_world\entities\dayzplayerimplement.c:3258-3262]

1.30: no weather factor is passed any more. Footsteps use only `NoiseAIEvaluate.GetNoiseMultiplier(this)` (speed × boots × surface) [scripts\4_world\entities\dayzplayerimplement.c:3472-3474; scripts\4_world\static\sensesaievaluate.c:5-16]; infected `AddNoise(this, sound_event.m_NoiseParams)` [scripts\4_world\entities\creatures\infected\zombiebase.c:597-600]. Weather/sea/sandstorm attenuation is applied by the engine, tuned in `class AIParams` [P:\DZ\data\aiconfigs\config.cpp:12-23; `snowfall/fog/wind/sandstormToNoiseMultiplier` new in 1.30]; `GetEnvironmentNoiseReduction(pos)` is the read-only view [noise.c:12-13], used by vanilla only in the presence-debug HUD [scripts\4_world\plugins\pluginbase\pluginpresencenotifier.c:153-159, comment :229]. It is an absolute reduction, not a 0..1 multiplier — never pass it (or any weather factor) as `external_strenght_multiplier`; you would dampen twice.
Obsolete on 1.30, no vanilla caller left: `NoiseAIEvaluate.GetNoiseReduction(Weather)` [sensesaievaluate.c:92] → its text says "Use GetNoiseReductionEx", and `Weather.GetNoiseReductionByWeather()` [scripts\3_game\weather.c:454] → "Use Weather.GetNoiseReductionByWeatherEx"; but `GetNoiseReductionEx(notnull Object)` [sensesaievaluate.c:18-25] and `GetNoiseReductionByWeatherEx(notnull Object)` [weather.c:407-420] are themselves `[Obsolete("1.30: AI noise dampening is now done natively via AIParams config parameters. Kept for HUD and old mods.")]`. Drop the factor instead of switching to `Ex`.
Before 1.30: the multiplier was reduced with rain/snowfall via `NoiseAIEvaluate.GetNoiseReduction(g_Game.GetWeather())` (1.29: `dayzplayerimplement.c:3344`, `:2497-2522`, `zombiebase.c:594`).
Footstep noise params come from `DayZPlayerType.GetNoiseParamsStand/Crouch/Prone()` by stance [scripts\3_game\dayzplayer.c:366-380; chosen at dayzplayerimplement.c:3459-3470]; landings use `GetNoiseParamsLandLight/LandHeavy()` [dayzplayer.c:381-390; dayzplayerimplement.c:2530-2556]. Gunfire uses `class NoiseShoot { strength = 82; type = "shot"; }` in weapon config [IMPWMODPart2\Weapons\Automatic\MCXSpear\config.cpp:73-77].

`AddNoiseTarget` allows creating a noise "decoy" at a fixed position with duration — useful for stun grenades or decoys.

**For LF_RollingStone:** if infected should react upon rolling, call `g_Game.GetNoiseSystem().AddNoise(this, noiseParams)` from item script (server) with a `NoiseParams` loaded from config.

---

## 9. Environment control (SoundControllerOverride)

```
proto native void SetSoundControllerOverride(string controllerName, float value, SoundControllerAction action);
proto native void MuteAllSoundControllers();
proto native void ResetAllSoundControllers();
```
[scripts\3_game\sound.c:38-48]

Available controllers (documented in proto comment):
`rain, night, meadow, trees, hills, houses, windy, deadBody, sea, forest, altitudeGround, altitudeSea, altitudeSurface, daytime, shooting, coast, waterDepth, overcast, fog, snowfall, caveSmall, caveBig`

---

## 10. What DOES NOT exist / typical confabulations

| Common confabulation | Verified reality |
|---|---|
| `GetGame().CreateSoundOnObject(obj, "MiSoundSet", ...)` for everything | Exists but returns `SoundOnVehicle`, NOT `EffectSound`. Prefer `SEffectManager.PlaySoundOnObject`. [game.c:692] (What kind of name `sound_name` expects was not re-checked.) |
| `SEffectManager` on server | **Sound side does not exist** — `Init()` is only called when `!IsDedicatedServer()` [game.c:56-69]. On the dedicated server only `InitServer()` runs, for particle effecters [effectmanager.c:517-521]. |
| Global script volume of a specific sound | No `SetMasterVolume`-type API for individual instances beyond `SetVolumeRelative(0-1)` in `AbstractWave`. Shader "volume" is fixed in config. |
| `OnSoundEvent` or automatic callback in ItemBase | NO such override exists. Item sounds are activated manually via `StartItemSoundServer` or `PlaySoundSet`. |
| `PlaySoundSet` working on server | Has guard `!g_Game.IsDedicatedServer()` — **silent on DS**. [object.c:1249] Still true on 1.30 (other sound paths additionally use `IsHeadlessOrDedicatedServer()`). |
| Public `DynamicMusicPlayer.PlayTrack(string)` | The `PlayTrack` method is **private**. Public API is `SetCategory`. [dynamicmusicplayer.c:511, :272] |
| Multiple simultaneous sounds with `StartItemSoundServer` impossible | Before 1.30 only (one play + one stop per sync). 1.30 batches all plays of a frame into one RPC [itembase.c:4440-4483]; the comment at [itemsoundhandler.c:19-20] is stale. |
| `GetNoiseReductionEx` / `GetNoiseReductionByWeatherEx` replace the old weather helpers | Both are `[Obsolete]` on 1.30 too [sensesaievaluate.c:18; weather.c:407] — dampening is native (`AIParams`); drop the factor. |
| `SurfaceInfo.GetSoundHitType()` / `SoundHitType` | `[CHANGELOG 1.30]` names them (T189758); no such identifier exists in `P:\scripts` on 164078. Do not call. |

---

## 11. Mod recipes

### A. Simple sound on an item (without server synchronization)
```cpp
// config.cpp
class CfgSoundShaders
{
    class LFRS_Roll_SoundShader
    {
        samples[] = {{"MiMod\Sounds\roll_loop", 1}};
        volume = 1.0;
        range = 40;
        rangeCurve[] = {{0,1},{20,0.8},{40,0}};
    };
};
class CfgSoundSets
{
    class LFRS_Roll_SoundSet
    {
        soundShaders[] = {"LFRS_Roll_SoundShader"};
    };
};
```
```c
// Script: on item (client only)
EffectSound m_RollSound;

void StartRolling()
{
    if (!g_Game.IsDedicatedServer())
        PlaySoundSet(m_RollSound, "LFRS_Roll_SoundSet", 0.1, 0.5, true);
}

void StopRolling()
{
    if (!g_Game.IsDedicatedServer())
        StopSoundSet(m_RollSound);
}
```

### B. Synchronized sound server→client (ItemSoundHandler pattern)
```c
// Add custom ID in subclass or reuse SoundConstants

override void InitItemSounds()
{
    super.InitItemSounds();
    GetItemSoundHandler().AddSound(SoundConstants.ITEM_PLACE, "LFRS_Impact_SoundSet");
}

// From server upon detecting impact:
StartItemSoundServer(SoundConstants.ITEM_PLACE);
// 1.30: clients play it from ItemBase.OnRPC (RPC_ITEM_SOUND_PLAY) — keep super.OnRPC in any OnRPC override.
// Before 1.30: clients played it from OnVariablesSynchronized.
```

### C. Dynamic contextual music
```c
// Register music zone when item enters game (from client/server):
DynamicMusicPlayer dmp = g_Game.GetMission().GetDynamicMusicPlayer(); // scripts\3_game\gameplay.c:752; vanilla contaminatedarea.c:21
dmp.RegisterDynamicLocation(this, DynamicMusicLocationTypes.CONTAMINATED_ZONE, 100.0);
// Al destruirse:
dmp.UnregisterDynamicLocation(this);                                   // vanilla contaminatedarea.c:29
```

### D. Noise for infected AI
```c
// On server, upon rolling / impacting:
NoiseParams np = new NoiseParams();
np.LoadFromPath("CfgVehicles SurvivorBase NoiseActionDefault"); // vanilla actionopendoors.c:83; class at P:\DZ\characters\data\config.cpp:942
// or np.Load("ZombieStepNoise");                                // a CfgNoises class name (P:\DZ\AI\config.cpp:6903-6941)
g_Game.GetNoiseSystem().AddNoise(this, np, 2.0);  // multiplicador x2
```

1.30: do not multiply that `2.0` by a script-calculated weather reduction; the engine already attenuates natively (`AIParams`).

---

## 12. DayZ 1.30 — radios, vegetation, destructor, horn, item-sound RPC

Detalle `[EXACT]` en `references/dayz-1-30-sound.md`.

- **Item sounds:** batched `RPC_ITEM_SOUND_PLAY` / `RPC_ITEM_SOUND_STOP` instead of sync vars (§4) [scripts\4_world\entities\itembase.c:4440-4507, :2800-2829].
- **Radio / bunker:** `TransmitterBase.SOUND_BUNKER_STATIC_NOISE`; `UseBunkerStaticNoise()` = `IsTunedToBunkerFrequency() && SOUND_BUNKER_STATIC_NOISE != ""` [scripts\4_world\entities\itembase\transmitterbase.c:5-6, :135-138]. Proto in `ItemTransmitter` [scripts\4_world\entities\core\inherited\inventoryitem.c:25]. `PersonalRadio` / `BaseRadio` assign `"bunkerbroadcast_staticnoise_SoundSet"` [transmitterbase\personalradio.c:6, baseradio.c:6] (that SoundSet is not defined in the unpacked `P:\DZ` configs — unverified).
- **Static vegetation:** `StaticObjectType` reads `CfgNonAIVehicles <Name> VegetationSounds` [scripts\3_game\entities\staticobjecttype.c:11-50]. Macro `CUSTOM_PLAYER_MOVEMENT_SOUNDSETS` in [P:\DZ\data\basicdefines.hpp:310-338].
- **Event destructor:** `~SoundEventBase` does `if (g_Game) Stop();` [scripts\4_world\classes\soundevents\soundevents.c:12-16]; `~InfectedSoundEventBase` was removed (1.29: `infectedsoundeventbase.c:15-18`). Do not reimplement the 1.29 unguarded destructor in `InfectedSoundEventBase` subclasses.
- **Horn:** `VehicleHornComponent` [scripts\4_world\entities\vehicles\components\vehiclehorncomponent.c] — sync var `m_HornComponent.m_Mode` [carscript.c:283], sets registered with `RegisterSound(VehicleHornMode.SHORT/LONG, …)`; `m_CarHornShortSoundName/LongSoundName` are deprecated and no longer read.
- **SurfaceInfo** sound-set getters (`GetStepSoundSetName`, `GetWheelSoundSetName`, …) [scripts\3_game\surfaceinfo.c:50-67].

---

## Fuentes

| Path | Content |
|---|---|
| `scripts\3_game\sound.c` | SoundParams, SoundObjectBuilder, SoundObject, AbstractWave, AbstractSoundScene, WaveKind |
| `scripts\3_game\effectmanager.c` | Full SEffectManager |
| `scripts\3_game\effects\effectsound.c` | Full EffectSound |
| `scripts\3_game\entities\object.c:1234-1325` | PlaySound/PlaySoundLoop (SoundOnVehicle), PlaySoundSet, PlaySoundSetAtMemoryPoint, StopSoundSet |
| `scripts\3_game\entities\soundonvehicle.c` | SoundOnVehicle, SoundWaveOnVehicle |
| `scripts\3_game\global\game.c:56-69,692,735,738,1130-1136` | SEffectManager init split, CreateSoundOnObject, GetSoundScene, GetNoiseSystem, IsHeadless/IsHeadlessOrDedicatedServer (1.30) |
| `scripts\3_game\noise.c` | NoiseSystem (+ `GetEnvironmentNoiseReduction`, 1.30), NoiseParams |
| `scripts\3_game\constants.c:422-464` | SoundConstants |
| `scripts\3_game\systems\dynamicmusicplayer\dynamicmusicplayer.c` | DynamicMusicPlayer |
| `scripts\4_world\entities\itembase.c:4420-4507` | InitItemSounds, StartItemSoundServer (queue + RPC), StopItemSoundServer (RPC) |
| `scripts\4_world\entities\itembase.c:2800-2829` | 1.30 OnRPC item-sound dispatch (1.29: OnVariablesSynchronized `itembase.c:3319-3332`) |
| `scripts\4_world\classes\soundhandlers\itemsoundhandler.c` | ItemSoundHandler |
| `scripts\4_world\classes\soundhandlers\playersoundmanager.c` | PlayerSoundManagerBase |
| `scripts\4_world\entities\dayzplayerimplement.c:3258,3269` | AddNoise, OnStepEvent |
| `scripts\4_world\static\sensesaievaluate.c`, `scripts\3_game\weather.c:407-467` | NoiseAIEvaluate / weather noise helpers (all reduction helpers obsolete on 1.30) |
| `P:\DZ\data\aiconfigs\config.cpp:12-23` | `AIParams` noise multipliers |
| `scripts\4_world\entities\vehicles\components\vehiclehorncomponent.c` | VehicleHornComponent (1.30) |
| `P:\DZ\sounds\hpp\config.cpp` | Vanilla CfgSoundCurves (:59), CfgDistanceFilters (:2422), CfgSound3DProcessors (:2587), CfgSoundShaders (:3072), CfgSoundSets (:318379) |
| `IMPWMODPart2\Weapons\Automatic\Sounds\config.cpp` | Prior art: CfgSoundShaders with inline rangeCurve |
| `IMPWMODPart2\Weapons\Automatic\F2000\Sounds\config.cpp` | Prior art: vanilla base inheritance |
| `IMPWMODPart2\Weapons\Automatic\MCXSpear\config.cpp:59` | Prior art: soundSetShot in weapon mode + NoiseShoot |
| `DoorLockSystem\Scripts\...\ActionUnlockDLSDoor.c:63` | Prior art: building.PlaySound / PlaySoundLoop |
| `scripts\3_game\entities\staticobjecttype.c` | StaticObjectType / VegetationSounds (1.30) |
| `scripts\4_world\entities\itembase\transmitterbase.c` | SOUND_BUNKER_STATIC_NOISE |
| `scripts\3_game\systems\dynamicmusicplayer\dynamicmusicplayerregistrynasdara.c` | Nasdara music registry |
| `references/dayz-1-30-sound.md` | `[EXACT]` 1.30 blocks |
