class CfgPatches
{
	class MSFZ_Old_Pillar
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {};
	};
};
class CfgVehicles
{
	class StaticObject;
	class HouseNoDestruct;
	class MSFZ_Old_Pillar: HouseNoDestruct
	{
		scope = 1;
		model = "MSFZ_Mifical_Pack\Static\Old_Pillar\MSFZ_Old_Pillar.p3d";
	};
};
