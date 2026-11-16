// «Древний корень»: тревога рощи (ТЗ_сервер_18.md п. 2б.10), подношение (2б.2) и Благословение рощи (2б.3) — только сервер.
//
// Тревога — счётчик у каждой сердцевины. Сбор пыльцы и отлом отростка: +1; пороги ALARM_WAVE1 / ALARM_WAVE2 при переходе
// вверх поднимают 1-ю и 2-ю волну стражей. Вырезание (начало резки): сразу ALARM_MAX и 3-я волна — не больше
// CUT_WAVES_MAX за рестарт и не пока жива прежняя. Тревога спадает на 1 за ALARM_DECAY_MS; спала ниже порога — порог
// снова «заряжен». Подношение сбрасывает тревогу в 0.
// Подношение: дар уходит роще (VB_AR_Offer.VB_Tier — что годится), игроку — дар рощи у ног и Благословение на BLESS_MS.
// Благословение: раны медленно затягиваются, стражи рощ не выбирают игрока целью (PlayerBase.CanBeTargetedByAI).
// Благословение снято, если ударил стража, поднял волну жадным сбором или начал резать сердцевину.
// Расположение леса (2б.11) — VB_AR_FavorServer.c: подношение его поднимает, сбор и резка — опускают.

modded class VB_AR_Heart
{
	static const int ALARM_WAVE1 = 2;
	static const int ALARM_WAVE2 = 4;
	static const int ALARM_MAX = 6;
	static const int ALARM_DECAY_MS = 600000;		// тревога спадает на 1 за 10 минут
	static const int CUT_WAVES_MAX = 2;				// волн вырезания на сердцевину за рестарт
	static const int BLESS_MS = 1800000;			// Благословение — полчаса

	// дары рощи по ценности подношения
	static const string AMBER = "VB_AR_Amber";
	static const string AMBER_INCL = "VB_AR_AmberInclusion";
	static const string POLLEN = "VB_AR_Spores";
	static const float BEST_INCL_CHANCE = 0.30;		// лучший дар: янтарь, пыльца и с шансом янтарь с вкраплением
	static const float MID_AMBER_CHANCE = 0.50;		// средний: пыльца и с шансом янтарь

	protected int m_VB_Alarm;
	protected bool m_VB_AlarmDecaying;
	protected ref VB_AR_Horde m_VB_CutHorde;		// волна вырезания (жива ли — VB_IsOver)
	protected int m_VB_CutWaves;

	int VB_AlarmLevel()
	{
		return m_VB_Alarm;
	}

	// сбор пыльцы, отлом отростка
	void VB_AlarmServer(PlayerBase who, int add)
	{
		if (m_VB_Dead || add <= 0)
			return;

		int before = m_VB_Alarm;
		m_VB_Alarm = Math.Min(ALARM_MAX, m_VB_Alarm + add);
		VB_AlarmDecayStart();
		VB_AlarmLog(who, before);
		VB_AR_FavorServer.VB_Add(who, VB_AR_FavorServer.GATHER * add, "сбор у рощи");
		if (before < ALARM_WAVE1 && m_VB_Alarm >= ALARM_WAVE1)
			VB_GuardWaveServer(1, who);
		if (before < ALARM_WAVE2 && m_VB_Alarm >= ALARM_WAVE2)
			VB_GuardWaveServer(2, who);
	}

	// начали вырезать: тревога — максимум, 3-я волна (не больше CUT_WAVES_MAX и не пока жива прежняя)
	void VB_AlarmCutServer(PlayerBase cutter)
	{
		if (m_VB_Dead)
			return;

		int before = m_VB_Alarm;
		m_VB_Alarm = ALARM_MAX;
		VB_AlarmDecayStart();
		VB_AlarmLog(cutter, before);
		VB_AR_FavorServer.VB_Anger(cutter, "Ты поднял нож на сердцевину рощи.");
		if (m_VB_CutWaves >= CUT_WAVES_MAX)
			return;
		if (m_VB_CutHorde && !m_VB_CutHorde.VB_IsOver())
			return;

		VB_AR_Horde wave = VB_GuardWaveServer(3, cutter);
		if (wave)
		{
			m_VB_CutHorde = wave;
			m_VB_CutWaves++;
		}
	}

	// волна стражей номер level (состав — VB_RootGuards.VB_WaveTypes; ступень рощи — шаг 5, пока «роща»)
	protected VB_AR_Horde VB_GuardWaveServer(int level, PlayerBase who)
	{
		VB_AR_Horde wave = VB_AR_Horde.VB_SpawnWaveTypes(GetPosition(), VB_RootGuards.VB_WaveTypes(level));
		if (!wave)
			return null;		// потолок стражей на сервере или рядом

		string whoStr = VB_AR_ItemsServer.VB_Who(who);
		string at = GetPosition().ToString();
		string lvl = level.ToString();
		Print("[VB_AncientRoot] роща в " + at + ": волна стражей " + lvl + ", виноват " + whoStr);
		// кто поднял волну жадным сбором, того стражи больше не щадят (иначе: подношение — и бери даром)
		VB_AR_FavorServer.VB_Anger(who, "Роща встревожена твоей жадностью.");
		return wave;
	}

	protected void VB_AlarmLog(PlayerBase who, int before)
	{
		string whoStr = VB_AR_ItemsServer.VB_Who(who);
		string at = GetPosition().ToString();
		string b = before.ToString();
		string a = m_VB_Alarm.ToString();
		Print("[VB_AncientRoot] тревога рощи в " + at + ": " + b + " -> " + a + " (" + whoStr + ")");
	}

	protected void VB_AlarmDecayStart()
	{
		if (m_VB_AlarmDecaying)
			return;
		m_VB_AlarmDecaying = true;
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_AlarmDecay, ALARM_DECAY_MS, true);
	}

	void VB_AlarmDecay()
	{
		if (m_VB_Alarm > 0)
			m_VB_Alarm--;
		if (m_VB_Alarm <= 0 || m_VB_Dead)
		{
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_AlarmDecay);
			m_VB_AlarmDecaying = false;
		}
	}

	override protected void VB_Cleanup()
	{
		if (g_Game)
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_AlarmDecay);
		super.VB_Cleanup();
	}

	// подношение item от player; false — роща не приняла (условия проверены и на клиенте, здесь — ещё раз)
	bool VB_OfferServer(PlayerBase player, ItemBase item)
	{
		if (m_VB_Dead || m_VB_OffersLeft <= 0 || !player || !item)
			return false;

		int tier = VB_AR_Offer.VB_Tier(item);
		if (tier == VB_AR_Offer.TIER_NONE)
			return false;
		int favorTier = VB_AR_FavorServer.VB_TierNow(player);
		if (favorTier == VB_AR_Favor.TIER_CURSED)
		{
			if (!VB_AR_FavorServer.VB_HasPardon(player))
			{
				NotificationSystem.SendNotificationToPlayerExtended(player, 6, "Роща не принимает дар", "Лес тебя ненавидит.");
				return false;
			}
			VB_AR_FavorServer.VB_UsePardon(player);		// Отвар примирения: один дар даже «Проклятому»
		}

		string what = item.GetType();
		// штучные стопки (руны, кристаллы) — одна штука, остальное — целиком
		if (!item.IsMeat() && item.ConfigGetBool("canBeSplit") && item.GetQuantity() > 1)
			item.AddQuantity(-1);
		else
			item.DeleteSafe();

		m_VB_OffersLeft--;
		SetSynchDirty();
		int alarmBefore = m_VB_Alarm;
		m_VB_Alarm = 0;

		bool isFriend = favorTier == VB_AR_Favor.TIER_FRIEND;
		string gifts = VB_GiveGiftsServer(player, tier, isFriend);
		int blessMs = BLESS_MS;
		if (isFriend)
			blessMs = BLESS_MS * 2;		// Друг леса — Благословение вдвое дольше
		if (player.VB_AmuletWorks())
			blessMs = blessMs * VB_AR_Amulet.BLESS_MUL;	// Янтарный оберег — ещё в полтора раза
		int blessMin = blessMs / 60000;
		string blessText = "Благословение рощи на " + blessMin.ToString() + " мин: раны затягиваются, стражи не нападают первыми.";
		VB_AR_Blessing.VB_Give(player, blessMs);
		NotificationSystem.SendNotificationToPlayerExtended(player, 7, "Роща приняла дар", blessText);
		float favorAdd = VB_AR_FavorServer.OFFER_MEAT;
		if (tier == VB_AR_Offer.TIER_BEST)
			favorAdd = VB_AR_FavorServer.OFFER_BEST;
		else if (tier == VB_AR_Offer.TIER_MID)
			favorAdd = VB_AR_FavorServer.OFFER_MID;
		VB_AR_FavorServer.VB_Add(player, favorAdd, "подношение роще");

		string who = VB_AR_ItemsServer.VB_Who(player);
		string at = GetPosition().ToString();
		string t = tier.ToString();
		string left = m_VB_OffersLeft.ToString();
		string ab = alarmBefore.ToString();
		string msg = "[VB_AncientRoot] подношение в " + at + ": " + who + " отдал " + what + " (ценность " + t + "), дар: " + gifts;
		msg += "; тревога " + ab + " -> 0, подношений осталось " + left;
		Print("" + msg);
		return true;
	}

	protected string VB_GiveGiftsServer(PlayerBase player, int tier, bool isFriend)
	{
		TStringArray drops = new TStringArray;
		if (isFriend)
			drops.Insert(POLLEN);		// Друг леса — дар щедрее
		if (tier == VB_AR_Offer.TIER_BEST)
		{
			drops.Insert(AMBER);
			drops.Insert(POLLEN);
			if (Math.RandomFloat01() < BEST_INCL_CHANCE)
				drops.Insert(AMBER_INCL);
		}
		else if (tier == VB_AR_Offer.TIER_MID)
		{
			drops.Insert(POLLEN);
			if (Math.RandomFloat01() < MID_AMBER_CHANCE)
				drops.Insert(AMBER);
		}
		else
		{
			drops.Insert(POLLEN);
		}

		string got = "";
		foreach (string type : drops)
		{
			if (VB_AR_ItemsServer.VB_DropAtFeet(player, type))
			{
				if (got != "")
					got += ", ";
				got += type;
			}
		}
		if (got == "")
			got = "ничего";
		return got;
	}
}

// Благословение рощи: полчаса медленного лечения, стражи не выбирают целью. Хранится в памяти сервера (до рестарта),
// по SteamID: перезаход не снимает.
class VB_AR_Blessing
{
	static const int TICK_MS = 10000;
	static const float HEAL_HEALTH = 0.5;	// за тик: ~90 здоровья за полчаса
	static const float HEAL_BLOOD = 20.0;	// за тик: ~3600 крови за полчаса

	protected static ref map<string, int> s_VB_Until;		// SteamID -> g_Game.GetTime(), до которого действует
	protected static bool s_VB_Ticking;

	protected static string VB_Id(PlayerBase p)
	{
		if (!p || !p.GetIdentity())
			return "";
		return p.GetIdentity().GetPlainId();
	}

	static void VB_Give(PlayerBase p, int ms)
	{
		string id = VB_Id(p);
		if (id == "")
			return;
		if (!s_VB_Until)
			s_VB_Until = new map<string, int>;
		s_VB_Until.Set(id, g_Game.GetTime() + ms);
		if (!s_VB_Ticking)
		{
			s_VB_Ticking = true;
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_Tick, TICK_MS, true);
		}
	}

	static bool VB_Has(PlayerBase p)
	{
		if (!s_VB_Until || s_VB_Until.Count() == 0)
			return false;
		string id = VB_Id(p);
		if (id == "" || !s_VB_Until.Contains(id))
			return false;
		return s_VB_Until.Get(id) > g_Game.GetTime();
	}

	// снять (ударил стража)
	static void VB_Break(PlayerBase p, string why)
	{
		if (!VB_Has(p))
			return;
		s_VB_Until.Remove(VB_Id(p));
		NotificationSystem.SendNotificationToPlayerExtended(p, 6, "Благословение рощи снято", why);
		Print("[VB_AncientRoot] Благословение снято: " + VB_AR_ItemsServer.VB_Who(p) + " — " + why);
	}

	static void VB_Tick()
	{
		if (!s_VB_Until || s_VB_Until.Count() == 0)
		{
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_Tick);
			s_VB_Ticking = false;
			return;
		}

		int now = g_Game.GetTime();
		array<Man> players = new array<Man>;
		g_Game.GetPlayers(players);
		PlayerBase p;
		string id;
		foreach (Man m : players)
		{
			p = PlayerBase.Cast(m);
			id = VB_Id(p);
			if (id == "" || !s_VB_Until.Contains(id) || !p.IsAlive())
				continue;
			if (s_VB_Until.Get(id) <= now)
			{
				s_VB_Until.Remove(id);
				NotificationSystem.SendNotificationToPlayerExtended(p, 5, "Благословение рощи прошло", "");
				continue;
			}
			p.AddHealth("", "Health", HEAL_HEALTH);
			p.AddHealth("", "Blood", HEAL_BLOOD);
		}

		// истёкшие у тех, кто вышел
		TStringArray gone = new TStringArray;
		foreach (string key, int until : s_VB_Until)
		{
			if (until <= now)
				gone.Insert(key);
		}
		foreach (string g : gone)
			s_VB_Until.Remove(g);
	}
}

modded class PlayerBase
{
	// стражи рощ не нападают первыми на благословлённого и на «Своего»/«Друга леса» (VB_AR_FavorServer.VB_Peaceful)
	override bool CanBeTargetedByAI(EntityAI ai)
	{
		if (!super.CanBeTargetedByAI(ai))
			return false;
		if (VB_AR_Guard.VB_Is(ai) && VB_AR_FavorServer.VB_Peaceful(this))
			return false;
		return true;
	}
}

// ударил стража — Благословение снято (волки и Заноза — животные, Хранитель — по классу зомби)
class VB_AR_GuardHit
{
	static void VB_OnHit(EntityAI guard, EntityAI source)
	{
		if (!source || !VB_AR_Guard.VB_Is(guard))
			return;
		PlayerBase p = PlayerBase.Cast(source);
		if (!p)
			p = PlayerBase.Cast(source.GetHierarchyRootPlayer());
		if (p)
			VB_AR_FavorServer.VB_Anger(p, "Ты поднял руку на стража рощи.");
	}

	// страж убит игроком: расположение −3, Хранитель −10
	static void VB_OnKilled(EntityAI guard, Object killer)
	{
		if (!killer || !VB_AR_Guard.VB_Is(guard))
			return;
		PlayerBase p = PlayerBase.Cast(killer);
		EntityAI ek = EntityAI.Cast(killer);
		if (!p && ek)
			p = PlayerBase.Cast(ek.GetHierarchyRootPlayer());
		if (!p)
			return;
		float d = VB_AR_FavorServer.KILL_GUARD;
		if (guard.IsKindOf(VB_RootGuards.ENT))
			d = VB_AR_FavorServer.KILL_KEEPER;
		VB_AR_FavorServer.VB_Add(p, d, "убил стража " + guard.GetType());
	}
}

modded class AnimalBase
{
	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
		VB_AR_GuardHit.VB_OnHit(this, source);
	}

	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);
		VB_AR_GuardHit.VB_OnKilled(this, killer);
		VB_AR_FavorServer.VB_OnCreatureKilled(this, killer);		// пауки MSFZ — изгнание нечисти
	}
}

modded class ZombieBase
{
	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
		VB_AR_GuardHit.VB_OnHit(this, source);
	}

	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);
		VB_AR_GuardHit.VB_OnKilled(this, killer);
		VB_AR_FavorServer.VB_OnCreatureKilled(this, killer);		// ERA-зомби — изгнание нечисти
	}
}

modded class VB_ActionDrinkReconcile
{
	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);
		PlayerBase p = action_data.m_Player;
		ItemBase item = action_data.m_MainItem;
		if (!p || !item || !VB_AR_Recovery.VB_LiveHeartNear(p.GetPosition(), VB_AR_Recovery.GROVE_NEAR))
			return;
		if (VB_AR_FavorServer.VB_Reconcile(p))
			item.DeleteSafe();
	}
}

modded class VB_ActionPlantSeed
{
	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);
		PlayerBase p = action_data.m_Player;
		ItemBase item = action_data.m_MainItem;
		if (!p || !item)
			return;
		VB_AR_Heart heart = VB_AR_Recovery.VB_DeadHeartNear(p.GetPosition(), VB_AR_Recovery.SEED_NEAR);
		if (!heart)
			return;
		if (!VB_AR_Nests.VB_OnSeedPlanted(heart))
		{
			NotificationSystem.SendNotificationToPlayerExtended(p, 6, "Семя не прижилось", "Здесь уже посажено семя.");
			return;
		}
		item.DeleteSafe();
		VB_AR_FavorServer.VB_Add(p, VB_AR_FavorServer.SEED, "посадил Семя Древнего корня");
		NotificationSystem.SendNotificationToPlayerExtended(p, 7, "Семя посажено", "К следующему рестарту здесь прорастёт новая роща.");
	}
}

modded class VB_ActionOfferRoot
{
	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);

		VB_AR_Heart heart = VB_AR_Heart.Cast(action_data.m_Target.GetObject());
		if (heart)
			heart.VB_OfferServer(action_data.m_Player, action_data.m_MainItem);
	}
}
