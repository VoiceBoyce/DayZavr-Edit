class FOG_Bag_Base extends Backpack_Base
{
    override void SetActions()
    {
        super.SetActions();
        AddAction(ActionWringClothes);
    }
}; 
class FOG_Bag_BlackJack_ColorBase : FOG_Bag_Base
{
    const int                         SIMPLE_SELECTION_MELEE_RIFLE = 0;
    const int                         SIMPLE_SELECTION_MELEE_MELEE = 1;
    const int                         SIMPLE_SELECTION_SHOULDER_RIFLE = 2;
    const int                         SIMPLE_SELECTION_SHOULDER_MELEE = 3;

    void UpdateShoulderProxyVisibility(EntityAI item, string slot_name)
    {
        string slot = slot_name;
        bool boo;
        boo = item.IsWeapon();
        
        if ( slot == "Melee" )
        {
            SetSimpleHiddenSelectionState(SIMPLE_SELECTION_MELEE_RIFLE,boo);
            SetSimpleHiddenSelectionState(SIMPLE_SELECTION_MELEE_MELEE,!boo);
        }
        else if ( slot == "Shoulder" )
        {
            SetSimpleHiddenSelectionState(SIMPLE_SELECTION_SHOULDER_RIFLE,boo);
            SetSimpleHiddenSelectionState(SIMPLE_SELECTION_SHOULDER_MELEE,!boo);
        }
    }
    
    override void EEItemAttached(EntityAI item, string slot_name)
    {
        super.EEItemAttached(item, slot_name);

        UpdateShoulderProxyVisibility(item, slot_name);
    }   
};

class FOG_Bag_6ShTBag_ColorBase extends FOG_Bag_Base {};
class FOG_Bergen_Rucksack_ColorBase extends FOG_Bag_Base {};
class FOG_Bag_DrawBridge_ColorBase extends FOG_Bag_Base {};
class FOG_Bag_F4Terminator_ColorBase extends FOG_Bag_Base {};
class FOG_Bag_MRASAP_ColorBase extends FOG_Bag_Base {};
class FOG_Bag_SATL_ColorBase extends FOG_Bag_Base {};
class FOG_Bag_SSOBag_ColorBase extends FOG_Bag_Base {};
class FOG_Bag_Trizip_ColorBase extends FOG_Bag_Base {};
class FOG_Bag_CommsBag_Base extends FOG_Bag_Base {};
class FOG_Bag_ArcteryxLEAF_ColorBase extends FOG_Bag_Base {};
class FOG_Bag_LBT1475A_ColorBase extends FOG_Bag_Base {};

class FOG_Bag_CommsBag_Transmitter_Base extends BaseRadio {};