class OOTS_item_PsyBox: Container_Base
{
	bool HasComponents()
	{
		array<string> attachmenet_slots = {"PsyTower_osnova", "PsyTower_battery"};
		
		foreach(string slot: attachmenet_slots)
		{
			EntityAI attachment = FindAttachmentBySlotName(slot);
			if(!attachment || attachment.IsRuined())
				return false;
		}
		
		return true;
	}
	
	string GetHologram()
	{
		return "OOTS_item_PsyTower";
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionTogglePlacePsiTower); //action toggle place
		AddAction(ActionPlacePsiTower); //action place
	}
}

class OOTS_item_PsyTower: ItemBase
{
	protected bool m_IsActive = false;
	protected PointLightBase m_Light;
	
	void OOTS_item_PsyTower()
	{
		RegisterNetSyncVariableBool("m_IsActive");
	}

	void ~OOTS_item_PsyTower()
	{
		if ( m_Light ) m_Light.FadeOut();
	}
	
	bool IsActive()
	{
		return m_IsActive;
	}
	
	protected void SetActive()
	{
		
		if(m_IsActive)
		{
			//TODO Make a sound
		
			//TODO Make a light
			if ( !m_Light )
			{
				m_Light = PsyTower_Light.Cast( ScriptedLightBase.CreateLight(PsyTower_Light, "0 0.4 0", 3) );
				m_Light.AttachOnObject(this, "0 0.4 0", "0 0 0");
			}
		}
		else
		{
			//TODO Remove a sound
		
			//TODO Remove a light
			if ( m_Light ) m_Light.FadeOut();
		}
	}
	
	override bool CanPutInCargo(EntityAI parent)
	{
		if(!super.CanPutInCargo(parent))
			return false;
		
		if(m_IsActive)
			return false;
		
		return true;
	}
	
	override bool CanPutIntoHands(EntityAI parent)
	{
		if(!super.CanPutIntoHands(parent))
			return false;
		
		if(m_IsActive)
			return false;
		
		return true;
	}
		
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		
		SetActive();
	}
}

class OOTS_item_PsyDecoder: ItemBase 
{
	override void SetActions()
	{
		super.SetActions();

		AddAction(ActionTurnOnWhileInHands);
		AddAction(ActionTurnOffWhileInHands);
	}
}

class PsyTower_Light extends PointLightBase
{		
	void PsyTower_Light()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo( 16 );
		SetBrightnessTo( 5 );
		SetCastShadow(false);
		SetFadeOutTime(3);
		SetDiffuseColor(0.8, 0.8, 1.0);
		SetAmbientColor(0.8, 0.8, 1.0);
		SetFlareVisible(true);
		SetFlickerAmplitude(0.5);
		SetFlickerSpeed(1.2);
		SetDancingShadowsMovementSpeed(1.2);
		SetDancingShadowsAmplitude(0.1);
	}
}