# Script API Reference — Particle System

All signatures verified from vanilla source (Particle.c, ParticleSource.c,
ParticleManager.c, ParticleBase.c). Inheritance: ParticleBase → Particle → ParticleSource.
Re-checked on DayZ 1.30.164078 (`P:\scripts\3_game\particles\`): `particle.c` is unchanged from 1.29; the only
1.30 changes in these files are the headless gate in `ParticleManager.GetInstance()`, `ParticlePropertiesFlags.DISABLED_IN_UI`
+ `ParticleProperties.IsDisabledInUI()`, and the `ParticleBase` start-sound helpers (see the 1.30 section at the end).

---

## CLASS HIERARCHY

```
Entity
 └─ ParticleBase          (abstract base, events, IsParticle)
     └─ Particle           (legacy, EOnFrame lifetime, #particlesourceenf child)
         └─ ParticleSource  (native C++ backed, no EOnFrame, proto methods)

ParticleManager (Managed)  — pool of ParticleSource, singleton
SEffectManager (static)    — wraps Effect objects (particle+sound combos)
```

---

## Particle (legacy) — Static Factory Methods

```
// Create without playing
static Particle CreateOnObject(int particle_id, Object parent_obj,
    vector local_pos = "0 0 0", vector local_ori = "0 0 0",
    bool force_world_rotation = false);

static Particle CreateInWorld(int particle_id, vector global_pos,
    vector global_ori = "0 0 0", bool force_world_rotation = false);

// Create AND play immediately
static Particle PlayOnObject(int particle_id, Object parent_obj,
    vector local_pos = "0 0 0", vector local_ori = "0 0 0",
    bool force_world_rotation = false);

static Particle PlayInWorld(int particle_id, vector global_pos);

// Legacy aliases (backwards compat)
static Particle Play(int particle_id, Object parent_obj, vector local_pos, vector local_ori);
static Particle Play(int particle_id, vector global_pos, vector global_ori);
static Particle Create(int particle_id, Object parent_obj, vector local_pos, vector local_ori);
static Particle Create(int particle_id, vector global_pos, vector global_ori);
```

## Particle — Instance Methods

```
// Playback
void PlayParticle(int particle_id = -1);
bool PlayParticleEx(int particle_id = -1, int flags = 0);
bool StopParticle(int flags = 0);
void Stop();                              // legacy alias
void Play(int particle_id = -1);          // legacy alias

// State
bool IsParticlePlaying();                 // from ParticleBase
bool HasActiveParticle();
int GetParticleCount();
bool IsRepeat();
float GetMaxLifetime();

// Properties
void SetSource(int particle_id);          // set ID (does NOT change live particle)
int GetParticleID();
Object GetDirectParticleEffect();         // child #particlesourceenf entity
Object GetParticleParent();

// Parenting
void AddAsChild(Object parent, vector local_pos = "0 0 0",
    vector local_ori = "0 0 0", bool force_rotation_to_world = false);

// Parameter tuning
void SetParticleParam(int parameter_id, float value);             // all emitters
void SetParameter(int emitter, int parameter, float value);       // -1=all
void GetParameter(int emitter, int parameter, out float value);
float GetParameterEx(int emitter, int parameter);
float GetParameterOriginal(int emitter, int parameter);
void ScaleParticleParamFromOriginal(int parameter_id, float coef);
void ScaleParticleParam(int parameter_id, float coef);
void IncrementParticleParamFromOriginal(int parameter_id, float value);
void IncrementParticleParam(int parameter_id, float value);

// Wiggle API
void SetWiggle(float random_angle, float random_interval);
void StopWiggle();
bool IsWiggling();
```

---

## ParticleSource — Additional/Override Methods

```
// Static factory (preferred over Particle statics for new code)
static ParticleSource CreateParticle(int id, vector pos,
    bool playOnCreation = false, Object parent = null,
    vector ori = vector.Zero, bool forceWorldRotation = false,
    Class owner = null);

static ParticleSource CreateParticleEx(int id, vector pos,
    int flags = ParticlePropertiesFlags.NONE, Object parent = null,
    vector ori = vector.Zero, Class owner = null);

// Playback (native C++ backed — faster than Particle)
override bool PlayParticleEx(int particle_id = -1, int flags = 0);
override bool StopParticle(int flags = 0);   // flags: StopParticleFlags enum
override bool ResetParticle();
override bool RestartParticle();              // reset + play
override bool IsParticlePlaying();

// Auto-destroy control
void SetParticleAutoDestroyFlags(ParticleAutoDestroyFlags flags);
void DisableAutoDestroy();                    // alias for flags=NONE
int GetParticleAutoDestroyFlags();

// Particle assignment
bool SetParticleByID(int id);
override void SetSource(int particle_id);     // calls SetParticleByID
bool GetParticle(out string path, EGetParticleMode mode);
override int GetParticleID();
```

## ParticleSource Enums

```
enum ParticleAutoDestroyFlags {
    NONE,       // no auto destroy — must ObjectDelete manually
    ON_END,     // destroy when particle ends (looping=never)
    ON_STOP,    // destroy when stopped
    ALL         // ON_END | ON_STOP (DEFAULT)
};

enum StopParticleFlags {
    NONE,       // = 0; no-op flag value. The default StopParticle() behavior
                // (called with no flags or NONE) IS gradual fade — but that's
                // the default mode, not something the NONE flag activates.
    RESET,      // reset state after stopping
    IMMEDIATE,  // stop NOW, clear VISIBLE flag
    VISIBLE,    // keep visible (use with IMMEDIATE for PAUSE)
    PAUSE       // IMMEDIATE|VISIBLE — freeze visible
};

enum ParticlePropertiesFlags {
    NONE,
    PLAY_ON_CREATION,    // auto-play after create
    FORCE_WORLD_ROT,     // orientation in world space not parent
    KEEP_PARENT_ON_END,  // don't unparent when particle ends
    DISABLED_IN_UI       // 1.30: "Not created in player review" (particlesource.c:62-63)
};

enum EGetParticleMode {
    FULL,    // "graphics/particles/name.ptc"
    NO_EXT,  // "graphics/particles/name"
    FILE     // "name"
};
```

---

## ParticleManager — Pool Management

```
// Singleton access (returns null on dedicated server AND headless client!)
// 1.30: pool skipped when IsHeadlessOrDedicatedServer — particlemanager.c:65. Before 1.30: only IsDedicatedServer.
static ParticleManager GetInstance();
static void CleanupInstance();

// Create (mirrors Particle/ParticleSource statics)
ParticleSource CreateParticle(int id, vector pos,
    bool playOnCreation = false, Object parent = null,
    vector ori = vector.Zero, bool forceWorldRotation = false,
    Class owner = null);

ParticleSource CreateOnObject(int particle_id, Object parent_obj,
    vector local_pos = "0 0 0", vector local_ori = "0 0 0",
    bool force_world_rotation = false);

ParticleSource CreateInWorld(int particle_id, vector global_pos,
    vector global_ori = "0 0 0", bool force_world_rotation = false);

// Play (create+play in one call)
ParticleSource PlayOnObject(int particle_id, Object parent_obj,
    vector local_pos = "0 0 0", vector local_ori = "0 0 0",
    bool force_world_rotation = false);

ParticleSource PlayInWorld(int particle_id, vector global_pos);

// Batch operations (native)
proto native int CreateParticles(out array<ParticleSource> particles,
    string path, notnull array<ref ParticleProperties> properties, int count);
proto native int PlayParticles(out array<ParticleSource> particles,
    string path, notnull array<vector> positions, int count);

// Pool info
proto native int GetPoolSize();
proto native int GetAllocatedCount();
proto native int GetVirtualCount();
proto native int GetPlayingCount();
proto native bool IsFinishedAllocating();

// Direct access
proto native ParticleSource GetParticle(int index);
proto native int GetParticles(out array<ParticleSource> outArray, int startIndex, int count);
```

---

## ParticleList — Registration & Lookup

```
// Register from mod (3_Game layer)
// modded class ParticleList { static const int MY_ID = RegisterParticle("path/", "name"); }
static int RegisterParticle(string file_name);                    // uses default path
static int RegisterParticle(string root_path, string file_name);  // custom path

// Lookup (1.30: paths stored lowercase; GetParticleID / GetParticleIDByName lowercase their argument —
//         particlelist.c:455-456,543,554. Before 1.30: case-sensitive, name key = file_name as passed.)
static string GetParticlePath(int particle_id);       // without .ptc suffix (lowercase on 1.30)
static string GetParticleFullPath(int particle_id);    // with .ptc suffix
static int GetParticleID(string particle_file);        // by path (without .ptc)
static int GetParticleIDByName(string name);           // by filename only — last path segment on 1.30 ("hit_wood_ent_01", not "impacts/hit_wood_ent_01"); NONE if missing
static bool IsValidId(int id);                         // != NONE && != INVALID
static string GetPathToParticles();                    // "graphics/particles/"
static void PreloadParticles();                        // client-side preload

// Constants
static const int INVALID = -1;
static const int NONE = 0;
```

---

## ParticleEvents — Event System

```
class ParticleEvents {
    ref ScriptInvoker Event_OnParticleStart;     // particle begins playing
    ref ScriptInvoker Event_OnParticleStop;      // particle stops (or lifetime over)
    ref ScriptInvoker Event_OnParticleReset;     // particle reset (ParticleSource only)
    ref ScriptInvoker Event_OnParticleEnd;       // particle fully ended (no active emitters)
    ref ScriptInvoker Event_OnParticleParented;  // received parent (ParticleSource only)
    ref ScriptInvoker Event_OnParticleUnParented; // lost parent (ParticleSource only)
};

// Usage:
ParticleSource ps = ...;
ps.GetEvents().Event_OnParticleEnd.Insert(MyCallback);
// void MyCallback(ParticleBase psrc) { ... }
```

---

## FireplaceBase Pattern (recommended for LFPG)

Vanilla FireplaceBase helper methods — the gold standard for particle lifecycle
(1.30: `P:\scripts\4_world\entities\itembase\fireplacebase.c:1110-1141`, shown here condensed):

```
// Play: creates particle if not already playing, returns success
protected bool PlayParticle(out Particle particle, int particle_type, vector pos, bool worldpos = false)
{
    if (!particle && g_Game && (!g_Game.IsHeadlessOrDedicatedServer()))
    {
        if (!worldpos)
            particle = ParticleManager.GetInstance().PlayOnObject(particle_type, this, pos);
        else
            particle = ParticleManager.GetInstance().PlayInWorld(particle_type, pos);
        return true;
    }
    return false;
}

// Stop: stops and nulls reference, returns success
protected bool StopParticle(out Particle particle)
{
    if (particle && g_Game && (!g_Game.IsHeadlessOrDedicatedServer()))
    {
        particle.Stop();
        particle = NULL;
        return true;
    }
    return false;
}
```

Key insight: `out Particle particle` ensures the caller's reference is nulled,
preventing double-stop bugs.

Before 1.30: FireplaceBase used `!g_Game.IsDedicatedServer()` (1.29: `fireplacebase.c:1096,1116`). The block above is
the 1.30 check (`fireplacebase.c:1112,1132`); copy it for new helpers.

## DayZ 1.30: SurfaceInfo, vehicle VFX, synced spawn, explosions

Signatures verified on 1.30.164078 (`P:\scripts`). Full narrative: `dayz-1-30-particles.md`.

`proto native bool IsHeadlessOrDedicatedServer();` — `P:\scripts\3_game\global\game.c:1136` (comment at `:1132-1135`:
also true for headed dedicated servers); `proto native bool IsHeadless();` at `:1130`.

`ParticleManager.GetInstance()` creates the global pool only when `!g_Game.IsHeadlessOrDedicatedServer()`
(`P:\scripts\3_game\particles\particlemanager\particlemanager.c:63-75`). Before 1.30: `!IsDedicatedServer()`.

Synced effecter API (`P:\scripts\3_game\effectmanager.c`; existed in 1.29 except `GetEffecterByID`):

```
static int CreateParticleServer(vector pos, EffecterParameters parameters);      // :586
static void ReinitParticleServer(int effecterID, EffecterParameters parameters);  // :602
static void ReactivateParticleServer(int effecterID);                             // :611
static void StartParticleServer(int effecterID);                                  // :620
static void StopParticleServer(int effecterID);                                   // :630
static void DestroyEffecterParticleServer(int effecterID);                        // :639
static EffecterBase GetEffecterByID(int effecterID);                              // :649, new in 1.30
// ParticleEffecter (class at :775):
protected typename GetEffectTypename();   // :857, new in 1.30 — return your EffectParticleGeneral subclass
```

1.30 player ceiling dust (`DustEffectCeilingHandlerSynced`) is a new caller of `CreateParticleServer`.

```
// EffectParticle (P:\scripts\3_game\effects\effectparticle.c), new in 1.30
void SelectFromSurface(string surface);              // :611 empty virtual, no vanilla override
void SelectFromSurface(SurfaceInfo surfaceInfo);     // :612 overridden by EffVehicleDust, EffWheelContact, EffWheelSmoke, EffCloudDust
void SetParticleState(int state);                    // :614 empty virtual (was only on some subclasses in 1.29)
void ParticleParamsOverride(notnull ParticleParamsOverrideData data);  // :619
void DisableParticleInUI(bool disable);              // :569 sets ParticlePropertiesFlags.DISABLED_IN_UI
bool IsDisableParticleInUI();                        // :577
void SetEffectSound(EffectSound effectSound);        // :599 sound started/stopped with the effect
// removed in 1.30: field bool m_ForceRotationRelativeToWorld — use ForceParticleRotationRelativeToWorld(bool) (:542)
//                  / IsParticleRotationRelativeToWorld() (:555)

class ParticleParamsOverrideData {                   // :651
    ref TFloatArray m_FloatModifiers;  // [0]=speed, [1]=weight in vehicle dust/contact; [0]=speed in exhaust
    void ClearAll();
};

// ParticleBase (P:\scripts\3_game\particles\particlebase.c), new in 1.30
void SetStartSoundParams(string soundSetName, vector position);  // :166, played on Event_OnParticleStart (:77)
string GetStartSoundName();                                      // :172
void PlayStartSound();                                           // :180
```

`Surface.GetStepsParticleID` / `GetWheelParticleID` are `[Obsolete("1.30: use SurfaceInfo.Get…ParticleId directly")]`
wrappers around `SurfaceInfo.GetByName(name).GetStepParticleId()` / `GetWheelParticleId()` (`P:\scripts\4_world\static\surface.c:64-74`).

`VehicleVFXComponent` (`P:\scripts\4_world\entities\vehicles\components\vehiclevfxcomponent.c:96`):
`RegisterEffect(EVehicleVFXEffect, typename, VehicleVFXDataParams)` (`:126`), `RegisterEffectGroup(EVehicleVFXEffectGroup, typename, int, VehicleVFXDataParams)` (`:139`),
`PlayEffect` (`:270`), `StopEffect` (`:282`), `SetEffectParticleState` (`:364`), `GetParticleParamsOverrideData` (`:373`),
`PlayEffectGroup` (`:390`), `GetEffectGroupParticleParamsOverrideData` (`:530`). Enums in `P:\scripts\3_game\enums\evehiclevfxtypes.c`.

`EffWheelSmoke.SetSurface(string)` is `[Obsolete("1.30: use SelectFromSurface instead")]` (`P:\scripts\4_world\entities\effects\wheelsmoke.c:32`).

```
// AmmoTypesAPI (P:\scripts\3_game\global\ammotypes.c) — explosion particles, 1.30
class ExplosiveEffectData { int m_ParticleID; string m_SoundSetName; }                           // :1
static void AddExplosionParticleEffect(string ammoName, string surfaceName, int particleID,
                                       string soundSetName = "");                                // :78 (1.29: 3 params)
static ExplosiveEffectData GetExplosiveEffectData(string ammoName, string surfaceName);         // :90
// removed in 1.30: static int GetExplosionParticleID(string ammoName, string surfaceName)
```
Fallback when no surface/"default" entry: CfgAmmo `<ammo> particle` resolved by `ParticleList.GetParticleIDByName`
(`ammotypes.c:108-112`). Played by `DayZGame.ExplosionEffects` (`P:\scripts\3_game\dayzgame.c:3634-3638`).
