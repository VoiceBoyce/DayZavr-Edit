class CfgPatches
{
	class Vp_SET12
	{
		units[] = {"Vp_SET12_Top_Black"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Characters"};
	};
};
class cfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base{};
	class Vp_SET12_Top_ColorBase: Clothing
	{
		scope = 0;
		displayName = "$STR_Rider_Top";
		descriptionShort = "";
		model = "set_Rider\set\Top\Top_g.p3d";
		attachments[] = {};
		inventorySlot[] = {"Body"};
		itemInfo[] = {"Clothing","Body"};
		quickBarBonus = 2;
		weight = 310;
		itemSize[] = {3,2};
		varWetMax = 0.05;
		heatIsolation = 0.5;
		itemsCargoSize[] = {9,5};
		repairableWithKits[] = {5,2};
		repairCosts[] = {75.0,50.0};
		hiddenSelections[] = {"camo","monster"};
		class ClothingTypes
		{
			male = "set_Rider\set\Top\Top_mf.p3d";
			female = "set_Rider\set\Top\Top_mf.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"set_Rider\set\Top\data\Top.rvmat","set_Rider\set\Top\data\Monset_top.rvmat"}},{0.7,{"set_Rider\set\Top\data\Top.rvmat","set_Rider\set\Top\data\Monset_top.rvmat"}},{0.5,{"set_Rider\set\Top\data\Top_damage.rvmat","set_Rider\set\Top\data\Monset_top_damage.rvmat"}},{0.3,{"set_Rider\set\Top\data\Top_damage.rvmat","set_Rider\set\Top\data\Monset_top_damage.rvmat"}},{0.0,{"set_Rider\set\Top\data\Top_destruct.rvmat","set_Rider\set\Top\data\Monset_top_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health
					{
						damage = 0.85;
					};
					class Blood
					{
						damage = 0.8;
					};
					class Shock
					{
						damage = 0.8;
					};
				};
				class Melee
				{
					class Health
					{
						damage = 0.85;
					};
					class Blood
					{
						damage = 0.8;
					};
					class Shock
					{
						damage = 0.8;
					};
				};
				class Infected
				{
					class Health
					{
						damage = 0.85;
					};
					class Blood
					{
						damage = 0.8;
					};
					class Shock
					{
						damage = 0.8;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage = 0.2;
					};
					class Blood
					{
						damage = 0.2;
					};
					class Shock
					{
						damage = 0.2;
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
					soundSet = "Shirt_pickup_SoundSet";
					id = 797;
				};
				class drop
				{
					soundset = "Shirt_drop_SoundSet";
					id = 898;
				};
			};
		};
	};
	class Vp_SET12_Top_Black: Vp_SET12_Top_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"set_Rider\set\Top\data\Top_co.paa","set_Rider\set\Top\data\Monset_top_co.paa"};
	};
};
