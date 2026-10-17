# HUD Action Info Panels & Cursor Construction Grids — DayZ 1.30 Reference

> Target: DayZ 1.30 stable (1.30.164078). Version labels: ../../_shared/dayz-version-policy.md.

Added 2026-09-16 from the 1.30 Experimental build (1.30.164014); re-verified line by line on 2026-10-15
against 1.30.164078 stable (`P:\`). Paths are relative to `P:\scripts\` or `P:\gui\layouts\`:
`5_mission/gui/actiontargetscursor.c`, `5_mission/gui/inventorynew/containeditems/{actioninfopanels,
actioninfogrids,constructioninfoicons,genericicon,slotsiconbase}.c`,
`4_world/classes/useractionscomponent/actionbase.c`, layouts `new_ui/hud/action_info_spacer.layout`,
`new_ui/hud/simpleicontemplate.layout`, `day_z_hud.layout`. Everything in this file is new in 1.30
unless a line says "1.29". Corrected on re-verification (marked *Corrected* below): the separator
widget, the icon template tree, the tool-icon mapping, the materials data source (§3, §4) and the
history of `SetControllerIcon` (§2.4). Whether Exp really differed there or the earlier text was simply
wrong was not checked (no Exp tree was compared); the text now matches 164078.

---

## 1. Overview & Architecture

In DayZ 1.30, the interaction cursor (`ActionTargetsCursor`) introduces a modular subsystem for contextual action information, specifically designed for multi-stage construction, crafting, and repair actions:

```
ActionTargetsCursor (5_mission/gui/actiontargetscursor.c)
 └── m_TargetActionInfoPanelMap: map<int, ref array<ref TargetActionInfoPanelBase>>   (:94)
      └── TargetActionInfoPanelBase : ActionInfoPanelBase : LayoutHolder (actioninfopanels.c:5, :2)
           ├── Layout: "Gui/layouts/new_ui/hud/action_info_spacer.layout"  (SetLayoutName, :29-32)
           └── ConstructionActionInfoPanel (actioninfopanels.c:82)
                ├── ConstructionActionInfoToolsGrid  → widget "SecondaryGrid0" (actioninfogrids.c:103)
                │    └── array<ref GenericIconBase> of ConstructionToolInfoIcon (constructioninfoicons.c:52)
                ├── Widget m_Separator ("Separator", shown only when both grids are visible, :100-107)
                └── ConstructionActionInfoMaterialsGrid → widget "SecondaryGrid1" (actioninfogrids.c:236)
                     └── array<ref GenericIconBase> of ConstructionMaterialInfoIcon (constructioninfoicons.c:161)
```

It adds, under the cursor rows, dynamic grids showing:
1. **Required tools** for the action (highlighted or ghosted depending on what is in hands).
2. **Required materials** with RichText quantity text (`<color hex="...">X</color><color hex="...">/Y</color>`).

Data source: an action opts in by owning an `ActionInfoDataBase` in `ActionBase.m_InfoData`
(`4_world/classes/useractionscomponent/actionbase.c:70`); `IsTargetInfoAction()` returns
`m_InfoData != null` (`:277-280`) and `GetClientActionInfo()` returns it (`:282-285`). Vanilla users:
`ActionBuildPart` (`…/actions/continuous/actionbuildpart.c:76`, `ActionInfoDataConstructionPossible`)
and the new indication-only `ActionConstructionPartInfoContinuous : ActionBuildPart` /
`ActionConstructionPartInfoInteract` (`…/actions/continuous/actionpartinfo.c:1, 200`; sets
`ActionInfoDataConstructionImpossible` at `:9`); `ActionInfoDataBase` itself is declared at
`actionpartinfo.c:209`. `ActionBase.CanBeStarted()` (`actionbase.c:288`, default `true`) is what
those info-only actions return `false` from, which triggers the dimming in §2.3.

---

## 2. ActionTargetsCursor Integration

### 2.1. Panel Initialization & Registration

In `5_mission/gui/actiontargetscursor.c:152-176`, panels are registered by action category
(`InitActionInfoPanels()` is called once from `OnWidgetScriptInit`, `:190`):

```c
// [EXACT] P:\scripts\5_mission\gui\actiontargetscursor.c:152-176
protected void InitActionInfoPanels()
{
	m_TargetActionInfoPanelMap = new map<int, ref array<ref TargetActionInfoPanelBase>>();
	m_ActiveInfoPanels = new array<ref TargetActionInfoPanelBase>();
	
	CreateActionInfoPanel(ConstructionActionInfoPanel,ACTION_ORDER_INTERACT_CONTINUOUS); //hand builds (regular + indication only)
	CreateActionInfoPanel(ConstructionActionInfoPanel,ACTION_ORDER_CONTINUOUS); //build (regular + indication only)
	
	m_Root.Update();
}

protected void CreateActionInfoPanel(typename panelType, int actionCategory)
{
	TargetActionInfoPanelBase tInfoPanel = TargetActionInfoPanelBase.Cast(panelType.Spawn());
	tInfoPanel.SetParentWidget(m_Root);
	m_Root.AddChild(tInfoPanel.GetMainWidget(),false); //ce n'est pas un LayoutHolder, bypassing inheritance
	tInfoPanel.Init();
	
	if (!m_TargetActionInfoPanelMap.Get(actionCategory))
		m_TargetActionInfoPanelMap.Set(actionCategory,new array<ref TargetActionInfoPanelBase>());
	
	array<ref TargetActionInfoPanelBase> tmp = m_TargetActionInfoPanelMap.Get(actionCategory);
	tmp.Insert(tInfoPanel);
	m_TargetActionInfoPanelMap.Set(actionCategory, tmp);
}
```

### 2.2. Per-Frame Target Info Update

At the end of every cursor content rebuild (`PrepareCursorContent`, `actiontargetscursor.c:264`),
`UpdateAdditionalActionInfo` (`:1268-1292`) iterates over `m_ActionMap` (filled per category at
`:671-677`) and, for each action with `IsTargetInfoAction()`, updates its panels (or clears them if it
no longer applies):

```c
// [EXACT] P:\scripts\5_mission\gui\actiontargetscursor.c:1268-1292
	protected void UpdateAdditionalActionInfo()
	{
		m_ActiveInfoPanels.Clear();
		
		foreach (int actionType, ActionBase action : m_ActionMap)
		{
			bool update = action != null && action.IsTargetInfoAction();
			array<ref TargetActionInfoPanelBase> tmp = m_TargetActionInfoPanelMap.Get(actionType);
			if (tmp)
			{
				foreach (TargetActionInfoPanelBase panel : tmp)
				{
					if (update)
					{
						panel.UpdateTargetActionInfoPanel(action.GetClientActionInfo(),m_Target);
						m_ActiveInfoPanels.Insert(panel);
					}
					else if (panel.IsVisible())
					{
						panel.PerformPanelCleanup();
					}
				}
			}
		}
	}
```

### 2.3. Visual Dimming for Non-Startable Actions

In `5_mission/gui/actiontargetscursor.c:1118-1127`, if an action cannot currently be started (`!action.CanBeStarted()`), button prompt icons are dimmed to alpha `0.149` (the `else` branch restores `1.0`). Not present in 1.29:

```c
// [EXACT] P:\scripts\5_mission\gui\actiontargetscursor.c:1118-1122
if (!action.CanBeStarted())
{
	widget.FindAnyWidget(actionWidget + "_btn_icon_xbox").SetAlpha(0.149);
	widget.FindAnyWidget(actionWidget + "_btn_icon").SetAlpha(0.149);
}
```

### 2.4. Controller Icon Binding (Gamepad / Console)

The legacy Xbox-specific icon setters carry `[Obsolete]` in 1.30 (moved to the end of the class):
```c
// [EXACT] P:\scripts\5_mission\gui\actiontargetscursor.c:1368-1377
[Obsolete("no replacement")]
void SetInteractXboxIcon(string imageset_name, string image_name);
[Obsolete("no replacement")]
void SetContinuousInteractXboxIcon(string imageset_name, string image_name);
[Obsolete("no replacement")]
void SetSingleXboxIcon(string imageset_name, string image_name);
[Obsolete("no replacement")]
void SetContinuousXboxIcon(string imageset_name, string image_name);
[Obsolete("no replacement")]
protected void SetXboxIcon(string name, string imageset_name, string image_name);
```

The live path is `SetControllerIcon`, called for the four rows from `UpdateControllerInputIcons()`
(`:208-214`):
```c
// [EXACT] P:\scripts\5_mission\gui\actiontargetscursor.c:146-150 (identical in 1.29 at :139-143)
protected void SetControllerIcon(string pWidgetName, string pInputName)
{
	RichTextWidget w = RichTextWidget.Cast(m_Root.FindAnyWidget(pWidgetName + "_btn_icon_xbox"));	
	w.SetText(InputUtils.GetRichtextButtonIconFromInputAction(pInputName, "", EUAINPUT_DEVICE_CONTROLLER));
}
```
Before 1.30: the same five `Set*XboxIcon` prototypes already existed **without bodies** under a
`//! DEPRECATED` comment (1.29: `scripts/5_mission/gui/actiontargetscursor.c:130-136`), and
`SetControllerIcon` was already the working path. So 1.30 only adds the `[Obsolete]` attribute
(compile-time `SCRIPT (W)` warning); calls in an old mod were no-ops then and are no-ops now — delete
them. *(Corrected 2026-10-15: the Exp note said SetControllerIcon replaced fixed Xbox images in 1.30.)*

### 2.5. Override breakers: `Update(bool)` and `BuildFixedCursor(bool)`

```c
// [EXACT] P:\scripts\5_mission\gui\actiontargetscursor.c:267, :281, :289-297, :310
protected void BuildFixedCursor(bool forceRebuild = true)   // 1.29: BuildFixedCursor()
protected void BuildFloatingCursor(bool forceRebuild)       // unchanged
override bool OnUpdate(Widget w) { if (m_Root == w) { Update(false); return true; } return false; }
void Update(bool fullUpdate = true)                          // 1.29: Update()
```

- A 1.29 `override void Update()` or `override void BuildFixedCursor()` in `modded class
  ActionTargetsCursor` does not compile on 1.30 (bzed reports the same; confirmed against `P:\`).
- `IngameHud.Update(float)` still calls the full `m_ActionTargetsCursor.Update()`
  (`5_mission/gui/ingamehud.c:1145`). The widget's `OnUpdate` now calls `Update(false)`: no
  `GetTarget()/GetActions()` (`:340-344`) and `BuildFixedCursor(false)` / `BuildFloatingCursor(false)`,
  i.e. reposition only, no `PrepareCursorContent()`. Before 1.30: `OnUpdate` called the full
  `Update()` (1.29: `:285-293`).
- Port:
  ```c
  modded class ActionTargetsCursor
  {
      override void Update(bool fullUpdate = true)
      {
          super.Update(fullUpdate);
          if (!fullUpdate)
              return;          // light reposition pass: skip mod work
          // mod work that needs fresh m_Target / actions
      }
  }
  ```

---

## 3. Layout Structure

### 3.1. `action_info_spacer.layout`

Path: `P:\gui\layouts\new_ui\hud\action_info_spacer.layout` (classic header form; new in 1.30)

Hierarchy:
```
WrapSpacerWidgetClass MainWrap (size 350 51, style Empty, Size To Content V 1)          :1
 └── GridSpacerWidgetClass MainGrid (Columns 100, Rows 100, Size To Content H/V)       :19
      ├── GridSpacerWidgetClass SecondaryGrid0 (size 51 51, Rows 100)  <-- tools grid  :35
      │    └── WrapSpacerWidgetClass IconMarginCard_Wrap4..6 (size 55 55)
      │         └── ImageWidgetClass IconBg_Image4 / IconBg_Image5 / IconBg_Imag6 (51x51, alpha 0)
      ├── PanelWidgetClass Separator (size 14 55)                                      :134
      │    └── ImageWidgetClass Spacer_Image (size 3 15, image0 "")
      └── GridSpacerWidgetClass SecondaryGrid1 (size 51 51, Columns 5, Rows 100) <-- materials :161
           └── WrapSpacerWidgetClass IconMarginCard_Wrap* (size 55 55)
```
*(Corrected 2026-10-15: the Exp text had `ImageWidgetClass Separator (size 2 28, dayz_gui:line_vertical)`.)*
The pre-placed margin cards are hidden on grid init (`ActionInfoGridBase.InitInfoGrid`,
`actioninfogrids.c:14-31`); icons are created at runtime.

### 3.2. `simpleicontemplate.layout`

Path: `P:\gui\layouts\new_ui\hud\simpleicontemplate.layout` (the `P:\` unpack lower-cases file
names; the script asks for `"Gui/layouts/new_ui/hud/SimpleIconTemplate.layout"`, `genericicon.c:65`).

Hierarchy used by `GenericIconBase` (top-level children of `Icon`, abridged):
```
FrameWidgetClass Icon (size 59 59)                                           :1
 ├── ImageWidgetClass ImageBackdrop (color 0 0 0 0.231)                      :14
 ├── PanelWidgetClass EmptySelected (style Outline)                          :30
 ├── ImageWidgetClass GhostSlot ("set:dayz_inventory image:missing", alpha 0.4)  :44
 ├── PanelWidgetClass RadialIconPanel (RadialIconClosed, RadialIcon)         :63
 ├── PanelWidgetClass Col (style rover_sim_colorable)                        :122
 ├── ImageWidgetClass SomethingWrongIcon / GenericWarning (+ TextWidget Exclamation, style Normal_SDF)  :136, :156
 ├── PanelWidgetClass PanelWidget (style blank)                              :199
 │    └── ItemPreviewWidgetClass Render
 │         └── OutOfReach, Mounted, AmmoIcon, Cursor, TemperaturePanel/Temperature
 └── PanelWidgetClass QuantityPanel                                          :338
      ├── ProgressBarWidgetClass QuantityBar
      ├── GridSpacerWidgetClass QuantityStackPanel → RichTextWidgetClass Quantity
      └── GridSpacerWidgetClass QuantityStackPanel0 → TextWidgetClass Quantity0
```
*(Corrected 2026-10-15: the Exp text listed `PanelWidget` as a GridSpacer holding `Col`, and put
`Quantity`/`QuantityBar`/`QuantityStackPanel` directly under `Icon`.)*

---

## 4. Class Hierarchy & Script API

### 4.1. `TargetActionInfoPanelBase` & `ConstructionActionInfoPanel`

Located in `5_mission/gui/inventorynew/containeditems/actioninfopanels.c:1-108`:

```c
// [EXACT] P:\scripts\5_mission\gui\inventorynew\containeditems\actioninfopanels.c:5-19
class TargetActionInfoPanelBase: ActionInfoPanelBase
{
	protected int m_VisibleGridCount;
	protected ActionInfoDataBase m_ActionInfo;
	protected ActionTarget m_Target;
	protected ref array<ref ActionInfoGridBase> m_GridsArray;
	
	void Init()
	{
		InitGridStructure();
	}
	
	protected void InitGridStructure()
	{
		m_GridsArray = new array<ref ActionInfoGridBase>();
	}
```

`ConstructionActionInfoPanel` (`actioninfopanels.c:82-108`) creates the two grids and toggles the separator in `UpdateInterval` (`:100-107`):

```c
// [EXACT] P:\scripts\5_mission\gui\inventorynew\containeditems\actioninfopanels.c:82-98
class ConstructionActionInfoPanel: TargetActionInfoPanelBase
{
	protected Widget m_Separator;
	
	void ConstructionActionInfoPanel(LayoutHolder parent)
	{
	}
	
	override protected void InitGridStructure()
	{
		super.InitGridStructure();
		
		m_GridsArray.Insert(new ConstructionActionInfoToolsGrid(this, m_RootWidget.FindAnyWidget("SecondaryGrid0")));
		m_GridsArray.Insert(new ConstructionActionInfoMaterialsGrid(this, m_RootWidget.FindAnyWidget("SecondaryGrid1")));
		
		m_Separator = m_RootWidget.FindAnyWidget("Separator");
	}
```

### 4.2. `ActionInfoGrids`

Located in `5_mission/gui/inventorynew/containeditems/actioninfogrids.c:1-289`:

- **`ActionInfoGridBase: LayoutHolder`** (`:1`): handler for an existing grid widget (its
  `SetLayoutName()` sets `""`, `:34-37`); hides the pre-placed margin cards (`:14-31`), keeps an array
  of `GenericIconBase`, adds icons via `CreateNewIcon(int idx = -1)` (`:40-43`), and shows/hides itself
  from `m_IconVisibleCount` in `UpdateInterval` (`:65`).
- **`ConstructionActionInfoToolsGrid`** (`:103`): runs when `m_InfoContextMask` has
  `EConstructionInfoCategories.TOOL_PRESENCE`; reads `ConstructionPart.GetBuildIndicationTypeMask()`
  (`:134`) and the in-hands tool's `cfgVehicles <type> build_action_type` (`:140-148`; no config entry →
  `TOOL_NO_ACTION`, no tool → `TOOL_HANDS`), creating one `ConstructionToolInfoIcon` per
  `EConstructionTools` bit (`:115`, `CreateNewIcon(1 << (i - 1))`).
- **`ConstructionActionInfoMaterialsGrid`** (`:236`): runs when the mask has `MATERIALS_PRESENCE`;
  pre-creates `DEFAULT_MATERIAL_ICON_COUNT = 5` icons (`:238-249`) and hands each the
  `ConstructionPart` and target (`:256-282`). The icon reads the requirements itself from
  `m_Part.GetMaterialsData()` (`constructioninfoicons.c:223`).
  *(Corrected 2026-10-15: the Exp text cited `part.GetBuildTypeMask()`, `part.GetMaterialsSlotName(i)`
  and `part.GetMaterialSlotQuantity(i)`; the 1.30 grids call none of them.)*

### 4.3. `GenericIconBase` & `ConstructionInfoIcons`

- **`GenericIconBase: SlotsIconBase`** (`5_mission/gui/inventorynew/containeditems/genericicon.c:1`):
  Extends `SlotsIconBase` to render standalone icons outside `InventoryMenu`, on
  `new_ui/hud/SimpleIconTemplate.layout` (`:63-66`). It overrides `InitIcon` / `InitIconWidgets`
  (`:9`, `:16`) — the same hooks `SlotsIcon` overrides — and casts the quantity text to `RichTextWidget`,
  not `TextWidget`: `m_QuantityItem = RichTextWidget.Cast(m_MainWidget.FindAnyWidget("Quantity"));` (`:37`).
  Because it derives from `SlotsIconBase`, a `modded class SlotsIcon` does not reach these icons.

- **`ConstructionToolInfoIcon: ConstructionInfoIcon`** (`constructioninfoicons.c:52`; base
  `ConstructionInfoIcon : GenericIconBase` at `:8`): maps each `EConstructionTools` bit
  (`4_world/classes/basebuilding/constructionconstants.c:14-26`) to a `dayz_inventory` image (`:66-100`):
  - `TOOL_MOUNT_WIRE` → `plierstools`
  - `TOOL_NAIL`, `TOOL_MOUNT_LOG` → `hammertools`
  - `TOOL_DIG_BASE` → `shoveltools`
  - `TOOL_HEAVY` → `sladgehammertools`
  - `TOOL_EXCAVATION` → `pickaxetools`
  - `TOOL_HANDS` → `hands`
  - `TOOL_BRICKLAYING` → `bricklayingtools`
  - anything else (e.g. `TOOL_WOODWORK`) → `missing`
  All of these images except `hands`/`missing` are new in the 1.30 `dayz_inventory` imageset.
  *(Corrected 2026-10-15: the Exp text used non-existent names `TOOL_HAMMER`, `TOOL_WOODHAMMER`,
  `TOOL_SHOVEL`, `TOOL_TROWEL`.)*

- **`ConstructionMaterialInfoIcon: ConstructionInfoIcon`** (`constructioninfoicons.c:161`):
  reads `m_Part.GetMaterialsData()` (`:223`) and writes the amounts as RichText (`:347`, `:377`):
  `RichTextWidget.Cast(m_QuantityItem).SetText(string.Format("<color hex=\"%1\">%2</color><color hex=\"%3\">/%4</color>", ...))`

---

## 5. Extension & Modding Patterns

Derived from the 1.30 source above; **not tested in game**. To attach a custom information panel to
the cursor:
1. Subclass `TargetActionInfoPanelBase`; override `InitGridStructure()` (call `super` first, it
   creates `m_GridsArray`) to add your `ActionInfoGridBase` handlers on named grid widgets, and
   `SetLayoutName()` if you ship your own spacer layout (`actioninfopanels.c:29-32` shows the vanilla one).
2. In `modded class ActionTargetsCursor`, `override void InitActionInfoPanels()` → `super.InitActionInfoPanels();`
   then `CreateActionInfoPanel(MyInfoPanel, ACTION_ORDER_CONTINUOUS)` (or another `ACTION_ORDER_*`
   category). Call `super` first: it re-creates `m_TargetActionInfoPanelMap` (`:154-155`), so panels
   registered before it are lost.
3. In your action, create the payload in the constructor — `m_InfoData = new MyActionInfoData();`
   (an `ActionInfoDataBase` subclass, `actionpartinfo.c:209`), as `ActionBuildPart` does
   (`actionbuildpart.c:76`). The base `IsTargetInfoAction()` / `GetClientActionInfo()` then work
   without overrides (`actionbase.c:277-285`); fill `m_InfoContextMask` and your fields client-side.
4. Return `false` from `CanBeStarted()` for an info-only action: the prompt is then dimmed (§2.3) and
   input orders it last (`4_world/classes/useractionscomponent/actioninput.c:470`).
