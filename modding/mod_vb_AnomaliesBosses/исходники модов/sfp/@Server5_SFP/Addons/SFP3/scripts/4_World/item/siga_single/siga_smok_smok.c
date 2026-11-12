
class SFP_SingleCigarette : Edible_Base
{
    override void OnConsume( float amount, PlayerBase consumer )
    {
        float    v_PlayerHealth;
        
        if ( consumer.GetModifiersManager().IsModifierActive( eModifiers.MDF_PAINKILLERS ))
        {
            consumer.GetModifiersManager().DeactivateModifier( eModifiers.MDF_PAINKILLERS );
        }
        
        consumer.GetModifiersManager().ActivateModifier( eModifiers.MDF_PAINKILLERS );
        
        v_PlayerHealth    =    consumer.GetHealth( "GlobalHealth", "Health" );
        consumer.SetHealth( "GlobalHealth", "Health", v_PlayerHealth + 4 );
    }
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionSmokSmokers);
		AddAction(ActionOsmotrItem);
	}
} 
