class CfgPatches {
	class Vp_SET22 {
		units[] = {""};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Characters", "DZ_Scripts"};
	};
};

class cfgVehicles {
	class Clothing;
	
	class Vp_SET22_Gloves_ColorBase : Clothing {
		scope = 0;
		displayName = "$STR_DEMON_Glove";
		descriptionShort = "";
		model = "\set_demon\set\gloves\model\gloves_g.p3d";
		inventorySlot[] = {"Gloves"};
		simulation = "clothing";
		itemInfo[] = {"Clothing", "Gloves"};
		rotationFlags = 34;
		weight = 454;
		itemSize[] = {2, 2};
		varWetMax = 0.049;
		heatIsolation = 1;
		repairableWithKits[] = {5, 2};
		repairCosts[] = {30, 25};
		hiddenSelections[] = {"camo", "personality"};
		
		class ClothingTypes {
			male = "\set_demon\set\gloves\model\gloves_m.p3d";
			female = "\set_demon\set\gloves\model\gloves_f.p3d";
		};
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 500;
					healthLevels[] = {{1.0, {"\set_demon\set\gloves\data\Gloves.rvmat"}},
									 {0.7, {"\set_demon\set\gloves\data\Gloves.rvmat"}},
									 {0.5, {"\set_demon\set\gloves\data\Gloves_damage.rvmat"}},
									 {0.3, {"\set_demon\set\gloves\data\Gloves_damage.rvmat"}},
									 {0.0, {"\set_demon\set\gloves\data\Gloves_destruct.rvmat"}}};
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
	
	class Vp_SET22_Gloves_Red : Vp_SET22_Gloves_ColorBase {
		scope = 2;
		hiddenSelectionsTextures[] = {"\set_demon\set\gloves\data\Gloves_co.paa"};
	};
};
