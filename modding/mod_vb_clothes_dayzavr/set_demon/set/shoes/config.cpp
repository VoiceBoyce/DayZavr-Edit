class CfgPatches {
	class VP_SET22{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Characters", "DZ_Scripts"};
	};
};

class CfgVehicles {
	class Clothing;	// External class reference
	
	class VP_SET22_Shoes_ColorBase : Clothing {
		scope = 0;
		displayName = "$STR_set_DEMON_Shoes";
		descriptionShort = "";
		model = "\set_demon\set\shoes\model\shoes_g.p3d";
		inventorySlot[] = {"Feet"};
		itemInfo[] = {"Clothing", "Feet"};
		itemSize[] = {4, 2};
		weight = 500;
		durability = 1;
		varWetMax = 1;
		heatIsolation = 1;
		visibilityModifier = 0.9;
		repairableWithKits[] = {3};
		repairCosts[] = {25};
		soundAttType = "Sneakers";
		soundImpactType = "default";
		hiddenSelections[] = {"shoes"};
		
		class ClothingTypes {
			male = "\set_demon\set\shoes\model\shoes_mf.p3d";
			female = "\set_demon\set\shoes\model\shoes_mf.p3d";
		};
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 290;
					healthLevels[] = {{1, {"\set_demon\set\shoes\data\shoes.rvmat"}},
									{0.7, {"\set_demon\set\shoes\data\shoes.rvmat"}},
									{0.5, {"\set_demon\set\shoes\data\shoes_damage.rvmat"}},
									{0.3, {"\set_demon\set\shoes\data\shoes_damage.rvmat"}},
									{0, {"\set_demon\set\shoes\data\shoes_destruct.rvmat"}}};
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
		
		class Protection {
			biological = 1;
			chemical = 1;
		};		
		
		class AnimEvents {
			class SoundWeapon {
				class pickUpItem {
					soundSet = "AthleticShoes_pickup_SoundSet";
					id = 797;
				};
				
				class drop {
					soundset = "AthleticShoes_drop_SoundSet";
					id = 898;
				};
			};
		};
	};
	
	class VP_SET22_Shoes_Red : VP_SET22_Shoes_ColorBase {
		scope = 2;
		hiddenSelectionsTextures[] = {"\set_demon\set\shoes\data\shoes_co.paa"};
	};
};
