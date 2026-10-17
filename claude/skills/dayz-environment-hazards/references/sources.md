# Sources — dayz-environment-hazards (1.30.164078)

Every file below was re-opened on `P:\` (DayZ 1.30 stable 1.30.164078) on 2026-10-15; existence checked with Glob/find. The skill was first written against the 1.30 Experimental unpack (1.30.164014, `E:\DayZ-Exp-Extract\…`, no longer on this machine); those Exp paths were replaced. 1.29 comparisons use `O:\DayZ 1.29 Старый распакованный клиент\`.

## Re-opened files (1.30 stable)

| Path on `P:\` | Purpose |
|---|---|
| `P:\scripts\3_game\sandstorm.c` | `SandstormController` protos, `GetScriptedType` :124, typedef :135 |
| `P:\scripts\3_game\weather.c` | `GetSandstorm` :210, obsolete noise :407-467 |
| `P:\scripts\3_game\worlddata.c` | `StartSandstorm` :477-490, sandstorm events :495-528, sun effect :355-360, dust settings :548-551, frequency :427-443, `IsInRange` :645-654 |
| `P:\scripts\3_game\cfggameplaydatajson.c` | `ITEM_SandstormData` :440-459, root field :23 |
| `P:\scripts\3_game\cfggameplayhandler.c` | `GetSandstormFrequency` :533-536 |
| `P:\scripts\3_game\enums\eagents.c` | `SILICOSIS` 512, `EYES_IRRITATION` 1024 (:14-15) |
| `P:\scripts\3_game\enums\ecachedequipmentitemcategory.c` | `EYEWEAR` 32, `MASK` 64 |
| `P:\scripts\3_game\constants.c` | AGT/DEF dust :545-550, shadow tick 30 s :760, moto/boat HC :792-799, noise weights :1143-1145 |
| `P:\scripts\3_game\playerconstants.c` | `THERMAL_BIAS_*` :203-210, `SL_WATER_*` :49-50 |
| `P:\scripts\3_game\surfaceinfo.c` | dust protos :49-70, `GetByName` :16 |
| `P:\scripts\3_game\dustkickupeffects.c` | kickup :43-45 |
| `P:\scripts\3_game\ppemanager\requesters\ppersandstorm.c` | PPE requester |
| `P:\scripts\3_game\vehicles\transport.c` | `GetWeightCoef` :475-478 |
| `P:\scripts\3_game\entities\entityai.c` | `OnCEIterate` :3893-3911, obsolete `OnCEUpdate` :4924-4925, `GetCachedEquipmentCategory` :322-325 |
| `P:\scripts\3_game\entities\object.c` | `IsInfluencedBySandstorm`, `OnClientSandstormEnter/Leave` :1375-1381 |
| `P:\scripts\3_game\dayzgame.c` | noise calls without weather multiplier :3618/3793/3833; DEVELOPER sandstorm RPCs :3408-3463 |
| `P:\scripts\3_game\worldlighting.c` | Nasdara lighting path :6 |
| `P:\scripts\4_world\systems\sandstorm\scriptedsandstormcontroller.c` | shelter |
| `P:\scripts\4_world\systems\sandstorm\playersandstormdata.c` | per-player state |
| `P:\scripts\4_world\classes\playermodifiers\emodifiers.c` | new IDs :62-69 |
| `P:\scripts\4_world\classes\playermodifiers\modifiersmanager.c` | eyes/heat sync bits :11-12 |
| `P:\scripts\4_world\classes\playermodifiers\modifiers\heatcomfortmdfr.c` | bias feed :74-111 |
| `P:\scripts\4_world\classes\playermodifiers\modifiers\diseases\heatstroke.c` | 4 phases |
| `P:\scripts\4_world\classes\playermodifiers\modifiers\diseases\silicosis.c` | thresholds |
| `P:\scripts\4_world\classes\playermodifiers\modifiers\diseases\irritatedeyes.c` | widget ≥ 15 |
| `P:\scripts\4_world\classes\playermodifiers\modifiers\conditions\sandstormexposure.c` | transmission, cough |
| `P:\scripts\4_world\classes\playermodifiers\modifiers\immunesystem.c` | `NTF_SICK` :44-55 |
| `P:\scripts\4_world\classes\transmissionagents\agents\silicosisagent.c` | die-off |
| `P:\scripts\4_world\classes\transmissionagents\agents\eyesirritation.c` | die-off |
| `P:\scripts\4_world\plugins\pluginbase\plugintransmissionagents.c` | `AGT_AIRBOURNE_SOLID` :414-436, cached protection :577-592 |
| `P:\scripts\4_world\entities\cachedequipmentstorage.c` | category gate :31 |
| `P:\scripts\4_world\entities\itembase.c` | `GetProtectionLevel` config keys :4073-4102 |
| `P:\scripts\4_world\entities\itembase\clothing_base.c` | `Glasses_Base` EYEWEAR :176-179, `Mask_Base` MASK :196-199 |
| `P:\scripts\4_world\entities\itembase\clothing\gp5gasmask.c` | filter-driven protection :13-28 |
| `P:\scripts\4_world\entities\itembase\nvgoggles.c` | NVG category :11-14, lowered eye protection :83-95 |
| `P:\scripts\4_world\classes\thermalbiashandler.c` | Add/resist/save |
| `P:\scripts\4_world\classes\environment\environment.c` | shadow, sun, moto/boat |
| `P:\scripts\4_world\classes\worlds\nasdara.c` | sand weather, sun table, dust |
| `P:\scripts\4_world\classes\worlds\chernarusplus.c` | no sun table, no `StartSandstorm` |
| `P:\scripts\4_world\classes\useractionscomponent\actions\continuous\actionwashheadwettingclothesbase.c` | wash |
| `P:\scripts\4_world\classes\useractionscomponent\actions\continuous\actionwashheaditemcontinuous.c` | frozen bottle |
| `P:\scripts\4_world\classes\useractionscomponent\actions\continuous\actionwetclothinginhandsbase.c` | wet clothes |
| `P:\scripts\4_world\classes\contaminatedarea\oilpitarea.c` | delay 3–9 s |
| `P:\scripts\4_world\classes\contaminatedarea\effectarea.c` | `OnCEIterate` :191-195, `OIL_PIT = 8` :10-16, `SurfaceRoadY` :412-417 |
| `P:\scripts\4_world\classes\contaminatedarea\contaminatedarealoader.c` | JSON area spawn :93-96 |
| `P:\scripts\4_world\entities\scriptedentities\triggers\oilpittrigger.c` | 20 `HeatDamage` |
| `P:\scripts\4_world\classes\dust\dusteffectceilinghandler.c` | ceiling |
| `P:\scripts\4_world\classes\dust\dusteffectsmanager.c` | manager |
| `P:\scripts\4_world\entities\effects\vehicledust.c` | override params |
| `P:\scripts\4_world\entities\effects\effwheelcontact.c` | contact |
| `P:\scripts\4_world\entities\effects\wheelsmoke.c` | obsolete `SetSurface` :32 |
| `P:\scripts\4_world\static\surface.c` | obsolete getters :64, :70 |
| `P:\scripts\4_world\static\sensesaievaluate.c` | obsolete `NoiseAIEvaluate.GetNoiseReduction*` :18, :92 |
| `P:\scripts\4_world\entities\vehicles\components\vehiclevfxcomponent.c` | `GetSurface` :177-185 |
| `P:\scripts\4_world\entities\vehicles\inheritedcars\offroad_02.c` | weight 1.5 |
| `P:\scripts\4_world\entities\vehicles\inheritedcars\truck_01_base.c` | weight 2.0 |
| `P:\scripts\4_world\entities\vehicles\inheritedmotorbikes\motorbike_01.c` / `motorbike_02.c` | weight 0.5 / 0.75 |
| `P:\scripts\4_world\entities\vehicles\boatscript.c` / `motorbikescript.c` | `GetHeatComfortOverride` (0.0) |
| `P:\scripts\4_world\entities\manbase\playerbase.c` | handler ctor :426, `HasDisease` :1119, store :7395/:7520 |
| `P:\scripts\4_world\classes\playersymptoms\states\secondary\squintstate.c` | eyes PPE |
| `P:\DZ\data\config.cpp` | `CfgWorlds > CAWorld > Weather > Sandstorm` :1532-1541 |
| `P:\DZ\data\aiconfigs\config.cpp` | `AIParams` :12-24, `sandstormToNoiseMultiplier` :20 |
| `P:\DZ\characters\masks\config.cpp`, `glasses\config.cpp`, `headgear\config.cpp` | `Protection { dust_particle_* }` values |
| `P:\DZ\worlds\{chernarusplus,enoch,sakhal}\ce\cfggameplay.json` | no `SandstormData` block |
| `P:\graphics\materials\postprocess\sandstorm.emat` | `SnowEffect` :1, colour :29 |
| `P:\graphics\particles\sandstorm\sandstorm.ptc` | storm particle (existence only) |

## 1.29 comparison files (`O:\DayZ 1.29 Старый распакованный клиент\`)

- `scripts\3_game\entities\entityai.c:3869` — `OnCEUpdate` with body; `scripts\4_world\classes\contaminatedarea\effectarea.c:190-194` — `EffectArea` overrode it.
- `scripts\4_world\classes\playermodifiers\modifiers\heatcomfortmdfr.c:51-56, 73-76` — heat damage before 1.30.
- `scripts\3_game\dayzgame.c:3517, 3692, 3730` — noise × `GetNoiseReductionByWeather()`.
- `scripts\4_world\entities\vehicles\carscript.c:1172, 1182` — `EffWheelSmoke.SetSurface` callers.
- `DZ\data\aiconfigs\config.cpp:12-20` — `AIParams` with rain/sea multipliers only.

## Not available on this machine (Exp-only sources)

- Nasdara data PBO (`DZ\data_takistan` `config.cpp`, `basicDefines.hpp`, `aiconfigs\config.cpp`) and the Exp PBO listing: seen on 1.30 Exp 164014 only; not unpacked on `P:\`.
- The 1.30 Exp modding changelog: source of the `[CHANGELOG 1.30]` labels.

## Located but not reviewed

- Eye-veins / squint screen widgets: classes `GameplayEffectsMetaDataEyeVeins` / `GameplayEffectsMetaDataSquinting` (`P:\scripts\5_mission\mission\gameplayeffectwidgets\gewidgetsmetadataeyeveins.c:2`, `gewidgetsmetadatasquinting.c:2`), driven via `EffectWidgetsTypes.EYE_VEINS/SQUINTING` from `irritatedeyes.c` / `squintstate.c` symptoms. Older notes called them `GEWidgetsMetaData…` (that is the file name, not the class).

## Still unverified

- Runtime of everything above (no in-game measurement).
