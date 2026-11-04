class CfgPatches {
	class EFT_Vertex {
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Gear_Medical"};
	};
};

class CfgVehicles {
	class Inventory_Base;	// External class reference
	
	class EFT_Vertex : Inventory_Base {
		scope = 2;
		displayName = "#EFT_Vertex_displayName";
		descriptionShort = "#EFT_Vertex_descriptionShort";
		model = "EFT\barter\Vertex\EFT_Vertex.p3d";
		isMeleeWeapon = 1;
		weight = 300;
		absorbency = 0.1;
		itemSize[] = {1,1};
		rotationFlags = 17;
		
		class DamageSystem {
			class GlobalHealth {
				class Health {
					hitpoints = 1000;
					healthLevels[] = {{1, {"EFT\barter\Vertex\data\barter.rvmat"}}, {0.7, {"EFT\barter\Vertex\data\barter.rvmat"}}, {0.5, {"EFT\barter\Vertex\data\barter_damage.rvmat"}}, {0.3, {"EFT\barter\Vertex\data\barter_damage.rvmat"}}, {0, {"EFT\barter\Vertex\data\barter_destruct.rvmat"}}};
				};
			};
		};
	};
};
