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

	// --- DayZavr: в хранилище подъёмника кладутся только вещи из рецептов этого верстака ---
	// Список собирается из конфига крафта (есть и на сервере, и на клиенте) и пересобирается при смене конфига.
	// Загрузку из базы (CanLoadItemIntoCargo) не трогаем: то, что уже лежит, не пропадёт.
	protected static ref map<string, bool> s_DZV_Allowed;   // классы ингредиентов и результатов (нижний регистр)
	protected static ref array<string> s_DZV_AllowedBases;  // классы с UseInheritance: подходят и наследники
	protected static DRL_Craft_Config s_DZV_Cfg;
	protected static int s_DZV_CfgCount = -1;

	protected static void DZV_Allow(string cls, bool withInheritance)
	{
		if (cls == "")
			return;
		string lc = cls;
		lc.ToLower();
		s_DZV_Allowed.Set(lc, true);
		if (withInheritance && s_DZV_AllowedBases.Find(cls) == -1)
			s_DZV_AllowedBases.Insert(cls);
	}

	protected void DZV_BuildAllowed(DRL_Craft_Config cfg)
	{
		s_DZV_Allowed = new map<string, bool>();
		s_DZV_AllowedBases = new array<string>();
		s_DZV_Cfg = cfg;
		s_DZV_CfgCount = cfg.CraftItems.Count();

		foreach (DRL_Craft_Recipe rec : cfg.CraftItems)
		{
			if (!rec || !rec.WorkbenchClasses)
				continue;

			bool forThisWb = false;
			foreach (string wbCls : rec.WorkbenchClasses)
			{
				if (wbCls != "" && IsKindOf(wbCls))
				{
					forThisWb = true;
					break;
				}
			}
			if (!forThisWb)
				continue;

			DZV_Allow(rec.Result, false);
			if (rec.ResultClasses)
			{
				foreach (string resCls : rec.ResultClasses)
				{
					DZV_Allow(resCls, false);
				}
			}

			if (!rec.CraftComponents)
				continue;

			foreach (DRL_Craft_Component comp : rec.CraftComponents)
			{
				if (!comp)
					continue;
				bool inh = (comp.UseInheritance == 1);
				DZV_Allow(comp.Classname, inh);
				if (comp.Classnames)
				{
					foreach (string compCls : comp.Classnames)
					{
						DZV_Allow(compCls, inh);
					}
				}
			}
		}
	}

	protected bool DZV_IsAllowedInCargo(EntityAI item)
	{
		if (!item)
			return false;

		DRL_Craft_Config cfg = GetCraft_Config();
		if (!cfg || !cfg.CraftItems || cfg.CraftItems.Count() == 0)
			return true; // клиент ещё не получил конфиг — проверку сделает сервер

		if (!s_DZV_Allowed || cfg != s_DZV_Cfg || cfg.CraftItems.Count() != s_DZV_CfgCount)
			DZV_BuildAllowed(cfg);

		string type = item.GetType();
		type.ToLower();
		if (s_DZV_Allowed.Contains(type))
			return true;

		foreach (string baseCls : s_DZV_AllowedBases)
		{
			if (item.IsKindOf(baseCls))
				return true;
		}
		return false;
	}

	override bool CanReceiveItemIntoCargo(EntityAI item)
	{
		if (!DZV_IsAllowedInCargo(item))
			return false;
		return super.CanReceiveItemIntoCargo(item);
	}

	override bool CanSwapItemInCargo(EntityAI child_entity, EntityAI new_entity)
	{
		if (!DZV_IsAllowedInCargo(new_entity))
			return false;
		return super.CanSwapItemInCargo(child_entity, new_entity);
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
