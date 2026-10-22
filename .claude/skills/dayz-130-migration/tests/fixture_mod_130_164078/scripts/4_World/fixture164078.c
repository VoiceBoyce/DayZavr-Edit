// Fixture: a mod written for DayZ 1.30.164078. Every marked line must be reported by the hotfix step.
modded class CarScript
{
	bool HasCrew()
	{
		return CrewSize() > 0;
	}
	bool IsMoving()
	{
		return GetSpeedometerAbsolute() > 1;
	}
}

modded class MotorbikeScript
{
	bool HasCrew()
	{
		return CrewSize() > 0;
	}
	bool IsMoving()
	{
		return GetSpeedometerAbsolute() > 1;
	}
}

class FixtureBoat : BoatScript
{
	int IsMoving(float threshold)
	{
		return 0;
	}
}

class FixtureMath
{
	static vector Normal(vector v1, vector v2)
	{
		return v1.Cross(v2).Normalized();
	}
	static vector Side(Object o, vector x)
	{
		return o.GetDirection().Cross(x);
	}
}

class FixturePart : ConstructionPart
{
	override int GetSupportProvided(int category)
	{
		return super.GetSupportProvided(category) + 1;
	}
	override void TryHandleUtilityParts(EntityAI parent)
	{
		super.TryHandleUtilityParts(parent);
	}
	bool InvertSupportBehavior()
	{
		return true;
	}
}

class FixturePartType : ConstructionPartTypeData
{
	void Fixture_List()
	{
		array<string> list = GetPartTypeUtilityAutobuildPartsList();
		int n = m_UtilityAutomaticBuild.Count();
	}
}

class FixtureNV
{
	int m_SavedNV = NVTypes.NV_GOGGLES_DAY;
	void Save(ParamsWriteContext ctx)
	{
		ctx.Write(m_SavedNV);
		if (m_SavedNV == 15)
			Print("goggles day");
	}
	bool IsSandstormOptics(int type)
	{
		return type == NVTypes.NV_OPTICS_SANDSTORM_DAY;
	}
}

class FixtureCursor : CCTCursor
{
	float m_MaximalActionDistance;
	override bool Can(PlayerBase player, ActionTarget target)
	{
		return true;
	}
}

class FixtureNav : NavmeshTest
{
	void Spawn(vector pos)
	{
		GetGame().CreateObject("NavmeshTest_A", pos);
	}
}

class FixtureOk
{
	bool Ok(ConstructionPart part, int category)
	{
		int strength;
		return part.GetSupportProvided(category, strength) && strength > 0;
	}
}
