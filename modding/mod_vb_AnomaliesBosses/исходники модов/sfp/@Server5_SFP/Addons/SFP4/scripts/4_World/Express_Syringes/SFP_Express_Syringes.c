// ================================================================
// Express Syringes - items
// ================================================================
// Base class: SFP_Express_Syringe_Base (inherits from Edible_Base)
//
// Effects on consume:
//  - Restores health (configurable per subclass)
//  - Restores blood (configurable per subclass)
//  - Applies painkiller effect (MDF_PAINKILLERS), timer reset
//
// Note:
//  - Item removal after use is handled by ActionUseExpressSyringe
// ================================================================

class SFP_Express_Syringe_Base extends Edible_Base
{
	protected float GetExpressHealthGain()
	{
		return 0.0;
	}

	protected float GetExpressBloodGain()
	{
		return 0.0;
	}

	override void SetActions()
	{
		super.SetActions();

		// Tablet animation, mask-friendly, moving-friendly
		AddAction(ActionUseExpressSyringe);
	}

	override void OnConsume(float amount, PlayerBase consumer)
	{
		super.OnConsume(amount, consumer);

		if (!consumer)
			return;

		float hpGain = GetExpressHealthGain();
		float bloodGain = GetExpressBloodGain();

		if (hpGain != 0)
			consumer.AddHealth("", "Health", hpGain);

		if (bloodGain != 0)
			consumer.AddHealth("", "Blood", bloodGain);

			// Painkiller effect (+reset)
		ModifiersManager mm = consumer.GetModifiersManager();
		if (mm)
		{
			if (mm.IsModifierActive(eModifiers.MDF_PAINKILLERS))
				mm.DeactivateModifier(eModifiers.MDF_PAINKILLERS);

			mm.ActivateModifier(eModifiers.MDF_PAINKILLERS);

				// Blood regeneration (+reset) - complements the instant blood restore above
				if (mm.IsModifierActive(eModifiers.MDF_BLOOD_REGEN))
					mm.DeactivateModifier(eModifiers.MDF_BLOOD_REGEN);
				mm.ActivateModifier(eModifiers.MDF_BLOOD_REGEN);
		}
	}
}

class SFP_Express_Syringe25 extends SFP_Express_Syringe_Base
{
	override protected float GetExpressHealthGain()
	{
		return 25.0;
	}

	override protected float GetExpressBloodGain()
	{
		return 250.0;
	}
}

class SFP_Express_Syringe50 extends SFP_Express_Syringe_Base
{
	override protected float GetExpressHealthGain()
	{
		return 50.0;
	}

	override protected float GetExpressBloodGain()
	{
		return 500.0;
	}
}

class SFP_Express_Syringe75 extends SFP_Express_Syringe_Base
{
	override protected float GetExpressHealthGain()
	{
		return 75.0;
	}

	override protected float GetExpressBloodGain()
	{
		return 750.0;
	}
}

class SFP_Express_Syringe100 extends SFP_Express_Syringe_Base
{
	override protected float GetExpressHealthGain()
	{
		return 100.0;
	}

	override protected float GetExpressBloodGain()
	{
		return 1000.0;
	}
}
