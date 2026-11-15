// Пути вверх расположения леса (ТЗ_сервер_18.md п. 2б.11) — клиентская часть: предметы и действия.
//   «Отвар примирения» (VB_AR_Reconcile): выпить ночью у живой рощи — расположение +15 и роща разово примет дар даже
//     у «Проклятого». Не чаще раза в сутки (считает сервер; не прошли сутки — отвар не тратится).
//   «Семя Древнего корня» (VB_AR_Seed): посадить у вырезанной сердцевины — гнездо этой рощи встанет на следующем
//     рестарте, расположение +20. Падает при вырезании сердцевины; крафт — в DRL 18 (шаг 6).
// Изгнание нечисти (пауки MSFZ, ERA-зомби) и что делает сервер — VB_AncientRoot_Server, VB_AR_FavorServer.c.
// Модели временные (ампула Живицы, ванильные семена) — свой облик на шаге 4.

class VB_AR_Reconcile extends Inventory_Base
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(VB_ActionDrinkReconcile);
	}
}

class VB_AR_Seed extends Inventory_Base
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(VB_ActionPlantSeed);
	}
}

class VB_AR_Recovery
{
	static const float GROVE_NEAR = 30;		// отвар — у живой рощи не дальше
	static const float SEED_NEAR = 8;		// семя — у вырезанной сердцевины не дальше

	// вырезанная сердцевина рядом (клиент и сервер: объекты рядом, без пульса)
	static VB_AR_Heart VB_DeadHeartNear(vector pos, float maxDist)
	{
		array<Object> objs = new array<Object>;
		g_Game.GetObjectsAtPosition(pos, maxDist, objs, null);
		VB_AR_Heart h;
		foreach (Object o : objs)
		{
			h = VB_AR_Heart.Cast(o);
			if (h && h.VB_IsDead())
				return h;
		}
		return null;
	}

	// живая сердцевина рядом (объекты рядом — годится и серверу, где нет пульса)
	static VB_AR_Heart VB_LiveHeartNear(vector pos, float maxDist)
	{
		array<Object> objs = new array<Object>;
		g_Game.GetObjectsAtPosition(pos, maxDist, objs, null);
		VB_AR_Heart h;
		foreach (Object o : objs)
		{
			h = VB_AR_Heart.Cast(o);
			if (h && !h.VB_IsDead())
				return h;
		}
		return null;
	}
}

class VB_ActionDrinkReconcileCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(VB_ActionDrinkReconcile.DRINK_TIME);
	}
}

class VB_ActionDrinkReconcile : ActionContinuousBase
{
	static const float DRINK_TIME = 4.0;

	void VB_ActionDrinkReconcile()
	{
		m_CallbackClass = VB_ActionDrinkReconcileCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_DRINK;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_DRINK;
		m_Text = "Выпить отвар у рощи";
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
		if (!VB_AR_Reconcile.Cast(item))
			return false;
		if (VB_AR_Pulse.VB_NightFactor() < VB_AR_Pulse.NIGHT_FULL_ABOVE)
			return false;
		// условие считается каждый кадр: на клиенте — список сердцевин пульса, на сервере (пульса нет) — объекты рядом
		if (g_Game.IsDedicatedServer())
			return VB_AR_Recovery.VB_LiveHeartNear(player.GetPosition(), VB_AR_Recovery.GROVE_NEAR) != null;
		return VB_AR_Pulse.VB_NearestLiveHeart(player.GetPosition(), VB_AR_Recovery.GROVE_NEAR) != null;
	}
}

class VB_ActionPlantSeedCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(VB_ActionPlantSeed.PLANT_TIME);
	}
}

class VB_ActionPlantSeed : ActionContinuousBase
{
	static const float PLANT_TIME = 6.0;

	void VB_ActionPlantSeed()
	{
		m_CallbackClass = VB_ActionPlantSeedCB;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;	// DIGMANIPULATE только стоя, а сажают присев
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_FullBody = true;
		m_Text = "Посадить семя на месте рощи";
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
		if (!VB_AR_Seed.Cast(item))
			return false;
		return VB_AR_Recovery.VB_DeadHeartNear(player.GetPosition(), VB_AR_Recovery.SEED_NEAR) != null;
	}
}

modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(VB_ActionDrinkReconcile);
		actions.Insert(VB_ActionPlantSeed);
	}
}
