// Янтарь (ТЗ_сервер_18.md п. 2б.5) — сервер: что выпадает при «Расколоть» (действие — клиентский VB_AR_Amber.c).
//   Окаменевшая сердцевина → 2–3 куска янтаря, каждый с шансом 25 % — с вкраплением.
//   Янтарь с вкраплением → кристалл MSFZ (60 %) или одна из шести базовых рун MSFZ (40 %).
//   Высших рун нет: их дерево крафтится в DRL 18 из базовых, янтарь его не обходит.
// Всё падает к ногам, инструмент изнашивается. Списки классов — с cfgrandompresets 18 (Era_Zombies).

class VB_AR_AmberServer
{
	static const string AMBER = "VB_AR_Amber";
	static const string AMBER_INCL = "VB_AR_AmberInclusion";
	static const int CORE_AMBER_MIN = 2;
	static const int CORE_AMBER_MAX = 3;
	static const float INCL_CHANCE = 0.25;		// кусок из сердцевины — с вкраплением
	static const float CRYSTAL_CHANCE = 0.6;	// во вкраплении кристалл (иначе базовая руна)
	static const float TOOL_DAMAGE = 4;			// износ инструмента за один раскол, ед. здоровья

	static string VB_RandomInclusion()
	{
		if (Math.RandomFloat01() < CRYSTAL_CHANCE)
		{
			TStringArray crystals = {"MSFZ_Crystal_Blue", "MSFZ_Crystal_Pink", "MSFZ_Crystal_Gold", "MSFZ_Crystal_Brass",
				"MSFZ_Crystal_Cooper", "MSFZ_Crystal_Iron", "MSFZ_Crystal_Silver", "MSFZ_Crystal_Amethyst",
				"MSFZ_Crystal_Mithril", "MSFZ_Crystal_Obsidian", "MSFZ_Crystal_Stone", "MSFZ_Crystal_Damascus",
				"MSFZ_Crystal_Pyrite"};
			return crystals.GetRandomElement();
		}
		TStringArray runes = {"MSFZ_RuneStone_FireSmall", "MSFZ_RuneStone_IceSmall", "MSFZ_RuneStone_StormSmall",
			"MSFZ_RuneStone_Fire", "MSFZ_RuneStone_Ice", "MSFZ_RuneStone_Storm"};
		return runes.GetRandomElement();
	}

	static void VB_Split(PlayerBase player, ItemBase target, ItemBase tool)
	{
		if (!player || !target)
			return;
		string who = VB_AR_ItemsServer.VB_Who(player);
		EntityAI drop;
		string got = "";

		if (target.IsKindOf(AMBER_INCL))
		{
			target.DeleteSafe();
			string incl = VB_RandomInclusion();
			drop = VB_AR_ItemsServer.VB_DropAtFeet(player, incl);
			if (!drop)
			{
				// класса MSFZ нет (мод не загружен) — хотя бы янтарь назад, чтобы не пропало
				drop = VB_AR_ItemsServer.VB_DropAtFeet(player, AMBER);
				incl = AMBER + " (нет класса " + incl + ")";
			}
			if (drop)
				NotificationSystem.SendNotificationToPlayerExtended(player, 6, "Янтарь раскололся", "Внутри застыло: " + drop.GetDisplayName() + ".");
			Print("[VB_AncientRoot] расколол янтарь с вкраплением: " + who + " получил " + incl);
		}
		else
		{
			target.DeleteSafe();
			int n = Math.RandomIntInclusive(CORE_AMBER_MIN, CORE_AMBER_MAX);
			int incls = 0;
			string type;
			for (int i = 0; i < n; i++)
			{
				type = AMBER;
				if (Math.RandomFloat01() < INCL_CHANCE)
				{
					type = AMBER_INCL;
					incls++;
				}
				drop = VB_AR_ItemsServer.VB_DropAtFeet(player, type);
				if (drop)
					got += type + " ";
			}
			string detail = "Под коркой — " + n + " куска янтаря.";
			if (incls > 0)
				detail += " В одном что-то застыло.";
			NotificationSystem.SendNotificationToPlayerExtended(player, 6, "Сердцевина раскололась", detail);
			Print("[VB_AncientRoot] расколол окаменевшую сердцевину: " + who + " получил " + got);
		}

		if (tool)
			MiscGameplayFunctions.DealAbsoluteDmg(tool, TOOL_DAMAGE);
	}
}

modded class VB_ActionSplitAmber
{
	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);
		if (!action_data.m_Target)
			return;
		ItemBase target = ItemBase.Cast(action_data.m_Target.GetObject());
		if (!VB_AR_AmberSplit.VB_CanSplit(target))
			return;
		VB_AR_AmberServer.VB_Split(action_data.m_Player, target, action_data.m_MainItem);
	}
}
