class CfgPatches
{
	class MSFZ_Magic_Knife_Blade
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Weapons_Melee"};
	};
};
class cfgVehicles
{
	class HuntingKnife;
	class MSFZ_Magic_Knife: HuntingKnife
	{
		scope = 2;
		visibilityModifier = 0.85;
		displayName = "$STR_Magic_Knife";
		descriptionShort = "$STR_Magic_Knife1";
		model = "MSFZ_Mifical_Pack\Weapons\Knife\MSFZ_Magic_Knife.p3d";
		debug_ItemCategory = 2;
		inventorySlot[] = {"Knife"};
		SkinningDurationSpeed = 1.2;
		repairableWithKits[] = {4};
		repairCosts[] = {10};
		rotationFlags = 17;
		RestrainUnlockType = 1;
		canSkinBodies = 1;
		weight = 200;
		itemSize[] = {1,3};
		lootTag[] = {"Hunting","Camping"};
		lootCategory = "Tools";
		itemInfo[] = {"Knife"};
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Weapons\Knife\data\MSFZ_Mifical_Knife.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Weapons\Knife\data\MSFZ_Mifical_Knife.rvmat"};
		openItemSpillRange[] = {10,20};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 1200;
					healthLevels[] = {{1,{"MSFZ_Mifical_Pack\Weapons\Knife\data\MSFZ_Mifical_Knife.rvmat"}},{0.7,{"MSFZ_Mifical_Pack\Weapons\Knife\data\MSFZ_Mifical_Knife.rvmat"}},{0.5,{"MSFZ_Mifical_Pack\Weapons\Knife\data\MSFZ_Mifical_Knife_damage.rvmat"}},{0.3,{"MSFZ_Mifical_Pack\Weapons\Knife\data\MSFZ_Mifical_Knife_damage.rvmat"}},{0,{"MSFZ_Mifical_Pack\Weapons\Knife\data\MSFZ_Mifical_Knife_destruct.rvmat"}}};
				};
			};
		};
		isMeleeWeapon = 1;
		suicideAnim = "onehanded";
		class MeleeModes
		{
			class Default
			{
				ammo = "MeleeKnife";
				range = 1.2;
			};
			class Heavy
			{
				ammo = "MeleeKnife_Heavy";
				range = 1.2;
			};
			class Sprint
			{
				ammo = "MeleeKnife_Heavy";
				range = 3.3;
			};
		};
		soundImpactType = "metal";
		class AnimEvents
		{
			class SoundWeapon
			{
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
				class CutTies_end
				{
					soundSet = "CutTies_end_SoundSet";
					id = 519;
				};
				class CutTies_loop
				{
					soundSet = "CutTies_loop_SoundSet";
					id = 520;
				};
				class shaving_knife_clean
				{
					soundSet = "shaving_knife_clean_SoundSet";
					id = 600;
				};
				class shaving_knife1
				{
					soundSet = "shaving_knife1_SoundSet";
					id = 601;
				};
				class shaving_knife2
				{
					soundSet = "shaving_knife2_SoundSet";
					id = 602;
				};
				class shaving_knife3
				{
					soundSet = "shaving_knife3_SoundSet";
					id = 603;
				};
				class shaving_knife4
				{
					soundSet = "shaving_knife4_SoundSet";
					id = 604;
				};
				class shaving_knife5
				{
					soundSet = "shaving_knife5_SoundSet";
					id = 605;
				};
				class shaving_knife6
				{
					soundSet = "shaving_knife6_SoundSet";
					id = 606;
				};
				class shaving_knife7
				{
					soundSet = "shaving_knife7_SoundSet";
					id = 607;
				};
				class shaving_knife8
				{
					soundSet = "shaving_knife8_SoundSet";
					id = 608;
				};
				class shaving_knife9
				{
					soundSet = "shaving_knife9_SoundSet";
					id = 609;
				};
				class shaving_knife10
				{
					soundSet = "shaving_knife10_SoundSet";
					id = 610;
				};
				class shaving_knife11
				{
					soundSet = "shaving_knife11_SoundSet";
					id = 611;
				};
				class shaving_knife12
				{
					soundSet = "shaving_knife12_SoundSet";
					id = 612;
				};
				class shaving_knife13
				{
					soundSet = "shaving_knife13_SoundSet";
					id = 613;
				};
				class shaving_knife14
				{
					soundSet = "shaving_knife14_SoundSet";
					id = 614;
				};
				class shaving_knife15
				{
					soundSet = "shaving_knife15_SoundSet";
					id = 615;
				};
				class shaving_knife16
				{
					soundSet = "shaving_knife16_SoundSet";
					id = 616;
				};
				class shaving_knife17
				{
					soundSet = "shaving_knife17_SoundSet";
					id = 617;
				};
				class shaving_knife18
				{
					soundSet = "shaving_knife18_SoundSet";
					id = 618;
				};
				class shaving_knife19
				{
					soundSet = "shaving_knife19_SoundSet";
					id = 619;
				};
				class shaving_knife20
				{
					soundSet = "shaving_knife20_SoundSet";
					id = 620;
				};
				class shaving_knife21
				{
					soundSet = "shaving_knife21_SoundSet";
					id = 621;
				};
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
			};
		};
	};
	class MSFZ_Magic_Knife_Blue: MSFZ_Magic_Knife
	{
		scope = 2;
		visibilityModifier = 0.85;
		SkinningDurationSpeed = 1.2;
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Weapons\Knife\data\MSFZ_Mifical_Knife_Blue.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Weapons\Knife\data\MSFZ_Mifical_Knife_Blue.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 1200;
					healthLevels[] = {{1,{"MSFZ_Mifical_Pack\Weapons\Knife\data\MSFZ_Mifical_Knife_Blue.rvmat"}},{0.7,{"MSFZ_Mifical_Pack\Weapons\Knife\data\MSFZ_Mifical_Knife_Blue.rvmat"}},{0.5,{"MSFZ_Mifical_Pack\Weapons\Knife\data\MSFZ_Mifical_Knife_damage.rvmat"}},{0.3,{"MSFZ_Mifical_Pack\Weapons\Knife\data\MSFZ_Mifical_Knife_damage.rvmat"}},{0,{"MSFZ_Mifical_Pack\Weapons\Knife\data\MSFZ_Mifical_Knife_destruct.rvmat"}}};
				};
			};
		};
		isMeleeWeapon = 1;
		suicideAnim = "onehanded";
		class MeleeModes
		{
			class Default
			{
				ammo = "MeleeKnife";
				range = 1.2;
			};
			class Heavy
			{
				ammo = "MeleeKnife_Heavy";
				range = 1.2;
			};
			class Sprint
			{
				ammo = "MeleeKnife_Heavy";
				range = 3.3;
			};
		};
	};
	class MSFZ_Magic_Knife_Green: MSFZ_Magic_Knife
	{
		scope = 2;
		visibilityModifier = 0.85;
		SkinningDurationSpeed = 1.2;
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Weapons\Knife\data\MSFZ_Mifical_Knife_Green.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Weapons\Knife\data\MSFZ_Mifical_Knife_Green.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 1200;
					healthLevels[] = {{1,{"MSFZ_Mifical_Pack\Weapons\Knife\data\MSFZ_Mifical_Knife_Green.rvmat"}},{0.7,{"MSFZ_Mifical_Pack\Weapons\Knife\data\MSFZ_Mifical_Knife_Green.rvmat"}},{0.5,{"MSFZ_Mifical_Pack\Weapons\Knife\data\MSFZ_Mifical_Knife_damage.rvmat"}},{0.3,{"MSFZ_Mifical_Pack\Weapons\Knife\data\MSFZ_Mifical_Knife_damage.rvmat"}},{0,{"MSFZ_Mifical_Pack\Weapons\Knife\data\MSFZ_Mifical_Knife_destruct.rvmat"}}};
				};
			};
		};
		isMeleeWeapon = 1;
		suicideAnim = "onehanded";
		class MeleeModes
		{
			class Default
			{
				ammo = "MeleeKnife";
				range = 1.2;
			};
			class Heavy
			{
				ammo = "MeleeKnife_Heavy";
				range = 1.2;
			};
			class Sprint
			{
				ammo = "MeleeKnife_Heavy";
				range = 3.3;
			};
		};
	};
	class MSFZ_Magic_Knife_Purple: MSFZ_Magic_Knife
	{
		scope = 2;
		visibilityModifier = 0.85;
		SkinningDurationSpeed = 1.2;
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Weapons\Knife\data\MSFZ_Mifical_Knife_Purple.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Weapons\Knife\data\MSFZ_Mifical_Knife_Purple.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 1200;
					healthLevels[] = {{1,{"MSFZ_Mifical_Pack\Weapons\Knife\data\MSFZ_Mifical_Knife_Purple.rvmat"}},{0.7,{"MSFZ_Mifical_Pack\Weapons\Knife\data\MSFZ_Mifical_Knife_Purple.rvmat"}},{0.5,{"MSFZ_Mifical_Pack\Weapons\Knife\data\MSFZ_Mifical_Knife_damage.rvmat"}},{0.3,{"MSFZ_Mifical_Pack\Weapons\Knife\data\MSFZ_Mifical_Knife_damage.rvmat"}},{0,{"MSFZ_Mifical_Pack\Weapons\Knife\data\MSFZ_Mifical_Knife_destruct.rvmat"}}};
				};
			};
		};
		isMeleeWeapon = 1;
		suicideAnim = "onehanded";
		class MeleeModes
		{
			class Default
			{
				ammo = "MeleeKnife";
				range = 1.2;
			};
			class Heavy
			{
				ammo = "MeleeKnife_Heavy";
				range = 1.2;
			};
			class Sprint
			{
				ammo = "MeleeKnife_Heavy";
				range = 3.3;
			};
		};
	};
	class MSFZ_Magic_Knife_Red: MSFZ_Magic_Knife
	{
		scope = 2;
		visibilityModifier = 0.85;
		SkinningDurationSpeed = 1.2;
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Weapons\Knife\data\MSFZ_Mifical_Knife_Red.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Weapons\Knife\data\MSFZ_Mifical_Knife_Red.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 1200;
					healthLevels[] = {{1,{"MSFZ_Mifical_Pack\Weapons\Knife\data\MSFZ_Mifical_Knife_Red.rvmat"}},{0.7,{"MSFZ_Mifical_Pack\Weapons\Knife\data\MSFZ_Mifical_Knife_Red.rvmat"}},{0.5,{"MSFZ_Mifical_Pack\Weapons\Knife\data\MSFZ_Mifical_Knife_damage.rvmat"}},{0.3,{"MSFZ_Mifical_Pack\Weapons\Knife\data\MSFZ_Mifical_Knife_damage.rvmat"}},{0,{"MSFZ_Mifical_Pack\Weapons\Knife\data\MSFZ_Mifical_Knife_destruct.rvmat"}}};
				};
			};
		};
		isMeleeWeapon = 1;
		suicideAnim = "onehanded";
		class MeleeModes
		{
			class Default
			{
				ammo = "MeleeKnife";
				range = 1.2;
			};
			class Heavy
			{
				ammo = "MeleeKnife_Heavy";
				range = 1.2;
			};
			class Sprint
			{
				ammo = "MeleeKnife_Heavy";
				range = 3.3;
			};
		};
	};
};
