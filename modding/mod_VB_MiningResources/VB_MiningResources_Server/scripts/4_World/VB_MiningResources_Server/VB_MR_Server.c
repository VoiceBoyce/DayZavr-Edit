// VB_MiningResources_Server: результаты добычи и промывки, пресс, плавильная чаша, хранение прогресса.

// ---------------------------------------------------------------- общие функции
// Найденное кладётся в предмет (сковороду, станок), а если места нет — к ногам
ItemBase VB_MR_GiveOrDrop(EntityAI container, vector dropPos, string type, int quantity)
{
	ItemBase item;
	if (container)
		item = ItemBase.Cast(container.GetInventory().CreateInInventory(type));
	if (!item)
		item = ItemBase.Cast(GetGame().CreateObjectEx(type, dropPos, ECE_PLACE_ON_SURFACE));
	if (item && quantity > 0)
		item.SetQuantity(quantity);
	return item;
}

// Забрать порцию из стопки: остаток остаётся лежать.
// DeleteSafe: горячую чашу можно нести, удаление в инвентаре игрока должно идти через juncture.
void VB_MR_TakePortion(ItemBase item, int portion)
{
	if (item.GetQuantity() <= portion)
		item.DeleteSafe();
	else
		item.AddQuantity(-portion);
}

// Есть ли рядом живой объект класса. «Живой» спрашивается по имени метода VB_IsDead (у рощ Древнего корня):
// так мод не зависит от мода Корня; у объекта без такого метода вызов ничего не меняет — он считается живым.
bool VB_MR_IsNear(vector pos, string className, float radius)
{
	if (className == "" || radius <= 0)
		return false;

	array<Object> objects = new array<Object>;
	GetGame().GetObjectsAtPosition3D(pos, radius, objects, null);
	foreach (Object obj : objects)
	{
		if (!obj || !obj.IsKindOf(className))
			continue;

		bool dead = false;
		g_Game.GameScript.CallFunction(obj, "VB_IsDead", dead, 0);
		if (!dead)
			return true;
	}
	return false;
}

// ---------------------------------------------------------------- хранение прогресса станков
// Прогресс пресса и чаши переживает рестарт. Хранится в отдельном файле по постоянному ID станка,
// а не в сохранении мира: старые BRDK_* записаны без этих данных, менять формат сохранения нельзя.
class VB_MR_ProgressEntry
{
	string Key;
	string Item;		// какой предмет перерабатывался (другой — прогресс не подходит)
	float Done;			// сколько секунд уже прошло
	bool Working;		// пресс был запущен (у чаши не важно — она идёт по температуре)
	int Missed;			// сколько запусков сервера станок не появлялся (чаша у игрока не в сети, станок уничтожен)
}

class VB_MR_ProgressFile
{
	ref array<ref VB_MR_ProgressEntry> Entries = new array<ref VB_MR_ProgressEntry>;
}

class VB_MR_Progress
{
	protected static const string FILE = "$profile:VB_Mods\\VB_MiningResources\\progress.json";
	// столько запусков подряд запись ждёт свой станок (чаша в рюкзаке у игрока, который долго не заходит)
	protected static const int MAX_MISSED = 30;
	protected static ref VB_MR_Progress s_Instance;

	protected ref map<string, ref VB_MR_ProgressEntry> m_All = new map<string, ref VB_MR_ProgressEntry>;
	protected bool m_Dirty;
	protected bool m_Timer;

	protected static VB_MR_Progress Inst()
	{
		if (!s_Instance)
		{
			s_Instance = new VB_MR_Progress();
			s_Instance.Load();
		}
		return s_Instance;
	}

	protected void Load()
	{
		if (!FileExist(FILE))
			return;

		VB_MR_ProgressFile file = new VB_MR_ProgressFile();
		string error;
		if (!JsonFileLoader<VB_MR_ProgressFile>.LoadFile(FILE, file, error) || !file || !file.Entries)
		{
			Print("[VB_MiningResources] Не удалось прочитать прогресс станков: " + error);
			return;
		}
		foreach (VB_MR_ProgressEntry entry : file.Entries)
		{
			if (!entry || entry.Key == "")
				continue;
			// новый запуск: пока станок не отозвался, запись считается пропустившей его
			entry.Missed++;
			if (entry.Missed <= MAX_MISSED)
				m_All.Set(entry.Key, entry);
		}
		// счётчики пропусков должны попасть в файл, даже если в этом запуске станки не работали
		if (file.Entries.Count() > 0)
			MarkDirty();
	}

	static string Key(EntityAI station)
	{
		int b1, b2, b3, b4;
		station.GetPersistentID(b1, b2, b3, b4);
		if (b1 == 0 && b2 == 0 && b3 == 0 && b4 == 0)
			return "";
		return string.Format("%1_%2_%3_%4", b1, b2, b3, b4);
	}

	static VB_MR_ProgressEntry Get(EntityAI station)
	{
		VB_MR_Progress inst = Inst();
		string key = Key(station);
		if (key == "")
			return null;

		VB_MR_ProgressEntry entry = inst.m_All.Get(key);
		if (entry && entry.Missed != 0)
		{
			entry.Missed = 0;
			inst.MarkDirty();
		}
		return entry;
	}

	static void Set(EntityAI station, string item, float done, bool working)
	{
		VB_MR_Progress inst = Inst();
		string key = Key(station);
		if (key == "")
			return;

		VB_MR_ProgressEntry entry = inst.m_All.Get(key);
		if (!entry)
		{
			entry = new VB_MR_ProgressEntry();
			entry.Key = key;
			inst.m_All.Set(key, entry);
		}
		entry.Item = item;
		entry.Done = done;
		entry.Working = working;
		entry.Missed = 0;
		inst.MarkDirty();
	}

	static void Remove(EntityAI station)
	{
		VB_MR_Progress inst = Inst();
		string key = Key(station);
		if (key == "" || !inst.m_All.Contains(key))
			return;

		inst.m_All.Remove(key);
		inst.MarkDirty();
	}

	protected void MarkDirty()
	{
		m_Dirty = true;
		if (!m_Timer)
		{
			m_Timer = true;
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.Write, 60000, true);
		}
	}

	// В файл попадают все записи: станок, который в этом запуске не появился (чаша у игрока не в сети),
	// ждёт MAX_MISSED запусков, потом запись отпадает сама
	protected void Write()
	{
		if (!m_Dirty)
			return;

		VB_MR_ProgressFile file = new VB_MR_ProgressFile();
		foreach (string key, VB_MR_ProgressEntry entry : m_All)
			file.Entries.Insert(entry);

		string error;
		if (JsonFileLoader<VB_MR_ProgressFile>.SaveFile(FILE, file, error))
			m_Dirty = false;
		else
			Print("[VB_MiningResources] Не удалось записать прогресс станков: " + error);
	}

	static void Flush()
	{
		if (s_Instance)
			s_Instance.Write();
	}
}

// ---------------------------------------------------------------- добыча
modded class VB_MR_ActionMine
{
	override void OnFinishProgressServer(ActionData action_data)
	{
		// жилу уже выкопали в этом кадре (удаление отложено) — второй раз добыча не выдаётся
		VB_MR_Vein_Base vein = VB_MR_Vein_Base.Cast(action_data.m_Target.GetObject());
		if (!vein || vein.IsSetForDeletion())
			return;

		VB_MR_VeinData data = VB_MR_Settings.Get().FindVein(vein);
		if (!data)
			return;

		// Базовый метод пустой, но на него вешаются другие моды: квесты INT_QUESTS (цель ACTION)
		// и планшет. Без super добыча им не видна (баг №59). Здесь, после проверок: только реальная добыча.
		super.OnFinishProgressServer(action_data);

		float chance = data.OreChance;
		float damage = data.ToolDamageOnUse;
		float rareChance = data.RareChance;
		int count = Math.RandomIntInclusive(data.MinCount, data.MaxCount);

		VB_MR_ToolData tool = VB_MR_Shared.FindTool(action_data.m_MainItem);
		if (tool)
		{
			chance += tool.OreChanceAdd;
			damage *= tool.ToolDamageMul;
			rareChance += tool.RareChanceAdd;
			if (tool.ExtraRollsMax > 0 || tool.ExtraRollsMin > 0)
			{
				int extraMax = tool.ExtraRollsMax;
				if (extraMax < tool.ExtraRollsMin)
					extraMax = tool.ExtraRollsMin;
				int extra = Math.RandomIntInclusive(tool.ExtraRollsMin, extraMax);
				count += extra;
			}
		}

		vector pos = action_data.m_Player.GetPosition();
		for (int i = 0; i < count; i++)
		{
			if (Math.RandomFloat(0, 100) < chance)
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

		// редкая находка жилы — один бросок на всю добычу с жилы
		if (data.RareItem != "" && Math.RandomFloat(0, 100) < rareChance)
			GetGame().CreateObjectEx(data.RareItem, pos, ECE_PLACE_ON_SURFACE);

		// находка инструмента (Велесова кирка — «Дар Велеса»: янтарь) — тоже один бросок на жилу, к ногам
		if (tool && tool.BonusItem != "" && Math.RandomFloat(0, 100) < tool.BonusChance)
			GetGame().CreateObjectEx(tool.BonusItem, pos, ECE_PLACE_ON_SURFACE);

		action_data.m_MainItem.AddHealth(-damage);
		vein.Delete();
	}
}

// ---------------------------------------------------------------- промывка
// Как в старом моде: на каждый бросок выбирается случайный вид песка, потом бросается его шанс
// (в зоне «золотоносной реки» он умножается). Найденное — в сковороду, если места нет — к ногам.
modded class VB_MR_ActionPan
{
	override void OnFinishProgressServer(ActionData action_data)
	{
		// для квестов INT_QUESTS и планшета (см. добычу выше, баг №59)
		super.OnFinishProgressServer(action_data);

		VB_MR_Settings settings = VB_MR_Settings.Get();
		ItemBase pan = action_data.m_MainItem;
		vector pos = action_data.m_Player.GetPosition();

		if (settings.SandDataList && settings.SandDataList.Count() > 0)
		{
			int count = Math.RandomIntInclusive(settings.MinCountInRiver, settings.MaxCountInRiver);
			for (int i = 0; i < count; i++)
			{
				VB_MR_SandData sand = settings.SandDataList.GetRandomElement();
				float chance = sand.SandChance * settings.PanZoneMul(pos, sand.SandName);
				if (chance > Math.RandomFloat(0, 100))
				{
					int quantity = Math.RandomIntInclusive(sand.MinQuant, sand.MaxQuant);
					VB_MR_GiveOrDrop(pan, pos, sand.SandName, quantity);
				}
				else if (settings.BadRiverBounty && settings.BadRiverBounty.Count() > 0)
				{
					VB_MR_GiveOrDrop(pan, pos, settings.BadRiverBounty.GetRandomElement(), 0);
				}
			}
		}

		// янтарь у живой рощи Древнего корня (без мода Корня класса нет — ничего не происходит)
		if (settings.AmberItem != "" && settings.AmberChance > 0 && Math.RandomFloat(0, 100) < settings.AmberChance)
		{
			if (VB_MR_IsNear(pos, settings.AmberSourceClass, settings.AmberRadius))
				VB_MR_GiveOrDrop(pan, pos, settings.AmberItem, 0);
		}
	}
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
	protected string m_VB_MR_Item;		// какой предмет прессуется
	protected float m_VB_MR_Period;

	void VB_MR_StartPressing()
	{
		if (m_VB_MR_State == VB_MR_STATION_WORK)
			return;

		ItemBase stone = VB_MR_GetStone();
		VB_MR_RecipeInfo recipe = VB_MR_Shared.FindRecipe(stone);
		if (!stone || stone.IsRuined() || !recipe || stone.GetQuantity() < recipe.Portion)
			return;

		if (m_VB_MR_Item != stone.GetType())
			m_VB_MR_Done = 0;
		m_VB_MR_Item = stone.GetType();
		m_VB_MR_Total = recipe.Time;
		m_VB_MR_State = VB_MR_STATION_WORK;
		m_VB_MR_Period = VB_MR_Settings.Get().PressUpdatePeriod;
		if (m_VB_MR_Period <= 0)
			m_VB_MR_Period = 5;
		SetSynchDirty();

		int delay = m_VB_MR_Period * 1000;
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(VB_MR_PressTick, delay, true);
		VB_MR_Progress.Set(this, m_VB_MR_Item, m_VB_MR_Done, true);
	}

	void VB_MR_PressTick()
	{
		ItemBase stone = VB_MR_GetStone();
		VB_MR_RecipeInfo recipe = VB_MR_Shared.FindRecipe(stone);
		if (!stone || stone.IsRuined() || !recipe || stone.GetType() != m_VB_MR_Item || stone.GetQuantity() < recipe.Portion)
		{
			VB_MR_StopPressing();
			return;
		}

		m_VB_MR_Done += m_VB_MR_Period;
		if (m_VB_MR_Done >= m_VB_MR_Total)
		{
			// хранилище полное — рядом с прессом, а не в точку его центра (внутри модели не видно)
			vector dropPos = GetPosition() + GetDirection() * 1.2;
			VB_MR_GiveOrDrop(this, dropPos, recipe.Result, 0);
			VB_MR_TakePortion(stone, recipe.Portion);
			VB_MR_StopPressing();
			return;
		}

		SetSynchDirty();
		VB_MR_Progress.Set(this, m_VB_MR_Item, m_VB_MR_Done, true);
	}

	protected void VB_MR_StopPressing()
	{
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(VB_MR_PressTick);
		m_VB_MR_State = VB_MR_STATION_IDLE;
		m_VB_MR_Done = 0;
		m_VB_MR_Item = "";
		SetSynchDirty();
		VB_MR_Progress.Remove(this);
	}

	// после рестарта: вложения к этому моменту ещё грузятся, поэтому с задержкой
	override void AfterStoreLoad()
	{
		super.AfterStoreLoad();
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(VB_MR_Restore, 3000, false);
	}

	protected void VB_MR_Restore()
	{
		VB_MR_ProgressEntry entry = VB_MR_Progress.Get(this);
		if (!entry)
			return;

		ItemBase stone = VB_MR_GetStone();
		if (!stone || stone.GetType() != entry.Item)
		{
			VB_MR_Progress.Remove(this);
			return;
		}

		m_VB_MR_Item = entry.Item;
		m_VB_MR_Done = entry.Done;
		if (entry.Working)
			VB_MR_StartPressing();
		else
			SetSynchDirty();
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);
		if (GetGame())
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(VB_MR_PressTick);
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(VB_MR_Restore);
		}
	}
}

// ---------------------------------------------------------------- плавильная чаша
// Чаша в скриптах — Cauldron (см. клиентскую часть), поэтому всё ниже работает только при m_VB_MR_IsMeltingBowl.
// Нагретая до MeltTempTreshold чаша раз в MeltUpdatePeriod добавляет время порции из слота металла.
// Остыла — плавка на паузе (прогресс сохраняется); вынули металл — прогресс сгорает.
modded class Cauldron
{
	protected bool m_VB_MR_Hot;
	protected float m_VB_MR_Period;
	protected string m_VB_MR_Item;

	override void SetTemperatureEx(TemperatureData data)
	{
		super.SetTemperatureEx(data);
		if (!m_VB_MR_IsMeltingBowl)
			return;

		VB_MR_Settings settings = VB_MR_Settings.Get();
		bool hot = GetTemperature() >= settings.MeltTempTreshold;
		if (hot && !m_VB_MR_Hot)
		{
			m_VB_MR_Hot = true;
			m_VB_MR_Period = settings.MeltUpdatePeriod;
			if (m_VB_MR_Period <= 0)
				m_VB_MR_Period = 30;
			int delay = m_VB_MR_Period * 1000;
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(VB_MR_MeltTick, delay, true);
		}
		else if (!hot && m_VB_MR_Hot)
		{
			// И10: флаг сбрасывается — после нового нагрева плавка продолжится
			m_VB_MR_Hot = false;
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(VB_MR_MeltTick);
			if (m_VB_MR_State != VB_MR_STATION_IDLE)
			{
				m_VB_MR_State = VB_MR_STATION_IDLE;
				SetSynchDirty();
			}
		}
	}

	void VB_MR_MeltTick()
	{
		ItemBase metal = ItemBase.Cast(FindAttachmentBySlotName("BRDK_METALS"));
		VB_MR_RecipeInfo recipe = VB_MR_Shared.FindRecipe(metal);
		if (!metal || metal.IsRuined() || !recipe || metal.GetQuantity() < recipe.Portion)
		{
			if (m_VB_MR_State != VB_MR_STATION_IDLE)
			{
				m_VB_MR_State = VB_MR_STATION_IDLE;
				SetSynchDirty();
			}
			return;
		}

		if (metal.GetType() != m_VB_MR_Item)
		{
			m_VB_MR_Item = metal.GetType();
			m_VB_MR_Done = 0;
		}
		m_VB_MR_State = VB_MR_STATION_WORK;
		m_VB_MR_Total = recipe.Time;
		m_VB_MR_Done += m_VB_MR_Period;

		if (m_VB_MR_Done >= m_VB_MR_Total)
		{
			m_VB_MR_Done = 0;
			VB_MR_GiveOrDrop(this, GetPosition(), recipe.Result, 0);
			VB_MR_TakePortion(metal, recipe.Portion);
		}

		SetSynchDirty();
		VB_MR_Progress.Set(this, m_VB_MR_Item, m_VB_MR_Done, false);
	}

	// вынули металл (или порция кончилась) — накопленное время сгорает
	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);
		if (!m_VB_MR_IsMeltingBowl || slot_name != "BRDK_METALS")
			return;

		// чашу удаляют целиком (игрок вышел с ней, остановка сервера): вложения снимаются первыми,
		// это не «вынули металл» — прогресс остаётся в файле (Object.IsPendingDeletion)
		if (IsSetForDeletion() || !GetGame())
			return;

		m_VB_MR_Done = 0;
		m_VB_MR_Total = 0;
		m_VB_MR_Item = "";
		m_VB_MR_State = VB_MR_STATION_IDLE;
		SetSynchDirty();
		VB_MR_Progress.Remove(this);
	}

	override void AfterStoreLoad()
	{
		super.AfterStoreLoad();
		if (m_VB_MR_IsMeltingBowl)
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(VB_MR_Restore, 3000, false);
	}

	protected void VB_MR_Restore()
	{
		VB_MR_ProgressEntry entry = VB_MR_Progress.Get(this);
		if (!entry)
			return;

		ItemBase metal = ItemBase.Cast(FindAttachmentBySlotName("BRDK_METALS"));
		if (!metal || metal.GetType() != entry.Item)
		{
			VB_MR_Progress.Remove(this);
			return;
		}
		m_VB_MR_Item = entry.Item;
		m_VB_MR_Done = entry.Done;
		SetSynchDirty();
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);
		if (m_VB_MR_IsMeltingBowl && GetGame())
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(VB_MR_MeltTick);
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(VB_MR_Restore);
		}
	}
}

// Чаша для костра — котёл, и ванильная готовка раз в 3 с снимала здоровье со всего в её хранилище
// (песок, самородки, слитки — не еда), пока через ~1,5 ч не удаляла. Нагрев чаше даёт сам костёр
// (FireplaceBase.BurnItemsInFireplace), готовка ей не нужна.
modded class Cooking
{
	override int CookWithEquipment(ItemBase cooking_equipment, float cooking_time_coef = 1)
	{
		Cauldron bowl = Cauldron.Cast(cooking_equipment);
		if (bowl && bowl.VB_MR_IsMeltingBowl())
			return 0;

		return super.CookWithEquipment(cooking_equipment, cooking_time_coef);
	}
}
