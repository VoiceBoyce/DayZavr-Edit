# New members on existing classes: DayZ 1.29 → 1.30 (stable 1.30.164078)

<!-- Generated 2026-10-15 from a declaration-level parse of P:\scripts (DayZ 1.30.164078 stable) against the 1.29 extraction (O:\DayZ 1.29 ...\scripts). Paths are relative to P:\scripts. A parser, not the compiler: confirm a line with grep before you quote it. -->


Methods and fields that 1.30 added to classes that already existed in 1.29 (new classes: see `api-delta-1.29-to-1.30.md` §11). `from ancestor` = it existed on an ancestor in 1.29 and was moved down/duplicated.


## 1_core

### `EnumTools` — `1_core/proto/enconvert.c`
- `static string EnumFlagsToString(typename, int)`

### `typename` — `1_core/proto/enconvert.c`
- `static string EnumFlagsToString(typename, int)`

### `vector` — `1_core/proto/enconvert.c`
- `vector Cross(vector)`
- `static void SanitizeMinsMaxs(inout vector, inout vector)`


## 3_game

### `AmmoTypesAPI` — `3_game/global/ammotypes.c`
- `static ExplosiveEffectData GetExplosiveEffectData(string, string)`

### `Boat` — `3_game/vehicles/boat.c`
- `float GetSpeedometer()`
- `float GetSpeedometerAbsolute()`

### `Building` — `3_game/entities/building.c`
- `bool DisableVicinityIcon()` *(was on Object)*
- `AdditionalDoorInfo GetDoorInfo(int)`
- `AdditionalDoorInfosHolder GetDoorInfoHolder()`
- `protected void InitializeDoorInfos()`
- `protected void InitializeInstanceAttachmentGroupsData()`
- `bool IsDetectableByVicinityCone()`
- `bool ShowZonesHealth()` *(was on Object)*
- field `InstancedAttachmentGroupDataBase m_InstancedAttGroupData`

### `CGame` — `3_game/global/game.c`
- `proto native BunkerBroadcastManager GetBunkerBroadcastManager()`
- `proto SurfaceInfo GetSurfaceInfoOn(float, float, out float)`
- `proto SurfaceInfo GetSurfaceInfoUnderObject(notnull Object)`
- `proto SurfaceInfo GetSurfaceInfoUnderObjectByBone(notnull Object, int)`
- `proto native bool IsHeadless()`
- `proto native bool IsHeadlessOrDedicatedServer()`
- `proto native bool SurfaceIsHole(float, float)`

### `CachedEquipmentItemAttribute` — `3_game/entities/cachedequipmentstoragebase.c`
- field `int m_ParentInventoryPlacement` = `InventorySlots.INVALID`

### `CachedEquipmentStorageBase` — `3_game/entities/cachedequipmentstoragebase.c`
- `void UpdateParents()`

### `CachedEquipmentStorageQuery` — `3_game/entities/cachedequipmentstoragebase.c`
- field `TIntArray m_ParentPlacement` = `new TIntArray()`

### `Car` — `3_game/vehicles/car.c`
- `proto native vector WheelGetPositionLS(int)`

### `CfgGameplayHandler` — `3_game/cfggameplayhandler.c`
- `static int GetBunkerBroadcastEnabled()`
- `static bool GetCanDamageAttachedBikeWheels()`
- `static bool GetCanDamageAttachedCarWheels()`
- `static bool GetCanDetachAttachedBikeWheels()`
- `static bool GetCanDetachAttachedCarWheels()`
- `static bool GetDisableBaseDecay()`
- `static bool GetDisableDropAttachmentsOnFall()`
- `static bool GetDisableItemFalling()`
- `static bool GetDisableRefundMaterialsOnFall()`
- `static float GetExternalLockProtectionCountStageOne()`
- `static float GetExternalLockProtectionCountStageTwo()`
- `static float GetExternalLockProtectionResetTime()`
- `static float GetExternalLockProtectionTime()`
- `static float GetSandstormFrequency()`

### `CfgGameplayJson` — `3_game/cfggameplaydatajson.c`
- field `ITEM_ExternalLockData ExternalLockData` = `new ITEM_ExternalLockData`
- field `ITEM_SandstormData SandstormData` = `new ITEM_SandstormData`

### `Colors` — `3_game/colors.c`
- field `int COLOR_GHOST_ICON` = `0xC8C8C8B4`

### `ContentDLC` — `3_game/services/contentdlc.c`
- `proto bool BuyDLC(EDLCId)`
- `private void ContentDLC()`
- `proto EDLCId GetDLCFromWorld(string)`
- `proto string GetLocalizedName(EDLCId)`
- `proto bool IsDLCMounted(EDLCId)`
- `static proto native bool IsValidDLCID(EDLCId)`
- `proto void Subscribe(func)`
- `proto void Unsubscribe(func)`
- `proto bool UseDLC(EDLCId)`
- `private void ~ContentDLC()`

### `DayZGame` — `3_game/dayzgame.c`
- `protected void ForceOvercastReset()`
- field `bool m_IsRoboclient` = `false`

### `DayZPlayer` — `3_game/dayzplayer.c`
- `proto SurfaceInfo GetCurrentSurfaceInfo()`
- `proto native bool IsFirstRenderFrame()`

### `Debug` — `3_game/tools/debug.c`
- `static void AutotestLog(string=LOG_DEFAULT, string=LOG_DEFAULT, string=LOG_DEFAULT, string=LOG_DEFAULT, string=LOG_DEFAULT)`
- `static void RadioGameplayLog(string=LOG_DEFAULT, string=LOG_DEFAULT, string=LOG_DEFAULT, string=LOG_DEFAULT, string=LOG_DEFAULT)`
- field `string LOG_DEBUG_AUTOTEST` = `"Autotest"`
- field `string LOG_DEBUG_RADIO_GAMEPLAY` = `"RadioGameplay"`

### `DynamicMusicPlayerTimeOfDay` — `3_game/systems/dynamicmusicplayer/dynamicmusicplayer.c`
- `static string ToString(int)` *(was on WorldDataDaytime)*
- `static int Translate(int)`

### `EffExhaustSmoke` — `3_game/effects/effectparticle/vehiclesmoke/exhaustsmoke.c`
- `void ParticleParamsOverride(notnull ParticleParamsOverrideData)`

### `EffectParticle` — `3_game/effects/effectparticle.c`
- `protected void AddParticlePropertiesFlag(ParticlePropertiesFlags)`
- `void DisableParticleInUI(bool)`
- `bool IsDisableParticleInUI()`
- `protected bool IsSetParticlePropertiesFlag(ParticlePropertiesFlags)`
- `void ParticleParamsOverride(notnull ParticleParamsOverrideData)`
- `protected void RemoveParticlePropertiesFlag(ParticlePropertiesFlags)`
- `void SelectFromSurface(SurfaceInfo)`
- `void SetEffectSound(EffectSound)`
- `void SetParticleState(int)`
- field `EffectSound m_EffectSound`
- field `int m_PPFlags`

### `EntityAI` — `3_game/entities/entityai.c`
- `bool CanDisplayAttachmentCargo(EntityAI)`
- `bool CanUseConstruction()` *(was on Object)*
- `bool CanUseConstructionBuild()` *(was on Object)*
- `bool CanUseHandConstruction()` *(was on Object)*
- `protected void ConstructionInit()`
- `protected ConstructionBasic CreateConstructionComponent()`
- `bool DestroyOnFall()`
- `void EEFired(int, int, string)`
- `void EEInventoryLocked()`
- `void EEInventoryUnlocked()`
- `void EEItemIntoHands(EntityAI)`
- `void EEItemOutOfHands(EntityAI)`
- `void EERestore()`
- `void EEUserAnimDone()`
- `ConstructionBasic GetConstructionBasic()`
- `protected InstancedAttachmentGroupDataBase GetInstancedAttGroupData()`
- `TypeAttachmentGroupsDataHolder GetTypeAttachmentGroupInfoHolder()`
- `protected void HandleDamageLevelConstruction(int, int, string)`
- `bool HasAnyAttachments()`
- `bool HasAnyAttachmentsOrCargo()`
- `protected void InitializeInstanceAttachmentGroupsData()`
- `bool IsDetectableByVicinityCone()`
- `bool IsFacingCamera(string)`
- `bool IsFacingPlayerEx(DayZPlayer, string)`
- `bool IsPlayerInsideEx(DayZPlayer, string)`
- `void OnCEIterate(float, float)`
- `void OnCustomRebuildDebug()`
- `void OnItemFallRebuilding()`
- `void OverrideActionAnimation(typename, int, int=-1, int=-1)`
- `protected void RegisterConstructionSyncVariables()`
- `void SetActionAnimOverrides()`
- `bool UseCustomRebuildDebug()`

### `EntityAIType` — `3_game/entities/entityaitype.c`
- `ConstructionDataTypeHolderBasic GetConstructionDataHolderBasic()`
- `TypeAttachmentGroupsDataHolder GetTypeAttachmentGroupsDataHolder()`
- `protected void InitConstructionDataTypeHolder(string)`
- `protected void InitTypeAttachmentGroupsData(string)`
- field `bool m_CanDisplayWeight`
- field `float m_ConfigWeight`
- field `TypeAttachmentGroupsDataHolder m_TypeAttachmentGroupsHolder`
- field `ConstructionDataTypeHolderBasic m_TypedConstructionDataHolder`
- field `float m_VarHeatPermeabilityCoef`
- field `float m_VarTemperatureFreezeThreshold`
- field `float m_VarTemperatureFreezeTime`
- field `float m_VarTemperatureInit`
- field `float m_VarTemperatureMax`
- field `float m_VarTemperatureMin`
- field `float m_VarTemperatureOverheatTime`
- field `float m_VarTemperatureThawThreshold`
- field `float m_VarTemperatureThawTime`

### `GameConstants` — `3_game/constants.c`
- field `int AI_SPRINT_STEP_MAX_ID` = `20`
- field `int AI_SPRINT_STEP_MIN_ID` = `5`
- field `float ENVIRO_HEATCOMFORT_BOAT_SPEED_MAX` = `50.0`
- field `float ENVIRO_HEATCOMFORT_BOAT_SPEED_MIN` = `15.0`
- field `float ENVIRO_HEATCOMFORT_BOAT_TARGET_MAX` = `0.8`
- field `float ENVIRO_HEATCOMFORT_MOTORBIKE_SPEED_MAX` = `100.0`
- field `float ENVIRO_HEATCOMFORT_MOTORBIKE_SPEED_MIN` = `20.0`
- field `float ENVIRO_HEATCOMFORT_MOTORBIKE_TARGET_MAX` = `0.8`
- field `float ENVIRO_TICK_SHADOW_RC_CHECK` = `30`
- field `float MOTORBIKES_CONTACT_DMG_KILLCREW` = `80.0`
- field `float MOTORBIKES_CONTACT_DMG_MIN` = `10.0`
- field `float MOTORBIKES_CONTACT_DMG_THRESHOLD_MID` = `40.0`
- field `float MOTORBIKES_CONTACT_DMG_THRESHOLD_SMALL` = `25.0`
- field `float SANDSTORM_NOISE_REDUCTION_WEIGHT` = `0.5`
- field `int VEHICLE_FLUIDS_TICK` = `1`

### `GetServersInput` — `3_game/services/bioslobbyservice.c`
- `void SetDLCs(int)`
- field `int m_DLCs`
- field `bool m_UseDLCs`

### `GetServersResultRow` — `3_game/services/bioslobbyservice.c`
- field `int m_DLCs`

### `Human` — `3_game/human.c`
- `proto native bool PhysicsIsRagdoll()`
- `proto native void PhysicsSetSimpleDeath(bool)`
- `proto native void SetAnimationInstanceByName(string, float)`

### `HumanAnimInterface` — `3_game/human.c`
- `proto native bool IsTag(TAnimGraphTag)`

### `HumanCommandUnconscious` — `3_game/human.c`
- `proto native bool IsRagdoll()`

### `HumanCommandVehicle` — `3_game/human.c`
- `bool IsTransitioning()`

### `HumanInputController` — `3_game/human.c`
- `proto native void DisableCrouchCameratHorizontalRotation(bool)`
- `proto native void DisableErectCameratHorizontalRotation(bool)`
- `proto native void DisableProneCameraHorizontalRotation(bool)`
- `proto native int GetCrouchSpeedLimit()`
- `proto native int GetErectSpeedLimit()`
- `proto native int GetProneSpeedLimit()`
- `proto native bool IsCrouchCameraHorizontalRotationDisabled()`
- `proto native bool IsErectCameraHorizontalRotationDisabled()`
- `proto native bool IsProneCameraHorizontalRotationDisabled()`
- `proto native void SetCrouchSpeedLimit(bool, int=DayZPlayerConstants.MOVEMENTIDX_SPRINT)`
- `proto native void SetErectSpeedLimit(bool, int=DayZPlayerConstants.MOVEMENTIDX_SPRINT)`
- `proto native void SetProneSpeedLimit(bool, int=DayZPlayerConstants.MOVEMENTIDX_SPRINT)`

### `HumanInventory` — `3_game/systems/inventory/humaninventory.c`
- `bool ExecuteUserReservationCancelRemote(inout Serializer)`

### `HumanItemAccessor` — `3_game/humanitems.c`
- `proto native void EnableAutoAnimInstUpdateOnHandsChange(bool)`
- `proto native bool IsAutoAnimInstUpdateOnHandsChangeEnabled()`

### `ITEM_ConstructionData` — `3_game/cfggameplaydatajson.c`
- field `bool disableDropAttachmentsOnFall`
- field `bool disableItemFalling`
- field `bool disableRefundMaterialsOnFall`

### `ITEM_GeneralData` — `3_game/cfggameplaydatajson.c`
- field `bool disableBaseDecay`

### `ITEM_VehicleData` — `3_game/cfggameplaydatajson.c`
- field `bool canDamageAttachedBikeWheels` = `true`
- field `bool canDamageAttachedCarWheels` = `true`
- field `bool canDetachAttachedBikeWheels` = `true`
- field `bool canDetachAttachedCarWheels` = `true`

### `ITEM_WorldData` — `3_game/cfggameplaydatajson.c`
- field `bool bunkerBroadcastEnabled`

### `ImpactMaterials` — `3_game/impacteffects.c`
- field `int MUD_BRICK` = `RegisterSurface("Hit_Mud_Brick")`

### `InventoryItemType` — `3_game/inventoryitemtype.c`
- `string GetGhostIconImagepath()`
- `protected void LoadAdditionalInfo()`
- field `string m_AlternateGhostIcon` = `""`

### `JsonUndergroundAreaBreadcrumb` — `3_game/undergroundarealoader.c`
- field `string Comment`

### `JsonUndergroundAreaTriggerData` — `3_game/undergroundarealoader.c`
- field `string Comment`

### `LogManager` — `3_game/tools/debug.c`
- `static bool IsRadioGameplayLogEnabled()`
- field `bool m_DoRadioGameplayLog`

### `Man` — `3_game/entities/man.c`
- `bool IsSurrendered()`
- `void OnItemInHandsChangedEx(bool=false, bool=true)`

### `ModInfo` — `3_game/modinfo.c`
- `proto EDLCId GetDLCID()`

### `NoiseSystem` — `3_game/noise.c`
- `proto native float GetEnvironmentNoiseReduction(vector)`

### `Object` — `3_game/entities/object.c`
- `ELiquidSourceObjectType GetLiquidSourceObjectType()`
- `bool IsExplosive()`
- `bool IsInfluencedBySandstorm()`
- `void OnClientSandstormEnter()`
- `void OnClientSandstormLeave()`

### `PPEGlow` — `3_game/ppemanager/materials/matclasses/ppeglow.c`
- field `int L_20_SANDSTORM` = `50`
- field `int L_21_SANDSTORM_NVG` = `50`
- field `int L_22_SANDSTORM_NVG` = `50`

### `PPEGodRays` — `3_game/ppemanager/materials/matclasses/ppegodrays.c`
- field `int L_0_SANDSTORM` = `200`

### `PPELightIntensityParamsNative` — `3_game/ppemanager/materials/matclasses/exceptions/ppelightintensityparamsnative.c`
- field `int L_0_SANDSTORM` = `50`

### `PPERequesterBank` — `3_game/ppemanager/pperequesterbank.c`
- `static bool IsRequesterRunning(int)`
- field `int REQ_SANDSTORMEFFECT` = `RegisterRequester(PPERequester_SandstormEffect)`
- field `int REQ_SQUINT` = `RegisterRequester(PPERequester_SquintEffects)`

### `PPERequester_CameraNV` — `3_game/ppemanager/requesters/ppercameranv.c`
- `protected void ApplyGlassesProfile(float, float)`
- `protected void ApplyOpticsProfile(float, float)`
- `protected void ApplyPumpkinProfile(float, float)`
- `protected float ComputeSafeExposure(float)`
- field `int NV_DAYTIME_GLASSES` = `9`
- field `int NV_DAYTIME_PUMPKIN` = `13`
- field `float NV_EXPOSURE_MAX` = `7.0`
- field `int NV_SANDSTORM_GLASSES` = `11`
- field `int NV_SANDSTORM_OPTICS` = `7`
- field `int NV_SANDSTORM_PUMPKIN` = `15`
- field `int NV_UNDERGROUND_GLASSES` = `10`
- field `int NV_UNDERGROUND_OPTICS` = `6`
- field `int NV_UNDERGROUND_PUMPKIN` = `14`
- field `int NV_UNDERGROUND_SANDSTORM_GLASSES` = `12`
- field `int NV_UNDERGROUND_SANDSTORM_OPTICS` = `8`
- field `int NV_UNDERGROUND_SANDSTORM_PUMPKIN` = `16`

### `PPERequester_UnconEffects` — `3_game/ppemanager/requesters/pperunconeffects.c`
- `void FadeInEffect(float, float=FADING_TIME_DEFAULT)`
- `protected void ProcessFadeIn(float)`
- field `bool m_FadeIn`
- field `float m_FadeInStartIntensity`
- field `float m_FadeInTargetIntensity`

### `ParticleBase` — `3_game/particles/particlebase.c`
- `string GetStartSoundName()`
- `void PlayStartSound()`
- `void SetStartSoundParams(string, vector)`
- `void ~ParticleBase()`
- field `vector m_SoundPosition`
- field `string m_StartSoundSetName`

### `ParticleEffecter` — `3_game/effectmanager.c`
- `protected typename GetEffectTypename()`

### `ParticleList` — `3_game/particles/particlelist.c`
- field `int BUILDNG_DESTRUCT_BRICK` = `RegisterParticle("building_destruction_brick")`
- field `int BUILDNG_DESTRUCT_METAL` = `RegisterParticle("building_destruction_metal")`
- field `int BUILDNG_DESTRUCT_WOOD` = `RegisterParticle("building_destruction_wood")`
- field `int CEILING_WOOD` = `RegisterParticleByFullPath("graphics/particles/dust/dust_ceiling_wood")`
- field `int DUST_CLOUD` = `RegisterParticle("dust/dust_effect_01")`
- field `int DUST_GROUND` = `RegisterParticle("dust/dust_grnd_01")`
- field `int DUST_VEHICLE` = `RegisterParticle("dust_vehicle")`
- field `int DUST_VEHICLE_SOFT` = `RegisterParticle("dust_vehicle_soft")`
- field `int EXPLOSION_GRENADE_DESERT_SAND` = `RegisterParticle("explosion_grenade_desert_sand")`
- field `int EXPLOSION_GRENADE_SAND` = `RegisterParticle("explosion_grenade_sand")`
- field `int IMPACT_DESERT_SAND_ENTER` = `RegisterParticle("impacts/hit_desert_sand_ent_01")`
- field `int IMPACT_DESERT_SAND_EXIT` = `RegisterParticle("impacts/hit_desert_sand_ext_01")`
- field `int IMPACT_DESERT_SAND_RICOCHET` = `RegisterParticle("impacts/hit_desert_sand_ric_01")`
- field `int IMPACT_MUD_BRICK_ENTER` = `RegisterParticle("impacts/hit_mud_brick_ent_01")`
- field `int IMPACT_MUD_BRICK_EXIT` = `RegisterParticle("impacts/hit_mud_brick_ext_01")`
- field `int IMPACT_MUD_BRICK_RICOCHET` = `RegisterParticle("impacts/hit_mud_brick_ric_01")`
- field `int OIL_FIRE1` = `RegisterParticle("fire_oil1")`
- field `int OIL_FIRE2` = `RegisterParticle("fire_oil2")`
- field `int TANDOOR_HOUSE_FIRE_END` = `RegisterParticle("tandoor_fire_small_house_01_end")`
- field `int TANDOOR_HOUSE_FIRE_STEAM_2END` = `RegisterParticle("tandoor_steam_medium_house_2end")`
- field `int TANDOOR_HOUSE_NORMAL_FIRE` = `RegisterParticle("tandoor_fire_small_house_01")`
- field `int TANDOOR_HOUSE_NORMAL_SMOKE` = `RegisterParticle("tandoor_smoke_medium_house_01")`
- field `int TANDOOR_HOUSE_SMALL_FIRE` = `RegisterParticle("tandoor_fire_small_house_01_start")`
- field `int TANDOOR_HOUSE_SMALL_SMOKE` = `RegisterParticle("tandoor_smoke_small_house_01")`
- field `int VEHICLE_WHEEL_DESERT` = `RegisterParticle("vehicle_wheel_desert")`
- field `int VEHICLE_WHEEL_SOIL` = `RegisterParticle("vehicle_wheel_soil")`

### `ParticleProperties` — `3_game/particles/particlemanager/particlesource.c`
- `proto bool IsDisabledInUI()`

### `PlayerConstants` — `3_game/playerconstants.c`
- field `float CHANCE_TO_BLEED_HAND_CONSTRUCTION` = `0.5`
- field `float GLOVES_DAMAGE_HAND_CONSTRUCTION` = `-10`
- field `float PARTICLE_STEP_EFFECT_VELOCITY_THRESHOLD` = `5.0`
- field `float THERMAL_BIAS_POSITIVE_HC_DECREMENT` = `0.0001`
- field `float THERMAL_BIAS_POSITIVE_HC_HIGH_INCREMENT` = `0.00050`
- field `float THERMAL_BIAS_POSITIVE_HC_LOW_INCREMENT` = `0.00022`
- field `float THERMAL_BIAS_POSITIVE_HC_MEDIUM_INCREMENT` = `0.00032`
- field `float THERMAL_BIAS_WASH_HEAD_DECREMENT` = `0.175`
- field `float THERMAL_BIAS_WASH_HEAD_TEMPORARY_RESISTANCE_TIME` = `30.0`

### `SEffectManager` — `3_game/effectmanager.c`
- `static EffecterBase GetEffecterByID(int)`

### `SimpleCircularBuffer` — `3_game/tools/simplecircularbuffer.c`
- `void Resize(int, T)`

### `SoundConstants` — `3_game/constants.c`
- field `int ITEM_ARMY_FLASHLIGHT_TURN_OFF` = `61`
- field `int ITEM_ARMY_FLASHLIGHT_TURN_ON` = `60`
- field `int ITEM_CARWHEEL_EXPLOSION_BIG` = `51`
- field `int ITEM_CARWHEEL_EXPLOSION_SMALL` = `50`
- field `int ITEM_COMBINATION_LOCK_CHANGE_DIAL` = `33`
- field `int ITEM_COMBINATION_LOCK_CHANGE_NUMBER` = `32`
- field `int ITEM_COMBINATION_LOCK_LOCK` = `30`
- field `int ITEM_COMBINATION_LOCK_UNLOCK` = `31`
- field `int ITEM_COMBINATION_LOCK_WRONG_COMBINATION` = `35`
- field `int ITEM_COMBINATION_LOCK_WRONG_PIN` = `34`
- field `int ITEM_DIGITAL_LOCK_ACCESS` = `40`
- field `int ITEM_DIGITAL_LOCK_CLEAR_INPUT` = `45`
- field `int ITEM_DIGITAL_LOCK_CONFIRM_INPUT` = `44`
- field `int ITEM_DIGITAL_LOCK_DIGIT_INPUT` = `46`
- field `int ITEM_DIGITAL_LOCK_ERROR` = `43`
- field `int ITEM_DIGITAL_LOCK_LOCK` = `41`
- field `int ITEM_DIGITAL_LOCK_UNLOCK` = `42`

### `SurfaceInfo` — `3_game/surfaceinfo.c`
- `proto int GetCeilingDustParticleId()`
- `proto int GetCloudEffectParticleId()`
- `proto string GetCloudEffectSoundSetName()`
- `proto int GetDustKickupEffectBigId()`
- `proto int GetDustKickupEffectId()`
- `proto int GetDustKickupEffectSmallId()`
- `proto int GetGroundEffectParticleId()`
- `proto string GetGroundEffectSoundSetName()`
- `proto string GetStepSoundSetName()`
- `proto int GetVehicleDustParticleId()`
- `proto string GetVehicleDustSoundSetName()`
- `proto int GetWheelContactParticleId()`
- `proto string GetWheelContactSoundSetName()`
- `proto string GetWheelSoundSetName()`

### `Transport` — `3_game/vehicles/transport.c`
- `bool CanGetIn()`
- `bool CanPutIntoHands(EntityAI)` *(was on EntityAI)*
- `protected void CheckVitalItemCallback(bool, int, Object=null, string="", Param=null)`
- `void CleanupEffects()`
- `protected EffectSound CreateSoundForAnimationSource(string)`
- `bool CrewCanEject(int)`
- `bool CrewShouldEject(int)`
- `void EEDelete(EntityAI)` *(was on EntityAI)*
- `void FillScript(ETransportFluid, float)`
- `string GetAnimSourceFromSelection(string)`
- `protected bool GetCrashHeavySound()`
- `protected bool GetCrashLightSound()`
- `EntityAI GetElectricitySourceDevice()`
- `string GetElectricitySourceDeviceType()`
- `int GetFluidCapacityScript(ETransportFluid)`
- `float GetFluidFractionScript(ETransportFluid)`
- `int GetSeatAnimationTypeDirectional(int, int)`
- `protected string GetSelectionFromAnimSource(string)`
- `Managed GetVehicleHornComponent()`
- `float GetWeightCoef()`
- `bool HasDirectionalInOutAction()`
- `bool IsAreaAtDoorFreeDiag(int, float, inout vector, out vector[])`
- `bool IsDetectableByVicinityCone()`
- `bool IsVitalFuelTank()`
- `bool IsVitalGlowPlug()`
- `bool NeedElectricitySourceDevice()`
- `protected void PartsHealthCheck()`
- `protected void PlayCrashHeavySound()`
- `protected void PlayCrashLightSound()`
- `void PlaySound(string, EffectSound, out bool)` *(was on Object)*
- `protected void PlaySoundEx(string, EffectSound, out bool)`
- `protected void SynchCrashHeavySound(bool)`
- `protected void SynchCrashLightSound(bool)`
- `void UpdateBattery(IEntity)`
- `void ~Transport()`
- field `EVehicleDebugOutputType DEBUG_OUTPUT_TYPE`
- field `VehicleContactCache m_ContactCache`
- field `bool m_ContactCalled`
- field `VehicleContactData m_ContactData`
- field `EffectSound m_CrashSoundHeavy`
- field `EffectSound m_CrashSoundLight`
- field `float m_CrewContactDamageCoef` = `0.023`
- field `bool m_EngineDestroyed`
- field `bool m_EngineStarted`
- field `int m_HeadlightsState`
- field `float m_MomentumPrevTick`
- field `bool m_PlayCrashSoundHeavy`
- field `bool m_PlayCrashSoundLight`
- field `int m_RearLightType`
- field `float m_VehicleContactDamageCoef` = `0.12`
- field `vector m_VelocityPrevTick`
- field `float m_dmgContactCoef` = `0.023`

### `UAInterface` — `3_game/inputapi/uainput.c`
- `proto bool SyncedGetInputOverride(int, OverrideInputType, bool, out float)`
- `proto native bool SyncedOverride(string, float, OverrideInputType, bool)`
- `proto native bool SyncedOverride_ID(int, float, OverrideInputType, bool)`
- `proto native bool SyncedRemoveAllOverrides()`
- `proto native bool SyncedRemoveOverride(string)`
- `proto native bool SyncedRemoveOverride_ID(int)`

### `Weather` — `3_game/weather.c`
- `float GetNoiseReductionByWeatherEx(notnull Object)` *(obsolete)*
- `private float GetRainNoiseReduction()`
- `proto native Sandstorm GetSandstorm()`
- `private float GetSandstormNoiseReduction(notnull Object)`
- `private float GetSnowfallNoiseReduction()`

### `World` — `3_game/global/world.c`
- `proto native vector GetSunOrMoonDirection()`
- `proto native ThirdPersonMode GetThirdPersonViewMode()`
- `proto native bool UpdatePathgraphEntityState(notnull Object)`

### `WorldData` — `3_game/worlddata.c`
- `protected int DayOfYear(int, int)`
- `DustEffectsSettings GetDustEffectsSettings()`
- `float GetSunEffectOnTemperature(int)`
- `protected float GetSunriseTime(float)`
- `protected float GetSunsetTime(float)`
- `protected void OnSandstormStart(bool)`
- `protected void OnSandstormStop(bool)`
- `protected void SetupDustEffects()`
- `protected void StartSandstorm(float)`
- `void WeatherSandstormStart(bool)`
- `void WeatherSandstormStop(bool)`
- field `DustEffectsSettings m_DustEffectsSettings`
- field `bool m_IsSandstorm`
- field `bool m_IsSandstormStartedByWeather`
- field `int m_LongestDayOfYear` = `1`
- field `int m_ShortestDayOfYear` = `1`

### `WorldDataDaytime` — `3_game/worlddata.c`
- `static bool IsInRange(int, TIntArray)`
- field `int AFTERNOON` = `6`
- field `int EVENING` = `7`
- field `int MORNING` = `4`
- field `int NOON` = `5`

### `WorldDataWeatherSettings` — `3_game/worlddata.c`
- field `float m_OvercastEffectMaxLimitOnSunExposure` = `1`
- field `float m_OvercastEffectMinLimitOnSunExposure` = `0.6`
- field `float m_SandstormMaxWindDirection` = `-1.20`
- field `float m_SandstormMinWindDirection` = `-1.77`
- field `float m_SandstormMinWindMagnitude` = `18`
- field `float m_sandstormFrequency` = `1`

### `WorldLighting` — `3_game/worldlighting.c`
- field `string lighting_nasdara` = `"DZ\\worlds\\nasdara\\data\\lighting\\lighting_nasdara.txt"`


## 4_world

### `ActionAttachOnProxy` — `4_world/classes/useractionscomponent/actions/singleuse/actionattachonproxy.c`
- `protected void AttachItem(AttachActionData)` *(was on ActionAttach)*
- `protected bool CanAttachToProxy(PlayerBase, EntityAI, ItemBase, int=-1)`

### `ActionAttachToConstruction` — `4_world/classes/useractionscomponent/actions/singleuse/actionattachtoconstruction.c`
- `protected int FindAndTranslateAttachmentSlot(PlayerBase, ActionTarget, ItemBase)`
- `bool Post_SetupAction(ActionData)` *(was on ActionBase)*

### `ActionBase` — `4_world/classes/useractionscomponent/actionbase.c`
- `bool CanBeStarted()`
- `ActionInfoDataBase GetClientActionInfo()`
- `void InitInfoData()`
- `bool IsTargetInfoAction()`
- field `ActionInfoDataBase m_InfoData`

### `ActionBuildPart` — `4_world/classes/useractionscomponent/actions/continuous/actionbuildpart.c`
- `protected void ClearInfoData()`
- `protected void ComposeBuildActionDataClient(PlayerBase, ActionTarget, ItemBase, out ActionData, Param=NULL)`
- `protected bool GatherInfoData(PlayerBase, ActionTarget, ItemBase)`
- `protected ConstructionPart GetTargetPart(ConstructionActionData)`
- `void InitInfoData()`
- `protected bool PositioningCheck(PlayerBase, ActionTarget)`
- `protected void SetBuildingAnimationEx(PlayerBase, ActionTarget, ItemBase, ActionData, Param=NULL)`

### `ActionDismantlePart` — `4_world/classes/useractionscomponent/actions/continuous/actiondismantlepart.c`
- `protected void DbgPrintFailure(ConstructionPart, string)`

### `ActionDrinkPondContinuous` — `4_world/classes/useractionscomponent/actions/continuous/actiondrinkpondcontinuous.c`
- `bool IsLockTargetOnUse()` *(was on ActionBase)*

### `ActionGetOutTransport` — `4_world/classes/useractionscomponent/actions/interact/actiongetouttransport.c`
- field `float MOTO_JUMPOUT_THRESHOLD` = `10.0`

### `ActionLockAttachment` — `4_world/classes/useractionscomponent/actions/continuous/actionlockattachment.c`
- field `string CODE_LOCK_SELECTION_BACK` = `"back"`

### `ActionManagerClient` — `4_world/classes/useractionscomponent/actionmanagerclient.c`
- `int GetActionState()` *(was on ActionManagerBase)*

### `ActionTarget` — `4_world/classes/useractionscomponent/actiontargets.c`
- `int GetSurfaceLiquidType()`
- `string GetSurfaceName()`
- field `int m_SurfaceLiquidType` = `LIQUID_NONE`
- field `string m_SurfaceName`

### `ActionTargets` — `4_world/classes/useractionscomponent/actiontargets.c`
- field `float c_UtilityValueCoef` = `0.35`
- field `SurfaceInfo m_SurfaceInfo`

### `ActionWashHandsWater` — `4_world/classes/useractionscomponent/actions/continuous/actionwashhandswater.c`
- `bool IsLockTargetOnUse()` *(was on ActionBase)*
- `void OnEnd(ActionData)` *(was on ActionBase)*
- `void OnStart(ActionData)` *(was on ActionBase)*

### `AirborneMask` — `4_world/entities/itembase/clothing/airbornemask.c`
- `ECachedEquipmentItemCategory GetCachedEquipmentCategory()` *(was on EntityAI)*
- `float GetProtectionLevel(int, bool=false, int=0)` *(was on ItemBase)*

### `Ammo_40mm_Explosive` — `4_world/entities/itembase/magazine/ammunitionpiles.c`
- `bool IsExplosive()` *(was on ItemBase)*

### `AnimalBase` — `4_world/entities/creatures/animals/animalbase.c`
- `protected void HandleCeilingDustStepEffect(SurfaceInfo)`
- `bool TrackNaturalInstinctsAchievement()`
- field `DustEffectCeilingHandlerLocal m_CeilingDustHandler`

### `Animal_CapreolusCapreolus` — `4_world/entities/creatures/animals/animalbase.c`
- `bool TrackNaturalInstinctsAchievement()`

### `Animal_CervusElaphus` — `4_world/entities/creatures/animals/animalbase.c`
- `bool TrackNaturalInstinctsAchievement()`

### `Animal_RangiferTarandus` — `4_world/entities/creatures/animals/animalbase.c`
- `bool TrackNaturalInstinctsAchievement()`

### `BarrelHoles_ColorBase` — `4_world/entities/itembase/fireplacebase/barrelholes_colorbase.c`
- `void OnItemFallRebuilding()`

### `BaseBuildingBase` — `4_world/entities/itembase/basebuildingbase.c`
- `protected ConstructionBasic CreateConstructionComponent()`
- `protected void DestroyConstructedParts()`
- `void DropAttachmentAndCargoItems()`
- `ConstructionBasic GetConstructionBasic()`
- `protected void HandleFallingForConstructedParts()`
- `void OnItemFallRebuilding()`

### `Battery9V` — `4_world/entities/itembase/battery9v.c`
- field `int SLOT_ID` = `InventorySlots.GetSlotIdFromString("BatteryD")`

### `BatteryCharger` — `4_world/entities/itembase/batterycharger.c`
- `void EEInit()` *(was on EntityAI)*
- field `string SEL_LIGHT_CHARGED_ON` = `"light_charged_on"`
- field `string SEL_LIGHT_CHARGING_ON` = `"light_charging_on"`
- field `string SEL_STANDBY_LIGHT` = `"light_stand_by_on"`
- field `string SEL_SWITCH_LIGHT_ON` = `"light_switch_on"`

### `BearTrap` — `4_world/entities/itembase/trapbase/trap_bear.c`
- `void OnTriggeredByFall(Contact)`
- `void OnTriggeredByFallEffectsClient()`

### `BoatScript` — `4_world/entities/vehicles/boatscript.c`
- `void FillScript(ETransportFluid, float)`
- `int GetFluidCapacityScript(ETransportFluid)`
- `float GetFluidFractionScript(ETransportFluid)`
- `float GetHeatComfortOverride()`
- field `float m_HeatComfortOverride`

### `Bot` — `4_world/systems/bot/bot.c`
- `void ChangeStateMachine(int)`
- `void CheckForEnemies(bool=false)`
- `void GetRespawnData(out bool, out int)`
- `int GetRespawnTime()`
- `bool GetShouldRespawn()`
- `void GetStateMachines()`
- `private bool IsEntityEnemy(EntityAI)`
- `void RegisterStateMachineDiag()`
- `void Restart()`
- `void ~Bot()`
- field `BotEventBase __Atkd__` = `new BotEventAttacked`
- field `BotEventBase __Obst__` = `new BotEventObstacle`
- field `BotEventBase __VaFw__` = `new BotEventValidForward`
- field `bool m_Active` = `true`
- field `BotFSM m_ActiveFSM` = `null`
- field `BotStateBase m_Attack`
- field `vector m_InfectedHalfRange` = `"2 0 2"`
- field `BotStateBase m_MoveForward`
- field `EntityAI m_Target` = `null`
- field `BotStateBase m_TurnValidForward`

### `BoxDraw` — `4_world/plugins/pluginbase/plugindiagmenu/undergrounddiag.c`
- `private void DrawDebugs(bool)`
- `ScriptInvoker Event_OnTransformChanged()`
- `vector GetCenter()`
- `vector GetOrientation()`
- `vector GetSize()`
- `UndergroundTriggerHandle GetTriggerHandle()`
- `private void OnCenterGizmoTransformChanged(vector[])`
- `private void OnMaxGizmoTransformChanged(vector[])`
- `private void OnMinGizmoTransformChanged(vector[])`
- `void Select()`
- `void SetAllowTransform(bool)`
- `private void UpdateMaxGizmo()`
- `private void UpdateMinGizmo()`
- field `BoxDrawCenterGizmo m_GizmoCenter`
- field `BoxDrawCornerGizmo m_GizmoMax`
- field `BoxDrawCornerGizmo m_GizmoMin`
- field `bool m_IsSelected`
- field `bool m_IsShown`
- field `vector m_LocalMax`
- field `vector m_LocalMin`
- field `ScriptInvoker m_OnTransformChanged`

### `BuildingBase` — `4_world/entities/game/super/building.c`
- `bool CanDisplayAttachmentCategory(string)` *(was on EntityAI)*
- `bool CanDisplayAttachmentSlot(int)` *(was on EntityAI)*
- `bool CanDoorBeOpened(notnull DoorManipulationParams)` *(was on Building)*
- `bool CanReceiveAttachment(EntityAI, int)` *(was on EntityAI)*
- `bool CanReleaseAttachment(EntityAI)` *(was on EntityAI)*
- `protected ConstructionBasic CreateConstructionComponent()`
- `bool DoorsDirectionalCheck(PlayerBase, int)`
- `DigitalCodeLock GetCodeLock(string)`
- `CombinationLock GetCombinationLock(string)`
- `ConstructionBasic GetConstructionBasic()`
- `protected InstancedAttachmentGroupDataBase GetInstancedAttGroupData()`
- `protected bool HasConflictingDoorAttachment(string, string, string)`
- `bool IsLocked(int)`
- `void OnDoorCloseStart(DoorStartParams)` *(was on Building)*
- `void OnDoorOpenStart(DoorStartParams)` *(was on Building)*
- `protected void RegisterConstructionSyncVariables()`
- field `string ATTACHMENT_SLOT_CODE_LOCK` = `"Att_CodeLock"`
- field `string ATTACHMENT_SLOT_COMBINATION_LOCK` = `"Att_CombinationLock"`
- field `ConstructionBasic m_Construction`

### `BuildingWithFireplace` — `4_world/entities/building/buildingwithfireplace.c`
- `string GetOvenType(int)`

### `CableReel` — `4_world/entities/itembase/cablereel.c`
- `void OnItemFallRebuilding()`

### `CachedEquipmentStorageInventoryEntry` — `4_world/entities/cachedequipmentstorage.c`
- field `ECachedEquipmentPlacement m_InventoryPlacement` = `ECachedEquipmentPlacement.ANY`

### `CarLightBase` — `4_world/entities/scriptedlightbase/spotlightbase/carlightbase.c`
- `void SetLightProfile(notnull VehicleLightProfileBase)`
- field `VehicleLightProfileBase m_LightProfile`

### `CarScript` — `4_world/entities/vehicles/carscript.c`
- `void EOnPostFrame(IEntity, int)` *(was on IEntity)*
- `void FillScript(ETransportFluid, float)`
- `int GetFluidCapacityScript(ETransportFluid)`
- `float GetFluidFractionScript(ETransportFluid)`
- `VehicleHornComponent GetVehicleHornComponent()`
- `bool HasGangedTailAndBrakeLights()`
- `protected void InitializeParticles()`
- `protected void PartsHealthCheck()`
- field `DebugTextWorldSpace m_DebugText`
- field `VehicleHornComponent m_HornComponent`
- field `VehicleLightsComponent m_LightsComponent`
- field `VehicleVFXComponent m_VFXComponent`

### `CarWheel` — `4_world/entities/core/inherited/inventoryitem.c`
- `bool CanDetachAttachment(EntityAI)` *(was on EntityAI)*
- `bool EEOnDamageCalculated(TotalDamageResult, int, EntityAI, int, string, string, vector, float)` *(was on Object)*
- `void InitItemSounds()` *(was on ItemBase)*
- field `string WHEEL_ATTACH_SOUND` = `"Offroad_Wheel_Attach_SoundSet"`
- field `string WHEEL_DETACH_SOUND` = `"Offroad_Wheel_Remove_SoundSet"`

### `CarWheelType` — `4_world/entities/core/inherited/inventoryitemtype.c`
- `void CarWheelType()`
- `TStringArray GetExcludedReplacementTypes()`
- `protected void SetReplacementTypes()`
- field `string m_NormalType`
- field `string m_RuinedType`

### `CarWheel_Ruined` — `4_world/entities/core/inherited/inventoryitem.c`
- `void InitItemSounds()` *(was on ItemBase)*
- `void PlayWheelExlosionSound()`
- field `string WHEEL_EXPLOSION_BIG_SOUND` = `"tire_big_explosion_SoundSet"`
- field `string WHEEL_EXPLOSION_SMALL_SOUND` = `"tire_small_explosion_SoundSet"`

### `CivilianSedan` — `4_world/entities/vehicles/inheritedcars/civiliansedan.c`
- `EntityAI GetElectricitySourceDevice()`
- `string GetElectricitySourceDeviceType()`
- `bool NeedElectricitySourceDevice()`

### `ClaymoreMine` — `4_world/entities/explosivesbase/claymoremine.c`
- `protected void ClearIsFalling()`
- `void EOnContact(IEntity, Contact)` *(was on ItemBase)*
- `void OnItemFallRebuilding()`
- `protected void OnTriggeredByFall(Contact)`
- field `bool m_FallTriggered` = `false`
- field `bool m_IsFalling` = `false`

### `CombinationLock` — `4_world/entities/itembase/combinationlock.c`
- `void AnimateDigits(string)` *(obsolete)*
- `protected void AnimateDigitsInside(string)`
- `protected void AnimateDigitsOutside(string)`
- `void EEItemLocationChanged(notnull InventoryLocation, notnull InventoryLocation)` *(was on ItemBase)*
- `int GetCombinationInside()`
- `int GetDialIndexInside()`
- `static float GetDigitRotation(int)`
- `int GetLockSideByComponentIndex(int)`
- `void InitItemSounds()` *(was on ItemBase)*
- `bool IsExternalLockType()`
- `bool IsLockedOnParent()`
- `void SetCombinationInside(int, bool=true)`
- `void SetDigitRotationAnimationPhase(string, float)` *(obsolete)*
- `protected void SetDigitRotationAnimationPhaseInside(string, float)`
- `protected void SetDigitRotationAnimationPhaseOutside(string, float)`
- `protected void ShuffleCombination(string, bool=false)`
- `void Unlock()` *(obsolete)*
- `void UnlockOnServer(EntityAI, EntityAI, bool=false)`
- `protected void UpdateLockDigits()`
- field `int FACING_INSIDE` = `1`
- field `int FACING_OUTSIDE` = `0`
- field `string LOCK_SELECTION_INSIDE` = `"lock_attached_inside"`
- field `string LOCK_SELECTION_OUTSIDE` = `"lock_attached_outside"`
- field `string SOUND_LOCK_WRONG_PIN` = `"combinationlock_wrong_combination_SoundSet"`
- field `int m_CombinationInside`
- field `int m_DialIndexInside`
- field `int m_LockDigitsCount`

### `Construction` — `4_world/classes/basebuilding/construction.c`
- `void BuildPartServerEx(Man, string, int)`
- `void DismantlePartServerEx(Man, string, int)`
- `protected typename GetDefaultPartInstanceTypename()`
- `void OnHealthLevelChanged(int, int, string)`

### `ConstructionActionData` — `4_world/classes/basebuilding/constructionactiondata.c`
- `ConstructionPart GetBuildPartHandsGenericAtIndex(int)`
- `ConstructionPart GetBuildPartInfoAtIndex(int)`
- `ConstructionPart GetBuildPartInstantAtIndex(int)`
- `int GetCombinationLockSide()`
- `int GetFastConstructionMode()`
- `void Invalidate()`
- `void OnUpdateActionsHandsGeneric(Object, Object, int)`
- `void OnUpdateActionsInstant(Object, Object, int)`
- `void OnUpdateInfoActions(Object, Object, int)`
- `void SetCombinationLockSide(int)`
- `void SetFastConstructionMode(int)`
- `protected void Validate()`
- `protected void ValidateConstructionData(Object, Object, int)`
- field `ActionVariantManager m_ActionHandsGenericVariantManager`
- field `ActionVariantManager m_ActionInfoContinuousVariantManager`
- field `ActionVariantManager m_ActionVariantManagerInstant`
- field `int m_CombinationLockSide` = `-1`
- field `int m_FastConstructionMode`
- field `bool m_IsValid`

### `ConstructionPart` — `4_world/classes/basebuilding/constructionpart.c`
- `protected int CalculateBitMaskOnBuild(int)`
- `protected int CalculateBitMaskOnDestroy(int)`
- `protected int CalculateBitMaskOnDismantle(int)`
- `int CalculateLocalSyncBitMask(int, int)`
- `bool CanBeRepaired(int, ItemBase=null, PlayerBase=null)`
- `bool CanPartDecay()`
- `bool CanSimulateItemFalling()`
- `protected void ClearPartBitMask(inout int, int)`
- `protected void CloseDoorOnPartUnbuild(EntityAI)`
- `void ComposeSyncDataOnBuild(inout int, int)`
- `void ComposeSyncDataOnDestroy(inout int, int)`
- `void ComposeSyncDataOnDismantle(inout int, int)`
- `bool DamageHandsOnBuild()`
- `bool DamageHandsOnDeconstruct()`
- `bool DamageToolOnBuild()`
- `bool DamageToolOnDeconstruct()`
- `int GetBuildActionTypeMask()`
- `int GetBuildIndicationTypeMask()`
- `string GetBuildTextOverride()`
- `int GetDismantleActionTypeMask()`
- `string GetDismantleTextOverride()`
- `int GetHandBuildFBAnimOverride()`
- `TConstructionPartInsiderBoxData GetInsiderBoxData()`
- `int GetLocalSyncBitMask()`
- `int GetMaterialType()`
- `protected int GetMaxRepairLevel(ItemBase=null, PlayerBase=null)`
- `float GetPartDecayRate()`
- `float GetPartDecayThreshold()`
- `ConstructionPartStaticsSupportData GetPartStaticsSupportData()`
- `TConstructionPartVariantArray GetRequiredPartsVariants()`
- `int GetSupportProvided(int)`
- `int GetSupportRequirement(int)`
- `int GetSyncDataSize()`
- `void HandleAddPhysics(EntityAI)`
- `void HandleHideConstructionPart(EntityAI)`
- `void HandleRemovePhysics(EntityAI)`
- `void HandleShowConstructionPart(EntityAI)`
- `bool HasEssentialAttachmentSlots()`
- `bool HasRelatedGUICategories()`
- `void InitAdditionalPartInstanceInfo(PartInstanceInitData)`
- `protected void InitInsiderBoxData(EntityAI)`
- `bool IsLocalSyncBuilt()`
- `bool IsReversedLocal()`
- `bool IsReversible()`
- `bool IsUtilityPart()`
- `void OnBeforeBuildServer(EntityAI, Man)`
- `void OnBeforeDestroyServer(EntityAI, Man)`
- `void OnBeforeDismantleServer(EntityAI, Man)`
- `void OnBuiltServer(EntityAI, Man)`
- `void OnDestroyedServer(EntityAI, Man)`
- `void OnDismantledServer(EntityAI, Man)`
- `void PartInstanceDeferredInit(EntityAI)`
- `protected void PerformHideAnim(string, notnull EntityAI)`
- `protected void PerformShowAnim(string, notnull EntityAI)`
- `void PlayConstructionEffects(EntityAI)`
- `protected void PlayConstructionParticle(EntityAI, vector)`
- `protected void PlayConstructionSound(EntityAI, vector)`
- `void SetPartTypeData(ConstructionPartTypeData)`
- `void StoreLocalSyncBitMask(int)`
- `void TryHandleUtilityParts(EntityAI)`
- `protected void TryHideAnim(string, notnull EntityAI)`
- `protected void TryShowAnim(string, notnull EntityAI)`
- field `int ANIMATION_EFFECT_DELAY_BUILD` = `350`
- field `int BITWISE_SYNCINFO_SIZE_BASE` = `2`
- field `TConstructionPartInsiderBoxData m_InsiderBoxData`
- field `int m_LocalSyncBitMask` = `0`
- field `ConstructionPartTypeData m_PartTypeData`

### `DarkMotoHelmet_ColorBase` — `4_world/entities/itembase/clothing/helmetbase/darkmotohelmet_colorbase.c`
- `ECachedEquipmentItemCategory GetCachedEquipmentCategory()` *(was on EntityAI)*

### `DayZPlayerCameraBase` — `4_world/entities/manbase/dayzplayer/dayzplayercamera_base.c`
- `bool IsPlayerUnderground(PlayerBase)`
- `bool IsSandstormActiveForPlayer(PlayerBase)`
- `int ResolveNVType(int, PlayerBase)`
- field `float SANDSTORM_NVG_THRESHOLD` = `0.15`
- field `int m_ResolvedNVType`

### `DayZPlayerCameras` — `4_world/entities/manbase/dayzplayer/dayzplayercameras.c`
- field `int DAYZCAMERA_3RD_VEHICLE_MOTORBIKE` = `32`

### `DayZPlayerImplement` — `4_world/entities/dayzplayerimplement.c`
- `protected void HandleCeilingDustStepEffect(SurfaceInfo, string)`
- `void OnItemInHandsChangedEx(bool=false, bool=true)`
- field `DustEffectCeilingHandlerSynced m_CeilingDustHandler`

### `DayZPlayerImplementFallDamage` — `4_world/entities/dayzplayerimplementfalldamage.c`
- `float CurveExp(float, float, float, float)`

### `DayZPlayerInventory` — `4_world/systems/inventory/dayzplayerinventory.c`
- `bool ExecuteHandEventCommandClient(inout Serializer)`
- `bool ExecuteHandEventCommandRemote(inout Serializer)`
- `bool ExecuteHandEventCommandServer(inout Serializer)`
- `bool ExecuteInventoryCommandClient(ParamsReadContext)`
- `bool ExecuteInventoryCommandRemote(ParamsReadContext)`
- `bool ExecuteInventoryCommandServer(ParamsReadContext)`
- `bool ExecuteSwapCommandClient(inout Serializer)`
- `bool ExecuteSwapCommandRemote(inout Serializer)`
- `bool ExecuteSwapCommandServer(inout Serializer)`
- `bool ExecuteSyncMoveCommandClient(ParamsReadContext)`
- `bool ExecuteSyncMoveCommandRemote(ParamsReadContext)`
- `bool ExecuteSyncMoveCommandServer(ParamsReadContext)`
- `bool ValidateHandEventServer(inout Serializer, InventoryValidation)`
- `bool ValidateInventoryCommandServer(ParamsReadContext)`
- `bool ValidateSwapServer(inout Serializer, InventoryValidation)`
- `bool ValidateSyncMoveServer(ParamsReadContext, InventoryValidation)`
- field `float INVENTORY_REPAIR_MAX_DISTANCE` = `500.0`
- field `int INVENTORY_REPAIR_MAX_ITEMS` = `5`

### `DebugMonitorValues` — `4_world/classes/debugmonitorvalues.c`
- `protected void CheckEnvironmentTemperature()`
- `protected void CheckHeatComfort()`
- `protected void CheckThermalBias()`
- `float GetEnvironmentTemperature()`
- `float GetHeatComfort()`
- `float GetThermalBias()`
- field `int TYPE_ENVIRONMENT_TEMPERATURE` = `6`
- field `int TYPE_HEAT_COMFORT` = `7`
- field `int TYPE_THERMAL_BIAS` = `8`
- field `float m_EnvironmentTemperatureClient`
- field `float m_HeatComfortClient`
- field `float m_LastStatsUpdate`
- field `float m_ThermalBiasClient`

### `DirtBikeHelmet_ColorBase` — `4_world/entities/itembase/clothing/helmetbase/dirtbikehelmet_colorbase.c`
- `ECachedEquipmentItemCategory GetCachedEquipmentCategory()` *(was on EntityAI)*
- `float GetProtectionLevel(int, bool=false, int=0)` *(was on ItemBase)*

### `DirtBikeHelmet_Mouthguard` — `4_world/entities/itembase/clothing/helmetbase/dirtbikehelmet_colorbase.c`
- field `int SLOT_ID` = `InventorySlots.GetSlotIdFromString("DBHelmetMouth")`

### `EffWheelSmoke` — `4_world/entities/effects/wheelsmoke.c`
- `void SelectFromSurface(SurfaceInfo)`

### `EffectArea` — `4_world/classes/contaminatedarea/effectarea.c`
- `void OnCEIterate(float, float)`

### `EnvDebugPlayerData` — `4_world/classes/environment/environment.c`
- field `bool m_InShadow`

### `Environment` — `4_world/classes/environment/environment.c`
- `protected void CachePlayerInventoryItemAttributes()`
- `protected void CheckShadowPresence()`
- `int GetNextShadowCheck()`
- `private float HeadGearProtectionAgainstSun()`
- `bool IsInShadow()`
- field `float HC_EXPOSED_HEAD_TO_SUN` = `1.2`
- field `int m_DayTime`
- field `float m_ShadowCheckTimer`
- field `bool m_ShadowPresence`
- field `float m_TargetHeatComfortRaw`

### `ExplosivesBase` — `4_world/entities/explosivesbase.c`
- `string GetExplosionSoundSetName(string)` *(obsolete)*
- `void OnCEIterate(float, float)`

### `Fence` — `4_world/entities/itembase/basebuildingbase/fence.c`
- `bool DestroyOnFall()`
- `DigitalCodeLock GetCodeLock()`
- `bool IsActionTargetVisible()` *(was on ItemBase)*
- `protected void UpdateNavmesh()` *(was on BaseBuildingBase)*
- field `typename ATTACHMENT_CODE_LOCK` = `DigitalCodeLock`
- field `string ATTACHMENT_SLOT_BARBEDWIRE_DOWN` = `"Wall_Barbedwire_1"`
- field `string ATTACHMENT_SLOT_BARBEDWIRE_UP` = `"Wall_Barbedwire_2"`
- field `string ATTACHMENT_SLOT_CAMONET` = `"Wall_Camonet"`
- field `string ATTACHMENT_SLOT_CODE_LOCK` = `"Att_CodeLock"`
- field `string ATT_CATEGORY_ATTACHMENTS` = `"Attachments"`
- field `string ATT_CATEGORY_MATERIAL` = `"Material"`
- field `string MEM_POINT_SPAWN` = `"kit_spawn_position"`

### `FirearmActionDetachMagazine` — `4_world/classes/useractionscomponent/actions/weapons/firearmactiondetachmagazine.c`
- `bool CanBePerformedFromInventory()` *(was on ActionSequentialBase)*

### `FireplaceBase` — `4_world/entities/itembase/fireplacebase.c`
- `void EEItemLocationChanged(notnull InventoryLocation, notnull InventoryLocation)` *(was on ItemBase)*
- field `typename ATTACHMENT_BARK_PALM` = `Bark_Palm`

### `GP5GasMask` — `4_world/entities/itembase/clothing/gp5gasmask.c`
- `float GetProtectionLevel(int, bool=false, int=0)` *(was on ItemBase)*

### `Glasses_Base` — `4_world/entities/itembase/clothing_base.c`
- `ECachedEquipmentItemCategory GetCachedEquipmentCategory()` *(was on EntityAI)*

### `GlowPlug` — `4_world/entities/core/inherited/inventoryitem.c`
- field `int SLOT_ID` = `InventorySlots.GetSlotIdFromString("GlowPlug")`

### `GorkaHelmet` — `4_world/entities/itembase/clothing/helmetbase/gorkahelmetcomplete.c`
- `ECachedEquipmentItemCategory GetCachedEquipmentCategory()` *(was on EntityAI)*
- `float GetProtectionLevel(int, bool=false, int=0)` *(was on ItemBase)*

### `GorkaHelmetVisor` — `4_world/entities/itembase/gear/crafting/crafting.c`
- field `int SLOT_ID` = `InventorySlots.GetSlotIdFromString("Glass")`

### `GreatHelm` — `4_world/entities/itembase/clothing/greathelm.c`
- `ECachedEquipmentItemCategory GetCachedEquipmentCategory()` *(was on EntityAI)*

### `Hatchback_02` — `4_world/entities/vehicles/inheritedcars/hatchback_02.c`
- `EntityAI GetElectricitySourceDevice()`
- `string GetElectricitySourceDeviceType()`
- `bool NeedElectricitySourceDevice()`

### `HeavyMetalPhase2Mdfr` — `4_world/classes/playermodifiers/modifiers/diseases/heavymetal.c`
- `protected void OnDeactivate(PlayerBase)` *(was on HeavyMetalMdfr)*

### `ItemBase` — `4_world/entities/itembase.c`
- `bool CanPlayAttachSound()`
- `bool CanPlayDetachSound()`
- `protected void FlushItemSoundQueue()`
- `bool IsDetectableByVicinityCone()`
- `bool IsExternalLockType()`
- `bool IsFacingPlayerEx(DayZPlayer, string)`
- `bool IsPlayerInsideEx(DayZPlayer, string)`
- `protected void LegacyItemSoundSync()`
- `void LegacyStopItemSoundServer(int)`
- `void StartItemSoundClient(int, int=InventorySlots.INVALID)`

### `ItemBaseType` — `4_world/entities/itembasetype.c`
- `void ItemBaseType()`
- field `bool m_CanBeDigged`
- field `bool m_CanBeSplit`
- field `bool m_CanShowQuantity`
- field `int m_CleannessInit`
- field `int m_CleannessMax`
- field `int m_CleannessMin`
- field `TIntArray m_CompatibleLocks`
- field `int m_Count`
- field `TIntFloatMap m_DryingIncrements`
- field `bool m_HasQuantityBar`
- field `TStringArray m_HeadHidingSelections`
- field `float m_HeatIsolation`
- field `bool m_HideSelectionsBySlot`
- field `float m_ItemModelLength float m_ItemAttachOffset`
- field `int m_ItemBehaviour`
- field `int m_LiquidContainerMask`
- field `string m_LockSoundSet`
- field `int m_LockType`
- field `int m_QuickBarBonus`
- field `TIntFloatMap m_SoakingIncrements`
- field `float m_TemperaturePerQuantityWeight`
- field `int m_VarLiquidTypeInit`
- field `int m_VarQuantityInit`
- field `int m_VarQuantityMax`
- field `int m_VarQuantityMin`
- field `float m_VarStackMax`
- field `float m_VarWetInit`
- field `float m_VarWetMax`
- field `float m_VarWetMin`

### `ItemOptics` — `4_world/entities/core/inherited/itemoptics.c`
- `bool IsSwitchedOn()`
- `void OnSwitchOff()` *(was on EntityAI)*
- `void OnSwitchOn()` *(was on EntityAI)*

### `ItemTransmitter` — `4_world/entities/core/inherited/inventoryitem.c`
- `proto native bool IsTunedToBunkerFrequency()`

### `KobraOptic` — `4_world/entities/itembase/inventory_base/optics.c`
- `bool IsUsableWithNV()` *(was on ItemOptics)*

### `LandMineTrap` — `4_world/entities/itembase/trapbase/trap_landmine.c`
- `void OnTriggeredByFall(Contact)`

### `Land_FuelStation_Feed` — `4_world/entities/building/specific/land_fuelstation_feed.c`
- `ELiquidSourceObjectType GetLiquidSourceObjectType()`
- `void SetActionAnimOverrides()`

### `Land_Misc_Through_Static` — `4_world/entities/building/industrial/misc/land_misc_through_static.c`
- `void SetActionAnimOverrides()`

### `Magazine` — `4_world/entities/itembase/magazine/magazine.c`
- `bool LocalAcquireCartridgeEx(out float, out string)`
- `bool ServerAcquireCartridgeEx(out float, out string)`
- `bool ServerStoreCartridgeEx(float, string)`
- `protected void UpdateAnimation()`

### `Magnum_Base` — `4_world/entities/firearms/pistol/magnum.c`
- `void SetGunParts()`

### `MaskBase` — `4_world/entities/itembase/clothing/maskbase.c`
- `ECachedEquipmentItemCategory GetCachedEquipmentCategory()` *(was on EntityAI)*

### `Mask_Base` — `4_world/entities/itembase/clothing_base.c`
- `ECachedEquipmentItemCategory GetCachedEquipmentCategory()` *(was on EntityAI)*

### `MilitaryBeret_ColorBase` — `4_world/entities/itembase/clothing/militaryberet_colorbase.c`
- `void SetActions()` *(was on ItemBase)*

### `MiscGameplayFunctions` — `4_world/static/miscgameplayfunctions.c`
- `static bool ComponentIdxContainsSelection(Object, int, string)`
- `static void GenerateAINoiseAtPositionEx(notnull Object, float, NoiseParams)`
- `static bool GenericDirectionalCheck(vector, vector, float)`
- `static float GetPlayerHeadHeight(notnull PlayerBase)`
- `static vector GetPlayerHeadPosition(notnull PlayerBase)`
- `static bool IsUnder(EntityAI, array<typename>excludedTypes, out Object, float=GameConstants.ROOF_CHECK_RAYCAST_DIST, int=ObjIntersectIFire)`
- `static bool MemPointDirectionalCheck(PlayerBase, EntityAI, string, string, float=Math.PI_HALF)`
- `static void ThrowAllItemsInInventoryEx(notnull EntityAI, int, vector)`

### `MotoHelmet_ColorBase` — `4_world/entities/itembase/clothing/helmetbase/motohelmet_colorbase.c`
- `ECachedEquipmentItemCategory GetCachedEquipmentCategory()` *(was on EntityAI)*

### `NVGHeadstrap` — `4_world/entities/itembase/clothing/nvgheadstrap.c`
- `ECachedEquipmentItemCategory GetCachedEquipmentCategory()` *(was on EntityAI)*
- `void OnDebugSpawn()` *(was on EntityAI)*

### `NVGoggles` — `4_world/entities/itembase/nvgoggles.c`
- `float GetProtectionLevel(int, bool=false, int=0)` *(was on ItemBase)*

### `NoiseAIEvaluate` — `4_world/static/sensesaievaluate.c`
- `static float GetNoiseReductionEx(notnull Object)` *(obsolete)*

### `NotifiersManager` — `4_world/classes/playernotifiers/notifiersmanager.c`
- `protected void ClearImmediatePending(int)`
- `bool HasNotifier(int)`
- `bool IsNotifierActive(int)`
- `protected bool IsNotifierIDValid(int)`
- `protected bool ProcessImmediateNotifier(int)`
- `void RequestImmediateUpdate(int)`
- `bool SetNotifierActive(int, bool, bool=false)`
- `protected void TickSingleNotifier(int)`
- field `bool m_ImmediatePending`

### `OffroadHatchback` — `4_world/entities/vehicles/inheritedcars/offroadhatchback.c`
- `EntityAI GetElectricitySourceDevice()`
- `string GetElectricitySourceDeviceType()`
- `bool NeedElectricitySourceDevice()`

### `Offroad_02` — `4_world/entities/vehicles/inheritedcars/offroad_02.c`
- `EntityAI GetElectricitySourceDevice()`
- `string GetElectricitySourceDeviceType()`
- `float GetWeightCoef()`
- `bool HasGangedTailAndBrakeLights()`
- `bool NeedElectricitySourceDevice()`

### `PASBroadcaster` — `4_world/entities/advancedcommunication/advancedcommunication.c`
- `bool IsDetectableByVicinityCone()`

### `Pistol_Base` — `4_world/entities/firearms/pistol_base.c`
- `int GetChargedPistolAnimState()`
- `int GetDischargedPistolAnimState()`
- `int GetJammedPistolAnimState()`
- `int GetOpenPistolAnimState()`

### `PlayerBase` — `4_world/entities/manbase/playerbase.c`
- `void AdjustKeffiyeh(EntityAI, string)`
- `void CheckShouldCancelSurrender()`
- `ThermalBiasHandler GetThermalBiasHandler()`
- `EUndergroundPresence GetUndergroundPresence()`
- `protected void OnFinishTransitionFromSurrender()`
- `protected void OnFinishTransitionToSurrender()`
- `protected void OnStartTransitionFromSurrender(bool=true)`
- `protected void OnStartTransitionToSurrender()`
- `private void RequestRespawnBot()`
- `protected void RestartAllMovement()`
- `void SetSurrenderState(bool)`
- `protected void StopAllMovement()`
- field `bool m_IsSurrendered`
- field `bool m_StartedSurrenderTransition`
- field `float m_SurrenderAnimationTransitionTime` = `0.8`
- field `float m_SurrenderTransitionTimer`
- field `ThermalBiasHandler m_ThermalBiasHandler`

### `PluginConfigDebugProfile` — `4_world/plugins/pluginbase/pluginfilehandler/pluginconfighandler/pluginconfigdebugprofile.c`
- `vector GetCharacterAgentsPos()`
- `vector GetCharacterStatsPos()`
- `vector GetHealthDebugPos()`
- `void SetCharacterAgentsPos(vector)`
- `void SetCharacterStatsPos(vector)`
- `void SetHealthDebugPos(vector)`
- field `string CHAR_AGENTS_POS` = `"console_character_agents_pos"`
- field `string CHAR_HEALTH_POS` = `"console_character_health_pos"`
- field `string CHAR_STATS_POS` = `"console_character_stats_pos"`

### `PluginDeveloper` — `4_world/plugins/pluginbase/plugindeveloper.c`
- `protected TStringArray GetItemClasses()`
- `protected void InitItems()`

### `PluginDiagMenuClient` — `4_world/plugins/pluginbase/plugindiagmenu/plugindiagmenuclient.c`
- `static void CBBaseBuildingCollisionDebug(bool)`
- `static void CBBaseBuildingDebug(bool)`
- `static void CBBaseBuildingFast(int)`
- `static void CBDECloudCooldown(float)`
- `static void CBDECloudDistance(float)`
- `static void CBDECloudDuration(float)`
- `static void CBDECloudEnable(bool)`
- `static void CBDECloudFromSurfaceEnable(bool)`
- `static void CBDECloudThreshold(float)`
- `static void CBDEGroundEnable(bool)`
- `static void CBDEGroundFromSurfaceEnable(bool)`
- `static void CBDEGroundIntensity(float)`
- `static void CBDEGroundLifetimeRange(float)`
- `static void CBDEGroundRadius(float)`
- `static void CBDEGroundSizeRange(float)`
- `static void CBDEGroundThreshold(float)`
- `static void CBDEWindWeight(float)`
- `static void CBOldProcessInputData(bool, int)`
- `static void CBRoboclientActive(bool)`
- `static void CBRoboclientChangeStateMachine(int)`
- `static void CBRoboclientRestart(bool)`
- `static void CBSetRadioGameplayLogging(bool)`
- `void UpdateBaseBuildingFullDebug()`
- `void UpdateCharacterStepSurfaceDebug()`

### `PluginInventoryDebug` — `4_world/plugins/pluginbase/plugininventorydebug.c`
- `bool IsOldProcessInputDataEnable()`
- `void SetOldProcessInputDataEnable(bool)`
- field `bool m_IsOldProcessInputDataEnable` = `false`

### `PluginRecipesManager` — `4_world/plugins/pluginbase/pluginrecipesmanager.c`
- `string GetRecipeClassName(int)`

### `PluginTransmissionAgents` — `4_world/plugins/pluginbase/plugintransmissionagents.c`
- `static float GetProtectionLevelCachedEquipment(int, int, Man)`

### `PowerGeneratorBase` — `4_world/entities/itembase/powergenerator.c`
- `void OnItemFallRebuilding()`

### `PumpkinHelmet` — `4_world/entities/itembase/pumpkinhelmet.c`
- `ECachedEquipmentItemCategory GetCachedEquipmentCategory()` *(was on EntityAI)*

### `RemoteDetonatorTrigger` — `4_world/entities/explosivesbase/remotedetonator.c`
- `void OnCEIterate(float, float)`

### `ScriptedLightBase` — `4_world/entities/scriptedlightbase.c`
- `void AttachOnObjectPivot(Object, int, vector=vector.Zero, vector=vector.Zero)`

### `Sedan_02` — `4_world/entities/vehicles/inheritedcars/sedan_02.c`
- `EntityAI GetElectricitySourceDevice()`
- `string GetElectricitySourceDeviceType()`
- `bool NeedElectricitySourceDevice()`

### `ShelterBase` — `4_world/entities/itembase/tentbase/shelter.c`
- `void DeconstructEx(bool=false)`
- `void OnItemFallRebuilding()`
- `void SpawnKitEx(bool=false)`

### `ShelterSite` — `4_world/entities/itembase/basebuildingbase/sheltersite.c`
- `bool DestroyOnFall()`
- `void OnDebugSpawn()` *(was on BaseBuildingBase)*
- `void OnItemFallRebuilding()`

### `Shemag_ColorBase` — `4_world/entities/itembase/gear/consumables/consumables.c`
- `ECachedEquipmentItemCategory GetCachedEquipmentCategory()` *(was on EntityAI)*
- `float GetProtectionLevel(int, bool=false, int=0)` *(was on ItemBase)*

### `SparkPlug` — `4_world/entities/core/inherited/inventoryitem.c`
- field `int SLOT_ID` = `InventorySlots.GetSlotIdFromString("SparkPlug")`

### `StandardActionInput` — `4_world/classes/useractionscomponent/actioninput.c`
- `protected void SortActions(array<ActionBase>allActions)`

### `StaticConstructionMethods` — `4_world/classes/constructionbase.c`
- `static void SpawnConstructionMaterialPilesEx(notnull EntityAI, Man, string, string, string="", bool=false, bool=false)`

### `TentBase` — `4_world/entities/itembase/tentbase.c`
- `protected void ClearIsFalling()`
- `void DropCargoItems()`
- `void OnItemFallRebuilding()`
- field `bool m_IsFalling`

### `TerritoryFlag` — `4_world/entities/itembase/basebuildingbase/totem.c`
- `bool DestroyOnFall()`
- `void OnCEIterate(float, float)`

### `TransmitterBase` — `4_world/entities/itembase/transmitterbase.c`
- `void OnVariablesSynchronized()` *(was on ItemBase)*
- `protected bool UseBunkerStaticNoise()`
- field `string SOUND_BUNKER_STATIC_NOISE` = `""`
- field `bool m_BunkerNoisePlaying`

### `TrapBase` — `4_world/entities/itembase/trapbase.c`
- `protected void ClearIsFalling()`
- `void EOnContact(IEntity, Contact)` *(was on ItemBase)*
- `void OnItemFallRebuilding()`
- `protected void OnTriggeredByFall(Contact)`
- `protected void OnTriggeredByFallEffectsClient()`
- field `bool m_FallTriggered` = `false`
- field `bool m_IsFalling` = `false`

### `TripwireTrap` — `4_world/entities/itembase/trapbase/trap_tripwire.c`
- `void OnTriggeredByFall(Contact)`
- `void OnTriggeredByFallEffectsClient()`

### `Truck_01_Base` — `4_world/entities/vehicles/inheritedcars/truck_01_base.c`
- `EntityAI GetElectricitySourceDevice()`
- `string GetElectricitySourceDeviceType()`
- `float GetWeightCoef()`
- `bool NeedElectricitySourceDevice()`

### `Truck_02` — `4_world/entities/vehicles/inheritedcars/truck_02.c`
- `EntityAI GetElectricitySourceDevice()`
- `string GetElectricitySourceDeviceType()`
- `bool NeedElectricitySourceDevice()`

### `UAMaxDistances` — `4_world/classes/useractionscomponent/actions/actionconstants.c`
- field `float FISHING` = `20.0`

### `UAMisc` — `4_world/classes/useractionscomponent/actions/actionconstants.c`
- field `string LIQUID_SOURCE_SELECTION` = `"liquid_source"`

### `UATimeSpent` — `4_world/classes/useractionscomponent/actions/actionconstants.c`
- field `float MORTAR_MIX` = `8`
- field `float WASH_FACE_ITEM` = `0.5`

### `UndergroundTrigger` — `4_world/entities/scriptedentities/triggers/undergroundtrigger.c`
- `EUndergroundPresence TranslateTriggerTypeToPresence()`

### `Van_01` — `4_world/entities/vehicles/inheritedcars/van_01.c`
- `EntityAI GetElectricitySourceDevice()`
- `string GetElectricitySourceDeviceType()`
- `bool NeedElectricitySourceDevice()`

### `Watchtower` — `4_world/entities/itembase/basebuildingbase/watchtower.c`
- `bool DestroyOnFall()`
- `bool IsActionTargetVisible()` *(was on ItemBase)*

### `WeaponDebug` — `4_world/classes/weapondebug.c`
- `void CycleWeaponDebugMode()`
- field `bool m_IsDrawKeyHeldDownPrev`
- field `int m_PointCount` = `0`

### `WeldingMask` — `4_world/entities/itembase/clothing/weldingmask.c`
- `ECachedEquipmentItemCategory GetCachedEquipmentCategory()` *(was on EntityAI)*

### `Well` — `4_world/entities/building/well.c`
- `void SetActionAnimOverrides()`

### `ZombieBase` — `4_world/entities/creatures/infected/zombiebase.c`
- `protected void HandleCeilingDustStepEffect(SurfaceInfo)`
- `void OnStepEvent(int, string)` *(was on DayZCreatureAI)*
- field `DustEffectCeilingHandlerLocal m_CeilingDustHandler`


## 5_mission

### `ActionTargetsCursor` — `5_mission/gui/actiontargetscursor.c`
- `protected void CreateActionInfoPanel(typename, int)`
- `protected void HideRootWidget()`
- `protected void InfoPanelCleanup()`
- `protected void InitActionInfoPanels()`
- `protected void UpdateAdditionalActionInfo()`
- field `int ACTION_ORDER_CONTINUOUS` = `3`
- field `int ACTION_ORDER_INTERACT_CONTINUOUS` = `1`
- field `int ACTION_ORDER_INTERACT_SINGLE` = `0`
- field `int ACTION_ORDER_SINGLE` = `2`
- field `float m_PosX`
- field `float m_PosY`

### `AttachmentCategoriesContainer` — `5_mission/gui/inventorynew/inherited/attachmentcategoriescontainer.c`
- `void UpdateNames()`

### `AttachmentCategoriesRow` — `5_mission/gui/inventorynew/inherited/attachmentcategoriesrow.c`
- `protected bool IsCategoryReachable(EntityAI, string, float=1.5)`

### `DayZIntroScene` — `5_mission/dayzintroscene.c`
- `protected void ClearIntroScene()`
- `protected void SetScene()`

### `DayZIntroScenePC` — `5_mission/dayzintroscenepc.c`
- `void SetScene()`

### `DebugMonitor` — `5_mission/gui/debugmonitor.c`
- `void SetEnvironmentTemperature(float)`
- `void SetHeatComfort(float)`
- `void SetThermalBias(float)`
- field `TextWidget m_EnvironmentTemperatureValue`
- field `TextWidget m_HeatComfortValue`
- field `TextWidget m_ThermalBiasValue`

### `HudDebug` — `5_mission/gui/huddebug.c`
- `bool OnMouseButtonDown(Widget, int, int, int)`
- `bool OnMouseButtonUp(Widget, int, int, int)`

### `HudDebugEventHandler` — `5_mission/gui/huddebug.c`
- `bool OnMouseButtonUp(Widget, int, int, int)` *(was on ScriptedWidgetEventHandler)*

### `HudDebugWinBase` — `5_mission/gui/scriptshuddebug/huddebugwinbase.c`
- `void OnDraggingEnd()`
- `void OnDraggingStart()`
- `void SetDragging(bool)`
- field `PluginConfigDebugProfile m_ConfigDebugProfile`
- field `vector m_DraggingOffset`
- field `bool m_IsDragging`

### `HudDebugWinCharAgents` — `5_mission/gui/scriptshuddebug/huddebugwincharagents.c`
- `void OnDraggingEnd()`
- `void OnInit()`
- `bool OnMouseButtonDown(Widget, int, int, int)`
- `bool OnMouseButtonUp(Widget, int, int, int)`
- field `float AGENTS_UPDATE_TIME` = `0.500`
- field `float m_UpdateAgentsTimer`
- field `Widget m_WgtHeader`

### `HudDebugWinCharStats` — `5_mission/gui/scriptshuddebug/huddebugwincharstats.c`
- `void OnDraggingEnd()`
- `void OnInit()`
- `bool OnMouseButtonDown(Widget, int, int, int)`
- `bool OnMouseButtonUp(Widget, int, int, int)`
- field `Widget m_WgtHeader`

### `HudDebugWinHealth` — `5_mission/gui/scriptshuddebug/huddebugwinhealth.c`
- `void OnDraggingEnd()`
- `bool OnMouseButtonDown(Widget, int, int, int)`
- `bool OnMouseButtonUp(Widget, int, int, int)`
- field `float m_ContenHeight`
- field `Widget m_WgtHeader`

### `HudDebugWinHealthEntry` — `5_mission/gui/scriptshuddebug/huddebugwinhealth.c`
- field `Widget m_SpacerLine`

### `InviteMenu` — `5_mission/gui/invitemenu.c`
- `protected void JoinTargetServer()`
- `protected void OnLogoutReplyTimeout()`
- `void SetLogoutTime()`
- `void StartCountdown(int)`
- field `int JOIN_LEAD_TIME_S` = `1`
- field `int LOGOUT_REPLY_TIMEOUT_MS` = `5000`
- field `bool m_bJoining`

### `LogoutMenu` — `5_mission/gui/logoutmenu.c`
- field `bool m_bCancelSent`

### `MainMenu` — `5_mission/gui/newui/mainmenu/mainmenu.c`
- `void OnDLCChange(EDLCId)`
- `protected void RefreshNewsCarousel()`

### `MainMenuConsole` — `5_mission/gui/newui/mainmenu/mainmenuconsoles.c`
- `protected void RefreshNewsCarousel()`

### `MainMenuData` — `5_mission/gui/newui/mainmenu/mainmenudata.c`
- `static ScriptInvoker GetOnDLCChangedSI()`
- `static void OnDLCChange(EDLCId)`
- `static void SubscribeDLCUpdate()`
- `static void UnsubscribeDLCUpdate()`
- field `ScriptInvoker m_OnDLCChangeSI`

### `MissionGameplay` — `5_mission/mission/missiongameplay.c`
- `void TakeOverLogoutMenu()`
- field `BunkerBroadcastHandler m_BunkerBroadcastHandler`

### `MissionServer` — `5_mission/mission/missionserver.c`
- field `BunkerBroadcastHandler m_BunkerBroadcastHandler`

### `ModsMenuDetailedEntry` — `5_mission/gui/newui/modsmenu/modsmenudetailedentry.c`
- `void OnDLCChange(EDLCId)`

### `NewsCarousel` — `5_mission/gui/newui/mainmenu/newscarousel.c`
- `int GetArticleCount()`
- `bool IsVisible()`

### `OptionsMenuGame` — `5_mission/gui/newui/options/optionsmenugame.c`
- `protected void InitPauseOption()`
- `protected void InitQuickbarOption()`

### `ScriptConsole` — `5_mission/gui/scriptconsole.c`
- `protected void BuildContextMenu()`
- `ButtonWidget GetContextElementButton(int)`
- `protected void HideContextSubElements()`
- `protected bool IsChildOfWidget(Widget, Widget)`
- `protected bool IsContextElementButton(Widget)`
- `protected void OnInputDeviceChanged(EInputDeviceType)`
- `protected void RegisterContextMenuElement(int, string)`
- field `GridSpacerWidget m_ButtonsMenu`
- field `GridSpacerWidget m_ConsoleButtonsMenuGrid`
- field `TextWidget m_ContectMenuButtonLabel`
- field `ButtonWidget m_ContextMenuButton`
- field `GridSpacerWidget m_ContextMenuGrid`
- field `EInputDeviceType m_CurrentInputDevice`
- field `Widget m_HeaderPanelConsole`
- field `Widget m_HeaderPanelPC`

### `ScriptConsoleCameraTab` — `5_mission/gui/scriptconsolecameratab.c`
- `protected void ButtonHighlight(Widget)`
- `protected void ButtonNormal(Widget)`
- `void InitializeTab(Widget, ScriptConsole, Widget, ScriptConsoleTabBase=null)`
- `protected void ToogleUIVisibility()`
- field `ButtonWidget m_HideUIButton`
- field `ImageWidget m_HideUIIcon`
- field `bool m_IsSHIFTPressed`
- field `bool m_UIVisibilityState`

### `ScriptConsoleConfigTab` — `5_mission/gui/scriptconsoleconfigtab.c`
- `void InitializeTab(Widget, ScriptConsole, Widget, ScriptConsoleTabBase=null)`

### `ScriptConsoleEnfScriptTab` — `5_mission/gui/scriptconsoleenfscripttab.c`
- `void InitializeTab(Widget, ScriptConsole, Widget, ScriptConsoleTabBase=null)`

### `ScriptConsoleGeneralTab` — `5_mission/gui/scriptconsolegeneraltab.c`
- `void InitializeTab(Widget, ScriptConsole, Widget, ScriptConsoleTabBase=null)`
- `void OnSelected()` *(was on ScriptConsoleTabBase)*

### `ScriptConsoleItemsTab` — `5_mission/gui/scriptconsoleitemstab.c`
- `protected void AddItemToClipboard()` *(was on ScriptConsoleTabBase)*
- `void InitializeTab(Widget, ScriptConsole, Widget, ScriptConsoleTabBase=null)`
- `void OnSelected()` *(was on ScriptConsoleTabBase)*
- field `int OBJECT_SCOPE` = `2`

### `ScriptConsoleOutputTab` — `5_mission/gui/scriptconsoleoutputtab.c`
- `void InitializeTab(Widget, ScriptConsole, Widget, ScriptConsoleTabBase=null)`

### `ScriptConsoleSoundsTab` — `5_mission/gui/scriptconsolesoundstab.c`
- `void InitializeTab(Widget, ScriptConsole, Widget, ScriptConsoleTabBase=null)`
- `void OnSelected()` *(was on ScriptConsoleTabBase)*

### `ScriptConsoleTabBase` — `5_mission/gui/scriptconsoletabbase.c`
- `void InitializeTab(Widget, ScriptConsole, Widget, ScriptConsoleTabBase=null)`
- `void OnInputDeviceChanged(EInputDeviceType)`

### `ScriptConsoleVicinityTab` — `5_mission/gui/scriptconsolevicinitytab.c`
- `void InitializeTab(Widget, ScriptConsole, Widget, ScriptConsoleTabBase=null)`

### `ScriptConsoleWeatherTab` — `5_mission/gui/scriptconsoleweathertab.c`
- `protected DebugSandstormRPCData GetSandstormRPCData(bool=false)`
- `void InitializeTab(Widget, ScriptConsole, Widget, ScriptConsoleTabBase=null)`
- `protected void InvokeSendSandstormRPC()`
- `protected void OnSandstormStart()`
- `protected void SendSandstormRPC(DebugSandstormRPCData)`
- `protected void StartSandstormFromWeather()`
- `protected void StopSandstormFromWeather()`
- `protected void StopSandstormRPC()`
- field `float m_SandstormDurationTimeValue` = `-1`
- field `float m_SandstormMagnitudeValue` = `-1`
- field `float m_SandstormSpeedValue` = `-1`
- field `TextWidget m_StandstormDurationText`
- field `TextWidget m_StandstormMagnitudeText`
- field `TextWidget m_StandstormSpeedText`
- field `ButtonWidget m_StartSandstormButton`
- field `ButtonWidget m_StartSandstormWeatherButton`
- field `ButtonWidget m_StopSandstormButton`
- field `ButtonWidget m_StopSandstormWeatherButton`
- field `ScriptConsoleWeatherTab m_WeatherTab`

### `SlotsIcon` — `5_mission/gui/inventorynew/containeditems/slotsicon.c`
- `protected void HandleConsoleToolbar()`
- `protected void InitIcon(LayoutHolder, Widget, int, EntityAI)`
- `protected void InitIconWidgets(LayoutHolder, Widget, int, EntityAI)`

