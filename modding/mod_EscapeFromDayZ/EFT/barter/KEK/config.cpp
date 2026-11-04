class CfgPatches {
	class EFT_KEK {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_KEK : Inventory_Base {
		scope = 2;
		displayName = "#EFT_KEK_displayName";
		descriptionShort = "#EFT_KEK_descriptionShort";
		model = "EFT\barter\KEK\EFT_KEK.p3d";
		isMeleeWeapon = 1;
		weight = 150;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\KEK\data\barter.rvmat"}}, {0.7, {"EFT\barter\KEK\data\barter.rvmat"}}, {0.5, {"EFT\barter\KEK\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\KEK\data\barter_damage.rvmat"}}, {0, {"EFT\barter\KEK\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
