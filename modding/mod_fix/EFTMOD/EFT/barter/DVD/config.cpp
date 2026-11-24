class CfgPatches {
	class EFT_DVD {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_DVD : Inventory_Base {
		scope = 2;
		displayName = "#EFT_DVD_displayName";
		descriptionShort = "#EFT_DVD_descriptionShort";
		model = "EFT\barter\DVD\EFT_DVD.p3d";
		isMeleeWeapon = 1;
		weight = 600;
		absorbency = 0.1;
		itemSize[] = {2,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\DVD\data\barter.rvmat"}}, {0.7, {"EFT\barter\DVD\data\barter.rvmat"}}, {0.5, {"EFT\barter\DVD\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\DVD\data\barter_damage.rvmat"}}, {0, {"EFT\barter\DVD\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
