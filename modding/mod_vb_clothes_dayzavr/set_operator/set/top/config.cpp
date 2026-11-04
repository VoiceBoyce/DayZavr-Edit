class CfgPatches {
	class Vp_SET23 {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Characters"};
	};
};

class cfgVehicles {
	class Clothing_Base;
	class Clothing : Clothing_Base {};
	
	class Vp_SET23_Top_ColorBase : Clothing {
		scope = 0;
		displayName = "$STR_set_OPERATOR_Top";
		descriptionShort = "";
		model = "\set_operator\set\top\model\top_g.p3d";
		attachments[] = {};
		inventorySlot[] = {"Body"};
		itemInfo[] = {"Clothing", "Body"};
		quickBarBonus = 2;
		weight = 310;
		itemSize[] = {3, 2};
		varWetMax = 0.05;
		heatIsolation = 0.5;
		itemsCargoSize[] = {10, 4};
		repairableWithKits[] = {5, 2};
		repairCosts[] = {75.0, 50.0};
		hiddenSelections[] = {"top", "top1"};
		
		class ClothingTypes {
			male = "\set_operator\set\top\model\top_mf.p3d";
			female = "\set_operator\set\top\model\top_mf.p3d";
		};
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 500;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0, {"\set_operator\set\top\data\top.rvmat", "\set_operator\set\top\data\top1.rvmat"}},
									 {0.7, {"\set_operator\set\top\data\top.rvmat", "\set_operator\set\top\data\top1.rvmat"}},
									 {0.5, {"\set_operator\set\top\data\top_damage.rvmat", "\set_operator\set\top\data\top1_damage.rvmat"}},
									 {0.3, {"\set_operator\set\top\data\top_damage.rvmat", "\set_operator\set\top\data\top1_damage.rvmat"}},
									 {0.0, {"\set_operator\set\top\data\top_destruct.rvmat", "\set_operator\set\top\data\top1_destruct.rvmat"}}};
				};
			};
			
			class GlobalArmor {
				class Projectile {
					class Health {
						damage = 0.85;
					};
					
					class Blood {
						damage = 0.8;
					};
					
					class Shock {
						damage = 0.8;
					};
				};
				
				class Melee {
					class Health {
						damage = 0.85;
					};
					
					class Blood {
						damage = 0.8;
					};
					
					class Shock {
						damage = 0.8;
					};
				};
				
				class Infected {
					class Health {
						damage = 0.85;
					};
					
					class Blood {
						damage = 0.8;
					};
					
					class Shock {
						damage = 0.8;
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
	
	class Vp_SET23_Top_Black : Vp_SET23_Top_ColorBase {
		scope = 2;
		hiddenSelectionsTextures[] = {"\set_operator\set\top\data\top_co.paa",
									  "\set_operator\set\top\data\top1_co.paa"};
	};
};
