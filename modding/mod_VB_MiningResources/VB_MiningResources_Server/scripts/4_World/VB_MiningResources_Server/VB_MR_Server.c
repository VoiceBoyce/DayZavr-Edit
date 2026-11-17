// VB_MiningResources_Server: результаты добычи и промывки, пресс, плавильная чаша.
// Логика перенесена 1:1 из BRDK_GoldRush_SERVER, без проверки лицензии и внешних запросов.

// ---------------------------------------------------------------- значения, которые сервер шлёт клиенту
modded class VB_MR_Vein_Base
{
	void VB_MR_Vein_Base()
	{
		VB_MR_VeinData data = VB_MR_Settings.Get().FindVein(this);
		if (data)
		{
			m_VB_MR_MineTime = data.MineTime;
			SetSynchDirty();
		}
	}
}

modded class ItemBase
{
	void ItemBase()
	{
		if (VB_MR_Settings.Get().IsMineTool(GetType()))
		{
			m_VB_MR_IsMineTool = true;
			SetSynchDirty();
		}
	}
}

modded class FryingPan
{
	void FryingPan()
	{
		m_VB_MR_PanTime = VB_MR_Settings.Get().GoldInRiverSearchTime;
		SetSynchDirty();
	}
}

// ---------------------------------------------------------------- добыча
modded class VB_MR_ActionMine
{
	override void OnFinishProgressServer(ActionData action_data)
	{
		VB_MR_Vein_Base vein = VB_MR_Vein_Base.Cast(action_data.m_Target.GetObject());
		if (!vein)
			return;

		VB_MR_VeinData data = VB_MR_Settings.Get().FindVein(vein);
		if (!data)
			return;

		vector pos = action_data.m_Player.GetPosition();
		int count = Math.RandomIntInclusive(data.MinCount, data.MaxCount);
		for (int i = 0; i < count; i++)
		{
			if (Math.RandomFloat(0, 100) < data.OreChance)
			{
				ItemBase ore = ItemBase.Cast(GetGame().CreateObjectEx(data.GoodOre, pos, ECE_PLACE_ON_SURFACE));
				if (ore)
				{
					int quantity = Math.RandomIntInclusive(data.MinQuant, data.MaxQuant);
					ore.SetQuantity(quantity);
				}
			}
			else if (data.BadOre != "")
			{
				GetGame().CreateObjectEx(data.BadOre, pos, ECE_PLACE_ON_SURFACE);
			}
		}

		action_data.m_MainItem.AddHealth(-data.ToolDamageOnUse);
		vein.Delete();
	}
}

// ---------------------------------------------------------------- промывка
// Как в старом моде: на каждый бросок выбирается случайный вид песка, потом бросается его шанс;
// результат кладётся в сковороду
modded class VB_MR_ActionPan
{
	override void OnFinishProgressServer(ActionData action_data)
	{
		VB_MR_Settings settings = VB_MR_Settings.Get();
		if (!settings.SandDataList || settings.SandDataList.Count() == 0)
			return;

		int count = Math.RandomIntInclusive(settings.MinCountInRiver, settings.MaxCountInRiver);
		for (int i = 0; i < count; i++)
		{
			VB_MR_SandData sand = settings.SandDataList.GetRandomElement();
			if (sand.SandChance > Math.RandomFloat(0, 100))
			{
				ItemBase found = ItemBase.Cast(action_data.m_MainItem.GetInventory().CreateInInventory(sand.SandName));
				if (found)
				{
					int quantity = Math.RandomIntInclusive(sand.MinQuant, sand.MaxQuant);
					found.SetQuantity(quantity);
				}
			}
			else if (settings.BadRiverBounty && settings.BadRiverBounty.Count() > 0)
			{
				action_data.m_MainItem.GetInventory().CreateInInventory(settings.BadRiverBounty.GetRandomElement());
			}
		}
	}
}

// ---------------------------------------------------------------- что во что перерабатывается
modded class VB_MR_Nugget_Base
{
	float m_VB_MR_ProcessTime;		// сколько плавить, с
	string m_VB_MR_ProcessResult;	// что получится
	float m_VB_MR_MeltDone;			// сколько уже плавится

	void VB_MR_Nugget_Base()
	{
		VB_MR_OreData data = VB_MR_Settings.Get().FindOre(this);
		if (data)
		{
			m_VB_MR_ProcessTime = data.MeltPressTime;
			m_VB_MR_ProcessResult = data.IngotName;
		}
	}

	// время копится только у полного стака
	void VB_MR_AddMeltTime(float seconds)
	{
		if (GetQuantity() == GetQuantityMax())
			m_VB_MR_MeltDone += seconds;
	}

	void VB_MR_ResetMeltTime()
	{
		m_VB_MR_MeltDone = 0;
	}
}

modded class VB_MR_Sand_Base
{
	float m_VB_MR_ProcessTime;
	string m_VB_MR_ProcessResult;
	float m_VB_MR_MeltDone;

	void VB_MR_Sand_Base()
	{
		VB_MR_SandData data = VB_MR_Settings.Get().FindSand(this);
		if (data)
		{
			m_VB_MR_ProcessTime = data.MeltPressTime;
			m_VB_MR_ProcessResult = data.OreName;
		}
	}

	void VB_MR_AddMeltTime(float seconds)
	{
		if (GetQuantity() == GetQuantityMax())
			m_VB_MR_MeltDone += seconds;
	}

	void VB_MR_ResetMeltTime()
	{
		m_VB_MR_MeltDone = 0;
	}
}

modded class VB_MR_StoneNugget_Base
{
	float m_VB_MR_ProcessTime;
	string m_VB_MR_ProcessResult;

	void VB_MR_StoneNugget_Base()
	{
		VB_MR_OreData data = VB_MR_Settings.Get().FindOre(this);
		if (data)
		{
			m_VB_MR_ProcessTime = data.MeltPressTime;
			m_VB_MR_ProcessResult = data.IngotName;
		}
	}
}

modded class VB_MR_StoneSand_Base
{
	float m_VB_MR_ProcessTime;
	string m_VB_MR_ProcessResult;

	void VB_MR_StoneSand_Base()
	{
		VB_MR_SandData data = VB_MR_Settings.Get().FindSand(this);
		if (data)
		{
			m_VB_MR_ProcessTime = data.MeltPressTime;
			m_VB_MR_ProcessResult = data.OreName;
		}
	}
}

// Результат переработки: в хранилище станка, если места нет — на землю рядом
ItemBase VB_MR_CreateResult(EntityAI station, string type)
{
	ItemBase result = ItemBase.Cast(station.GetInventory().CreateInInventory(type));
	if (!result)
		result = ItemBase.Cast(GetGame().CreateObjectEx(type, station.GetPosition(), ECE_PLACE_ON_SURFACE));
	return result;
}

// ---------------------------------------------------------------- пресс
modded class VB_MR_ActionPress
{
	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);

		VB_MR_Press press = VB_MR_Press.Cast(action_data.m_Target.GetObject());
		if (press)
			press.VB_MR_StartPressing();
	}
}

modded class VB_MR_Press
{
	void VB_MR_StartPressing()
	{
		float time = 0;
		string result;

		VB_MR_StoneSand_Base sand = VB_MR_StoneSand_Base.Cast(FindAttachmentBySlotName("BRDK_STONES"));
		if (sand)
		{
			result = sand.m_VB_MR_ProcessResult;
			time = sand.m_VB_MR_ProcessTime;
		}
		VB_MR_StoneNugget_Base nugget = VB_MR_StoneNugget_Base.Cast(FindAttachmentBySlotName("BRDK_STONES"));
		if (nugget)
		{
			result = nugget.m_VB_MR_ProcessResult;
			time = nugget.m_VB_MR_ProcessTime;
		}

		m_VB_MR_IsPressing = true;
		SetSynchDirty();

		int delay = time * 1000;
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(VB_MR_FinishPressing, delay, false, result);
	}

	void VB_MR_FinishPressing(string result)
	{
		ItemBase stone = ItemBase.Cast(FindAttachmentBySlotName("BRDK_STONES"));
		if (stone && result != "")
		{
			stone.Delete();
			VB_MR_CreateResult(this, result);
		}

		m_VB_MR_IsPressing = false;
		SetSynchDirty();
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);
		if (GetGame())
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(VB_MR_FinishPressing);
	}
}

// ---------------------------------------------------------------- плавильная чаша
// Нагретая до MeltTempTreshold чаша раз в MeltUpdatePeriod добавляет время полному стаку в слоте металла.
// Набралось MeltPressTime — стак превращается в результат.
// Чаша в скриптах — Cauldron (см. клиентскую часть), поэтому всё ниже работает только при m_VB_MR_IsMeltingBowl.
modded class Cauldron
{
	protected bool m_VB_MR_MeltStarted;
	protected float m_VB_MR_MeltPeriod;

	override void SetTemperatureEx(TemperatureData data)
	{
		super.SetTemperatureEx(data);
		if (!m_VB_MR_IsMeltingBowl)
			return;

		VB_MR_Settings settings = VB_MR_Settings.Get();
		float temperature = GetTemperature();
		if (!m_VB_MR_MeltStarted && temperature >= settings.MeltTempTreshold)
		{
			m_VB_MR_MeltStarted = true;
			m_VB_MR_MeltPeriod = settings.MeltUpdatePeriod;
			int delay = m_VB_MR_MeltPeriod * 1000;
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(VB_MR_MeltTick, delay, true);
		}
		else if (m_VB_MR_MeltStarted && temperature < settings.MeltTempTreshold)
		{
			// Как в старом моде: флаг m_VB_MR_MeltStarted здесь не сбрасывается, поэтому после остывания
			// чаша снова плавит только после рестарта (ошибка старого мода, оставлена до этапа исправлений)
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(VB_MR_MeltTick);
			VB_MR_ResetAttached();
		}
	}

	protected void VB_MR_ResetAttached()
	{
		VB_MR_Sand_Base sand = VB_MR_Sand_Base.Cast(FindAttachmentBySlotName("BRDK_METALS"));
		if (sand)
			sand.VB_MR_ResetMeltTime();

		VB_MR_Nugget_Base nugget = VB_MR_Nugget_Base.Cast(FindAttachmentBySlotName("BRDK_METALS"));
		if (nugget)
			nugget.VB_MR_ResetMeltTime();
	}

	void VB_MR_MeltTick()
	{
		VB_MR_Sand_Base sand = VB_MR_Sand_Base.Cast(FindAttachmentBySlotName("BRDK_METALS"));
		if (sand)
		{
			sand.VB_MR_AddMeltTime(m_VB_MR_MeltPeriod);
			if (sand.m_VB_MR_ProcessTime <= sand.m_VB_MR_MeltDone && sand.m_VB_MR_ProcessResult != "")
			{
				string sandResult = sand.m_VB_MR_ProcessResult;
				sand.Delete();
				VB_MR_CreateResult(this, sandResult);
			}
		}

		VB_MR_Nugget_Base nugget = VB_MR_Nugget_Base.Cast(FindAttachmentBySlotName("BRDK_METALS"));
		if (nugget)
		{
			nugget.VB_MR_AddMeltTime(m_VB_MR_MeltPeriod);
			if (nugget.m_VB_MR_ProcessTime <= nugget.m_VB_MR_MeltDone && nugget.m_VB_MR_ProcessResult != "")
			{
				string nuggetResult = nugget.m_VB_MR_ProcessResult;
				nugget.Delete();
				VB_MR_CreateResult(this, nuggetResult);
			}
		}
	}

	// вынули стак из слота — накопленное время сгорает
	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);
		if (!m_VB_MR_IsMeltingBowl)
			return;

		VB_MR_Sand_Base sand = VB_MR_Sand_Base.Cast(item);
		if (sand)
			sand.VB_MR_ResetMeltTime();

		VB_MR_Nugget_Base nugget = VB_MR_Nugget_Base.Cast(item);
		if (nugget)
			nugget.VB_MR_ResetMeltTime();
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);
		if (m_VB_MR_MeltStarted && GetGame())
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(VB_MR_MeltTick);
	}
}
