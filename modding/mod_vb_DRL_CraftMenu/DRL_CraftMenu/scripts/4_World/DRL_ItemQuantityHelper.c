class DRL_ItemQuantityHelper
{
	static int GetQuantityType(EntityAI item)
	{
		if (!item)
			return EDRLItemQuantityType.DISCRETE;

		Magazine mag = Magazine.Cast(item);
		if (mag)
		{
			if (mag.IsAmmoPile())
				return EDRLItemQuantityType.STACK;
			return EDRLItemQuantityType.MAGAZINE;
		}

		ItemBase ib = ItemBase.Cast(item);
		if (!ib)
		{
			if (item.IsKindOf("Ammunition_Base"))
				return EDRLItemQuantityType.STACK;
			if (item.IsKindOf("Magazine_Base"))
				return EDRLItemQuantityType.MAGAZINE;
			return EDRLItemQuantityType.DISCRETE;
		}

		if (ib.IsAmmoPile())
			return EDRLItemQuantityType.STACK;

		if (ib.IsMagazine())
			return EDRLItemQuantityType.MAGAZINE;

		if (ib.HasEnergyManager())
			return EDRLItemQuantityType.ENERGY;

		if (ib.IsLiquidContainer())
			return EDRLItemQuantityType.LIQUID;

		if (!ib.HasQuantity())
			return EDRLItemQuantityType.DISCRETE;

		if (ib.ConfigIsExisting("stackedUnit"))
		{
			string stackedUnit = ib.ConfigGetString("stackedUnit");
			stackedUnit.ToLower();
			if (stackedUnit == "pc." || stackedUnit == "pills" || stackedUnit == "pc")
			{
				if (ib.ConfigIsExisting("canBeSplit") && ib.ConfigGetInt("canBeSplit") == 1)
					return EDRLItemQuantityType.STACK;
				if (ib.GetQuantityMax() > 1)
					return EDRLItemQuantityType.STACK;
			}
			else if (stackedUnit == "percentage")
				return EDRLItemQuantityType.RESOURCE_BAR;
			else if (stackedUnit == "g")
				return EDRLItemQuantityType.FOOD;
			else if (stackedUnit == "ml")
				return EDRLItemQuantityType.LIQUID;
			else if (stackedUnit == "w")
				return EDRLItemQuantityType.ENERGY;
		}

		if (ib.ConfigIsExisting("quantityBar") && ib.ConfigGetInt("quantityBar") == 1)
			return EDRLItemQuantityType.RESOURCE_BAR;

		if (ib.ConfigIsExisting("canBeSplit") && ib.ConfigGetInt("canBeSplit") == 1 && !ib.IsInherited(Edible_Base))
			return EDRLItemQuantityType.STACK;

		if (ib.IsInherited(Edible_Base))
			return EDRLItemQuantityType.FOOD;

		return EDRLItemQuantityType.DISCRETE;
	}

	static int GetQuantityTypeByClass(string className)
	{
		if (className == "")
			return EDRLItemQuantityType.DISCRETE;

		if (GetGame().IsKindOf(className, "Ammunition_Base"))
			return EDRLItemQuantityType.STACK;

		if (GetGame().IsKindOf(className, "Magazine_Base") || GetGame().IsKindOf(className, "Magazine"))
			return EDRLItemQuantityType.MAGAZINE;

		string emPath = "CfgVehicles " + className + " EnergyManager";
		if (GetGame().ConfigIsExisting(emPath))
			return EDRLItemQuantityType.ENERGY;

		string stackedUnit = "";
		string suVeh = "CfgVehicles " + className + " stackedUnit";
		string suMag = "CfgMagazines " + className + " stackedUnit";
		if (GetGame().ConfigIsExisting(suVeh))
			GetGame().ConfigGetText(suVeh, stackedUnit);
		else if (GetGame().ConfigIsExisting(suMag))
			GetGame().ConfigGetText(suMag, stackedUnit);

		if (stackedUnit != "")
		{
			stackedUnit.ToLower();
			if (stackedUnit == "pc." || stackedUnit == "pills" || stackedUnit == "pc")
			{
				string maxVehStack = "CfgVehicles " + className + " varQuantityMax";
				string splitVehStack = "CfgVehicles " + className + " canBeSplit";
				if (GetGame().ConfigIsExisting(maxVehStack) && GetGame().ConfigGetInt(maxVehStack) > 0)
				{
					if (GetGame().ConfigIsExisting(splitVehStack) && GetGame().ConfigGetInt(splitVehStack) == 1)
						return EDRLItemQuantityType.STACK;
					if (stackedUnit == "pills")
						return EDRLItemQuantityType.STACK;
				}
			}
			else if (stackedUnit == "percentage")
				return EDRLItemQuantityType.RESOURCE_BAR;
			else if (stackedUnit == "g")
				return EDRLItemQuantityType.FOOD;
			else if (stackedUnit == "ml")
				return EDRLItemQuantityType.LIQUID;
			else if (stackedUnit == "w")
				return EDRLItemQuantityType.ENERGY;
		}

		string qbVeh = "CfgVehicles " + className + " quantityBar";
		if (GetGame().ConfigIsExisting(qbVeh) && GetGame().ConfigGetInt(qbVeh) == 1)
			return EDRLItemQuantityType.RESOURCE_BAR;

		string splitVeh = "CfgVehicles " + className + " canBeSplit";
		string maxVeh = "CfgVehicles " + className + " varQuantityMax";
		if (GetGame().ConfigIsExisting(splitVeh) && GetGame().ConfigGetInt(splitVeh) == 1)
		{
			if (!GetGame().IsKindOf(className, "Edible_Base"))
			{
				if (GetGame().ConfigIsExisting(maxVeh) && GetGame().ConfigGetInt(maxVeh) > 0)
					return EDRLItemQuantityType.STACK;
			}
		}

		string liqInit = "CfgVehicles " + className + " varLiquidTypeInit";
		if (GetGame().ConfigIsExisting(liqInit))
			return EDRLItemQuantityType.LIQUID;

		if (GetGame().IsKindOf(className, "Edible_Base"))
		{
			if (GetGame().ConfigIsExisting(maxVeh) && GetGame().ConfigGetInt(maxVeh) > 0)
				return EDRLItemQuantityType.FOOD;
		}

		return EDRLItemQuantityType.DISCRETE;
	}

	static bool IsStackable(EntityAI item)
	{
		return (GetQuantityType(item) == EDRLItemQuantityType.STACK);
	}

	static int GetStackAmount(EntityAI item)
	{
		if (!item)
			return 0;

		Magazine mag = Magazine.Cast(item);
		if (mag && mag.IsAmmoPile())
			return mag.GetAmmoCount();

		ItemBase ib = ItemBase.Cast(item);
		if (!ib)
			return 1;

		int qType = GetQuantityType(ib);
		if (qType == EDRLItemQuantityType.STACK)
		{
			int q = ib.GetQuantity();
			if (q <= 0)
				return 1;
			return q;
		}

		return 1;
	}

	static int GetStackMax(EntityAI item, string className = "")
	{
		if (item)
		{
			Magazine mag = Magazine.Cast(item);
			if (mag && mag.IsAmmoPile())
				return mag.GetAmmoMax();

			ItemBase ib = ItemBase.Cast(item);
			if (ib && GetQuantityType(ib) == EDRLItemQuantityType.STACK)
				return ib.GetQuantityMax();

			return 1;
		}

		if (className != "")
		{
			int qType = GetQuantityTypeByClass(className);
			if (qType == EDRLItemQuantityType.STACK)
			{
				string pMax = "CfgVehicles " + className + " varQuantityMax";
				if (GetGame().ConfigIsExisting(pMax))
					return GetGame().ConfigGetInt(pMax);

				pMax = "CfgMagazines " + className + " count";
				if (GetGame().ConfigIsExisting(pMax))
					return GetGame().ConfigGetInt(pMax);
			}
		}

		return 1;
	}

	static float GetQuantityRaw(EntityAI item)
	{
		if (!item)
			return 0.0;

		int qType = GetQuantityType(item);
		if (qType == EDRLItemQuantityType.ENERGY)
		{
			ComponentEnergyManager em = item.GetCompEM();
			if (em)
				return em.GetEnergy();
			return 0.0;
		}

		if (qType == EDRLItemQuantityType.MAGAZINE || qType == EDRLItemQuantityType.STACK)
		{
			Magazine mag = Magazine.Cast(item);
			if (mag)
				return mag.GetAmmoCount();
		}

		ItemBase ib = ItemBase.Cast(item);
		if (ib && ib.HasQuantity())
			return ib.GetQuantity();

		return 1.0;
	}

	static float GetQuantityMaxRaw(EntityAI item, string className = "")
	{
		if (item)
		{
			int qType = GetQuantityType(item);
			if (qType == EDRLItemQuantityType.ENERGY)
			{
				ComponentEnergyManager em = item.GetCompEM();
				if (em)
					return em.GetEnergyMax();
				return 1.0;
			}

			if (qType == EDRLItemQuantityType.MAGAZINE || qType == EDRLItemQuantityType.STACK)
			{
				Magazine mag = Magazine.Cast(item);
				if (mag)
					return mag.GetAmmoMax();
			}

			ItemBase ib = ItemBase.Cast(item);
			if (ib)
				return ib.GetQuantityMax();

			return 1.0;
		}

		if (className != "")
		{
			int qTypeCls = GetQuantityTypeByClass(className);
			if (qTypeCls == EDRLItemQuantityType.ENERGY)
			{
				string pStorage = "CfgVehicles " + className + " EnergyManager energyStorageMax";
				if (GetGame().ConfigIsExisting(pStorage))
					return GetGame().ConfigGetFloat(pStorage);
			}

			string pMaxVeh = "CfgVehicles " + className + " varQuantityMax";
			if (GetGame().ConfigIsExisting(pMaxVeh))
			{
				int maxV = GetGame().ConfigGetInt(pMaxVeh);
				if (maxV > 0)
					return maxV;
			}

			string pCountMag = "CfgMagazines " + className + " count";
			if (GetGame().ConfigIsExisting(pCountMag))
			{
				int maxM = GetGame().ConfigGetInt(pCountMag);
				if (maxM > 0)
					return maxM;
			}
		}

		return 1.0;
	}

	static float GetFillingFraction(EntityAI item)
	{
		if (!item)
			return 0.0;

		int qType = GetQuantityType(item);
		if (qType == EDRLItemQuantityType.ENERGY)
		{
			ComponentEnergyManager em = item.GetCompEM();
			if (em)
			{
				float maxE = em.GetEnergyMax();
				if (maxE > 0.0)
					return em.GetEnergy() / maxE;
			}
			return 0.0;
		}

		if (qType == EDRLItemQuantityType.MAGAZINE)
		{
			Magazine mag = Magazine.Cast(item);
			if (mag)
			{
				int maxAmmo = mag.GetAmmoMax();
				if (maxAmmo > 0)
					return (float)mag.GetAmmoCount() / (float)maxAmmo;
			}
			return 0.0;
		}

		ItemBase ib = ItemBase.Cast(item);
		if (ib && ib.HasQuantity())
		{
			int qMax = ib.GetQuantityMax();
			if (qMax > 0)
				return (float)ib.GetQuantity() / (float)qMax;
		}

		return 1.0;
	}

	static void SetFilling(EntityAI item, float frac, int rawCount = 1)
	{
		if (!item)
			return;

		int qType = GetQuantityType(item);
		switch (qType)
		{
			case EDRLItemQuantityType.ENERGY:
			{
				ComponentEnergyManager em = item.GetCompEM();
				if (em)
				{
					float fPower = Math.Clamp(frac, 0.0, 1.0);
					em.SetEnergy(em.GetEnergyMax() * fPower);
				}
				break;
			}

			case EDRLItemQuantityType.MAGAZINE:
			{
				Magazine mag = Magazine.Cast(item);
				if (mag)
				{
					int maxA = mag.GetAmmoMax();
					int ammoSet = 0;
					if (frac > 0.0)
						ammoSet = Math.Clamp(Math.Round(maxA * frac), 1, maxA);
					else
						ammoSet = Math.Clamp(rawCount, 1, maxA);

					if (GetGame().IsServer())
						mag.ServerSetAmmoCount(ammoSet);
					else
						mag.LocalSetAmmoCount(ammoSet);
				}
				break;
			}

			case EDRLItemQuantityType.STACK:
			{
				Magazine magPile = Magazine.Cast(item);
				if (magPile && magPile.IsAmmoPile())
				{
					int maxAmmoPile = magPile.GetAmmoMax();
					int ammoToSet = rawCount;
					if (frac > 0.0 && maxAmmoPile > 0)
						ammoToSet = Math.Clamp(Math.Round(maxAmmoPile * frac), 1, maxAmmoPile);
					else if (maxAmmoPile > 0)
						ammoToSet = Math.Clamp(ammoToSet, 1, maxAmmoPile);

					if (GetGame().IsServer())
						magPile.ServerSetAmmoCount(ammoToSet);
					else
						magPile.LocalSetAmmoCount(ammoToSet);
					break;
				}

				ItemBase ibPC = ItemBase.Cast(item);
				if (ibPC)
				{
					int qMaxPC = ibPC.GetQuantityMax();
					int setQ = rawCount;
					if (frac > 0.0 && qMaxPC > 0)
						setQ = Math.Clamp(Math.Round(qMaxPC * frac), 1, qMaxPC);
					else if (qMaxPC > 0)
						setQ = Math.Clamp(setQ, 1, qMaxPC);

					ibPC.SetQuantity(setQ);
				}
				break;
			}

			case EDRLItemQuantityType.LIQUID:
			case EDRLItemQuantityType.RESOURCE_BAR:
			case EDRLItemQuantityType.FOOD:
			{
				ItemBase ibFill = ItemBase.Cast(item);
				if (ibFill)
				{
					int qMaxF = ibFill.GetQuantityMax();
					if (qMaxF > 0)
					{
						if (frac > 0.0)
						{
							float fFill = Math.Clamp(frac, 0.0, 1.0);
							ibFill.SetQuantity(Math.Clamp(Math.Round(qMaxF * fFill), 0, qMaxF));
						}
						else if (rawCount > 1)
						{
							ibFill.SetQuantity(Math.Clamp(rawCount, 0, qMaxF));
						}
						else
						{
							ibFill.SetQuantity(0);
						}
					}
				}
				break;
			}

			default:
			{
				ItemBase ibDef = ItemBase.Cast(item);
				if (ibDef && ibDef.HasQuantity())
				{
					int qmDef = ibDef.GetQuantityMax();
					if (qmDef > 0)
					{
						if (frac > 0.0)
							ibDef.SetQuantity(Math.Clamp(Math.Round(qmDef * frac), 1, qmDef));
						else if (rawCount > 1)
							ibDef.SetQuantity(Math.Clamp(rawCount, 1, qmDef));
					}
				}
				break;
			}
		}
	}

	static bool IsFillingType(int qType)
	{
		return (qType == EDRLItemQuantityType.RESOURCE_BAR || qType == EDRLItemQuantityType.LIQUID || qType == EDRLItemQuantityType.ENERGY || qType == EDRLItemQuantityType.FOOD);
	}
}
