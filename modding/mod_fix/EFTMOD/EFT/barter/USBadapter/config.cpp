class CfgPatches {
	class EFT_USBadapter {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_USBadapter : Inventory_Base {
		scope = 2;
displayName = "#EFT_USBadapter_displayName";
descriptionShort = "#EFT_USBadapter_descriptionShort";
		model = "EFT\barter\USBadapter\EFT_USBadapter.p3d";
		isMeleeWeapon = 1;
		weight = 400;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\USBadapter\data\barter.rvmat"}}, {0.7, {"EFT\barter\USBadapter\data\barter.rvmat"}}, {0.5, {"EFT\barter\USBadapter\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\USBadapter\data\barter_damage.rvmat"}}, {0, {"EFT\barter\USBadapter\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
