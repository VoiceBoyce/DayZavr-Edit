// Изделия «Древнего корня»: Корень-вещун, Пыльцевая ловушка, Светоч корня, Живица, Терновый клинок, Плащ лешего
// (крафт — в DRL 18, в самом конце работ). Аккумулятор, канистра, жгут и нить убраны (ТЗ 2б.5).
// Конфиги — config.cpp (раздел «изделия»), модели — tools/gen_ar_veins.py, перекраски — data/retex.
// Здесь клиентская часть: классы, поля, синхронизация, сохранение, условия действий, эффекты, приём RPC.
// Серверная часть (таймеры, заряды, вызов стражей, Живица, клинок, плащ, логи) — VB_AncientRoot_Server,
// VB_AR_GearServer.c; сон и терновая рана существ — VB_AR_CreatureServer.c.

class VB_AR_Gear
{
	static const int RPC_LISTEN = 1447186012;	// сервер -> клиент: результат «прислушаться»
	static const int RPC_BURST = 1447186013;	// сервер -> клиенты рядом: вспышка пыльцы в точке
	static const float BURST_FX_DIST = 200.0;
	static const string BURST_SOUND = "VB_AR_Burst_SoundSet";

	// клиент: облако золотой пыльцы и хлопок
	static void VB_BurstFxClient(vector pos)
	{
		ParticleManager pm = ParticleManager.GetInstance();
		if (pm)
			pm.PlayInWorld(ParticleList.VB_AR_BURST, pos + "0 0.6 0");

		EffectSound snd = SEffectManager.PlaySound(BURST_SOUND, pos);
		if (snd)
			snd.SetAutodestroy(true);
	}

	// где игрок держит предмет: в руках у этого игрока
	static bool VB_InHandsOf(EntityAI item, PlayerBase player)
	{
		return player && player.GetHumanInventory().GetEntityInHands() == item;
	}
}

// ============================================================================================
// «Корень-вещун» (ТЗ 2б.5): «прислушаться» в любое время.
//   Ночью (1 заряд) сервер ищет ближайшую живую сердцевину в RANGE и отдаёт только направление (по сектору
//   DIR_SECTOR_DEG) и ступень расстояния (DIST_STEP), не координаты. Клиент слышит серию ударов с той стороны:
//   ближе — громче и чаще, если сердцевину режут — бешеный стук. Днём сердцевины молчат, заряд не тратится.
//   Всегда: гроза (облачность сейчас или прогноз не ниже STORM_OVERCAST — порог молний Ливонии) и что лес думает
//   об игроке (ступень расположения, VB_AR_Favor).
// Подзарядка — у живой сердцевины ночью (VB_ActionFeedListenRoot): заряды до полного, сердцевина тревожится.

class VB_AR_ListenRoot extends Inventory_Base
{
	static const float RANGE = 3000.0;
	static const float DIST_STEP = 250.0;		// м: ступень расстояния в ответе сервера
	static const float DIR_SECTOR_DEG = 10.0;	// направление округляется до сектора
	static const float STORM_OVERCAST = 0.85;	// порог молний на Ливонии (ваниль: WorldData m_StormThreshold, Enoch не меняет)
	static const int STORM_NONE = 0;
	static const int STORM_COMING = 1;			// прогноз облачности дошёл до порога — гроза идёт
	static const int STORM_NOW = 2;				// облачность уже выше порога

	override void SetActions()
	{
		super.SetActions();
		AddAction(VB_ActionListenRoot);
		AddAction(VB_ActionFeedListenRoot);
	}
}

// клиент: серия ударов в стороне сердца
class VB_AR_Listen
{
	static const string SOUND = "VB_AR_Listen_SoundSet";
	static const float SOUND_DIST = 6.0;	// м от игрока в сторону сердца
	static const int TOTAL_MS = 7000;

	protected static ref VB_AR_Listen s_VB_Cur;

	protected vector m_VB_Dir;
	protected float m_VB_Volume;
	protected int m_VB_Left;

	static void VB_Play(bool night, bool found, vector dir, int step, bool frantic, int storm, int stage)
	{
		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		if (!player)
			return;

		if (!night)
			player.MessageStatus("Днём сердцевины спят: корень-вещун их не слышит.");
		else if (!found)
			player.MessageStatus("Корень-вещун молчит: живых сердцевин поблизости нет.");
		else
		{
			if (s_VB_Cur)
				s_VB_Cur.VB_Stop();
			s_VB_Cur = new VB_AR_Listen(dir, step, frantic);
			player.MessageStatus(VB_StageWords(stage));
		}

		if (storm == VB_AR_ListenRoot.STORM_NOW)
			player.MessageStatus("Корень дрожит: гроза уже над лесом.");
		else if (storm == VB_AR_ListenRoot.STORM_COMING)
			player.MessageStatus("Корень гудит: к лесу идёт гроза.");

		player.MessageStatus(VB_ForestMood());
	}

	// какая сердцевина отозвалась — по ступени рощи (ТЗ 2б.4)
	static string VB_StageWords(int stage)
	{
		if (stage <= VB_AR_Heart.STAGE_SPROUT)
			return "Корень тихо поёт: где-то проросла молодая сердцевина.";
		if (stage >= VB_AR_Heart.STAGE_ANCIENT)
			return "Корень гудит: отозвалась древняя сердцевина.";
		return "Корень поёт: бьётся живая сердцевина.";
	}

	// что лес думает об игроке — по ступени расположения (ТЗ 2б.11)
	static string VB_ForestMood()
	{
		int tier = VB_AR_Favor.TIER_STRANGER;
		if (VB_AR_Favor.s_VB_Known)
			tier = VB_AR_Favor.VB_MyTier();
		if (tier == VB_AR_Favor.TIER_FRIEND)
			return "Лес поёт тебе навстречу.";
		if (tier == VB_AR_Favor.TIER_OWN)
			return "Лес тебя знает.";
		if (tier == VB_AR_Favor.TIER_ENEMY)
			return "Лес насторожен.";
		if (tier == VB_AR_Favor.TIER_CURSED)
			return "Лес ненавидит тебя.";
		return "Лес молчит.";
	}

	void VB_AR_Listen(vector dir, int step, bool frantic)
	{
		vector d = dir;
		d[1] = 0;
		d.Normalize();
		m_VB_Dir = d;

		// середина ступени: 0 — до 250 м, 1 — 250–500 м и т. д.
		float dist = (step + 0.5) * VB_AR_ListenRoot.DIST_STEP;
		float k = Math.Clamp(dist / VB_AR_ListenRoot.RANGE, 0.0, 1.0);
		m_VB_Volume = Math.Lerp(1.0, 0.3, k);
		int intervalMs = Math.Lerp(650, 2000, k);
		if (frantic)
		{
			m_VB_Volume = 1.0;
			intervalMs = 420;
		}
		m_VB_Left = TOTAL_MS / intervalMs;
		VB_Beat();
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_Beat, intervalMs, true);
	}

	void VB_Stop()
	{
		m_VB_Left = 0;
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_Beat);
	}

	void VB_Beat()
	{
		Man player = g_Game.GetPlayer();
		if (m_VB_Left <= 0 || !player)
		{
			VB_Stop();
			return;
		}
		m_VB_Left--;

		vector pos = player.GetPosition() + m_VB_Dir * SOUND_DIST + "0 1.5 0";
		EffectSound snd = SEffectManager.CreateSound(SOUND, pos);
		if (!snd)
			return;
		snd.SetSoundVolume(m_VB_Volume);
		snd.SetAutodestroy(true);
		snd.SoundPlay();
	}
}

class VB_ActionListenRoot : ActionSingleUseBase
{
	void VB_ActionListenRoot()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_ITEM_ON;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_ITEM_ON;
		m_Text = "Прислушаться";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINonRuined();
		m_ConditionTarget = new CCTNone();
	}

	override bool HasTarget()
	{
		return false;
	}

	// днём — всегда (заряд не тратится), ночью — пока есть заряды
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (VB_AR_Pulse.VB_NightFactor() < VB_AR_Pulse.NIGHT_LIGHT_ABOVE)
			return true;
		return item.GetQuantity() > 0;
	}
}

class VB_ActionFeedListenRootCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(VB_ActionFeedListenRoot.FEED_TIME);
	}
}

class VB_ActionFeedListenRoot : ActionContinuousBase
{
	static const float FEED_TIME = 5.0;

	void VB_ActionFeedListenRoot()
	{
		m_CallbackClass = VB_ActionFeedListenRootCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_FullBody = true;
		m_Text = "Подпитать корень";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINonRuined();
		m_ConditionTarget = new CCTCursor(UAMaxDistances.DEFAULT);
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		VB_AR_Heart heart = VB_AR_Heart.Cast(target.GetObject());
		if (!heart || heart.VB_IsDead() || item.GetQuantity() >= item.GetQuantityMax())
			return false;

		return VB_AR_Pulse.VB_NightFactor() >= VB_AR_Pulse.NIGHT_FULL_ABOVE;
	}
}

// ============================================================================================
// «Пыльцевая ловушка» (ТЗ 2б.5): «Раскрыть» в руках, потом бросить. Облако пыльцы шумит (нечисть и звери идут),
// через ARM_MS вспыхивает: все живые зомби и звери в BURST_RADIUS (стражи и мутанты тоже, кроме крупных — здоровье
// больше 1000: Хранитель, боссы) засыпают на SLEEP_MS — ИИ стоит на месте. Удар будит того, кого ударили. Людей не
// трогает. Сон — VB_AR_CreatureServer.c. Не заменилась после вспышки — сервер через секунду удаляет ловушку.

class VB_AR_SporeTrap extends Inventory_Base
{
	static const int ARM_MS = 10000;
	static const int NOISE_MS = 2000;
	static const string NOISE = "VB_AR_TrapNoise";
	static const float BURST_RADIUS = 8.0;
	static const int SLEEP_MS = 20000;

	protected bool m_VB_Armed;				// синхронизируется: облако спор над ловушкой
	protected ParticleSource m_VB_Cloud;

	void VB_AR_SporeTrap()
	{
		RegisterNetSyncVariableBool("m_VB_Armed");
	}

	override void SetActions()
	{
		super.SetActions();
		AddAction(VB_ActionArmSporeTrap);
	}

	bool VB_IsArmed()
	{
		return m_VB_Armed;
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		VB_UpdateCloud();
	}

	override void EEInit()
	{
		super.EEInit();
		if (!g_Game.IsDedicatedServer())
			VB_UpdateCloud();
	}

	override void EEDelete(EntityAI parent)
	{
		if (m_VB_Cloud)
		{
			m_VB_Cloud.StopParticle();
			m_VB_Cloud = null;
		}
		super.EEDelete(parent);
	}

	protected void VB_UpdateCloud()
	{
		if (m_VB_Armed && !m_VB_Cloud)
		{
			ParticleManager pm = ParticleManager.GetInstance();
			if (pm)
				m_VB_Cloud = pm.PlayOnObject(ParticleList.VB_AR_SPORES, this, "0 0.05 0", "0 0 0", true);
		}
	}
}

class VB_ActionArmSporeTrap : ActionSingleUseBase
{
	void VB_ActionArmSporeTrap()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_ITEM_ON;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_ITEM_ON;
		m_Text = "Раскрыть ловушку";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINonRuined();
		m_ConditionTarget = new CCTNone();
	}

	override bool HasTarget()
	{
		return false;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		VB_AR_SporeTrap trap = VB_AR_SporeTrap.Cast(item);
		return trap && !trap.VB_IsArmed();
	}
}

// ============================================================================================
// «Светоч корня» (ТЗ 2б.5): горит золотым без батареек (в руках или на земле), количество = минуты пыльцы — тратятся,
// только пока светоч на виду (в рюкзаке не тратятся). Клиент: пульс света, вдвое-втрое чаще, если живой зомби ближе
// ALARM_DIST; такт идёт только у горящего светоча. Сервер: обычные заражённые (ванильные Zmb*) не трогают игроков в
// SCARE_DIST от горящего светоча (CanBeTargetedByAI; стражей, боссов и мутантов это не касается); ночью раз в CALL_MS
// с шансом CALL_CHANCE стражи видят свет издалека — выходит Лесовой волк (без лута), если у светоча есть игрок.

class VB_AR_LanternLight extends PointLightBase
{
	void VB_AR_LanternLight()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(8.0);
		SetBrightnessTo(0.8);
		SetCastShadow(false);
		SetFlareVisible(false);
		SetAmbientColor(1.0, 0.55, 0.15);
		SetDiffuseColor(1.0, 0.55, 0.15);
	}
}

class VB_AR_Lantern extends Inventory_Base
{
	static const int MINUTE_MS = 60000;
	static const int BURN_TICK_MS = 10000;		// сервер: раз в столько проверяем, на виду ли светоч (расход пыльцы)
	static const int CALL_MS = 180000;
	static const float CALL_CHANCE = 0.45;
	static const string CALL_NOISE = "VB_AR_ChunkNoise";
	static const float CALL_DIST_MIN = 60.0;	// стражи выходят на свет издалека
	static const float CALL_DIST_MAX = 90.0;
	static const float CALL_PLAYER_DIST = 150.0;	// страж выходит, только если у светоча есть живой игрок
	static const float SCARE_DIST = 12.0;		// обычные зомби не трогают игроков ближе к горящему светочу
	static const float ALARM_DIST = 30.0;
	static const int TICK_MS = 50;
	static const float BEAT_PERIOD = 1.6;

	protected bool m_VB_On;					// синхронизируется, в сохранении (формат прежний: один bool)
	protected VB_AR_LanternLight m_VB_Light;
	protected bool m_VB_Ticking;			// клиент: идёт такт света
	protected float m_VB_Phase;
	protected float m_VB_Rate = 1.0;
	protected int m_VB_NearCheckMs;
	protected bool m_VB_Alarm;

	void VB_AR_Lantern()
	{
		RegisterNetSyncVariableBool("m_VB_On");
	}

	override void SetActions()
	{
		super.SetActions();
		AddAction(VB_ActionLanternOn);
		AddAction(VB_ActionLanternOff);
	}

	bool VB_IsOn()
	{
		return m_VB_On;
	}

	// светит в руках или на земле (в рюкзаке, на поясе, в ящике — нет)
	bool VB_IsShown()
	{
		EntityAI parent = GetHierarchyParent();
		if (!parent)
			return true;

		return VB_AR_Gear.VB_InHandsOf(this, PlayerBase.Cast(parent));
	}

	// Формат сохранения не меняли: первым полем стоит bool, а маркер версии — int; прочитать int на месте bool
	// безопасно нельзя (см. отчёт 2026-11-13). Новые поля фонарю — только новым классом или отдельным решением.
	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);
		ctx.Write(m_VB_On);
	}

	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
			return false;

		if (!ctx.Read(m_VB_On))
			return false;

		return true;
	}

	override void EEInit()
	{
		super.EEInit();
		VB_UpdateTickClient();
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		VB_UpdateTickClient();
	}

	override void EEDelete(EntityAI parent)
	{
		if (g_Game)
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_TickClient);
		m_VB_Ticking = false;
		VB_SetLight(false);
		super.EEDelete(parent);
	}

	// клиент: такт света только у горящего фонаря (выключенный не тикает каждые 50 мс)
	protected void VB_UpdateTickClient()
	{
		if (g_Game.IsDedicatedServer() || m_VB_On == m_VB_Ticking)
			return;

		m_VB_Ticking = m_VB_On;
		if (m_VB_On)
		{
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_TickClient, TICK_MS, true);
		}
		else
		{
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_TickClient);
			VB_SetLight(false);
		}
	}

	protected void VB_SetLight(bool on)
	{
		if (on && !m_VB_Light)
		{
			m_VB_Light = VB_AR_LanternLight.Cast(ScriptedLightBase.CreateLight(VB_AR_LanternLight, GetPosition()));
			if (m_VB_Light)
				m_VB_Light.AttachOnObject(this, "0 0.158 0");	// модель поднята на 0,038 под хват газовой лампы (HAND_XF)
		}
		else if (!on && m_VB_Light)
		{
			m_VB_Light.Destroy();
			m_VB_Light = null;
		}
	}

	void VB_TickClient()
	{
		VB_SetLight(m_VB_On && VB_IsShown());
		if (!m_VB_Light)
			return;

		int nowMs = g_Game.GetTime();
		if (nowMs - m_VB_NearCheckMs > 1000)
		{
			m_VB_NearCheckMs = nowMs;
			m_VB_Alarm = VB_ZombieNear();
		}

		float target = 1.0;
		if (m_VB_Alarm)
			target = 2.8;
		m_VB_Rate += (target - m_VB_Rate) * 0.08;
		m_VB_Phase = Math.ModFloat(m_VB_Phase + TICK_MS * 0.001 * m_VB_Rate, BEAT_PERIOD);
		// два удара — в локальные: два вызова одной функции в аргументах затирают результат (E71)
		float beat1 = VB_AR_Pulse.VB_Beat(m_VB_Phase, 0.1);
		float beat2 = VB_AR_Pulse.VB_Beat(m_VB_Phase, 0.45);
		float k = Math.Clamp(beat1 + 0.65 * beat2, 0.0, 1.0);
		m_VB_Light.SetBrightnessTo(0.55 + 1.5 * k);
		m_VB_Light.SetRadiusTo(6.5 + 2.5 * k);
	}

	protected bool VB_ZombieNear()
	{
		array<Object> objs = new array<Object>;
		g_Game.GetObjectsAtPosition(VB_AR_Items.VB_WorldPos(this), ALARM_DIST, objs, null);
		ZombieBase zmb;
		foreach (Object o : objs)
		{
			zmb = ZombieBase.Cast(o);
			if (zmb && zmb.IsAlive())
				return true;
		}
		return false;
	}
}

class VB_ActionLanternOn : ActionSingleUseBase
{
	void VB_ActionLanternOn()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_ITEM_ON;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_ITEM_ON;
		m_Text = "Разжечь светоч";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINonRuined();
		m_ConditionTarget = new CCTNone();
	}

	override bool HasTarget()
	{
		return false;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		VB_AR_Lantern lantern = VB_AR_Lantern.Cast(item);
		return lantern && !lantern.VB_IsOn() && lantern.GetQuantity() > 0;
	}
}

// «Усыпить» доступно и сломанному фонарю (сервер гасит сломанный и сам, но действие не должно пропадать)
class VB_ActionLanternOff : VB_ActionLanternOn
{
	void VB_ActionLanternOff()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_ITEM_OFF;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_ITEM_OFF;
		m_Text = "Погасить светоч";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone();
		m_ConditionTarget = new CCTNone();
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		VB_AR_Lantern lantern = VB_AR_Lantern.Cast(item);
		return lantern && lantern.VB_IsOn();
	}
}

// ============================================================================================
// «Живица» (ТЗ 2б.5): ванильный укол (себе или другому). Кровь и здоровье сразу (сервер, OnApply). Потом BLOOD_MS
// у игрока синхронизируемый флаг: руки дрожат при прицеливании (тряска как от холода), а перелом сам срастается —
// раз в HEAL_TICK_MS каждой зоне ног прибавляется LEG_HEAL здоровья; ваниль (BrokenLegsMdfr) снимает перелом,
// когда обе ноги здоровы (100). С шиной быстрее: ванильное BoneRegen работает тоже. Логика — VB_AR_GearServer.c.
// Подсветка нечисти, что была у «Крови Изнанки», уходит в «Настой пыльцы» (новое изделие).

class VB_AR_Blood extends Inventory_Base
{
	static const float BLOOD_ADD = 1500.0;
	static const float HEALTH_ADD = 40.0;
	static const int BLOOD_MS = 600000;
	static const int HEAL_TICK_MS = 10000;
	static const float LEG_HEAL = 3.0;		// за такт каждой зоне ног: от 0 до 100 — около 5,5 мин

	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionInjectTarget);
		AddAction(ActionInjectSelf);
	}
}

// ============================================================================================
// «Терновый клинок» (ТЗ 2б.5): удар по зверю или нечисти (любой DayZCreatureAI: зомби, звери, стражи, мутанты MSFZ)
// оставляет терновую рану — BLEED_MS цель теряет BLEED_DPS здоровья в секунду и идёт шагом. Убийство (и раной
// тоже) — владельцу +BLOOD_PER_KILL крови, клинку лишний износ, голод сбрасывается.
// Голод копится, только пока клинок у игрока в игре: после HUNGER_AFTER_S такой игры без убийств раз в минуту
// забирает HUNGER_BLOOD крови, но не ниже BLOOD_FLOOR. Голод — в сохранении.
// Логика — сервер: голод в VB_AR_GearServer.c, рана и кормление в VB_AR_CreatureServer.c.

class VB_AR_Blade extends FangeKnife
{
	static const float BLOOD_PER_KILL = 150.0;
	static const float EXTRA_WEAR = 1.0;
	static const int BLEED_MS = 8000;
	static const float BLEED_DPS = 4.0;
	static const int HUNGER_AFTER_S = 3600;
	static const int HUNGER_TICK_MS = 60000;
	static const float HUNGER_BLOOD = 60.0;
	static const float BLOOD_FLOOR = 3000.0;	// ниже голод кровь не опускает: клинок не убивает
	static const int HUNGER_MSG_S = 600;
	static const int STORE_VER = 1;

	protected int m_VB_Hunger;				// сервер: секунд игры с клинком без убийств, в сохранении

	// формат 1: STORE_MARK, версия, голод (секунды). Мод ещё нигде не стоял — старых форматов нет.
	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);
		int mark = VB_AR_Items.STORE_MARK;
		int ver = STORE_VER;
		ctx.Write(mark);
		ctx.Write(ver);
		ctx.Write(m_VB_Hunger);
	}

	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
			return false;

		int mark;
		int ver;
		int hunger;
		if (!ctx.Read(mark) || mark != VB_AR_Items.STORE_MARK)
			return false;
		if (!ctx.Read(ver) || ver < 1 || ver > STORE_VER)
			return false;
		if (!ctx.Read(hunger))
			return false;

		m_VB_Hunger = Math.Max(hunger, 0);
		return true;
	}
}

// ============================================================================================
// «Янтарный оберег» (ТЗ 2б.5): лежит где угодно в инвентаре игрока — стражи рощ не нападают на него первыми,
// Благословение рощи на 50 % дольше. Тому, кто в этот рестарт вырезал сердцевину, не помогает. Сломанный не работает.
// Всё считает сервер (VB_AR_GearServer.c: PlayerBase.VB_AmuletWorks, раз в AMULET_CHECK_MS перебор инвентаря).

class VB_AR_Amulet extends Inventory_Base
{
	static const float BLESS_MUL = 1.5;
	static const int AMULET_CHECK_MS = 5000;
}

// ============================================================================================
// «Настой пыльцы» (ТЗ 2б.5): выпить (VB_ActionDrinkPollenBrew) — BREW_MS у игрока синхронизируемый флаг, и у него на
// клиенте живые звери (в т. ч. мутанты MSFZ) и стражи рощи в DIST подсвечены золотым светом (VB_AR_BrewFx: лампа на
// существе, MAX_LIT ближайших, стражи первыми). Материалы существ не трогаем: у них материал ставит healthLevels (E77).

class VB_AR_PollenBrew extends Inventory_Base
{
	static const int BREW_MS = 600000;

	override void SetActions()
	{
		super.SetActions();
		AddAction(VB_ActionDrinkPollenBrew);
	}
}

class VB_ActionDrinkPollenBrewCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(VB_ActionDrinkPollenBrew.DRINK_TIME);
	}
}

class VB_ActionDrinkPollenBrew : ActionContinuousBase
{
	static const float DRINK_TIME = 3.0;

	void VB_ActionDrinkPollenBrew()
	{
		m_CallbackClass = VB_ActionDrinkPollenBrewCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_DRINK;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_DRINK;
		m_Text = "Выпить настой";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINonRuined();
		m_ConditionTarget = new CCTNone();
	}

	override bool HasTarget()
	{
		return false;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		return VB_AR_PollenBrew.Cast(item) != null;
	}
}

class VB_AR_BrewLight extends PointLightBase
{
	void VB_AR_BrewLight()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(3.0);
		SetBrightnessTo(1.4);
		SetCastShadow(false);
		SetFlareVisible(false);
		SetAmbientColor(1.0, 0.62, 0.18);
		SetDiffuseColor(1.0, 0.62, 0.18);
	}
}

// Клиент: подсветка у своего игрока. Каждый такт (SCAN_MS) проверяет, что у ТЕКУЩЕГО локального персонажа флаг ещё
// стоит: после смерти, возрождения или перезахода эффект гаснет сам (как было у «Крови Изнанки», баг №27) — и гасит
// умерших и ушедших. Полный перебор объектов в DIST — реже (FULL_SCAN_MS): горят MAX_LIT ближайших, стражи рощи
// первыми, лишние гаснут. Уже горящих держим до DIST * KEEP_K, чтобы свет не мигал на границе.
class VB_AR_BrewFx
{
	static const float DIST = 60.0;
	static const float KEEP_K = 1.1;			// горящий гаснет дальше DIST * KEEP_K
	static const int SCAN_MS = 1000;
	static const int FULL_SCAN_MS = 2500;
	static const int MAX_LIT = 8;
	static const float GUARD_FIRST = 1000000.0;	// к квадрату расстояния не-стража: стражи всегда ближе в очереди

	protected static ref VB_AR_BrewFx s_VB_Inst;

	protected ref array<EntityAI> m_VB_Lit;
	protected ref array<VB_AR_BrewLight> m_VB_Lights;
	protected bool m_VB_Halted;			// остановлен изнутри своего такта, ждёт замены/удаления снаружи
	protected int m_VB_FullScanMs = -100000;	// когда был полный перебор

	static void VB_Set(bool on)
	{
		if (on)
		{
			if (!s_VB_Inst || s_VB_Inst.m_VB_Halted)
				s_VB_Inst = new VB_AR_BrewFx();
		}
		else if (s_VB_Inst)
		{
			if (!s_VB_Inst.m_VB_Halted)
				s_VB_Inst.VB_Stop();
			s_VB_Inst = null;
		}
	}

	static bool VB_IsRunning()
	{
		return s_VB_Inst && !s_VB_Inst.m_VB_Halted;
	}

	// кого подсвечивать: живые звери (волки, олени, мутанты-звери MSFZ) и стражи рощи (Хранитель — по классу зомби)
	static bool VB_Wants(Object o)
	{
		EntityAI e = EntityAI.Cast(o);
		if (!e || !e.IsAlive())
			return false;
		if (AnimalBase.Cast(e))
			return true;
		return ZombieBase.Cast(e) && VB_AR_Guard.VB_Is(e);
	}

	void VB_AR_BrewFx()
	{
		m_VB_Lit = new array<EntityAI>;
		m_VB_Lights = new array<VB_AR_BrewLight>;
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_Scan, SCAN_MS, true);
		VB_Scan();
	}

	void VB_Stop()
	{
		m_VB_Halted = true;
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_Scan);
		for (int i = m_VB_Lit.Count() - 1; i >= 0; i--)
			VB_Unlit(i);
	}

	protected bool VB_StillOn()
	{
		PlayerBase me = PlayerBase.Cast(g_Game.GetPlayer());
		if (me && me.IsAlive() && me.VB_HasBrew())
			return true;

		VB_Stop();
		return false;
	}

	protected void VB_Unlit(int i)
	{
		VB_AR_BrewLight l = m_VB_Lights[i];
		if (l)
			l.Destroy();
		m_VB_Lit.Remove(i);
		m_VB_Lights.Remove(i);
	}

	void VB_Scan()
	{
		if (!VB_StillOn())
			return;

		vector ppos = g_Game.GetPlayer().GetPosition();
		float keepDist = DIST * KEEP_K;
		float keepSq = keepDist * keepDist;
		for (int i = m_VB_Lit.Count() - 1; i >= 0; i--)
		{
			if (!m_VB_Lit[i] || !m_VB_Lit[i].IsAlive() || vector.DistanceSq(ppos, m_VB_Lit[i].GetPosition()) > keepSq)
				VB_Unlit(i);
		}

		int now = g_Game.GetTime();
		if (now - m_VB_FullScanMs < FULL_SCAN_MS)
			return;
		m_VB_FullScanMs = now;

		// кандидаты: новые — в DIST, горящие — в DIST * KEEP_K; ключ — квадрат расстояния (+GUARD_FIRST не-стражам)
		array<Object> objs = new array<Object>;
		g_Game.GetObjectsAtPosition(ppos, keepDist, objs, null);
		array<EntityAI> cands = new array<EntityAI>;
		array<float> keys = new array<float>;
		float distSq;
		float maxSq = DIST * DIST;
		EntityAI e;
		foreach (Object o : objs)
		{
			if (!VB_Wants(o))
				continue;
			e = EntityAI.Cast(o);
			distSq = vector.DistanceSq(ppos, e.GetPosition());
			if (distSq > maxSq && m_VB_Lit.Find(e) < 0)
				continue;
			if (!VB_AR_Guard.VB_Is(e))
				distSq += GUARD_FIRST;
			cands.Insert(e);
			keys.Insert(distSq);
		}

		// MAX_LIT лучших: выбор минимума по очереди (кандидатов немного)
		array<EntityAI> best = new array<EntityAI>;
		int pick;
		float pickKey;
		while (best.Count() < MAX_LIT && cands.Count() > 0)
		{
			pick = 0;
			pickKey = keys[0];
			for (int k = 1; k < keys.Count(); k++)
			{
				if (keys[k] < pickKey)
				{
					pick = k;
					pickKey = keys[k];
				}
			}
			best.Insert(cands[pick]);
			cands.Remove(pick);
			keys.Remove(pick);
		}

		// лишние гаснут, новые загораются
		for (int j = m_VB_Lit.Count() - 1; j >= 0; j--)
		{
			if (best.Find(m_VB_Lit[j]) < 0)
				VB_Unlit(j);
		}
		VB_AR_BrewLight light;
		foreach (EntityAI b : best)
		{
			if (m_VB_Lit.Find(b) >= 0)
				continue;
			light = VB_AR_BrewLight.Cast(ScriptedLightBase.CreateLight(VB_AR_BrewLight, b.GetPosition()));
			if (!light)
				continue;
			light.AttachOnObject(b, "0 0.9 0");
			m_VB_Lit.Insert(b);
			m_VB_Lights.Insert(light);
		}
	}
}

// ============================================================================================
// Игрок: флаги «Живицы» (тряска) и «Настоя пыльцы» (подсветка), приём RPC «прислушаться» и вспышки пыльцы.
// Таймеры, сращивание переломов, плащ, светоч и оберег — сервер (VB_AR_GearServer.c).

modded class PlayerBase
{
	static const int VB_BLOOD_SHAKE = 3;			// из SHAKE_LEVEL_MAX = 7

	protected bool m_VB_AR_Blood;					// синхронизируется: действует «Живица»
	protected bool m_VB_AR_Brew;					// синхронизируется: действует «Настой пыльцы»

	void PlayerBase()
	{
		RegisterNetSyncVariableBool("m_VB_AR_Blood");
		RegisterNetSyncVariableBool("m_VB_AR_Brew");
	}

	bool VB_HasRootBlood()
	{
		return m_VB_AR_Blood;
	}

	bool VB_HasBrew()
	{
		return m_VB_AR_Brew;
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();

		if (PlayerBase.Cast(g_Game.GetPlayer()) != this)
			return;

		bool want = m_VB_AR_Brew && IsAlive();
		if (want != VB_AR_BrewFx.VB_IsRunning())
			VB_AR_BrewFx.VB_Set(want);
	}

	override void EEDelete(EntityAI parent)
	{
		if (g_Game && !g_Game.IsDedicatedServer())
		{
			Man me = g_Game.GetPlayer();
			if (!me || me == this)
				VB_AR_BrewFx.VB_Set(false);
		}
		super.EEDelete(parent);
	}

	// тряска при прицеливании считается и на сервере, и у клиента: берём синхронизируемый флаг
	override int GetShakeLevel()
	{
		int lvl = super.GetShakeLevel();
		if (m_VB_AR_Blood && lvl < VB_BLOOD_SHAKE)
			return VB_BLOOD_SHAKE;
		return lvl;
	}

	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);

		if (g_Game.IsDedicatedServer())
			return;

		if (rpc_type == VB_AR_Gear.RPC_LISTEN)
		{
			bool night;
			bool found;
			vector dir;
			int step;
			bool frantic;
			int storm;
			int stage;
			if (ctx.Read(night) && ctx.Read(found) && ctx.Read(dir) && ctx.Read(step) && ctx.Read(frantic) && ctx.Read(storm) && ctx.Read(stage))
				VB_AR_Listen.VB_Play(night, found, dir, step, frantic, storm, stage);
		}
		else if (rpc_type == VB_AR_Gear.RPC_BURST)
		{
			vector at;
			if (ctx.Read(at))
				VB_AR_Gear.VB_BurstFxClient(at);
		}
	}
}

modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(VB_ActionListenRoot);
		actions.Insert(VB_ActionFeedListenRoot);
		actions.Insert(VB_ActionArmSporeTrap);
		actions.Insert(VB_ActionLanternOn);
		actions.Insert(VB_ActionLanternOff);
		actions.Insert(VB_ActionDrinkPollenBrew);
	}
}
