class SFP_Item_scanner_spawnMutant extends Inventory_Base
{
	//=============================
	// CONFIG (hardcoded only for visuals / sounds / slots)
	//=============================
	// NOTE: Wave lists, counts, time between waves, alive-cap limits, and radiuses are now in JSON:
	// $profile:SFP/ScannerEvent.json
	// (created automatically on first server start with this mod)
	static const float	MAX_TRACK_RADIUS				= 250.0; // stop tracking far-away mobs

	static const string	BATTERY_SLOT_NAME			= "CarBattery";

	static const string	SOUNDSET_WORK_LOOP			= "SFP_Scaner_teleportIdle_Soundset";
	static const string	SOUNDSET_PULSE				= "SFP_Scaner_teleportWork_Soundset";

	// Timer display selections (edit if your model uses different names)
	static const string	TIMER_SELECTION_LEFT		= "time1_X";
	static const string	TIMER_SELECTION_RIGHT		= "timeX_1";

	static const string	DIGIT_TEX_0	= "SFP3\\1_item\\props\\data\\scaner\\timer\\scan_time_0.paa";
	static const string	DIGIT_TEX_1	= "SFP3\\1_item\\props\\data\\scaner\\timer\\scan_time_1.paa";
	static const string	DIGIT_TEX_2	= "SFP3\\1_item\\props\\data\\scaner\\timer\\scan_time_2.paa";
	static const string	DIGIT_TEX_3	= "SFP3\\1_item\\props\\data\\scaner\\timer\\scan_time_3.paa";
	static const string	DIGIT_TEX_4	= "SFP3\\1_item\\props\\data\\scaner\\timer\\scan_time_4.paa";
	static const string	DIGIT_TEX_5	= "SFP3\\1_item\\props\\data\\scaner\\timer\\scan_time_5.paa";
	static const string	DIGIT_TEX_6	= "SFP3\\1_item\\props\\data\\scaner\\timer\\scan_time_6.paa";
	static const string	DIGIT_TEX_7	= "SFP3\\1_item\\props\\data\\scaner\\timer\\scan_time_7.paa";
	static const string	DIGIT_TEX_8	= "SFP3\\1_item\\props\\data\\scaner\\timer\\scan_time_8.paa";
	static const string	DIGIT_TEX_9	= "SFP3\\1_item\\props\\data\\scaner\\timer\\scan_time_9.paa";

	//=============================
	// STATE (network)
	//=============================
	protected int		m_SecondsLeft;			// 0..99 (visual timer)
	protected bool		m_IsEventRunning;		// for JIP / client effects
	protected int		m_WaveStage;				// 0..2 (3 groups)
	protected int		m_PulseCounter;			// increments each wave -> triggers pulse effects on clients
	protected float		m_EventPlayerRadius;	// used by clients for pulse shake/sound distance (synced from JSON)

	//=============================
	// STATE (server)
	//=============================
	protected ref Timer	m_ServerTickTimer;
	protected bool		m_PausedByAliveCap;
	protected ref array<EntityAI>	m_TrackedSpawned;

	//=============================
	// STATE (client)
	//=============================
	protected EffectSound	m_WorkLoopSound;
	protected EffectSound	m_PulseSound;
	protected int			m_PulseCounterLast;

	protected ref map<int, string>	m_DigitTextureMap;
	protected int			m_TimerSelLeftIdx = INDEX_NOT_FOUND;
	protected int			m_TimerSelRightIdx = INDEX_NOT_FOUND;
	protected int			m_LastLeftDigit = -1;
	protected int			m_LastRightDigit = -1;

	//=============================
	// JSON CONFIG (server)
	//=============================
	protected ref SFP_ScannerEventConfig m_CachedCfg;

	//=============================
	// CTOR / DTOR
	//=============================
	void SFP_Item_scanner_spawnMutant()
	{
		m_SecondsLeft = 99;
		m_IsEventRunning = false;
		m_WaveStage = 0;
		m_PulseCounter = 0;
		m_PulseCounterLast = 0;
		m_EventPlayerRadius = 80.0;
		m_PausedByAliveCap = false;
		m_TrackedSpawned = new array<EntityAI>;

		RegisterNetSyncVariableInt("m_SecondsLeft", 0, 99);
		RegisterNetSyncVariableBool("m_IsEventRunning");
		RegisterNetSyncVariableInt("m_WaveStage", 0, 2);
		RegisterNetSyncVariableInt("m_PulseCounter");
		RegisterNetSyncVariableFloat("m_EventPlayerRadius", 0, 500, 1);

		BuildDigitTextureMap();
	}

	void ~SFP_Item_scanner_spawnMutant()
	{
		StopServerTimer();

		#ifndef SERVER
		StopSoundSet(m_WorkLoopSound);
		StopSoundSet(m_PulseSound);
		#endif
	}

	//=============================
	// LIFE CYCLE
	//=============================
	override void EEInit()
	{
		super.EEInit();
		CacheTimerSelectionIndices();
		UpdateTimerDisplay(m_SecondsLeft);
	}

	override void EEDelete(EntityAI parent)
	{
		StopServerTimer();
		super.EEDelete(parent);
	}

	override void OnItemLocationChanged(EntityAI old_owner, EntityAI new_owner)
	{
		super.OnItemLocationChanged(old_owner, new_owner);

		// Если кто-то поднял/переместил устройство во время работы - выключаем, чтобы ивент был ТОЛЬКО на земле
		if (GetGame().IsServer() && GetCompEM() && GetCompEM().IsWorking())
		{
			if (!IsOnGround())
			{
				GetCompEM().SwitchOff();
			}
		}
	}

	//=============================
	// ACTIONS
	//=============================
	// ВАНИЛЬНЫЕ экшены включения/выключения на земле.
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionTurnOnWhileOnGround);
		AddAction(ActionTurnOffWhileOnGround);
	}

	//=============================
	// ENERGY EVENTS
	//=============================
	override void OnSwitchOn()
	{
		super.OnSwitchOn();

		// Доп. защита: не даём включать без батареи или не на земле
		if (GetGame().IsServer())
		{
			if (!IsOnGround() || !HasRequiredBattery())
			{
				GetCompEM().SwitchOff();
			}
		}
	}

	override void OnWorkStart()
	{
		super.OnWorkStart();

		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			StartEventServer();
		}

		#ifndef SERVER
		StartWorkLoopSoundClient();
		#endif
	}

	override void OnWorkStop()
	{
		super.OnWorkStop();

		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			StopEventServer();
		}

		#ifndef SERVER
		StopSoundSet(m_WorkLoopSound);
		#endif
	}

	//=============================
	// NET SYNC
	//=============================
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();

		#ifndef SERVER
		CacheTimerSelectionIndices();
		UpdateTimerDisplay(m_SecondsLeft);

		// JIP: если уже работает - поднимаем луп
		if (m_IsEventRunning && GetCompEM() && GetCompEM().IsWorking())
		{
			StartWorkLoopSoundClient();
		}
		else
		{
			StopSoundSet(m_WorkLoopSound);
		}

		// pulse effect
		if (m_PulseCounter != m_PulseCounterLast)
		{
			m_PulseCounterLast = m_PulseCounter;
			DoPulseEffectsClient();
		}
		#endif
	}

	//=============================
	// SERVER: EVENT LOOP
	//=============================
	protected SFP_ScannerEventConfig GetCfgServer(bool refresh = false)
	{
		if (!(GetGame().IsServer() || !GetGame().IsMultiplayer()))
			return null;

		if (refresh || !m_CachedCfg)
			m_CachedCfg = SFP_ScannerConfigManager.Get();

		return m_CachedCfg;
	}

	protected void StartEventServer()
	{
		SFP_ScannerEventConfig cfg = GetCfgServer(true);
		m_EventPlayerRadius = 80.0;
		if (cfg)
			m_EventPlayerRadius = cfg.EventPlayerRadius;
		if (m_EventPlayerRadius < 0)
			m_EventPlayerRadius = 0;
		if (m_EventPlayerRadius > 500)
			m_EventPlayerRadius = 500;

		m_IsEventRunning = true;
		m_WaveStage = 0;
		m_SecondsLeft = GetNextWaveIntervalSeconds();
		m_PausedByAliveCap = false;
		if (m_TrackedSpawned)
			m_TrackedSpawned.Clear();
		SetSynchDirty();

		StartServerTimer();
	}

	protected void StopEventServer()
	{
		StopServerTimer();

		m_IsEventRunning = false;
		m_SecondsLeft = 99;
		m_PausedByAliveCap = false;
		m_EventPlayerRadius = 0.0;
		if (m_TrackedSpawned)
			m_TrackedSpawned.Clear();
		SetSynchDirty();
	}

	protected int GetNextWaveIntervalSeconds()
	{
		SFP_ScannerEventConfig cfg = GetCfgServer();
		int minS = 10;
		int maxS = 60;
		if (cfg)
		{
			minS = cfg.IntervalMinSeconds;
			maxS = cfg.IntervalMaxSeconds;
		}
		if (maxS < minS)
			maxS = minS;

		int v = Math.RandomIntInclusive(minS, maxS);
		return Math.Clamp(v, 0, 99);
	}

	protected void StartServerTimer()
	{
		if (!m_ServerTickTimer)
			m_ServerTickTimer = new Timer(CALL_CATEGORY_SYSTEM);

		if (m_ServerTickTimer.IsRunning())
			m_ServerTickTimer.Stop();

		m_ServerTickTimer.Run(1.0, this, "OnServerTick", NULL, true);
	}

	protected void StopServerTimer()
	{
		if (m_ServerTickTimer && m_ServerTickTimer.IsRunning())
			m_ServerTickTimer.Stop();
	}

	protected void OnServerTick()
	{
		if (!GetCompEM() || !GetCompEM().IsWorking())
			return;

		// ивент только на земле + только с батареей
		if (!IsOnGround() || !HasRequiredBattery())
		{
			GetCompEM().SwitchOff();
			return;
		}

		SFP_ScannerEventConfig cfg = GetCfgServer();
		float countRadius = 70.0;
		int pauseMoreThan = 20;
		int resumeLessThan = 15;
		if (cfg)
		{
			countRadius = cfg.AliveCountRadius;
			pauseMoreThan = cfg.PauseIfAliveMoreThan;
			resumeLessThan = cfg.ResumeWhenAliveLessThan;
		}

		// Anti-overflow: pause countdown if too many alive mobs are still around.
		int alive = CountTrackedAliveInRadius(countRadius);
		if (!m_PausedByAliveCap)
		{
			if (alive > pauseMoreThan)
			{
				m_PausedByAliveCap = true;
				return; // freeze timer
			}
		}
		else
		{
			// resume only when alive < threshold
			if (alive < resumeLessThan)
				m_PausedByAliveCap = false;
			else
				return;
		}

		m_SecondsLeft = Math.Clamp(m_SecondsLeft - 1, 0, 99);

		if (m_SecondsLeft <= 0)
		{
			SpawnWaveServer(m_WaveStage);
			m_WaveStage = (m_WaveStage + 1) % 3;
			m_SecondsLeft = GetNextWaveIntervalSeconds();
		}

		SetSynchDirty();
	}

	protected void SpawnWaveServer(int stage)
	{
		array<string> pool;
		int minCount;
		int maxCount;
		GetWaveDefinition(stage, pool, minCount, maxCount);
		if (!pool || pool.Count() == 0)
			return;

		int count = Math.RandomIntInclusive(minCount, maxCount);

		// Триггерим импульс на клиентах (звук+трясучка)
		m_PulseCounter++;
		SetSynchDirty();

		SFP_ScannerEventConfig cfg = GetCfgServer();
		float participantRadius = 80.0;
		if (cfg)
			participantRadius = cfg.EventPlayerRadius;
		array<PlayerBase> participants = GetPlayersInRange(participantRadius);

		for (int i = 0; i < count; i++)
		{
			string typeName = pool.Get(Math.RandomInt(0, pool.Count()));
			PlayerBase anchor = null;
			if (participants && participants.Count() > 0)
				anchor = participants.Get(Math.RandomInt(0, participants.Count()));

			vector spawnPos = FindSpawnPosition(anchor, participants);
			if (spawnPos == vector.Zero)
				continue;

			Object spawned = GetGame().CreateObjectEx(typeName, spawnPos, ECE_PLACE_ON_SURFACE | ECE_INITAI);
			EntityAI ent = EntityAI.Cast(spawned);
			if (ent)
			{
				TrackSpawned(ent);
				// Поворачиваем в сторону ближайшего игрока (чтобы сразу "видели")
				PlayerBase target = GetNearestPlayer(spawnPos, participants);
				if (target)
				{
					vector dir = target.GetPosition() - spawnPos;
					dir[1] = 0;
					if (dir.LengthSq() > 0.01)
					{
						dir.Normalize();
						ent.SetOrientation(dir.VectorToAngles());
					}
				}
			}
		}
	}

	protected void GetWaveDefinition(int stage, out array<string> pool, out int minCount, out int maxCount)
	{
		pool = null;
		minCount = 0;
		maxCount = 0;

		SFP_ScannerEventConfig cfg = GetCfgServer();
		if (!cfg)
			return;

		SFP_ScannerWaveConfig wave = null;
		if (stage == 0)
			wave = cfg.Easy;
		else if (stage == 1)
			wave = cfg.Hard;
		else
			wave = cfg.VeryHard;

		if (!wave)
			return;

		pool = wave.ClassNames;
		minCount = wave.CountMin;
		maxCount = wave.CountMax;
		if (maxCount < minCount)
			maxCount = minCount;
	}

	protected void TrackSpawned(EntityAI ent)
	{
		if (!GetGame().IsServer())
			return;
		if (!m_TrackedSpawned)
			m_TrackedSpawned = new array<EntityAI>;
		m_TrackedSpawned.Insert(ent);
	}

	protected int CountTrackedAliveInRadius(float radius)
	{
		if (!m_TrackedSpawned)
			return 0;

		vector myPos = GetPosition();
		float rSq = radius * radius;
		float maxTrackSq = MAX_TRACK_RADIUS * MAX_TRACK_RADIUS;

		int alive = 0;
		for (int i = m_TrackedSpawned.Count() - 1; i >= 0; i--)
		{
			EntityAI e = m_TrackedSpawned[i];
			if (!e)
			{
				m_TrackedSpawned.Remove(i);
				continue;
			}

			// prune dead
			if (!e.IsAlive())
			{
				m_TrackedSpawned.Remove(i);
				continue;
			}

			float dSq = vector.DistanceSq(e.GetPosition(), myPos);
			// stop tracking very far away
			if (dSq > maxTrackSq)
			{
				m_TrackedSpawned.Remove(i);
				continue;
			}

			if (dSq <= rSq)
				alive++;
		}

		return alive;
	}

	//=============================
	// CLIENT: EFFECTS
	//=============================
	#ifndef SERVER
	protected void StartWorkLoopSoundClient()
	{
		if (!m_WorkLoopSound || !m_WorkLoopSound.IsPlaying())
		{
			PlaySoundSetLoop(m_WorkLoopSound, SOUNDSET_WORK_LOOP, 0.7, 0.7);
		}
	}

	protected void DoPulseEffectsClient()
	{
		PlayerBase pb = PlayerBase.Cast(GetGame().GetPlayer());
		if (!pb)
			return;

		float dist = vector.Distance(pb.GetPosition(), GetPosition());
		float range = m_EventPlayerRadius;
		if (range <= 0)
			range = 80.0;
		if (dist > range)
			return;

		EffectSound s;
		PlaySoundSet(s, SOUNDSET_PULSE, 0, 0);
		if (s)
			s.SetAutodestroy(true);

		// Сильная тряска на пару секунд
		DayZPlayerCamera cam = pb.GetCurrentCamera();
		if (cam)
			cam.SpawnCameraShakeProper(1.6, 1.4, 0.5, 0.5);
	}
	#endif

	//=============================
	// HELPERS: BATTERY / LOCATION
	//=============================
	protected bool HasRequiredBattery()
	{
		EntityAI att = FindAttachmentBySlotName(BATTERY_SLOT_NAME);
		if (!att)
			return false;

		ItemBase ib = ItemBase.Cast(att);
		if (!ib || ib.IsRuined())
			return false;

		// если аккумулятор разряжен - не считаем подходящим
		if (ib.GetCompEM() && ib.GetCompEM().GetEnergy() <= 0)
			return false;

		return true;
	}

	protected bool IsOnGround()
	{
		InventoryLocation loc = new InventoryLocation;
		GetInventory().GetCurrentInventoryLocation(loc);
		return loc.GetType() == InventoryLocationType.GROUND;
	}

	//=============================
	// HELPERS: PLAYERS
	//=============================
	protected array<PlayerBase> GetPlayersInRange(float radius)
	{
		array<PlayerBase> outPlayers = new array<PlayerBase>;
		array<Man> players = new array<Man>;
		GetGame().GetPlayers(players);

		float rSq = radius * radius;
		vector myPos = GetPosition();

		foreach (Man m : players)
		{
			PlayerBase pb = PlayerBase.Cast(m);
			if (!pb || !pb.IsAlive())
				continue;

			if (vector.DistanceSq(pb.GetPosition(), myPos) <= rSq)
				outPlayers.Insert(pb);
		}

		return outPlayers;
	}

	protected PlayerBase GetNearestPlayer(vector fromPos, array<PlayerBase> cached)
	{
		PlayerBase nearest = null;
		float best = 999999999.0;

		if (cached)
		{
			for (int i = 0; i < cached.Count(); i++)
			{
				PlayerBase pb = cached[i];
				if (!pb || !pb.IsAlive())
					continue;

				float d = vector.DistanceSq(pb.GetPosition(), fromPos);
				if (d < best)
				{
					best = d;
					nearest = pb;
				}
			}
		}

		return nearest;
	}

	//=============================
	// HELPERS: SPAWN POSITION
	//=============================
	protected vector FindSpawnPosition(PlayerBase anchor, array<PlayerBase> participants)
	{
		SFP_ScannerEventConfig cfg = GetCfgServer();
		int attempts = 25;
		float spawnMin = 15.0;
		float spawnMax = 70.0;
		float minToPlayer = 12.0;
		if (cfg)
		{
			attempts = cfg.SpawnAttempts;
			spawnMin = cfg.SpawnRadiusMin;
			spawnMax = cfg.SpawnRadiusMax;
			minToPlayer = cfg.SpawnMinDistanceToPlayer;
		}
		if (attempts < 1)
			attempts = 1;
		if (spawnMax < spawnMin)
			spawnMax = spawnMin;

		vector devicePos = GetPosition();
		array<Object> excluded = new array<Object>;
		excluded.Insert(this);

		for (int i = 0; i < attempts; i++)
		{
			vector pos;

			// 1) если есть игроки - пытаемся спавнить ПОЗАДИ случайного игрока (чтобы не перед лицом)
			if (anchor)
			{
				pos = GetSpawnPosBehindPlayer(anchor, Math.RandomFloat(12.0, 25.0));
			}
			else
			{
				pos = GetSpawnPosAroundDevice();
			}

			if (pos == vector.Zero)
				continue;

			// держим радиус вокруг установки
			float distToDevice = vector.Distance(devicePos, pos);
			if (distToDevice < spawnMin || distToDevice > spawnMax)
				continue;

			// не в воде
			if (GetGame().SurfaceIsSea(pos[0], pos[2]) || GetGame().SurfaceIsPond(pos[0], pos[2]))
				continue;

			// не рядом с игроками
			if (participants && participants.Count() > 0)
			{
				if (!IsFarFromPlayers(pos, participants, minToPlayer))
					continue;
			}

			// чуть-чуть проверим, что не в стене
			pos[1] = GetGame().SurfaceY(pos[0], pos[2]);
			if (GetGame().IsBoxCollidingGeometry(pos + Vector(0, 1.0, 0), Vector(0, 0, 0), Vector(1.0, 2.0, 1.0), ObjIntersectView, ObjIntersectGeom, excluded, NULL))
				continue;

			return pos;
		}

		// fallback: хоть куда-то вокруг установки, но тоже не в воду и не в геометрию
		// (1.30 port: the old fallback had no checks and could put the AI straight into a lake or a swamp pond)
		for (int f = 0; f < attempts; f++)
		{
			vector fallback = GetSpawnPosAroundDevice();
			if (fallback == vector.Zero)
				continue;

			fallback[1] = GetGame().SurfaceY(fallback[0], fallback[2]);
			if (!SFP_Safety.IsFreeGroundPos(fallback, Vector(1.0, 2.0, 1.0), excluded))
				continue;

			return fallback;
		}

		return vector.Zero;
	}

	protected vector GetSpawnPosAroundDevice()
	{
		SFP_ScannerEventConfig cfg = GetCfgServer();
		float spawnMin = 15.0;
		float spawnMax = 70.0;
		if (cfg)
		{
			spawnMin = cfg.SpawnRadiusMin;
			spawnMax = cfg.SpawnRadiusMax;
		}
		if (spawnMax < spawnMin)
			spawnMax = spawnMin;

		vector center = GetPosition();
		float angle = Math.RandomFloat(0.0, Math.PI2);
		float radius = Math.RandomFloat(spawnMin, spawnMax);
		vector offset = Vector(Math.Cos(angle) * radius, 0, Math.Sin(angle) * radius);
		vector pos = center + offset;
		pos[1] = GetGame().SurfaceY(pos[0], pos[2]);
		return pos;
	}

	protected vector GetSpawnPosBehindPlayer(PlayerBase player, float distance)
	{
		if (!player)
			return vector.Zero;

		vector pPos = player.GetPosition();
		vector forward = player.GetDirection();
		forward[1] = 0;
		if (forward.LengthSq() < 0.01)
			forward = Vector(0, 0, 1);
		forward.Normalize();

		vector right = Vector(forward[2], 0, -forward[0]);
		right.Normalize();

		vector back = -forward;

		// небольшой разброс по углу, но всё равно "сзади"
		float a = Math.RandomFloat(-75.0, 75.0) * Math.DEG2RAD;
		vector dir = (back * Math.Cos(a)) + (right * Math.Sin(a));
		dir[1] = 0;
		if (dir.LengthSq() < 0.01)
			dir = back;
		dir.Normalize();

		vector pos = pPos + (dir * distance);
		pos[1] = GetGame().SurfaceY(pos[0], pos[2]);
		return pos;
	}

	protected bool IsFarFromPlayers(vector pos, array<PlayerBase> players, float minDist)
	{
		float minSq = minDist * minDist;
		for (int i = 0; i < players.Count(); i++)
		{
			PlayerBase pb = players[i];
			if (!pb || !pb.IsAlive())
				continue;

			if (vector.DistanceSq(pb.GetPosition(), pos) < minSq)
				return false;
		}
		return true;
	}

	//=============================
	// TIMER DISPLAY
	//=============================
	protected void BuildDigitTextureMap()
	{
		m_DigitTextureMap = new map<int, string>;
		m_DigitTextureMap.Insert(0, DIGIT_TEX_0);
		m_DigitTextureMap.Insert(1, DIGIT_TEX_1);
		m_DigitTextureMap.Insert(2, DIGIT_TEX_2);
		m_DigitTextureMap.Insert(3, DIGIT_TEX_3);
		m_DigitTextureMap.Insert(4, DIGIT_TEX_4);
		m_DigitTextureMap.Insert(5, DIGIT_TEX_5);
		m_DigitTextureMap.Insert(6, DIGIT_TEX_6);
		m_DigitTextureMap.Insert(7, DIGIT_TEX_7);
		m_DigitTextureMap.Insert(8, DIGIT_TEX_8);
		m_DigitTextureMap.Insert(9, DIGIT_TEX_9);
	}

	protected void CacheTimerSelectionIndices()
	{
		if (m_TimerSelLeftIdx == INDEX_NOT_FOUND)
			m_TimerSelLeftIdx = GetHiddenSelectionIndex(TIMER_SELECTION_LEFT);

		if (m_TimerSelRightIdx == INDEX_NOT_FOUND)
			m_TimerSelRightIdx = GetHiddenSelectionIndex(TIMER_SELECTION_RIGHT);
	}

	protected void UpdateTimerDisplay(int secondsLeft)
	{
		if (!m_DigitTextureMap)
			BuildDigitTextureMap();

		if (m_TimerSelLeftIdx == INDEX_NOT_FOUND || m_TimerSelRightIdx == INDEX_NOT_FOUND)
			return;

		int s = Math.Clamp(secondsLeft, 0, 99);
		int left = s / 10;
		int right = s % 10;

		if (left != m_LastLeftDigit)
		{
			SetObjectTexture(m_TimerSelLeftIdx, m_DigitTextureMap.Get(left));
			m_LastLeftDigit = left;
		}

		if (right != m_LastRightDigit)
		{
			SetObjectTexture(m_TimerSelRightIdx, m_DigitTextureMap.Get(right));
			m_LastRightDigit = right;
		}
	}
};
