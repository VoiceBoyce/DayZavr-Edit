// Серверная часть объектов Изнанки (только сервер, VB_Rift_Server). Клиентская часть тех же классов
// (синхронизируемые переменные, внешний вид, пульс, обвивки, дома) — VB_Rift, VB_RiftBase.c / VB_RiftHouses.c.
//   корень: отлом (VB_BreakServer) и «спит ли» на сервере (VB_IsDormantServer);
//   сердце: вырезание (VB_KillServer), тревога и волны зомби (VB_DisturbServer), споры (VB_TakeSporesServer),
//           тепло ночью (UniversalTemperatureSource), список сердец для «Слушающего корня» (VB_NearestServer);
//   молния: рассылка RPC всем игрокам (VB_RiftStrike.VB_BroadcastServer).
// Все события — строками [VB_Rift] в script-лог сервера.

modded class VB_RiftBase
{
	// сервер: от корня отломили кусок — гаснет до рестарта (гнёзда каждый рестарт ставятся заново)
	void VB_BreakServer()
	{
		if (m_VB_Broken)
			return;

		m_VB_Broken = true;
		SetSynchDirty();
	}

	// Сервер: корень спит так же, как его видит клиент (VB_IsDormant): отломан, или ближайшее сердце
	// в VB_RiftPulse.LINK_DIST вырезано, или сердца рядом нет вовсе. Связей с сердцами (m_VB_LinkHeart) на сервере
	// нет — ищем по списку сердец. Нужен проверке отлома корня (VB_FindRootServer): изменённый клиент
	// не должен отламывать корни вырезанного сердца (баг №39).
	bool VB_IsDormantServer()
	{
		if (m_VB_Broken)
			return true;

		VB_RiftHeart heart = VB_RiftHeart.VB_NearestAnyServer(GetPosition(), VB_RiftPulse.LINK_DIST);
		return !heart || heart.VB_IsDead();
	}
}

modded class VB_RiftHeart
{
	static const string CHUNK_TYPE = "VB_RiftHeartChunk";
	static const int CALM_MS = 90000;			// столько после последней попытки резать сердце остаётся потревоженным
	static const int NOISE_COOLDOWN_MS = 10000;	// шум у сердца не чаще (резка и сбор спор зовут его на каждое начало)
	static const int HORDE_MAX = 3;				// волн на сердце за рестарт (баг №29)

	// Тепло: живое сердце ночью греет, как слабый костёр (ванильный UniversalTemperatureSource, как у Bonfire).
	// На морозном Сахале к нему тянет погреться — а там споры, корни и зомби. Сахал сам срезает потолок на 3.
	// UTS держится по ref в самом сердце всю его жизнь (E73), включается и выключается SetActive.
	static const float HEAT_FULL = 2.5;		// м: полное тепло
	static const float HEAT_MAX = 7.0;		// м: дальше не греет
	static const float HEAT_CAP = 18.0;		// потолок температуры для игрока (у костра Bonfire 25)
	static const int HEAT_CHECK_MS = 15000;	// как часто сервер сверяет «ночь/жив»

	// все сердца гнёзд на сервере (для «Слушающего корня» и проверки «корень спит»)
	protected static ref array<VB_RiftHeart> s_VB_ServerHearts;

	protected ref UniversalTemperatureSource m_VB_Heat;
	protected ref UniversalTemperatureSourceSettings m_VB_HeatSettings;
	protected ref UniversalTemperatureSourceLambdaConstant m_VB_HeatLambda;
	protected ref VB_RiftHorde m_VB_Horde;		// последняя волна этого сердца (кончилась ли — VB_IsOver)
	protected int m_VB_HordeCount;				// сколько волн уже поднято за рестарт
	protected int m_VB_NoiseMs = -100000;		// когда последний раз шумело (g_Game.GetTime)

	// ближайшее живое сердце не дальше maxDist; null — нет
	static VB_RiftHeart VB_NearestServer(vector pos, float maxDist)
	{
		if (!s_VB_ServerHearts)
			return null;

		VB_RiftHeart best;
		float bestSq = maxDist * maxDist;
		float dSq;
		foreach (VB_RiftHeart h : s_VB_ServerHearts)
		{
			if (!h || h.VB_IsDead())
				continue;
			dSq = vector.DistanceSq(pos, h.GetPosition());
			if (dSq < bestSq)
			{
				bestSq = dSq;
				best = h;
			}
		}
		return best;
	}

	// ближайшее сердце не дальше maxDist, живое или вырезанное (как клиент связывает корни: VB_RiftPulse.VB_Relink)
	static VB_RiftHeart VB_NearestAnyServer(vector pos, float maxDist)
	{
		if (!s_VB_ServerHearts)
			return null;

		VB_RiftHeart best;
		float bestSq = maxDist * maxDist;
		float dSq;
		foreach (VB_RiftHeart h : s_VB_ServerHearts)
		{
			if (!h)
				continue;
			dSq = vector.DistanceSq(pos, h.GetPosition());
			if (dSq < bestSq)
			{
				bestSq = dSq;
				best = h;
			}
		}
		return best;
	}

	override void EEInit()
	{
		super.EEInit();
		if (m_VB_Bare)
			return;
		if (!g_Game.IsServer() && g_Game.IsMultiplayer())
			return;

		if (!s_VB_ServerHearts)
			s_VB_ServerHearts = new array<VB_RiftHeart>;
		s_VB_ServerHearts.Insert(this);

		m_VB_HeatSettings = new UniversalTemperatureSourceSettings();
		m_VB_HeatSettings.m_UpdateInterval = 2;
		m_VB_HeatSettings.m_Updateable = true;
		m_VB_HeatSettings.m_TemperatureItemCap = GameConstants.ITEM_TEMPERATURE_NEUTRAL_ZONE_MIDDLE;
		m_VB_HeatSettings.m_TemperatureCap = HEAT_CAP;
		m_VB_HeatSettings.m_RangeFull = HEAT_FULL;
		m_VB_HeatSettings.m_RangeMax = HEAT_MAX;
		m_VB_HeatSettings.m_ItemDryModifier = 2.0;
		m_VB_HeatLambda = new UniversalTemperatureSourceLambdaConstant();
		m_VB_Heat = new UniversalTemperatureSource(this, m_VB_HeatSettings, m_VB_HeatLambda);
		VB_UpdateHeatServer();
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_UpdateHeatServer, HEAT_CHECK_MS, true);
	}

	// Серверная очистка — ДО super: базовая VB_RiftBase.VB_Cleanup выходит сразу, если объект не активен,
	// а на сервере он не активен никогда (пульса там нет).
	override protected void VB_Cleanup()
	{
		if (g_Game)
		{
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_CalmServer);
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_UpdateHeatServer);
		}
		if (m_VB_Heat)
			m_VB_Heat.SetActive(false);
		if (s_VB_ServerHearts)
			s_VB_ServerHearts.RemoveItem(this);
		super.VB_Cleanup();
	}

	// сервер: забрать одну порцию спор; false — уже нет
	bool VB_TakeSporesServer()
	{
		if (m_VB_Dead || m_VB_SporesLeft <= 0)
			return false;

		m_VB_SporesLeft--;
		SetSynchDirty();
		return true;
	}

	// Сервер: сердце потревожено — бьётся чаще, шум тянет зомби (не чаще NOISE_COOLDOWN_MS).
	// Резка (horde = true) поднимает волну, если прежней ещё не было или она кончилась, — до HORDE_MAX за рестарт.
	// Сбор спор (horde = false) только будит сердце и шумит.
	void VB_DisturbServer(PlayerBase cutter, bool horde = true)
	{
		if (m_VB_Dead)
			return;

		if (!m_VB_Disturbed)
		{
			m_VB_Disturbed = true;
			SetSynchDirty();
			VB_UpdateHeatServer();
		}
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_CalmServer);
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_CalmServer, CALM_MS, false);

		int now = g_Game.GetTime();
		if (now - m_VB_NoiseMs >= NOISE_COOLDOWN_MS || now < m_VB_NoiseMs)
		{
			m_VB_NoiseMs = now;
			VB_RiftHorde.VB_CallNoise(GetPosition());
		}

		if (horde)
			VB_HordeServer(cutter);
	}

	// новая волна, если можно: прежняя кончилась (все зомби убиты или убраны) и лимит волн не выбран
	protected void VB_HordeServer(PlayerBase cutter)
	{
		if (m_VB_HordeCount >= HORDE_MAX)
			return;
		if (m_VB_Horde && !m_VB_Horde.VB_IsOver())
			return;

		VB_RiftHorde wave = VB_RiftHorde.VB_SpawnWave(GetPosition());
		if (!wave)
			return;		// потолок зомби на сервере или рядом: волна не поднята и не засчитана

		m_VB_Horde = wave;
		m_VB_HordeCount++;
		string who = VB_RiftItemsServer.VB_Who(cutter);
		string at = GetPosition().ToString();
		int hordeMax = HORDE_MAX;
		string numStr = m_VB_HordeCount.ToString();
		string maxStr = hordeMax.ToString();
		Print("[VB_Rift] сердце в " + at + ": волна " + numStr + " из " + maxStr + ", режет " + who);
	}

	void VB_CalmServer()
	{
		if (!m_VB_Disturbed)
			return;

		m_VB_Disturbed = false;
		SetSynchDirty();
	}

	// сервер: сердце вырезано — гаснет до рестарта, вырезавшему «Сердце Изнанки» (на землю у ног), всем игрокам удар молнии
	void VB_KillServer(PlayerBase cutter)
	{
		if (m_VB_Dead)
			return;

		m_VB_Dead = true;
		m_VB_Disturbed = false;
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_CalmServer);
		SetSynchDirty();

		// вырезанное до рестарта не греет: погасить и снять проверку тепла (баг №39)
		if (m_VB_Heat && m_VB_Heat.IsActive())
			m_VB_Heat.SetActive(false);
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_UpdateHeatServer);

		// для админов: кто и где вырезал (script_*.log сервера)
		string who = VB_RiftItemsServer.VB_Who(cutter);
		string at = GetPosition().ToString();
		Print("[VB_Rift] сердце вырезано: " + who + " в " + at);

		if (cutter)
			VB_RiftItemsServer.VB_DropAtFeet(cutter, CHUNK_TYPE);	// на землю у ног, не в инвентарь

		VB_RiftStrike.VB_BroadcastServer();
		VB_RiftNests.VB_OnHeartKilled(this);	// прорастание нового гнезда, если включено в настройках
	}

	// греет только живое сердце ночью (или пока его режут)
	void VB_UpdateHeatServer()
	{
		if (!m_VB_Heat)
			return;

		bool on = !m_VB_Dead && (m_VB_Disturbed || VB_RiftPulse.VB_NightFactor() >= VB_RiftPulse.NIGHT_LIGHT_ABOVE);
		if (m_VB_Heat.IsActive() != on)
			m_VB_Heat.SetActive(on);
	}
}

modded class VB_RiftStrike
{
	// сервер: каждому игроку по удару (у каждого клиента своя точка вокруг него, VB_RiftStrike.VB_Start)
	static void VB_BroadcastServer()
	{
		array<Man> players = new array<Man>;
		g_Game.GetPlayers(players);
		foreach (Man someone : players)
		{
			PlayerBase pb = PlayerBase.Cast(someone);
			if (pb && pb.GetIdentity())
				g_Game.RPCSingleParam(pb, RPC_STRIKE, new Param1<bool>(true), true, pb.GetIdentity());
		}
	}
}
