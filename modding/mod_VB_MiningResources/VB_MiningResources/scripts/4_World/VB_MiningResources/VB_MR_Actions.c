// VB_MiningResources: действия игрока. Результаты действий считает серверная часть
// (VB_MiningResources_Server), здесь условия, время, анимации и подсказки.

modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(VB_MR_ActionMine);
		actions.Insert(VB_MR_ActionPan);
		actions.Insert(VB_MR_ActionPress);
		actions.Insert(VB_MR_ActionDismantlePress);
		actions.Insert(VB_MR_ActionStationInfo);
	}
}

// ---------------------------------------------------------------- добыча из жилы
class VB_MR_ActionMineCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		float time = 10;
		VB_MR_VeinInfo vein = VB_MR_Shared.FindVein(m_ActionData.m_Target.GetObject());
		if (vein)
			time = vein.MineTime;

		VB_MR_ToolData tool = VB_MR_Shared.FindTool(m_ActionData.m_MainItem);
		if (tool)
			time = time * tool.MineTimeMul;

		m_ActionData.m_ActionComponent = new CAContinuousTime(time);
	}
}

class VB_MR_ActionMine : ActionContinuousBase
{
	void VB_MR_ActionMine()
	{
		m_CallbackClass = VB_MR_ActionMineCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_MINEROCK;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_HIGH;
		m_Text = "#STR_VB_MR_ACT_MINE";
	}

	override void CreateConditionComponents()
	{
		m_ConditionTarget = new CCTCursor(UAMaxDistances.DEFAULT);
		m_ConditionItem = new CCINonRuined;
	}

	// инструмент из списка сервера, жила из настроек и этим инструментом её можно добывать
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (player.GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
			return false;

		VB_MR_Vein_Base vein = VB_MR_Vein_Base.Cast(target.GetObject());
		// жила уже выкопана (удаление отложено на кадр) — второй раз не добывается
		if (!item || !vein || vein.IsSetForDeletion())
			return false;

		if (!VB_MR_Shared.FindVein(vein))
			return false;

		return VB_MR_Shared.ToolAllowsVein(VB_MR_Shared.FindTool(item), vein);
	}
}

// ---------------------------------------------------------------- промывка сковородой
class VB_MR_ActionPanCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		float time = 10;
		if (VB_MR_Shared.s_Data)
			time = VB_MR_Shared.s_Data.PanTime;

		m_ActionData.m_ActionComponent = new CAContinuousTime(time);
	}
}

class VB_MR_ActionPan : ActionContinuousBase
{
	void VB_MR_ActionPan()
	{
		m_CallbackClass = VB_MR_ActionPanCB;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_FullBody = true;
		m_LockTargetOnUse = false;
		m_Text = "#STR_VB_MR_ACT_PAN";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINonRuined;
		// пресная вода рек и прудов (поверхности fresh_water, жидкость LIQUID_FRESHWATER)
		m_ConditionTarget = new CCTLiquid(LIQUID_FRESHWATER, UAMaxDistances.DEFAULT, UAMaxDistances.DEFAULT);
	}

	// только поверхность воды, как в старом моде (CCTLiquid пускает и корыта — объекты-источники);
	// у игрока — когда данные сервера уже пришли, иначе полоса шла бы не то время
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (target.GetObject())
			return false;

		return VB_MR_Shared.s_Data != null;
	}
}

// ---------------------------------------------------------------- пресс: «Спрессовать»
class VB_MR_ActionPress : ActionInteractBase
{
	void VB_MR_ActionPress()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_OPENDOORFW;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_HUDCursorIcon = CursorIcons.CloseDoors;
		m_Text = "#STR_VB_MR_ACT_PRESS";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTCursor(UAMaxDistances.DEFAULT);
	}

	// пресс свободен и в слоте хватает на порцию
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		VB_MR_Press press = VB_MR_Press.Cast(target.GetObject());
		return press && press.VB_MR_CanStart();
	}
}

// ---------------------------------------------------------------- подсказка у пресса и чаши
// Ничего не делает: текст действия показывает, что происходит со станком.
// Текст собирается в ActionCondition на стороне игрока под тот станок, на который он смотрит.
class VB_MR_ActionStationInfo : ActionInteractBase
{
	void VB_MR_ActionStationInfo()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_OPENDOORFW;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_HUDCursorIcon = CursorIcons.Cursor;
		m_Text = "";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTCursor(UAMaxDistances.DEFAULT);
	}

	override bool IsInstant()
	{
		return true;
	}

	// нажатие ничего не делает и на сервер не уходит
	override bool IsLocal()
	{
		return true;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		string text = "";
		Object obj = target.GetObject();

		VB_MR_Press press = VB_MR_Press.Cast(obj);
		if (press)
			text = PressText(press);

		Cauldron bowl = Cauldron.Cast(obj);
		if (bowl && bowl.VB_MR_IsMeltingBowl())
			text = BowlText(bowl);

		m_Text = text;
		return text != "";
	}

	protected string PressText(VB_MR_Press press)
	{
		if (press.VB_MR_GetState() == VB_MR_STATION_WORK)
			return Format2("#STR_VB_MR_INFO_PRESS", Math.Floor(press.VB_MR_GetDone()), Math.Floor(press.VB_MR_GetTotal()));

		// свободный пресс с полной порцией — тогда видна «Спрессовать», подсказка не нужна;
		// испорченная порода не прессуется — «Нужно 5, лежит 5» было бы неправдой
		ItemBase stone = press.VB_MR_GetStone();
		if (!stone || stone.IsRuined() || press.VB_MR_CanStart())
			return "";

		return NeedText(stone);
	}

	protected string BowlText(Cauldron bowl)
	{
		ItemBase metal = ItemBase.Cast(bowl.FindAttachmentBySlotName("BRDK_METALS"));
		if (!metal)
			return "";

		VB_MR_RecipeInfo recipe = VB_MR_Shared.FindRecipe(metal);
		if (!recipe || metal.IsRuined())
			return "";
		if (metal.GetQuantity() < recipe.Portion)
			return NeedText(metal);

		int temperature = Math.Floor(bowl.GetTemperature());
		float meltTemp = 150;
		if (VB_MR_Shared.s_Data)
			meltTemp = VB_MR_Shared.s_Data.MeltTemp;
		if (temperature < meltTemp)
			return Format2("#STR_VB_MR_INFO_COLD", temperature, Math.Floor(meltTemp));

		// время порции — по рецепту металла в слоте (после замены металла синхронное значение ещё старое)
		return Format3("#STR_VB_MR_INFO_MELT", Math.Floor(bowl.VB_MR_GetDone()), Math.Floor(recipe.Time), temperature);
	}

	protected string NeedText(ItemBase item)
	{
		VB_MR_RecipeInfo recipe = VB_MR_Shared.FindRecipe(item);
		if (!recipe)
			return "";
		return Format2("#STR_VB_MR_INFO_NEED", recipe.Portion, Math.Floor(item.GetQuantity()));
	}

	// перевод строки есть только у игрока; на сервере подсказка не нужна
	protected string Format2(string key, int a, int b)
	{
		#ifndef SERVER
		return string.Format(Widget.TranslateString(key), a, b);
		#else
		return key;
		#endif
	}

	protected string Format3(string key, int a, int b, int c)
	{
		#ifndef SERVER
		return string.Format(Widget.TranslateString(key), a, b, c);
		#else
		return key;
		#endif
	}
}

// ---------------------------------------------------------------- пресс: разборка плоскогубцами
class VB_MR_ActionDismantlePressCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.DEFAULT_DECONSTRUCT);
	}
}

class VB_MR_ActionDismantlePress : ActionContinuousBase
{
	void VB_MR_ActionDismantlePress()
	{
		m_CallbackClass = VB_MR_ActionDismantlePressCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_HIGH;
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINonRuined;
		// как у ванильной разборки: по взгляду и рядом (CCTNonRuined на сервере проверял только 20 м, без стен)
		m_ConditionTarget = new CCTCursor(UAMaxDistances.DEFAULT);
	}

	override string GetText()
	{
		return "#STR_VB_MR_ACT_DISMANTLE";
	}

	// разбирается только пустой и неработающий пресс
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		VB_MR_Press press = VB_MR_Press.Cast(target.GetObject());
		return press && press.IsEmpty() && press.VB_MR_GetState() == VB_MR_STATION_IDLE;
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		VB_MR_Press press = VB_MR_Press.Cast(action_data.m_Target.GetObject());
		if (!press)
			return;

		vector pos = action_data.m_Player.GetPosition();
		action_data.m_MainItem.DecreaseHealth(5, false);
		GetGame().CreateObject("VB_MR_Press_Kit", pos);
		press.Delete();
	}
}
