class CfgPatches {
	class EFT_SiliconTube {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_SiliconTube : Inventory_Base {
		scope = 2;
displayName = "#EFT_SiliconTube_displayName";
descriptionShort = "#EFT_SiliconTube_descriptionShort";
		model = "EFT\barter\SiliconTube\EFT_SiliconTube.p3d";
		isMeleeWeapon = 1;
		weight = 220;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\SiliconTube\data\barter.rvmat"}}, {0.7, {"EFT\barter\SiliconTube\data\barter.rvmat"}}, {0.5, {"EFT\barter\SiliconTube\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\SiliconTube\data\barter_damage.rvmat"}}, {0, {"EFT\barter\SiliconTube\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
