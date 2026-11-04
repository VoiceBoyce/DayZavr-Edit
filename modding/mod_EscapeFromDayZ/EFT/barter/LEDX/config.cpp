class CfgPatches {
	class EFT_LEDX {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_LEDX : Inventory_Base {
		scope = 2;
displayName = "#EFT_LEDX_displayName";
descriptionShort = "#EFT_LEDX_descriptionShort";
		model = "EFT\barter\LEDX\EFT_LEDX.p3d";
		isMeleeWeapon = 1;
		weight = 200;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\LEDX\data\barter.rvmat"}}, {0.7, {"EFT\barter\LEDX\data\barter.rvmat"}}, {0.5, {"EFT\barter\LEDX\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\LEDX\data\barter_damage.rvmat"}}, {0, {"EFT\barter\LEDX\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
