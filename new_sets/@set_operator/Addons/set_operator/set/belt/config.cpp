class CfgPatches {
	class VP_SET23 {
		units[] = {""};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Characters", "DZ_Scripts"};
	};
};

class cfgVehicles {
	class Clothing_Base;
	class Clothing : Clothing_Base {};
	
	class VP_SET23_Belt_ColorBase : Clothing {
		scope = 0;
		displayName = "$STR_set_OPERATOR_Belt";
		descriptionShort = "";
		model = "\set_operator\set\belt\model\belt_g.p3d";
		attachments[] = {};
		vehicleClass = "Clothing";
		simulation = "clothing";
		inventorySlot[] = {"Hips"};
		itemInfo[] = {"Clothing", "Hips"};
		itemSize[] = {5, 5};
		itemsCargoSize[] = {10, 4};
		quickBarBonus = 2;
		varWetMax = 0.049;
		heatIsolation = 0.5;
		weight = 550;
		repairableWithKits[] = {5, 3};
		repairCosts[] = {30.0, 25.0};
		hiddenSelections[] = {"belt", "belt2", "belt3", "belt4"};
		
		class ClothingTypes {
			male = "\set_operator\set\belt\model\belt_mf.p3d";
			female = "\set_operator\set\belt\model\belt_mf.p3d";
		};
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 500;
					healthLevels[] = {{1, {"\set_operator\set\belt\data\belt.rvmat", "\set_operator\set\belt\data\belt2.rvmat", "\set_operator\set\belt\data\belt3.rvmat", "\set_operator\set\belt\data\belt4.rvmat"}},
									{0.7, {"\set_operator\set\belt\data\belt.rvmat", "\set_operator\set\belt\data\belt2.rvmat", "\set_operator\set\belt\data\belt3.rvmat", "\set_operator\set\belt\data\belt4.rvmat"}},
									{0.5, {"\set_operator\set\belt\data\belt_damage.rvmat", "\set_operator\set\belt\data\belt2_damage.rvmat", "\set_operator\set\belt\data\belt3_damage.rvmat", "\set_operator\set\belt\data\belt4_damage.rvmat"}},
									{0.3, {"\set_operator\set\belt\data\belt_damage.rvmat", "\set_operator\set\belt\data\belt2_damage.rvmat", "\set_operator\set\belt\data\belt3_damage.rvmat", "\set_operator\set\belt\data\belt4_damage.rvmat"}},
									{0, {"\set_operator\set\belt\data\belt_destruct.rvmat", "\set_operator\set\belt\data\belt2_destruct.rvmat", "\set_operator\set\belt\data\belt3_destruct.rvmat", "\set_operator\set\belt\data\belt4_destruct.rvmat"}}};
				};
			};
			
			class GlobalArmor {
				class Projectile {
					class Health {
						damage = 0.75;
					};
					
					class Blood {
						damage = 0.6;
					};
					
					class Shock {
						damage = 0.75;
					};
				};
				
				class Melee {
					class Health {
						damage = 0.75;
					};
					
					class Blood {
						damage = 0.6;
					};
					
					class Shock {
						damage = 0.75;
					};
				};
				
				class Infected {
					class Health {
						damage = 0.75;
					};
					
					class Blood {
						damage = 0.6;
					};
					
					class Shock {
						damage = 0.75;
					};
				};
				
				class FragGrenade {
					class Health {
						damage = 0.75;
					};
					
					class Blood {
						damage = 0.6;
					};
					
					class Shock {
						damage = 0.75;
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
					soundSet = "WorkingGloves_pickup_SoundSet";
					id = 797;
				};
				
				class drop {
					soundset = "WorkingGloves_drop_SoundSet";
					id = 898;
				};
			};
		};
	};
	
	class VP_SET23_Belt_Black : VP_SET23_Belt_ColorBase {
		scope = 2;
		hiddenSelectionsTextures[] = {"\set_operator\set\belt\data\belt_co.paa",
									  "\set_operator\set\belt\data\belt2_co.paa",
									  "\set_operator\set\belt\data\belt3_co.paa",
									  "\set_operator\set\belt\data\belt4_co.paa"};
	};
};
