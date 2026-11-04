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
	
	class Vp_Set23_Helmet_ColorBase : Clothing {
		scope = 0;
		displayName = "$STR_set_OPETATOR_cap";
		descriptionShort = "";
		model = "\set_operator\set\helmet\model\helmet_g.p3d";
		attachments[] = {};
		inventorySlot[] = {"Headgear"};
		simulation = "clothing";
		vehicleClass = "Clothing";
		itemInfo[] = {"Clothing", "Helmet"};
		ragQuantity = 6;
		quickBarBonus = 2;
		weight = 310;
		itemSize[] = {3, 3};
		varWetMax = 0.05;
		heatIsolation = 0.5;
		repairableWithKits[] = {3, 8};
		repairCosts[] = {75.0, 50.0};
		hiddenSelections[] = {"head1", "head2", "head3", "head4", "head5", "head6", "head7"};
		headSelectionsToHide[] = {"Clipping_Balaclava"};
		
		class ClothingTypes {
			male = "\set_operator\set\helmet\model\helmet_mf.p3d";
			female = "\set_operator\set\helmet\model\helmet_mf.p3d";
		};
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 500;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0, {"\set_operator\set\helmet\data\head1.rvmat", "\set_operator\set\helmet\data\head2.rvmat", "\set_operator\set\helmet\data\head3.rvmat", "\set_operator\set\helmet\data\head4.rvmat", "\set_operator\set\helmet\data\head5.rvmat", "\set_operator\set\helmet\data\head6.rvmat", "\set_operator\set\helmet\data\head7.rvmat"}},
									 {0.7, {"\set_operator\set\helmet\data\head1.rvmat", "\set_operator\set\helmet\data\head2.rvmat", "\set_operator\set\helmet\data\head3.rvmat", "\set_operator\set\helmet\data\head4.rvmat", "\set_operator\set\helmet\data\head5.rvmat", "\set_operator\set\helmet\data\head6.rvmat", "\set_operator\set\helmet\data\head7.rvmat"}},
									 {0.5, {"\set_operator\set\helmet\data\head1_damage.rvmat", "\set_operator\set\helmet\data\head2_damage.rvmat", "\set_operator\set\helmet\data\head3_damage.rvmat", "\set_operator\set\helmet\data\head4_damage.rvmat", "\set_operator\set\helmet\data\head5_damage.rvmat", "\set_operator\set\helmet\data\head6_damage.rvmat", "\set_operator\set\helmet\data\head7_damage.rvmat"}},
									 {0.3, {"\set_operator\set\helmet\data\head1_damage.rvmat", "\set_operator\set\helmet\data\head2_damage.rvmat", "\set_operator\set\helmet\data\head3_damage.rvmat", "\set_operator\set\helmet\data\head4_damage.rvmat", "\set_operator\set\helmet\data\head5_damage.rvmat", "\set_operator\set\helmet\data\head6_damage.rvmat", "\set_operator\set\helmet\data\head7_damage.rvmat"}},
									 {0.0, {"\set_operator\set\helmet\data\head1_destruct.rvmat", "\set_operator\set\helmet\data\head2_destruct.rvmat", "\set_operator\set\helmet\data\head3_destruct.rvmat", "\set_operator\set\helmet\data\head4_destruct.rvmat", "\set_operator\set\helmet\data\head5_destruct.rvmat", "\set_operator\set\helmet\data\head6_destruct.rvmat", "\set_operator\set\helmet\data\head7_destruct.rvmat"}}};
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
	
	class Vp_Set23_Helmet_Black : Vp_Set23_Helmet_ColorBase {
		scope = 2;
		hiddenSelectionsTextures[] = {"\set_operator\set\helmet\data\head1_co.paa",
									"\set_operator\set\helmet\data\head2_co.paa",
									"\set_operator\set\helmet\data\head3_co.paa",
									"\set_operator\set\helmet\data\head4_co.paa",
									"\set_operator\set\helmet\data\head5_co.paa",
									"\set_operator\set\helmet\data\head6_ca.paa",
									"\set_operator\set\helmet\data\head7_co.paa"};
	};
};
