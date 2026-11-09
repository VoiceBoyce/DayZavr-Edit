class CfgPatches
{
	class MSFZ_Spider_Lapka
	{
		units[] = {"MSFZ_Spider_Lapka"};
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
	class MSFZ_Spider_Lapka: Inventory_Base
	{
		scope = 2;
		displayName = "$STR_Spider_Lapka";
		descriptionShort = "$STR_Spider_Lapka";
		model = "MSFZ_Mifical_Pack\Spider\skining\MSFZ_Spider_Lapka.p3d";
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
