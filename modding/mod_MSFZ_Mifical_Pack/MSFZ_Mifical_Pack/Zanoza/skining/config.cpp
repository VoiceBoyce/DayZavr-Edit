class CfgPatches
{
	class MSFZ_Zanoza_eye
	{
		units[] = {"MSFZ_Zanoza_eye","MSFZ_Zanoza_stick"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data"};
	};
};
class CfgVehicles
{
	class Inventory_Base;
	class Edible_Base;
	class WolfSteakMeat;
	class MSFZ_Zanoza_eye: Inventory_Base
	{
		scope = 2;
		displayName = "$STR_Zanoza_eye";
		descriptionShort = "$STR_Zanoza_eye";
		model = "MSFZ_Mifical_Pack\Zanoza\skining\MSFZ_Zanoza_eye.p3d";
		weight = 200;
		itemSize[] = {2,2};
		peltGain = 6;
		leatherYield = 4;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 90000;
					healthLevels[] = {{1,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0,{}}};
				};
			};
		};
	};
	class MSFZ_Zanoza_stick: Inventory_Base
	{
		scope = 2;
		displayName = "$STR_Zanoza_stick";
		descriptionShort = "$STR_Zanoza_stick";
		model = "MSFZ_Mifical_Pack\Zanoza\skining\MSFZ_Zanoza_stick.p3d";
		weight = 200;
		itemSize[] = {2,2};
		peltGain = 6;
		leatherYield = 4;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 90000;
					healthLevels[] = {{1,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0,{}}};
				};
			};
		};
	};
};
