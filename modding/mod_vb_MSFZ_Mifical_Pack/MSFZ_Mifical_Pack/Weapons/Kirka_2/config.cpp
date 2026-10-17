class CfgPatches
{
	class MSFZ_Magic_Hatchet
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Weapons_Melee"};
	};
};
class CfgVehicles
{
	class Hatchet;
	class MSFZ_Magic_Hatchet: Hatchet
	{
		scope = 2;
		displayName = "$STR_Magic_Hatchet";
		descriptionShort = "$STR_Magic_Hatchet1";
		model = "MSFZ_Mifical_Pack\Weapons\Kirka_2\MSFZ_Magic_Hatchet.p3d";
		debug_ItemCategory = 2;
		build_action_type = 10;
		dismantle_action_type = 74;
		repairableWithKits[] = {4};
		repairCosts[] = {18};
		rotationFlags = 17;
		weight = 1000;
		itemSize[] = {2,4};
		fragility = 0.01;
		inventorySlot[] = {""};
		lootCategory = "Tools";
		lootTag[] = {"Work","Forester","Camping"};
		itemInfo[] = {"Axe"};
		openItemSpillRange[] = {20,40};
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Weapons\Kirka_2\data\MSFZ_Magic_Kirka_2.paa"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 1200;
					healthLevels[] = {{1,{"MSFZ_Mifical_Pack\Weapons\Kirka\data\MSFZ_Magic_Kirka_2.rvmat"}},{0.7,{"MSFZ_Mifical_Pack\Weapons\Kirka\data\MSFZ_Magic_Kirka_2.rvmat"}},{0.5,{"MSFZ_Mifical_Pack\Weapons\Kirka\data\MSFZ_Magic_Kirka_2_damage.rvmat"}},{0.3,{"MSFZ_Mifical_Pack\Weapons\Kirka\data\MSFZ_Magic_Kirka_2_damage.rvmat"}},{0,{"MSFZ_Mifical_Pack\Weapons\Kirka\data\MSFZ_Magic_Kirka_2_destruct.rvmat"}}};
				};
			};
		};
		class MeleeModes
		{
			class Default
			{
				ammo = "MeleeSharpLight_2";
				range = 2;
			};
			class Heavy
			{
				ammo = "MeleeSharpHeavy_2";
				range = 3.5;
			};
			class Sprint
			{
				ammo = "MeleeSharpHeavy_2";
				range = 5.5;
			};
		};
		isMeleeWeapon = 1;
		suicideAnim = "woodaxe";
		soundImpactType = "metal";
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickup_light
				{
					soundSet = "hatchet_pickup_light_SoundSet";
					id = 796;
				};
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
				class FirefighterAxe_loop_SoundSet
				{
					soundSet = "FirefighterAxe_loop_SoundSet";
					id = 1121;
				};
				class FirefighterAxe_end_SoundSet
				{
					soundSet = "FirefighterAxe_end_SoundSet";
					id = 1122;
				};
				class ShoulderR_Hide
				{
					soundset = "ShoulderR_Hide_SoundSet";
					id = 1210;
				};
				class ShoulderR_Show
				{
					soundset = "ShoulderR_Show_SoundSet";
					id = 1211;
				};
				class WoodHammer_SoundSet
				{
					soundSet = "WoodHammer_SoundSet";
					id = 11161;
				};
				class animalSkinning_in
				{
					soundSet = "animalSkinning_in_SoundSet";
					id = 516;
				};
				class animalSkinning
				{
					soundSet = "animalSkinning_SoundSet";
					id = 517;
				};
				class animalSkinning_out
				{
					soundSet = "animalSkinning_out_SoundSet";
					id = 518;
				};
			};
		};
	};
};
