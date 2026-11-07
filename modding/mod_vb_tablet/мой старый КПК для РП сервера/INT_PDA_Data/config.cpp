// INT_PDA — data addon: the PDA and its four modules (spec.md §4). Models come from the old BestPDA_CUSTOM (ODOL,
// copied unchanged); every selection is hidden and re-textured here, so the paths baked into the p3d are never used.
// Shipped inside @INT_PDA_CL/addons. Texts are stringtable keys of INT_PDA_CL.
// Battery: vanilla EnergyManager like the GPS receiver (the engine drains the 9V battery; no script timer of ours).
// 0.0025 per second = a full 9V battery (50) lasts about 5.5 hours of switched-on time.

class CfgPatches
{
	class INT_PDA_Data
	{
		units[] = {"INT_PDA", "INT_PDA_Module_GPS", "INT_PDA_Module_MAP", "INT_PDA_Module_Chat", "INT_PDA_Module_GP"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Scripts"};
	};
};

class CfgSlots
{
	class Slot_INT_PDA_Module_GPS
	{
		name = "INT_PDA_Module_GPS";
		displayName = "$STR_INT_PDA_SLOT_GPS";
		ghostIcon = "set:int_pda_slots image:gps";
	};
	class Slot_INT_PDA_Module_MAP
	{
		name = "INT_PDA_Module_MAP";
		displayName = "$STR_INT_PDA_SLOT_MAP";
		ghostIcon = "set:int_pda_slots image:map";
	};
	class Slot_INT_PDA_Module_Chat
	{
		name = "INT_PDA_Module_Chat";
		displayName = "$STR_INT_PDA_SLOT_CHAT";
		ghostIcon = "set:int_pda_slots image:chat";
	};
	class Slot_INT_PDA_Module_GP
	{
		name = "INT_PDA_Module_GP";
		displayName = "$STR_INT_PDA_SLOT_GP";
		ghostIcon = "set:int_pda_slots image:gp";
	};
};

class CfgVehicles
{
	class Inventory_Base;

	class INT_PDA_Base: Inventory_Base
	{
		scope = 0;
		displayName = "$STR_INT_PDA_ITEM_PDA";
		descriptionShort = "$STR_INT_PDA_ITEM_PDA_DESC";
		model = "INT_PDA_Data\data\pda\int_pda.p3d";
		hiddenSelections[] = {"corpus", "back_face", "glass"};
		hiddenSelectionsTextures[] =
		{
			"INT_PDA_Data\data\pda\corpus\pristine\pda_pristine_co.paa",
			"INT_PDA_Data\data\pda\back_face\pristine\pda_pristine_co.paa",
			"INT_PDA_Data\data\pda\glass\pristine\pda_pristine_ca.paa"
		};
		hiddenSelectionsMaterials[] =
		{
			"INT_PDA_Data\data\pda\corpus\pristine\pda_pristine.rvmat",
			"INT_PDA_Data\data\pda\back_face\pristine\pda_pristine.rvmat",
			"INT_PDA_Data\data\pda\glass\pristine\pda_pristine.rvmat"
		};
		attachments[] = {"BatteryD", "INT_PDA_Module_GPS", "INT_PDA_Module_MAP", "INT_PDA_Module_Chat", "INT_PDA_Module_GP"};
		weight = 250;
		absorbency = 0.5;
		itemSize[] = {2, 2};
		rotationFlags = 17;
		soundImpactType = "plastic";
		repairableWithKits[] = {7};
		repairCosts[] = {25};
		class EnergyManager
		{
			hasIcon = 1;
			autoSwitchOffWhenInCargo = 0;
			energyUsagePerSecond = 0.0025;
			plugType = 1;
			attachmentAction = 1;
			updateInterval = 30;
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 90;
					healthLevels[] =
					{
						{1, {"INT_PDA_Data\data\pda\corpus\pristine\pda_pristine.rvmat", "INT_PDA_Data\data\pda\back_face\pristine\pda_pristine.rvmat", "INT_PDA_Data\data\pda\glass\pristine\pda_pristine.rvmat"}},
						{0.7, {"INT_PDA_Data\data\pda\corpus\pristine\pda_pristine.rvmat", "INT_PDA_Data\data\pda\back_face\pristine\pda_pristine.rvmat", "INT_PDA_Data\data\pda\glass\pristine\pda_pristine.rvmat"}},
						{0.5, {"INT_PDA_Data\data\pda\corpus\pristine\pda_pristine.rvmat", "INT_PDA_Data\data\pda\back_face\pristine\pda_pristine.rvmat", "INT_PDA_Data\data\pda\glass\pristine\pda_pristine.rvmat"}},
						{0.3, {"INT_PDA_Data\data\pda\corpus\pristine\pda_pristine.rvmat", "INT_PDA_Data\data\pda\back_face\pristine\pda_pristine.rvmat", "INT_PDA_Data\data\pda\glass\pristine\pda_pristine.rvmat"}},
						{0, {"INT_PDA_Data\data\pda\corpus\ruined\pda_ruined.rvmat", "INT_PDA_Data\data\pda\back_face\ruined\pda_ruined.rvmat", "INT_PDA_Data\data\pda\glass\ruined\pda_ruined.rvmat"}}
					};
				};
			};
		};
	};

	class INT_PDA: INT_PDA_Base
	{
		scope = 2;
	};

	class INT_PDA_Module_Base: Inventory_Base
	{
		scope = 0;
		displayName = "$STR_INT_PDA_ITEM_MODULE";
		descriptionShort = "$STR_INT_PDA_ITEM_MODULE_DESC";
		model = "INT_PDA_Data\data\module\int_pda_module.p3d";
		rotationFlags = 2;
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"INT_PDA_Data\data\module\int_pda_module_co.paa"};
		weight = 20;
		itemSize[] = {1, 1};
		soundImpactType = "plastic";
	};

	class INT_PDA_Module_GPS: INT_PDA_Module_Base
	{
		scope = 2;
		displayName = "$STR_INT_PDA_ITEM_GPS";
		descriptionShort = "$STR_INT_PDA_ITEM_GPS_DESC";
		inventorySlot[] = {"INT_PDA_Module_GPS"};
		hiddenSelectionsTextures[] = {"INT_PDA_Data\data\module\int_pda_module_gps_co.paa"};
	};
	class INT_PDA_Module_MAP: INT_PDA_Module_Base
	{
		scope = 2;
		displayName = "$STR_INT_PDA_ITEM_MAP";
		descriptionShort = "$STR_INT_PDA_ITEM_MAP_DESC";
		inventorySlot[] = {"INT_PDA_Module_MAP"};
		hiddenSelectionsTextures[] = {"INT_PDA_Data\data\module\int_pda_module_map_co.paa"};
	};
	class INT_PDA_Module_Chat: INT_PDA_Module_Base
	{
		scope = 2;
		displayName = "$STR_INT_PDA_ITEM_CHAT";
		descriptionShort = "$STR_INT_PDA_ITEM_CHAT_DESC";
		inventorySlot[] = {"INT_PDA_Module_Chat"};
		hiddenSelectionsTextures[] = {"INT_PDA_Data\data\module\int_pda_module_chat_co.paa"};
	};
	class INT_PDA_Module_GP: INT_PDA_Module_Base
	{
		scope = 2;
		displayName = "$STR_INT_PDA_ITEM_GP";
		descriptionShort = "$STR_INT_PDA_ITEM_GP_DESC";
		inventorySlot[] = {"INT_PDA_Module_GP"};
		hiddenSelectionsTextures[] = {"INT_PDA_Data\data\module\int_pda_module_gp_co.paa"};
	};
};
