# Dust agents, PPE, and symptoms

Source: DayZ 1.30 stable `1.30.164078`; paths relative to `P:\scripts\` unless they start with `P:\`. All of these agents, modifiers and constants are new in 1.30 (api-delta §9-10).

## Enums

```c
// [EXACT] P:\scripts\3_game\enums\eagents.c:14
	SILICOSIS 		= 512,
	EYES_IRRITATION	= 1024,
```

```c
// [EXACT] P:\scripts\3_game\constants.c:545
const int AGT_AIRBOURNE_SOLID	= 15;

const int DEF_BIOLOGICAL			= 1;
const int DEF_CHEMICAL				= 2;
const int DEF_DUST_PARTICLE_BREATH	= 3;
const int DEF_DUST_PARTICLE_EYES	= 4;
```

Modifiers (`4_world\classes\playermodifiers\emodifiers.c:62-69`): `MDF_SANDSTORM_EXPOSURE_STATIC`, `_DYNAMIC`, `MDF_PARTICLES_BREATH`, `MDF_PARTICLES_EYES`, `MDF_HEAT_STROKE1..4`. The enum starts at `MDF_TEMPERATURE = 1` (`:3`), so `MDF_SANDSTORM_EXPOSURE_STATIC` = 60. Always use the names: the enum shifts between versions.

## Transmission

`SandstormExposureMdfrBase.TransmitAgents` uses `AGT_AIRBOURNE_SOLID` and does not transmit if unconscious (`4_world\classes\playermodifiers\modifiers\conditions\sandstormexposure.c:45-51`). Base dose 1.0 agent/s (`:16-17`). Dynamic: dose × `GetIntensityForPlayer` (`:111-122`). Static (test zone): 1/s without intensity (`:76-80`).

`SandstormExposureDynamicMdfr.ActivateCondition`: storm active and intensity > 0 (`:101-104`).

PPE channel:

```c
// [EXACT] P:\scripts\4_world\plugins\pluginbase\plugintransmissionagents.c:414
			case AGT_AIRBOURNE_SOLID:
				switch (sourceAgents)
				{
					float protectionLevel = 0.0;
					Man man = Man.Cast(target);

					case eAgents.SILICOSIS:
						protectionLevel = GetProtectionLevelCachedEquipment(
							DEF_DUST_PARTICLE_BREATH,
							ECachedEquipmentItemCategory.MASK,
							man,
						);
						break;				
					case eAgents.EYES_IRRITATION:
						protectionLevel = GetProtectionLevelCachedEquipment(
							DEF_DUST_PARTICLE_EYES,
							ECachedEquipmentItemCategory.EYEWEAR|ECachedEquipmentItemCategory.MASK|ECachedEquipmentItemCategory.NVG,
							man,
						);
						break;
				}
```

`GetProtectionLevelCachedEquipment` (`plugintransmissionagents.c:577-592`): query ATTACHMENT, `m_MaximumDepth = 2`, **sums** `GetProtectionLevel(type)` of every matching attachment.

## Agents

`SilicosisAgent` (`4_world\classes\transmissionagents\agents\silicosisagent.c:5-15`): `m_MaxCount = 1000`, `m_TransferabilityIn = 3.5`, `m_DieOffSpeed = 0.9`. `EyesIrritationAgent` (`4_world\classes\transmissionagents\agents\eyesirritation.c:5-15`): max 100, transfer 1.0, die-off 0.30. Both `GetDieOffSpeedEx` → 0 while unconscious (`:18-24`).

## SilicosisMdfr (`MDF_PARTICLES_BREATH`)

Active with count > 0 (`4_world\classes\playermodifiers\modifiers\diseases\silicosis.c:36-39`). Active tick 5 s (`:33`).

| Threshold | Effect |
|---|---|
| remap from 200 | `HEAVY_BREATHING` sound event, interval 25→3 s (`:15-17, 88-107`) |
| ≥ 600 | stamina `DISEASE_PNEUMONIA` (`:3, 112-123`) |
| > 700 and count rising | `AddHealth(-HEALTH_LOSS * remap)` with `HEALTH_LOSS = 2.0` (`:4, 13, 133-137`) |
| > 800 | `SYMPTOM_FAINT` vignette cycle + unconsciousness 5–10 s every 60–120 s, shock set to 25 (`:5, 8-11, 139-220`) |

`OnActivate` already queues `SYMPTOM_FAINT` (`:46-71`).

## IrritatedEyesMdfr (`MDF_PARTICLES_EYES`)

Activates with count > 0 (`4_world\classes\playermodifiers\modifiers\diseases\irritatedeyes.c:19-22`). Sync `MODIFIER_SYNC_PARTICLE_EYES` (`:14`). Widget parameter only from 15 agents (`:78-88`).

## Squint and exposure

`SandstormExposureMdfrBase.OnActivate` queues `SYMPTOM_SQUINT` (`sandstormexposure.c:32-38`). `SquintSymptom.OnUpdateClient` uses the **first** EYEWEAR|MASK item with a non-zero `DEF_DUST_PARTICLE_EYES` (not a sum) (`4_world\classes\playersymptoms\states\secondary\squintstate.c:25-40`).

Cough: `HandleCoughing` takes the first EYEWEAR|MASK item (not NVG) with non-zero `DEF_DUST_PARTICLE_BREATH` (`sandstormexposure.c:124-160`). Interval 25→3 s. A mask cancels the cough only if its protection ≥ intensity (`coughIntensity = Clamp(intensity - protection, 0, 1)`, `:143`).

## Test zone

`SandstormZoneSmall_TESTONLY : ContaminatedArea_Static` activates/deactivates `MDF_SANDSTORM_EXPOSURE_STATIC` on enter/exit (`sandstormexposure.c:1-12`). Do not use this class in production.

## PPE item authoring

Config: protection values live in `class Protection` of the item's `CfgVehicles` class. `ItemBase.GetProtectionLevel` maps `DEF_DUST_PARTICLE_BREATH` → `dust_particle_breath` and `DEF_DUST_PARTICLE_EYES` → `dust_particle_eyes` and returns 0 if the item is ruined or has quantity ≤ 0 (`4_world\entities\itembase.c:4073-4102`).

```cpp
// [EXACT] P:\DZ\characters\masks\config.cpp:256  (NioshFaceMask)
		class Protection
		{
			biological=0.75;
			dust_particle_breath=1;
		};
```

Vanilla values on 1.30 (`P:\DZ\characters\{masks,glasses,headgear}\config.cpp`): `GasMask` breath 1 / eyes 1 (`masks\config.cpp:134-140`), `NioshFaceMask` breath 1, `SurgicalMask` breath 0.25, `BalaclavaMask_ColorBase` breath 0.45, `TacticalGoggles`/`SkiGoggles_ColorBase` eyes 1, `SportGlasses_ColorBase` eyes 0.7, `AviatorGlasses` eyes 0.4, `ThinFramesGlasses` eyes 0.3; gas-mask filters (`GasMask_Filter`, `GasMask_Filter_Improvised`) breath 1. Several of these keys already existed in 1.29 configs with other values and were not read by 1.29 scripts (config-delta).

**Category trap (static reading):** the lookups only see items whose `GetCachedEquipmentCategory()` is not `NONE` — items with `NONE` are never stored in the cached equipment (`4_world\entities\cachedequipmentstorage.c:31`; default `NONE` at `3_game\entities\entityai.c:322-325`). Vanilla sets `Glasses_Base` → EYEWEAR and `Mask_Base` → MASK (`4_world\entities\itembase\clothing_base.c:176-179, 196-199`), `NVGoggles` → NVG|EYEWEAR (`4_world\entities\itembase\nvgoggles.c:11-14`), plus explicit MASK overrides on some helmets (e.g. `motohelmet_colorbase.c`, `weldingmask.c`, `greathelm.c`). So:

- A custom mask/goggle must inherit `Mask_Base` / `Glasses_Base` or override `GetCachedEquipmentCategory()`; `dust_particle_*` on a plain `HeadGear_Base`/`Clothing` item is never read. Vanilla examples with dust values but no category override: `BurlapSackCover`, `OKZKCap_ColorBase`, `SnowstormUshanka_ColorBase`, `WitchHood_ColorBase` (`HeadGear_Base`), `NorseHelm` (`HelmetBase : HeadGear_Base`) — their dust protection should therefore be inert [not measured].
- Filters: `GP5GasMask.GetProtectionLevel` returns the attached filter's value for breath and 1.0 for eyes (`4_world\entities\itembase\clothing\gp5gasmask.c:13-28`); NVG returns eyes 1.0 only while lowered (`nvgoggles.c:83-95`). Override `GetProtectionLevel` the same way if your item's protection depends on state.

Worn mesh / item authoring: `dayz-clothing`.
