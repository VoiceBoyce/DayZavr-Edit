class CfgPatches
{
	class Vp_SET20
	{
		units[] = {"Vp_SET20_Vest"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Characters","DZ_Scripts"};
	};
};
class CfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base{};
	class Vp_SET20_Vest_ColorBase: Clothing
	{
		scope = 0;
		displayName = "$STR_VBC_Legion_Vest";
		descriptionShort = "$STR_VBC_Legion_Vest_Desc";
		model = "\set_legion\set\Vest\model\vest_g.p3d";
		attachments[] = {};
		inventorySlot[] = {"Vest"};
		itemInfo[] = {"Clothing","Vest"};
		weight = 12000;
		itemSize[] = {5,5};
		quickBarBonus = 1;
		itemsCargoSize[] = {10,3};
		varWetMax = 0.49;
		heatIsolation = 0.8;
		repairableWithKits[] = {3,8};
		repairCosts[] = {75,50};
		visibilityModifier = 0.95;
		hiddenSelections[] = {"fast_vest","vest","vest_belt","vest3","znak_vest"};
		class ClothingTypes
		{
			male = "set_legion\set\Vest\model\vest_mf.p3d";
			female = "set_legion\set\Vest\model\vest_mf.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 2000;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1,{"set_legion\set\Vest\data\fast_vest.rvmat","set_legion\set\Vest\data\vest.rvmat","set_legion\set\Vest\data\vest_belt.rvmat","set_legion\set\Vest\data\vest3.rvmat","set_legion\set\Vest\data\znak_vest.rvmat"}},{0.7,{"set_legion\set\Vest\data\fast_vest.rvmat","set_legion\set\Vest\data\vest.rvmat","set_legion\set\Vest\data\vest_belt.rvmat","set_legion\set\Vest\data\vest3.rvmat","set_legion\set\Vest\data\znak_vest.rvmat"}},{0.5,{"set_legion\set\Vest\data\fast_vest_damage.rvmat","set_legion\set\Vest\data\vest_damage.rvmat","set_legion\set\Vest\data\vest_belt_damage.rvmat","set_legion\set\Vest\data\vest3_damage.rvmat","set_legion\set\Vest\data\znak_vest_damage.rvmat"}},{0.3,{"set_legion\set\Vest\data\fast_vest_damage.rvmat","set_legion\set\Vest\data\vest_damage.rvmat","set_legion\set\Vest\data\vest_belt_damage.rvmat","set_legion\set\Vest\data\vest3_damage.rvmat","set_legion\set\Vest\data\znak_vest_damage.rvmat"}},{0,{"set_legion\set\Vest\data\fast_vest_destruct.rvmat","set_legion\set\Vest\data\vest_destruct.rvmat","set_legion\set\Vest\data\vest_belt_destruct.rvmat","set_legion\set\Vest\data\vest3_destruct.rvmat","set_legion\set\Vest\data\znak_vest_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health
					{
						damage = 0.3;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0.6;
					};
				};
				class Melee
				{
					class Health
					{
						damage = 0.3;
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
						damage = 0.3;
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
						damage = 0.5;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0.26;
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
	class Vp_SET20_Vest: Vp_SET20_Vest_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"set_legion\set\Vest\data\fast_vest_co.paa","set_legion\set\Vest\data\vest_co.paa","set_legion\set\Vest\data\vest_belt_co.paa","set_legion\set\Vest\data\vest3_co.paa","set_legion\set\Vest\data\znak_vest_co.paa"};
	};
};
