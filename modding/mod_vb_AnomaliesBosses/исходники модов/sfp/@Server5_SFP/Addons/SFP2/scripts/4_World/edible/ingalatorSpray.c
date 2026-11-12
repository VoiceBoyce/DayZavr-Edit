class SFP_ingalator_Tetracycline : Edible_Base
{
	override void OnConsume(float amount, PlayerBase consumer)
	{
		if (consumer.GetModifiersManager().IsModifierActive(eModifiers.MDF_ANTIBIOTICS)) //effectively resets the timer
			consumer.GetModifiersManager().DeactivateModifier(eModifiers.MDF_ANTIBIOTICS);

		consumer.GetModifiersManager().ActivateModifier(eModifiers.MDF_ANTIBIOTICS);
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceConsumeSingle);
		AddAction(ActionEatTabletFromWrapper);
		AddAction(ActionOsmotrItem);
	}
}
//------------------------------------------------------------------------------------------------------------------------------------
class SFP_ingalator_Project01 : Edible_Base
{
	override void OnConsume(float amount, PlayerBase consumer)
	{
		if (consumer.GetModifiersManager().IsModifierActive(eModifiers.MDF_BLOOD_REGEN)) //effectively resets the timer
			consumer.GetModifiersManager().DeactivateModifier(eModifiers.MDF_BLOOD_REGEN);
			if (consumer.GetModifiersManager().IsModifierActive(eModifiers.MDF_SALINE)) //effectively resets the timer
			consumer.GetModifiersManager().DeactivateModifier(eModifiers.MDF_SALINE);
			if (consumer.GetModifiersManager().IsModifierActive(eModifiers.MDF_IMMUNITYBOOST)) //effectively resets the timer
			consumer.GetModifiersManager().DeactivateModifier(eModifiers.MDF_IMMUNITYBOOST);
			if (consumer.GetModifiersManager().IsModifierActive(eModifiers.MDF_ANTIBIOTICS)) //effectively resets the timer
			consumer.GetModifiersManager().DeactivateModifier(eModifiers.MDF_ANTIBIOTICS);
			if (consumer.GetModifiersManager().IsModifierActive(eModifiers.MDF_PAINKILLERS)) //effectively resets the timer
			consumer.GetModifiersManager().DeactivateModifier(eModifiers.MDF_PAINKILLERS);

		consumer.GetModifiersManager().ActivateModifier(eModifiers.MDF_BLOOD_REGEN);
		consumer.GetModifiersManager().ActivateModifier(eModifiers.MDF_SALINE);
		consumer.GetModifiersManager().ActivateModifier(eModifiers.MDF_IMMUNITYBOOST);
		consumer.GetModifiersManager().ActivateModifier(eModifiers.MDF_ANTIBIOTICS);
		consumer.GetModifiersManager().ActivateModifier(eModifiers.MDF_PAINKILLERS);
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceConsumeSingle);
		AddAction(ActionEatTabletFromWrapper);
		AddAction(ActionOsmotrItem);
	}
}
//------------------------------------------------------------------------------------------------------------------------------------
class SFP_ingalator_Project02 : Edible_Base
{
	override void OnConsume(float amount, PlayerBase consumer)
	{
		if (consumer.GetModifiersManager().IsModifierActive(eModifiers.MDF_CONTAMINATION1)) //effectively resets the timer
			consumer.GetModifiersManager().DeactivateModifier(eModifiers.MDF_CONTAMINATION1);
			if (consumer.GetModifiersManager().IsModifierActive(eModifiers.MDF_CONTAMINATION2)) //effectively resets the timer
			consumer.GetModifiersManager().DeactivateModifier(eModifiers.MDF_CONTAMINATION2);
			if (consumer.GetModifiersManager().IsModifierActive(eModifiers.MDF_CONTAMINATION3)) //effectively resets the timer
			consumer.GetModifiersManager().DeactivateModifier(eModifiers.MDF_CONTAMINATION3);
			

		consumer.GetModifiersManager().ActivateModifier(eModifiers.MDF_CONTAMINATION1);
		consumer.GetModifiersManager().ActivateModifier(eModifiers.MDF_CONTAMINATION2);
		consumer.GetModifiersManager().ActivateModifier(eModifiers.MDF_CONTAMINATION3);
		
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceConsumeSingle);
		AddAction(ActionEatTabletFromWrapper);
		AddAction(ActionOsmotrItem);
	}
}
//------------------------------------------------------------------------------------------------------------------------------------
class SFP_ingalator_Morphine : Edible_Base
{
	override void OnConsume(float amount, PlayerBase consumer)
	{
		if (consumer.GetModifiersManager().IsModifierActive(eModifiers.MDF_MORPHINE)) //effectively resets the timer
			consumer.GetModifiersManager().DeactivateModifier(eModifiers.MDF_MORPHINE);

		consumer.GetModifiersManager().ActivateModifier(eModifiers.MDF_MORPHINE);
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceConsumeSingle);
		AddAction(ActionEatTabletFromWrapper);
		AddAction(ActionOsmotrItem);
	}
}