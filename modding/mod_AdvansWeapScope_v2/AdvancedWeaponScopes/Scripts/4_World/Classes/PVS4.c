class AD_PVS4 extends ItemOptics
{
	override void InitOpticMode()
	{
		super.InitOpticMode();
		
		SetCurrentOpticMode(GameConstants.OPTICS_STATE_NIGHTVISION);
	}
	
	override int GetCurrentNVType()
	{
		if (IsWorking())
		{
			switch (m_CurrentOpticMode)
			{
				case GameConstants.OPTICS_STATE_DAY:
					return NVTypes.NV_OPTICS_STARLIGHT_DAY;
				
				case GameConstants.OPTICS_STATE_NIGHTVISION:
					return NVTypes.NV_OPTICS_STARLIGHT_NIGHT;
			}
			Error("Undefined optic mode of " + this);
			return NVTypes.NONE;
		}
		else
		{
			return NVTypes.NV_OPTICS_OFF;
		}
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionToggleNVMode);
	}
}; 
class AD_PVS4_B13 extends ItemOptics
{
	override void InitOpticMode()
	{
		super.InitOpticMode();
		
		SetCurrentOpticMode(GameConstants.OPTICS_STATE_NIGHTVISION);
	}
	
	override int GetCurrentNVType()
	{
		if (IsWorking())
		{
			switch (m_CurrentOpticMode)
			{
				case GameConstants.OPTICS_STATE_DAY:
					return NVTypes.NV_OPTICS_KAZUAR_DAY;
				
				case GameConstants.OPTICS_STATE_NIGHTVISION:
					return NVTypes.NV_OPTICS_KAZUAR_NIGHT;
			}
			Error("Undefined optic mode of " + this);
			return NVTypes.NONE;
		}
		else
		{
			return NVTypes.NV_OPTICS_OFF;
		}
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionToggleNVMode);
	}
}; 
class AD_PVS4_MosinMount : ItemOptics 
{
	override void InitOpticMode()
	{
		super.InitOpticMode();
		
		SetCurrentOpticMode(GameConstants.OPTICS_STATE_NIGHTVISION);
	}
	
	override int GetCurrentNVType()
	{
		if (IsWorking())
		{
			switch (m_CurrentOpticMode)
			{
				case GameConstants.OPTICS_STATE_DAY:
					return NVTypes.NV_OPTICS_KAZUAR_DAY;
				
				case GameConstants.OPTICS_STATE_NIGHTVISION:
					return NVTypes.NV_OPTICS_KAZUAR_NIGHT;
			}
			Error("Undefined optic mode of " + this);
			return NVTypes.NONE;
		}
		else
		{
			return NVTypes.NV_OPTICS_OFF;
		}
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionToggleNVMode);
	}
};