// Чужие детекторы (ТЗ 2.2): строки detectors.json с Signal = "Generic", например детекторы OOTS.
// Их экран про наши находки ничего не знает, поэтому мод подаёт свой писк. Кода чужого мода не касаемся:
// работаем только по имени класса предмета в руках.
//
// Сервер раз в секунду смотрит, что у каждого игрока в руках. Если это чужой детектор из detectors.json и он
// работает (у предмета нет менеджера энергии или GetCompEM().IsWorking()), ищется ближайшая разрешённая находка
// по его Tier и Radius (во все стороны). Ближе SpawnDistance — находка становится предметом, иначе игроку уходит
// RPC с силой сигнала, и клиент пищит из предмета: чем ближе, тем чаще и громче.
//
// Запуск: VB_AB_GenericDetectors.Start() на сервере (MissionServer.OnInit, после VB_AB_Server.Init()).

class VB_AB_GenericDetectors
{
	static const int TICK_MS = 1000;

	protected static ref VB_AB_GenericDetectors s_Instance;
	protected ref array<Man> m_Players = new array<Man>;
	protected ref array<string> m_Signaled = new array<string>;		// кому в прошлый такт ушёл сигнал
	protected ref array<string> m_SignaledNow = new array<string>;

	static void Start()
	{
		if (s_Instance || !GetGame().IsServer())
			return;
		s_Instance = new VB_AB_GenericDetectors;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(s_Instance.Tick, TICK_MS, true);
		VB_AB_Log.Info("писк для чужих детекторов (Signal Generic) запущен");
	}

	protected void Tick()
	{
		VB_AB_Cfg cfg = VB_AB_Cfg.Get();
		bool scan = VB_AB_Finds.Count() > 0 && HasGeneric(cfg);
		if (!scan && m_Signaled.Count() == 0)
			return;
		m_SignaledNow.Clear();
		m_Players.Clear();
		GetGame().GetPlayers(m_Players);
		PlayerBase p;
		string id;
		bool sent;
		foreach (Man m : m_Players)
		{
			p = PlayerBase.Cast(m);
			if (!p || !p.GetIdentity())
				continue;
			id = p.GetIdentity().GetId();
			sent = false;
			if (scan)
				sent = ScanPlayer(p, cfg);
			if (sent)
				m_SignaledNow.Insert(id);
			else if (m_Signaled.Find(id) != -1)
				Send(p, -1);		// сигнал пропал — клиент замолкает сразу, а не по таймауту
		}
		m_Signaled.Clear();
		m_Signaled.Copy(m_SignaledNow);
	}

	// true — игроку ушёл сигнал.
	protected bool ScanPlayer(PlayerBase p, VB_AB_Cfg cfg)
	{
		if (!p.IsAlive())
			return false;
		ItemBase item = p.GetItemInHands();
		if (!item)
			return false;
		string type = item.GetType();
		VB_AB_DetectorCfg det = cfg.m_Detectors.Get(type);
		if (!det || !IsGeneric(det))
			return false;
		if (item.HasEnergyManager() && !item.GetCompEM().IsWorking())
			return false;
		float dist;
		VB_AB_Find f = VB_AB_Finds.Nearest(p.GetPosition(), det.Radius, type, det.Tier, dist);
		if (!f)
			return false;
		if (dist < det.SpawnDistance)
		{
			vector at = f.m_Pos;
			EntityAI found = VB_AB_Finds.Materialize(f, p, type);
			GetGame().RPCSingleParam(p, VB_AB_GenericBeeper.RPC_FOUND, VB_AB_Finds.FoundParam(found, at), true);
			return false;
		}
		float k = 1;
		if (det.Radius > det.SpawnDistance)
			k = Math.Clamp(Math.InverseLerp(det.Radius, det.SpawnDistance, dist), 0, 1);
		int strength = Math.Round(k * 100);
		Send(p, strength);
		return true;
	}

	protected void Send(PlayerBase p, int strength)
	{
		GetGame().RPCSingleParam(p, VB_AB_GenericBeeper.RPC_SIGNAL, new Param1<int>(strength), true, p.GetIdentity());
	}

	static bool IsGeneric(VB_AB_DetectorCfg det)
	{
		string s = det.Signal;
		s.ToLower();
		return s == "generic";
	}

	protected bool HasGeneric(VB_AB_Cfg cfg)
	{
		foreach (string cls, VB_AB_DetectorCfg det : cfg.m_Detectors)
		{
			if (det && IsGeneric(det))
				return true;
		}
		return false;
	}
};
