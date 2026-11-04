class CfgPatches {
	class Vp_SET23 {
		units[] = {""};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Characters", "DZ_Characters_Vests"};
	};
};

class cfgVehicles {
	class Clothing_Base;
	class Clothing : Clothing_Base {};
	
	class Vp_SET23_Vest_ColorBase : Clothing {
		scope = 0;
		displayName = "$STR_VBC_Operator_Vest";
		descriptionShort = "$STR_VBC_Operator_Vest_Desc";
		model = "\set_operator\set\vest\model\vest_g.p3d";
		attachments[] = {};
		inventorySlot[] = {"Vest"};
		itemInfo[] = {"Clothing", "Vest"};
		quickBarBonus = 2;
		weight = 7000;
		itemSize[] = {5,4};
		varWetMax = 0.249;
		heatIsolation = 0.5;
		itemsCargoSize[] = {10,4};
		repairableWithKits[] = {3, 8};
		repairCosts[] = {75.0, 50.0};
		hiddenSelections[] = {"vest", "vest2", "vest3", "vest4"};
		
		class ClothingTypes {
			male = "\set_operator\set\vest\model\vest_mf.p3d";
			female = "\set_operator\set\vest\model\vest_mf.p3d";
		};
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 800;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0, {"\set_operator\set\vest\data\vest.rvmat", "\set_operator\set\vest\data\vest2.rvmat", "\set_operator\set\vest\data\vest3.rvmat", "\set_operator\set\vest\data\vest4.rvmat"}},
									 {0.7, {"\set_operator\set\vest\data\vest.rvmat", "\set_operator\set\vest\data\vest2.rvmat", "\set_operator\set\vest\data\vest3.rvmat", "\set_operator\set\vest\data\vest4.rvmat"}},
									 {0.5, {"\set_operator\set\vest\data\vest_damage.rvmat", "\set_operator\set\vest\data\vest2_damage.rvmat", "\set_operator\set\vest\data\vest3_damage.rvmat", "\set_operator\set\vest\data\vest4_damage.rvmat"}},
									 {0.3, {"\set_operator\set\vest\data\vest_damage.rvmat", "\set_operator\set\vest\data\vest2_damage.rvmat", "\set_operator\set\vest\data\vest3_damage.rvmat", "\set_operator\set\vest\data\vest4_damage.rvmat"}},
									 {0.0, {"\set_operator\set\vest\data\vest_destruct.rvmat", "\set_operator\set\vest\data\vest2_destruct.rvmat", "\set_operator\set\vest\data\vest3_destruct.rvmat", "\set_operator\set\vest\data\vest4_destruct.rvmat"}}};
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
		
		
		class AnimEvents {
			class SoundWeapon {
				class pickUpItem {
					soundSet = "SmershVest_pickup_SoundSet";
					id = 797;
				};
				
				class drop {
					soundset = "SmershVest_drop_SoundSet";
					id = 898;
				};
			};
		};
	};
	
	class Vp_SET23_Vest_Black : Vp_SET23_Vest_ColorBase {
		scope = 2;
		hiddenSelectionsTextures[] = {"\set_operator\set\vest\data\vest_co.paa",
									"\set_operator\set\vest\data\vest2_co.paa",
									"\set_operator\set\vest\data\vest3_co.paa",
									"\set_operator\set\vest\data\vest4_co.paa"};
	};
};
