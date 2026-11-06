modded class ModItemRegisterCallbacks
{
    override void RegisterOneHanded(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
    {
		super.RegisterOneHanded(pType, pBehavior);
	    pType.AddItemInHandsProfileIK("Ri_LaserDesignator_Base","dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/gear/flashlight.anm");
	}
}


modded class DayZPlayerImplementAiming
{
	float Getiy()
	{
		return y_axiss;
	}
	float Getix()
	{
		return x_axiss;
	}
	float x_axiss;
	float y_axiss;
	override protected void ApplyHorizontalNoise(out float x_axis, out float y_axis, float smooth_time,float max_velocity_low, float max_velocity_high, float velocity_modifier,  float max_distance, float weight, float pDt)
	{
		super.ApplyHorizontalNoise( x_axis,  y_axis,  smooth_time, max_velocity_low,  max_velocity_high,  velocity_modifier,   max_distance,  weight,  pDt);
		x_axiss = x_axis;
	}

	override protected void ApplyBreathingPattern(out float x_axis, out float y_axis, float pAmplitude, float pTotalTime, float weight)
	{
		super.ApplyBreathingPattern(x_axis,y_axis,pAmplitude,pTotalTime,weight);
		y_axiss = y_axis;
	}
}
