class SFP_Drink_Buster extends Bottle_Base
{
	override void SetActions()
	{
		super.SetActions();

		// Сохраняем только нужные действия напитка и возвращаем базовые Take/Drop.
		RemoveAction(ActionFillFuel);
		RemoveAction(ActionFillCoolant);
		RemoveAction(ActionFillGeneratorTank);
		RemoveAction(ActionExtinguishFireplaceByLiquid);
		// 1.30: ActionFillBottleBase is no longer registered (RemoveAction on it breaks SetActions); filling is ActionObtainLiquidBase now
		RemoveAction(ActionObtainLiquidBase);
		RemoveAction(ActionFillBottleSnow);
		RemoveAction(ActionWaterGardenSlot);
		RemoveAction(ActionWaterPlant);
		RemoveAction(ActionDrainLiquid);
		RemoveAction(ActionPourLiquid);
		RemoveAction(ActionWashHandsItemContinuous);
		// 1.30: new Bottle_Base actions, removed to keep the 1.29 behaviour (drink only)
		RemoveAction(ActionWashHeadItemContinuous);
		RemoveAction(ActionWetClothingFromContainer);
		RemoveAction(ActionEmptyBottleBase);
	}

	protected void Buster_DeleteEmpty()
	{
		if (GetGame() && GetGame().IsServer())
			GetGame().ObjectDelete(this);
	}

	override void OnConsume(float amount, PlayerBase consumer)
	{
		super.OnConsume(amount, consumer);

		if (consumer)
		{
			ModifiersManager mm = consumer.GetModifiersManager();
			if (mm)
			{
				BusterMdfr mdfr = BusterMdfr.Cast(mm.GetModifier(BusterWorldConstants.MDF_BUSTER));
				if (mdfr)
				{
					float qMax = GetQuantityMax();
					float portion = 1.0;
					if (qMax > 0) portion = Math.Clamp(amount / qMax, 0, 1);

					float base = Math.Max(BusterMdfr.MIN_LIFETIME_ON_DRINK, BusterMdfr.BASE_LIFETIME_FULL * portion);
					float add  = Math.Max(1, BusterMdfr.ADD_LIFETIME_FULL * portion);

					if (mdfr.IsActive()) mdfr.AddLifetime(add);
					else
					{
						if (mdfr.GetLifetime() > 0) mdfr.AddLifetime(add);
						else mdfr.SetLifetime(base + add);

						mm.ActivateModifier(BusterWorldConstants.MDF_BUSTER);
					}
				}
			}
		}

		if (GetGame() && GetGame().IsServer())
		{
			if (GetQuantity() <= 0.0)
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(Buster_DeleteEmpty, 1, false);
		}
	}
}
