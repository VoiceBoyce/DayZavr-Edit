class DRL_Craft_Lamp extends Spotlight
{
	static const string LIGHT_BEAM_START = "beamStart";
	static const string LIGHT_BEAM_END   = "beamEnd";

	override void OnWorkStart()
	{
		SpotlightLight light = SpotlightLight.Cast(ScriptedLightBase.CreateLight(SpotlightLight, "0 0 0"));
		if (light)
		{
			light.AttachOnMemoryPoint(this, LIGHT_BEAM_START, LIGHT_BEAM_END);
		}

		super.OnWorkStart();
	}

	override void OnItemLocationChanged(EntityAI old_owner, EntityAI new_owner)
	{
		super.OnItemLocationChanged(old_owner, new_owner);
	}

	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionTurnOnSpotlight);
		AddAction(ActionTurnOffSpotlight);
	}

	override bool CanPutInCargo(EntityAI parent)
	{
		return false;
	}
}