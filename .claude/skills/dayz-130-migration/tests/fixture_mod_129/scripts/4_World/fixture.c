modded class ItemBase
{
	override void OnCEUpdate()
	{
		super.OnCEUpdate();
		float dt = m_ElapsedSinceLastUpdate;
	}
	override void ProcessVariables()
	{
		super.ProcessVariables();
	}
	override bool CanDisplayAttachmentSlot(string slot_name)
	{
		return super.CanDisplayAttachmentSlot(slot_name);
	}
}

modded class CarScript
{
	override CarLightBase CreateFrontLight()
	{
		return CarLightBase.Cast(ScriptedLightBase.CreateLight(OffroadHatchbackFrontLight, "0 0 0"));
	}
	override bool IsVitalCarBattery()
	{
		return false;
	}
	void Fixture_Toggle()
	{
		ToggleHeadlights();
	}
}

modded class Construction
{
	override void BuildPartServer(notnull Man player, string part_name, int action_id)
	{
		super.BuildPartServer(player, part_name, action_id);
	}
	override protected void SetParent(BaseBuildingBase parent)
	{
		super.SetParent(parent);
	}
}

modded class CombinationLock
{
	void Fixture_Open(EntityAI p)
	{
		UnlockServer(null, p);
		SetCombination(123);
	}
}

class FixtureDlcCheck
{
	bool Has()
	{
		return GetGame().GetContentDLCService().IsDLCInstalled(EDLCId.DLC_FROSTLINE);
	}
	void ListFiles()
	{
		string name;
		FileAttr attr;
		FindFileHandle h = FindFile("$profile:Fixture\*.json", name, attr, 0);
	}
}

modded class CivilianSedan
{
	void CivilianSedan()
	{
		m_CarHornShortSoundName = "Fixture_Horn_SoundSet";
	}
}

modded class Clothing
{
	override float GetDryingIncrement(string pIncrementName)
	{
		return super.GetDryingIncrement(pIncrementName) * 2;
	}
}

class FixtureRadio : ItemBase
{
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		Print("rpc");
	}
	void Boom()
	{
		string ammo = "LandMineExplosion";
	}
}

modded class Building
{
	override bool CanDoorBeOpened(int doorIndex, bool checkIfLocked = false)
	{
		return super.CanDoorBeOpened(doorIndex, checkIfLocked);
	}
}

class FixtureCar : CarScript
{
	override bool IsVitalCarBattery()
	{
		return true;
	}
}
class FixtureSedan : CivilianSedan
{
}
