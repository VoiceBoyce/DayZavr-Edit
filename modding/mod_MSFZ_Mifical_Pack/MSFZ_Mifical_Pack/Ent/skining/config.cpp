class CfgPatches
{
	class MSFZ_Ent_head
	{
		units[] = {"MSFZ_Ent_head","MSFZ_Ent_head_medium","MSFZ_Ent_head_hard"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data"};
	};
};
class CfgVehicles
{
	class Inventory_Base;
	class MSFZ_Ent_head: Inventory_Base
	{
		scope = 2;
		displayName = "$STR_Ent_head";
		descriptionShort = "$STR_Ent_head";
		model = "MSFZ_Mifical_Pack\Ent\skining\MSFZ_Ent_head.p3d";
		weight = 1200;
		itemSize[] = {2,2};
		peltGain = 6;
		leatherYield = 4;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 100;
					healthLevels[] = {{1,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0,{}}};
				};
			};
		};
	};
	class MSFZ_Ent_head_medium: Inventory_Base
	{
		scope = 2;
		displayName = "$STR_Ent_head_medium";
		descriptionShort = "$STR_Ent_head_medium";
		model = "MSFZ_Mifical_Pack\Ent\skining\MSFZ_Ent_head_Medium.p3d";
		weight = 1200;
		itemSize[] = {2,2};
		peltGain = 6;
		leatherYield = 4;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 100;
					healthLevels[] = {{1,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0,{}}};
				};
			};
		};
	};
	class MSFZ_Ent_head_hard: Inventory_Base
	{
		scope = 2;
		displayName = "$STR_Ent_head_hard";
		descriptionShort = "$STR_Ent_head_hard";
		model = "MSFZ_Mifical_Pack\Ent\skining\MSFZ_Ent_head_Hard.p3d";
		weight = 1200;
		itemSize[] = {2,2};
		peltGain = 6;
		leatherYield = 4;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 100;
					healthLevels[] = {{1,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0,{}}};
				};
			};
		};
	};
};
