class CfgPatches {
	class EFT_Military_cable {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Military_cable : Inventory_Base {
		scope = 2;
displayName = "#EFT_Military_cable_displayName";
descriptionShort = "#EFT_Military_cable_descriptionShort";
		model = "EFT\barter\Military_cable\EFT_Military_cable.p3d";
		isMeleeWeapon = 1;
		weight = 650;
		absorbency = 0.1;
		itemSize[] = {2,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Military_cable\data\barter.rvmat"}}, {0.7, {"EFT\barter\Military_cable\data\barter.rvmat"}}, {0.5, {"EFT\barter\Military_cable\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Military_cable\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Military_cable\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
