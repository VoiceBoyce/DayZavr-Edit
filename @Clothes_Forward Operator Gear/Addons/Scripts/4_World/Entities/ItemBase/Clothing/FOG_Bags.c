class FOG_Bag_BlackJack_ColorBase : Backpack_Base
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
class FOG_Bag_BlackJack_MC extends FOG_Bag_BlackJack_ColorBase {};
class FOG_Bag_BlackJack_RG extends FOG_Bag_BlackJack_ColorBase {};
class FOG_Bag_BlackJack_CB extends FOG_Bag_BlackJack_ColorBase {};
class FOG_Bag_BlackJack_MCB extends FOG_Bag_BlackJack_ColorBase {};
class FOG_Bag_BlackJack_ERDL extends FOG_Bag_BlackJack_ColorBase {};
class FOG_Bag_BlackJack_Black extends FOG_Bag_BlackJack_ColorBase {};
class FOG_Bag_BlackJack_MCT extends FOG_Bag_BlackJack_ColorBase {};
class FOG_Bag_BlackJack_MCAL extends FOG_Bag_BlackJack_ColorBase {};
class FOG_Bag_BlackJack_OD extends FOG_Bag_BlackJack_ColorBase {};
class FOG_Bag_BlackJack_KHK extends FOG_Bag_BlackJack_ColorBase {};

class FOG_Bag_6ShTBag_ColorBase extends Backpack_Base {};
class FOG_Bag_6ShBagT_Green extends FOG_Bag_6ShTBag_ColorBase {};
class FOG_Bag_6ShBagT_BlackEMR extends FOG_Bag_6ShTBag_ColorBase {};
class FOG_Bag_6ShBagT_Black extends FOG_Bag_6ShTBag_ColorBase {};
class FOG_Bag_6ShBagT_RG extends FOG_Bag_6ShTBag_ColorBase {};
class FOG_Bag_6ShBagT_CB extends FOG_Bag_6ShTBag_ColorBase {};
class FOG_Bag_6ShBagT_White extends FOG_Bag_6ShTBag_ColorBase {};
class FOG_Bag_6ShBagT_EMR_White extends FOG_Bag_6ShTBag_ColorBase {};

class FOG_Bergen_Rucksack_ColorBase extends Backpack_Base {};
class FOG_Bergen_Rucksack_Black extends FOG_Bergen_Rucksack_ColorBase {};
class FOG_Bergen_Rucksack_Green extends FOG_Bergen_Rucksack_ColorBase {};
class FOG_Bergen_Rucksack_Woodland extends FOG_Bergen_Rucksack_ColorBase {};
class FOG_Bergen_Rucksack_Multicam extends FOG_Bergen_Rucksack_ColorBase {};
class FOG_Bergen_Rucksack_ERDL extends FOG_Bergen_Rucksack_ColorBase {};

class FOG_Bag_DrawBridge_ColorBase extends Backpack_Base {};
class FOG_Bag_DrawBridge_CB extends FOG_Bag_DrawBridge_ColorBase {};
class FOG_Bag_DrawBridge_RG extends FOG_Bag_DrawBridge_ColorBase {};
class FOG_Bag_DrawBridge_OD extends FOG_Bag_DrawBridge_ColorBase {};
class FOG_Bag_DrawBridge_Tan extends FOG_Bag_DrawBridge_ColorBase {};
class FOG_Bag_DrawBridge_Black extends FOG_Bag_DrawBridge_ColorBase {};
class FOG_Bag_DrawBridge_Grey extends FOG_Bag_DrawBridge_ColorBase {};
class FOG_Bag_DrawBridge_MC extends FOG_Bag_DrawBridge_ColorBase {};
class FOG_Bag_DrawBridge_MCAL extends FOG_Bag_DrawBridge_ColorBase {};
class FOG_Bag_DrawBridge_MCB extends FOG_Bag_DrawBridge_ColorBase {};
class FOG_Bag_DrawBridge_White extends FOG_Bag_DrawBridge_ColorBase {};

class FOG_Bag_F4Terminator_ColorBase extends Backpack_Base {};
class FOG_Bag_F4Terminator extends FOG_Bag_F4Terminator_ColorBase {};
class FOG_Bag_F4Terminator_BlackTiger extends FOG_Bag_F4Terminator_ColorBase {};
class FOG_Bag_F4Terminator_Black extends FOG_Bag_F4Terminator_ColorBase {};
class FOG_Bag_F4Terminator_RG extends FOG_Bag_F4Terminator_ColorBase {};
class FOG_Bag_F4Terminator_OD extends FOG_Bag_F4Terminator_ColorBase {};
class FOG_Bag_F4Terminator_CB extends FOG_Bag_F4Terminator_ColorBase {};

class FOG_Bag_MRASAP_ColorBase extends Backpack_Base {};
class FOG_Bag_MRASAP_Black extends FOG_Bag_MRASAP_ColorBase {};
class FOG_Bag_MRASAP_Grey extends FOG_Bag_MRASAP_ColorBase {};
class FOG_Bag_MRASAP_RG extends FOG_Bag_MRASAP_ColorBase {};
class FOG_Bag_MRASAP_CB extends FOG_Bag_MRASAP_ColorBase {};
class FOG_Bag_MRASAP_MC extends FOG_Bag_MRASAP_ColorBase {};

class FOG_Bag_SATL_ColorBase extends Backpack_Base {};
class FOG_Bag_SATL_MC extends FOG_Bag_SATL_ColorBase {};
class FOG_Bag_SATL_RG extends FOG_Bag_SATL_ColorBase {};
class FOG_Bag_SATL_CB extends FOG_Bag_SATL_ColorBase {};
class FOG_Bag_SATL_KHK extends FOG_Bag_SATL_ColorBase {};
class FOG_Bag_SATL_Black extends FOG_Bag_SATL_ColorBase {};
class FOG_Bag_SATL_Grey extends FOG_Bag_SATL_ColorBase {};

class FOG_Bag_SSOBag_ColorBase extends Backpack_Base {};
class FOG_Bag_SSO_Tan extends FOG_Bag_SSOBag_ColorBase {};
class FOG_Bag_SSO_Black extends FOG_Bag_SSOBag_ColorBase {};
class FOG_Bag_SSO_MC extends FOG_Bag_SSOBag_ColorBase {};
class FOG_Bag_SSO_CB extends FOG_Bag_SSOBag_ColorBase {};
class FOG_Bag_SSO_MCB extends FOG_Bag_SSOBag_ColorBase {};
class FOG_Bag_SSO_Toxic extends FOG_Bag_SSOBag_ColorBase {};

class FOG_Bag_Trizip_ColorBase extends Backpack_Base {};
class FOG_Bag_Trizip_Puke extends FOG_Bag_Trizip_ColorBase {};
class FOG_Bag_Trizip_Black extends FOG_Bag_Trizip_ColorBase {};
class FOG_Bag_Trizip_RG extends FOG_Bag_Trizip_ColorBase {};
class FOG_Bag_Trizip_OD extends FOG_Bag_Trizip_ColorBase {};
class FOG_Bag_Trizip_CB extends FOG_Bag_Trizip_ColorBase {};
class FOG_Bag_Trizip_RGCB extends FOG_Bag_Trizip_ColorBase {};
class FOG_Bag_Trizip_MARPAT extends FOG_Bag_Trizip_ColorBase {};
class FOG_Bag_Trizip_ATACSFG extends FOG_Bag_Trizip_ColorBase {};
class FOG_Bag_Trizip_DCU extends FOG_Bag_Trizip_ColorBase {};
class FOG_Bag_Trizip_ERDL extends FOG_Bag_Trizip_ColorBase {};
class FOG_Bag_Trizip_MC extends FOG_Bag_Trizip_ColorBase {};