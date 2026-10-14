class CfgPatches {
	class Vp_SET23 {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Characters", "DZ_Scripts"};
	};
};

class cfgVehicles {
	class Clothing_Base;
	class Clothing : Clothing_Base {};
	
	class Vp_SET23_Pants_ColorBase : Clothing {
		scope = 0;
		displayName = "$STR_set_OPERATOR_Pants";
		descriptionShort = "";
		model = "\set_operator\set\pants\model\pants_g.p3d";
		attachments[] = {};
		inventorySlot[] = {"Legs"};
		itemInfo[] = {"Clothing", "Legs"};
		quickBarBonus = 2;
		weight = 310;
		ragQuantity = 6;
		itemSize[] = {3, 2};
		varWetMax = 0.05;
		heatIsolation = 0.5;
		itemsCargoSize[] = {10, 4};
		repairableWithKits[] = {5, 2};
		repairCosts[] = {75.0, 50.0};
		hiddenSelections[] = {"pants", "personality"};
		
		class ClothingTypes {
			male = "\set_operator\set\pants\model\pants_m.p3d";
			female = "\set_operator\set\pants\model\pants_f.p3d";
		};
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 500;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0, {"\set_operator\set\pants\data\pants.rvmat"}},
									 {0.7, {"\set_operator\set\pants\data\pants.rvmat"}},
									 {0.5, {"\set_operator\set\pants\data\pants_damage.rvmat"}},
									 {0.3, {"\set_operator\set\pants\data\pants_damage.rvmat"}},
									 {0.0, {"\set_operator\set\pants\data\pants_destruct.rvmat"}}};
				};
			};
			
			class GlobalArmor {
				class Projectile {
					class Health {
						damage = 0.9;
					};
					
					class Blood {
						damage = 0.9;
					};
					
					class Shock {
						damage = 1;
					};
				};
				
				class Melee {
					class Health {
						damage = 0.9;
					};
					
					class Blood {
						damage = 0.9;
					};
					
					class Shock {
						damage = 1;
					};
				};
				
				class Infected {
					class Health {
						damage = 0.9;
					};
					
					class Blood {
						damage = 0.9;
					};
					
					class Shock {
						damage = 1;
					};
				};
				
				class FragGrenade {
					class Health {
						damage = 0.2;
					};
					
					class Blood {
						damage = 0.2;
					};
					
					class Shock {
						damage = 0.2;
					};
				};
			};
		};
				
		
		class AnimEvents {
			class SoundWeapon {
				class pickUpItem {
					soundSet = "Shirt_pickup_SoundSet";
					id = 797;
				};
				
				class drop {
					soundset = "Shirt_drop_SoundSet";
					id = 898;
				};
			};
		};
	};
	
	class Vp_SET23_Pants_Black : Vp_SET23_Pants_ColorBase {
		scope = 2;
		hiddenSelectionsTextures[] = {"\set_operator\set\pants\data\pants_co.paa"};
	};
};
