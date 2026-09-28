class FillPAPR_Filter extends RecipeBase	
{	
    override void Init()
    {
        m_Name = "Refill PAPR Filter";
        m_IsInstaRecipe = false;
        m_AnimationLength = 0.5;
        m_Specialty = 0.02;
        
        m_MinDamageIngredient[0] = -1;
        m_MaxDamageIngredient[0] = 3;
        
        m_MinQuantityIngredient[0] = 1;
        m_MaxQuantityIngredient[0] = -1;
        
        m_MinDamageIngredient[1] = -1;
        m_MaxDamageIngredient[1] = 3;
        
        m_MinQuantityIngredient[1] = -1;
        m_MaxQuantityIngredient[1] = -1;
        
        InsertIngredient(0,"CharcoalTablets");

        m_IngredientAddHealth[0] = 0;
        m_IngredientSetHealth[0] = -1;
        m_IngredientAddQuantity[0] = 0;
        m_IngredientDestroy[0] = false;
        m_IngredientUseSoftSkills[0] = false;
        
        InsertIngredient(1,"FOG_C420PAPR_Filter");
        
        m_IngredientAddHealth[1] = 0;
        m_IngredientSetHealth[1] = -1;
        m_IngredientAddQuantity[1] = 0;
        m_IngredientDestroy[1] = false;
        m_IngredientUseSoftSkills[1] = false;
    }

    override bool CanDo(ItemBase ingredients[], PlayerBase player)
    {
        ItemBase filter = ingredients[1];
        
        if (filter.GetQuantity() >= filter.GetQuantityMax())
        {
            return false;
        }		
        
        InventoryLocation il = new InventoryLocation;
        filter.GetInventory().GetCurrentInventoryLocation(il);
        EntityAI inv = il.GetParent();
        
        if (inv != null)
        {
            if (inv.IsKindOf("FOG_AVONM53_Gasmask_Base"))
            {	
                return false;
            }
        }		
        
        return true;
    }

    override void Do(ItemBase ingredients[], PlayerBase player, array<ItemBase> results, float specialty_weight)
    {
        ItemBase charcoal = ingredients[0];	
        float charcoalCurrent = charcoal.GetQuantity();
        
        ItemBase filter = ingredients[1];
        
        float filterMax = filter.GetQuantityMax();
        float filterCurrent = filter.GetQuantity();
        
        float fillAmount = filterCurrent + (charcoalCurrent * 10);
        
        if (fillAmount >= filterMax)
        {
            ingredients[1].SetQuantity(filterMax);
            
            fillAmount = fillAmount - filterMax;
            ingredients[0].SetQuantity(Math.Round(fillAmount / 10));
        }
        else
        {		
            ingredients[1].SetQuantity(fillAmount);
            ingredients[0].AddQuantity(-charcoalCurrent);
        }
    }
};