modded class Grenade_Base
{	
	void ForceActivateFuse()
	{
		if (GetGame().IsServer())
		{
			SetHealth("", "", 0.0);
			SetTakeable(false);
		}
	}
}