# Hooks and helpers vanilla 1.30 no longer calls

<!-- Generated 2026-10-15: call sites (non-declaration lines) of every method that is [Obsolete] or removed in 1.30, counted in 1.29 vanilla (O:\) and 1.30.164078 vanilla (P:\scripts). Grep-based: a call through a variable of another name is not seen. -->

**DEAD** = vanilla 1.29 called it, vanilla 1.30 never does. A mod that *overrides* it still compiles (with an obsolete warning) and silently stops working. A mod that *calls* an obsolete one usually still works (most are thin wrappers) — read the 1.30 body before relying on that.

Also dead, found by hand (moved to a new class, so not in the table below): `Construction/ConstructionBase.BuildPartServer` and `DismantlePartServer` — vanilla calls `BuildPartServerEx` / `DismantlePartServerEx(Man player, string part_name, int action_id)` (P:\scripts\4_world\classes\constructionbase.c:179, :190); the old ones only forward (constructionbase.c:1702-1716). `EntityAI.CanDisplayAttachmentSlot(string)` — vanilla calls only the `int slot_id` overload (entityai.c:1788, :1804).

| Status | Kind | Class.Method | 1.29 calls | 1.30 calls | 1.30 message / where it still is called |
|---|---|---|---|---|---|
| DEAD | obsolete | `ActionDrinkPondContinuous.GetLiquidSource` | 1 | 0 | 1.30: No replacement. Method is no longer needed as liquid source is now checked and provided by the CCTLiquid target condition. |
| unused | obsolete | `ActionTargetsCursor.SetContinuousInteractXboxIcon` | 0 | 0 | no replacement |
| unused | obsolete | `ActionTargetsCursor.SetContinuousXboxIcon` | 0 | 0 | no replacement |
| unused | obsolete | `ActionTargetsCursor.SetInteractXboxIcon` | 0 | 0 | no replacement |
| unused | obsolete | `ActionTargetsCursor.SetSingleXboxIcon` | 0 | 0 | no replacement |
| unused | obsolete | `ActionTargetsCursor.SetXboxIcon` | 0 | 0 | no replacement |
| unused | obsolete | `AgentBase.GrowDuringAntibioticsAttack` | 0 | 0 | replaced by GrowDuringMedicalDrugsAttack |
| DEAD | removed | `AmmoTypesAPI.GetExplosionParticleID` | 1 | 0 |  |
| DEAD | obsolete | `BaseBuildingBase.DebugCustomState` | 1 | 0 | obsolete debug method |
| unused | obsolete | `BaseBuildingBase.HandleItemFalling` | 0 | 0 | handled in the Construction component directly |
| DEAD | obsolete | `BaseBuildingBase.ItemFall` | 1 | 0 | handled in the Construction component directly |
| unused | removed | `BoxDraw.InverseTransformPointByMatrix` | 0 | 0 |  |
| unused | removed | `BoxDraw.OnGlobalRotationChanged` | 0 | 0 |  |
| DEAD | removed | `BoxDraw.TransformPointByMatrix` | 1 | 0 |  |
| DEAD | removed | `CGame.GoBuyWorldDLC` | 1 | 0 |  |
| DEAD | removed | `CGame.VerifyWorldOwnership` | 3 | 0 |  |
| DEAD | obsolete | `CarScript.BrakeAndReverseRearLight` | 1 | 0 | 1.30: use VehicleLightsComponent instead |
| DEAD | obsolete | `CarScript.BrakeLightsShineOff` | 4 | 0 | 1.30: use VehicleLightsComponent instead |
| DEAD | obsolete | `CarScript.BrakeLightsShineOn` | 3 | 0 | 1.30: VehicleLightsComponent instead |
| DEAD | obsolete | `CarScript.BrakesRearLight` | 2 | 0 | 1.30: use VehicleLightsComponent instead |
| DEAD | obsolete | `CarScript.CarPartsHealthCheck` | 1 | 0 | 1.30: use Transport::PartsHealthCheck instead |
| DEAD | obsolete | `CarScript.CreateFrontLight` | 1 | 0 | 1.30: use VehicleLightsComponent instead |
| DEAD | obsolete | `CarScript.CreateRearLight` | 1 | 0 | 1.30: use VehicleLightsComponent instead |
| DEAD | obsolete | `CarScript.DashboardShineOff` | 1 | 0 | 1.30: use VehicleLightsComponent instead |
| DEAD | obsolete | `CarScript.DashboardShineOn` | 1 | 0 | 1.30: use VehicleLightsComponent instead |
| DEAD | obsolete | `CarScript.ForceUpdateLightsEnd` | 5 | 0 | 1.30: no replacement |
| DEAD | obsolete | `CarScript.ForceUpdateLightsStart` | 6 | 0 | 1.30: no replacement |
| DEAD | removed | `CarScript.GenerateCarHornAINoise` | 1 | 0 |  |
| DEAD | obsolete | `CarScript.IsScriptedLightsOn` | 3 | 0 | 1.30: use Transport.LightIsOn |
| DEAD | obsolete | `CarScript.IsVitalCarBattery` | 4 | 0 | 1.30: use Transport.NeedElectricitySourceDevice |
| DEAD | obsolete | `CarScript.IsVitalTruckBattery` | 4 | 0 | 1.30: use Transport.NeedElectricitySourceDevice |
| DEAD | obsolete | `CarScript.LeftFrontLightShineOff` | 3 | 0 | 1.30: use VehicleLightsComponent instead |
| DEAD | obsolete | `CarScript.LeftFrontLightShineOn` | 2 | 0 | 1.30: use VehicleLightsComponent instead |
| DEAD | obsolete | `CarScript.NoRearLight` | 3 | 0 | 1.30: use VehicleLightsComponent instead |
| DEAD | obsolete | `CarScript.ReverseLightsShineOff` | 5 | 0 | 1.30: use VehicleLightsComponent instead |
| DEAD | obsolete | `CarScript.ReverseLightsShineOn` | 2 | 0 | 1.30: use VehicleLightsComponent instead |
| DEAD | obsolete | `CarScript.ReverseRearLight` | 1 | 0 | 1.30: use VehicleLightsComponent instead |
| DEAD | obsolete | `CarScript.RightFrontLightShineOff` | 3 | 0 | 1.30: use VehicleLightsComponent instead |
| DEAD | obsolete | `CarScript.RightFrontLightShineOn` | 2 | 0 | 1.30: use VehicleLightsComponent instead |
| DEAD | obsolete | `CarScript.TailLightsShineOff` | 1 | 0 | 1.30: use VehicleLightsComponent instead |
| DEAD | obsolete | `CarScript.TailLightsShineOn` | 1 | 0 | 1.30: use VehicleLightsComponent instead |
| DEAD | removed | `CarScript.ToggleHeadlights` | 3 | 0 |  |
| DEAD | obsolete | `CombinationLock.IsLockedOnGate` | 2 | 0 | 1.30: Use IsLocked instead |
| DEAD | obsolete | `CombinationLock.SoundLockChangeDial` | 1 | 0 | 1.30: Use ItemSoundHandler instead |
| DEAD | obsolete | `CombinationLock.SoundLockChangeNumber` | 1 | 0 | 1.30: Use ItemSoundHandler instead |
| DEAD | obsolete | `CombinationLock.SoundLockClose` | 1 | 0 | 1.30: Use ItemSoundHandler instead |
| DEAD | obsolete | `CombinationLock.SoundLockOpen` | 1 | 0 | 1.30: Use ItemSoundHandler instead |
| DEAD | removed | `CombinationLock.UnlockServer` | 4 | 0 |  |
| unused | obsolete | `ConstructionPart.SetRequestBuiltState` | 0 | 0 | 1.30: Use 'SetBuiltState' directly |
| unused | obsolete | `DayZPlayerImplementMeleeCombat.HitZoneSelection` | 0 | 0 | replaced by HitZoneSelectionRaycast |
| DEAD | obsolete | `EffWheelSmoke.SetSurface` | 2 | 0 | 1.30: use SelectFromSurface instead |
| unused | obsolete | `EmoteManager.CheckEmoteLockedState` | 0 | 0 | 1.30: no replacement |
| DEAD | obsolete | `EmoteManager.ForceSurrenderState` | 4 | 0 | 1.30: Use PlayerBase::SetSurrenderState instead |
| DEAD | obsolete | `EntityAI.LocalTakeEntityAsAttachment` | 4 | 0 | 1.30: Use LocalTakeEntityToTargetAttachment instead |
| DEAD | obsolete | `EntityAI.LocalTakeEntityAsAttachmentEx` | 3 | 0 | 1.30: Use LocalTakeEntityToTargetAttachmentEx instead |
| unused | obsolete | `EntityAI.LocalTakeEntityToCargo` | 0 | 0 | 1.30: Use LocalTakeEntityToTargetCargo instead |
| unused | obsolete | `EntityAI.LocalTakeEntityToCargoEx` | 0 | 0 | 1.30: Use LocalTakeEntityToTargetCargoEx instead |
| DEAD | obsolete | `EntityAI.OnCEUpdate` | 4 | 0 | 1.30: Use OnCEIterate(float currentTime, float elapsedTime)instead |
| DEAD | obsolete | `EntityAI.PredictiveTakeEntityAsAttachment` | 3 | 0 | 1.30: Use PredictiveTakeEntityToTargetAttachment instead |
| DEAD | obsolete | `EntityAI.PredictiveTakeEntityAsAttachmentEx` | 1 | 0 | 1.30: Use PredictiveTakeEntityToTargetAttachmentEx instead |
| unused | obsolete | `EntityAI.PredictiveTakeEntityToCargo` | 0 | 0 | 1.30: Use PredictiveTakeEntityToTargetCargo instead |
| unused | obsolete | `EntityAI.PredictiveTakeEntityToCargoEx` | 0 | 0 | 1.29: Use PredictiveTakeEntityToTargetCargoEx instead |
| unused | obsolete | `EntityAI.ServerTakeEntityAsAttachment` | 0 | 0 | 1.30: Use ServerTakeEntityToTargetAttachment instead |
| unused | obsolete | `EntityAI.ServerTakeEntityAsAttachmentEx` | 0 | 0 | 1.30: Use ServerTakeEntityToTargetAttachmentEx instead |
| unused | obsolete | `EntityAI.ServerTakeEntityToCargo` | 0 | 0 | 1.30: Use ServerTakeEntityToTargetCargo instead |
| DEAD | obsolete | `ExplosivesBase.AddExplosionEffectForSurface` | 2 | 0 | 1.30: Handled in AmmoTypes instead |
| DEAD | obsolete | `ExplosivesBase.GetParticleExplosionID` | 1 | 0 | 1.30: Handled in AmmoTypes instead |
| DEAD | obsolete | `ExplosivesBase.InitSpecificsExplosionEffectForSurface` | 1 | 0 | 1.30: Handled in AmmoTypes instead |
| DEAD | obsolete | `ExplosivesBase.SetParticleExplosion` | 7 | 0 | 1.30: Handled in AmmoTypes instead |
| unused | removed | `GetServersInput.SetIsDLC` | 0 | 0 |  |
| unused | obsolete | `Icon.GetRelevantInventoryAction` | 0 | 0 | no replacement |
| unused | removed | `InviteMenu.OnInputPresetChanged` | 0 | 0 |  |
| unused | obsolete | `ItemBase.ClearStartItemSoundServer` | 0 | 0 | 1.30: No replacement |
| DEAD | obsolete | `ItemBase.GetHeatIsolationInit` | 1 | 0 | 1.30: No replacement |
| DEAD | obsolete | `Man.JunctureTakeEntityAsAttachment` | 1 | 0 | 1.30: Use JunctureTakeEntityToTargetAttachment instead |
| DEAD | obsolete | `Man.JunctureTakeEntityAsAttachmentEx` | 1 | 0 | 1.30: Use JunctureTakeEntityToTargetAttachmentEx instead |
| DEAD | obsolete | `Man.JunctureTakeEntityToCargo` | 1 | 0 | 1.30: Use JunctureTakeEntityToTargetCargo instead |
| DEAD | obsolete | `MiscGameplayFunctions.GenerateAINoiseAtPosition` | 1 | 0 | Use MiscGameplayFunctions.GenerateAINoiseAtPositionEx instead! |
| DEAD | obsolete | `ModInfo.GetDLCImage` | 2 | 0 | 1.30: no replacement |
| DEAD | obsolete | `NoiseAIEvaluate.GetNoiseReduction` | 23 | 0 | Use GetNoiseReductionEx instead! |
| DEAD | obsolete | `OptionsMenu.ToggleDependentOptions` | 4 | 0 | 1.29: No replacement |
| DEAD | obsolete | `OptionsMenuControls.ToggleDependentOptions` | 4 | 0 | 1.29: No replacement |
| DEAD | obsolete | `OptionsMenuGame.ToggleDependentOptions` | 4 | 0 | 1.29: No replacement |
| DEAD | obsolete | `OptionsMenuSounds.ToggleDependentOptions` | 4 | 0 | 1.29: No replacement |
| DEAD | obsolete | `OptionsMenuVideo.ToggleDependentOptions` | 4 | 0 | 1.29: No replacement |
| DEAD | removed | `PluginDiagMenuClient.UpdateUndegroundDiag` | 1 | 0 |  |
| unused | obsolete | `PluginTransmissionAgents.GrowDuringAntibioticsAttack` | 0 | 0 | replaced by GrowDuringMedicalDrugsAttack |
| DEAD | removed | `ScriptConsole.SelectTabByButton` | 1 | 0 |  |
| DEAD | removed | `ScriptConsoleItemsTab.GetItemsClasses` | 1 | 0 |  |
| unused | removed | `ServerBrowserMenuNew.GetOfficalTab` | 0 | 0 |  |
| DEAD | removed | `ServerBrowserMenuNew.GetTabberUI` | 2 | 0 |  |
| unused | removed | `ServerBrowserMenuNew.SwitchToOfficalTab` | 0 | 0 |  |
| unused | removed | `ServerBrowserTab.IsLoadingServers` | 0 | 0 |  |
| DEAD | obsolete | `StaticConstructionMethods.SpawnConstructionMaterialPiles` | 2 | 0 | 1.30: Use SpawnConstructionMaterialPilesEx instead |
| DEAD | obsolete | `Surface.GetStepsParticleID` | 1 | 0 | 1.30: use SurfaceInfo.GetStepParticleId directly |
| DEAD | obsolete | `UndergroundStash.PlaceOnGround` | 1 | 0 | 1.30: No replacement |
| DEAD | obsolete | `VicinityItemManager.GetFixedHeadHeightAdjustment` | 1 | 0 | use MiscGameplayFunctions.GetPlayerHeadHeight instead |
| DEAD | removed | `WeaponDebug.CycleDebugMode` | 1 | 0 |  |
| DEAD | removed | `WeaponDebug.DrawBarrelMemoryPoints` | 1 | 0 |  |
| DEAD | removed | `WeaponDebug.DrawEyePoint` | 1 | 0 |  |
| DEAD | removed | `WeaponDebug.DrawLineOfFire` | 1 | 0 |  |
| DEAD | removed | `WeaponDebug.DrawLineOfFireCameraHybrid` | 1 | 0 |  |
| DEAD | removed | `WeaponDebug.GetEyePointPosition` | 1 | 0 |  |
| DEAD | obsolete | `World.Is3rdPersonDisabled` | 3 | 0 | 1.30: replaced by World::GetThirdPersonViewMode returning current thrid person mode |
| still called | obsolete | `ActionBuildPart.SetBuildingAnimation` | 2 | 1 | 4_world/classes/useractionscomponent/actions/continuous/actiondismantlepart.c:120 |
| still called | removed | `CachedEquipmentItemAttribute.HasParent` | 1 | 1 | 3_game/anim/animphysagent.c:123 |
| still called | obsolete | `CarScript.CheckVitalItem` | 3 | 1 | 4_world/entities/vehicles/carscript.c:2973 |
| still called | obsolete | `ConstructionActionData.GetSlotId` | 31 | 27 | 3_game/entities/entityai.c:4312, 3_game/entities/entityai.c:4372 |
| still called | obsolete | `ConstructionActionData.SetSlotId` | 3 | 1 | 4_world/entities/gardenbase.c:170 |
| still called | removed | `ContentDLC.OnChange` | 23 | 23 | 3_game/tools/uiscriptedmenu.c:484, 4_world/plugins/pluginbase/plugindayzinfecteddebug.c:14 |
| still called | obsolete | `DayZPlayerImplement.GetSurfaceType` | 14 | 21 | 3_game/entities/dayzanimal.c:365, 3_game/global/game.c:1276 |
| still called | obsolete | `DayZPlayerImplementMeleeCombat.IsObstructed` | 1 | 1 | 5_mission/gui/inventorynew/vicinityitemmanager.c:406 |
| still called | obsolete | `EntityAI.CanDisplayAttachmentSlot` | 11 | 15 | 3_game/entities/entityai.c:1804, 3_game/entities/entityai.c:4918 |
| still called | obsolete | `GameInventory.TakeEntityAsAttachment` | 7 | 5 | 3_game/entities/entityai.c:4897, 3_game/entities/entityai.c:4899 |
| still called | obsolete | `GameInventory.TakeEntityAsAttachmentEx` | 7 | 5 | 3_game/entities/entityai.c:4876, 3_game/entities/entityai.c:4878 |
| still called | obsolete | `GameInventory.TakeEntityToCargo` | 5 | 5 | 3_game/entities/entityai.c:4839, 3_game/entities/entityai.c:4841 |
| still called | obsolete | `GameInventory.TakeEntityToCargoEx` | 5 | 3 | 3_game/entities/entityai.c:4860, 3_game/entities/entityai.c:4862 |
| still called | removed | `HudDebugWinCharAgents.DebugGrowAgentsRequest` | 2 | 2 | 5_mission/gui/scriptshuddebug/huddebugwincharagents.c:56, 5_mission/gui/scriptshuddebug/huddebugwincharagents.c:61 |
| still called | removed | `HudDebugWinCharAgents.FitWindow` | 4 | 3 | 5_mission/gui/scriptshuddebug/huddebugwincharlevels.c:12, 5_mission/gui/scriptshuddebug/huddebugwincharlevels.c:113 |
| still called | obsolete | `HudDebugWinCharStats.FitWindow` | 4 | 3 | 5_mission/gui/scriptshuddebug/huddebugwincharlevels.c:12, 5_mission/gui/scriptshuddebug/huddebugwincharlevels.c:113 |
| still called | removed | `InviteMenu.Cancel` | 15 | 14 | 4_world/classes/emotemanager.c:307, 4_world/classes/emotemanager.c:432 |
| still called | removed | `InviteMenu.OnInputDeviceChanged` | 15 | 17 | 5_mission/gui/connectiondialogue.c:45, 5_mission/gui/controlsxboxnew.c:224 |
| still called | removed | `InviteMenu.UpdateControlsElementVisibility` | 23 | 22 | 5_mission/gui/connectiondialogue.c:75, 5_mission/gui/controlsxboxnew.c:67 |
| still called | removed | `InviteMenu.UpdateControlsElements` | 49 | 48 | 5_mission/gui/controlsxboxnew.c:66, 5_mission/gui/controlsxboxnew.c:203 |
| still called | obsolete | `ItemBase.GetDryingIncrement` | 6 | 6 | 4_world/classes/environment/environment.c:1169, 4_world/classes/environment/environment.c:1171 |
| still called | obsolete | `ItemBase.GetSoakingIncrement` | 3 | 3 | 4_world/classes/environment/environment.c:1081, 4_world/classes/environment/environment.c:1221 |
| still called | obsolete | `Man.TakeEntityAsAttachmentExImpl` | 4 | 1 | 3_game/entities/man.c:921 |
| still called | obsolete | `Man.TakeEntityAsAttachmentImpl` | 5 | 1 | 3_game/entities/man.c:904 |
| still called | obsolete | `Man.TakeEntityToCargoImpl` | 5 | 1 | 3_game/entities/man.c:887 |
| still called | obsolete | `MiscGameplayFunctions.BuildCondition` | 1 | 1 | 4_world/static/miscgameplayfunctions.c:988 |
| still called | obsolete | `MiscGameplayFunctions.ComplexBuildCollideCheckClient` | 2 | 4 | 4_world/classes/useractionscomponent/actions/continuous/actionbuildpart.c:115, 4_world/classes/useractionscomponent/actions/continuous/actionbuildpart.c:451 |
| still called | obsolete | `OptionsMenuControls.InitDependentOptionsVisibility` | 1 | 1 | 5_mission/gui/newui/options/optionsmenugame.c:144 |
| still called | obsolete | `OptionsMenuSounds.InitDependentOptionsVisibility` | 1 | 1 | 5_mission/gui/newui/options/optionsmenugame.c:144 |
| still called | obsolete | `OptionsMenuVideo.InitDependentOptionsVisibility` | 1 | 1 | 5_mission/gui/newui/options/optionsmenugame.c:144 |
| still called | obsolete | `Surface.GetWheelParticleID` | 2 | 1 | 4_world/entities/effects/wheelsmoke.c:35 |
| still called | obsolete | `Weather.GetNoiseReductionByWeather` | 6 | 1 | 4_world/static/sensesaievaluate.c:96 |
| still called | obsolete | `World.UpdatePathgraphDoorByAnimationSourceName` | 1 | 1 | 4_world/entities/building/underground/entrance/land_underground_entrance.c:149 |
