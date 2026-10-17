class DRL_Craft_Truck_Lift : DRL_Craft_WB
{
	protected bool m_IsCraftActive;
	protected Object m_CoveredCarObject;

	void DRL_Craft_Truck_Lift()
	{
		RegisterNetSyncVariableBool("m_IsCraftActive");
	}

	void ~DRL_Craft_Truck_Lift()
	{
		DeleteCoveredCar();
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);
		DeleteCoveredCar();
	}

	override bool NeedsScrewdriverForDeconstruct()
	{
		return false;
	}

	vector GetVehicleSpawnPosition()
	{
		return ModelToWorld(Vector(0, 0, 1.8));
	}

	vector GetCoveredCarSpawnPosition()
	{
		return GetVehicleSpawnPosition();
	}

	vector GetVehicleSpawnOrientation()
	{
		vector ori = GetOrientation();
		ori[0] = Math.NormalizeAngle(ori[0] + 90.0);
		return ori;
	}

	void SpawnCoveredCar()
	{
		DeleteCoveredCar();

		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			m_CoveredCarObject = GetGame().CreateObjectEx("DRL_Craft_Covered_Car", GetCoveredCarSpawnPosition(), ECE_CREATEPHYSICS);
			if (m_CoveredCarObject)
			{
				m_CoveredCarObject.SetPosition(GetCoveredCarSpawnPosition());
				m_CoveredCarObject.SetOrientation(GetVehicleSpawnOrientation());
			}
		}
	}

	void DeleteCoveredCar()
	{
		if (m_CoveredCarObject)
		{
			GetGame().ObjectDelete(m_CoveredCarObject);
			m_CoveredCarObject = null;
		}
	}

	override bool DRL_CanCraftResult(string resultCls, PlayerBase player, out string blockReason)
	{
		blockReason = "";
		if (m_IsCraftActive)
		{
			blockReason = "#STR_DRL_VEHICLE_ZONE_BLOCKED";
			return false;
		}

		if (!GetGame().IsKindOf(resultCls, "CarScript"))
			return true;

		array<Object> nearObjects = new array<Object>();
		GetGame().GetObjectsAtPosition(GetVehicleSpawnPosition(), 3.0, nearObjects, null);
		foreach (Object obj : nearObjects)
		{
			if (obj && obj != this && obj != m_CoveredCarObject && (obj.IsKindOf("CarScript") || obj.IsInherited(Man)))
			{
				blockReason = "#STR_DRL_VEHICLE_ZONE_BLOCKED";
				return false;
			}
		}
		return true;
	}

	override void DRL_OnCraftStart(string resultCls, PlayerBase player)
	{
		super.DRL_OnCraftStart(resultCls, player);
		m_IsCraftActive = true;
		if (GetGame().IsKindOf(resultCls, "CarScript"))
		{
			SpawnCoveredCar();
		}
		SetSynchDirty();
	}

	override void DRL_OnCraftFinish(string resultCls, EntityAI result, PlayerBase player)
	{
		super.DRL_OnCraftFinish(resultCls, result, player);
		m_IsCraftActive = false;
		DeleteCoveredCar();
		SetSynchDirty();
	}

	override EntityAI DRL_CreateCraftResult(string resultCls, PlayerBase player, bool groundOnly, vector customSpawnPos = "0 0 0")
	{
		if (GetGame().IsKindOf(resultCls, "CarScript"))
		{
			DeleteCoveredCar();
			int flags = ECE_CREATEPHYSICS | ECE_UPDATEPATHGRAPH | ECE_KEEPHEIGHT;
			EntityAI car = EntityAI.Cast(GetGame().CreateObjectEx(resultCls, GetVehicleSpawnPosition(), flags));
			if (car)
			{
				car.SetOrientation(GetVehicleSpawnOrientation());
				dBodyApplyImpulse(car, vector.Up);
				return car;
			}
		}
		return super.DRL_CreateCraftResult(resultCls, player, groundOnly, customSpawnPos);
	}
};

class DRL_Craft_Truck_Lift_Static : DRL_Craft_Truck_Lift
{
	override void SetActions()
	{
		super.SetActions();
		RemoveAction(ActionDeconstructDRLWorkbench);
	}

	override bool CanPutInCargo(EntityAI parent)
	{
		return false;
	}

	override bool CanPutIntoHands(EntityAI parent)
	{
		return false;
	}
};
