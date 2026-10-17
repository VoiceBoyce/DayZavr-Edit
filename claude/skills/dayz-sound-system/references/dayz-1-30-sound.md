# DayZ 1.30 — sound, item-sound RPC, NoiseSystem, music, radios

DayZ 1.30 stable, build 1.30.164078 (2026-10-15). Every `[EXACT]` block below was re-opened in the unpacked
stable tree `P:\scripts` / `P:\DZ`; line numbers are from those files. `1.29:` cites point at
`O:\DayZ 1.29 Старый распакованный клиент\` and exist only to recognise old code. Labels:
`../../_shared/dayz-version-policy.md`.

## Item sounds: RPC instead of sync vars

`StartItemSoundServer` queues events and sends one batched RPC per frame; `StopItemSoundServer` sends its own RPC:

```c
// [EXACT] P:\scripts\4_world\entities\itembase.c:4436
	void StartItemSoundServer(int id, int slotId)
	{
		if (!GetGame().IsServer() || id <= 0)
			return;
		
		if (!m_SoundEventQueue)
			m_SoundEventQueue = new array<ref ItemSoundEvent>;
		
		m_SoundEventQueue.Insert(new ItemSoundEvent(id, slotId));
		
		// flush once per frame instead of per-insert (batches multiple sounds)
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(FlushItemSoundQueue);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(FlushItemSoundQueue);
	}
```

```c
// [EXACT] P:\scripts\4_world\entities\itembase.c:4469
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(m_SoundEventQueue.Count());
		foreach (ItemSoundEvent ev : m_SoundEventQueue)
		{
			rpc.Write(ev.m_SoundId);
			rpc.Write(ev.m_SlotId);
		}
		rpc.Send(this, ERPCs.RPC_ITEM_SOUND_PLAY, true);
```

```c
// [EXACT] P:\scripts\4_world\entities\itembase.c:4495
	void StopItemSoundServer(int id)
	{
		if (!g_Game.IsServer())
			return;
		
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(id);
		rpc.Send(this, ERPCs.RPC_ITEM_SOUND_STOP, true);
	}
```

The client side lives in `ItemBase.OnRPC`, inside `#ifndef SERVER` / `#ifndef NO_GUI` (`itembase.c:2763-2764`,
cases `:2796-2825`), and calls `m_ItemSoundHandler.PlayItemSoundClient(soundIdPlay, slotIdPlay)` /
`StopItemSoundClient(soundIdStop)`. Enum values: `P:\scripts\3_game\enums\erpcs.c:114-115`.
`[DESIGN]` Any `OnRPC` override on an item must call `super.OnRPC(sender, rpc_type, ctx)`.

Legacy, kept "for modding support" (`itembase.c:115-119`, `:208-213`, `:3291-3310`, `:4481-4493`, `:4944-4949`):
the sync vars `m_SoundSyncPlay/m_SoundSyncStop/m_SoundSyncSlotID` are still registered when the item has a
handler, but no vanilla code sets `m_SoundSyncPlay` to an id any more and nothing in vanilla calls
`LegacyStopItemSoundServer(id)` (the only writer of `m_SoundSyncStop`); the vars are dispatched only if your
`OnVariablesSynchronized` override calls `LegacyItemSoundSync()`.
`ClearStartItemSoundServer` is `[Obsolete("1.30: No replacement")]`.

Before 1.30: `StartItemSoundServer` ran when `g_Game.IsServer() || !g_Game.IsMultiplayer()`, set
`m_SoundSyncSlotID`/`m_SoundSyncPlay`, called `SetSynchDirty()` and cleared them 100 ms later; `ItemBase.OnVariablesSynchronized`
played them (1.29: `itembase.c:4468-4509`, `:3319-3332`). One play + one stop per sync tick.
Not verified on this machine: whether the 1.30 RPC reaches the local player in offline single-player.

Vanilla example of moving direct `PlaySoundSet` calls to the handler: `CombinationLock`
(`P:\scripts\4_world\entities\itembase\combinationlock.c:808-819` registers, `:625-643` fires
`StartItemSoundServer(SoundConstants.ITEM_COMBINATION_LOCK_*)`, `:822-845` old `SoundLock*` methods
`[Obsolete("1.30: Use ItemSoundHandler instead")]`). New ids: `P:\scripts\3_game\constants.c:444-463`.

## Server / headless guards

`Object.PlaySoundSet` is **still** `!g_Game.IsDedicatedServer()` in 1.30:

```c
// [EXACT] P:\scripts\3_game\entities\object.c:1247
	bool PlaySoundSet( out EffectSound sound, string sound_set, float fade_in, float fade_out, bool loop = false )
	{
		if ( g_Game && !g_Game.IsDedicatedServer() )
		{
```

Before 1.30: `IsDedicatedServer()` was the only guard available (1.29: `object.c:1243-1245`).

1.30: code that must not run on headless clients uses the new protos.

```c
// [EXACT] P:\scripts\3_game\global\game.c:1127
	/**
	 \brief Check whether the application is headless or not (applies to both server and headless roboclients or anything else that is headless)
	*/
	proto native bool		IsHeadless();
	
	/**
	 \brief Check whether the application is headless or a dedicated server (it combines IsDedicatedServer and IsHeadless)
		\note This includes servers with a head as well (-headlessMode=0)
	*/
	proto native bool		IsHeadlessOrDedicatedServer();
```

```c
// [EXACT] P:\scripts\4_world\classes\soundhandlers\itemsoundhandler.c:119  (StopItemSoundClient)
	void StopItemSoundClient(int id)
	{
		if (g_Game.IsHeadlessOrDedicatedServer())
			return;
```

```c
// [EXACT] P:\scripts\4_world\classes\soundhandlers\playersoundmanager.c:105
		if( !g_Game.IsHeadlessOrDedicatedServer() ) 
		{
			m_ClientCharacterDistanceCheck.Run(2, this, "CheckAllowUpdate", null, true);
		}
```

```c
// [EXACT] P:\scripts\4_world\entities\manbase\dayzplayer\dayzplayercfgsounds.c:335
	if(!g_Game.IsHeadlessOrDedicatedServer())//sounds are unnecessary on server
	{
```

`SEffectManager.Cleanup` warns if sound maps exist on that side (`P:\scripts\3_game\effectmanager.c:535-543`).
Also `Torch` loop sound (`4_world/entities/itembase/torch.c:648,707`). `IsHeadless` / `IsHeadlessOrDedicatedServer`
are **not** in 1.29 `game.c`. Headless "roboclients" (`ROBOCLIENT` define, `-roboclient`) `[reported: bzed]`.

`[DESIGN]` New client play/stop paths: prefer `IsHeadlessOrDedicatedServer()` (per the proto note it is also
true for a server running with a head, `-headlessMode=0`). `PlaySoundSet` on `Object` still uses
`IsDedicatedServer()`, so on a headless client it is *not* skipped — do not assume `PlaySoundSet` migrated.

## EffectSound fade-in peak

```c
// [EXACT] P:\scripts\3_game\effects\effectsound.c:549
	void Event_OnSoundWaveStarted()
	{
		m_SoundWaveIsPlaying = true;
		if (m_SoundFadeInDuration > 0)
		{
			SetSoundVolume(0);
		}
```

Before 1.30: `Event_OnSoundWaveStarted` set `m_SoundWaveIsPlaying` and invoked the invokers with no volume zeroing (1.29: `effectsound.c:549-556`). `[DESIGN]` Looped item sounds with `SetSoundFadeIn` > 0 no longer start at full volume for one frame.

## `HEAVY_BREATHING` (anim event 907)

```c
// [EXACT] P:\scripts\4_world\classes\soundevents\playersoundevents\playersoundeventhandler.c:38
	HEAVY_BREATHING,
```

```c
// [EXACT] P:\scripts\4_world\classes\soundevents\playersoundevents\events\heavybreathevents.c:21
class HeavyBreathEvent1 : HeavyBreathSoundEventBase
{
	void HeavyBreathEvent1()
	{
		m_ID = EPlayerSoundEventID.HEAVY_BREATHING;
		m_SoundVoiceAnimEventClassID = 907;
	}
```

Registered in `playersoundeventhandler.c:99`. Vanilla trigger from silicosis: `P:\scripts\4_world\classes\playermodifiers\modifiers\diseases\silicosis.c:105` `RequestSoundEventEx(EPlayerSoundEventID.HEAVY_BREATHING, true, EPlayerSoundEventParam.HIGHEST_PRIORITY)`.

## Dynamic music per world

`RegisterDynamicLocation` is **still** on `P:\scripts\3_game\systems\dynamicmusicplayer\dynamicmusicplayer.c:289`
(that file is unchanged from 1.29 up to line 732, so the older cites into it hold).

Per-world registries are **not** new: 1.29 already had `DynamicMusicPlayerRegistryChernarus/Enoch/Sakhal`
and `RegisterTrackLocationStaticMultiRectangle` (1.29: `missionbase.c:113-137`, `dynamicmusicplayerregistry.c:228`).
1.30 adds Nasdara (and maps the `samplemap5` world to the same registry, `missionbase.c:135-138`):

```c
// [EXACT] P:\scripts\5_mission\mission\missionbase.c:130
			case "nasdara":
				m_WorldData = new NasdaraData();
				m_DynamicMusicPlayerRegistry = new DynamicMusicPlayerRegistryNasdara();
				break;
```

The switch is `MissionBase.InitialiseWorldData()` (`missionbase.c:107-152`), called before the
`DynamicMusicPlayer` is built from the registry (`:30`, `:36`). Nasdara static boxes use
`RegisterTrackLocationStaticMultiRectangle` (`P:\scripts\3_game\systems\dynamicmusicplayer\dynamicmusicplayerregistry.c:224-244`,
calls at `dynamicmusicplayerregistrynasdara.c:39-67`). 15 time-of-day tracks (DAY/DUSK/DAWN/NIGHT), not hourly (`:71-93`).
The `Music_nasdara_*` SoundSets themselves are not defined in any `config.cpp` under the unpacked `P:\DZ`
(grep is empty) — they ship in an addon that is not unpacked on this machine.

1.30 also makes the player use it: `m_ActualTimeOfDay = DynamicMusicPlayerTimeOfDay.Translate(...)`
(`dynamicmusicplayer.c:733`, `:737`). Before 1.30: `class DynamicMusicPlayerTimeOfDay : WorldDataDaytime {}`
was an empty alias (1.29: `dynamicmusicplayer.c:1053-1054`).

```c
// [EXACT] P:\scripts\3_game\systems\dynamicmusicplayer\dynamicmusicplayer.c:1053
//! simplified WorldDataDaytime
class DynamicMusicPlayerTimeOfDay : WorldDataDaytime
{
	//! translates WorldDataDaytime -> DynamicMusicPlayerTimeOfDay
	static int Translate(int value)
	{
		switch (value)
		{
			case MORNING:
				return DAY;
			case NOON:
				return DAY;
			case AFTERNOON:
				return DAY;
			case EVENING:
				return NIGHT;
```

## Transmitter bunker static

```c
// [EXACT] P:\scripts\4_world\entities\itembase\transmitterbase.c:5
	string SOUND_RADIO_TURNED_ON 		= "";
	string SOUND_BUNKER_STATIC_NOISE 	= "";
```

```c
// [EXACT] P:\scripts\4_world\entities\itembase\transmitterbase.c:135
	protected bool UseBunkerStaticNoise()
	{
		return IsTunedToBunkerFrequency()  &&  SOUND_BUNKER_STATIC_NOISE != "";
	}
```

The loop swaps sets when the tuning changes (`transmitterbase.c:125`, `:143-148`). The proto lives on class
`ItemTransmitter` (`inventoryitem.c:14`), not on class `InventoryItem`:

```c
// [EXACT] P:\scripts\4_world\entities\core\inherited\inventoryitem.c:25
	proto native bool IsTunedToBunkerFrequency();
```

```c
// [EXACT] P:\scripts\4_world\entities\itembase\transmitterbase\personalradio.c:6
		SOUND_BUNKER_STATIC_NOISE = "bunkerbroadcast_staticnoise_SoundSet";
```

Same assignment on `baseradio.c:6`. `SetNextFrequency` already calls `SetSynchDirty()` (`transmitterbase.c:45-49`).
`[UNVERIFIED]` native MHz that `IsTunedToBunkerFrequency` matches. `[UNVERIFIED]` the
`bunkerbroadcast_staticnoise_SoundSet` class: it is referenced by script but not defined in any config
under the unpacked `P:\DZ` (only `personalradio_/pastransmitter_/baseradio_staticnoise_SoundShader`,
`P:\DZ\sounds\hpp\config.cpp:7501-7550`).

## Vegetation step sounds on static objects

```c
// [EXACT] P:\scripts\3_game\entities\staticobjecttype.c:13
		string pathBase = "CfgNonAIVehicles " + GetName() + " VegetationSounds";
		if (!g_Game.ConfigIsExisting(pathBase))
			return;
```

Each subclass of `VegetationSounds` gives `soundSet` + `animEventIds[]` (`staticobjecttype.c:19-41`).

```c
// [EXACT] P:\scripts\3_game\entities\staticobjecttype.c:44
	SoundObjectBuilder GetVegetationSoundBuilderFromEventID(int stepEventID)
	{
		if (m_CustomVegetationSoundsMap)
			return m_CustomVegetationSoundsMap.Get(stepEventID);
```

Caller: on a step, for each colliding `WoodBase` with `HasPlayerCollisionSound()`, the custom lookup runs first
and falls back to `DayZPlayerType.GetVegetationSounds()` (`P:\scripts\4_world\entities\dayzplayerimplement.c:3394-3425`).

Macro `CUSTOM_PLAYER_MOVEMENT_SOUNDSETS(walkErc, walkCro, runErc, sprintErc, scuffErc)` —
`P:\DZ\data\basicdefines.hpp:310-338`. Anim event IDs: walk 1,2,23,24,51,52; crouch 21,22,31,32,33,34,131;
run 3,4,17,101,53,54; sprint 5,6,35,36,37,38,100,110,111; scuff 7,8.

## Infected sound-event destructor

```c
// [EXACT] P:\scripts\4_world\classes\soundevents\soundevents.c:12
	void ~SoundEventBase()
	{
		if (g_Game) 
			Stop();
	}
```

Before 1.30: `~SoundEventBase` stopped `m_SoundSetCallback` unguarded (1.29: `soundevents.c:13-16`) and
`InfectedSoundEventBase` had its own destructor doing the same (1.29: `infectedsoundevents\infectedsoundeventbase.c:15-18`);
1.30 removed the latter. `[DESIGN]` Do not re-add a 1.29-style `~InfectedSoundEventBase` that calls
`m_SoundSetCallback.Stop()` without the `g_Game` guard.

## NoiseSystem (shared with `dayz-ai-patterns`)

See also `dayz-ai-patterns/references/dayz-1-30-ai.md`.

```c
// [EXACT] P:\scripts\3_game\noise.c:12
	//! Absolute noise-strength reduction the environment (weather/sandstorm/sea) applies at 'pos'. Single source of truth used by AI.
	proto native float GetEnvironmentNoiseReduction(vector pos);
```

```cpp
// [EXACT] P:\DZ\data\aiconfigs\config.cpp:12
class AIParams
{
	maxNoiseRange=300;
	rainToNoiseMultiplier=10;
	snowfallToNoiseMultiplier=5;
	seaToNoiseMultiplier=15;
	fogToNoiseMultiplier=10;
	windToNoiseMultiplier=10;
	sandstormToNoiseMultiplier=10;
	noiseDampeningMultiplier=0.69999999;
	noiseCollisionDampeningMultiplier=0.2;
	noiseShotCollisionDampeningMultiplier=0.5;
```

(`snowfall/fog/wind/sandstormToNoiseMultiplier` are new in 1.30; 1.29 had only rain and sea.)

```c
// [EXACT] P:\scripts\4_world\static\sensesaievaluate.c:18
	[Obsolete("1.30: AI noise dampening is now done natively via AIParams config parameters. Kept for HUD and old mods.")]
	static float GetNoiseReductionEx(notnull Object object)
```

```c
// [EXACT] P:\scripts\3_game\weather.c:407
	[Obsolete("1.30: AI noise dampening is now done natively via AIParams config parameters. Kept for HUD and old mods.")]
	float GetNoiseReductionByWeatherEx(notnull Object object)
```

The older `NoiseAIEvaluate.GetNoiseReduction(Weather)` (`sensesaievaluate.c:92`) and `Weather.GetNoiseReductionByWeather()`
(`weather.c:454`) point at those `Ex` methods in their `[Obsolete]` text, but the `Ex` methods are obsolete too;
no vanilla script calls any of the four. Vanilla noise calls without a weather factor:
`dayzplayerimplement.c:3472-3474` (steps), `zombiebase.c:597-600` (infected),
`vehiclehorncomponent.c:105-107` (horn), `trapspawnbase.c:648` (trap catch); the presence HUD reads
`GetEnvironmentNoiseReduction` (`pluginpresencenotifier.c:153-159`) and its comment at `:229` says
"weather dampening is applied engine-side now".

```c
// [EXACT] P:\scripts\4_world\static\miscgameplayfunctions.c:1886
	static void GenerateAINoiseAtPositionEx(notnull Object source, float lifeTime, NoiseParams noiseParams)
	{
		if (g_Game.IsServer())
		{
			NoiseSystem noise = g_Game.GetNoiseSystem();
			if (noise)
			{
				noise.AddNoiseTarget(source.GetPosition(), lifeTime, noiseParams);
			}
		}
	}
```

`GenerateAINoiseAtPosition(vector, float, NoiseParams)` is `[Obsolete("Use MiscGameplayFunctions.GenerateAINoiseAtPositionEx instead!")]`
(`miscgameplayfunctions.c:2072`). Before 1.30 it passed `NoiseAIEvaluate.GetNoiseReduction(g_Game.GetWeather())`
as the strength multiplier (1.29: `miscgameplayfunctions.c:1753-1763`).

## Vehicle horn component

```c
// [EXACT] P:\scripts\4_world\entities\vehicles\components\vehiclehorncomponent.c:55
	void HandleSound()
	{
		switch (m_Mode)
		{
			case VehicleHornMode.SHORT:
				m_Parent.PlaySoundSet(m_SoundEffect, m_SoundToModeMap[VehicleHornMode.SHORT], 0, 0, false);
				break;

			case VehicleHornMode.LONG:
				m_Parent.PlaySoundSet(m_SoundEffect, m_SoundToModeMap[VehicleHornMode.LONG], 0, 0, true);
				break;
```

`m_Mode` is a sync var registered by `CarScript` (`carscript.c:283`); `SetMode` sets it on the server and
emits AI noise via `AddNoiseTarget(pos, 5, NoiseCarHorn, 1 or 2)` (`vehiclehorncomponent.c:41-53,95-110`);
`CarScript.OnVariablesSynchronized` → `HandleCarHornSound` → `HandleSound()` (`carscript.c:444`, `:1548-1551`).
Vanilla cars register their sets with `m_HornComponent.RegisterSound(VehicleHornMode.SHORT/LONG, …)`
(e.g. `inheritedcars\civiliansedan.c:24-25`); the old `m_CarHornShortSoundName/m_CarHornLongSoundName`
are still assigned there but marked deprecated and no longer read (`carscript.c:2765-2766`).
Before 1.30: `HandleCarHornSound` played `m_CarHornShortSoundName`/`LongSoundName` directly (1.29: `carscript.c:1767-1772`).
