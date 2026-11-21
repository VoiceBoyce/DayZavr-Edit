modded class PlayerBase
{
	override void OnStanceChange(int previousStance, int newStance)
	{
		super.OnStanceChange(previousStance, newStance);

		UpdateWeaponState(newStance == DayZPlayerConstants.STANCEIDX_PRONE || newStance == DayZPlayerConstants.STANCEIDX_RAISEDPRONE);
	}

	void UpdateWeaponState(bool s)
	{
		Weapon_Base wpn = Weapon_Base.Cast(GetItemInHands());

		if (wpn)
			wpn.SetOwnerState(s);
	}
}