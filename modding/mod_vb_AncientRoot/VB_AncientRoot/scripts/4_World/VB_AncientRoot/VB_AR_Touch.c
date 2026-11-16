// «Коснуться рощи» (решение пользователя 2026-11-16, вариант Б): действие взаимодействия у сердцевины или любого сетевого корня
// рощи — короткое описание именно этой рощи: ступень, примет ли дар и сколько, спит ли, тревожили ли, вырезана ли.
// Так игрок узнаёт, что роща принимает дары и что её можно вырезать. Только клиент: всё нужное синхронизируется
// у сердцевины (ступень, подношения, вырезана, потревожена) — серверу ничего не шлём.

class VB_ActionTouchGrove : ActionInteractBase
{
	static const int CACHE_MS = 1000;		// условие считается каждый кадр — сердцевину по корню ищем не чаще

	protected Object m_VB_LastObj;
	protected VB_AR_Heart m_VB_LastHeart;
	protected int m_VB_LastAt = -100000;

	void VB_ActionTouchGrove()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "Коснуться рощи";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone();
		m_ConditionTarget = new CCTCursor(UAMaxDistances.DEFAULT);
	}

	protected VB_AR_Heart VB_HeartCached(Object o)
	{
		int now = g_Game.GetTime();
		if (o == m_VB_LastObj && now - m_VB_LastAt < CACHE_MS)
			return m_VB_LastHeart;
		m_VB_LastObj = o;
		m_VB_LastAt = now;
		m_VB_LastHeart = VB_AR_Offer.VB_HeartOf(o, false);
		return m_VB_LastHeart;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!target)
			return false;		// что в руках — не важно: с руной в руках сразу видно, примет ли роща дар
		return VB_HeartCached(target.GetObject()) != null;
	}

	override void OnExecuteClient(ActionData action_data)
	{
		super.OnExecuteClient(action_data);
		if (!action_data || !action_data.m_Target)
			return;
		VB_AR_Heart heart = VB_AR_Offer.VB_HeartOf(action_data.m_Target.GetObject(), false);
		if (!heart)
			return;
		string title;
		string text = VB_Describe(heart, title);
		NotificationSystem.AddNotificationExtended(8, title, text);
	}

	static string VB_StageTitle(int stage)
	{
		if (stage <= VB_AR_Heart.STAGE_SPROUT)
			return "Росток Древнего корня";
		if (stage >= VB_AR_Heart.STAGE_ANCIENT)
			return "Древняя роща";
		return "Роща Древнего корня";
	}

	// 1 дар, 2–4 дара, 5 даров
	static string VB_GiftWord(int n)
	{
		int n100 = n % 100;
		int n10 = n % 10;
		if (n100 >= 11 && n100 <= 14)
			return "даров";
		if (n10 == 1)
			return "дар";
		if (n10 >= 2 && n10 <= 4)
			return "дара";
		return "даров";
	}

	static string VB_Describe(VB_AR_Heart h, out string title)
	{
		title = VB_StageTitle(h.VB_Stage());
		if (h.VB_IsDead())
			return "Сердцевина вырезана. Роща умирает и до рестарта не оживёт.";
		if (h.VB_UnderSiege())
		{
			int power = h.VB_SiegePower();
			return "Нечисть грызёт корни! Сила рощи " + power.ToString() + "%. Убей пауков — роща отблагодарит.";
		}
		if (VB_AR_Pulse.VB_NightFactor() < VB_AR_Pulse.NIGHT_FULL_ABOVE)
			return "Роща спит. Приходи ночью — тогда она примет дар.";

		string s;
		int left = h.VB_OffersLeft();
		if (left <= 0)
			s = "Роща больше не принимает даров до рестарта.";
		else
			s = "Роща примет ещё " + left.ToString() + " " + VB_GiftWord(left) + ": руну, кристалл, трофей или мясо.";
		if (h.VB_IsDisturbed())
			s += " Сердцевину тревожили — стражи рядом.";
		return s;
	}
}

modded class PlayerBase
{
	override void SetActions(out TInputActionMap InputActionMap)
	{
		super.SetActions(InputActionMap);
		AddAction(VB_ActionTouchGrove, InputActionMap);
	}
}

modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(VB_ActionTouchGrove);
	}
}
