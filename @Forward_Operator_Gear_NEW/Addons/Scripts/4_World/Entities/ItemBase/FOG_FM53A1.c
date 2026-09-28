class FOG_AVONM53_Gasmask_Base: GasMask 
{
    protected void FOG_HideSelection(string selectionName)
    {
        TStringArray selectionNames = new TStringArray;
        ConfigGetTextArray("simpleHiddenSelections",selectionNames);
        int selectionId = selectionNames.Find(selectionName);
        
        if (selectionId != -1)
        {
            SetSimpleHiddenSelectionState(selectionId, true);
        }
    }

    protected void FOG_ShowSelection(string selectionName)
    {
        TStringArray selectionNames = new TStringArray;
        ConfigGetTextArray("simpleHiddenSelections",selectionNames);
        int selectionId = selectionNames.Find(selectionName);
        
        if (selectionId != -1)
        {
            SetSimpleHiddenSelectionState(selectionId, false);
        }
    }
    
    override void EEInit()
    {
        super.EEInit();
        FOG_HideSelection("selection_filter");
        FOG_ShowSelection("selection_hose");
    }

    override void EEItemAttached(EntityAI item, string slot_name)
    {	
        super.EEItemAttached(item,slot_name);
        if (slot_name == "FOG_C420_PAPR")
        {
            FOG_ShowSelection("selection_filter");
            FOG_HideSelection("selection_hose");
        }
    }

    override void EEItemDetached(EntityAI item, string slot_name)
    {	
        super.EEItemDetached(item,slot_name);
        if (slot_name == "FOG_C420_PAPR")
        {
            FOG_HideSelection("selection_filter");
            FOG_ShowSelection("selection_hose");
        }
    }
    
    override bool IsGasMask()
    {
        return true;
    }
    
    override array<int> GetEffectWidgetTypes()
    {
        return {EffectWidgetsTypes.MASK_OCCLUDER, EffectWidgetsTypes.MASK_BREATH};
    }
    
    override bool AllowFoodConsumption()
    {
        return false;
    }
    
    override bool IsObstructingVoice()
    {
        return false;
    }
    
    override int GetVoiceEffect()
    {
        return 0;
    }
    
    override void EEHealthLevelChanged(int oldLevel, int newLevel, string zone)
    {
        super.EEHealthLevelChanged(oldLevel, newLevel, zone);
        
        if (GetGame().IsServer())
        {
            if (newLevel == GameConstants.STATE_RUINED)
            {
                SetQuantity(0);
            }
        }
    }
    
    override EntityAI GetExternalFilter()
    {
        return FindAttachmentBySlotName("FOG_C420_PAPR");
    }
    
    override bool HasValidFilter()
    {
        if (GetQuantity() > 0 && !IsRuined())
            return true;
            
        ItemBase filter = ItemBase.Cast(GetExternalFilter());
        if (filter && !filter.IsRuined() && filter.GetQuantity() > 0)
            return true;

        return false;
    }
    
    override float GetFilterQuantityMax()
    {
        ItemBase filter = ItemBase.Cast(GetExternalFilter());
        if (filter)
            return filter.GetQuantityMax();
        else if (HasQuantity())
            return GetQuantityMax();
        
        return 0;
    }
    
    override float GetFilterQuantity()
    {
        ItemBase filter = ItemBase.Cast(GetExternalFilter());
        if (filter)
            return filter.GetQuantity();
        else
            return GetQuantity();
    }
    
    override float GetFilterQuantity01()
    {
        if (!HasValidFilter())
            return 0;
        
        ItemBase filter = ItemBase.Cast(GetExternalFilter());
        float quantity, quantityMax;
        
        if (filter && filter.GetQuantity() > 0)
        {
            quantity = filter.GetQuantity();
            quantityMax = filter.GetQuantityMax();
        }
        else
        {
            quantity = GetQuantity();
            quantityMax = GetQuantityMax();
        }
        
        float result = Math.InverseLerp(0, quantityMax, quantity);
        
        if (HasQuantity() && GetQuantity() > 0)
        {
            return Math.Max(result, 0.21);
        }
        
        return result;
    }
    
    override bool IsExternalFilterAttached()
    {
        return ItemBase.Cast(FindAttachmentBySlotName("FOG_C420_PAPR")) != null;
    }
    
    override bool HasIntegratedFilter()
    {
        return HasQuantity();
    }
    
    override bool CanHaveExternalFilter()
    {
        return true;
    }
    
    override float GetProtectionLevel(int type, bool consider_filter = false, int system = 0)
    {
        bool maskHasQuantity = HasQuantity() && GetQuantity() > 0 && !IsRuined();
        
        ItemBase filter = ItemBase.Cast(FindAttachmentBySlotName("FOG_C420_PAPR"));
        bool filterHasQuantity = filter && filter.HasQuantity() && filter.GetQuantity() > 0 && !filter.IsRuined();
        
        float maskProtection = 0;
        float filterProtection = 0;
        
        if (maskHasQuantity)
        {
            string basePath = "CfgVehicles " + GetType() + " Protection ";
            
            if (type == 0 || type > 1)
            {
                maskProtection = GetGame().ConfigGetFloat(basePath + "biological");
            }
            else if (type == 1)
            {
                maskProtection = GetGame().ConfigGetFloat(basePath + "chemical");
            }
        }
        
        if (filterHasQuantity)
        {
            filterProtection = filter.GetProtectionLevel(type, false, system);
        }
        
        return Math.Max(maskProtection, filterProtection);
    }

    override bool ConsumeQuantity(float quantity, PlayerBase consumer_player)
    {
        ItemBase filter = ItemBase.Cast(FindAttachmentBySlotName("FOG_C420_PAPR"));
        if (filter && filter.HasQuantity() && filter.GetQuantity() > 0)
        {
            filter.AddQuantity(-quantity);
            OnQuantityConsumed(filter, consumer_player, quantity);
            return true;
        }
        
        if (HasQuantity() && GetQuantity() > 0)
        {
            this.AddQuantity(-quantity);
            OnQuantityConsumed(this, consumer_player, quantity);
            return true;
        }
        
        return false;
    }
    
    override void OnQuantityConsumed(notnull ItemBase filter, PlayerBase consumer_player, float quantity)
    {
        float damage = quantity * filter.GetFilterDamageRatio();
        filter.AddHealth("","", -damage);
    }
    
    override protected void InitGlobalExclusionValues()
    {
        super.InitGlobalExclusionValues();
        
        AddSingleExclusionValueGlobal(EAttExclusions.EXCLUSION_MASK_2);
        AddSingleExclusionValueGlobal(EAttExclusions.EXCLUSION_GLASSES_TIGHT_0);
        AddSingleExclusionValueGlobal(EAttExclusions.EXCLUSION_HEADGEAR_HELMET_0);
    }
};