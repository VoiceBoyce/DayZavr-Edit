class CfgPatches {
	class EFT_LootLord {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_LootLord : Inventory_Base {
		scope = 2;
displayName = "#EFT_LootLord_displayName";
descriptionShort = "#EFT_LootLord_descriptionShort";
		model = "EFT\barter\LootLord\EFT_LootLord.p3d";
		isMeleeWeapon = 1;
		weight = 200;
		absorbency = 0.1;
		itemSize[] = {2,2};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\LootLord\data\barter.rvmat"}}, {0.7, {"EFT\barter\LootLord\data\barter.rvmat"}}, {0.5, {"EFT\barter\LootLord\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\LootLord\data\barter_damage.rvmat"}}, {0, {"EFT\barter\LootLord\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
