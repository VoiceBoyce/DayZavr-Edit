// Действия у рощи Древнего корня (клиентская часть: классы, условия, регистрация).
// «Вырезать сердцевину» — долгое действие присев, любым инструментом со свежеванием (ножи, топоры, тесаки, пилы, штыки),
// только ночью и не во время нашествия нечисти. Как только начал — роща в 50 м бьётся чаще, выходят стражи.
// Сердцевина гаснет до рестарта, у ног вырезавшего падает добыча по ступени, у всех игроков бьёт золотая молния.
// Что делает сервер (тела OnStartServer/OnFinishProgressServer, поиск корня) — VB_AncientRoot_Server, VB_AR_ActionsServer.c.

class VB_ActionCutRootHeartCB : ActionContinuousBaseCB
{
	// время резки — по ступени рощи (ТЗ 2б.4): цель известна и клиенту, и серверу
	override void CreateActionComponent()
	{
		int stage = VB_AR_Heart.STAGE_GROVE;
		VB_AR_Heart heart;
		if (m_ActionData && m_ActionData.m_Target)
			heart = VB_AR_Heart.Cast(m_ActionData.m_Target.GetObject());
		if (heart)
			stage = heart.VB_Stage();
		m_ActionData.m_ActionComponent = new CAContinuousTime(VB_ActionCutRootHeart.VB_CutTime(stage));
	}
}

class VB_ActionCutRootHeart : ActionContinuousBase
{
	static const float CUT_TIME = 15.0;		// секунд держать (роща); росток — 8, древняя — 25
	static const float TOOL_DAMAGE = 15.0;	// урон инструменту за одно сердце

	static float VB_CutTime(int stage)
	{
		if (stage <= VB_AR_Heart.STAGE_SPROUT)
			return 8.0;
		if (stage >= VB_AR_Heart.STAGE_ANCIENT)
			return 25.0;
		return CUT_TIME;
	}

	void VB_ActionCutRootHeart()
	{
		m_CallbackClass = VB_ActionCutRootHeartCB;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_HIGH;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_ANIMALSKINNING;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_FullBody = true;
		m_Text = "Вырезать сердцевину";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINonRuined();
		m_ConditionTarget = new CCTCursor(UAMaxDistances.DEFAULT);
	}

	// только ночью, когда роща горит в полную силу (днём и в сумерках сердцевина спит — резать нечего).
	// Во время нашествия нечисти резать нельзя (осада синхронизируется — условие общее для клиента и сервера)
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		VB_AR_Heart heart = VB_AR_Heart.Cast(target.GetObject());
		if (!heart || heart.VB_IsDead() || heart.VB_UnderSiege())
			return false;

		return VB_AR_Pulse.VB_NightFactor() >= VB_AR_Pulse.NIGHT_FULL_ABOVE;
	}
}

// «Собрать споры»: тряпка в руках, у живого сердца ночью. Тряпка уходит (из пачки — одна), у ног падают
// «Споры Изнанки». У сердца SPORES_MAX порций за рестарт. Сердце просыпается и шумит, но волны зомби нет.

class VB_ActionCollectSporesCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(VB_ActionCollectSpores.COLLECT_TIME);
	}
}

class VB_ActionCollectSpores : ActionContinuousBase
{
	static const float COLLECT_TIME = 6.0;
	static const string SPORES_TYPE = "VB_AR_Spores";

	void VB_ActionCollectSpores()
	{
		m_CallbackClass = VB_ActionCollectSporesCB;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_FullBody = true;
		m_Text = "Собрать пыльцу";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINonRuined();
		m_ConditionTarget = new CCTCursor(UAMaxDistances.DEFAULT);
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		VB_AR_Heart heart = VB_AR_Heart.Cast(target.GetObject());
		if (!heart || heart.VB_IsDead() || heart.VB_SporesLeft() <= 0 || heart.VB_UnderSiege())
			return false;		// в осаде нечисти сбора нет (ТЗ 2в п.1)

		return VB_AR_Pulse.VB_NightFactor() >= VB_AR_Pulse.NIGHT_FULL_ABOVE;
	}
}

// «Отломить корень»: нож (всё, чем свежуют) в руках, рядом живой корень гнезда (VB_AR_Roots_*), ночью.
// Цель — ближайший корень в ROOT_DIST от игрока, не прицел: «ручка» корня у самой земли, в неё трудно навести.
// Корень гаснет до рестарта (второй раз с него не взять), у ног падает «Корень Изнанки», рядом шум.

class VB_ActionBreakRootRootCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(VB_ActionBreakRootRoot.BREAK_TIME);
	}
}

class VB_ActionBreakRootRoot : ActionContinuousBase
{
	static const float BREAK_TIME = 8.0;
	static const float ROOT_DIST = 3.0;		// м от игрока до места, где корень выходит из земли
	static const float TOOL_DAMAGE = 4.0;
	static const string ROOT_TYPE = "VB_AR_RootPiece";
	static const string NOISE = "VB_AR_ChunkNoise";	// тихий треск: зомби рядом проверят, деревня не сбегается
	static const float NOISE_TIME = 6.0;
	static const int CACHE_MS = 250;		// условие без цели зовётся каждый кадр, пока нож в руках: перебор рощи не чаще

	protected bool m_VB_LastOk;
	protected int m_VB_LastAt = -100000;

	void VB_ActionBreakRootRoot()
	{
		m_CallbackClass = VB_ActionBreakRootRootCB;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_MEDIUM;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_ANIMALSKINNING;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_FullBody = true;
		m_Text = "Отломить отросток";
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

	// Клиент ищет живой корень по пульсу (связь корня с сердцем знает только он). На сервере пульса нет:
	// здесь только ночь, корень рядом ищет серверный мод (VB_AR_ActionsServer.c).
	// Клиент перебирает объекты рощи не чаще CACHE_MS (как VB_ActionTouchGrove): условие без цели зовётся каждый кадр
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (VB_AR_Pulse.VB_NightFactor() < VB_AR_Pulse.NIGHT_FULL_ABOVE)
			return false;

		if (g_Game.IsDedicatedServer())
			return true;

		int now = g_Game.GetTime();
		if (now >= m_VB_LastAt && now - m_VB_LastAt < CACHE_MS)
			return m_VB_LastOk;
		m_VB_LastAt = now;

		// в осаде нечисти отлома нет (ТЗ 2в п.1; сервер проверяет сам, VB_AR_ActionsServer.c)
		vector pos = player.GetPosition();
		if (VB_AR_Pulse.VB_NearestSiegeHeart(pos, VB_AR_Pulse.LINK_DIST))
			m_VB_LastOk = false;
		else
			m_VB_LastOk = VB_AR_Pulse.VB_NearestLiveRoot(pos, ROOT_DIST) != null;
		return m_VB_LastOk;
	}
}

modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(VB_ActionCutRootHeart);
		actions.Insert(VB_ActionCollectSpores);
		actions.Insert(VB_ActionBreakRootRoot);
	}
}

modded class ItemBase
{
	// всё, чем можно свежевать, годится, чтобы вырезать сердце и отломить корень Изнанки
	override void AddAction(typename actionName)
	{
		super.AddAction(actionName);
		if (actionName == ActionSkinning)
		{
			super.AddAction(VB_ActionCutRootHeart);
			super.AddAction(VB_ActionBreakRootRoot);
		}
	}
}

modded class Rag
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(VB_ActionCollectSpores);
	}
}
