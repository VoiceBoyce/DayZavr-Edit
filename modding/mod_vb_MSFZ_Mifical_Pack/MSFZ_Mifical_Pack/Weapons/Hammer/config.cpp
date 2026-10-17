class CfgPatches
{
	class MSFZ_Magic_Hammer_Tools
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Weapons_Melee"};
	};
};
class cfgVehicles
{
	class Hammer;
	class MSFZ_Magic_Hammer: Hammer
	{
		scope = 2;
		displayName = "$STR_Magic_Hammer";
		descriptionShort = "$STR_Magic_Hammer1";
		model = "MSFZ_Mifical_Pack\Weapons\Hammer\MSFZ_Magic_Hammer.p3d";
		debug_ItemCategory = 2;
		build_action_type = 10;
		dismantle_action_type = 0;
		rotationFlags = 17;
		weight = 940;
		itemSize[] = {2,3};
		fragility = 0.008;
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Weapons\Hammer\data\MSFZ_Magic_Hammer.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Weapons\Hammer\data\MSFZ_Magic_Hammer.rvmat"};
		openItemSpillRange[] = {10,20};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 1200;
					healthLevels[] = {{1,{"MSFZ_Mifical_Pack\Weapons\Hammer\data\MSFZ_Magic_Hammer.rvmat"}},{0.7,{"MSFZ_Mifical_Pack\Weapons\Hammer\data\MSFZ_Magic_Hammer.rvmat"}},{0.5,{"MSFZ_Mifical_Pack\Weapons\Hammer\data\MSFZ_Magic_Hammer_damage.rvmat"}},{0.3,{"MSFZ_Mifical_Pack\Weapons\Hammer\data\MSFZ_Magic_Hammer_damage.rvmat"}},{0,{"MSFZ_Mifical_Pack\Weapons\Hammer\data\MSFZ_Magic_Hammer_destruct.rvmat"}}};
				};
			};
		};
		class MeleeModes
		{
			class Default
			{
				ammo = "MeleeBluntLight_2";
				range = 1.1;
			};
			class Heavy
			{
				ammo = "MeleeBluntHeavy_2";
				range = 1.1;
			};
			class Sprint
			{
				ammo = "MeleeBluntHeavy_2";
				range = 3.3;
			};
		};
		soundImpactType = "metal";
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickup
				{
					soundSet = "hatchet_pickup_SoundSet";
					id = 797;
				};
				class drop
				{
					soundset = "woodaxe_drop_SoundSet";
					id = 898;
				};
				class MinningBuilding
				{
					soundLookupTable = "MinningBuilding_LookupTable";
					id = 11161;
				};
			};
		};
	};
};
