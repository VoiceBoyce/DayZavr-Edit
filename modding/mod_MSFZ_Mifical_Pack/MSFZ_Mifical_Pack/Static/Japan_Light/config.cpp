class CfgPatches
{
	class MSFZ_Japan_light
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {};
	};
};
class CfgVehicles
{
	class HouseNoDestruct;
	class MSFZ_Japan_light: HouseNoDestruct
	{
		scope = 1;
		model = "MSFZ_Mifical_Pack\Static\Japan_Light\MSFZ_Japan_Light.p3d";
	};
};
