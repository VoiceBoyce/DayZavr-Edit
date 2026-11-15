// «Древний корень» (сервер 18, ТЗ_сервер_18.md п. 2б.1): стражи рощи вместо зомби Изнанки — только сервер.
// Классы стражей (VB_RootGuard_*, vbRootGuard = 1) — в config.cpp клиента VB_AncientRoot, текстуры и материалы
// делает tools/gen_ar_guards.py.
//
// Волны сердцевины по нарастающей (2б.10, цифры черновые, подбираются на стенде):
//   1-я — 2–3 лесовых волка; 2-я — 3–4 волка и Терновая заноза; 3-я — 2–3 волка и две занозы,
//   в древней роще (ступень 3, рост рощ — шаг 5) ещё Хранитель рощи.
// Разделка стража: ванильный лут по конфигу (шкура, мясо, трофеи MSFZ) + шанс на отросток и пыльцу.

class VB_RootGuards
{
	static const string WOLF = "VB_RootGuard_Wolf";
	static const string ZANOZA = "VB_RootGuard_Zanoza";
	static const string ENT = "VB_RootGuard_Ent";
	static const string ROOT_TYPE = "VB_AR_RootPiece";
	static const string POLLEN_TYPE = "VB_AR_Spores";

	// состав волны: wave — номер волны у этой сердцевины (1..), level — ступень рощи (1 росток, 2 роща, 3 древняя)
	static TStringArray VB_WaveTypes(int wave, int level = 2)
	{
		TStringArray types = new TStringArray;
		int wolves;
		int zanozas;
		if (wave <= 1)
		{
			wolves = Math.RandomIntInclusive(2, 3);
			zanozas = 0;
		}
		else if (wave == 2)
		{
			wolves = Math.RandomIntInclusive(3, 4);
			zanozas = 1;
		}
		else
		{
			wolves = Math.RandomIntInclusive(2, 3);
			zanozas = 2;
		}
		if (level <= 1 && zanozas > 1)
			zanozas = 1;					// у ростка не больше одной занозы
		for (int i = 0; i < wolves; i++)
			types.Insert(WOLF);
		for (int k = 0; k < zanozas; k++)
			types.Insert(ZANOZA);
		if (level >= 3 && wave >= 3)
			types.Insert(ENT);				// Хранитель — только у древней рощи, последней волной
		return types;
	}

	static bool VB_Is(EntityAI body)
	{
		return body && body.ConfigGetBool("vbRootGuard");
	}

	// сервер: страж разделан игроком player — шанс на отросток и пыльцу к ногам (ванильный лут уже выпал)
	static void VB_SkinServer(EntityAI body, PlayerBase player)
	{
		if (!body || !player)
			return;

		string who = VB_AR_ItemsServer.VB_Who(player);
		string what = body.GetType();
		// стражи, которых позвал фонарь (волна с loot = false), — только ванильный лут
		if (!VB_AR_Horde.VB_HasLoot(body))
		{
			Print("[VB_AncientRoot] разделал стража " + what + ": " + who + " — страж фонаря, только ванильный лут");
			return;
		}

		float rootChance = 0.15;
		float pollenChance = 0.05;
		int roots = 1;
		if (body.IsKindOf(ZANOZA))
		{
			rootChance = 0.30;
			pollenChance = 0.10;
		}
		else if (body.IsKindOf(ENT))
		{
			rootChance = 1.0;
			pollenChance = 0.50;
			roots = 2;
		}

		string got = "";
		EntityAI drop;
		if (Math.RandomFloat01() < rootChance)
		{
			for (int i = 0; i < roots; i++)
			{
				drop = VB_AR_ItemsServer.VB_DropAtFeet(player, ROOT_TYPE);
				if (drop)
					got += ROOT_TYPE + " ";
			}
		}
		if (Math.RandomFloat01() < pollenChance)
		{
			drop = VB_AR_ItemsServer.VB_DropAtFeet(player, POLLEN_TYPE);
			if (drop)
				got += POLLEN_TYPE;
		}
		if (got == "")
			got = "ничего сверх ванили";
		Print("[VB_AncientRoot] разделал стража " + what + ": " + who + " получил: " + got);
	}
}
