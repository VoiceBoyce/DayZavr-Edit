// «Лес помнит» (ТЗ_сервер_18.md, ветка 2в п.4) — только сервер. Ночью в лесу (8+ деревьев в 20 м или у рощи), не в
// машине, не под крышей, не у флага территории (те же условия, что у волн за расположение — VB_AR_FavorServer.VB_WaveSpotOk):
//   Враг леса  — изредка шёпот рощи за спиной (в среднем раз в ~5 мин);
//   Проклятый  — шёпот чаще (~раз в 2 мин) и «хватка корней» раз в GRIP_MIN_MS..GRIP_MAX_MS: корни у ног, выносливость
//                в ноль, пара секунд на месте, без хромоты, «Корни хватают тебя за ноги — лес помнит тебя».
// Сбой метки на карте не делаем: карта на 18 — сторонние моды. Клиент — VB_AR_Memory.c.

class VB_AR_Memory
{
	static const int TICK_MS = 30000;
	static const float ENEMY_WHISPER = 0.10;		// на проверку раз в 30 с
	static const float CURSED_WHISPER = 0.25;
	static const int GRIP_MIN_MS = 300000;
	static const int GRIP_MAX_MS = 600000;
	static const int FIRST_MIN_MS = 60000;			// первая хватка после того, как Проклятый оказался ночью в лесу
	static const int FIRST_MAX_MS = 180000;
	static const float BEHIND = 6.0;				// м за спиной
	static const float SEE_DIST = 100.0;			// кто видит корни у ног

	protected static ref map<string, int> s_VB_NextGrip;	// SteamID -> когда следующая хватка
	protected static bool s_VB_Started;

	static void VB_Start()
	{
		if (s_VB_Started)
			return;
		s_VB_Started = true;
		s_VB_NextGrip = new map<string, int>;
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_Tick, TICK_MS, true);
	}

	static void VB_Tick()
	{
		if (VB_AR_Pulse.VB_NightFactor() < VB_AR_Pulse.NIGHT_FULL_ABOVE)
			return;
		array<Man> players = new array<Man>;
		g_Game.GetPlayers(players);
		int now = g_Game.GetTime();
		PlayerBase p;
		string id;
		int tier;
		int next;
		float chance;
		foreach (Man m : players)
		{
			p = PlayerBase.Cast(m);
			if (!p || !p.IsAlive() || !p.GetIdentity())
				continue;
			tier = VB_AR_FavorServer.VB_TierNow(p);
			if (tier > VB_AR_Favor.TIER_ENEMY)
				continue;
			if (!VB_AR_FavorServer.VB_WaveSpotOk(p))
				continue;
			id = p.GetIdentity().GetPlainId();
			chance = ENEMY_WHISPER;
			if (tier == VB_AR_Favor.TIER_CURSED)
				chance = CURSED_WHISPER;
			if (Math.RandomFloat01() < chance)
				VB_WhisperBehind(p);
			if (tier != VB_AR_Favor.TIER_CURSED || p.IsUnconscious())
				continue;
			if (!s_VB_NextGrip.Find(id, next))
			{
				s_VB_NextGrip.Set(id, now + Math.RandomIntInclusive(FIRST_MIN_MS, FIRST_MAX_MS));	// первая — через 1–3 мин
				continue;
			}
			if (now < next)
				continue;
			s_VB_NextGrip.Set(id, now + Math.RandomIntInclusive(GRIP_MIN_MS, GRIP_MAX_MS));
			VB_Grip(p, players);
		}
	}

	// шёпот рощи у игрока за спиной
	static void VB_WhisperBehind(PlayerBase p)
	{
		vector dir = p.GetDirection();
		vector pos = p.GetPosition() - dir * BEHIND + "0 1.2 0";
		VB_WhisperAt(p, pos);
	}

	static void VB_WhisperAt(PlayerBase p, vector pos)
	{
		if (p && p.GetIdentity())
			g_Game.RPCSingleParam(p, VB_AR_MemoryFx.RPC_WHISPER, new Param1<vector>(pos), true, p.GetIdentity());
	}

	protected static void VB_Grip(PlayerBase target, array<Man> players)
	{
		vector tp = target.GetPosition();
		if (target.GetStaminaHandler())
			target.GetStaminaHandler().SetStamina(0);
		float seeSq = SEE_DIST * SEE_DIST;
		PlayerBase p;
		bool isTarget;
		foreach (Man m : players)
		{
			p = PlayerBase.Cast(m);
			if (!p || !p.GetIdentity() || vector.DistanceSq(p.GetPosition(), tp) > seeSq)
				continue;
			isTarget = p == target;
			g_Game.RPCSingleParam(p, VB_AR_MemoryFx.RPC_MEMORY_GRIP, new Param2<vector, bool>(tp, isTarget), true, p.GetIdentity());
		}
		NotificationSystem.SendNotificationToPlayerExtended(target, 5, "Корни хватают тебя за ноги", "Лес помнит тебя.");
		Print("[VB_AncientRoot] лес помнит: хватка корней — " + VB_AR_ItemsServer.VB_Who(target));
	}
}
