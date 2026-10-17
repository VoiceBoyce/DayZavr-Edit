# DayZ 1.30 — persistence deltas

> Target: DayZ 1.30 stable (1.30.164078). Version labels: ../../_shared/dayz-version-policy.md. Porting a ≤1.29 mod: dayz-130-migration skill.

Companion to `dayz-persistence`. Every `[EXACT]` block below was re-opened in `P:\scripts` (1.30.164078,
2026-10-15). This file was first written against 1.30 Experimental 164014; stable shifted several line numbers
(e.g. `EntityAI.OnStoreSave` 2887 → 2910, `DigitalCodeLock.OnStoreSave` 469 → 484) and the stable numbers replace
them. `1.29: O:\…` cites the unpacked 1.29 tree.

Path prefix for all script cites: `P:\scripts\`.

## What changed

| Surface | 1.29 | 1.30 | Effect on a 1.29 mod or save |
|---|---|---|---|
| `GAME_STORAGE_VERSION` | `142` (1.29: `O:\…\3_game\global\game.c:5`) | `144` at `3_game\global\game.c:5` | Hook `version` on a 1.30 save is 144. `g_Game.SaveVersion()` is at `:435`. |
| `EntityAI` prefix | energy-or-nothing, then `SaveVariables` | `GetConstructionBasic().HandleStoreSave` **before** energy; load of that block only if `version >= 143` | Mods that skip `super` or assume a fixed EntityAI prefix misalign on rebuilt houses. |
| Player stats (`PlayerStatsPCO`) | `_current` = `v115`, 11 stats | `_current` = `v130`, 12 stats (`THERMAL_BIAS`); handler knows only v100, v101, current | A 1.29 player record (`version` 142) is read with the v130 layout: one value too many, every later block shifted. |
| `PlayerBase` MP stream | ends at `ArrowManagerPlayer.Save/Load` | appends `m_ThermalBiasHandler.OnStoreSave/OnStoreLoad` with **no** version gate | One more float read than a 1.29 record holds. |
| `CombinationLock` | `m_Combination`, `m_CombinationLocked` | plus `m_CombinationInside`; load gated `version >= 143` | Subclass `OnStoreLoad` that does not call `super` after v143 desyncs later bytes. |
| `DigitalCodeLock` | class did not exist | `super` then `CodeLockComponent` PIN / locked / door index | New entity; old worlds have no such items. |
| `Mag_MP18_Drum32Rnd` | class did not exist | `super` + `m_LastAnimPhase`, read at `version >= 144` | New entity. |
| `BaseBuildingBase` subclass stream | `m_SyncParts01/02/03` + `m_HasBase` | same order | No 1.30 break at the subclass suffix. |
| `Rebuilding` (map houses) | class did not exist | own 11-int payload via `EntityAI` construction handle | Not `BaseBuildingBase`. Do not copy its version check as a future-reject pattern. |
| `$mission:BunkerBroadcastPersistenceStorage.bin` | file did not exist | `FileSerializer` of two objects, no header, no `Read` check | Mission file, not the character/world bin. Not the recoverable JSON sidecar. |

## `GAME_STORAGE_VERSION` = 144

```
// [EXACT] P:\scripts\3_game\global\game.c:5
static int GAME_STORAGE_VERSION = 144;
```

`CGame` publishes it through `StorageVersion(GAME_STORAGE_VERSION)` at `:50` and `proto native int SaveVersion()` at
`:435`. Before 1.30: `GAME_STORAGE_VERSION = 142`.

Version gates that read new fields on 1.30: CombinationLock and EntityAI construction branch on `version >= 143`, the new
`Mag_MP18_Drum32Rnd` on `version >= 144` (`4_world\entities\itembase\magazine\magazines.c:112`), so a 1.29 record whose
stored version is 142 skips those new fields. `PlayerStatsPCO` and `ThermalBiasHandler` do **not** gate (sections below).

## EntityAI prefix (construction then energy)

```
// [EXACT] P:\scripts\3_game\entities\entityai.c:2910
	void OnStoreSave (ParamsWriteContext ctx)
	{
		ConstructionBasic constructionComponent = GetConstructionBasic();
		if (constructionComponent)
			constructionComponent.HandleStoreSave(ctx);
		
		// Saving of energy related states
		if ( m_EM )
		{		
			// Save energy amount
			ctx.Write( m_EM.GetEnergy() );
			
			// Save passive/active state
			ctx.Write( m_EM.IsPassive() );
			
			// Save ON/OFF state
			ctx.Write( m_EM.IsSwitchedOn() );
			
			// Save plugged/unplugged state
			ctx.Write( m_EM.IsPlugged() );
```

Before 1.30: `OnStoreSave` began at the `m_EM` block (1.29: `O:\…\3_game\entities\entityai.c:2930-2933`), no construction
handle. 1.30: `HandleStoreSave` is declared without a body on `ConstructionBasic`
(`3_game\systems\construction_basic.c:34`) and `HandleStorageEvents()` returns `false` there (`:29-32`); only `Rebuilding`
overrides both and writes (`4_world\classes\rebuilding\rebuilding.c:468-480`). Load:

```
// [EXACT] P:\scripts\3_game\entities\entityai.c:2978
	bool OnStoreLoad (ParamsReadContext ctx, int version)
	{
		ConstructionBasic constructionComponent = GetConstructionBasic();
		if (version >= 143 && constructionComponent)
		{
			constructionComponent.HandleStoreLoad(ctx,version);
		}
		
		// Restoring of energy related states
		if ( m_EM )
		{
```

The return value of `HandleStoreLoad` is ignored here (it is `void`). Energy field count is unchanged versus 1.29: eight
typed writes when `m_EM` exists (`GetEnergy`, `IsPassive`, `IsSwitchedOn`, `IsPlugged`, four persistent-ID ints,
`entityai.c:2917-2948`) then `SaveVariables` at `:2951`. (Older text in this skill said "nine fields"; both 1.29 and 1.30
write eight.) `LoadVariables` remains behind `version >= 140` (`entityai.c:3053-3058`); `SaveVariables` / `LoadVariables`
are byte-for-byte the same code as on 1.29.

## Player record: `PlayerStatsPCO_v130` and `ThermalBiasHandler`

The persistence hooks are `PlayerBase.OnStoreSave` `4_world\entities\manbase\playerbase.c:7353` and `OnStoreLoad` `:7408`.
On a server MP load the order is: `super` (EntityAI), hands FSM, lifespan, `GetPlayerStats().LoadStats(ctx, version)`
(`:7432`), modifiers, agents, symptoms, bleeding, stomach, legs (`>= 116`), flags (`>= 125`), ladder (`>= 131`), arrows
(`>= 134`), then the new thermal float (`:7520-7524`).

### Stats layout

```
// [EXACT] P:\scripts\4_world\classes\playerstats\playerstatspco.c:8
	void PCOHandlerStats(Man player)
	{
		m_Player = player;

		RegisterPCO(new PlayerStatsPCO_v100);
		RegisterPCO(new PlayerStatsPCO_v101);
		RegisterPCO(new PlayerStatsPCO_current);
	}
```

`PlayerStatsPCO_current extends PlayerStatsPCO_v130` (`:362`; 1.29: `extends PlayerStatsPCO_v115`). `PlayerStatsPCO_v130`
(`:328-351`, `GetVersion()` 130) registers 12 stats: the eleven of v115 plus `THERMAL_BIAS` (`EPlayerStats_v130`, `:312-326`).
`GetPCO(version)` returns the closest registered version at or below the requested one (`:29-55`, loop `for (int i = version; i > 100; --i)`
at `:37`), and both `SaveStats` and `LoadStats` look the layout up by storage version
(`4_world\classes\playerstats\playerstats.c:99-113`, `:115-128`). Each `PlayerStat<T>` reads and writes exactly one value
(`playerstatbase.c:169-182`).

Consequence (static reading, not measured):

- 1.29 saved a character with `SaveVersion() = 142` → `GetPCO(142)` → v115 → **11** values.
- 1.30 loads it with `LoadStats(ctx, 142)` → `GetPCO(142)` → v130 (v115 is not registered any more) → reads **12** values.
  The twelfth read consumes the first value of the modifiers block, and every later block is read shifted by one value.
- All blocks before the stats are unchanged between 1.29 and 1.30 (EntityAI `SaveVariables`/`LoadVariables`, hands FSM,
  lifespan), and so are the stream formats of modifiers, agents, symptoms, bleeding, stomach and arrows (compared
  function by function against the 1.29 tree) — the stats layout is the **first** divergence.
- Which `---- failed to load … ----` line a real 1.29 record produces is not determinable statically; what the engine then
  does with the character was not measured on 1.30.

`[DESIGN]` Treat a 1.29 → 1.30 server upgrade as a character wipe unless a test on a copy of `storage_1` shows otherwise.
A mod stat registered on `PlayerStatsPCO_current` must use index ≥ 12 on 1.30 (1.29 mods used > 10, so index 11 now
collides with `THERMAL_BIAS`).

### Thermal float

On server MP save, after `ArrowManagerPlayer.Save`:

```
// [EXACT] P:\scripts\4_world\entities\manbase\playerbase.c:7392
			ArrowManagerPlayer arrowManager = ArrowManagerPlayer.Cast(GetArrowManager());
			arrowManager.Save(ctx);
			
			m_ThermalBiasHandler.OnStoreSave(ctx); //save thermal bias handler
		}
	}
```

Load is **not** version-gated:

```
// [EXACT] P:\scripts\4_world\entities\manbase\playerbase.c:7514
			if (version >= 134)
			{
				ArrowManagerPlayer arrowManager = ArrowManagerPlayer.Cast(GetArrowManager());
				arrowManager.Load(ctx);
			}
			
			if (!m_ThermalBiasHandler.OnStoreLoad(ctx, version))
			{
				Print("---- failed to load ThermalBiasHandler, read fail  ----");
				return false;
			}
```

Handler payload is one float:

```
// [EXACT] P:\scripts\4_world\classes\thermalbiashandler.c:67
    void OnStoreSave(ParamsWriteContext ctx)
    {
        ctx.Write(m_TemporaryResistanceTime);
    }

    bool OnStoreLoad(ParamsReadContext ctx, int version)
    {
        if (!ctx.Read(m_TemporaryResistanceTime))
            return false;

        return true;
    }
```

[DESIGN] A 1.29 player record has no such float, and 1.30 always reads it on server MP. Contrast `CombinationLock`, which
**does** gate its new int. Mods that override `PlayerBase.OnStoreSave/OnStoreLoad` must still call `super` in the same
relative position; inserting fields before `super` or after a copied 1.29 suffix without the thermal float desyncs the
stream. Before 1.30 the save ended at `arrowManager.Save` (1.29: `O:\…\4_world\entities\manbase\playerbase.c:7073-7075`).

## `CombinationLock` stream v143

Before 1.30: save wrote `m_Combination` then `m_CombinationLocked`; load had the `version < 105` attached-bool remnant.
1.30: save always writes a third int; load reads it only at `version >= 143`.

```
// [EXACT] P:\scripts\4_world\entities\itembase\combinationlock.c:128
	override void OnStoreSave( ParamsWriteContext ctx )
	{   
		super.OnStoreSave(ctx);
		
		//write data
		ctx.Write( m_Combination );
		ctx.Write( m_CombinationLocked );
		ctx.Write( m_CombinationInside );
	}
```

```
// [EXACT] P:\scripts\4_world\entities\itembase\combinationlock.c:169
		if (version >= 143)						//added with 143
		{
			//combination two (inside lock)
			if (!ctx.Read(m_CombinationInside))
			{
				m_CombinationInside = 0;
				return false;
			}
		}
```

A 1.29 lock (`version` 142) still loads: the third int is skipped. A 1.30 lock (`version` 144) writes three ints. A mod
that calls `super.OnStoreSave` but implements its own `OnStoreLoad` without the `>= 143` branch reads `m_CombinationInside`
as the next subclass field.

Vanilla still assigns defaults (`m_Combination = 0`) **before** `return false` on a failed read (`:145-149`). That violates
Hard stop 5 in this skill; do not copy it. Stage, then commit.

## `DigitalCodeLock` / `CodeLockComponent`

New 1.30 item. Entity stream is `super` then the component:

```
// [EXACT] P:\scripts\4_world\entities\itembase\codelock.c:484
	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);
		
		m_CodeLockComponent.OnStoreSave(ctx);
	}
	
	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
			return false;
		
		if (!m_CodeLockComponent.OnStoreLoad(ctx, version))
			return false;
```

```
// [EXACT] P:\scripts\4_world\classes\codelockcomponent.c:257
	void OnStoreSave(ParamsWriteContext ctx)
	{
		ctx.Write(m_LockPIN);
		ctx.Write(m_IsLocked);
		ctx.Write(m_DoorIndex);
	}
	
	bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!ctx.Read(m_LockPIN))
		{
			m_LockPIN = "";
			m_HasPIN = false;
			return false;
		}
		
		if (!ctx.Read(m_IsLocked))
			return false;

		if (!ctx.Read(m_DoorIndex))
			return false;
```

No `version` gate: the class did not exist in 1.29. A failed PIN read mutates `m_LockPIN` / `m_HasPIN` before returning
`false` — same Hard stop 5 warning. `DigitalCodeLock_Bunker : DigitalCodeLock` is at `codelock.c:567`.

## `BaseBuildingBase` suffix unchanged; `Rebuilding` is a different stream

```
// [EXACT] P:\scripts\4_world\entities\itembase\basebuildingbase.c:420
	override void OnStoreSave( ParamsWriteContext ctx )
	{   
		super.OnStoreSave( ctx );
		
		//sync parts 01
		ctx.Write( m_SyncParts01 );
		ctx.Write( m_SyncParts02 );
		ctx.Write( m_SyncParts03 );
		
		ctx.Write( m_HasBase );
	}
```

`ConstructionBasic.HandleStorageEvents` returns `false`, so a traditional player-built fence still has an empty construction
handle in `super`. The 93-part / three-int packing lives here, not on `Rebuilding`.

`Rebuilding` (map-house reconstruction) writes through `EntityAI`'s handle:

```
// [EXACT] P:\scripts\4_world\classes\rebuilding\rebuilding.c:1
class Rebuilding : ConstructionBase
{
	static int REBUILDING_STORAGE_VERSION = 1;
```

```
// [EXACT] P:\scripts\4_world\classes\rebuilding\rebuilding.c:501
	protected void SerializeConstructionData(ParamsWriteContext ctx)
	{
		ctx.Write(REBUILDING_STORAGE_VERSION);
		ctx.Write(m_SyncParts1);
		ctx.Write(m_SyncParts2);
		ctx.Write(m_SyncParts3);
		ctx.Write(m_SyncParts4);
		ctx.Write(m_SyncParts5);
		ctx.Write(m_SyncParts6);
		ctx.Write(m_SyncParts7);
		ctx.Write(m_SyncParts8);
		ctx.Write(m_SyncParts9);
		ctx.Write(m_SyncParts10);
	}
```

`DeserializeConstructionData` (`:516`) rejects `m_LastStorageVersion < REBUILDING_STORAGE_VERSION` and **accepts** a stored
version greater than current (`:518-524`). That is the opposite of this skill's `future-version` cell. [DESIGN] Do not copy
it. A future extra int after the ten sync fields would be consumed as `EntityAI` energy and desync the rest of the house.
A failed deserialize only logs `ErrorEx(... ErrorExSeverity.INFO)` (`:489-490`); the outer `EntityAI.OnStoreLoad` ignores it.

## `$mission:BunkerBroadcastPersistenceStorage.bin`

Not an entity stream. Independent mission file:

```
// [EXACT] P:\scripts\4_world\classes\bunkerbroadcasthandler.c:16
class BunkerBroadcastPersistenceStorage : Managed
{
	private const string STORAGE_PATH = "$mission:BunkerBroadcastPersistenceStorage.bin";

	ref BunkerBroadcastSchedulerPersistenceData m_SchedulerData;
	ref BunkerBroadcastHandlerPersistenceData m_HandlerData;
```

```
// [EXACT] P:\scripts\4_world\classes\bunkerbroadcasthandler.c:56
	bool Load()
	{
		FileSerializer file = new FileSerializer();
		if (file.Open(STORAGE_PATH, FileMode.READ))
		{
			file.Read(m_HandlerData);
			file.Read(m_SchedulerData);
			file.Close();
			
			return true;
		}
		
		return false;
	}
```

```
// [EXACT] P:\scripts\4_world\classes\bunkerbroadcasthandler.c:73
	void Save()
	{
		FileSerializer file = new FileSerializer();
		if (file.Open(STORAGE_PATH, FileMode.WRITE))
		{
			file.Write(m_HandlerData);
			file.Write(m_SchedulerData);
			file.Close();			
		}
	}
```

Payload classes (`:5-14`): `m_LastReportHour` / `m_LastReportMinute` ints, and `TIntStringMap m_LastActiveBunkers`
(bunker id → lock code). No version header. `Serializer.Read` returns `bool` (`1_core\proto\serializer.c:58`) and vanilla
**ignores** it, then returns `true` after a successful `Open`. `DeleteFile` works only on `$profile:` / `$saves:` and
`CopyFile`'s destination must be there (`1_core\proto\ensystem.c:527-531`), so the recoverable sidecar replace in
`sidecar-files.md` cannot be used on `$mission:` with those primitives.

[DESIGN] Treat this as a vanilla mission-file exception, not a template. A mod that persists bunker-like state should still
own a version header, check every `Read`, and pick a location (`$profile:` / `$saves:`) where backup + delete + copy is possible.

## What breaks (1.29 mod or save → 1.30)

1. A 1.29 player record: read with the v130 stats layout (one value too many) and an ungated thermal float — not loaded
   correctly by vanilla 1.30 (static reading; the resulting server behaviour was not measured).
2. `PlayerBase` override that copies the 1.29 MP suffix and omits `ThermalBiasHandler`, or that never calls `super`.
   Symptom on a 1.30 record: `---- failed to load ThermalBiasHandler, read fail  ----` or a later misread.
3. A custom `PlayerStatsPCO_current` stat at index 11 — collides with `THERMAL_BIAS`.
4. `CombinationLock` subclass `OnStoreLoad` that does not call `super` or that assumes two ints after `super`. Symptom:
   later fields (or a following attachment) read as `m_CombinationInside`.
5. Any `EntityAI` subclass on a `Rebuilding` house that skips `super` or seeks a fixed offset past "energy". 1.30 writes
   construction data first when the component is present.
6. Claiming a `$mission:` `FileSerializer` overwrite is atomic, or that `BunkerBroadcastPersistenceStorage` follows
   `JsonFileLoader` + backup.
7. Sidecar code that enumerates files with `FindFile("$profile:…")` — on a 1.30 server it silently lists the server's
   working directory [reported: bzed, DZEXP-134]; see `sidecar-files.md`.

`JsonFileLoader<T>.LoadFile` / deprecated `JsonLoadFile` are at `3_game\tools\jsonfileloader.c:7-40` / `:99-133` on 1.30.
`DeleteFile` / `CopyFile` at `1_core\proto\ensystem.c:528,531`. CF ModStorage is not part of `P:\` and was not re-verified for 1.30.

## Migration checklist

- [ ] Call `super.OnStoreSave` / `super.OnStoreLoad` on `PlayerBase`, `CombinationLock`, `DigitalCodeLock`, and any `EntityAI`
      with a construction component; do not re-implement the 1.29 suffix by copy.
- [ ] `CombinationLock.OnStoreLoad`: `if (version >= 143) ctx.Read(m_CombinationInside)` (or rely on `super`).
- [ ] Do not version-gate `ThermalBiasHandler` yourself on a `PlayerBase` subclass unless you also stop writing it; vanilla
      always writes it on 1.30 MP save.
- [ ] Classify a 1.29 player record on 1.30 as `truncated` / misaligned (stats v115 → v130, thermal float), not as
      `legacy-no-header` success. Vanilla does not migrate it. Back up `storage_1` and test the upgrade on a copy.
- [ ] Custom PCO stats: index ≥ 12.
- [ ] Keep `BaseBuildingBase` 93-part packing; do not apply `Rebuilding`'s ten ints / `REBUILDING_STORAGE_VERSION` to
      player-built fences.
- [ ] If persisting mission-wide bunker state, do not clone vanilla's unchecked `FileSerializer` overwrite. Use the sidecar
      contract on `$profile:` / `$saves:`.
- [ ] Replace sidecar `FindFile` enumeration with `CF.FindFileEx` or a placeholder resolver on 1.30 servers.
