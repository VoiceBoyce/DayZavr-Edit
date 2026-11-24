class CfgPatches {
	class EFT_RFIDreader {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_RFIDreader : Inventory_Base {
		scope = 2;
displayName = "#EFT_RFIDreader_displayName";
descriptionShort = "#EFT_RFIDreader_descriptionShort";
		model = "EFT\barter\RFIDreader\EFT_RFIDreader.p3d";
		isMeleeWeapon = 1;
		weight = 500;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\RFIDreader\data\barter.rvmat"}}, {0.7, {"EFT\barter\RFIDreader\data\barter.rvmat"}}, {0.5, {"EFT\barter\RFIDreader\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\RFIDreader\data\barter_damage.rvmat"}}, {0, {"EFT\barter\RFIDreader\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
