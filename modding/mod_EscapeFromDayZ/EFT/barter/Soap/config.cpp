class CfgPatches {
	class EFT_Soap {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Soap : Inventory_Base {
		scope = 2;
displayName = "#EFT_Soap_displayName";
descriptionShort = "#EFT_Soap_descriptionShort";
		model = "EFT\barter\Soap\EFT_Soap.p3d";
		isMeleeWeapon = 1;
		weight = 80;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Soap\data\barter.rvmat"}}, {0.7, {"EFT\barter\Soap\data\barter.rvmat"}}, {0.5, {"EFT\barter\Soap\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Soap\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Soap\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
