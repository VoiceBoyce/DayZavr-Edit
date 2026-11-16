// «Сделать подношение» (ТЗ_сервер_18.md п. 2б.2–2б.3) — второй путь у сердцевины вместо вырезания.
// Ночью у живой сердцевины с даром в руках: дар уходит роще, игроку — дар рощи у ног и Благословение рощи,
// тревога рощи сбрасывается. Одна роща принимает OFFERS_MAX подношений за рестарт (VB_AR_Heart.m_VB_OffersLeft).
// Здесь — клиентская часть (класс действия, условие, что годится в дар); что выдаёт сервер — VB_AncientRoot_Server,
// VB_AR_OfferServer.c.
//
// Ценность дара (схема утверждена пользователем 2026-11-15):
//   лучший  — руны и кристаллы MSFZ, трофеи боссов аномалий (VB_AB_Trophy_*);
//   средний — трофеи мутантов MSFZ (глаз и шип Занозы, лапа паука, голова Ричара, голова Энта);
//   мясо    — любое мясо (Object.IsMeat: звериное, мутантов, рыба).

class VB_AR_Offer
{
	static const int TIER_NONE = 0;
	static const int TIER_MEAT = 1;
	static const int TIER_MID = 2;
	static const int TIER_BEST = 3;

	// начала имён классов (IndexOf == 0)
	protected static const ref TStringArray BEST = {"MSFZ_RuneStone", "MSFZ_Crystal_", "VB_AB_Trophy_"};
	protected static const ref TStringArray MID = {"MSFZ_Zanoza_eye", "MSFZ_Zanoza_stick", "MSFZ_Spider_Lapka",
		"MSFZ_Ricar_Head", "MSFZ_Ent_head"};

	static int VB_Tier(EntityAI item)
	{
		if (!item || item.IsRuined())
			return TIER_NONE;

		string type = item.GetType();
		foreach (string b : BEST)
		{
			if (type.IndexOf(b) == 0)
				return TIER_BEST;
		}
		foreach (string m : MID)
		{
			if (type.IndexOf(m) == 0)
				return TIER_MID;
		}
		if (item.IsMeat())
			return TIER_MEAT;
		return TIER_NONE;
	}

	static const float GROVE_PART_DIST = 15.0;	// корень или щупальце не дальше этого от своей сердцевины

	// Сердцевина рощи по любой её части (решение пользователя 2026-11-16, вариант Г): сама сердцевина или сетевой корень,
	// щупальце рядом. Обвивки деревьев и корни на домах — локальные объекты клиента (сетевого ID нет): сервер их не
	// знает, поэтому через них ничего не делаем. aliveOnly — только живая (для подношения).
	static VB_AR_Heart VB_HeartOf(Object o, bool aliveOnly = true)
	{
		VB_AR_Heart h = VB_AR_Heart.Cast(o);
		if (h)
			return h;
		VB_AR_Base part = VB_AR_Base.Cast(o);
		if (!part)
			return null;
		int low;
		int high;
		part.GetNetworkID(low, high);
		if (low == 0 && high == 0)
			return null;
		if (!aliveOnly)
			return VB_NearestHeartAny(part.GetPosition(), GROVE_PART_DIST);
		if (g_Game.IsDedicatedServer())
			return VB_AR_Recovery.VB_LiveHeartNear(part.GetPosition(), GROVE_PART_DIST);
		return VB_AR_Heart.Cast(VB_AR_Pulse.VB_NearestLiveHeart(part.GetPosition(), GROVE_PART_DIST));	// Pulse отдаёт VB_AR_Base
	}

	// ближайшая сердцевина, живая или вырезанная (объекты рядом — годится и клиенту, и серверу)
	static VB_AR_Heart VB_NearestHeartAny(vector pos, float maxDist)
	{
		array<Object> objs = new array<Object>;
		g_Game.GetObjectsAtPosition(pos, maxDist, objs, null);
		VB_AR_Heart best;
		float bestD = maxDist * maxDist;
		float d;
		VB_AR_Heart h;
		foreach (Object o : objs)
		{
			h = VB_AR_Heart.Cast(o);
			if (!h)
				continue;
			d = vector.DistanceSq(h.GetPosition(), pos);
			if (d <= bestD)
			{
				bestD = d;
				best = h;
			}
		}
		return best;
	}
}

class VB_ActionOfferRootCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(VB_ActionOfferRoot.OFFER_TIME);
	}
}

class VB_ActionOfferRoot : ActionContinuousBase
{
	static const float OFFER_TIME = 5.0;

	void VB_ActionOfferRoot()
	{
		m_CallbackClass = VB_ActionOfferRootCB;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_FullBody = true;
		m_Text = "Сделать подношение";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINonRuined();
		m_ConditionTarget = new CCTCursor(UAMaxDistances.DEFAULT);
	}

	// ночью, у живой сердцевины, которая ещё принимает дары, с годным даром в руках
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		VB_AR_Heart heart = VB_AR_Offer.VB_HeartOf(target.GetObject());	// сердцевина или любой корень рощи
		if (!heart || heart.VB_IsDead() || heart.VB_OffersLeft() <= 0)
			return false;
		if (VB_AR_Offer.VB_Tier(item) == VB_AR_Offer.TIER_NONE)
			return false;
		// «Проклятому» роща даров не принимает (расположение леса, 2б.11); сервер проверяет ещё раз
		if (!g_Game.IsDedicatedServer() && VB_AR_Favor.VB_MyTier() == VB_AR_Favor.TIER_CURSED && !VB_AR_Favor.s_VB_Pardon)
			return false;

		return VB_AR_Pulse.VB_NightFactor() >= VB_AR_Pulse.NIGHT_FULL_ABOVE;
	}
}

modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(VB_ActionOfferRoot);
	}
}

modded class ItemBase
{
	// подносить можно что угодно из списка — действие у всех предметов, отбор в условии
	override void SetActions()
	{
		super.SetActions();
		AddAction(VB_ActionOfferRoot);
	}
}
