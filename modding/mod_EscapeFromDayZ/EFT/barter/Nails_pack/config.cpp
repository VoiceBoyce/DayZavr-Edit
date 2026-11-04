class CfgPatches {
	class EFT_Nails_pack {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Nails_pack : Inventory_Base {
		scope = 2;
displayName = "#EFT_Nails_pack_displayName";
descriptionShort = "#EFT_Nails_pack_descriptionShort";
		model = "EFT\barter\Nails_pack\EFT_Nails_pack.p3d";
		isMeleeWeapon = 1;
		weight = 600;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Nails_pack\data\barter.rvmat"}}, {0.7, {"EFT\barter\Nails_pack\data\barter.rvmat"}}, {0.5, {"EFT\barter\Nails_pack\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Nails_pack\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Nails_pack\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
