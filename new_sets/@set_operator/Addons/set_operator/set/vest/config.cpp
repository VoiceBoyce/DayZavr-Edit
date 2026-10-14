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
		displayName = "$STR_set_OPERATOR_vest";
		descriptionShort = "";
		model = "\set_operator\set\vest\model\vest_g.p3d";
		attachments[] = {};
		inventorySlot[] = {"Vest"};
		itemInfo[] = {"Clothing", "Vest"};
		quickBarBonus = 2;
		weight = 3000;
		itemSize[] = {4, 4};
		varWetMax = 0.05;
		heatIsolation = 0.5;
		itemsCargoSize[] = {5, 5};
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
					hitpoints = 1500;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0, {"\set_operator\set\vest\data\vest.rvmat", "\set_operator\set\vest\data\vest2.rvmat", "\set_operator\set\vest\data\vest3.rvmat", "\set_operator\set\vest\data\vest4.rvmat"}},
									 {0.7, {"\set_operator\set\vest\data\vest.rvmat", "\set_operator\set\vest\data\vest2.rvmat", "\set_operator\set\vest\data\vest3.rvmat", "\set_operator\set\vest\data\vest4.rvmat"}},
									 {0.5, {"\set_operator\set\vest\data\vest_damage.rvmat", "\set_operator\set\vest\data\vest2_damage.rvmat", "\set_operator\set\vest\data\vest3_damage.rvmat", "\set_operator\set\vest\data\vest4_damage.rvmat"}},
									 {0.3, {"\set_operator\set\vest\data\vest_damage.rvmat", "\set_operator\set\vest\data\vest2_damage.rvmat", "\set_operator\set\vest\data\vest3_damage.rvmat", "\set_operator\set\vest\data\vest4_damage.rvmat"}},
									 {0.0, {"\set_operator\set\vest\data\vest_destruct.rvmat", "\set_operator\set\vest\data\vest2_destruct.rvmat", "\set_operator\set\vest\data\vest3_destruct.rvmat", "\set_operator\set\vest\data\vest4_destruct.rvmat"}}};
				};
			};
			class GlobalArmor {
				class Projectile {
					class Health {
						damage = 0.1;
					};
					
					class Blood {
						damage = 0.1;
					};
					
					class Shock {
						damage = 0.1;
					};
				};
				
				class Melee {
					class Health {
						damage = 0.1;
					};
					
					class Blood {
						damage = 0.1;
					};
					
					class Shock {
						damage = 0.1;
					};
				};
				
				class Infected {
					class Health {
						damage = 0.1;
					};
					
					class Blood {
						damage = 0.1;
					};
					
					class Shock {
						damage = 0.1;
					};
				};
				
				class FragGrenade {
					class Health {
						damage = 0.1;
					};
					
					class Blood {
						damage = 0.1;
					};
					
					class Shock {
						damage = 0.1;
					};
				};
			};
		};
		
		class Protection {
			biological = 1;
			chemical = 1;
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
