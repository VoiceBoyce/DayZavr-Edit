class CfgPatches
{
	class MSFZ_Magic_Kirka
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Characters","DZ_Sounds_Effects","DZ_Weapons_Melee","DZ_Data"};
	};
};
class CfgVehicles
{
	class Pickaxe;
	class MSFZ_Magic_Kirka: Pickaxe
	{
		scope = 2;
		displayName = "$STR_Magic_Kirka";
		descriptionShort = "$STR_Magic_Kirka1";
		model = "MSFZ_Mifical_Pack\Weapons\Kirka\MSFZ_Magic_Kirka.p3d";
		debug_ItemCategory = 2;
		inventorySlot[] = {"Shoulder","Melee"};
		rotationFlags = 12;
		repairableWithKits[] = {4};
		repairCosts[] = {25};
		build_action_type = 36;
		dismantle_action_type = 46;
		itemInfo[] = {"Pickaxe"};
		RestrainUnlockType = 1;
		itemSize[] = {3,6};
		weight = 2500;
		itemBehaviour = 2;
		lootTag[] = {"Work"};
		lootCategory = "Tools";
		openItemSpillRange[] = {40,70};
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Weapons\Kirka\data\MSFZ_Magic_Kirka.paa"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 250;
					healthLevels[] = {{1,{"MSFZ_Mifical_Pack\Weapons\Kirka\data\MSFZ_Magic_Kirka.rvmat"}},{0.7,{"MSFZ_Mifical_Pack\Weapons\Kirka\data\MSFZ_Magic_Kirka.rvmat"}},{0.5,{"MSFZ_Mifical_Pack\Weapons\Kirka\data\MSFZ_Magic_Kirka_damage.rvmat"}},{0.3,{"MSFZ_Mifical_Pack\Weapons\Kirka\data\MSFZ_Magic_Kirka_damage.rvmat"}},{0,{"MSFZ_Mifical_Pack\Weapons\Kirka\data\MSFZ_Magic_Kirka_destruct.rvmat"}}};
				};
			};
		};
		isMeleeWeapon = 1;
		suicideAnim = "fireaxe";
		class MeleeModes
		{
			class Default
			{
				ammo = "MeleeSharpLight_4";
				range = 1.8;
			};
			class Heavy
			{
				ammo = "MeleeSharpHeavy_4";
				range = 1.8;
			};
			class Sprint
			{
				ammo = "MeleeSharpHeavy_4";
				range = 3.7;
			};
		};
		class Horticulture
		{
			ToolAnim = "CHOPPINGWOOD";
			DiggingTimeToComplete = 4.5;
		};
		soundImpactType = "metal";
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickup_light
				{
					soundSet = "pickUpShovelLight_SoundSet";
					id = 796;
				};
				class pickup
				{
					soundSet = "pickUpShovel_SoundSet";
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
				class MSFZ_Magic_Kirka_Mine_Sound
				{
					soundset = "MSFZ_Magic_Kirka_Mine_Sound_SoundSet";
					id = 101010;
				};
			};
		};
	};
};
class CfgSoundSets
{
	class baseAnimal_SoundSet;
	class MSFZ_Magic_Kirka_Mine_Sound_SoundSet
	{
		soundShaders[] = {"MSFZ_Magic_Kirka_Mine_Sound_SoundShader"};
		sound3DProcessingType = "smallItem3DProcessingType";
		volumeCurve = "LinearCurve";
		volumeFactor = 0.5;
		spatial = 1;
		loop = 1;
	};
};
class CfgSoundShaders
{
	class MSFZ_Magic_Kirka_Mine_Sound_SoundShader
	{
		samples[] = {{"MSFZ_Mifical_Pack\Weapons\Kirka\sound\MSFZ_Magic_Kirka_Mine",1}};
		volume = 1;
		range = 20;
	};
};
