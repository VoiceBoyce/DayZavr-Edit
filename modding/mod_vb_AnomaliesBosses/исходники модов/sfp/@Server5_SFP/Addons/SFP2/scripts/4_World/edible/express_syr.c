/*class SFP_Express_Syringe_Base : Edible_Base
{
    /*override void OnConsume(float amount, PlayerBase consumer)
	{
		if (consumer.GetModifiersManager().IsModifierActive(eModifiers.MDF_PAINKILLERS)) //effectively resets the timer
			consumer.GetModifiersManager().DeactivateModifier(eModifiers.MDF_PAINKILLERS);
			(consumer.GetModifiersManager().IsModifierActive(eModifiers.MDF_SALINE)) //effectively resets the timer
			consumer.GetModifiersManager().DeactivateModifier(eModifiers.MDF_SALINE);

		consumer.GetModifiersManager().ActivateModifier(eModifiers.MDF_PAINKILLERS);
		consumer.GetModifiersManager().ActivateModifier(eModifiers.MDF_SALINE);
	}
	*/
/*	override void OnConsume( float amount, PlayerBase consumer )
    {
        float    v_PlayerHealth;
        
        if ( consumer.GetModifiersManager().IsModifierActive( eModifiers.MDF_PAINKILLERS ))
        {
            consumer.GetModifiersManager().DeactivateModifier( eModifiers.MDF_PAINKILLERS );
        }
        if (consumer.GetModifiersManager().IsModifierActive( eModifiers.MDF_SALINE ) )
        {
            consumer.GetModifiersManager().DeactivateModifier( eModifiers.MDF_SALINE );
        }
        consumer.GetModifiersManager().ActivateModifier( eModifiers.MDF_PAINKILLERS );
        consumer.GetModifiersManager().ActivateModifier( eModifiers.MDF_SALINE );
        
        v_PlayerHealth    =    consumer.GetHealth( "GlobalHealth", "Health" );
        consumer.SetHealth( "GlobalHealth", "Health", v_PlayerHealth + 20 );
    }
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceConsumeSingle);
		AddAction(ActionEatTabletFromWrapper);
		AddAction(ActionOsmotrItem);
	}
}
class SFP_Express_Syringe25 : Edible_Base
{
    override void OnConsume( float amount, PlayerBase consumer )
    {
        float    v_PlayerHealth;
        
        if ( consumer.GetModifiersManager().IsModifierActive( eModifiers.MDF_PAINKILLERS ))
        {
            consumer.GetModifiersManager().DeactivateModifier( eModifiers.MDF_PAINKILLERS );
        }
        if (consumer.GetModifiersManager().IsModifierActive( eModifiers.MDF_SALINE ) )
        {
            consumer.GetModifiersManager().DeactivateModifier( eModifiers.MDF_SALINE );
        }
        consumer.GetModifiersManager().ActivateModifier( eModifiers.MDF_PAINKILLERS );
        consumer.GetModifiersManager().ActivateModifier( eModifiers.MDF_SALINE );
        
        v_PlayerHealth    =    consumer.GetHealth( "GlobalHealth", "Health" );
        consumer.SetHealth( "GlobalHealth", "Health", v_PlayerHealth + 25 );
    }
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceConsumeSingle);
		AddAction(ActionEatTabletFromWrapper);
	}
} 
class SFP_Express_Syringe50 : Edible_Base
{
    override void OnConsume( float amount, PlayerBase consumer )
    {
        float    v_PlayerHealth;
        
        if ( consumer.GetModifiersManager().IsModifierActive( eModifiers.MDF_PAINKILLERS ))
        {
            consumer.GetModifiersManager().DeactivateModifier( eModifiers.MDF_PAINKILLERS );
        }
        if (consumer.GetModifiersManager().IsModifierActive( eModifiers.MDF_SALINE ) )
        {
            consumer.GetModifiersManager().DeactivateModifier( eModifiers.MDF_SALINE );
        }
        consumer.GetModifiersManager().ActivateModifier( eModifiers.MDF_PAINKILLERS );
        consumer.GetModifiersManager().ActivateModifier( eModifiers.MDF_SALINE );
        
        v_PlayerHealth    =    consumer.GetHealth( "GlobalHealth", "Health" );
        consumer.SetHealth( "GlobalHealth", "Health", v_PlayerHealth + 50 );
    }
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceConsumeSingle);
		AddAction(ActionEatTabletFromWrapper);
	}
} 
class SFP_Express_Syringe75 : Edible_Base
{
   override void OnConsume( float amount, PlayerBase consumer )
    {
        float    v_PlayerHealth;
        
        if ( consumer.GetModifiersManager().IsModifierActive( eModifiers.MDF_PAINKILLERS ))
        {
            consumer.GetModifiersManager().DeactivateModifier( eModifiers.MDF_PAINKILLERS );
        }
        if (consumer.GetModifiersManager().IsModifierActive( eModifiers.MDF_SALINE ) )
        {
            consumer.GetModifiersManager().DeactivateModifier( eModifiers.MDF_SALINE );
        }
        consumer.GetModifiersManager().ActivateModifier( eModifiers.MDF_PAINKILLERS );
        consumer.GetModifiersManager().ActivateModifier( eModifiers.MDF_SALINE );
        
        v_PlayerHealth    =    consumer.GetHealth( "GlobalHealth", "Health" );
        consumer.SetHealth( "GlobalHealth", "Health", v_PlayerHealth + 75 );
    }
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceConsumeSingle);
		AddAction(ActionEatTabletFromWrapper);
	}
} 
class SFP_Express_Syringe100 : Edible_Base
{
    override void OnConsume( float amount, PlayerBase consumer )
    {
        float    v_PlayerHealth;
        
        if ( consumer.GetModifiersManager().IsModifierActive( eModifiers.MDF_PAINKILLERS ))
        {
            consumer.GetModifiersManager().DeactivateModifier( eModifiers.MDF_PAINKILLERS );
        }
        if (consumer.GetModifiersManager().IsModifierActive( eModifiers.MDF_SALINE ) )
        {
            consumer.GetModifiersManager().DeactivateModifier( eModifiers.MDF_SALINE );
        }
        consumer.GetModifiersManager().ActivateModifier( eModifiers.MDF_PAINKILLERS );
        consumer.GetModifiersManager().ActivateModifier( eModifiers.MDF_SALINE );
        
        v_PlayerHealth    =    consumer.GetHealth( "GlobalHealth", "Health" );
        consumer.SetHealth( "GlobalHealth", "Health", v_PlayerHealth + 100 );
    }
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceConsumeSingle);
		AddAction(ActionEatTabletFromWrapper);
	}
} 