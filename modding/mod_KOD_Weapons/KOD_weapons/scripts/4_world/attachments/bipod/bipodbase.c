class KOD_Bipod_Base : Inventory_Base
{
	float Recoil;

	void KOD_Bipod_Base()
	{
		UpdateVisual(false);
		Recoil = GetRecoil();
	}

	void UpdateVisual(bool s)
	{
		if (s)
		{
			SetSimpleHiddenSelectionState(0, false);
			SetSimpleHiddenSelectionState(1, true);
		}
		else
		{
			SetSimpleHiddenSelectionState(0, true);
			SetSimpleHiddenSelectionState(1, false);
		}
	}

	float GetRecoil()
	{
		float value;
		if(ConfigIsExisting("recoilDecrease"))
			value = ConfigGetFloat("recoilDecrease");
		return value;
	}
}