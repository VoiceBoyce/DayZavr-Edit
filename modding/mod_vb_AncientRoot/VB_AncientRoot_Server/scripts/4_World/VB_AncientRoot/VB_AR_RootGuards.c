// «Древний корень» (сервер 18, ТЗ_сервер_18.md п. 2б.1): стражи рощи вместо зомби Изнанки — только сервер.
// Классы стражей (VB_RootGuard_*, vbRootGuard = 1) — в config.cpp клиента VB_AncientRoot, текстуры и материалы
// делает tools/gen_ar_guards.py.
//
// Волны сердцевины по нарастающей (2б.10, цифры черновые, подбираются на стенде):
//   1-я — 2–3 лесовых волка; 2-я — 3–4 волка и Терновая заноза; 3-я — 2–3 волка и две занозы.
//   Росток: только волки, вырезание — 3 волка и заноза. Древняя: на волка больше, при вырезании ещё Хранитель рощи.
// Разделка стража рощи: лут по конфигу (шкура, мясо; Занозе и Энту — трофеи MSFZ), волку — Клык лесового волка
// скриптом (VB_SkinServer), + шанс на отросток, пыльцу и второй клык.
// Призванные стражи (волна с loot = false: волны за дурную славу, волки светоча, стая Хранителя) — без добычи вовсе
// (VB_AR_GuardSkin.c: тело рассыпается трухой) и без штрафа расположения за убийство (VB_AR_GuardHit.VB_OnKilled).

class VB_RootGuards
{
	static const string WOLF = "VB_RootGuard_Wolf";
	static const string ZANOZA = "VB_RootGuard_Zanoza";
	static const string ENT = "VB_RootGuard_Ent";
	static const string ROOT_TYPE = "VB_AR_RootPiece";
	static const string POLLEN_TYPE = "VB_AR_Spores";
	static const string FANG_TYPE = "VB_AR_WolfFang";
	static const string KEEPER_HEART = "VB_AR_KeeperHeart";	// трофей Хранителя (ТЗ 2в п.2) — всегда
	static const float FANG2_CHANCE = 0.25;		// второй клык у волка рощи (первый — всегда)

	// страж призван (не встал на защиту рощи сам): без добычи и без штрафа за убийство
	static bool VB_Summoned(EntityAI body)
	{
		return body && !VB_AR_Horde.VB_HasLoot(body);
	}

	// состав волны: wave — номер волны у этой сердцевины (1..), level — ступень рощи (1 росток, 2 роща, 3 древняя);
	// keeper = false — без Хранителя (он выходит на вырезание древней только раз за рестарт)
	static TStringArray VB_WaveTypes(int wave, int level = 2, bool keeper = true)
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
		if (level <= 1)
		{
			// росток (ТЗ 2б.4): только волки; вырезание — 3 волка и одна заноза
			zanozas = 0;
			if (wave >= 3)
			{
				wolves = 3;
				zanozas = 1;
			}
		}
		else if (level >= 3)
		{
			wolves++;						// древняя: на волка больше в каждой волне
		}
		for (int i = 0; i < wolves; i++)
			types.Insert(WOLF);
		for (int k = 0; k < zanozas; k++)
			types.Insert(ZANOZA);
		if (level >= 3 && wave >= 3 && keeper)
			types.Insert(ENT);				// Хранитель — только у древней рощи, последней волной
		return types;
	}

	static bool VB_Is(EntityAI body)
	{
		return body && body.ConfigGetBool("vbRootGuard");
	}

	// сервер: страж рощи разделан игроком player — клык волку, шанс на отросток и пыльцу к ногам (лут по конфигу уже
	// выпал). Призванных сюда не зовут: у них разделка без добычи вовсе (VB_AR_GuardSkin.c)
	static void VB_SkinServer(EntityAI body, PlayerBase player)
	{
		if (!body || !player || VB_Summoned(body))
			return;

		string who = VB_AR_ItemsServer.VB_Who(player);
		string what = body.GetType();

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
		// клык волку — здесь, а не в Skinning конфига: у призванных волков его быть не должно; второй — шанс
		if (body.IsKindOf(WOLF))
		{
			int fangs = 1;
			if (Math.RandomFloat01() < FANG2_CHANCE)
				fangs = 2;
			for (int f = 0; f < fangs; f++)
			{
				drop = VB_AR_ItemsServer.VB_DropAtFeet(player, FANG_TYPE);
				if (drop)
					got += FANG_TYPE + " ";
			}
		}
		if (body.IsKindOf(ENT))
		{
			drop = VB_AR_ItemsServer.VB_DropAtFeet(player, KEEPER_HEART);
			if (drop)
				got += KEEPER_HEART + " ";
		}
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
