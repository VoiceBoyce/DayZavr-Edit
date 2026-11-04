class CfgPatches
{
	class VP_SET15
	{
		units[] = {"VP_SET15_Shoes_Black"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Characters","DZ_Scripts"};
	};
};
class CfgVehicles
{
	class Clothing;
	class VP_SET15_Shoes_ColorBase: Clothing
	{
		scope = 0;
		displayName = "$STR_VBC_Moriarty_Shoes";
		descriptionShort = "$STR_VBC_Moriarty_Shoes_Desc";
		quickBarBonus = 0;
		model = "set_Elegant\set\Boots\model\boots_g.p3d";
		inventorySlot[] = {"Feet"};
		itemInfo[] = {"Clothing","Feet"};
		itemSize[] = {3,2};
		weight = 1300;
		durability = 1;
		varWetMax = 0.249;
		heatIsolation = 0.8;
		visibilityModifier = 0.9;
		repairableWithKits[] = {3};
		repairCosts[] = {25};
		soundAttType = "Sneakers";
		soundImpactType = "default";
		hiddenSelections[] = {"camo"};
		class ClothingTypes
		{
			male = "set_Elegant\set\Boots\model\boots_mf.p3d";
			female = "set_Elegant\set\Boots\model\boots_mf.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					healthLevels[] = {{1,{"set_Elegant\set\Boots\data\Boots.rvmat"}},{0.7,{"set_Elegant\set\Boots\data\Boots.rvmat"}},{0.5,{"set_Elegant\set\Boots\data\Boots_damage.rvmat"}},{0.3,{"set_Elegant\set\Boots\data\Boots_damage.rvmat"}},{0,{"set_Elegant\set\Boots\data\Boots_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Melee
				{
					class Health
					{
						damage = 0.75;
					};
					class Blood
					{
						damage = 0.75;
					};
					class Shock
					{
						damage = 0.75;
					};
				};
				class Infected
				{
					class Health
					{
						damage = 0.75;
					};
					class Blood
					{
						damage = 0.75;
					};
					class Shock
					{
						damage = 0.75;
					};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem
				{
					soundSet = "AthleticShoes_pickup_SoundSet";
					id = 797;
				};
				class drop
				{
					soundset = "AthleticShoes_drop_SoundSet";
					id = 898;
				};
			};
		};
	};
	class VP_SET15_Shoes_Black: VP_SET15_Shoes_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"set_Elegant\set\Boots\data\Boots_co.paa"};
	};
};
