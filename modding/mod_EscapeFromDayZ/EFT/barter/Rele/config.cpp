class CfgPatches {
	class EFT_Rele {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Rele : Inventory_Base {
		scope = 2;
displayName = "#EFT_Rele_displayName";
descriptionShort = "#EFT_Rele_descriptionShort";
		model = "EFT\barter\Rele\EFT_Rele.p3d";
		isMeleeWeapon = 1;
		weight = 80;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Rele\data\barter.rvmat"}}, {0.7, {"EFT\barter\Rele\data\barter.rvmat"}}, {0.5, {"EFT\barter\Rele\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Rele\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Rele\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
