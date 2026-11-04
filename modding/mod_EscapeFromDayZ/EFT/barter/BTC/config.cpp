class CfgPatches {
	class EFT_BTC {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_BTC : Inventory_Base {
		scope = 2;
		displayName = "#EFT_BTC_displayName";
		descriptionShort = "#EFT_BTC_descriptionShort";
		model = "EFT\barter\BTC\EFT_BTC.p3d";
		isMeleeWeapon = 1;
		weight = 10;
		absorbency = 0.1;
		itemSize[] = {1, 1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\BTC\data\barter.rvmat"}}, {0.7, {"EFT\barter\BTC\data\barter.rvmat"}}, {0.5, {"EFT\barter\BTC\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\BTC\data\barter_damage.rvmat"}}, {0, {"EFT\barter\BTC\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
