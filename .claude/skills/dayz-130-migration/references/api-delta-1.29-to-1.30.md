# Script API delta: DayZ 1.29 → 1.30 (stable 1.30.164078)

<!-- Generated 2026-10-15 from a declaration-level parse of P:\scripts (DayZ 1.30.164078 stable) against the 1.29 extraction (O:\DayZ 1.29 ...\scripts). Paths are relative to P:\scripts. A parser, not the compiler: confirm a line with grep before you quote it. -->

> Generated against 1.30.164078. Its `P:\scripts` line refs were re-pointed to 1.30.164200 on 2026-10-21 (`_shared/tools/remap_vanilla_refs.py`). What 164200 changed on top: `api-delta-1.30.164078-to-164200.md`.



How to read this file: every row is a declaration that exists in one version and not the other, or whose declaration changed. `old`/`new` are normalised signatures: modifiers, return type, name, parameter **types** (names dropped) and defaults. Line refs point at the 1.30 file. **An override whose parameter list no longer matches does not compile** (`marked as override, but there is no function with this name` / `Overloaded function not compatible`), even if the new parameter has a default.

Sections: 1 removed classes · 2 base-class changes · 3 signature changes · 4 removed methods · 5 removed fields · 6 newly [Obsolete] · 7 visibility changes · 8 changed constants · 9 enums · 10 globals · 11 new classes (index)


## 1. Removed classes

| Class | Was in (1.29) | Base |
|---|---|---|
| `BenchmarkConfig` | `5_mission/mission/missionbenchmark.c` | `None` |
| `BenchmarkLocation` | `5_mission/mission/missionbenchmark.c` | `None` |
| `DebugAgentData` | `5_mission/gui/scriptshuddebug/huddebugwincharagents.c` | `None` |
| `GameSettings` | `2_gamelib/settings.c` | `Settings` |
| `MissionBenchmark` | `5_mission/mission/missionbenchmark.c` | `MissionGameplay` |
| `Settings` | `2_gamelib/settings.c` | `None` |
| `SettingsMenu` | `2_gamelib/settings.c` | `MenuBase` |
| `UndergroundDiag` | `4_world/plugins/pluginbase/plugindiagmenu/undergrounddiag.c` | `None` |
| `UndergroundDiagData` | `4_world/plugins/pluginbase/plugindiagmenu/undergrounddiag.c` | `None` |

## 2. Base class changed

| Class | 1.29 base | 1.30 base | File |
|---|---|---|---|
| `ActionActionBuildPartNoTool` | `ActionBuildPart` | `ActionBuildPartHandsGeneric` | `4_world/classes/useractionscomponent/actions/continuous/actionbuildpart.c` |
| `ActionFillBottleSnow` | `ActionFillBottleBase` | `ActionObtainLiquidBase` | `4_world/classes/useractionscomponent/actions/continuous/actionfillbottlesnow.c` |
| `BotGuardHasItemInHands` | `HandGuardBase` | `BotGuardBase` | `4_world/systems/bot/botguards.c` |
| `BoxDraw` | `None` | `UndergroundTriggerDrawBase` | `4_world/plugins/pluginbase/plugindiagmenu/undergrounddiag.c` |
| `BoxDrawGizmo` | `Managed` | `DrawGizmoBase` | `4_world/plugins/pluginbase/plugindiagmenu/undergrounddiag.c` |
| `CarContactData` | `None` | `VehicleContactData` | `4_world/entities/vehicles/carscript.c` |
| `CarHornLongActionInput` | `CarHornShortActionInput` | `VehicleHornLongActionInput` | `4_world/classes/useractionscomponent/actioninput.c` |
| `CarHornShortActionInput` | `ContinuousDefaultActionInput` | `VehicleHornShortActionInput` | `4_world/classes/useractionscomponent/actioninput.c` |
| `Construction` | `None` | `ConstructionBase` | `4_world/classes/basebuilding/construction.c` |
| `ItemPreviewWidget` | `Widget` | `PreviewWidget` | `3_game/gameplay.c` |
| `OptionsAccess` | `Managed` | `Class` | `3_game/gameplay.c` |
| `PlayerPreviewWidget` | `Widget` | `PreviewWidget` | `3_game/gameplay.c` |
| `PlayerStatsPCO_current` | `PlayerStatsPCO_v115` | `PlayerStatsPCO_v130` | `4_world/classes/playerstats/playerstatspco.c` |
| `SlotsIcon` | `LayoutHolder` | `SlotsIconBase` | `5_mission/gui/inventorynew/containeditems/slotsicon.c` |

## 3. Signature changes (overrides break)

One row per method whose set of declarations changed in a class that exists in both versions.


### 3_game

| Class | Method | 1.29 | 1.30 | 1.30 at |
|---|---|---|---|---|
| `AmmoTypesAPI` | `AddExplosionParticleEffect` | `static void AddExplosionParticleEffect(string, string, int)` | `static void AddExplosionParticleEffect(string, string, int, string="")` | `3_game/global/ammotypes.c:78` |
| `Building` | `CanDoorBeClosed` | `bool CanDoorBeClosed(int)` | `bool CanDoorBeClosed(int)`<br>`bool CanDoorBeClosed(int, Object)` | `3_game/entities/building.c:169` |
| `Building` | `CanDoorBeOpened` | `bool CanDoorBeOpened(int, bool=false)` | `bool CanDoorBeOpened(int, bool=false)`<br>`bool CanDoorBeOpened(notnull DoorManipulationParams)` | `3_game/entities/building.c:141` |
| `CGame` | `CreateObject` | `proto native Object CreateObject(string, vector, bool=false, bool=false, bool=true)` | `proto native Object CreateObject(string, vector, bool=false, bool=false, bool=true, bool=false)` | `3_game/global/game.c:691` |
| `CGame` | `CreateStaticObjectUsingP3D` | `proto native Object CreateStaticObjectUsingP3D(string, vector, vector, float=1.0, bool=false)` | `proto native Object CreateStaticObjectUsingP3D(string, vector, vector, float=1.0, bool=false, bool=false)` | `3_game/global/game.c:680` |
| `ContentDLC` | `IsDLCInstalled` | `proto native bool IsDLCInstalled(EDLCId)` | `proto bool IsDLCInstalled(EDLCId)` | `3_game/services/contentdlc.c:42` |
| `CrashDebugData` | `SendData` | `static void SendData(PlayerBase)` | `static void SendData(DayZPlayer)` | `3_game/vehicles/transport.c:1099` |
| `EntityAI` | `ProcessVariables` | `void ProcessVariables()` | `void ProcessVariables(float)` | `3_game/entities/entityai.c:3913` |
| `HumanItemAccessor` | `OnItemInHandsChanged` | `proto native void OnItemInHandsChanged(bool=false)` | `proto native void OnItemInHandsChanged(bool=false, bool=true)` | `3_game/humanitems.c:112` |
| `InventoryItemType` | `InventoryItemType` | `void InventoryItemType()` | `private void InventoryItemType()` | `3_game/inventoryitemtype.c:147` |
| `Transport` | `IsAnyCrewPresent` | `bool IsAnyCrewPresent()` | `bool IsAnyCrewPresent()`<br>`bool IsAnyCrewPresent(TIntArray)` | `3_game/vehicles/transport.c:624` |

### 4_world

| Class | Method | 1.29 | 1.30 | 1.30 at |
|---|---|---|---|---|
| `ActionTarget` | `ActionTarget` | `void ActionTarget(Object, Object, int, vector, float)` | `void ActionTarget(Object, Object, int, vector, float, string="")` | `4_world/classes/useractionscomponent/actiontargets.c:125` |
| `BoatScript` | `CleanupEffects` | `protected void CleanupEffects()` | `void CleanupEffects()` | `4_world/entities/vehicles/boatscript.c:844` |
| `BotGuardHasItemInHands` | `GuardCondition` | `bool GuardCondition(HandEventBase)` | `bool GuardCondition(BotEventBase)` | `4_world/systems/bot/botguards.c:64` |
| `BoxDraw` | `BoxDraw` | `void BoxDraw()` | `void BoxDraw(vector, vector="0 0 0", vector="1 1 1")` | `4_world/plugins/pluginbase/plugindiagmenu/undergrounddiag.c:276` |
| `BoxDraw` | `DrawRotatedBox` | `private void DrawRotatedBox(vector[], vector, int)` | `private void DrawRotatedBox(vector[], int)` | `4_world/plugins/pluginbase/plugindiagmenu/undergrounddiag.c:501` |
| `CarScript` | `PlayCrashHeavySound` | `void PlayCrashHeavySound()` | `protected void PlayCrashHeavySound()` | `4_world/entities/vehicles/carscript.c:461` |
| `CarScript` | `PlayCrashLightSound` | `void PlayCrashLightSound()` | `protected void PlayCrashLightSound()` | `4_world/entities/vehicles/carscript.c:454` |
| `CombinationLock` | `CheckLockedStateServer` | `void CheckLockedStateServer()` | `void CheckLockedStateServer(bool=false)` | `4_world/entities/itembase/combinationlock.c:511` |
| `CombinationLock` | `DestroyLock` | `void DestroyLock()` | `protected void DestroyLock()` | `4_world/entities/itembase/combinationlock.c:546` |
| `CombinationLock` | `DialNextNumber` | `void DialNextNumber()` | `void DialNextNumber(int=FACING_OUTSIDE)` | `4_world/entities/itembase/combinationlock.c:273` |
| `CombinationLock` | `SetCombination` | `void SetCombination(int)` | `void SetCombination(int, bool=true)` | `4_world/entities/itembase/combinationlock.c:233` |
| `CombinationLock` | `SetNextDial` | `void SetNextDial()` | `void SetNextDial(int=FACING_OUTSIDE)` | `4_world/entities/itembase/combinationlock.c:340` |
| `CombinationLock` | `ShuffleLock` | `void ShuffleLock()` | `protected void ShuffleLock()` | `4_world/entities/itembase/combinationlock.c:471` |
| `CombinationLock` | `SoundLockChangeDial` | `void SoundLockChangeDial()` | `protected void SoundLockChangeDial()` | `4_world/entities/itembase/combinationlock.c:840` |
| `CombinationLock` | `SoundLockChangeNumber` | `void SoundLockChangeNumber()` | `protected void SoundLockChangeNumber()` | `4_world/entities/itembase/combinationlock.c:834` |
| `CombinationLock` | `UpdateSound` | `void UpdateSound()` | `protected void UpdateSound()` | `4_world/entities/itembase/combinationlock.c:618` |
| `CombinationLock` | `UpdateVisuals` | `void UpdateVisuals()` | `protected void UpdateVisuals()` | `4_world/entities/itembase/combinationlock.c:552` |
| `Construction` | `Construction` | `void Construction(BaseBuildingBase)` | `void Construction(BaseBuildingBase)`<br>`void Construction(EntityAI)` | `4_world/classes/basebuilding/construction.c:6` |
| `Construction` | `SetParent` | `protected void SetParent(BaseBuildingBase)` | `protected void SetParent(EntityAI)` | `4_world/classes/basebuilding/construction.c:44` |
| `EmoteManager` | `ClearSurrenderState` | `protected void ClearSurrenderState()` | `void ClearSurrenderState()` | `4_world/classes/emotemanager.c:1246` |
| `Environment` | `ApplyDrynessToItemEx` | `protected void ApplyDrynessToItemEx(ItemBase, EnvironmentDrynessData)` | `private void ApplyDrynessToItemEx(EnvironmentDrynessData, notnull map<ItemBase, ref EnvironmentEntityData>itemDataCached)`<br>`protected void ApplyDrynessToItemEx(ItemBase, EnvironmentDrynessData, EnvironmentEntityData=null)` | `4_world/classes/environment/environment.c:1159` |
| `Environment` | `ApplyWetnessToItem` | `protected void ApplyWetnessToItem(ItemBase)` | `private void ApplyWetnessToItem(notnull map<ItemBase, ref EnvironmentEntityData>itemDataCached)`<br>`protected void ApplyWetnessToItem(ItemBase, EnvironmentEntityData=null)` | `4_world/classes/environment/environment.c:1040` |
| `ExplosivesBase` | `AddExplosionEffectForSurface` | `void AddExplosionEffectForSurface(string, int)` | `void AddExplosionEffectForSurface(ExplosionEffectsData)`<br>`void AddExplosionEffectForSurface(string, int)` | `4_world/entities/explosivesbase.c:386` |
| `ItemBase` | `GetDryingIncrement` | `float GetDryingIncrement(string)` | `float GetDryingIncrement(EEnvironmentDryingCategories)`<br>`float GetDryingIncrement(string)` | `4_world/entities/itembase.c:3662` |
| `ItemBase` | `GetSoakingIncrement` | `float GetSoakingIncrement(string)` | `float GetSoakingIncrement(EEnvironmentSoakingCategories)`<br>`float GetSoakingIncrement(string)` | `4_world/entities/itembase.c:3667` |
| `ItemBase` | `ProcessVariables` | `void ProcessVariables()` | `void ProcessVariables(float)` | `4_world/entities/itembase.c:4663` |
| `MapNavigationBehaviour` | `IsOutOfMap` | `static bool IsOutOfMap(EntityAI)` | `static bool IsOutOfMap(EntityAI)`<br>`static bool IsOutOfMap(vector)` | `4_world/entities/itembase/mapnavigationbehaviour.c:188` |
| `PluginSceneManager` | `DeselectObject` | `private void DeselectObject()` | `protected void DeselectObject()` | `4_world/plugins/pluginbase/pluginscenemanager.c:1137` |
| `PluginSceneManager` | `EditorUpdateRuler` | `private void EditorUpdateRuler()` | `protected void EditorUpdateRuler()` | `4_world/plugins/pluginbase/pluginscenemanager.c:1242` |
| `PluginSceneManager` | `GetMousePositionInScene` | `private vector GetMousePositionInScene()` | `protected vector GetMousePositionInScene()` | `4_world/plugins/pluginbase/pluginscenemanager.c:1263` |
| `PluginSceneManager` | `IsMouseInScene` | `private bool IsMouseInScene()` | `protected bool IsMouseInScene()` | `4_world/plugins/pluginbase/pluginscenemanager.c:1227` |
| `PluginSceneManager` | `SceneEditorMenuCmdPopup` | `private void SceneEditorMenuCmdPopup(string)` | `protected void SceneEditorMenuCmdPopup(string)` | `4_world/plugins/pluginbase/pluginscenemanager.c:1211` |
| `PluginSceneManager` | `SceneEditorMenuCmdRefresh` | `private void SceneEditorMenuCmdRefresh()` | `protected void SceneEditorMenuCmdRefresh()` | `4_world/plugins/pluginbase/pluginscenemanager.c:1203` |
| `PluginSceneManager` | `SceneEditorMenuCmdSave` | `private void SceneEditorMenuCmdSave()` | `protected void SceneEditorMenuCmdSave()` | `4_world/plugins/pluginbase/pluginscenemanager.c:1219` |
| `ScriptedLightBase` | `AttachOnObject` | `void AttachOnObject(Object, vector="0 0 0", vector="0 0 0")` | `void AttachOnObject(Object, vector=vector.Zero, vector=vector.Zero)` | `4_world/entities/scriptedlightbase.c:127` |

### 5_mission

| Class | Method | 1.29 | 1.30 | 1.30 at |
|---|---|---|---|---|
| `ActionTargetsCursor` | `BuildFixedCursor` | `protected void BuildFixedCursor()` | `protected void BuildFixedCursor(bool=true)` | `5_mission/gui/actiontargetscursor.c:267` |
| `ActionTargetsCursor` | `Update` | `void Update()` | `void Update(bool=true)` | `5_mission/gui/actiontargetscursor.c:310` |
| `HudDebugWinBase` | `SetValueListSize` | `void SetValueListSize(int, int, TextListboxWidget)` | `void SetValueListSize(int, int, Widget)` | `5_mission/gui/scriptshuddebug/huddebugwinbase.c:87` |
| `HudDebugWinBase` | `Update` | `void Update()` | `void Update(float)` | `5_mission/gui/scriptshuddebug/huddebugwinbase.c:31` |
| `HudDebugWinCharAgents` | `Update` | `void Update()` | `void Update(float)` | `5_mission/gui/scriptshuddebug/huddebugwincharagents.c:139` |
| `HudDebugWinCharDebug` | `Update` | `void Update()` | `void Update(float)` | `5_mission/gui/scriptshuddebug/huddebugwinchardebug.c:27` |
| `HudDebugWinCharLevels` | `Update` | `void Update()` | `void Update(float)` | `5_mission/gui/scriptshuddebug/huddebugwincharlevels.c:87` |
| `HudDebugWinCharModifiers` | `Update` | `void Update()` | `void Update(float)` | `5_mission/gui/scriptshuddebug/huddebugwincharmodifiers.c:86` |
| `HudDebugWinCharStats` | `Update` | `void Update()` | `void Update(float)` | `5_mission/gui/scriptshuddebug/huddebugwincharstats.c:104` |
| `HudDebugWinCharStomach` | `Update` | `void Update()` | `void Update(float)` | `5_mission/gui/scriptshuddebug/huddebugwincharstomach.c:88` |
| `HudDebugWinHealth` | `Update` | `void Update()` | `void Update(float)` | `5_mission/gui/scriptshuddebug/huddebugwinhealth.c:56` |
| `HudDebugWinTemperature` | `Update` | `void Update()` | `void Update(float)` | `5_mission/gui/scriptshuddebug/huddebugwintemperature.c:73` |
| `HudDebugWinVersion` | `Update` | `void Update()` | `void Update(float)` | `5_mission/gui/scriptshuddebug/huddebugwinversion.c:29` |
| `IngameHudVisibility` | `SetContextFlag` | `void SetContextFlag(EHudContextFlags, bool)` | `void SetContextFlag(EHudContextFlags, bool)`<br>`void SetContextFlag(EHudContextFlags, bool, bool)` | `5_mission/gui/ingamehudvisibility.c:90` |
| `IngameHudVisibility` | `UpdateElement` | `protected void UpdateElement(EHudElement)` | `protected void UpdateElement(EHudElement)`<br>`protected void UpdateElement(EHudElement, bool)` | `5_mission/gui/ingamehudvisibility.c:128` |
| `IngameHudVisibility` | `UpdateVisibility` | `protected void UpdateVisibility(EHudElement, bool)` | `protected void UpdateVisibility(EHudElement, bool)`<br>`protected void UpdateVisibility(EHudElement, bool, bool)` | `5_mission/gui/ingamehudvisibility.c:177` |
| `MainMenuConsole` | `OnDLCChange` | `void OnDLCChange(EDLCId)` | `void OnDLCChange()` | `5_mission/gui/newui/mainmenu/mainmenuconsoles.c:153` |
| `ScriptConsole` | `RegisterTab` | `void RegisterTab(ScriptConsoleTabBase)` | `void RegisterTab(int, ScriptConsoleTabBase)` | `5_mission/gui/scriptconsole.c:193` |
| `ScriptConsoleItemsTab` | `ChangeFilter` | `void ChangeFilter(TStringArray, TextListboxWidget, MultilineEditBoxWidget, int=-1, bool=false)` | `void ChangeFilter(TextListboxWidget, MultilineEditBoxWidget, int=-1, bool=false)` | `5_mission/gui/scriptconsoleitemstab.c:939` |
| `ServerBrowserMenuNew` | `Play` | `void Play()` | `void Play(bool=true)` | `5_mission/gui/newui/serverbrowsermenu/serverbrowsermenunew.c:701` |
| `ServerBrowserTab` | `OnDLCChange` | `void OnDLCChange(EDLCId)` | `void OnDLCChange()` | `5_mission/gui/newui/serverbrowsermenu/serverbrowsertab.c:135` |

## 4. Methods removed from a class

`moved to` = the method now lives on that ancestor (calls and same-signature overrides keep compiling). Empty `moved to` = gone from the whole hierarchy: calls and overrides break.


### 3_game

| Class | 1.29 declaration | Moved to |
|---|---|---|
| `AmmoTypesAPI` | `static int GetExplosionParticleID(string, string)` | **gone** |
| `CGame` | `proto native bool GoBuyWorldDLC(string)` | **gone** |
| `CGame` | `proto native bool VerifyWorldOwnership(string)` | **gone** |
| `CachedEquipmentItemAttribute` | `bool HasParent()` | **gone** |
| `ContentDLC` | `void OnChange(EDLCId)` | **gone** |
| `EffVehicleSmoke` | `void EffVehicleSmoke()` | **gone** |
| `GetServersInput` | `void SetIsDLC(bool)` | **gone** |
| `HumanInventory` | `bool TakeEntityAsAttachmentEx(InventoryMode, notnull EntityAI, int)` | `GameInventory` |
| `HumanInventory` | `bool TakeEntityToCargoEx(InventoryMode, notnull EntityAI, int, int, int)` | `GameInventory` |
| `ItemPreviewWidget` | `proto native vector GetModelOrientation()` | `PreviewWidget` |
| `ItemPreviewWidget` | `proto native vector GetModelPosition()` | `PreviewWidget` |
| `ItemPreviewWidget` | `proto native void SetModelOrientation(vector)` | `PreviewWidget` |
| `ItemPreviewWidget` | `proto native void SetModelPosition(vector)` | `PreviewWidget` |
| `Man` | `bool LocalTakeEntityAsAttachment(notnull EntityAI)` | `EntityAI` |
| `Man` | `bool LocalTakeEntityAsAttachmentEx(notnull EntityAI, int)` | `EntityAI` |
| `Man` | `bool LocalTakeEntityToCargo(notnull EntityAI)` | `EntityAI` |
| `Man` | `bool PredictiveTakeEntityAsAttachment(notnull EntityAI)` | `EntityAI` |
| `Man` | `bool PredictiveTakeEntityAsAttachmentEx(notnull EntityAI, int)` | `EntityAI` |
| `Man` | `bool PredictiveTakeEntityToCargo(notnull EntityAI)` | `EntityAI` |
| `Man` | `bool ServerTakeEntityAsAttachment(notnull EntityAI)` | `EntityAI` |
| `Man` | `bool ServerTakeEntityAsAttachmentEx(notnull EntityAI, int)` | `EntityAI` |
| `Man` | `bool ServerTakeEntityToCargo(notnull EntityAI)` | `EntityAI` |
| `PlayerPreviewWidget` | `proto native vector GetModelOrientation()` | `PreviewWidget` |
| `PlayerPreviewWidget` | `proto native vector GetModelPosition()` | `PreviewWidget` |
| `PlayerPreviewWidget` | `proto native void SetModelOrientation(vector)` | `PreviewWidget` |
| `PlayerPreviewWidget` | `proto native void SetModelPosition(vector)` | `PreviewWidget` |

### 4_world

| Class | 1.29 declaration | Moved to |
|---|---|---|
| `ActionActionBuildPartNoTool` | `typename GetInputType()` | `ActionContinuousBase` |
| `ActionActionBuildPartNoTool` | `bool HasAlternativeInterrupt()` | `ActionContinuousBase` |
| `ActionActionBuildPartNoTool` | `bool HasProgress()` | `ActionBase` |
| `ActionActionBuildPartNoTool` | `bool UseMainItem()` | `ActionBuildPartHandsGeneric` |
| `ActionDismantlePart` | `string GetText()` | `ActionBase` |
| `ActionGetInTransport` | `void OnStartServer(ActionData)` | `ActionBase` |
| `Ammo_40mm_Explosive` | `bool ShootsExplosiveAmmo()` | `Object` |
| `BadgeHeartbeat` | `void UpdateHUD()` | `DisplayElementBadge` |
| `BarrelHoles_ColorBase` | `string GetDeploySoundset()` | `ItemBase` |
| `Barrel_ColorBase` | `string GetDeploySoundset()` | `ItemBase` |
| `BaseBuildingBase` | `bool CanReceiveAttachment(EntityAI, int)` | `ItemBase` |
| `BaseBuildingBase` | `void ConstructionInit()` | `EntityAI` (signature differs there: `protected void ConstructionInit()`) |
| `BaseBuildingBase` | `void EEInit()` | `EntityAI` |
| `BaseBuildingBase` | `bool IsFacingCamera(string)` | `EntityAI` |
| `BatteryCharger` | `string GetDeploySoundset()` | `ItemBase` |
| `BoatScript` | `void EEDelete(EntityAI)` | `Transport` |
| `BoatScript` | `void ~BoatScript()` | **gone** |
| `BoxDraw` | `vector InverseTransformPointByMatrix(vector[], vector)` | **gone** |
| `BoxDraw` | `private void OnGlobalRotationChanged(vector[])` | **gone** |
| `BoxDraw` | `vector TransformPointByMatrix(vector[], vector)` | **gone** |
| `BoxDrawGizmo` | `void BoxDrawGizmo(vector)` | **gone** |
| `BoxDrawGizmo` | `vector GetPosition()` | `DrawGizmoBase` |
| `BoxDrawGizmo` | `void GetTransform(inout vector[])` | `DrawGizmoBase` |
| `BoxDrawGizmo` | `void Gizmo_GetWorldTransform(vector[])` | `DrawGizmoBase` |
| `BoxDrawGizmo` | `bool Gizmo_IsSupported()` | `DrawGizmoBase` |
| `BoxDrawGizmo` | `void Gizmo_SetWorldTransform(vector[], bool)` | `DrawGizmoBase` |
| `BoxDrawGizmo` | `void SetPosition(vector)` | `DrawGizmoBase` |
| `BoxDrawGizmo` | `void SetTransform(vector[])` | `DrawGizmoBase` |
| `CableReel` | `string GetDeploySoundset()` | `ItemBase` |
| `CarContactData` | `void CarContactData(vector, IEntity, float)` | **gone** |
| `CarHornLongActionInput` | `void CarHornLongActionInput(PlayerBase)` | **gone** |
| `CarHornShortActionInput` | `void CarHornShortActionInput(PlayerBase)` | **gone** |
| `CarHornShortActionInput` | `ActionBase GetAction()` | `VehicleHornShortActionInput` |
| `CarHornShortActionInput` | `void UpdatePossibleActions(PlayerBase, ActionTarget, ItemBase, int)` | `VehicleHornShortActionInput` |
| `CarScript` | `bool CanPutIntoHands(EntityAI)` | `Transport` |
| `CarScript` | `void EEDelete(EntityAI)` | `Transport` |
| `CarScript` | `void EEHitBy(TotalDamageResult, int, EntityAI, int, string, string, vector, float)` | `Transport` |
| `CarScript` | `void EEInit()` | `EntityAI` |
| `CarScript` | `protected void GenerateCarHornAINoise(int)` | **gone** |
| `CarScript` | `string GetAnimSourceFromSelection(string)` | `Transport` |
| `CarScript` | `bool GetCrashHeavySound()` | `Transport` (signature differs there: `protected bool GetCrashHeavySound()`) |
| `CarScript` | `bool GetCrashLightSound()` | `Transport` (signature differs there: `protected bool GetCrashLightSound()`) |
| `CarScript` | `string GetSelectionFromAnimSource(string)` | `Transport` (signature differs there: `protected string GetSelectionFromAnimSource(string)`) |
| `CarScript` | `bool IsVitalFuelTank()` | `Transport` |
| `CarScript` | `bool IsVitalGlowPlug()` | `Transport` |
| `CarScript` | `void OnContact(string, vector, IEntity, Contact)` | `Transport` |
| `CarScript` | `void PlaySound(string, EffectSound, out bool)` | `Transport` |
| `CarScript` | `void PlaySoundEx(string, EffectSound, out bool)` | `Transport` (signature differs there: `protected void PlaySoundEx(string, EffectSound, out bool)`) |
| `CarScript` | `void SynchCrashHeavySound(bool)` | `Transport` (signature differs there: `protected void SynchCrashHeavySound(bool)`) |
| `CarScript` | `void SynchCrashLightSound(bool)` | `Transport` (signature differs there: `protected void SynchCrashLightSound(bool)`) |
| `CarScript` | `void ToggleHeadlights()` | **gone** |
| `CarScript` | `void ~CarScript()` | **gone** |
| `CarWheel` | `void OnWasAttached(EntityAI, int)` | `ItemBase` |
| `CarWheel` | `void OnWasDetached(EntityAI, int)` | `ItemBase` |
| `CivilianSedan` | `CarLightBase CreateFrontLight()` | `CarScript` (signature differs there: `protected CarLightBase CreateFrontLight()`) |
| `CivilianSedan` | `CarRearLightBase CreateRearLight()` | `CarScript` (signature differs there: `protected CarRearLightBase CreateRearLight()`) |
| `CivilianSedan` | `bool IsVitalTruckBattery()` | `CarScript` |
| `CombinationLock` | `void UnlockServer(EntityAI, EntityAI)` | **gone** |
| `Construction` | `void AddToConstructedParts(string)` | `ConstructionBase` |
| `Construction` | `void BuildPartServer(notnull Man, string, int)` | `ConstructionBase` |
| `Construction` | `bool CanBuildPart(string, ItemBase, bool)` | `ConstructionBase` |
| `Construction` | `bool CanDestroyPart(string)` | `ConstructionBase` |
| `Construction` | `bool CanDismantlePart(string, ItemBase)` | `ConstructionBase` |
| `Construction` | `bool CanUseToolToBuildPart(string, ItemBase)` | `ConstructionBase` |
| `Construction` | `bool CanUseToolToDismantlePart(string, ItemBase)` | `ConstructionBase` |
| `Construction` | `void CreateCollisionTrigger(string, vector[], vector)` | `ConstructionBase` |
| `Construction` | `void DestroyCollisionTrigger()` | `ConstructionBase` |
| `Construction` | `void DestroyConnectedParts(string)` | `ConstructionBase` |
| `Construction` | `protected void DestroyDebugCollisionBox()` | `ConstructionBase` |
| `Construction` | `protected void DestroyMaterialsServer(Man, string)` | `ConstructionBase` |
| `Construction` | `void DismantlePartServer(notnull Man, string, int)` | `ConstructionBase` |
| `Construction` | `protected void DrawDebugCollisionBox(vector[], int)` | `ConstructionBase` |
| `Construction` | `void DropNonUsableMaterialsServer(Man, string)` | `ConstructionBase` |
| `Construction` | `ConstructionPart GetBaseConstructionPart()` | `ConstructionBase` |
| `Construction` | `vector GetBoxCenter(vector[])` | `ConstructionBase` |
| `Construction` | `protected void GetCollisionBoxData(string, out vector[])` | `ConstructionBase` |
| `Construction` | `vector GetCollisionBoxSize(vector[])` | `ConstructionBase` |
| `Construction` | `ConstructionPart GetConstructionPart(string)` | `ConstructionBase` |
| `Construction` | `ConstructionPart GetConstructionPartToDestroy(string)` | `ConstructionBase` |
| `Construction` | `ConstructionPart GetConstructionPartToDismantle(string, ItemBase)` | `ConstructionBase` |
| `Construction` | `void GetConstructionPartsToBuild(string, out array<ConstructionPart>construction_parts, ItemBase, out string, bool)` | `ConstructionBase` |
| `Construction` | `ConstructionMaterialType GetMaterialType(string)` | `ConstructionBase` |
| `Construction` | `void GetTriggerExtents(vector[], out vector[])` | `ConstructionBase` |
| `Construction` | `protected bool HasConflictPart(string)` | `ConstructionBase` (signature differs there: `bool HasConflictPart(string)`; `bool HasConflictPart(string, out string)`) |
| `Construction` | `bool HasDependentPart(string)` | `ConstructionBase` |
| `Construction` | `protected bool HasMaterialWithQuantityAttached(string, float)` | `ConstructionBase` |
| `Construction` | `bool HasMaterials(string, bool=false)` | `ConstructionBase` |
| `Construction` | `protected bool HasRequiredPart(string)` | `ConstructionBase` (signature differs there: `bool HasRequiredPart(string)`) |
| `Construction` | `protected void HideConstructionPart(string)` | `ConstructionBase` |
| `Construction` | `void HideConstructionPartPhysics(string)` | `ConstructionBase` |
| `Construction` | `void InitVisuals()` | `ConstructionBase` |
| `Construction` | `bool IsColliding(string)` | `ConstructionBase` |
| `Construction` | `bool IsCollidingEx(CollisionCheckData)` | `ConstructionBase` |
| `Construction` | `bool IsPartConstructed(string)` | `ConstructionBase` |
| `Construction` | `bool IsTriggerColliding()` | `ConstructionBase` |
| `Construction` | `bool MaterialIsRuined(string)` | `ConstructionBase` |
| `Construction` | `protected void ReceiveMaterialsServer(notnull Man, string, string)` | `ConstructionBase` |
| `Construction` | `void RemoveFromConstructedParts(string)` | `ConstructionBase` |
| `Construction` | `void SetLockOnAttachedMaterials(string, bool)` | `ConstructionBase` |
| `Construction` | `protected void ShowConstructionPart(string)` | `ConstructionBase` |
| `Construction` | `void ShowConstructionPartPhysics(string)` | `ConstructionBase` |
| `Construction` | `void TakeMaterialsServer(string, bool=false)` | `ConstructionBase` |
| `Construction` | `protected void UpdateConstructionParts()` | `ConstructionBase` |
| `Construction` | `void UpdatePhysics()` | `ConstructionBase` |
| `Construction` | `void UpdateVisuals()` | `ConstructionBase` |
| `EffWheelSmoke` | `void EffWheelSmoke()` | **gone** |
| `EffectArea` | `void OnCEUpdate()` | `EntityAI` |
| `ExplosivesBase` | `void EEDelete(EntityAI)` | `ItemBase` |
| `ExplosivesBase` | `void OnCEUpdate()` | `EntityAI` |
| `FirearmActionDetachMagazine` | `void FirearmActionDetachMagazine()` | **gone** |
| `FireplaceBase` | `string GetDeploySoundset()` | `ItemBase` |
| `Flag_Base` | `void AfterStoreLoad()` | `ItemBase` |
| `Flag_Base` | `bool OnStoreLoad(ParamsReadContext, int)` | `ItemBase` |
| `Flag_Base` | `void OnStoreSave(ParamsWriteContext)` | `ItemBase` |
| `Flag_Base` | `void OnVariablesSynchronized()` | `ItemBase` |
| `Flag_Base` | `void ~Flag_Base()` | **gone** |
| `Grenade_Base` | `void InitSpecificsExplosionEffectForSurface()` | `ExplosivesBase` |
| `Hatchback_02` | `CarLightBase CreateFrontLight()` | `CarScript` (signature differs there: `protected CarLightBase CreateFrontLight()`) |
| `Hatchback_02` | `CarRearLightBase CreateRearLight()` | `CarScript` (signature differs there: `protected CarRearLightBase CreateRearLight()`) |
| `Hatchback_02` | `bool IsVitalTruckBattery()` | `CarScript` |
| `HeavyMetalMdfr` | `protected void OnActivate(PlayerBase)` | `ModifierBase` (signature differs there: `void OnActivate(PlayerBase)`) |
| `HeavyMetalMdfr` | `protected void OnDeactivate(PlayerBase)` | `ModifierBase` (signature differs there: `void OnDeactivate(PlayerBase)`) |
| `InfectedSoundEventBase` | `void ~InfectedSoundEventBase()` | **gone** |
| `ItemBase` | `void OverrideActionAnimation(typename, int, int=-1, int=-1)` | `EntityAI` |
| `ItemBase` | `void SetActionAnimOverrides()` | `EntityAI` |
| `KobraOptic` | `bool IsSightOnly()` | `ItemOptics` |
| `OffroadHatchback` | `CarLightBase CreateFrontLight()` | `CarScript` (signature differs there: `protected CarLightBase CreateFrontLight()`) |
| `OffroadHatchback` | `CarRearLightBase CreateRearLight()` | `CarScript` (signature differs there: `protected CarRearLightBase CreateRearLight()`) |
| `OffroadHatchback` | `bool IsVitalTruckBattery()` | `CarScript` |
| `Offroad_02` | `CarLightBase CreateFrontLight()` | `CarScript` (signature differs there: `protected CarLightBase CreateFrontLight()`) |
| `Offroad_02` | `CarRearLightBase CreateRearLight()` | `CarScript` (signature differs there: `protected CarRearLightBase CreateRearLight()`) |
| `Offroad_02` | `bool IsVitalTruckBattery()` | `CarScript` |
| `PlayerBase` | `protected bool TakeEntityAsAttachmentImpl(InventoryMode, notnull EntityAI)` | `Man` |
| `PlayerBase` | `protected bool TakeEntityToCargoImpl(InventoryMode, notnull EntityAI)` | `Man` |
| `PluginConfigHandler` | `void OnInit()` | `PluginFileHandler` |
| `PluginDiagMenuClient` | `void UpdateUndegroundDiag(float)` | **gone** |
| `PowerGeneratorBase` | `string GetDeploySoundset()` | `ItemBase` |
| `RemoteDetonatorTrigger` | `void OnCEUpdate()` | `EntityAI` |
| `Sedan_02` | `CarLightBase CreateFrontLight()` | `CarScript` (signature differs there: `protected CarLightBase CreateFrontLight()`) |
| `Sedan_02` | `CarRearLightBase CreateRearLight()` | `CarScript` (signature differs there: `protected CarRearLightBase CreateRearLight()`) |
| `Sedan_02` | `bool IsVitalTruckBattery()` | `CarScript` |
| `ShelterSite` | `bool CanUseHandConstruction()` | `EntityAI` |
| `ShelterSite` | `void ShelterSite()` | **gone** |
| `Shemag_ColorBase` | `void Shemag_ColorBase()` | **gone** |
| `SurrenderDummyItem` | `void ~SurrenderDummyItem()` | **gone** |
| `TerritoryFlag` | `void OnCEUpdate()` | `EntityAI` |
| `Truck_01_Base` | `CarLightBase CreateFrontLight()` | `CarScript` (signature differs there: `protected CarLightBase CreateFrontLight()`) |
| `Truck_01_Base` | `CarRearLightBase CreateRearLight()` | `CarScript` (signature differs there: `protected CarRearLightBase CreateRearLight()`) |
| `Truck_01_Base` | `bool IsVitalCarBattery()` | `CarScript` |
| `Truck_02` | `bool IsVitalCarBattery()` | `CarScript` |
| `Van_01` | `bool IsVitalTruckBattery()` | `CarScript` |
| `WeaponDebug` | `void CycleDebugMode()` | **gone** |
| `WeaponDebug` | `void DrawBarrelMemoryPoints(vector, vector)` | **gone** |
| `WeaponDebug` | `void DrawEyePoint(Weapon)` | **gone** |
| `WeaponDebug` | `void DrawLineOfFire(vector, vector)` | **gone** |
| `WeaponDebug` | `void DrawLineOfFireCameraHybrid(vector, vector, vector, vector)` | **gone** |
| `WeaponDebug` | `vector GetEyePointPosition(Weapon)` | **gone** |
| `Well` | `bool IsWell()` | `Object` |

### 5_mission

| Class | 1.29 declaration | Moved to |
|---|---|---|
| `CrossHair` | `void ~CrossHair()` | **gone** |
| `DayZIntroScenePC` | `void DayZIntroScenePC()` | **gone** |
| `HudDebugWinCharAgents` | `void DebugGrowAgentsRequest(int, bool)` | **gone** |
| `HudDebugWinCharAgents` | `void FitWindow()` | **gone** |
| `InviteMenu` | `void Cancel()` | **gone** |
| `InviteMenu` | `bool OnClick(Widget, int, int, int)` | `UIScriptedMenu` |
| `InviteMenu` | `protected void OnInputDeviceChanged(EInputDeviceType)` | **gone** |
| `InviteMenu` | `protected void OnInputPresetChanged()` | **gone** |
| `InviteMenu` | `void Update(float)` | `UIScriptedMenu` |
| `InviteMenu` | `protected void UpdateControlsElementVisibility()` | **gone** |
| `InviteMenu` | `protected void UpdateControlsElements()` | **gone** |
| `ScriptConsole` | `void SelectTabByButton(Widget)` | **gone** |
| `ScriptConsoleCameraTab` | `void ScriptConsoleCameraTab(Widget, ScriptConsole, Widget, ScriptConsoleTabBase=null)` | **gone** |
| `ScriptConsoleConfigTab` | `void ScriptConsoleConfigTab(Widget, ScriptConsole, Widget, ScriptConsoleTabBase=null)` | **gone** |
| `ScriptConsoleEnfScriptTab` | `void ScriptConsoleEnfScriptTab(Widget, ScriptConsole, Widget, ScriptConsoleTabBase=null)` | **gone** |
| `ScriptConsoleGeneralTab` | `void ScriptConsoleGeneralTab(Widget, ScriptConsole, Widget, ScriptConsoleTabBase=null)` | **gone** |
| `ScriptConsoleItemsTab` | `TStringArray GetItemsClasses()` | **gone** |
| `ScriptConsoleItemsTab` | `void ScriptConsoleItemsTab(Widget, ScriptConsole, Widget, ScriptConsoleTabBase=null)` | **gone** |
| `ScriptConsoleOutputTab` | `void ScriptConsoleOutputTab(Widget, ScriptConsole, Widget, ScriptConsoleTabBase=null)` | **gone** |
| `ScriptConsoleSoundsTab` | `void ScriptConsoleSoundsTab(Widget, ScriptConsole, Widget, ScriptConsoleTabBase=null)` | **gone** |
| `ScriptConsoleTabBase` | `void ScriptConsoleTabBase(Widget, ScriptConsole, Widget, ScriptConsoleTabBase=null)` | **gone** |
| `ScriptConsoleVicinityTab` | `void ScriptConsoleVicinityTab(Widget, ScriptConsole, Widget, ScriptConsoleTabBase=null)` | **gone** |
| `ScriptConsoleWeatherTab` | `void ScriptConsoleWeatherTab(Widget, ScriptConsole, Widget, ScriptConsoleTabBase=null)` | **gone** |
| `ServerBrowserMenuNew` | `ServerBrowserTab GetOfficalTab()` | **gone** |
| `ServerBrowserMenuNew` | `TabberUI GetTabberUI()` | **gone** |
| `ServerBrowserMenuNew` | `protected void SwitchToOfficalTab()` | **gone** |
| `ServerBrowserTab` | `bool IsLoadingServers()` | **gone** |
| `SlotsIcon` | `void CheckHasQuantity()` | `SlotsIconBase` |
| `SlotsIcon` | `void CheckHasTemperature()` | `SlotsIconBase` |
| `SlotsIcon` | `void CheckIsMagazine()` | `SlotsIconBase` |
| `SlotsIcon` | `void CheckIsWeapon()` | `SlotsIconBase` |
| `SlotsIcon` | `ImageWidget GetAmmoIcon()` | `SlotsIconBase` |
| `SlotsIcon` | `Widget GetColWidget()` | `SlotsIconBase` |
| `SlotsIcon` | `Widget GetColorWidget()` | `SlotsIconBase` |
| `SlotsIcon` | `Container GetContainer()` | `SlotsIconBase` |
| `SlotsIcon` | `Widget GetCursorWidget()` | `SlotsIconBase` |
| `SlotsIcon` | `Widget GetEmptySelectedPanel()` | `SlotsIconBase` |
| `SlotsIcon` | `EntityAI GetEntity()` | `SlotsIconBase` |
| `SlotsIcon` | `ImageWidget GetGhostSlot()` | `SlotsIconBase` |
| `SlotsIcon` | `ItemBase GetItem()` | `SlotsIconBase` |
| `SlotsIcon` | `Widget GetItemSizePanel()` | `SlotsIconBase` |
| `SlotsIcon` | `TextWidget GetItemSizeWidget()` | `SlotsIconBase` |
| `SlotsIcon` | `Widget GetMicromanagedPanel()` | `SlotsIconBase` |
| `SlotsIcon` | `Widget GetMountedWidget()` | `SlotsIconBase` |
| `SlotsIcon` | `static int GetNormalHeight()` | `SlotsIconBase` |
| `SlotsIcon` | `static int GetNormalWidth()` | `SlotsIconBase` |
| `SlotsIcon` | `Object GetObject()` | `SlotsIconBase` |
| `SlotsIcon` | `Widget GetOutOfReachWidget()` | `SlotsIconBase` |
| `SlotsIcon` | `Widget GetPanelWidget()` | `SlotsIconBase` |
| `SlotsIcon` | `TextWidget GetQuantityItem()` | `SlotsIconBase` |
| `SlotsIcon` | `Widget GetQuantityPanel()` | `SlotsIconBase` |
| `SlotsIcon` | `ProgressBarWidget GetQuantityProgress()` | `SlotsIconBase` |
| `SlotsIcon` | `Widget GetQuantityStack()` | `SlotsIconBase` |
| `SlotsIcon` | `Widget GetRadialIcon()` | `SlotsIconBase` |
| `SlotsIcon` | `Widget GetRadialIconClosed()` | `SlotsIconBase` |
| `SlotsIcon` | `Widget GetRadialIconPanel()` | `SlotsIconBase` |
| `SlotsIcon` | `ItemPreviewWidget GetRender()` | `SlotsIconBase` |
| `SlotsIcon` | `Widget GetReservedWidget()` | `SlotsIconBase` |
| `SlotsIcon` | `Widget GetSelectedPanel()` | `SlotsIconBase` |
| `SlotsIcon` | `string GetSlotDesc()` | `SlotsIconBase` |
| `SlotsIcon` | `string GetSlotDisplayName()` | `SlotsIconBase` |
| `SlotsIcon` | `int GetSlotID()` | `SlotsIconBase` |
| `SlotsIcon` | `EntityAI GetSlotParent()` | `SlotsIconBase` |
| `SlotsIcon` | `void Init(EntityAI, bool=false)` | `SlotsIconBase` |
| `SlotsIcon` | `bool IsFocused()` | `SlotsIconBase` |
| `SlotsIcon` | `bool IsOutOfReach()` | `SlotsIconBase` |
| `SlotsIcon` | `bool IsReserved()` | `SlotsIconBase` |
| `SlotsIcon` | `bool IsVisible()` | `SlotsIconBase` |
| `SlotsIcon` | `void Refresh()` | `SlotsIconBase` |
| `SlotsIcon` | `void RefreshMuzzleIcon()` | `SlotsIconBase` |
| `SlotsIcon` | `void SetActive(bool)` | `SlotsIconBase` |
| `SlotsIcon` | `void SetContainer(Container)` | `SlotsIconBase` |
| `SlotsIcon` | `void SetItemPreview()` | `SlotsIconBase` |
| `SlotsIcon` | `void SetItemSize()` | `SlotsIconBase` |
| `SlotsIcon` | `void SetLayoutName()` | `SlotsIconBase` |
| `SlotsIcon` | `void SetQuantity()` | `SlotsIconBase` |
| `SlotsIcon` | `void SetSlotDesc(string)` | `SlotsIconBase` |
| `SlotsIcon` | `void SetSlotDisplayName(string)` | `SlotsIconBase` |
| `SlotsIcon` | `void SetSlotID(int)` | `SlotsIconBase` |
| `SlotsIcon` | `void SetSlotParent(EntityAI)` | `SlotsIconBase` |
| `SlotsIcon` | `void SetTemperature()` | `SlotsIconBase` |
| `SlotsIcon` | `void SlotsIcon(LayoutHolder, Widget, int, EntityAI)` | **gone** |
| `SlotsIcon` | `void UpdateFlip(bool)` | `SlotsIconBase` |
| `SlotsIcon` | `void UpdateInterval()` | `SlotsIconBase` |
| `SlotsIcon` | `void ~SlotsIcon()` | **gone** |

## 5. Fields removed from a class


### 3_game

| Class | 1.29 field | Moved to |
|---|---|---|
| `ContentDLC` | `ScriptInvoker m_OnChange` | **gone** |
| `EffectParticle` | `bool m_ForceRotationRelativeToWorld` | **gone** |
| `EntityAI` | `bool m_CanDisplayWeight` | **gone** |
| `EntityAI` | `float m_ConfigWeight` | **gone** |
| `EntityAI` | `float m_ElapsedSinceLastUpdate` | **gone** |
| `EntityAI` | `float m_LastUpdatedTime` | **gone** |
| `EntityAI` | `float m_VarHeatPermeabilityCoef` | **gone** |
| `EntityAI` | `float m_VarTemperatureFreezeThreshold` | **gone** |
| `EntityAI` | `float m_VarTemperatureFreezeTime` | **gone** |
| `EntityAI` | `float m_VarTemperatureInit` | **gone** |
| `EntityAI` | `float m_VarTemperatureMax` | **gone** |
| `EntityAI` | `float m_VarTemperatureMin` | **gone** |
| `EntityAI` | `float m_VarTemperatureOverheatTime` | **gone** |
| `EntityAI` | `float m_VarTemperatureThawThreshold` | **gone** |
| `EntityAI` | `float m_VarTemperatureThawTime` | **gone** |
| `GetServersInput` | `bool m_IsDLC` | **gone** |
| `GetServersInput` | `bool m_UseIsDLC` | **gone** |
| `GetServersResultRow` | `bool m_IsDLC` | **gone** |
| `World` | `int DM_GAME_WORLD_SIMUL_ACCELERATION` | **gone** |

### 4_world

| Class | 1.29 field | Moved to |
|---|---|---|
| `BaseBuildingBase` | `m_Construction`: type `Construction` → `ConstructionBasic` | type changed |
| `BatteryCharger` | `string DEFAULT_MATERIAL` | **gone** |
| `BatteryCharger` | `string GREEN_LIGHT_GLOW` | **gone** |
| `BatteryCharger` | `string RED_LIGHT_GLOW` | **gone** |
| `BatteryCharger` | `string SWITCH_LIGHT_GLOW` | **gone** |
| `BatteryCharger` | `string YELLOW_LIGHT_GLOW` | **gone** |
| `BoatScript` | `VehicleContactData m_ContactData` | `Transport` |
| `BoatScript` | `float m_MomentumPrevTick` | `Transport` |
| `BoatScript` | `vector m_VelocityPrevTick` | `Transport` |
| `Bot` | `BotFSM m_FSM` | **gone** |
| `BoxDraw` | `BoxDrawGizmo m_AA` | **gone** |
| `BoxDraw` | `BoxDrawGizmo m_BB` | **gone** |
| `BoxDraw` | `BoxDrawGizmo m_Global` | **gone** |
| `BoxDraw` | `PlayerBase m_Player` | **gone** |
| `BoxDrawGizmo` | `ScriptInvoker m_OnRotationChanged` | **gone** |
| `BoxDrawGizmo` | `vector m_Position` | **gone** |
| `BoxDrawGizmo` | `vector m_Rotation` | `DrawGizmoBase` |
| `BoxDrawGizmo` | `vector m_Transform` | `DrawGizmoBase` |
| `CarContactData` | `float impulse` | **gone** |
| `CarContactData` | `vector localPos` | **gone** |
| `CarContactData` | `IEntity other` | **gone** |
| `CarHornShortActionInput` | `ActionTarget targetNew` | `VehicleHornShortActionInput` |
| `CarScript` | `EVehicleDebugOutputType DEBUG_OUTPUT_TYPE` | `Transport` |
| `CarScript` | `CarContactCache m_ContactCache` | `Transport` |
| `CarScript` | `bool m_ContactCalled` | `Transport` |
| `CarScript` | `EffectSound m_CrashSoundHeavy` | `Transport` |
| `CarScript` | `EffectSound m_CrashSoundLight` | `Transport` |
| `CarScript` | `bool m_EngineDestroyed` | `Transport` |
| `CarScript` | `bool m_EngineStarted` | `Transport` |
| `CarScript` | `bool m_ForceUpdateLights` | **gone** |
| `CarScript` | `int m_HeadlightsState` | `Transport` |
| `CarScript` | `float m_MomentumPrevTick` | `Transport` |
| `CarScript` | `bool m_PlayCrashSoundHeavy` | `Transport` |
| `CarScript` | `bool m_PlayCrashSoundLight` | `Transport` |
| `CarScript` | `int m_RearLightType` | `Transport` |
| `CarScript` | `vector m_VelocityPrevTick` | `Transport` |
| `CarScript` | `float m_dmgContactCoef` | `Transport` |
| `CarWheel` | `string m_AttachSound` | **gone** |
| `CarWheel` | `string m_DetachSound` | **gone** |
| `Construction` | `float DECONSTURCT_MATERIAL_LOSS` | `ConstructionBase` |
| `Construction` | `float REPAIR_MATERIAL_PERCENTAGE` | `ConstructionBase` |
| `Construction` | `Shape m_CollisionBox` | `ConstructionBase` |
| `Construction` | `ConstructionBoxTrigger m_ConstructionBoxTrigger` | `ConstructionBase` |
| `DayZPlayerImplement` | `m_NextVoNNoiseTime`: type `int` → `int m_StepCounterVegetation int` | type changed |
| `Fence` | `string ATTSLOT_BARBEDWIRE_DOWN` | **gone** |
| `Fence` | `string ATTSLOT_BARBEDWIRE_UP` | **gone** |
| `Fence` | `string ATTSLOT_CAMONET` | **gone** |
| `ItemBase` | `bool m_CanBeDigged` | **gone** |
| `ItemBase` | `bool m_CanShowQuantity` | **gone** |
| `ItemBase` | `int m_CleannessInit` | **gone** |
| `ItemBase` | `int m_CleannessMax` | **gone** |
| `ItemBase` | `int m_CleannessMin` | **gone** |
| `ItemBase` | `int m_Count` | **gone** |
| `ItemBase` | `bool m_HasQuantityBar` | **gone** |
| `ItemBase` | `TStringArray m_HeadHidingSelections` | **gone** |
| `ItemBase` | `bool m_HideSelectionsBySlot` | **gone** |
| `ItemBase` | `float m_ItemAttachOffset` | **gone** |
| `ItemBase` | `int m_ItemBehaviour` | **gone** |
| `ItemBase` | `float m_ItemModelLength` | **gone** |
| `ItemBase` | `int m_LiquidContainerMask` | **gone** |
| `ItemBase` | `string m_LockSoundSet` | **gone** |
| `ItemBase` | `int m_LockType` | **gone** |
| `ItemBase` | `int m_QuickBarBonus` | **gone** |
| `ItemBase` | `float m_TemperaturePerQuantityWeight` | **gone** |
| `ItemBase` | `int m_VarQuantityInit` | **gone** |
| `ItemBase` | `int m_VarQuantityMax` | **gone** |
| `ItemBase` | `int m_VarQuantityMin` | **gone** |
| `ItemBase` | `float m_VarStackMax` | **gone** |
| `ItemBase` | `float m_VarWetInit` | **gone** |
| `ItemBase` | `float m_VarWetMax` | **gone** |
| `ItemBase` | `float m_VarWetMin` | **gone** |
| `PlayerBase` | `m_EffectWidgets`: type `PlayerStat<int>m_StatWet PlayerStat<int>m_StatBloodType PlayerStat<float>m_StatDiet PlayerStat<float>m_StatStamina PlayerStat<float>m_StatSpecialty PlayerStat<float>m_StatHeatBuffer GameplayEffectWidgets_base` → `GameplayEffectWidgets_base` | type changed |
| `PluginDiagMenuClient` | `UndergroundDiag m_UndergroundDiag` | **gone** |
| `SurrenderDummyItem` | `PlayerBase m_player` | **gone** |
| `WeaponDebug` | `float CAMERA_BULLET_ORIGIN_OFFSET` | **gone** |
| `WeaponDebug` | `float CAMERA_TRACE_MIN_DISTANCE_TOLERANCE` | **gone** |
| `WeaponDebug` | `float COLLISIONS_DISTANCE_TOLERANCE` | **gone** |
| `WeaponDebug` | `float MAX_MUZZLE_DISTANCE_TOLERANCE` | **gone** |
| `WeaponDebug` | `Shape m_HitShape2` | **gone** |
| `WeaponDebug` | `Shape m_HitShape3` | **gone** |
| `WeaponDebug` | `Shape m_HitShape4` | **gone** |
| `WeaponDebug` | `bool m_IsLMBPressed` | **gone** |
| `WeaponDebug` | `bool m_IsToggleKeyPressed` | **gone** |
| `WeaponDebug` | `Shape m_PermanentShape1` | **gone** |
| `WeaponDebug` | `Shape m_PermanentShape2` | **gone** |
| `WeaponDebug` | `Shape m_ShapeEye` | **gone** |
| `WeaponDebug` | `Shape m_ShapeFireDirection1` | **gone** |
| `WeaponDebug` | `Shape m_ShapeFireDirection2` | **gone** |
| `WeaponDebug` | `Shape m_Shape_konec` | **gone** |
| `WeaponDebug` | `Shape m_Shape_usti` | **gone** |
| `WeaponDebug` | `Weapon m_WeaponInHands` | **gone** |
| `WeaponDebug` | `m_CurrentMode`: type `eDebugMode` → `eWeaponDebugMode` | type changed |

### 5_mission

| Class | 1.29 field | Moved to |
|---|---|---|
| `HudDebug` | `Timer m_TimerUpdate` | **gone** |
| `HudDebugWinCharAgents` | `AutoHeightSpacer WgtModifiersContentPanelScript` | **gone** |
| `HudDebugWinCharAgents` | `m_WgtAgents`: type `Widget` → `GridSpacerWidget` | type changed |
| `HudDebugWinCharStats` | `m_WgtPanel`: type `Widget` → `GridSpacerWidget` | type changed |
| `HudDebugWinHealth` | `m_WgtPanel`: type `Widget` → `GridSpacerWidget` | type changed |
| `InviteMenu` | `ButtonWidget m_bCancel` | **gone** |
| `InviteMenu` | `ButtonWidget m_bCancelConsole` | **gone** |
| `ScriptConsole` | `Widget m_ButtonsWindowWidget` | **gone** |
| `ScriptConsole` | `int m_Id` | **gone** |
| `ScriptConsoleCameraTab` | `bool m_IsCAPSPressed` | **gone** |
| `ScriptConsoleItemsTab` | `int m_ObjectsScope` | **gone** |
| `SlotsIcon` | `ImageWidget m_AmmoIcon` | `SlotsIconBase` |
| `SlotsIcon` | `Widget m_ColWidget` | `SlotsIconBase` |
| `SlotsIcon` | `Widget m_ColorWidget` | `SlotsIconBase` |
| `SlotsIcon` | `Container m_Container` | `SlotsIconBase` |
| `SlotsIcon` | `float m_CurrQuantity` | `SlotsIconBase` |
| `SlotsIcon` | `Widget m_CursorWidget` | `SlotsIconBase` |
| `SlotsIcon` | `Widget m_EmptySelectedPanel` | `SlotsIconBase` |
| `SlotsIcon` | `ImageWidget m_GhostSlot` | `SlotsIconBase` |
| `SlotsIcon` | `bool m_HasItemSize` | `SlotsIconBase` |
| `SlotsIcon` | `bool m_HasQuantity` | `SlotsIconBase` |
| `SlotsIcon` | `bool m_HasTemperature` | `SlotsIconBase` |
| `SlotsIcon` | `bool m_IsDragged` | `SlotsIconBase` |
| `SlotsIcon` | `bool m_IsMagazine` | `SlotsIconBase` |
| `SlotsIcon` | `bool m_IsWeapon` | `SlotsIconBase` |
| `SlotsIcon` | `ItemBase m_Item` | `SlotsIconBase` |
| `SlotsIcon` | `ItemPreviewWidget m_ItemPreview` | `SlotsIconBase` |
| `SlotsIcon` | `Widget m_ItemSizePanel` | `SlotsIconBase` |
| `SlotsIcon` | `TextWidget m_ItemSizeWidget` | `SlotsIconBase` |
| `SlotsIcon` | `Widget m_MicromanagedPanel` | `SlotsIconBase` |
| `SlotsIcon` | `Widget m_MountedWidget` | `SlotsIconBase` |
| `SlotsIcon` | `int m_NormalHeight` | `SlotsIconBase` |
| `SlotsIcon` | `int m_NormalWidth` | `SlotsIconBase` |
| `SlotsIcon` | `EntityAI m_Obj` | `SlotsIconBase` |
| `SlotsIcon` | `Widget m_OutOfReachWidget` | `SlotsIconBase` |
| `SlotsIcon` | `Widget m_PanelWidget` | `SlotsIconBase` |
| `SlotsIcon` | `TextWidget m_QuantityItem` | `SlotsIconBase` |
| `SlotsIcon` | `Widget m_QuantityPanel` | `SlotsIconBase` |
| `SlotsIcon` | `ProgressBarWidget m_QuantityProgress` | `SlotsIconBase` |
| `SlotsIcon` | `Widget m_QuantityStack` | `SlotsIconBase` |
| `SlotsIcon` | `Widget m_RadialIcon` | `SlotsIconBase` |
| `SlotsIcon` | `Widget m_RadialIconClosed` | `SlotsIconBase` |
| `SlotsIcon` | `Widget m_RadialIconPanel` | `SlotsIconBase` |
| `SlotsIcon` | `bool m_Reserved` | `SlotsIconBase` |
| `SlotsIcon` | `Widget m_ReservedWidget` | `SlotsIconBase` |
| `SlotsIcon` | `Widget m_SelectedPanel` | `SlotsIconBase` |
| `SlotsIcon` | `string m_SlotDesc` | `SlotsIconBase` |
| `SlotsIcon` | `string m_SlotDisplayName` | `SlotsIconBase` |
| `SlotsIcon` | `int m_SlotID` | `SlotsIconBase` |
| `SlotsIcon` | `EntityAI m_SlotParent` | `SlotsIconBase` |

## 6. Newly `[Obsolete]` in 1.30 (compiles with `SCRIPT (W)`; some are no longer called)

Vanilla's own message is quoted. An obsolete method may still compile but **never be called by the engine or vanilla any more** (e.g. `OnCEUpdate`, `CreateFrontLight`) — read the message and grep vanilla for callers.

| Class | Method | 1.30 message | At |
|---|---|---|---|
| `ActionBuildPart` | `protected void SetBuildingAnimation(ItemBase)` | Use 'SetBuildingAnimationEx' instead | `4_world/classes/useractionscomponent/actions/continuous/actionbuildpart.c:370` |
| `ActionDrinkPondContinuous` | `protected int GetLiquidSource(ActionTarget)` | 1.30: No replacement. Method is no longer needed as liquid source is now checked and provided by the CCTLiquid target condition. | `4_world/classes/useractionscomponent/actions/continuous/actiondrinkpondcontinuous.c:136` |
| `ActionTargetsCursor` | `void SetContinuousInteractXboxIcon(string, string)` | no replacement | `5_mission/gui/actiontargetscursor.c:1370` |
| `ActionTargetsCursor` | `void SetContinuousXboxIcon(string, string)` | no replacement | `5_mission/gui/actiontargetscursor.c:1374` |
| `ActionTargetsCursor` | `void SetInteractXboxIcon(string, string)` | no replacement | `5_mission/gui/actiontargetscursor.c:1368` |
| `ActionTargetsCursor` | `void SetSingleXboxIcon(string, string)` | no replacement | `5_mission/gui/actiontargetscursor.c:1372` |
| `ActionTargetsCursor` | `protected void SetXboxIcon(string, string, string)` | no replacement | `5_mission/gui/actiontargetscursor.c:1376` |
| `AgentBase` | `bool GrowDuringAntibioticsAttack(PlayerBase)` | replaced by GrowDuringMedicalDrugsAttack | `4_world/classes/transmissionagents/agents/agentbase.c:153` |
| `BaseBuildingBase` | `protected void DebugCustomState()` | obsolete debug method | `4_world/entities/itembase/basebuildingbase.c:1285` |
| `BaseBuildingBase` | `protected void HandleItemFalling(ConstructionPart)` | handled in the Construction component directly | `4_world/entities/itembase/basebuildingbase.c:1275` |
| `BaseBuildingBase` | `protected void ItemFall(vector, vector)` | handled in the Construction component directly | `4_world/entities/itembase/basebuildingbase.c:1280` |
| `CarScript` | `protected void BrakeAndReverseRearLight()` | 1.30: use VehicleLightsComponent instead | `4_world/entities/vehicles/carscript.c:2841` |
| `CarScript` | `protected void BrakeLightsShineOff()` | 1.30: use VehicleLightsComponent instead | `4_world/entities/vehicles/carscript.c:2895` |
| `CarScript` | `protected void BrakeLightsShineOn()` | 1.30: VehicleLightsComponent instead | `4_world/entities/vehicles/carscript.c:2889` |
| `CarScript` | `protected void BrakesRearLight()` | 1.30: use VehicleLightsComponent instead | `4_world/entities/vehicles/carscript.c:2829` |
| `CarScript` | `protected void CarPartsHealthCheck()` | 1.30: use Transport::PartsHealthCheck instead | `4_world/entities/vehicles/carscript.c:2976` |
| `CarScript` | `protected void CheckVitalItem(bool, int)` | 1.30: use Transport::CheckVitalItemCallback instead | `4_world/entities/vehicles/carscript.c:2951` |
| `CarScript` | `protected CarLightBase CreateFrontLight()` | 1.30: use VehicleLightsComponent instead | `4_world/entities/vehicles/carscript.c:2944` |
| `CarScript` | `protected CarRearLightBase CreateRearLight()` | 1.30: use VehicleLightsComponent instead | `4_world/entities/vehicles/carscript.c:2937` |
| `CarScript` | `protected void DashboardShineOff()` | 1.30: use VehicleLightsComponent instead | `4_world/entities/vehicles/carscript.c:2930` |
| `CarScript` | `protected void DashboardShineOn()` | 1.30: use VehicleLightsComponent instead | `4_world/entities/vehicles/carscript.c:2924` |
| `CarScript` | `void ForceUpdateLightsEnd()` | 1.30: no replacement | `4_world/entities/vehicles/carscript.c:2808` |
| `CarScript` | `void ForceUpdateLightsStart()` | 1.30: no replacement | `4_world/entities/vehicles/carscript.c:2805` |
| `CarScript` | `bool IsScriptedLightsOn()` | 1.30: use Transport.LightIsOn | `4_world/entities/vehicles/carscript.c:2823` |
| `CarScript` | `bool IsVitalCarBattery()` | 1.30: use Transport.NeedElectricitySourceDevice | `4_world/entities/vehicles/carscript.c:2811` |
| `CarScript` | `bool IsVitalTruckBattery()` | 1.30: use Transport.NeedElectricitySourceDevice | `4_world/entities/vehicles/carscript.c:2817` |
| `CarScript` | `protected void LeftFrontLightShineOff()` | 1.30: use VehicleLightsComponent instead | `4_world/entities/vehicles/carscript.c:2865` |
| `CarScript` | `protected void LeftFrontLightShineOn()` | 1.30: use VehicleLightsComponent instead | `4_world/entities/vehicles/carscript.c:2853` |
| `CarScript` | `protected void NoRearLight()` | 1.30: use VehicleLightsComponent instead | `4_world/entities/vehicles/carscript.c:2847` |
| `CarScript` | `protected void ReverseLightsShineOff()` | 1.30: use VehicleLightsComponent instead | `4_world/entities/vehicles/carscript.c:2883` |
| `CarScript` | `protected void ReverseLightsShineOn()` | 1.30: use VehicleLightsComponent instead | `4_world/entities/vehicles/carscript.c:2877` |
| `CarScript` | `protected void ReverseRearLight()` | 1.30: use VehicleLightsComponent instead | `4_world/entities/vehicles/carscript.c:2835` |
| `CarScript` | `protected void RightFrontLightShineOff()` | 1.30: use VehicleLightsComponent instead | `4_world/entities/vehicles/carscript.c:2871` |
| `CarScript` | `protected void RightFrontLightShineOn()` | 1.30: use VehicleLightsComponent instead | `4_world/entities/vehicles/carscript.c:2859` |
| `CarScript` | `protected void TailLightsShineOff()` | 1.30: use VehicleLightsComponent instead | `4_world/entities/vehicles/carscript.c:2918` |
| `CarScript` | `protected void TailLightsShineOn()` | 1.30: use VehicleLightsComponent instead | `4_world/entities/vehicles/carscript.c:2901` |
| `CombinationLock` | `bool IsLockedOnGate()` | 1.30: Use IsLocked instead | `4_world/entities/itembase/combinationlock.c:846` |
| `CombinationLock` | `protected void SoundLockChangeDial()` | 1.30: Use ItemSoundHandler instead | `4_world/entities/itembase/combinationlock.c:840` |
| `CombinationLock` | `protected void SoundLockChangeNumber()` | 1.30: Use ItemSoundHandler instead | `4_world/entities/itembase/combinationlock.c:834` |
| `CombinationLock` | `protected void SoundLockClose()` | 1.30: Use ItemSoundHandler instead | `4_world/entities/itembase/combinationlock.c:828` |
| `CombinationLock` | `protected void SoundLockOpen()` | 1.30: Use ItemSoundHandler instead | `4_world/entities/itembase/combinationlock.c:822` |
| `ConstructionActionData` | `int GetSlotId()` | 1.30: Unsafe, overridden ActionData used in-system instead | `4_world/classes/basebuilding/constructionactiondata.c:623` |
| `ConstructionActionData` | `void SetSlotId(int)` | 1.30: Unsafe, overridden ActionData used in-system instead | `4_world/classes/basebuilding/constructionactiondata.c:617` |
| `ConstructionPart` | `void SetRequestBuiltState(bool)` | 1.30: Use 'SetBuiltState' directly | `4_world/classes/basebuilding/constructionpart.c:510` |
| `DayZPlayerImplement` | `string GetSurfaceType(SurfaceAnimationBone)` | 1.30: uses CGame GetSurfaceInfo* methods instead | `4_world/entities/dayzplayerimplement.c:4121` |
| `DayZPlayerImplementMeleeCombat` | `protected void HitZoneSelection()` | replaced by HitZoneSelectionRaycast | `4_world/entities/dayzplayerimplementmeleecombat.c:603` |
| `DayZPlayerImplementMeleeCombat` | `protected bool IsObstructed(Object)` | replaced by MeleeTargeting.IsMeleeTargetObstructed | `4_world/entities/dayzplayerimplementmeleecombat.c:670` |
| `EffWheelSmoke` | `void SetSurface(string)` | 1.30: use SelectFromSurface instead | `4_world/entities/effects/wheelsmoke.c:32` |
| `EmoteManager` | `void CheckEmoteLockedState()` | 1.30: no replacement | `4_world/classes/emotemanager.c:1211` |
| `EmoteManager` | `void ForceSurrenderState(bool)` | 1.30: Use PlayerBase::SetSurrenderState instead | `4_world/classes/emotemanager.c:1255` |
| `EntityAI` | `bool CanDisplayAttachmentSlot(string)` | 1.30: Use CanDisplayAttachmentSlot(int slot_id)instead | `3_game/entities/entityai.c:4918` |
| `EntityAI` | `bool LocalTakeEntityAsAttachment(notnull EntityAI)` | 1.30: Use LocalTakeEntityToTargetAttachment instead | `3_game/entities/entityai.c:4901` |
| `EntityAI` | `bool LocalTakeEntityAsAttachmentEx(notnull EntityAI, int)` | 1.30: Use LocalTakeEntityToTargetAttachmentEx instead | `3_game/entities/entityai.c:4880` |
| `EntityAI` | `bool LocalTakeEntityToCargo(notnull EntityAI)` | 1.30: Use LocalTakeEntityToTargetCargo instead | `3_game/entities/entityai.c:4843` |
| `EntityAI` | `bool LocalTakeEntityToCargoEx(notnull EntityAI, int, int, int)` | 1.30: Use LocalTakeEntityToTargetCargoEx instead | `3_game/entities/entityai.c:4864` |
| `EntityAI` | `void OnCEUpdate()` | 1.30: Use OnCEIterate(float currentTime, float elapsedTime)instead | `3_game/entities/entityai.c:4924` |
| `EntityAI` | `bool PredictiveTakeEntityAsAttachment(notnull EntityAI)` | 1.30: Use PredictiveTakeEntityToTargetAttachment instead | `3_game/entities/entityai.c:4893` |
| `EntityAI` | `bool PredictiveTakeEntityAsAttachmentEx(notnull EntityAI, int)` | 1.30: Use PredictiveTakeEntityToTargetAttachmentEx instead | `3_game/entities/entityai.c:4872` |
| `EntityAI` | `bool PredictiveTakeEntityToCargo(notnull EntityAI)` | 1.30: Use PredictiveTakeEntityToTargetCargo instead | `3_game/entities/entityai.c:4835` |
| `EntityAI` | `bool PredictiveTakeEntityToCargoEx(notnull EntityAI, int, int, int)` | 1.29: Use PredictiveTakeEntityToTargetCargoEx instead | `3_game/entities/entityai.c:4856` |
| `EntityAI` | `bool ServerTakeEntityAsAttachment(notnull EntityAI)` | 1.30: Use ServerTakeEntityToTargetAttachment instead | `3_game/entities/entityai.c:4906` |
| `EntityAI` | `bool ServerTakeEntityAsAttachmentEx(notnull EntityAI, int)` | 1.30: Use ServerTakeEntityToTargetAttachmentEx instead | `3_game/entities/entityai.c:4885` |
| `EntityAI` | `bool ServerTakeEntityToCargo(notnull EntityAI)` | 1.30: Use ServerTakeEntityToTargetCargo instead | `3_game/entities/entityai.c:4848` |
| `ExplosivesBase` | `void AddExplosionEffectForSurface(ExplosionEffectsData)` | 1.30: Handled in AmmoTypes instead | `4_world/entities/explosivesbase.c:386` |
| `ExplosivesBase` | `int GetParticleExplosionID(string)` | 1.30: Handled in AmmoTypes instead | `4_world/entities/explosivesbase.c:394` |
| `ExplosivesBase` | `void InitSpecificsExplosionEffectForSurface()` | 1.30: Handled in AmmoTypes instead | `4_world/entities/explosivesbase.c:380` |
| `ExplosivesBase` | `void SetParticleExplosion(int)` | 1.30: Handled in AmmoTypes instead | `4_world/entities/explosivesbase.c:306` |
| `GameInventory` | `bool TakeEntityAsAttachment(InventoryMode, notnull EntityAI)` | 1.30: Use TakeEntityAsTargetAttachment instead | `3_game/systems/inventory/inventory.c:1382` |
| `GameInventory` | `bool TakeEntityAsAttachmentEx(InventoryMode, notnull EntityAI, int)` | 1.30: Use TakeEntityAsTargetAttachmentEx instead | `3_game/systems/inventory/inventory.c:1374` |
| `GameInventory` | `bool TakeEntityToCargo(InventoryMode, notnull EntityAI)` | 1.30: Use TakeEntityToTargetCargo instead | `3_game/systems/inventory/inventory.c:1345` |
| `GameInventory` | `bool TakeEntityToCargoEx(InventoryMode, notnull EntityAI, int, int, int)` | 1.30: Use TakeEntityToTargetCargoEx instead | `3_game/systems/inventory/inventory.c:1357` |
| `HudDebugWinCharStats` | `void FitWindow()` | no replacement | `5_mission/gui/scriptshuddebug/huddebugwincharstats.c:306` |
| `Icon` | `int GetRelevantInventoryAction(int)` | no replacement | `5_mission/gui/inventorynew/containeditems/icon.c:1773` |
| `ItemBase` | `protected void ClearStartItemSoundServer()` | 1.30: No replacement | `4_world/entities/itembase.c:4948` |
| `ItemBase` | `float GetDryingIncrement(string)` | 1.30: replaced by GetDryingIncrement taking enum/int arguments | `4_world/entities/itembase.c:3672` |
| `ItemBase` | `float GetHeatIsolationInit()` | 1.30: No replacement | `4_world/entities/itembase.c:4942` |
| `ItemBase` | `float GetSoakingIncrement(string)` | 1.30: replaced by GetSoakingIncrement taking enum/int arguments | `4_world/entities/itembase.c:3682` |
| `Man` | `bool JunctureTakeEntityAsAttachment(notnull EntityAI)` | 1.30: Use JunctureTakeEntityToTargetAttachment instead | `3_game/entities/man.c:901` |
| `Man` | `bool JunctureTakeEntityAsAttachmentEx(notnull EntityAI, int)` | 1.30: Use JunctureTakeEntityToTargetAttachmentEx instead | `3_game/entities/man.c:918` |
| `Man` | `bool JunctureTakeEntityToCargo(notnull EntityAI)` | 1.30: Use JunctureTakeEntityToTargetCargo instead | `3_game/entities/man.c:884` |
| `Man` | `protected bool TakeEntityAsAttachmentExImpl(InventoryMode, notnull EntityAI, int)` | 1.30: Use TakeEntityToTargetAttachmentExImpl instead | `3_game/entities/man.c:924` |
| `Man` | `protected bool TakeEntityAsAttachmentImpl(InventoryMode, notnull EntityAI)` | 1.30: Use TakeEntityToTargetAttachmentImpl instead | `3_game/entities/man.c:907` |
| `Man` | `protected bool TakeEntityToCargoImpl(InventoryMode, notnull EntityAI)` | 1.30: Use TakeEntityToTargetCargoImpl instead | `3_game/entities/man.c:890` |
| `MiscGameplayFunctions` | `static bool BuildCondition(PlayerBase, ActionTarget, ItemBase, bool)` | Obsolete collision checking method, use the other 'BuildCondition' | `4_world/static/miscgameplayfunctions.c:2004` |
| `MiscGameplayFunctions` | `static bool ComplexBuildCollideCheckClient(PlayerBase, ActionTarget, ItemBase, string="")` | Obsolete collision checking method, use the other 'ComplexBuildCollideCheckClient' | `4_world/static/miscgameplayfunctions.c:1998` |
| `MiscGameplayFunctions` | `static void GenerateAINoiseAtPosition(vector, float, NoiseParams)` | Use MiscGameplayFunctions.GenerateAINoiseAtPositionEx instead! | `4_world/static/miscgameplayfunctions.c:2010` |
| `ModInfo` | `static string GetDLCImage(string)` | 1.30: no replacement | `3_game/modinfo.c:25` |
| `NoiseAIEvaluate` | `static float GetNoiseReduction(Weather)` | Use GetNoiseReductionEx instead! | `4_world/static/sensesaievaluate.c:92` |
| `OptionsMenu` | `void ToggleDependentOptions(int, bool)` | 1.29: No replacement | `5_mission/gui/newui/options/optionsmenu.c:917` |
| `OptionsMenuControls` | `void InitDependentOptionsVisibility()` | 1.29: No replacement | `5_mission/gui/newui/options/optionsmenucontrols.c:935` |
| `OptionsMenuControls` | `void ToggleDependentOptions(int, bool)` | 1.29: No replacement | `5_mission/gui/newui/options/optionsmenucontrols.c:932` |
| `OptionsMenuGame` | `void ToggleDependentOptions(int, bool)` | 1.29: No replacement | `5_mission/gui/newui/options/optionsmenugame.c:714` |
| `OptionsMenuSounds` | `void InitDependentOptionsVisibility()` | 1.29: No replacement | `5_mission/gui/newui/options/optionsmenusounds.c:422` |
| `OptionsMenuSounds` | `void ToggleDependentOptions(int, bool)` | 1.29: No replacement | `5_mission/gui/newui/options/optionsmenusounds.c:419` |
| `OptionsMenuVideo` | `void InitDependentOptionsVisibility()` | 1.29: No replacement | `5_mission/gui/newui/options/optionsmenuvideo.c:677` |
| `OptionsMenuVideo` | `void ToggleDependentOptions(int, bool)` | 1.29: No replacement | `5_mission/gui/newui/options/optionsmenuvideo.c:674` |
| `PluginTransmissionAgents` | `bool GrowDuringAntibioticsAttack(int, PlayerBase)` | replaced by GrowDuringMedicalDrugsAttack | `4_world/plugins/pluginbase/plugintransmissionagents.c:685` |
| `StaticConstructionMethods` | `static void SpawnConstructionMaterialPiles(notnull EntityAI, Man, string, string, string="", bool=false)` | 1.30: Use SpawnConstructionMaterialPilesEx instead | `4_world/classes/constructionbase.c:1957` |
| `Surface` | `static int GetStepsParticleID(string)` | 1.30: use SurfaceInfo.GetStepParticleId directly | `4_world/static/surface.c:64` |
| `Surface` | `static int GetWheelParticleID(string)` | 1.30: use SurfaceInfo.GetWheelParticleId directly | `4_world/static/surface.c:70` |
| `UndergroundStash` | `void PlaceOnGround()` | 1.30: No replacement | `4_world/entities/undergroundstash.c:51` |
| `VicinityItemManager` | `protected float GetFixedHeadHeightAdjustment(PlayerBase)` | use MiscGameplayFunctions.GetPlayerHeadHeight instead | `5_mission/gui/inventorynew/vicinityitemmanager.c:480` |
| `Weather` | `float GetNoiseReductionByWeather()` | Use Weather.GetNoiseReductionByWeatherEx instead! | `3_game/weather.c:454` |
| `World` | `proto native bool Is3rdPersonDisabled()` | 1.30: replaced by World::GetThirdPersonViewMode returning current thrid person mode | `3_game/global/world.c:291` |
| `World` | `proto native void UpdatePathgraphDoorByAnimationSourceName(notnull Object, string)` | 1.30: Use UpdatePathgraphEntityState | `3_game/global/world.c:122` |
| `CombinationLock` | `void AnimateDigits(string)` (new in 1.30 and already obsolete) | 1.30: Use AnimateDigitsOutside & AnimateDigitsInside instead | `4_world/entities/itembase/combinationlock.c:865` |
| `CombinationLock` | `void SetDigitRotationAnimationPhase(string, float)` (new in 1.30 and already obsolete) | 1.30: Use SetDigitRotationAnimationPhaseOutside & SetDigitRotationAnimationPhaseInside instead | `4_world/entities/itembase/combinationlock.c:863` |
| `CombinationLock` | `void Unlock()` (new in 1.30 and already obsolete) | 1.30: Use UnlockOnServer instead | `4_world/entities/itembase/combinationlock.c:861` |
| `ExplosivesBase` | `string GetExplosionSoundSetName(string)` (new in 1.30 and already obsolete) | 1.30: Handled in AmmoTypes instead | `4_world/entities/explosivesbase.c:419` |
| `NoiseAIEvaluate` | `static float GetNoiseReductionEx(notnull Object)` (new in 1.30 and already obsolete) | 1.30: AI noise dampening is now done natively via AIParams config parameters. Kept for HUD and old mods. | `4_world/static/sensesaievaluate.c:18` |
| `Weather` | `float GetNoiseReductionByWeatherEx(notnull Object)` (new in 1.30 and already obsolete) | 1.30: AI noise dampening is now done natively via AIParams config parameters. Kept for HUD and old mods. | `3_game/weather.c:407` |
| `ConstructionPart` | field `m_Id` | | |
| `ConstructionPart` | field `m_IsBase` | | |
| `ConstructionPart` | field `m_IsGate` | | |
| `ConstructionPart` | field `m_MainPartName` | | |
| `ConstructionPart` | field `m_Name` | | |
| `ConstructionPart` | field `m_PartName` | | |

### 6b. Classes newly `[Obsolete]` in 1.30 (class-level attribute)

Extending or instantiating them compiles with a warning; most are no longer used by vanilla.

| Class | 1.30 message | At |
|---|---|---|
| `ActionBuildShelter` | Obsolete("1.30: driven by ActionBuildPart") | `4_world/classes/useractionscomponent/actions/interact/actionbuildshelter.c:1` |
| `ActionCarHornBase` | Obsolete("replaced by ActionVehicleHornBase") | `4_world/classes/useractionscomponent/actions/continuous/vehicles/actioncarhorn.c:1` |
| `ActionCarHornLong` | Obsolete("replaced by ActionVehicleHornLong") | `4_world/classes/useractionscomponent/actions/continuous/vehicles/actioncarhorn.c:213` |
| `ActionCarHornShort` | Obsolete("replaced by ActionVehicleHornShort") | `4_world/classes/useractionscomponent/actions/continuous/vehicles/actioncarhorn.c:69` |
| `ActionCarHornShortCB` | Obsolete("replaced by ActionVehicleHornShortCB") | `4_world/classes/useractionscomponent/actions/continuous/vehicles/actioncarhorn.c:66` |
| `ActionFillBottleBase` | Obsolete("1.30: replaced by class ActionObtainLiquidBase") | `4_world/classes/useractionscomponent/actions/continuous/actionfillbottlebase.c:28` |
| `ActionFillBottleBaseCB` | Obsolete("1.30: replaced by ActionObtainLiquidBaseCB") | `4_world/classes/useractionscomponent/actions/continuous/actionfillbottlebase.c:2` |
| `ActionWashHandsItem` | Obsolete("1.30: Not used and not registered in action constructor!") | `4_world/classes/useractionscomponent/actions/singleuse/actionwashhandsitem.c:1` |
| `ActionWashHandsWaterOne` | Obsolete("1.30: Not used and not registered in action constructor!") | `4_world/classes/useractionscomponent/actions/interact/actionwashhandswaterone.c:11` |
| `ActionWashHandsWaterOneCB` | Obsolete("1.30: Not used!") | `4_world/classes/useractionscomponent/actions/interact/actionwashhandswaterone.c:2` |
| `ActionWashHandsWellOne` | Obsolete("1.30: Not used and not registered in action constructor!") | `4_world/classes/useractionscomponent/actions/interact/actionwashhandswellone.c:11` |
| `ActionWashHandsWellOneCB` | Obsolete("1.30: Not used!") | `4_world/classes/useractionscomponent/actions/interact/actionwashhandswellone.c:2` |
| `CCTWaterSurface` | Obsolete("1.30: replaced by CCTLiquid") | `4_world/classes/useractionscomponent/targetconditionscomponents/cctwatersurface.c:2` |
| `CCTWaterSurfaceEx` | Obsolete("1.30: replaced by CCTLiquid") | `4_world/classes/useractionscomponent/targetconditionscomponents/cctwatersurface.c:59` |
| `CivilianSedanFrontLight` | Obsolete("use VehicleLightBase and VehicleLightProfileBase instead") | `4_world/entities/scriptedlightbase/spotlightbase/carlightbase/civiliansedanfrontlight.c:1` |
| `Hatchback_02FrontLight` | Obsolete("use VehicleLightBase and VehicleLightProfileBase instead") | `4_world/entities/scriptedlightbase/spotlightbase/carlightbase/hatchback_02frontlight.c:1` |
| `OffroadHatchbackFrontLight` | Obsolete("use VehicleLightBase and VehicleLightProfileBase instead") | `4_world/entities/scriptedlightbase/spotlightbase/carlightbase/offroadhatchbackfrontlight.c:1` |
| `Offroad_02FrontLight` | Obsolete("use VehicleLightBase and VehicleLightProfileBase instead") | `4_world/entities/scriptedlightbase/spotlightbase/carlightbase/offroad_02frontlight.c:1` |
| `Sedan_02FrontLight` | Obsolete("use VehicleLightBase and VehicleLightProfileBase instead") | `4_world/entities/scriptedlightbase/spotlightbase/carlightbase/sedan_02frontlight.c:1` |
| `SurrenderDummyItem` | Obsolete("1.30: no replacement - no longer in use") | `4_world/entities/itembase/surrenderdummyitem.c:1` |
| `Truck_01FrontLight` | Obsolete("use VehicleLightBase and VehicleLightProfileBase instead") | `4_world/entities/scriptedlightbase/spotlightbase/carlightbase/truck_01frontlight.c:1` |

## 7. Visibility changes

| Class | Method | 1.29 | 1.30 |
|---|---|---|---|
| `BoatScript` | `CleanupEffects` | protected | public |
| `CarScript` | `PlayCrashHeavySound` | public | protected |
| `CarScript` | `PlayCrashLightSound` | public | protected |
| `CombinationLock` | `DestroyLock` | public | protected |
| `CombinationLock` | `ShuffleLock` | public | protected |
| `CombinationLock` | `SoundLockChangeDial` | public | protected |
| `CombinationLock` | `SoundLockChangeNumber` | public | protected |
| `CombinationLock` | `UpdateSound` | public | protected |
| `CombinationLock` | `UpdateVisuals` | public | protected |
| `EmoteManager` | `ClearSurrenderState` | protected | public |
| `Environment` | `ApplyDrynessToItemEx` | protected | private, protected |
| `Environment` | `ApplyWetnessToItem` | protected | private, protected |
| `InventoryItemType` | `InventoryItemType` | public | private |
| `PluginSceneManager` | `DeselectObject` | private | protected |
| `PluginSceneManager` | `EditorUpdateRuler` | private | protected |
| `PluginSceneManager` | `GetMousePositionInScene` | private | protected |
| `PluginSceneManager` | `IsMouseInScene` | private | protected |
| `PluginSceneManager` | `SceneEditorMenuCmdPopup` | private | protected |
| `PluginSceneManager` | `SceneEditorMenuCmdRefresh` | private | protected |
| `PluginSceneManager` | `SceneEditorMenuCmdSave` | private | protected |

## 8. Changed class constants

| Class | Constant | 1.29 | 1.30 |
|---|---|---|---|
| `ActionFillGeneratorTankCB` | `QUANTITY_FILLED_PER_SEC` | `400` | `50` |
| `ActionTargets` | `c_UtilityMaxDistFromRaySqr` | `0.8 * 0.8` | `0.65 * 0.65` |
| `DayZPlayerImplementFallDamage` | `BLEEDINGHANDS_HEIGHT_LOW` | `5` | `3` |
| `DayZPlayerImplementFallDamage` | `BROKENLEGS_HEIGHT_HIGH` | `9` | `8` |
| `DayZPlayerImplementFallDamage` | `BROKENLEGS_HEIGHT_LOW` | `5` | `3` |
| `DayZPlayerImplementFallDamage` | `HEALTH_HEIGHT_HIGH` | `14` | `12` |
| `DayZPlayerImplementFallDamage` | `HEALTH_HEIGHT_LOW` | `5` | `2` |
| `DayZPlayerImplementFallDamage` | `SHOCK_HEIGHT_HIGH` | `12` | `8` |
| `DayZPlayerImplementFallDamage` | `SHOCK_HEIGHT_LOW` | `3` | `0` |
| `GameConstants` | `CARS_FLUIDS_TICK` | `1` | `VEHICLE_FLUIDS_TICK` |
| `GameConstants` | `ITEM_TEMPERATURE_NEUTRAL_ZONE_MIDDLE` | `(GameConstants.ITEM_TEMPERATURE_NEUTRAL_ZONE_UPPER_LIMIT + GameConstants.ITEM_TEMPERATURE_NEUTRAL_ZONE_LOWER_LIMIT)* 0.5` | `(ITEM_TEMPERATURE_NEUTRAL_ZONE_UPPER_LIMIT + ITEM_TEMPERATURE_NEUTRAL_ZONE_LOWER_LIMIT)* 0.5` |
| `GameConstants` | `LIQUID_RAIN_AMOUNT_COEF_BASE` | `10.0` | `30.0` |
| `PlayerConstants` | `SL_WATER_NORMAL` | `800` | `1000` |

## 9. Enums

Removed enums: `eDebugMode`

Values that **shift** break any code that stores or sends the raw integer (always use the enum name). An enum declared `enum X : ETransportFluid {}` inherits its values (the 'removed' list then only means 'moved to the base enum').

| Enum | File | Removed / renamed values | Added values | Values whose number changed |
|---|---|---|---|---|
| `BoatFluid` | `3_game/vehicles/boat.c` | `FUEL` (now `: ETransportFluid`) |  | 0 |
| `CarFluid` | `3_game/vehicles/car.c` | `FUEL`, `OIL`, `BRAKE`, `COOLANT`, `USER1`, `USER2`, `USER3`, `USER4` (now `: ETransportFluid`) |  | 0 |
| `ConstructionMaterialType` | `4_world/classes/constructionbase.c` |  | `MATERIAL_BRICK`, `MATERIAL_RUBBLE` | 0 |
| `DayZInfectedConstants` | `3_game/entities/dayzinfected.c` |  | `MINDSTATE_COWER` | 0 |
| `DayZPlayerConstants` | `3_game/dayzplayer.c` |  | `CMD_ACTIONMOD_COMBINATIONLOCK`, `CMD_ACTIONMOD_WASHFACE`, `CMD_ACTIONFB_COMBINATIONLOCK`, `CMD_ACTIONFB_WASHFACEPOND`, `CMD_ACTIONFB_BUILDROPELADDER`, `CMD_ACTIONFB_BRICKSTACK`, `CMD_ACTIONFB_BRICKTROWEL`, `CMD_ACTIONFB_MIX_MORTAR`, `CMD_ACTIONFB_DRINKWELL_BUCKET`, `CMD_ACTIONFB_SHARPEN`, `CMD_ACTIONFB_WASHFACEWELL`, `CMD_ACTIONFB_WETCLOTH`, `CMD_ACTIONFB_WETCLOTHWELL` | 0 |
| `DiagMenuIDs` | `3_game/enums/ediagmenuids.c` | `MISC_VEHICLE_GETOUT_BOX`, `MISC_VEHICLE_GETOUT_RAYCAST`, `UNDERGROUND_DEBUG` | `CHEATS_OLD_PROCESSINPUTDATA`, `TRANSPORT_MENU`, `TRANSPORT_FREESPACE_DISABLED`, `TRANSPORT_FREESPACE_VIS`, `MISC_PLAYER_SURFACE_DEBUG`, `MISC_DUST_EFFECTS_GROUND_LIFETIME_MIN`, `MISC_DUST_EFFECTS_GROUND_LIFETIME_MAX`, `MISC_DUST_EFFECTS_GROUND_SIZE_MIN`, `MISC_DUST_EFFECTS_GROUND_SIZE_MAX`, `BASEBUILDING_FASTBUILD`, `BASEBUILDING_COLLISIONDEBUG`, `BASEBUILDING_FULLDEBUG`, `RADIOGAMEPLAY_MENU`, `RADIOGAMEPLAY_LOGGING`, `RADIOGAMEPLAY_DEBUG`, `ROBOCLIENTS_MENU`, `ROBOCLIENTS_ACTIVE`, `ROBOCLIENTS_RESTART`, `ROBOCLIENTS_STATE_MACHINES`, `FISHING_MENU`, `SYNTHETIC_FISHING_CURSOR` | 186 |
| `EActions` | `3_game/enums/eactions.c` |  | `VEHICLE_HORN_START_LONG`, `VEHICLE_HORN_START_SHORT`, `VEHICLE_HORN_STOP`, `VEHICLE_FUEL_FULL`, `VEHICLE_FUEL_EMPTY`, `VEHICLE_FUEL_INCREASE`, `VEHICLE_FUEL_DECREASE`, `PLAYER_BOT_RANDOMIZE_MOVEMENT` | 30 |
| `EBuildingLockType` | `3_game/enums/ebuildinglocktypes.c` |  | `SHIP_CONTAINER_4` | 0 |
| `ECachedEquipmentItemCategory` | `3_game/enums/ecachedequipmentitemcategory.c` |  | `OPTICS`, `EYEWEAR`, `MASK` | 1 |
| `ECarDebugMode` | `4_world/entities/vehicles/carscript.c` |  | `RANDOM_DRIVING`, `PREDEFINED_PATH` | 0 |
| `EDLCId` | `3_game/services/contentdlc.c` | `DLC_UNKNOWN`, `DLC_FROSTLINE` | `UNKNOWN`, `FROSTLINE`, `BADLANDS` | 0 |
| `EEffectAreaType` | `4_world/classes/contaminatedarea/effectarea.c` |  | `OIL_PIT` | 0 |
| `EEnvironmentHeatcomfortBehaviorCategory` | `4_world/classes/environment/environment.c` |  | `MOTORBIKE`, `BOAT` | 0 |
| `EInputRestrictors` | `3_game/constants.c` |  | `SURRENDER_TRANSITION` | 0 |
| `EPlayerSoundEventID` | `4_world/classes/soundevents/playersoundevents/playersoundeventhandler.c` |  | `HEAVY_BREATHING` | 1 |
| `ERPCs` | `3_game/enums/erpcs.c` |  | `RPC_SYMPTOM_PARAM_SYNC`, `DEV_SET_SANDSTORM`, `DEV_STOP_SANDSTORM`, `DEV_SET_SANDSTORM_WEATHER`, `DEV_STOP_SANDSTORM_WEATHER`, `RPC_ITEM_SOUND_PLAY`, `RPC_ITEM_SOUND_STOP`, `RPC_CODE_LOCK_REQUEST`, `RPC_CODE_LOCK_RESPONSE`, `RPC_CODE_LOCK_UI_ACTION`, `RPC_BUNKERBROADCAST_CONFIG_SYNC`, `DIAG_MISC_RADIOGAMEPLAY_DEBUG_DATA`, `DIAG_MISC_RADIOGAMEPLAY_BUNKERS_LIST`, `DIAG_BASEBUILDING_FASTBUILD`, `DIAG_RADIO_GAMEPLAY_LOGGING` | 160 |
| `EffectWidgetHandles` | `3_game/constants.c` |  | `SQUINTING`, `EYE_VEINS` | 0 |
| `EffectWidgetsTypes` | `3_game/constants.c` |  | `EYE_VEINS`, `SQUINTING` | 0 |
| `LockAction` | `4_world/entities/itembase/combinationlock.c` | `DIAL_NUMBER_CHANED`, `DIAL_INDEX_CHANGED` | `DIAL_NUMBER_CHANED_INSIDE`, `DIAL_NUMBER_CHANED_OUTSIDE`, `DIAL_INDEX_CHANGED_INSIDE`, `DIAL_INDEX_CHANGED_OUTSIDE` | 3 |
| `NVTypes` | `4_world/entities/manbase/dayzplayer/dayzplayercamera_base.c` |  | `NV_OPTICS_UNDERGROUND`, `NV_OPTICS_SANDSTORM_DAY`, `NV_OPTICS_SANDSTORM_NIGHT`, `NV_OPTICS_UNDERGROUND_SANDSTORM`, `NV_GOGGLES_DAY`, `NV_GOGGLES_UNDERGROUND`, `NV_GOGGLES_SANDSTORM`, `NV_GOGGLES_UNDERGROUND_SANDSTORM`, `NV_PUMPKIN_DAY`, `NV_PUMPKIN_UNDERGROUND`, `NV_PUMPKIN_SANDSTORM`, `NV_PUMPKIN_UNDERGROUND_SANDSTORM` | 1 |
| `ParticlePropertiesFlags` | `3_game/particles/particlemanager/particlesource.c` |  | `DISABLED_IN_UI` | 0 |
| `SymptomIDs` | `4_world/classes/playersymptoms/statemanager.c` |  | `SYMPTOM_SQUINT`, `SYMPTOM_IRRITATED_EYES`, `SYMPTOM_FAINT` | 1 |
| `TabType` | `5_mission/gui/newui/serverbrowsermenu/serverbrowsertab.c` |  |  | 4 |
| `VehicleAnimInstances` | `4_world/entities/vehicles/vehicleaniminstances.c` |  | `MOTO1`, `MOTO2` | 0 |
| `eAgents` | `3_game/enums/eagents.c` |  | `SILICOSIS`, `EYES_IRRITATION` | 0 |
| `eModifierSyncIDs` | `4_world/classes/playermodifiers/modifiersmanager.c` |  | `MODIFIER_SYNC_PARTICLE_EYES`, `MODIFIER_SYNC_HEAT_STROKE` | 1 |
| `eModifiers` | `4_world/classes/playermodifiers/emodifiers.c` |  | `MDF_SANDSTORM_EXPOSURE_STATIC`, `MDF_SANDSTORM_EXPOSURE_DYNAMIC`, `MDF_PARTICLES_BREATH`, `MDF_PARTICLES_EYES`, `MDF_HEAT_STROKE1`, `MDF_HEAT_STROKE2`, `MDF_HEAT_STROKE3`, `MDF_HEAT_STROKE4` | 1 |

New enums: `CodeLockLightState`, `CodeLockStage`, `CodeLockUIAction`, `EConstructionDebugModes`, `EConstructionDirCheckType`, `EConstructionInfoCategories`, `EConstructionTools`, `EDLCChange`, `EDiagMenu`, `EDiagMenuGame`, `EEnvironmentDryingCategories`, `EEnvironmentSoakingCategories`, `ELiquidSourceObjectType`, `EMotorbikeDebugMode`, `EMotorbikeOperationalState`, `EOilPitState`, `EPlayerStats_v130`, `ETransportFluid`, `EVehicleVFXEffect`, `EVehicleVFXEffectGroup`, `MotorbikeEngineSoundState`, `MotorbikeFluid`, `MotorbikeHeadlightState`, `MotorbikeRearLightType`, `MotorbikeSoundCtrl`, `OverrideInputType`, `ScriptConsoleTabID`, `ThirdPersonMode`, `eConstructionIconItemState`, `eWeaponDebugMode`


## 10. Global functions, constants and typedefs

| Name | Change | 1.29 | 1.30 |
|---|---|---|---|
| `AGT_AIRBOURNE_SOLID` | added |  | `int = 15` |
| `COLOR_CYAN` | added |  | `int = 0xFF00FFFF` |
| `COLOR_MAGENTA` | added |  | `int = 0xFFFF00FF` |
| `DEF_DUST_PARTICLE_BREATH` | added |  | `int = 3` |
| `DEF_DUST_PARTICLE_EYES` | added |  | `int = 4` |
| `DIGIT_ROTATION_POSITION_0` | added |  | `float = 0.9` |
| `DIGIT_ROTATION_POSITION_1` | added |  | `float = 0.0` |
| `DIGIT_ROTATION_POSITION_2` | added |  | `float = 0.1` |
| `DIGIT_ROTATION_POSITION_3` | added |  | `float = 0.2` |
| `DIGIT_ROTATION_POSITION_4` | added |  | `float = 0.3` |
| `DIGIT_ROTATION_POSITION_5` | added |  | `float = 0.4` |
| `DIGIT_ROTATION_POSITION_6` | added |  | `float = 0.5` |
| `DIGIT_ROTATION_POSITION_7` | added |  | `float = 0.6` |
| `DIGIT_ROTATION_POSITION_8` | added |  | `float = 0.7` |
| `DIGIT_ROTATION_POSITION_9` | added |  | `float = 0.8` |
| `ECE_OBJECT_SPAWNER` | added |  | `int = 256` |
| `GAME_STORAGE_VERSION` | changed | `int = 142` | `int = 144` |
| `HSVToRGB` | added |  | `static vector HSVToRGB(vector)` |
| `INPUT_UDT_UNDERGROUND_SYNC` | added |  | `int = 17` |
| `MENU_DIGITAL_CODELOCK` | added |  | `int = 47` |
| `MENU_UNDERGROUND_TRIGGER_EDITOR` | added |  | `int = 48` |
| `ProjectViewportToWorld` | added |  | `proto vector ProjectViewportToWorld(float, float, float, float, out vector, int)` |
| `ProjectWorldToViewport` | added |  | `proto vector ProjectWorldToViewport(vector, float, float, int)` |
| `RGBToHSV` | added |  | `static vector RGBToHSV(vector)` |
| `UA_AM_ACCEPTED` | changed | `int = 15` | `int = 16` |
| `UA_AM_INIT` | added |  | `int = 14` |
| `UA_AM_PENDING` | changed | `int = 14` | `int = 15` |
| `UA_AM_REJECTED` | changed | `int = 16` | `int = 17` |
| `UA_IN_CRAFTING` | changed | `int = 21` | `int = 22` |
| `UA_IN_END` | changed | `int = 18` | `int = 19` |
| `UA_IN_START` | changed | `int = 17` | `int = 18` |
| `UA_SPAWN_DUST_A` | changed | `int = 19` | `int = 20` |
| `UA_SPAWN_DUST_B` | changed | `int = 20` | `int = 21` |
| `_motorbike` | added |  | `MotorbikeScript = None` |

## 11. New classes in 1.30 (index)

Grouped by folder. Open the file for the API; this index only tells you the class exists and what it extends.

- **3_game/** — `BunkerBroadcastConfigInstance` : BunkerBroadcastJsonDataBase, `BunkerBroadcastJsonDataBase`, `BunkerBroadcastManager`, `BunkerBroadcastPeriod` : BunkerBroadcastJsonDataBase, `BunkerBroadcastPlaybackSettings` : BunkerBroadcastJsonDataBase, `BunkerBroadcastProxySpawn` : BunkerBroadcastJsonDataBase, `BunkerBroadcastSchedulerSettings` : BunkerBroadcastJsonDataBase, `BunkerBroadcastSettings` : BunkerBroadcastJsonDataBase, `CfgBunkerBroadcastHandler`, `DebugSandstormRPCData`, `DoOnce`, `DustEffectsSettings`, `DustKickupEffectDistanceData`, `DustKickupEffects`, `DustKickupParams`, `DustKickupParticleData`, `ITEM_ExternalLockData` : ITEM_DataBase, `ITEM_SandstormData` : ITEM_DataBase, `PreviewWidget` : Widget, `SandstormController`
- **3_game/effects/** — `ParticleParamsOverrideData`
- **3_game/effects/effectparticle/bulletimpactbase/** — `Hit_Desert_Sand` : EffBulletImpactBase, `Hit_Mud_Brick` : EffBulletImpactBase
- **3_game/entities/** — `AdditionalDoorInfo`, `AdditionalDoorInfosHolder`, `AttachmentGroupBoxData`, `AttachmentGroupPositionData`, `AttachmentGroupProximityMetadata`, `DoorManipulationParams`, `InstancedAttachmentGroupDataBase`, `InstancedAttachmentGroupDataBuilding` : InstancedAttachmentGroupDataBase, `StaticObjectType` : EntityType, `TypeAttachmentGroupData`, `TypeAttachmentGroupsDataHolder`
- **3_game/global/** — `ExplosiveEffectData`
- **3_game/ppemanager/requesters/** — `PPERequester_SandstormEffect` : PPERequester_GameplayBase, `PPERequester_SquintEffects` : PPERequester_GameplayBase
- **3_game/services/** — `ContentDLCStorePrompt` : UIScriptedMenu
- **3_game/systems/** — `ConstructionBasic`, `ConstructionDataTypeHolderBasic`, `WorldTimeTracker`
- **3_game/systems/dynamicmusicplayer/** — `DynamicMusicPlayerRegistryNasdara` : DynamicMusicPlayerRegistry
- **3_game/systems/inventory/** — `HandGuardIsNotSurrendered` : HandGuardBase
- **3_game/vehicles/** — `Motorbike` : Transport, `MotorbikeMove` : TransportMove, `MotorbikeOwnerState` : TransportOwnerState, `MotorbikeType` : TransportType
- **4_world/** — `DigitalCodeLockUIBase` : UIScriptedMenu
- **4_world/classes/** — `BunkerBroadcastComponent` : BunkerBroadcastComponentBase, `BunkerBroadcastComponentBase` : Managed, `BunkerBroadcastComponentDebugData` : Param, `BunkerBroadcastHandler` : Managed, `BunkerBroadcastHandlerPersistenceData`, `BunkerBroadcastLockCodeMode`, `BunkerBroadcastPersistenceStorage` : Managed, `BunkerBroadcastProxy` : Building, `BunkerBroadcastScheduler` : Managed, `BunkerBroadcastSchedulerPersistenceData`, `CodeLockComponent` : Managed, `CodeLockComponentStatic` : CodeLockComponent, `CodeLockVisualManager`, `ConstructionBase` : ConstructionBasic, `ConstructionManagerStatic`, `ThermalBiasHandler`
- **4_world/classes/basebuilding/** — `ActionDismantleDebugData`, `ActionInfoDebugData`, `CharacterStepsDebug`, `ConstructionDebug`, `ConstructionDismantleDebugData`, `ConstructionMaterialData`, `ConstructionPartRebuild` : ConstructionPart, `ConstructionPartRebuildInverted` : ConstructionPartRebuild, `ConstructionPartRebuildReversibleBarricade` : ConstructionPartRebuildReversibleBase, `ConstructionPartRebuildReversibleBase` : ConstructionPartRebuild, `ConstructionPartRebuildReversibleDoor` : ConstructionPartRebuildReversibleBase, `ConstructionPartRebuildRubble` : ConstructionPartRebuild, `PartInstanceInitData`, `PartTypeInitData`, `RebuildingSpawner`
- **4_world/classes/basebuilding/typeconstructiondata/** — `ConstructionDataTypeHolder` : ConstructionDataTypeHolderBasic, `ConstructionPartRebuildInvertedTypeData` : ConstructionPartRebuildTypeData, `ConstructionPartRebuildReversibleBarricadeTypeData` : ConstructionPartRebuildReversibleBaseTypeData, `ConstructionPartRebuildReversibleBaseTypeData` : ConstructionPartRebuildTypeData, `ConstructionPartRebuildReversibleDoorTypeData` : ConstructionPartRebuildReversibleBaseTypeData, `ConstructionPartRebuildRubbleTypeData` : ConstructionPartRebuildTypeData, `ConstructionPartRebuildTypeData` : ConstructionPartTypeData, `ConstructionPartTypeData`
- **4_world/classes/contaminatedarea/** — `OilPitArea` : EffectArea
- **4_world/classes/dust/** — `CeilingDustEffectBase` : EffectParticleGeneral, `CeilingDustEffectWood` : CeilingDustEffectBase, `CeilingDustEffecter` : ParticleEffecter, `DustEffectCeilingHandlerBase`, `DustEffectCeilingHandlerLocal` : DustEffectCeilingHandlerBase, `DustEffectCeilingHandlerSynced` : DustEffectCeilingHandlerBase, `DustEffectManager`
- **4_world/classes/environment/** — `EnvironmentEntityData`, `ShadowDetectionCollisionCallback` : CollisionOverlapCallback
- **4_world/classes/playermodifiers/modifiers/conditions/** — `SandstormExposureDynamicMdfr` : SandstormExposureMdfrBase, `SandstormExposureMdfrBase` : ModifierBase, `SandstormExposureStaticMdfr` : SandstormExposureMdfrBase, `SandstormZoneSmall_TESTONLY` : ContaminatedArea_Static
- **4_world/classes/playermodifiers/modifiers/diseases/** — `HeatStrokeMdfr` : ModifierBase, `HeatStrokePhase1Mdfr` : HeatStrokeMdfr, `HeatStrokePhase2Mdfr` : HeatStrokeMdfr, `HeatStrokePhase3Mdfr` : HeatStrokeMdfr, `HeatStrokePhase4Mdfr` : HeatStrokeMdfr, `IrritatedEyesMdfr` : ModifierBase, `SilicosisMdfr` : ModifierBase
- **4_world/classes/playerstats/** — `PlayerStatsPCO_v130` : PlayerStatsPCO_Base
- **4_world/classes/playersymptoms/** — `SymptomSyncData` : Param
- **4_world/classes/playersymptoms/states/secondary/** — `FaintSymptom` : SymptomBase, `IrritatedEyes` : SymptomBase, `SquintSymptom` : SymptomBase
- **4_world/classes/rebuilding/** — `ConstructionPartEffectsData`, `ConstructionPartStaticsSupportData`, `Rebuilding` : ConstructionBase, `StaticsSupportCategoryValues`
- **4_world/classes/recipes/recipes/** — `CloseGunCase` : RecipeBase, `CraftHandDrillKitBirch` : RecipeBase, `CraftHandDrillKitPalm` : RecipeBase, `CraftLeatherBelt` : RecipeBase, `CraftLeatherHippack` : RecipeBase, `CraftLeatherSheath` : RecipeBase, `CraftMortarMix` : RecipeBase, `CraftWaterskin` : RecipeBase, `DeCraftHandDrillKitBirch` : RecipeBase, `DeCraftHandDrillKitPalm` : RecipeBase, `DeCraftWaterskin` : RecipeBase, `OpenGunCase` : RecipeBase, `PrepareCatfish` : PrepareFish, `PrepareLizard` : PrepareAnimal
- **4_world/classes/recoilbase/recoils/** — `LeeEnfieldRecoil` : RecoilBase, `LugerRecoil` : RecoilBase, `MP18Recoil` : RecoilBase, `SCARLRecoil` : RecoilBase
- **4_world/classes/soundevents/playersoundevents/events/** — `HeavyBreathEvent1` : HeavyBreathSoundEventBase, `HeavyBreathSoundEventBase` : PlayerSoundEventBase
- **4_world/classes/transmissionagents/agents/** — `EyesIrritationAgent` : AgentBase, `SilicosisAgent` : AgentBase
- **4_world/classes/useractionscomponent/** — `ToggleVehicleLightsActionInput` : DefaultActionInput, `VehicleHornLongActionInput` : VehicleHornShortActionInput, `VehicleHornShortActionInput` : ContinuousDefaultActionInput
- **4_world/classes/useractionscomponent/actions/continuous/** — `ActionBuildPartHandsGeneric` : ActionBuildPart, `ActionConstructionPartInfoContinuous` : ActionBuildPart, `ActionConstructionPartInfoInteract` : ActionConstructionPartInfoContinuous, `ActionDrinkWellContinuousBucket` : ActionDrinkWellContinuous, `ActionInfoDataBase`, `ActionInfoDataConstructionBase` : ActionInfoDataBase, `ActionInfoDataConstructionImpossible` : ActionInfoDataConstructionBase, `ActionInfoDataConstructionPossible` : ActionInfoDataConstructionBase, `ActionMixMortarAtWater` : ActionContinuousBase, `ActionMixMortarAtWell` : ActionContinuousBase, `ActionMixMortarCB` : ActionContinuousBaseCB, `ActionObtainLiquidBase` : ActionContinuousBase, `ActionObtainLiquidBaseCB` : ActionContinuousBaseCB, `ActionPickUpBricks` : ActionContinuousBase, `ActionPickUpBricksCB` : ActionContinuousBaseCB, `ActionRepairMotorbikeChassis` : ActionRepairVehiclePartBase, `ActionRepairMotorbikeChassisWithBlowtorch` : ActionRepairMotorbikeChassis, `ActionRepairMotorbikeChassisWithBlowtorchCB` : ActionRepairCarChassisCB, `ActionRepairMotorbikeEngine` : ActionRepairVehiclePartBase, `ActionRepairMotorbikeEngineWithBlowtorch` : ActionRepairMotorbikeEngine, `ActionRepairMotorbikeEngineWithBlowtorchCB` : ActionRepairCarEngineCB, `ActionRepairMotorbikePart` : ActionRepairVehiclePartBase, `ActionRepairMotorbikePartCB` : ActionContinuousBaseCB, `ActionRepairMotorbikePartWithBlowTorch` : ActionRepairMotorbikePart, `ActionRepairMotorbikePartWithBlowTorchCB` : ActionRepairMotorbikePartCB, `ActionWashHeadItemContinuous` : ActionWashHeadWettingClothesBase, `ActionWashHeadItemContinuousCB` : ActionContinuousBaseCB, `ActionWashHeadWater` : ActionWashHeadWettingClothesBase, `ActionWashHeadWaterCB` : ActionContinuousBaseCB, `ActionWashHeadWell` : ActionWashHeadWettingClothesBase, `ActionWashHeadWellCB` : ActionContinuousBaseCB, `ActionWashHeadWettingClothesBase` : ActionContinuousBase, `ActionWetClothingFromContainer` : ActionContinuousBase, `ActionWetClothingFromContainerCB` : ActionContinuousBaseCB, `ActionWetClothingInHandsBase` : ActionContinuousBase, `ActionWetClothingInHandsBaseCB` : ActionContinuousBaseCB, `ActionWetClothingInHandsWater` : ActionWetClothingInHandsBase, `ActionWetClothingInHandsWaterCB` : ActionWetClothingInHandsBaseCB, `ActionWetClothingInHandsWell` : ActionWetClothingInHandsBase, `ActionWetClothingInHandsWellCB` : ActionWetClothingInHandsBaseCB, `ObtainLiquidActionData` : ActionData, `ObtainLiquidActionReciveData` : ActionReciveData, `PickUpBricksActionData` : ActionData, `RepairMotorbikePartActionData` : RepairVehiclePartActionData, `RepairMotorbikePartReceiveData` : RepairVehiclePartActionReciveData
- **4_world/classes/useractionscomponent/actions/continuous/vehicles/** — `ActionStartEngineMotorbike` : ActionContinuousBase, `ActionStartEngineMotorbikeCB` : ActionContinuousBaseCB, `ActionVehicleHornBase`, `ActionVehicleHornLong` : ActionContinuousBase, `ActionVehicleHornShort` : ActionSingleUseBase, `ActionVehicleHornShortCB` : ActionSingleUseBaseCB, `VehicleHornActionData` : ActionData, `VehicleHornActionReceiveData` : ActionReciveData
- **4_world/classes/useractionscomponent/actions/instant/** — `ActionBuildPartInstantDebug` : ActionInstantBase, `ActionDestroyPartInstantDebug` : ActionInstantBase, `ActionDismantlePartInstantDebug` : ActionInstantBase
- **4_world/classes/useractionscomponent/actions/interact/** — `ActionDetachBattery` : ActionInteractBase, `BuildShelterFromShelterSiteLambda` : ReplaceItemWithNewLambdaBase
- **4_world/classes/useractionscomponent/actions/interact/vehicles/** — `ActionAnimateMotorbikeSeat` : ActionAnimateSeats, `ActionPickUpMotorbike` : ActionInteractBase
- **4_world/classes/useractionscomponent/actions/singleuse/** — `ActionCombinationLockUnlock` : ActionInteractBase, `ActionInteractCodeLockBase` : ActionInteractBase, `ActionInteractCodeLockItem` : ActionInteractCodeLockBase, `ActionResetCodeOnCodeLock` : ActionInteractBase, `AttachToConstructionActionData` : ActionData, `InteractCodeLockActionData` : ActionData, `InteractCodeLockActionReciveData` : ActionReciveData
- **4_world/classes/useractionscomponent/actions/singleuse/vehicles/** — `ActionStopEngineMotorbike` : ActionSingleUseBase
- **4_world/classes/useractionscomponent/targetconditionscomponents/** — `CCTCursorInherited` : CCTBase, `CCTLiquid` : CCTBase
- **4_world/classes/worlds/** — `NasdaraData` : WorldData
- **4_world/entities/** — `ExplosionEffectsData`, `ItemSoundEvent`, `Static_rock_sand_apart1` : RockBase, `Static_rock_sand_apart2` : RockBase, `Static_rock_sand_monolith1` : RockBase, `Static_rock_sand_monolith2` : RockBase, `Static_rock_sand_monolith3` : RockBase, `Static_rock_sand_monolith4` : RockBase, `Static_rock_sand_spike1` : RockBase, `Static_rock_sand_spike2` : RockBase, `Static_rock_sand_spike3` : RockBase, `Static_rock_sand_wallh1` : RockBase, `Static_rock_sand_wallh2` : RockBase, `Static_rock_sand_wallh3` : RockBase, `Static_rock_sand_wallhv` : RockBase, `Static_stone1_sand` : RockBase, `Static_stone2_sand` : RockBase, `Static_stone3_sand` : RockBase, `Static_stone4_sand` : RockBase, `Static_stone5_sand` : RockBase, `Static_stones_sand_erosion` : RockBase
- **4_world/entities/building/** — `BroadcastedBunker` : Bunker, `BuildingWithFireplaceNasdara` : BuildingWithFireplace, `Bunker` : House, `Land_Bunker_Basement_Civilian` : BroadcastedBunker, `Land_Bunker_Basement_Medical` : BroadcastedBunker, `Land_Bunker_Basement_Storage` : BroadcastedBunker, `Land_Bunker_Shelter_Medical` : BroadcastedBunker, `Land_Bunker_Shelter_Office` : BroadcastedBunker, `Land_Bunker_Shelter_Storage` : BroadcastedBunker, `Land_WaterWell_Brick` : Well
- **4_world/entities/building/rebuildable/** — `Land_House_1M1` : House, `Land_House_1M2` : House, `Land_House_1M2_Dam2` : House, `Land_House_1M3` : BuildingWithFireplaceNasdara, `Land_House_1M3_Dam2` : BuildingWithFireplaceNasdara, `Land_House_1M4` : BuildingWithFireplaceNasdara, `Land_House_1M4_Dam2` : BuildingWithFireplaceNasdara, `Land_House_1M5` : House, `Land_House_1S1` : BuildingWithFireplaceNasdara, `Land_House_1S2` : BuildingWithFireplace, `Land_House_1S2_Dam2` : BuildingWithFireplace, `Land_House_2M1` : BuildingWithFireplaceNasdara, `Land_House_2M1_Dam2` : BuildingWithFireplaceNasdara, `Land_House_2M2` : BuildingWithFireplaceNasdara, `Land_House_2M2_Dam1` : House, `Land_House_2S1` : BuildingWithFireplace, `Land_House_2S1_Dam3` : BuildingWithFireplace, `Land_House_3S1` : BuildingWithFireplace, `Land_House_3S1_Dam3` : BuildingWithFireplace, `Land_House_3S2` : BuildingWithFireplace, `Land_House_3S2_Dam2` : House, `Land_IrrigationTunnel_Entrance_01` : House
- **4_world/entities/building/residential/houses/** — `Land_House_2S2` : BuildingWithFireplaceNasdara, `Land_House_2S2_Dam1` : BuildingWithFireplaceNasdara
- **4_world/entities/building/residential/misc/** — `Land_Water_Well1` : Well, `Land_Water_Well2` : Well
- **4_world/entities/building/specific/** — `Land_FuelStation_Feed_Takistan` : Land_FuelStation_Feed
- **4_world/entities/building/wrecks/** — `Land_Wreck_Truck01_Aban1_Cistern` : Well, `Land_Wreck_Truck01_Aban1_Cistern_DE` : Land_Wreck_Truck01_Aban1_Cistern, `Land_Wreck_Truck01_Aban2_Cistern` : Well, `Land_Wreck_Truck01_Aban2_Cistern_DE` : Land_Wreck_Truck01_Aban2_Cistern, `StaticObj_Wreck_Chinook` : CrashBase, `StaticObj_Wreck_Chinook_DE` : StaticObj_Wreck_Chinook, `StaticObj_Wreck_Mi24_TK` : CrashBase, `StaticObj_Wreck_Mi24_TK_DE` : StaticObj_Wreck_Mi24_TK
- **4_world/entities/core/inherited/** — `MotorbikePart` : ItemBase, `MotorbikeWheel` : CarWheel, `MotorbikeWheel_Ruined` : CarWheel_Ruined, `Motorbike_01_Wheel_1` : MotorbikeWheel, `Motorbike_01_Wheel_1_Ruined` : MotorbikeWheel_Ruined, `Motorbike_01_Wheel_2` : MotorbikeWheel, `Motorbike_01_Wheel_2_Ruined` : MotorbikeWheel_Ruined, `Motorbike_02_ShieldFront_ColorBase` : MotorbikePart, `Motorbike_02_ShieldLeft_ColorBase` : MotorbikePart, `Motorbike_02_ShieldRight_ColorBase` : MotorbikePart, `Motorbike_02_Wheel_1` : MotorbikeWheel, `Motorbike_02_Wheel_1_Ruined` : MotorbikeWheel_Ruined, `Motorbike_02_Wheel_2` : MotorbikeWheel, `Motorbike_02_Wheel_2_Ruined` : MotorbikeWheel_Ruined, `Offroad_02_Door_1_1_Beige` : CarDoor, `Offroad_02_Door_1_1_BeigeRust` : CarDoor, `Offroad_02_Door_1_2_Beige` : CarDoor, `Offroad_02_Door_1_2_BeigeRust` : CarDoor, `Offroad_02_Door_2_1_Beige` : CarDoor, `Offroad_02_Door_2_1_BeigeRust` : CarDoor, `Offroad_02_Door_2_2_Beige` : CarDoor, `Offroad_02_Door_2_2_BeigeRust` : CarDoor, `Offroad_02_Trunk_Beige` : CarDoor, `Offroad_02_Trunk_BeigeRust` : CarDoor
- **4_world/entities/creatures/animals/** — `Animal_CanisFamiliaris_Dobermann_Bald` : Animal_CanisFamiliaris_Dobermann_ColorBase, `Animal_CanisFamiliaris_Dobermann_Bloody` : Animal_CanisFamiliaris_Dobermann_ColorBase, `Animal_CanisFamiliaris_Dobermann_Brown` : Animal_CanisFamiliaris_Dobermann_ColorBase, `Animal_CanisFamiliaris_Dobermann_ColorBase` : AnimalBase, `Animal_CanisFamiliaris_Dobermann_Skinny` : Animal_CanisFamiliaris_Dobermann_ColorBase, `Animal_CanisFamiliaris_Dobermann_Vest` : Animal_CanisFamiliaris_Dobermann_ColorBase, `Animal_CanisFamiliaris_GermanShepherd_Balding` : Animal_CanisFamiliaris_GermanShepherd_ColorBase, `Animal_CanisFamiliaris_GermanShepherd_Bloody` : Animal_CanisFamiliaris_GermanShepherd_ColorBase, `Animal_CanisFamiliaris_GermanShepherd_ColorBase` : AnimalBase, `Animal_CanisFamiliaris_GermanShepherd_Harness` : Animal_CanisFamiliaris_GermanShepherd_ColorBase, `Animal_CanisFamiliaris_GermanShepherd_Skinny` : Animal_CanisFamiliaris_GermanShepherd_ColorBase, `Animal_OvisGmelini` : AnimalBase, `Animal_VaranusGriseus` : AnimalBase
- **4_world/entities/creatures/infected/** — `ZmbF_PatrolNormal_Base` : ZombieFemaleBase, `ZmbF_SoldierNormal_Base` : ZombieFemaleBase, `ZmbF_eastSoldier_Base` : ZombieFemaleBase, `ZmbF_takiCitizenRural_Base` : ZombieFemaleBase, `ZmbF_takiCitizenUrbanA_Base` : ZombieFemaleBase, `ZmbF_takiCitizenUrbanB_Base` : ZombieFemaleBase, `ZmbF_takiCitizenUrbanC_Base` : ZombieFemaleBase, `ZmbF_takiCitizenUrbanD_Base` : ZombieFemaleBase, `ZmbF_takiOilRigWorker_Base` : ZombieFemaleBase, `ZmbF_takiPoliceWoman_Base` : ZombieFemaleBase, `ZmbM_IonSoldier_Normal_1_Base` : ZombieMaleBase, `ZmbM_IonSoldier_Normal_2_Base` : ZombieMaleBase, `ZmbM_IonSoldier_Normal_3_Base` : ZombieMaleBase, `ZmbM_takiCitizenRural_Base` : ZombieMaleBase, `ZmbM_takiCitizenUrbanA_Base` : ZombieMaleBase, `ZmbM_takiCitizenUrbanB_Base` : ZombieMaleBase, `ZmbM_takiCitizenUrbanC_Base` : ZombieMaleBase, `ZmbM_takiHermitA_Base` : ZombieMaleBase, `ZmbM_takiHermitB_Base` : ZombieMaleBase, `ZmbM_takiHiker_Base` : ZombieMaleBase, `ZmbM_takiImam_Base` : ZombieMaleBase, `ZmbM_takiOilRigWorker_Base` : ZombieMaleBase, `ZmbM_takiPoliceman_Base` : ZombieMaleBase, `ZmbM_takiSoldier_Normal_Base` : ZombieMaleBase, `ZmbM_takiSoldier_Officer_Base` : ZombieMaleBase, `Zmbf_IonSoldier_Normal_1` : ZombieFemaleBase
- **4_world/entities/effects/** — `EffCloudDust` : EffectParticle, `EffVehicleDust` : EffectParticle, `EffWheelContact` : EffectParticle
- **4_world/entities/firearms/automaticrifle/** — `AKM` : AKM_Base, `AKM_Gold` : AKM_Base, `SCARL_Base` : SCARH_Base
- **4_world/entities/firearms/pistol/** — `Luger` : Luger_Base, `Luger_Base` : Pistol_Base, `Magnum_Cylinder_Gold` : Magnum_Cylinder, `Magnum_Ejector_GOld` : Magnum_Ejector, `Magnum_Gold` : Magnum, `SawedoffMagnum_Gold` : SawedoffMagnum
- **4_world/entities/firearms/rifle/** — `LeeEnfield_Base` : BoltActionRifle_ExternalMagazine_Base
- **4_world/entities/firearms/smg/** — `MP18_Base` : OpenBolt_Base
- **4_world/entities/game/super/** — `BuildingLockData`
- **4_world/entities/itembase/** — `Akinaka` : ToolBase, `BrickTrowel` : ToolBase, `CommonBrick` : ItemBase, `DigitalCodeLock` : ItemBase, `DigitalCodeLock_Bunker` : DigitalCodeLock, `DogPelt_ColorBase` : Pelt_Base, `DonerKnife` : ToolBase, `GunCaseKeys_Blue` : GunCaseKeys_Colorbase, `GunCaseKeys_Colorbase` : ItemBase, `GunCaseKeys_Orange` : GunCaseKeys_Colorbase, `GunCaseKeys_Red` : GunCaseKeys_Colorbase, `GunCase_Black` : GunCase_ColorBase, `GunCase_ColorBase` : Container_Base, `GunCase_Orange` : GunCase_ColorBase, `GunCase_White` : GunCase_ColorBase, `HandDrillKitBirch` : HandDrillKit, `HandDrillKitPalm` : HandDrillKit, `Jambiya` : ToolBase, `Land_Mil_ContainerHabitat_Armory_Locked_Green_DE` : ContainerLockedBase, `LizardPelt` : Pelt_Base, `MortarMix` : ItemBase, `MortarMix_Opened` : ItemBase, `PileOfBricks` : ItemBase, `PoliceBaton` : ItemBase, `Scimitar` : ToolBase, `ShippingContainerKeys_Green` : ShippingContainerKeys_ColorBase
- **4_world/entities/itembase/bark_colorbase/** — `Bark_Palm` : Bark_ColorBase
- **4_world/entities/itembase/clothing/** — `ChestRigVest_ColorBase` : Vest_Base, `ChestRigVest_Khaki` : ChestRigVest_ColorBase, `ChestRigVest_Olive` : ChestRigVest_ColorBase, `ChestRigVest_Tan` : ChestRigVest_ColorBase, `DustGoggles_Black` : DustGoggles_ColorBase, `DustGoggles_ColorBase` : Glasses_Base, `DustGoggles_Transparent` : DustGoggles_ColorBase, `FNOJacket` : Top_Base, `FNOPants` : Pants_Base, `GhillieBushrag_Desert` : GhillieBushrag_ColorBase, `GhillieHood_Desert` : GhillieHood_ColorBase, `GhillieSuit_Desert` : GhillieSuit_ColorBase, `GhillieTop_Desert` : GhillieTop_ColorBase, `HighCapacityVest_ION` : HighCapacityVest_ColorBase, `KameezShirt_Beige` : KameezShirt_ColorBase, `KameezShirt_Black` : KameezShirt_ColorBase, `KameezShirt_BlueFlowers` : KameezShirt_ColorBase, `KameezShirt_Brown` : KameezShirt_ColorBase, `KameezShirt_ColorBase` : Top_Base, `KameezShirt_Grey` : KameezShirt_ColorBase, `KameezShirt_NavyBlue` : KameezShirt_ColorBase, `KameezShirt_Olive` : KameezShirt_ColorBase, `KameezShirt_Orange` : KameezShirt_ColorBase, `KameezShirt_Pink` : KameezShirt_ColorBase, `KameezShirt_Purple` : KameezShirt_ColorBase, `KameezShirt_Red` : KameezShirt_ColorBase, `KameezShirt_SkyBlue` : KameezShirt_ColorBase, `KameezShirt_White` : KameezShirt_ColorBase, `KameezShirt_Yellow` : KameezShirt_ColorBase, `LeatherBelt_Lizard` : LeatherBelt_ColorBase, `LeatherHipPack_ColorBase` : Belt_Base, `LeatherHipPack_Lizard` : LeatherHipPack_ColorBase, `LinenVest_Black` : LinenVest_ColorBase, `LinenVest_Brown` : LinenVest_ColorBase, `LinenVest_ColorBase` : Vest_Base, `LinenVest_DarkGrey` : LinenVest_ColorBase, `LinenVest_Green` : LinenVest_ColorBase, `LinenVest_Grey` : LinenVest_ColorBase, `LinenVest_NavyBlue` : LinenVest_ColorBase, `LizardKnifeSheath` : Belt_Base, `MensShalwarPants_Beige` : MensShalwarPants_ColorBase, `MensShalwarPants_Black` : MensShalwarPants_ColorBase, `MensShalwarPants_Brown` : MensShalwarPants_ColorBase, `MensShalwarPants_ColorBase` : Pants_Base, `MensShalwarPants_Grey` : MensShalwarPants_ColorBase, `MensShalwarPants_NavyBlue` : MensShalwarPants_ColorBase, `MensShalwarPants_SkyBlue` : MensShalwarPants_ColorBase, `MensShalwarPants_White` : MensShalwarPants_ColorBase, `MilitaryBeret_TK` : MilitaryBeret_ColorBase, `OBRCap_Beige` : OBRCap_ColorBase, `OBRCap_ColorBase` : HeadGear_Base, `OBRCap_Olive` : OBRCap_ColorBase, `OBRJacket_Beige` : OBRJacket_ColorBase, `OBRJacket_ColorBase` : Top_Base, `OBRJacket_Officer` : OBRJacket_ColorBase, `OBRJacket_Olive` : OBRJacket_ColorBase, `OBRPants_Beige` : OBRPants_ColorBase, `OBRPants_ColorBase` : Pants_Base, `OBRPants_Olive` : OBRPants_ColorBase, `PakolHat_Beige` : PakolHat_ColorBase, `PakolHat_Black` : PakolHat_ColorBase, `PakolHat_Brown` : PakolHat_ColorBase, `PakolHat_ColorBase` : HeadGear_Base, `PakolHat_Grey` : PakolHat_ColorBase, `PakolHat_White` : PakolHat_ColorBase, `PoliceShortSleeveShirt_ColorBase` : Top_Base, `RobeVest_Beige` : RobeVest_ColorBase, `RobeVest_Brown` : RobeVest_ColorBase, `RobeVest_ColorBase` : Vest_Base, `RobeVest_Green` : RobeVest_ColorBase, `RobeVest_Grey` : RobeVest_ColorBase, `RobeVest_Navy` : RobeVest_ColorBase, `SandalsShoes_Black` : SandalsShoes_ColorBase, `SandalsShoes_BlueNavy` : SandalsShoes_ColorBase, `SandalsShoes_Brown` : SandalsShoes_ColorBase, `SandalsShoes_ColorBase` : Shoes_Base, `SandalsShoes_Grey` : SandalsShoes_ColorBase, `ShortSleeveShirt_Black` : ShortSleeveShirt_ColorBase, `ShortSleeveShirt_BlueRectangles` : ShortSleeveShirt_ColorBase, `ShortSleeveShirt_ColorBase` : Top_Base, `ShortSleeveShirt_CreamSquares` : ShortSleeveShirt_ColorBase, `ShortSleeveShirt_Green` : ShortSleeveShirt_ColorBase, `ShortSleeveShirt_Hawaiian_Black` : ShortSleeveShirt_ColorBase, `ShortSleeveShirt_Hawaiian_Blue` : ShortSleeveShirt_ColorBase, `ShortSleeveShirt_Hawaiian_White` : ShortSleeveShirt_ColorBase, `ShortSleeveShirt_Waves` : ShortSleeveShirt_ColorBase, `ShortSleeveShirt_White` : ShortSleeveShirt_ColorBase, `TacticalShirt_ION_Olive` : TacticalShirt_ColorBase, `TacticalShirt_ION_Tan` : TacticalShirt_ColorBase, `TraditionalSandalShoes_Black` : TraditionalSandalShoes_ColorBase, `TraditionalSandalShoes_Brown` : TraditionalSandalShoes_ColorBase, `TraditionalSandalShoes_ColorBase` : Shoes_Base, `TraditionalSandalShoes_LightBrown` : TraditionalSandalShoes_ColorBase, `TraditionalSandalShoes_NavyBlue` : TraditionalSandalShoes_ColorBase, `TurbanHat_Beige` : TurbanHat_ColorBase, `TurbanHat_Blue` : TurbanHat_ColorBase, `TurbanHat_Brown` : TurbanHat_ColorBase, `TurbanHat_ColorBase` : HeadGear_Base, `TurbanHat_Gold` : TurbanHat_ColorBase, `TurbanHat_Green` : TurbanHat_ColorBase, `TurbanHat_Grey` : TurbanHat_ColorBase, `TurbanHat_LightGreen` : TurbanHat_ColorBase, `TurbanHat_Pink` : TurbanHat_ColorBase, `TurbanHat_Purple` : TurbanHat_ColorBase, `TurbanHat_Red` : TurbanHat_ColorBase, `TurbanHat_Yellow` : TurbanHat_ColorBase, `WomensShalwarPants_ColorBase` : Pants_Base, `WomensShalwarPants_NavyBlue` : WomensShalwarPants_ColorBase, `WomensShalwarPants_Olive` : WomensShalwarPants_ColorBase, `WomensShalwarPants_Orange` : WomensShalwarPants_ColorBase, `WomensShalwarPants_Pink` : WomensShalwarPants_ColorBase, `WomensShalwarPants_Purple` : WomensShalwarPants_ColorBase, `WomensShalwarPants_Red` : WomensShalwarPants_ColorBase, `WomensShalwarPants_SkyBlue` : WomensShalwarPants_ColorBase
- **4_world/entities/itembase/container_base/** — `StorageBox_Colorbase` : Container_Base, `StorageBox_Wooden` : StorageBox_Colorbase
- **4_world/entities/itembase/edible_base/** — `Catfish` : Edible_Base, `CatfishFilletMeat` : Edible_Base, `DeadLizard` : Edible_Base, `DogSteakMeat` : Edible_Base, `Figs` : Edible_Base, `LizardMeat` : Edible_Base, `RoundGoby` : Edible_Base
- **4_world/entities/itembase/edible_base/bottle_base/** — `AyranBottle` : Bottle_Base, `CeramicPot_ColorBase` : Pot
- **4_world/entities/itembase/fireplacebase/** — `FireplaceTandoor` : FireplaceIndoor
- **4_world/entities/itembase/gear/consumables/** — `Keffiyeh_Black` : Keffiyeh_ColorBase, `Keffiyeh_Blue` : Keffiyeh_ColorBase, `Keffiyeh_Brown` : Keffiyeh_ColorBase, `Keffiyeh_ColorBase` : Clothing, `Keffiyeh_Gray` : Keffiyeh_ColorBase, `Keffiyeh_LightBlue` : Keffiyeh_ColorBase, `Keffiyeh_LightBrown` : Keffiyeh_ColorBase, `Keffiyeh_LightRed` : Keffiyeh_ColorBase, `Keffiyeh_Red` : Keffiyeh_ColorBase
- **4_world/entities/itembase/gear/containers/** — `NBCPack_ColorBase` : Container_Base, `NBCPack_Grey` : NBCPack_ColorBase, `NBCPack_White` : NBCPack_ColorBase, `NBCPack_Yellow` : NBCPack_ColorBase
- **4_world/entities/itembase/gear/drinks/** — `Ayran` : Edible_Base, `Lassi_Colorbase` : Edible_Base, `Lassi_Mango` : Lassi_Colorbase, `Lassi_Masala` : Lassi_Colorbase, `Lassi_Mint` : Lassi_Colorbase, `SmallGlassBottle_Afza` : SmallGlassBottle_ColorBase, `SmallGlassBottle_ColorBase` : SodaCan_ColorBase, `SmallGlassBottle_Jallab` : SmallGlassBottle_ColorBase, `SodaCan_Cola` : SodaCan_ColorBase, `SodaCan_Fronta` : SodaCan_ColorBase, `SodaCan_Kvass` : SodaCan_ColorBase, `SodaCan_Pipsi` : SodaCan_ColorBase, `SodaCan_Spite` : SodaCan_ColorBase, `SodaCan_Takola` : SodaCan_ColorBase, `SodaCan_ZumZam` : SodaCan_ColorBase, `Waterskin_ColorBase` : Bottle_Base, `Waterskin_Natural` : Waterskin_ColorBase, `WineBottle` : Bottle_Base
- **4_world/entities/itembase/gear/food/** — `Achar` : Edible_Base, `HalvaCan` : Edible_Base, `HalvaCan_Opened` : Edible_Base, `SarmaCan` : Edible_Base, `SarmaCan_Opened` : Edible_Base
- **4_world/entities/itembase/inventory_base/** — `P1907_Bayonet` : ItemBase
- **4_world/entities/itembase/magazine/** — `Mag_LeeEnfield_10Rnd` : MagazineStorage, `Mag_Luger_8Rnd` : MagazineStorage, `Mag_MP18_Drum32Rnd` : MagazineStorage
- **4_world/entities/itembase/weaponattachments/** — `GhillieAtt_Desert` : GhillieAtt_ColorBase
- **4_world/entities/manbase/dayzplayer/** — `DayZPlayerCamera3rdPersonVehicleMotorbike` : DayZPlayerCameraBase
- **4_world/entities/navmeshtest/** — `NavmeshTest` : Building, `NavmeshTest_A` : NavmeshTest, `NavmeshTest_B` : NavmeshTest, `NavmeshTest_C` : NavmeshTest, `NavmeshTest_D` : NavmeshTest, `NavmeshTest_E` : NavmeshTest, `NavmeshTest_F` : NavmeshTest
- **4_world/entities/scriptedentities/triggers/** — `OilPitTrigger` : EffectTrigger
- **4_world/entities/scriptedlightbase/spotlightbase/** — `VehicleLightBase` : SpotLightBase
- **4_world/entities/scriptedlightbase/spotlightbase/vehiclelightprofiles/** — `CivilianSedanLightProfileFront` : VehicleLightProfileFront, `CivilianSedanLightProfileRear` : VehicleLightProfileBrakeAndReverse, `Hatchback_02LightProfileFront` : VehicleLightProfileFront, `Hatchback_02LightProfileRear` : VehicleLightProfileBrakeAndReverse, `Motorbike_01LightProfileFront` : VehicleLightProfileFront, `Motorbike_01LightProfileRear` : VehicleLightProfileBrakeAndReverse, `Motorbike_02LightProfileFront` : VehicleLightProfileFront, `Motorbike_02LightProfileRear` : VehicleLightProfileBrakeAndReverse, `OffroadHatchbackLightProfileFront` : VehicleLightProfileFront, `OffroadHatchbackLightProfileRear` : VehicleLightProfileBrakeAndReverse, `Offroad_02LightProfileFront` : VehicleLightProfileFront, `Offroad_02LightProfileRear` : VehicleLightProfileBrakeAndReverse, `Sedan_02LightProfileFront` : VehicleLightProfileFront, `Sedan_02LightProfileRear` : VehicleLightProfileBrakeAndReverse, `Truck_01LightProfileFront` : VehicleLightProfileFront, `Truck_01LightProfileRear` : VehicleLightProfileBrakeAndReverse, `VehicleLightProfileBase`, `VehicleLightProfileBrakeAndReverse` : VehicleLightProfileBase, `VehicleLightProfileFront` : VehicleLightProfileBase, `VehicleLightProfileRear` : VehicleLightProfileBase
- **4_world/entities/vehicles/** — `MotorbikeScript` : Motorbike, `MotorbikeScriptMove` : MotorbikeMove, `MotorbikeScriptOwnerState` : MotorbikeOwnerState
- **4_world/entities/vehicles/components/** — `VehicleHornComponent` : Managed, `VehicleHornMode`, `VehicleLightData`, `VehicleLightMode`, `VehicleLightSelectionData`, `VehicleLightType`, `VehicleLightsComponent`, `VehicleVFXComponent`, `VehicleVFXData`, `VehicleVFXDataParams`
- **4_world/entities/vehicles/inheritedcars/** — `Offroad_02_Beige` : Offroad_02
- **4_world/entities/vehicles/inheritedmotorbikes/** — `Motorbike_01_Blue` : Motorbike_01_ColorBase, `Motorbike_01_ColorBase` : MotorbikeScript, `Motorbike_01_Red` : Motorbike_01_ColorBase, `Motorbike_01_Yellow` : Motorbike_01_ColorBase, `Motorbike_02_ColorBase` : MotorbikeScript, `Motorbike_02_Red` : Motorbike_02_ColorBase
- **4_world/entities/woodbase/** — `BushHard_b_Cycas_2s` : BushHard, `BushHard_b_FicusBenjamina_2d` : BushHard, `BushHard_b_FicusBenjamina_2s` : BushHard, `BushHard_b_FicusBenjamina_2sb` : BushHard, `BushHard_b_Gardenia_1s` : BushHard, `BushHard_b_NeriumOleander_1d` : BushHard, `BushHard_b_NeriumOleander_1s` : BushHard, `BushHard_b_NeriumOleander_1sb` : BushHard, `BushHard_b_OpuntiaFicusIndica_1s` : BushHard, `BushHard_b_OpuntiaFicusIndica_1sb` : BushHard, `BushHard_b_OpuntiaFicusIndica_2s` : BushHard, `BushHard_b_OpuntiaFicusIndica_2sb` : BushHard, `BushHard_b_OpuntiaFicusIndica_3s` : BushHard, `BushHard_b_OpuntiaFicusIndica_3sb` : BushHard, `TreeHard_t_Albizia_3s` : TreeHard, `TreeHard_t_Broussonetia_1s` : TreeHard, `TreeHard_t_FicusBenjamina_2d` : TreeHard, `TreeHard_t_FicusBenjamina_2s` : TreeHard, `TreeHard_t_FraxinusExcelsior_2s_spring` : TreeHard, `TreeHard_t_FraxinusExcelsior_2w_spring` : TreeHard, `TreeHard_t_OleaEuropaea_2s` : TreeHard, `TreeHard_t_PhoenixRupicola_1s` : TreeHard, `TreeHard_t_PhoenixRupicola_3s` : TreeHard, `TreeHard_t_PinusBrutia_1s` : TreeHard, `TreeHard_t_PinusBrutia_2sb` : TreeHard, `TreeHard_t_Populus_2s` : TreeHard, `TreeHard_t_Populus_2sb` : TreeHard, `TreeHard_t_PrunusDomestica_2s` : TreeHard, `TreeSoft_t_FicusBenjamina_1s` : TreeSoft, `TreeSoft_t_OleaEuropaea_1s` : TreeSoft, `TreeSoft_t_PinusBrutia_2s` : TreeSoft, `TreeSoft_t_PrunusDomestica_1s` : TreeSoft
- **4_world/plugins/pluginbase/** — `PluginUndergroundTriggerManager` : PluginBase, `ScriptConsoleItem`
- **4_world/plugins/pluginbase/plugindiagmenu/** — `ArrayBoxDraw` : array, `ArrayBreadcrumbDraw` : array, `BoxDrawCenterGizmo` : BoxDrawGizmo, `BoxDrawCornerGizmo` : BoxDrawGizmo, `BreadcrumbDraw` : UndergroundTriggerDrawBase, `BreadcrumbGizmo` : DrawGizmoBase, `DrawGizmoBase` : Managed, `UndergroundBreadcrumbHandle` : ItemBase, `UndergroundTriggerDrawBase` : Managed, `UndergroundTriggerHandle` : ItemBase
- **4_world/systems/animalcatchingsystem/yielditems/** — `YieldItemCatfish` : FishYieldItemBase, `YieldItemDeadLizard` : LandAnimalYieldItemBase, `YieldItemRoundGoby` : FishYieldItemBase
- **4_world/systems/bot/** — `BotAttack` : BotStateBase, `BotEventAttacked` : BotEventBase, `BotEventObstacle` : BotEventBase, `BotEventValidForward` : BotEventBase, `BotMoveForward` : BotStateBase, `BotMovementRandomizer` : BotStateBase, `BotStateMachineData`, `BotTurnToValidForward` : BotStateBase, `RoboclientUtils`
- **4_world/systems/sandstorm/** — `PlayerSandstormData`, `ScriptedSandstormController` : SandstormController
- **5_mission/gui/** — `DigitalCodeLockUI` : DigitalCodeLockUIBase, `ScriptConsoleMenuElement` : ScriptedWidgetEventHandler, `ScriptConsoleTabRegistry`, `ScriptConsoleTabRegistryEntry`, `UndergroundTriggerEditorCustomListElement` : ScriptedWidgetEventHandler, `UndergroundTriggerEditorDialog` : UndergroundTriggerEditorPopupBase, `UndergroundTriggerEditorDiscardBreadcrumbsDialog` : UndergroundTriggerEditorDialog, `UndergroundTriggerEditorMenu` : UIScriptedMenu, `UndergroundTriggerEditorPopupBase` : ScriptedWidgetEventHandler, `UndergroundTriggerEditorReloadDialog` : UndergroundTriggerEditorDialog, `UndergroundTriggerEditorRenameBreadcrumbDialog` : UndergroundTriggerEditorDialog, `UndergroundTriggerEditorRenameDialog` : UndergroundTriggerEditorDialog
- **5_mission/gui/inventorynew/containeditems/** — `ActionInfoGridBase` : LayoutHolder, `ActionInfoPanelBase` : LayoutHolder, `ConstructionActionInfoGrid` : ActionInfoGridBase, `ConstructionActionInfoMaterialsGrid` : ConstructionActionInfoGrid, `ConstructionActionInfoPanel` : TargetActionInfoPanelBase, `ConstructionActionInfoToolsGrid` : ConstructionActionInfoGrid, `ConstructionInfoIcon` : GenericIconBase, `ConstructionMaterialInfoIcon` : ConstructionInfoIcon, `ConstructionToolInfoIcon` : ConstructionInfoIcon, `GenericIconBase` : SlotsIconBase, `SlotsIconBase` : LayoutHolder, `TargetActionInfoPanelBase` : ActionInfoPanelBase
- **5_mission/gui/scriptshuddebug/** — `HudDebugWinCharAgentsEntry` : ScriptedWidgetEventHandler
- **5_mission/gui/vehicles/** — `MotorbikeHud` : VehicleHudBase
- **5_mission/mission/gameplayeffectwidgets/** — `GameplayEffectsMetaDataEyeVeins` : GameplayEffectsData, `GameplayEffectsMetaDataSquinting` : GameplayEffectsData
