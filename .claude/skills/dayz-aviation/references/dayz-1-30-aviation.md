# DayZ 1.30 changes for CarScript-as-aviation (verified on 1.30.164078)

> Target: DayZ 1.30 stable (1.30.164078). Cites point at `P:\scripts` (1.30) unless marked `1.29:`; re-verified
> 2026-10-15 (first written on 1.30 Exp 164014; the script files involved have identical line counts on stable).
> Labels: `../../_shared/dayz-version-policy.md`. Full API delta: the `dayz-130-migration` skill.

A plane or helicopter that **inherits `CarScript`** inherits the whole 1.30 vehicle refactor: lights through
`VehicleLightsComponent`, battery through `NeedElectricitySourceDevice`, horn through `VehicleHornComponent`,
contact damage on `Transport`. Native `HelicopterScript` is still a stub. Compose with
`dayz-vehicles/references/dayz-1-30-vehicles.md` for the full car-side table and port checklist.

## Native aviation class is still a stub

Unchanged from 1.29 (`helicopterscript.c` has 35 lines on both builds): `HelicopterScript extends HelicopterAuto`
registers only `POSTSIMULATE` and has an empty `EOnPostSimulate` plus empty engine hooks.

```c
// [EXACT] P:\scripts\4_world\entities\vehicles\helicopterscript.c:4-13
class HelicopterScript extends HelicopterAuto
{
	void HelicopterScript()
	{
		SetEventMask(EntityEvent.POSTSIMULATE);
	}

	override void EOnPostSimulate(IEntity other, float timeSlice)
	{
	}
```

Do not inherit it expecting a flight model. CarScript-as-aviation stays the pattern.

## CarScript ctor / per-tick (line drift)

```c
// [EXACT] P:\scripts\4_world\entities\vehicles\carscript.c:207-208
		SetEventMask(EntityEvent.POSTSIMULATE);
		SetEventMask(EntityEvent.POSTFRAME);
```

`EOnPostSimulate` is at `carscript.c:807`; `OnDriverExit` gear-stop at `carscript.c:940-948`; empty
`Transport.OnDriverExit` at `transport.c:224`; `CleanupEffects` is now declared on `Transport` (`transport.c:998`)
and overridden by `CarScript` (`carscript.c:390`). Before 1.30 (1.29): `carscript.c:325-326`, `:948`, `:1207-1215`,
`transport.c:161`, `CleanupEffects` on `CarScript` (`carscript.c:553`).

## Lights

`CreateFrontLight`/`CreateRearLight` are `[Obsolete("1.30: use VehicleLightsComponent instead")]`
(`carscript.c:2937-2948`) and a grep of `P:\scripts` finds no caller: an aircraft that only overrides them shows the
generic beam `CarScript` registers by default (`carscript.c:270-271`; static reading), not its own. Register a
profile in the aircraft's constructor, as vanilla cars do:

```c
// [EXACT] P:\scripts\4_world\entities\vehicles\inheritedcars\offroad_02.c:21-22
		m_LightsComponent.RegisterLight("Front", new VehicleLightData(new Offroad_02LightProfileFront()));
		m_LightsComponent.RegisterLight("Rear", new VehicleLightData(new Offroad_02LightProfileRear()));
```

The profile carries the brightness/radius/angle/colour fields that a `CarLightBase` subclass used to set
(`vehiclelightprofiles\vehiclelightprofilebase.c:2-29`; example `vehiclelightprofiles\offroad_02.c:1-18`).

`ForceUpdateLightsStart`/`End` are `[Obsolete("1.30: no replacement")]` with empty bodies (`carscript.c:2805-2809`):
the parked-beam recipe that calls them is a 1.29 pattern (see `dayz-vehicles/SKILL.md` LIGHTS §3 for the 1.30 port).
`CarScript.ToggleHeadlights()` no longer exists; the switch action calls native `transport.LightToggle()`
(`actionswitchlights.c:57-65`) and is bound to `UAToggleVehicleLights` (`ToggleVehicleLightsActionInput`,
`actioninput.c:849-855`), not `UAToggleHeadlight` (headgear, `:776-782`). Custom flight `UA*` bindings are unchanged.

Nav/strobe lights built on `PointLightBase` + `ScriptedLightBase.CreateLight` (`effects-and-lights.md`) are unaffected
(`scriptedlightbase.c:127`, `:227`; `AttachOnObject` now defaults its vectors to `vector.Zero`, same meaning).

## Battery and vital parts

Battery: `NeedElectricitySourceDevice()` (no parameters) / `GetElectricitySourceDevice()` /
`GetElectricitySourceDeviceType()` on `Transport` (`transport.c:821-834`; defaults `true` / `null` / `""`).
`IsVitalCarBattery`/`IsVitalTruckBattery` are obsolete and never called (`carscript.c:2811-2821`).
Who reads what (static reading of `P:\`):
- engine start: `CarScript.CheckOperationalRequirements` → `NeedElectricitySourceDevice()` then `GetBattery()`
  (`carscript.c:1768-1774`); `GetBattery()` returns `GetElectricitySourceDevice()` cast to `ItemBase` (`:2437-2440`);
- light switch and horn actions call `GetElectricitySourceDevice()` directly (`actionswitchlights.c:38-46`,
  `actionvehiclehorn.c:15-17`).

So an aircraft must override `GetElectricitySourceDevice()` to return its battery (e.g. an aviation battery slot), or
return `false` from `NeedElectricitySourceDevice()` for a magneto engine (vanilla bikes do:
`inheritedmotorbikes\motorbike_01.c:33-36`, `motorbike_02.c:34-37`). A 1.29 aircraft that overrode only `GetBattery()`
(the RFFS pattern, `helicopters.md` "Aviation attachments") still passes the start check on 1.30, but the light and horn
actions see `null` — override `GetElectricitySourceDevice()` as well. Overrides of `IsVitalSparkPlug`/`IsVitalGlowPlug`
(now on `Transport`, `transport.c:404-412`) and `IsVitalRadiator`/`IsVitalEngineBelt` (`carscript.c:2201-2209`) still work.

## Horn

Register the sound sets on the component (`m_HornComponent.RegisterSound(VehicleHornMode.SHORT/LONG, "<SoundSet>")`,
`civiliansedan.c:24-25`); the 1.29 fields `m_CarHornShortSoundName`/`m_CarHornLongSoundName` are deprecated and never read
(`carscript.c:2765-2766`). `CarScript.SetActions()` adds `ActionVehicleHornShort/Long` (`carscript.c:2319-2320`); remove
them in the aircraft's `SetActions` override if the aircraft has no horn.

## Contact damage

`OnContact` moved to `Transport` and caches `VehicleContactData` with field `m_Impulse` (`transport.c:315-342`,
`:1144-1148`); `CarScript.CheckContactCache` reads `data[0].m_Impulse` (`carscript.c:1291-1292`). An aircraft that read
`data[0].impulse` (1.29 `CarContactData`) must switch to `m_Impulse`.

## Wreck scripts without models (new in 1.30)

`StaticObj_Wreck_Chinook : CrashBase` (`4_world\entities\building\wrecks\wreck_chinook.c:1-23`) and
`StaticObj_Wreck_Mi24_TK : CrashBase` (`wreck_mi24_tk.c:1-10`) exist and spawn a smoking particle when not dedicated.
No `config.cpp` under `P:\DZ` declares those classnames and `P:\DZ\structures_wrecks.txt` lists no matching model on
1.30.164078. Do not config-depend on them as placeable wrecks.

## Port checklist (CarScript aircraft, 1.29 → 1.30)

1. Re-verify every `carscript.c:NNNN` / `transport.c:NNNN` cite you copied from an old note against `P:\scripts`.
2. Replace `CreateFrontLight`/`CreateRearLight` with `m_LightsComponent.RegisterLight` + a `VehicleLightProfile*` class.
3. Delete `ForceUpdateLightsStart`/`End` calls and any `override void ToggleHeadlights()` (does not compile).
4. Override `GetElectricitySourceDevice` (+ `NeedElectricitySourceDevice`, `GetElectricitySourceDeviceType`) instead of
   `IsVitalCarBattery`.
5. Horn: `m_HornComponent.RegisterSound(...)`, or drop the horn actions.
6. `data[0].impulse` → `data[0].m_Impulse`.
7. Bind cockpit light toggle to `UAToggleVehicleLights` if it should use `ActionSwitchLights`; keep custom `UA*` flight axes.
8. Do not switch the entity base to `HelicopterScript`.
