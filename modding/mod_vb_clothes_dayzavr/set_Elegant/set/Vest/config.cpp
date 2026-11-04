class CfgPatches
{
	class Vp_SET15
	{
		units[] = {"Vp_SET15_Vest_Black"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Characters","DZ_Characters_Vests"};
	};
};
class cfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base{};
	class Vp_SET15_Vest_ColorBase: Clothing
	{
		scope = 0;
		displayName = "$STR_VBC_Moriarty_Vest";
		descriptionShort = "$STR_VBC_Moriarty_Vest_Desc";
		itemsCargoSize[] = {10,4};
		model = "set_Elegant\set\Vest\model\vest_g.p3d";
		attachments[] = {"pistol"};
		inventorySlot[] = {"Vest"};
		itemInfo[] = {"Clothing","Vest"};
		quickBarBonus = 2;
		weight = 7000;
		itemSize[] = {5,4};
		varWetMax = 0.249;
		heatIsolation = 0.5;
		repairableWithKits[] = {3,8};
		repairCosts[] = {75.0,50.0};
		hiddenSelections[] = {"karman","magaz_vest","planshet","vest"};
		class ClothingTypes
		{
			male = "set_Elegant\set\Vest\model\vest_mf.p3d";
			female = "set_Elegant\set\Vest\model\vest_mf.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 800;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"set_Elegant\set\Vest\data\karman.rvmat","set_Elegant\set\Vest\data\Magaz.rvmat","set_Elegant\set\Vest\data\planshet.rvmat","set_Elegant\set\Vest\data\vest.rvmat"}},{0.7,{"set_Elegant\set\Vest\data\karman.rvmat","set_Elegant\set\Vest\data\Magaz.rvmat","set_Elegant\set\Vest\data\planshet.rvmat","set_Elegant\set\Vest\data\vest.rvmat"}},{0.5,{"set_Elegant\set\Vest\data\karman_damage.rvmat","set_Elegant\set\Vest\data\Magaz_damage.rvmat","set_Elegant\set\Vest\data\planshet_damage.rvmat","set_Elegant\set\Vest\data\vest_damage.rvmat"}},{0.3,{"set_Elegant\set\Vest\data\karman_damage.rvmat","set_Elegant\set\Vest\data\Magaz_damage.rvmat","set_Elegant\set\Vest\data\planshet_damage.rvmat","set_Elegant\set\Vest\data\vest_damage.rvmat"}},{0.0,{"set_Elegant\set\Vest\data\karman_destruct.rvmat","set_Elegant\set\Vest\data\Magaz_destruct.rvmat","set_Elegant\set\Vest\data\planshet_destruct.rvmat","set_Elegant\set\Vest\data\vest_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health
					{
						damage = 0.2;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0.5;
					};
				};
				class Melee
				{
					class Health
					{
						damage = 0.25;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0.25;
					};
				};
				class Infected
				{
					class Health
					{
						damage = 0.25;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0.25;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage = 0.4;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0.4;
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
					soundSet = "SmershVest_pickup_SoundSet";
					id = 797;
				};
				class drop
				{
					soundset = "SmershVest_drop_SoundSet";
					id = 898;
				};
			};
		};
	};
	class Vp_SET15_Vest_Black: Vp_SET15_Vest_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"set_Elegant\set\Vest\data\karman_vest.paa","set_Elegant\set\Vest\data\Magaz_vest.paa","set_Elegant\set\Vest\data\planshet_co.paa","set_Elegant\set\Vest\data\vest_co.paa"};
	};
};
