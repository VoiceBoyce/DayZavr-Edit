modded class Weapon_Base
{
	// 1.30.164200: a modded vanilla class can not name KOD_Bipod_Base in a field or a signature
	// ("Bad type 'KOD_Bipod_Base'" when the World module links Weapon_Base); the cast lives in the bodies.
	private ItemBase bipods;
	private bool ownerInProne;

	bool HasBipods()
	{
		return bipods != null;
	}

	bool HasBipodsAndProne(out float rec)
	{
		KOD_Bipod_Base bip = KOD_Bipod_Base.Cast(bipods);
		if (bip && ownerInProne)
		{
			rec = bip.Recoil;
			return true;
		}
		return false;
	}

	void SetBipods(ItemBase _bip)
	{
		bipods = _bip;
	}

	ItemBase GetBipods()
	{
		return bipods;
	}

	void SetOwnerState(bool s)
	{
		ownerInProne = s;
		KOD_Bipod_Base bip = KOD_Bipod_Base.Cast(bipods);
		if (bip)
			bip.UpdateVisual(ownerInProne);
	}

	override void EEItemAttached (EntityAI item, string slot_name)
	{
		super.EEItemAttached(item, slot_name);

		KOD_Bipod_Base bip = KOD_Bipod_Base.Cast(item);
		if (bip)
		{
			SetBipods(bip);
			bip.UpdateVisual(ownerInProne);
		}
	}

	override void EEItemDetached (EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);

		KOD_Bipod_Base bip = KOD_Bipod_Base.Cast(item);
		if (bip)
		{
			bip.UpdateVisual(false);
			SetBipods(null);
		}
	}
}
