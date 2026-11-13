// Изделия Изнанки (крафт в DRL_CraftMenu): слушающий корень, сердце-аккумулятор, споровая ловушка, фонарь-сердце,
// кровь Изнанки, топливная капсула, голодный клинок, плащ-кокон, живой жгут, нить Изнанки.
// Конфиги — config.cpp (раздел «изделия Изнанки»), модели — tools/gen_rift_veins.py, перекраски — data/retex.
// Здесь клиентская часть: классы, поля, синхронизация, сохранение, условия действий, эффекты, приём RPC.
// Серверная часть (таймеры, заряды, вызов зомби, кровь, клинок, жгут, нить, логи) — VB_Rift_Server, VB_RiftGearServer.c.

class VB_RiftGear
{
	static const int RPC_LISTEN = 1447186012;	// сервер -> клиент: результат «прислушаться»
	static const int RPC_BURST = 1447186013;	// сервер -> клиенты рядом: хлопок спор в точке
	static const float BURST_FX_DIST = 200.0;
	static const string BURST_SOUND = "VB_RiftBurst_SoundSet";

	// клиент: облако красных спор и мокрый хлопок
	static void VB_BurstFxClient(vector pos)
	{
		ParticleManager pm = ParticleManager.GetInstance();
		if (pm)
			pm.PlayInWorld(ParticleList.VB_RIFT_BURST, pos + "0 0.6 0");

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
// «Слушающий корень»: ночью «прислушаться» (1 заряд) — сервер ищет ближайшее живое сердце в RANGE и отдаёт
// только направление (по сектору DIR_SECTOR_DEG) и ступень расстояния (DIST_STEP), не координаты сердца.
// Клиент слышит серию ударов с той стороны: ближе — громче и чаще, если сердце режут — бешеный стук.
// Подзарядка — у живого сердца ночью (VB_ActionFeedListenRoot): заряды до полного, сердце просыпается.

class VB_RiftListenRoot extends Inventory_Base
{
	static const float RANGE = 3000.0;
	static const float DIST_STEP = 250.0;		// м: ступень расстояния в ответе сервера
	static const float DIR_SECTOR_DEG = 10.0;	// направление округляется до сектора

	override void SetActions()
	{
		super.SetActions();
		AddAction(VB_ActionListenRoot);
		AddAction(VB_ActionFeedListenRoot);
	}
}

// клиент: серия ударов в стороне сердца
class VB_RiftListen
{
	static const string SOUND = "VB_RiftListen_SoundSet";
	static const float SOUND_DIST = 6.0;	// м от игрока в сторону сердца
	static const int TOTAL_MS = 7000;

	protected static ref VB_RiftListen s_VB_Cur;

	protected vector m_VB_Dir;
	protected float m_VB_Volume;
	protected int m_VB_Left;

	static void VB_Play(bool found, vector dir, int step, bool frantic)
	{
		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		if (!player)
			return;

		if (!found)
		{
			player.MessageStatus("Корень молчит: живых сердец поблизости нет.");
			return;
		}

		if (s_VB_Cur)
			s_VB_Cur.VB_Stop();
		s_VB_Cur = new VB_RiftListen(dir, step, frantic);
	}

	void VB_RiftListen(vector dir, int step, bool frantic)
	{
		vector d = dir;
		d[1] = 0;
		d.Normalize();
		m_VB_Dir = d;

		// середина ступени: 0 — до 250 м, 1 — 250–500 м и т. д.
		float dist = (step + 0.5) * VB_RiftListenRoot.DIST_STEP;
		float k = Math.Clamp(dist / VB_RiftListenRoot.RANGE, 0.0, 1.0);
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

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		return item.GetQuantity() > 0 && VB_RiftPulse.VB_NightFactor() >= VB_RiftPulse.NIGHT_LIGHT_ABOVE;
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
		VB_RiftHeart heart = VB_RiftHeart.Cast(target.GetObject());
		if (!heart || heart.VB_IsDead() || item.GetQuantity() >= item.GetQuantityMax())
			return false;

		return VB_RiftPulse.VB_NightFactor() >= VB_RiftPulse.NIGHT_FULL_ABOVE;
	}
}

// ============================================================================================
// «Сердце-аккумулятор»: автомобильный аккумулятор. Сервер раз в минуту: ночью +CHARGE_PER_MIN, днём -DRAIN_PER_MIN.
// Клиент: игрок в машине с таким аккумулятором, ночь, живое сердце в CABIN_DIST — стук в салоне, ближе — чаще.

class VB_RiftHeartBattery extends CarBattery
{
	static const int SELF_MS = 60000;
	static const float CHARGE_PER_MIN = 10.0;	// из 500 (2 %): полный за ~50 минут ночи
	static const float DRAIN_PER_MIN = 1.25;	// 0.25 %
	static const float CABIN_DIST = 500.0;
	static const int CABIN_TICK_MS = 250;
	static const string CABIN_SOUND = "VB_RiftChunkBeat_SoundSet";

	protected float m_VB_NextBeat;

	override void EEInit()
	{
		super.EEInit();
		if (!g_Game.IsDedicatedServer())
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_CabinClient, CABIN_TICK_MS, true);
	}

	override void EEDelete(EntityAI parent)
	{
		if (g_Game)
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_CabinClient);
		super.EEDelete(parent);
	}

	void VB_CabinClient()
	{
		CarScript car = CarScript.Cast(GetHierarchyParent());
		if (!car)
			return;

		PlayerBase me = PlayerBase.Cast(g_Game.GetPlayer());
		if (!me)
			return;

		HumanCommandVehicle hcv = me.GetCommand_Vehicle();
		if (!hcv || hcv.GetTransport() != car)
			return;

		if (VB_RiftPulse.VB_NightFactor() < VB_RiftPulse.NIGHT_LIGHT_ABOVE)
			return;

		VB_RiftBase heart = VB_RiftPulse.VB_NearestLiveHeart(car.GetPosition(), CABIN_DIST);
		if (!heart)
			return;

		float now = g_Game.GetTime() * 0.001;
		if (now < m_VB_NextBeat)
			return;

		float k = vector.Distance(car.GetPosition(), heart.GetPosition()) / CABIN_DIST;
		m_VB_NextBeat = now + Math.Lerp(0.9, 3.0, k);
		EffectSound snd = SEffectManager.PlaySoundOnObject(CABIN_SOUND, me);
		if (snd)
		{
			snd.SetSoundVolume(Math.Lerp(1.0, 0.35, k));
			snd.SetAutodestroy(true);
		}
	}
}

// ============================================================================================
// «Споровая ловушка»: «Раскрыть» в руках, потом бросить. Облако спор шумит (зомби идут), через ARM_MS лопается:
// живым зомби в BURST_RADIUS урон BURST_DAMAGE (обычных с полным здоровьем это почти валит, зомби Изнанки — нет).

class VB_RiftSporeTrap extends Inventory_Base
{
	static const int ARM_MS = 10000;
	static const int NOISE_MS = 2000;
	static const string NOISE = "VB_RiftTrapNoise";
	static const float BURST_RADIUS = 7.0;
	static const float BURST_DAMAGE = 70.0;

	protected bool m_VB_Armed;				// синхронизируется: облако спор над ловушкой
	protected ParticleSource m_VB_Cloud;

	void VB_RiftSporeTrap()
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
				m_VB_Cloud = pm.PlayOnObject(ParticleList.VB_RIFT_SPORES, this, "0 0.05 0", "0 0 0", true);
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
		VB_RiftSporeTrap trap = VB_RiftSporeTrap.Cast(item);
		return trap && !trap.VB_IsArmed();
	}
}

// ============================================================================================
// «Фонарь-сердце»: горит красным без батареек (в руках или на земле), количество = минуты спор.
// Клиент: пульс света, вдвое-втрое чаще, если живой зомби ближе ALARM_DIST; такт идёт только у горящего фонаря.
// Сервер: ночью раз в CALL_MS с шансом CALL_CHANCE из темноты выходит зомби Изнанки (без лута) и идёт на свет.

class VB_RiftLanternLight extends PointLightBase
{
	void VB_RiftLanternLight()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(8.0);
		SetBrightnessTo(0.8);
		SetCastShadow(false);
		SetFlareVisible(false);
		SetAmbientColor(1.0, 0.12, 0.15);
		SetDiffuseColor(1.0, 0.12, 0.15);
	}
}

class VB_RiftLantern extends Inventory_Base
{
	static const int MINUTE_MS = 60000;
	static const int CALL_MS = 180000;
	static const float CALL_CHANCE = 0.45;
	static const string CALL_NOISE = "VB_RiftChunkNoise";
	static const float ALARM_DIST = 30.0;
	static const int TICK_MS = 50;
	static const float BEAT_PERIOD = 1.6;

	protected bool m_VB_On;					// синхронизируется, в сохранении (формат прежний: один bool)
	protected VB_RiftLanternLight m_VB_Light;
	protected bool m_VB_Ticking;			// клиент: идёт такт света
	protected float m_VB_Phase;
	protected float m_VB_Rate = 1.0;
	protected int m_VB_NearCheckMs;
	protected bool m_VB_Alarm;

	void VB_RiftLantern()
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

		return VB_RiftGear.VB_InHandsOf(this, PlayerBase.Cast(parent));
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
			m_VB_Light = VB_RiftLanternLight.Cast(ScriptedLightBase.CreateLight(VB_RiftLanternLight, GetPosition()));
			if (m_VB_Light)
				m_VB_Light.AttachOnObject(this, "0 0.12 0");
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
		float beat1 = VB_RiftPulse.VB_Beat(m_VB_Phase, 0.1);
		float beat2 = VB_RiftPulse.VB_Beat(m_VB_Phase, 0.45);
		float k = Math.Clamp(beat1 + 0.65 * beat2, 0.0, 1.0);
		m_VB_Light.SetBrightnessTo(0.55 + 1.5 * k);
		m_VB_Light.SetRadiusTo(6.5 + 2.5 * k);
	}

	protected bool VB_ZombieNear()
	{
		array<Object> objs = new array<Object>;
		g_Game.GetObjectsAtPosition(VB_RiftItems.VB_WorldPos(this), ALARM_DIST, objs, null);
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
		m_Text = "Разбудить фонарь";
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
		VB_RiftLantern lantern = VB_RiftLantern.Cast(item);
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
		m_Text = "Усыпить фонарь";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone();
		m_ConditionTarget = new CCTNone();
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		VB_RiftLantern lantern = VB_RiftLantern.Cast(item);
		return lantern && lantern.VB_IsOn();
	}
}

// ============================================================================================
// «Кровь Изнанки»: ванильный укол (себе или другому). Кровь и здоровье сразу (сервер, OnApply); у игрока на время
// синхронизируемый флаг: руки дрожат при прицеливании (уровень тряски как от холода), у него на клиенте живые зомби
// в GLOW_DIST светятся красным, экран слабо пульсирует красным в такт сердцу.

class VB_RiftBlood extends Inventory_Base
{
	static const float BLOOD_ADD = 1500.0;
	static const float HEALTH_ADD = 40.0;

	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionInjectTarget);
		AddAction(ActionInjectSelf);
	}
}

// Клиент: эффект «Крови Изнанки» у своего игрока. Каждый такт проверяет, что у ТЕКУЩЕГО локального персонажа
// флаг ещё стоит: после смерти, возрождения или перезахода эффект гаснет сам (баг №27).
class VB_RiftBloodFx
{
	static const string GLOW_MAT = "VB_Rift\\data\\zombies\\vb_rift_zmb_glow.rvmat";
	static const float GLOW_DIST = 40.0;
	static const int SCAN_MS = 1000;
	static const int BEAT_MS = 1600;
	static const string BEAT_SOUND = "VB_RiftChunkBeat_SoundSet";
	static const float FLASH = 0.05;

	protected static ref VB_RiftBloodFx s_VB_Inst;

	protected ref array<ZombieBase> m_VB_Lit;
	protected ref array<string> m_VB_LitMat;
	protected bool m_VB_Halted;				// остановлен изнутри своего такта, ждёт замены/удаления снаружи

	static void VB_Set(bool on)
	{
		if (on)
		{
			if (!s_VB_Inst || s_VB_Inst.m_VB_Halted)
				s_VB_Inst = new VB_RiftBloodFx();
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

	void VB_RiftBloodFx()
	{
		m_VB_Lit = new array<ZombieBase>;
		m_VB_LitMat = new array<string>;
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_Scan, SCAN_MS, true);
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_Beat, BEAT_MS, true);
		VB_Scan();
	}

	void VB_Stop()
	{
		m_VB_Halted = true;
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_Scan);
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_Beat);
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_FlashOff);
		VB_FlashOff();
		for (int i = m_VB_Lit.Count() - 1; i >= 0; i--)
			VB_Restore(i);
	}

	// Действует ли ещё «Кровь» у текущего локального персонажа. Нет — остановиться (сам объект не удаляем:
	// мы внутри его же такта; s_VB_Inst заменит или обнулит следующий VB_Set).
	protected bool VB_StillOn()
	{
		PlayerBase me = PlayerBase.Cast(g_Game.GetPlayer());
		if (me && me.IsAlive() && me.VB_HasRiftBlood())
			return true;

		VB_Stop();
		return false;
	}

	protected void VB_Restore(int i)
	{
		ZombieBase zmb = m_VB_Lit[i];
		if (zmb)
		{
			int idx = zmb.GetHiddenSelectionIndex("camo");
			if (idx >= 0)
				zmb.SetObjectMaterial(idx, m_VB_LitMat[i]);
		}
		m_VB_Lit.Remove(i);
		m_VB_LitMat.Remove(i);
	}

	// обычные живые зомби в GLOW_DIST: подсветить новых, вернуть материал ушедшим и умершим
	void VB_Scan()
	{
		if (!VB_StillOn())
			return;

		Man player = g_Game.GetPlayer();
		vector ppos = player.GetPosition();
		float glowSq = GLOW_DIST * GLOW_DIST * 1.2;
		for (int i = m_VB_Lit.Count() - 1; i >= 0; i--)
		{
			if (!m_VB_Lit[i] || !m_VB_Lit[i].IsAlive() || vector.DistanceSq(ppos, m_VB_Lit[i].GetPosition()) > glowSq)
				VB_Restore(i);
		}

		array<Object> objs = new array<Object>;
		g_Game.GetObjectsAtPosition(ppos, GLOW_DIST, objs, null);
		ZombieBase zmb;
		TStringArray mats = new TStringArray;
		int idx;
		foreach (Object o : objs)
		{
			zmb = ZombieBase.Cast(o);
			if (!zmb || !zmb.IsAlive() || VB_RiftZombie.VB_Is(zmb) || m_VB_Lit.Find(zmb) >= 0)
				continue;
			idx = zmb.GetHiddenSelectionIndex("camo");
			if (idx < 0)
				continue;
			mats.Clear();
			zmb.ConfigGetTextArray("hiddenSelectionsMaterials", mats);
			if (mats.Count() <= idx)
				continue;	// исходный материал не известен — вернуть было бы нечем
			zmb.SetObjectMaterial(idx, GLOW_MAT);
			m_VB_Lit.Insert(zmb);
			m_VB_LitMat.Insert(mats[idx]);
		}
	}

	void VB_Beat()
	{
		if (!VB_StillOn())
			return;

		Man player = g_Game.GetPlayer();
		EffectSound snd = SEffectManager.PlaySoundOnObject(BEAT_SOUND, player);
		if (snd)
		{
			snd.SetSoundVolume(0.45);
			snd.SetAutodestroy(true);
		}

		PPERequester_HealthHitReaction req = PPERequester_HealthHitReaction.Cast(PPERequesterBank.GetRequester(PPERequester_HealthHitReaction));
		if (req)
		{
			req.Start();
			req.SetHitIntensity(FLASH);
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_FlashOff, 180, false);
		}
	}

	void VB_FlashOff()
	{
		PPERequester_HealthHitReaction req = PPERequester_HealthHitReaction.Cast(PPERequesterBank.GetRequester(PPERequester_HealthHitReaction));
		if (req)
			req.Stop();
	}
}

// ============================================================================================
// «Топливная капсула»: залить в бак (+FUEL_LITERS). Машина полчаса тянет красный шлейф спор; если она стоит
// и рядом игрок, из земли вокруг лезут 2–3 зомби Изнанки без лута (сервер, VB_RiftGearServer.c).

class VB_RiftFuelCapsule extends Inventory_Base
{
	static const float FUEL_LITERS = 20.0;

	override void SetActions()
	{
		super.SetActions();
		AddAction(VB_ActionPourFuelCapsule);
	}
}

class VB_ActionPourFuelCapsuleCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(VB_ActionPourFuelCapsule.POUR_TIME);
	}
}

class VB_ActionPourFuelCapsule : ActionContinuousBase
{
	static const float POUR_TIME = 6.0;

	void VB_ActionPourFuelCapsule()
	{
		m_CallbackClass = VB_ActionPourFuelCapsuleCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_EMPTY_VESSEL;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_FullBody = true;
		m_Text = "Залить капсулу в бак";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINonRuined();
		m_ConditionTarget = new CCTCursor(UAMaxDistances.DEFAULT);
	}

	static CarScript VB_Car(ActionTarget target)
	{
		CarScript car = CarScript.Cast(target.GetObject());
		if (!car)
			car = CarScript.Cast(target.GetParent());
		return car;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		CarScript car = VB_Car(target);
		return car && !car.IsRuined() && car.GetFluidFraction(CarFluid.FUEL) < 0.99;
	}
}

modded class CarScript
{
	protected bool m_VB_RiftFuel;					// синхронизируется: красный шлейф
	protected ParticleSource m_VB_Trail;

	void CarScript()
	{
		RegisterNetSyncVariableBool("m_VB_RiftFuel");
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		VB_UpdateTrail();
	}

	protected void VB_UpdateTrail()
	{
		if (m_VB_RiftFuel && !m_VB_Trail)
		{
			ParticleManager pm = ParticleManager.GetInstance();
			if (!pm)
				return;

			vector mm[2];
			vector rear = "0 0.5 -2";
			if (GetCollisionBox(mm))
				rear = Vector(0, mm[0][1] + 0.5, mm[0][2] - 0.2);
			m_VB_Trail = pm.PlayOnObject(ParticleList.VB_RIFT_SPORES, this, rear, "0 0 0", true);
		}
		else if (!m_VB_RiftFuel && m_VB_Trail)
		{
			m_VB_Trail.StopParticle();
			m_VB_Trail = null;
		}
	}

	override void EEDelete(EntityAI parent)
	{
		if (m_VB_Trail)
		{
			m_VB_Trail.StopParticle();
			m_VB_Trail = null;
		}
		super.EEDelete(parent);
	}
}

// ============================================================================================
// «Голодный клинок»: убийство зомби (этим ножом) — владельцу +BLOOD_PER_KILL крови, ножу +1 заряд и лишний износ;
// на CHARGE_MAX зарядов — хлопок спор вокруг владельца. Бьёт зомби сильнее ванильного ножа (+HIT_BONUS).
// Голод копится, только пока клинок у игрока в игре: после HUNGER_AFTER_S такой игры без еды раз в минуту забирает
// HUNGER_BLOOD крови, но не ниже BLOOD_FLOOR. Заряд и накопленный голод — в сохранении. Логика — сервер.

class VB_RiftBlade extends FangeKnife
{
	static const int CHARGE_MAX = 10;
	static const float BLOOD_PER_KILL = 150.0;
	static const float EXTRA_WEAR = 1.0;
	static const float STUN_RADIUS = 6.0;
	static const float STUN_DAMAGE = 60.0;
	static const int HUNGER_AFTER_S = 3600;
	static const int HUNGER_TICK_MS = 60000;
	static const float HUNGER_BLOOD = 60.0;
	static const float BLOOD_FLOOR = 3000.0;	// ниже голод кровь не опускает: клинок не убивает
	static const int HUNGER_MSG_S = 600;
	static const float HIT_BONUS = 30.0;		// доп. урон зомби за удар (расчёт — в VB_RiftGearServer.c)
	static const int STORE_VER = 1;

	protected int m_VB_Charge;				// синхронизируется, в сохранении
	protected int m_VB_Hunger;				// сервер: секунд игры с клинком без еды, в сохранении

	void VB_RiftBlade()
	{
		RegisterNetSyncVariableInt("m_VB_Charge", 0, CHARGE_MAX);
	}

	int VB_Charge()
	{
		return m_VB_Charge;
	}

	// формат 1: STORE_MARK, версия, заряд, голод (секунды)
	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);
		int mark = VB_RiftItems.STORE_MARK;
		int ver = STORE_VER;
		ctx.Write(mark);
		ctx.Write(ver);
		ctx.Write(m_VB_Charge);
		ctx.Write(m_VB_Hunger);
	}

	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
			return false;

		int first;
		if (!ctx.Read(first))
			return false;

		// старый формат: заряд, время последнего убийства (UTC старого календаря). Голод тогда считался и офлайн —
		// после обновления клинок считается сытым.
		if (first != VB_RiftItems.STORE_MARK)
		{
			int lastFed;
			if (!ctx.Read(lastFed))
				return false;
			m_VB_Charge = Math.Clamp(first, 0, CHARGE_MAX);
			m_VB_Hunger = 0;
			return true;
		}

		int ver;
		if (!ctx.Read(ver) || ver < 1 || ver > STORE_VER)
			return false;

		int charge;
		int hunger;
		if (!ctx.Read(charge) || !ctx.Read(hunger))
			return false;

		m_VB_Charge = Math.Clamp(charge, 0, CHARGE_MAX);
		m_VB_Hunger = Math.Max(hunger, 0);
		return true;
	}
}

// ============================================================================================
// «Живой жгут»: лежит у игрока где угодно; при кровотечении сам закрывает самое сильное, забирает BLOOD_COST крови
// и исчезает (сервер).

class VB_RiftTourniquet extends Inventory_Base
{
	static const int CHECK_MS = 3000;
	static const float BLOOD_COST = 150.0;
}

// ============================================================================================
// «Нить Изнанки»: перетащить на одежду (рецепт VB_RecipeRiftThread). Вещь THREAD_MINUTES минут сама латается на
// 1 % в минуту, но не выше THREAD_CAP (70 %, как ванильная починка до «Worn»). Бронежилеты, плитники, шлемы и прочую
// броню нить не берёт. Состояние только в памяти сервера: рестарт его сбрасывает (в сохранение одежды ничего не
// пишем, чтобы мод можно было снять без порчи вещей).

class VB_RecipeRiftThread extends RecipeBase
{
	static const float THREAD_CAP = 0.7;			// доля здоровья, выше которой нить не чинит
	static const float ARMOR_BELOW = 0.8;			// броня: урон от пуль по вещи режется сильнее этого

	override void Init()
	{
		m_Name = "Пришить нить Изнанки";
		m_IsInstaRecipe = false;
		m_AnimationLength = 1.5;
		m_Specialty = -0.02;

		m_MinDamageIngredient[0] = -1;
		m_MaxDamageIngredient[0] = 3;
		m_MinQuantityIngredient[0] = -1;
		m_MaxQuantityIngredient[0] = -1;
		m_MinDamageIngredient[1] = -1;
		m_MaxDamageIngredient[1] = 3;
		m_MinQuantityIngredient[1] = -1;
		m_MaxQuantityIngredient[1] = -1;

		InsertIngredient(0, "VB_RiftThread");
		m_IngredientAddHealth[0] = 0;
		m_IngredientSetHealth[0] = -1;
		m_IngredientAddQuantity[0] = 0;
		m_IngredientDestroy[0] = true;

		InsertIngredient(1, "Clothing");
		m_IngredientAddHealth[1] = 0;
		m_IngredientSetHealth[1] = -1;
		m_IngredientAddQuantity[1] = 0;
		m_IngredientDestroy[1] = false;
	}

	// Броня: шлемы (HelmetBase), плитники (PlateCarrierVest) и всё, у чего GlobalArmor Projectile режет урон от
	// пуль сильнее ARMOR_BELOW (в ванили: плитник 0.3, пресс/полиция 0.5, нагрудник 0.55, кольчуга 0.7; обычная
	// одежда — без защиты от пуль). Так же ловится броня из модов одежды.
	static bool VB_IsArmor(EntityAI item)
	{
		if (HelmetBase.Cast(item) || PlateCarrierVest.Cast(item))
			return true;

		string path = "CfgVehicles " + item.GetType() + " DamageSystem GlobalArmor Projectile Health damage";
		if (!g_Game.ConfigIsExisting(path))
			return false;

		return g_Game.ConfigGetFloat(path) < ARMOR_BELOW;
	}

	// Клиент и сервер: целая не-броня, изношенная хотя бы до «Worn» (здоровье ниже 70 %). Уже прошитую вещь
	// отсекает серверный мод (состояние нити знает только сервер).
	override bool CanDo(ItemBase ingredients[], PlayerBase player)
	{
		Clothing cloth = Clothing.Cast(ingredients[1]);
		if (!cloth || cloth.IsRuined() || VB_IsArmor(cloth))
			return false;

		return cloth.GetHealthLevel() >= GameConstants.STATE_WORN;
	}
}

modded class PluginRecipesManagerBase
{
	override void RegisterRecipies()
	{
		super.RegisterRecipies();
		RegisterRecipe(new VB_RecipeRiftThread);
	}
}

// ============================================================================================
// Игрок: «Кровь Изнанки» (флаг и тряска), RPC «прислушаться» и хлопков спор. Плащ-кокон, таймер крови и снятие
// флага при смерти — сервер (VB_RiftGearServer.c).

modded class PlayerBase
{
	static const int VB_BLOOD_SHAKE = 3;			// из SHAKE_LEVEL_MAX = 7

	protected bool m_VB_RiftBlood;					// синхронизируется: действует «Кровь Изнанки»

	void PlayerBase()
	{
		RegisterNetSyncVariableBool("m_VB_RiftBlood");
	}

	bool VB_HasRiftBlood()
	{
		return m_VB_RiftBlood;
	}

	// тряска при прицеливании считается и на сервере, и у клиента: берём синхронизируемый флаг
	override int GetShakeLevel()
	{
		int lvl = super.GetShakeLevel();
		if (m_VB_RiftBlood && lvl < VB_BLOOD_SHAKE)
			return VB_BLOOD_SHAKE;
		return lvl;
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();

		if (PlayerBase.Cast(g_Game.GetPlayer()) != this)
			return;

		bool want = m_VB_RiftBlood && IsAlive();
		if (want != VB_RiftBloodFx.VB_IsRunning())
			VB_RiftBloodFx.VB_Set(want);
	}

	override void EEDelete(EntityAI parent)
	{
		if (g_Game && !g_Game.IsDedicatedServer())
		{
			Man me = g_Game.GetPlayer();
			if (!me || me == this)
				VB_RiftBloodFx.VB_Set(false);
		}
		super.EEDelete(parent);
	}

	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);

		if (g_Game.IsDedicatedServer())
			return;

		if (rpc_type == VB_RiftGear.RPC_LISTEN)
		{
			bool found;
			vector dir;
			int step;
			bool frantic;
			if (ctx.Read(found) && ctx.Read(dir) && ctx.Read(step) && ctx.Read(frantic))
				VB_RiftListen.VB_Play(found, dir, step, frantic);
		}
		else if (rpc_type == VB_RiftGear.RPC_BURST)
		{
			vector at;
			if (ctx.Read(at))
				VB_RiftGear.VB_BurstFxClient(at);
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
		actions.Insert(VB_ActionPourFuelCapsule);
	}
}
