// INT Banking - client part (loaded with -mod on the clients AND on the server).
// GUI, ATM objects (models + textures), the open action, the shared protocol.
//
// ATM class names of the old MF Banking / SM mapping mods are kept 1:1 (objects placed on the servers
// with DayZ Editor keep spawning): MF_BankObjectBase, MF_BankObjectTest, MF_BankObject_Test,
// DC_BankingATM*, DC_BankingLocker, sm_atm_DayZilla, sm_atm_DayZavr. The INT_* classes are the same
// objects with scope 2 (visible in admin tools and editors).
class CfgPatches
{
	class INT_Banking_CL
	{
		units[] = {"MF_BankObjectTest", "MF_BankObject_Test", "DC_BankingATM", "DC_BankingATMGreen", "DC_BankingATMBlue", "DC_BankingATMRed", "DC_BankingATMOrange", "DC_BankingATMYellow", "DC_BankingATMPurple", "DC_BankingATMDarkGreen", "DC_BankingATMDarkBlue", "DC_BankingATMWhite", "DC_BankingATMGray", "DC_BankingLocker", "sm_atm_DayZilla", "sm_atm_DayZavr", "BankingATM", "BankingATMGreen", "BankingATMBlue", "BankingATMRed", "BankingATMOrange", "BankingATMYellow", "BankingATMPurple", "BankingATMDarkGreen", "BankingATMDarkBlue", "BankingATMWhite", "BankingATMGray", "BankingLocker", "INT_BankingATM", "INT_BankingATMGreen", "INT_BankingATMBlue", "INT_BankingATMRed", "INT_BankingATMOrange", "INT_BankingATMYellow", "INT_BankingATMPurple", "INT_BankingATMDarkGreen", "INT_BankingATMDarkBlue", "INT_BankingATMWhite", "INT_BankingATMGray", "INT_BankingLocker", "INT_atm_DayZilla", "INT_atm_DayZavr"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Scripts", "DZ_Sounds_Effects", "INT_UIFramework"};
	};
	// Names of the replaced addons, so a mod that lists one of them in requiredAddons still loads.
	class MF_BankingClient
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data"};
	};
	class MF_BankingClient_Assets
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data"};
	};
	class MF_Banking_ATM
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data"};
	};
	class sm_mapping_atm
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data"};
	};
};

class CfgMods
{
	class INT_Banking_CL
	{
		type = "mod";
		dir = "INT_Banking_CL";
		name = "INT Banking";
		author = "Intenso";
		version = "1.0.2";
		dependencies[] = {"Game", "World", "Mission"};
		class defs
		{
			class gameScriptModule
			{
				value = "";
				files[] = {"INT_Banking_CL/scripts/3_Game"};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {"INT_Banking_CL/scripts/4_World"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"INT_Banking_CL/scripts/5_Mission"};
			};
		};
	};
};

class CfgModels
{
	class ATM
	{
		sections[] = {"everything"};
	};
};

class CfgVehicles
{
	class HouseNoDestruct;
	// Every placeable ATM derives DIRECTLY from HouseNoDestruct: VPP Object Builder lists only classes whose
	// immediate config parent is HouseNoDestruct (like INT_P2P_Stand). Do not put a shared parent between them.
	// The bank logic comes from the script classes of the same names (INT_Bank_ATM.c, all extend MF_BankObjectBase).
	// Class names of the old MF Banking / SM mapping mods are kept 1:1 for the existing mappings.
	class MF_BankObjectBase : HouseNoDestruct
	{
		scope = 0;
		storageCategory = 10;
		carveNavmesh = 1;
		physLayer = "item_large";
		displayName = "$STR_INT_BANK_ATM_NAME";
		descriptionShort = "$STR_INT_BANK_ATM_DESC";
	};
	class MF_BankObjectTest : HouseNoDestruct
	{
		scope = 2;
		storageCategory = 10;
		carveNavmesh = 1;
		physLayer = "item_large";
		displayName = "$STR_INT_BANK_ATM_TEST";
		descriptionShort = "$STR_INT_BANK_ATM_DESC";
		model = "\dz\gear\camping\spotlight.p3d";
	};
	class MF_BankObject_Test : HouseNoDestruct
	{
		scope = 2;
		storageCategory = 10;
		carveNavmesh = 1;
		physLayer = "item_large";
		displayName = "$STR_INT_BANK_ATM_TEST";
		descriptionShort = "$STR_INT_BANK_ATM_DESC";
		model = "\dz\gear\camping\spotlight.p3d";
	};
	class DC_BankingATM : HouseNoDestruct
	{
		scope = 2;
		storageCategory = 10;
		carveNavmesh = 1;
		physLayer = "item_large";
		displayName = "$STR_INT_BANK_ATM_BLACK";
		descriptionShort = "$STR_INT_BANK_ATM_DESC";
		model = "INT_Banking_CL\data\ATM.p3d";
		hiddenSelections[] = {"everything"};
		hiddenSelectionsTextures[] = {"INT_Banking_CL\data\ATMBlack_co.paa"};
	};
	class DC_BankingATMGreen : HouseNoDestruct
	{
		scope = 2;
		storageCategory = 10;
		carveNavmesh = 1;
		physLayer = "item_large";
		displayName = "$STR_INT_BANK_ATM_GREEN";
		descriptionShort = "$STR_INT_BANK_ATM_DESC";
		model = "INT_Banking_CL\data\ATM.p3d";
		hiddenSelections[] = {"everything"};
		hiddenSelectionsTextures[] = {"INT_Banking_CL\data\ATMGreen_co.paa"};
	};
	class DC_BankingATMBlue : HouseNoDestruct
	{
		scope = 2;
		storageCategory = 10;
		carveNavmesh = 1;
		physLayer = "item_large";
		displayName = "$STR_INT_BANK_ATM_BLUE";
		descriptionShort = "$STR_INT_BANK_ATM_DESC";
		model = "INT_Banking_CL\data\ATM.p3d";
		hiddenSelections[] = {"everything"};
		hiddenSelectionsTextures[] = {"INT_Banking_CL\data\ATMBlue_co.paa"};
	};
	class DC_BankingATMRed : HouseNoDestruct
	{
		scope = 2;
		storageCategory = 10;
		carveNavmesh = 1;
		physLayer = "item_large";
		displayName = "$STR_INT_BANK_ATM_RED";
		descriptionShort = "$STR_INT_BANK_ATM_DESC";
		model = "INT_Banking_CL\data\ATM.p3d";
		hiddenSelections[] = {"everything"};
		hiddenSelectionsTextures[] = {"INT_Banking_CL\data\ATMRed_co.paa"};
	};
	class DC_BankingATMOrange : HouseNoDestruct
	{
		scope = 2;
		storageCategory = 10;
		carveNavmesh = 1;
		physLayer = "item_large";
		displayName = "$STR_INT_BANK_ATM_ORANGE";
		descriptionShort = "$STR_INT_BANK_ATM_DESC";
		model = "INT_Banking_CL\data\ATM.p3d";
		hiddenSelections[] = {"everything"};
		hiddenSelectionsTextures[] = {"INT_Banking_CL\data\ATMOrange_co.paa"};
	};
	class DC_BankingATMYellow : HouseNoDestruct
	{
		scope = 2;
		storageCategory = 10;
		carveNavmesh = 1;
		physLayer = "item_large";
		displayName = "$STR_INT_BANK_ATM_YELLOW";
		descriptionShort = "$STR_INT_BANK_ATM_DESC";
		model = "INT_Banking_CL\data\ATM.p3d";
		hiddenSelections[] = {"everything"};
		hiddenSelectionsTextures[] = {"INT_Banking_CL\data\ATMYellow_co.paa"};
	};
	class DC_BankingATMPurple : HouseNoDestruct
	{
		scope = 2;
		storageCategory = 10;
		carveNavmesh = 1;
		physLayer = "item_large";
		displayName = "$STR_INT_BANK_ATM_PURPLE";
		descriptionShort = "$STR_INT_BANK_ATM_DESC";
		model = "INT_Banking_CL\data\ATM.p3d";
		hiddenSelections[] = {"everything"};
		hiddenSelectionsTextures[] = {"INT_Banking_CL\data\ATMPurple_co.paa"};
	};
	class DC_BankingATMDarkGreen : HouseNoDestruct
	{
		scope = 2;
		storageCategory = 10;
		carveNavmesh = 1;
		physLayer = "item_large";
		displayName = "$STR_INT_BANK_ATM_DARKGREEN";
		descriptionShort = "$STR_INT_BANK_ATM_DESC";
		model = "INT_Banking_CL\data\ATM.p3d";
		hiddenSelections[] = {"everything"};
		hiddenSelectionsTextures[] = {"INT_Banking_CL\data\ATMDarkGreen_co.paa"};
	};
	class DC_BankingATMDarkBlue : HouseNoDestruct
	{
		scope = 2;
		storageCategory = 10;
		carveNavmesh = 1;
		physLayer = "item_large";
		displayName = "$STR_INT_BANK_ATM_DARKBLUE";
		descriptionShort = "$STR_INT_BANK_ATM_DESC";
		model = "INT_Banking_CL\data\ATM.p3d";
		hiddenSelections[] = {"everything"};
		hiddenSelectionsTextures[] = {"INT_Banking_CL\data\ATMDarkBlue_co.paa"};
	};
	class DC_BankingATMWhite : HouseNoDestruct
	{
		scope = 2;
		storageCategory = 10;
		carveNavmesh = 1;
		physLayer = "item_large";
		displayName = "$STR_INT_BANK_ATM_WHITE";
		descriptionShort = "$STR_INT_BANK_ATM_DESC";
		model = "INT_Banking_CL\data\ATM.p3d";
		hiddenSelections[] = {"everything"};
		hiddenSelectionsTextures[] = {"INT_Banking_CL\data\ATMWhite_co.paa"};
	};
	class DC_BankingATMGray : HouseNoDestruct
	{
		scope = 2;
		storageCategory = 10;
		carveNavmesh = 1;
		physLayer = "item_large";
		displayName = "$STR_INT_BANK_ATM_GRAY";
		descriptionShort = "$STR_INT_BANK_ATM_DESC";
		model = "INT_Banking_CL\data\ATM.p3d";
		hiddenSelections[] = {"everything"};
		hiddenSelectionsTextures[] = {"INT_Banking_CL\data\ATMGray_co.paa"};
	};
	class BankingATM : HouseNoDestruct
	{
		scope = 2;
		storageCategory = 10;
		carveNavmesh = 1;
		physLayer = "item_large";
		displayName = "$STR_INT_BANK_ATM_BLACK";
		descriptionShort = "$STR_INT_BANK_ATM_DESC";
		model = "INT_Banking_CL\data\ATM.p3d";
		hiddenSelections[] = {"everything"};
		hiddenSelectionsTextures[] = {"INT_Banking_CL\data\ATMBlack_co.paa"};
	};
	class BankingATMGreen : HouseNoDestruct
	{
		scope = 2;
		storageCategory = 10;
		carveNavmesh = 1;
		physLayer = "item_large";
		displayName = "$STR_INT_BANK_ATM_GREEN";
		descriptionShort = "$STR_INT_BANK_ATM_DESC";
		model = "INT_Banking_CL\data\ATM.p3d";
		hiddenSelections[] = {"everything"};
		hiddenSelectionsTextures[] = {"INT_Banking_CL\data\ATMGreen_co.paa"};
	};
	class BankingATMBlue : HouseNoDestruct
	{
		scope = 2;
		storageCategory = 10;
		carveNavmesh = 1;
		physLayer = "item_large";
		displayName = "$STR_INT_BANK_ATM_BLUE";
		descriptionShort = "$STR_INT_BANK_ATM_DESC";
		model = "INT_Banking_CL\data\ATM.p3d";
		hiddenSelections[] = {"everything"};
		hiddenSelectionsTextures[] = {"INT_Banking_CL\data\ATMBlue_co.paa"};
	};
	class BankingATMRed : HouseNoDestruct
	{
		scope = 2;
		storageCategory = 10;
		carveNavmesh = 1;
		physLayer = "item_large";
		displayName = "$STR_INT_BANK_ATM_RED";
		descriptionShort = "$STR_INT_BANK_ATM_DESC";
		model = "INT_Banking_CL\data\ATM.p3d";
		hiddenSelections[] = {"everything"};
		hiddenSelectionsTextures[] = {"INT_Banking_CL\data\ATMRed_co.paa"};
	};
	class BankingATMOrange : HouseNoDestruct
	{
		scope = 2;
		storageCategory = 10;
		carveNavmesh = 1;
		physLayer = "item_large";
		displayName = "$STR_INT_BANK_ATM_ORANGE";
		descriptionShort = "$STR_INT_BANK_ATM_DESC";
		model = "INT_Banking_CL\data\ATM.p3d";
		hiddenSelections[] = {"everything"};
		hiddenSelectionsTextures[] = {"INT_Banking_CL\data\ATMOrange_co.paa"};
	};
	class BankingATMYellow : HouseNoDestruct
	{
		scope = 2;
		storageCategory = 10;
		carveNavmesh = 1;
		physLayer = "item_large";
		displayName = "$STR_INT_BANK_ATM_YELLOW";
		descriptionShort = "$STR_INT_BANK_ATM_DESC";
		model = "INT_Banking_CL\data\ATM.p3d";
		hiddenSelections[] = {"everything"};
		hiddenSelectionsTextures[] = {"INT_Banking_CL\data\ATMYellow_co.paa"};
	};
	class BankingATMPurple : HouseNoDestruct
	{
		scope = 2;
		storageCategory = 10;
		carveNavmesh = 1;
		physLayer = "item_large";
		displayName = "$STR_INT_BANK_ATM_PURPLE";
		descriptionShort = "$STR_INT_BANK_ATM_DESC";
		model = "INT_Banking_CL\data\ATM.p3d";
		hiddenSelections[] = {"everything"};
		hiddenSelectionsTextures[] = {"INT_Banking_CL\data\ATMPurple_co.paa"};
	};
	class BankingATMDarkGreen : HouseNoDestruct
	{
		scope = 2;
		storageCategory = 10;
		carveNavmesh = 1;
		physLayer = "item_large";
		displayName = "$STR_INT_BANK_ATM_DARKGREEN";
		descriptionShort = "$STR_INT_BANK_ATM_DESC";
		model = "INT_Banking_CL\data\ATM.p3d";
		hiddenSelections[] = {"everything"};
		hiddenSelectionsTextures[] = {"INT_Banking_CL\data\ATMDarkGreen_co.paa"};
	};
	class BankingATMDarkBlue : HouseNoDestruct
	{
		scope = 2;
		storageCategory = 10;
		carveNavmesh = 1;
		physLayer = "item_large";
		displayName = "$STR_INT_BANK_ATM_DARKBLUE";
		descriptionShort = "$STR_INT_BANK_ATM_DESC";
		model = "INT_Banking_CL\data\ATM.p3d";
		hiddenSelections[] = {"everything"};
		hiddenSelectionsTextures[] = {"INT_Banking_CL\data\ATMDarkBlue_co.paa"};
	};
	class BankingATMWhite : HouseNoDestruct
	{
		scope = 2;
		storageCategory = 10;
		carveNavmesh = 1;
		physLayer = "item_large";
		displayName = "$STR_INT_BANK_ATM_WHITE";
		descriptionShort = "$STR_INT_BANK_ATM_DESC";
		model = "INT_Banking_CL\data\ATM.p3d";
		hiddenSelections[] = {"everything"};
		hiddenSelectionsTextures[] = {"INT_Banking_CL\data\ATMWhite_co.paa"};
	};
	class BankingATMGray : HouseNoDestruct
	{
		scope = 2;
		storageCategory = 10;
		carveNavmesh = 1;
		physLayer = "item_large";
		displayName = "$STR_INT_BANK_ATM_GRAY";
		descriptionShort = "$STR_INT_BANK_ATM_DESC";
		model = "INT_Banking_CL\data\ATM.p3d";
		hiddenSelections[] = {"everything"};
		hiddenSelectionsTextures[] = {"INT_Banking_CL\data\ATMGray_co.paa"};
	};
	class INT_BankingATM : HouseNoDestruct
	{
		scope = 2;
		storageCategory = 10;
		carveNavmesh = 1;
		physLayer = "item_large";
		displayName = "$STR_INT_BANK_ATM_BLACK";
		descriptionShort = "$STR_INT_BANK_ATM_DESC";
		model = "INT_Banking_CL\data\ATM.p3d";
		hiddenSelections[] = {"everything"};
		hiddenSelectionsTextures[] = {"INT_Banking_CL\data\ATMBlack_co.paa"};
	};
	class INT_BankingATMGreen : HouseNoDestruct
	{
		scope = 2;
		storageCategory = 10;
		carveNavmesh = 1;
		physLayer = "item_large";
		displayName = "$STR_INT_BANK_ATM_GREEN";
		descriptionShort = "$STR_INT_BANK_ATM_DESC";
		model = "INT_Banking_CL\data\ATM.p3d";
		hiddenSelections[] = {"everything"};
		hiddenSelectionsTextures[] = {"INT_Banking_CL\data\ATMGreen_co.paa"};
	};
	class INT_BankingATMBlue : HouseNoDestruct
	{
		scope = 2;
		storageCategory = 10;
		carveNavmesh = 1;
		physLayer = "item_large";
		displayName = "$STR_INT_BANK_ATM_BLUE";
		descriptionShort = "$STR_INT_BANK_ATM_DESC";
		model = "INT_Banking_CL\data\ATM.p3d";
		hiddenSelections[] = {"everything"};
		hiddenSelectionsTextures[] = {"INT_Banking_CL\data\ATMBlue_co.paa"};
	};
	class INT_BankingATMRed : HouseNoDestruct
	{
		scope = 2;
		storageCategory = 10;
		carveNavmesh = 1;
		physLayer = "item_large";
		displayName = "$STR_INT_BANK_ATM_RED";
		descriptionShort = "$STR_INT_BANK_ATM_DESC";
		model = "INT_Banking_CL\data\ATM.p3d";
		hiddenSelections[] = {"everything"};
		hiddenSelectionsTextures[] = {"INT_Banking_CL\data\ATMRed_co.paa"};
	};
	class INT_BankingATMOrange : HouseNoDestruct
	{
		scope = 2;
		storageCategory = 10;
		carveNavmesh = 1;
		physLayer = "item_large";
		displayName = "$STR_INT_BANK_ATM_ORANGE";
		descriptionShort = "$STR_INT_BANK_ATM_DESC";
		model = "INT_Banking_CL\data\ATM.p3d";
		hiddenSelections[] = {"everything"};
		hiddenSelectionsTextures[] = {"INT_Banking_CL\data\ATMOrange_co.paa"};
	};
	class INT_BankingATMYellow : HouseNoDestruct
	{
		scope = 2;
		storageCategory = 10;
		carveNavmesh = 1;
		physLayer = "item_large";
		displayName = "$STR_INT_BANK_ATM_YELLOW";
		descriptionShort = "$STR_INT_BANK_ATM_DESC";
		model = "INT_Banking_CL\data\ATM.p3d";
		hiddenSelections[] = {"everything"};
		hiddenSelectionsTextures[] = {"INT_Banking_CL\data\ATMYellow_co.paa"};
	};
	class INT_BankingATMPurple : HouseNoDestruct
	{
		scope = 2;
		storageCategory = 10;
		carveNavmesh = 1;
		physLayer = "item_large";
		displayName = "$STR_INT_BANK_ATM_PURPLE";
		descriptionShort = "$STR_INT_BANK_ATM_DESC";
		model = "INT_Banking_CL\data\ATM.p3d";
		hiddenSelections[] = {"everything"};
		hiddenSelectionsTextures[] = {"INT_Banking_CL\data\ATMPurple_co.paa"};
	};
	class INT_BankingATMDarkGreen : HouseNoDestruct
	{
		scope = 2;
		storageCategory = 10;
		carveNavmesh = 1;
		physLayer = "item_large";
		displayName = "$STR_INT_BANK_ATM_DARKGREEN";
		descriptionShort = "$STR_INT_BANK_ATM_DESC";
		model = "INT_Banking_CL\data\ATM.p3d";
		hiddenSelections[] = {"everything"};
		hiddenSelectionsTextures[] = {"INT_Banking_CL\data\ATMDarkGreen_co.paa"};
	};
	class INT_BankingATMDarkBlue : HouseNoDestruct
	{
		scope = 2;
		storageCategory = 10;
		carveNavmesh = 1;
		physLayer = "item_large";
		displayName = "$STR_INT_BANK_ATM_DARKBLUE";
		descriptionShort = "$STR_INT_BANK_ATM_DESC";
		model = "INT_Banking_CL\data\ATM.p3d";
		hiddenSelections[] = {"everything"};
		hiddenSelectionsTextures[] = {"INT_Banking_CL\data\ATMDarkBlue_co.paa"};
	};
	class INT_BankingATMWhite : HouseNoDestruct
	{
		scope = 2;
		storageCategory = 10;
		carveNavmesh = 1;
		physLayer = "item_large";
		displayName = "$STR_INT_BANK_ATM_WHITE";
		descriptionShort = "$STR_INT_BANK_ATM_DESC";
		model = "INT_Banking_CL\data\ATM.p3d";
		hiddenSelections[] = {"everything"};
		hiddenSelectionsTextures[] = {"INT_Banking_CL\data\ATMWhite_co.paa"};
	};
	class INT_BankingATMGray : HouseNoDestruct
	{
		scope = 2;
		storageCategory = 10;
		carveNavmesh = 1;
		physLayer = "item_large";
		displayName = "$STR_INT_BANK_ATM_GRAY";
		descriptionShort = "$STR_INT_BANK_ATM_DESC";
		model = "INT_Banking_CL\data\ATM.p3d";
		hiddenSelections[] = {"everything"};
		hiddenSelectionsTextures[] = {"INT_Banking_CL\data\ATMGray_co.paa"};
	};
	class DC_BankingLocker : HouseNoDestruct
	{
		scope = 2;
		storageCategory = 10;
		carveNavmesh = 1;
		physLayer = "item_large";
		displayName = "$STR_INT_BANK_ATM_LOCKER";
		descriptionShort = "$STR_INT_BANK_ATM_DESC";
		model = "DZ\structures\furniture\cases\locker\locker_closed_v3.p3d";
		hiddenSelections[] = {"everything"};
	};
	class BankingLocker : HouseNoDestruct
	{
		scope = 2;
		storageCategory = 10;
		carveNavmesh = 1;
		physLayer = "item_large";
		displayName = "$STR_INT_BANK_ATM_LOCKER";
		descriptionShort = "$STR_INT_BANK_ATM_DESC";
		model = "DZ\structures\furniture\cases\locker\locker_closed_v3.p3d";
		hiddenSelections[] = {"everything"};
	};
	class INT_BankingLocker : HouseNoDestruct
	{
		scope = 2;
		storageCategory = 10;
		carveNavmesh = 1;
		physLayer = "item_large";
		displayName = "$STR_INT_BANK_ATM_LOCKER";
		descriptionShort = "$STR_INT_BANK_ATM_DESC";
		model = "DZ\structures\furniture\cases\locker\locker_closed_v3.p3d";
		hiddenSelections[] = {"everything"};
	};
	class sm_atm_DayZilla : HouseNoDestruct
	{
		scope = 2;
		storageCategory = 10;
		carveNavmesh = 1;
		physLayer = "item_large";
		displayName = "$STR_INT_BANK_ATM_KIOSK1";
		descriptionShort = "$STR_INT_BANK_ATM_DESC";
		model = "\INT_Banking_CL\atm\atm.p3d";
	};
	class INT_atm_DayZilla : HouseNoDestruct
	{
		scope = 2;
		storageCategory = 10;
		carveNavmesh = 1;
		physLayer = "item_large";
		displayName = "$STR_INT_BANK_ATM_KIOSK1";
		descriptionShort = "$STR_INT_BANK_ATM_DESC";
		model = "\INT_Banking_CL\atm\atm.p3d";
	};
	class sm_atm_DayZavr : HouseNoDestruct
	{
		scope = 2;
		storageCategory = 10;
		carveNavmesh = 1;
		physLayer = "item_large";
		displayName = "$STR_INT_BANK_ATM_KIOSK2";
		descriptionShort = "$STR_INT_BANK_ATM_DESC";
		model = "\INT_Banking_CL\atm\atm_02.p3d";
	};
	class INT_atm_DayZavr : HouseNoDestruct
	{
		scope = 2;
		storageCategory = 10;
		carveNavmesh = 1;
		physLayer = "item_large";
		displayName = "$STR_INT_BANK_ATM_KIOSK2";
		descriptionShort = "$STR_INT_BANK_ATM_DESC";
		model = "\INT_Banking_CL\atm\atm_02.p3d";
	};
};

class CfgSoundShaders
{
	class INT_Bank_Base_SoundShader
	{
		volume = 0.5;
		range = 5;
		limitation = 0;
	};
	class INT_Bank_Key_SoundShader : INT_Bank_Base_SoundShader
	{
		samples[] = {{"INT_Banking_CL\data\sound\int_bank_key", 1}};
		volume = 0.42;
	};
	class INT_Bank_CashOut_SoundShader : INT_Bank_Base_SoundShader
	{
		samples[] = {{"INT_Banking_CL\data\sound\int_bank_cash_out", 1}};
		volume = 0.6;
	};
	class INT_Bank_CashIn_SoundShader : INT_Bank_Base_SoundShader
	{
		samples[] = {{"INT_Banking_CL\data\sound\int_bank_cash_in", 1}};
		volume = 0.6;
	};
	class INT_Bank_Card_SoundShader : INT_Bank_Base_SoundShader
	{
		samples[] = {{"INT_Banking_CL\data\sound\int_bank_card", 1}};
	};
	class INT_Bank_Notify_SoundShader : INT_Bank_Base_SoundShader
	{
		samples[] = {{"INT_Banking_CL\data\sound\int_bank_notify", 1}};
		volume = 0.45;
	};
	class INT_Bank_Click_SoundShader : INT_Bank_Base_SoundShader
	{
		samples[] = {{"INT_Banking_CL\data\sound\int_bank_ui_click", 1}};
	};
	class INT_Bank_Tab_SoundShader : INT_Bank_Base_SoundShader
	{
		samples[] = {{"INT_Banking_CL\data\sound\int_bank_ui_tab", 1}};
	};
	class INT_Bank_Select_SoundShader : INT_Bank_Base_SoundShader
	{
		samples[] = {{"INT_Banking_CL\data\sound\int_bank_ui_select", 1}};
	};
	class INT_Bank_Confirm_SoundShader : INT_Bank_Base_SoundShader
	{
		samples[] = {{"INT_Banking_CL\data\sound\int_bank_ui_confirm", 1}};
	};
	class INT_Bank_Error_SoundShader : INT_Bank_Base_SoundShader
	{
		samples[] = {{"INT_Banking_CL\data\sound\int_bank_ui_error", 1}};
	};
	class INT_Bank_Open_SoundShader : INT_Bank_Base_SoundShader
	{
		samples[] = {{"INT_Banking_CL\data\sound\int_bank_ui_open", 1}};
	};
	class INT_Bank_Close_SoundShader : INT_Bank_Base_SoundShader
	{
		samples[] = {{"INT_Banking_CL\data\sound\int_bank_ui_close", 1}};
	};
};

class CfgSoundSets
{
	class INT_Bank_Base_SoundSet
	{
		volumeFactor = 1.0;
		frequencyFactor = 1.0;
		spatial = 0;
		doppler = 0;
		loop = 0;
	};
	class INT_Bank_Key_SoundSet : INT_Bank_Base_SoundSet
	{
		soundShaders[] = {"INT_Bank_Key_SoundShader"};
	};
	class INT_Bank_CashOut_SoundSet : INT_Bank_Base_SoundSet
	{
		soundShaders[] = {"INT_Bank_CashOut_SoundShader"};
	};
	class INT_Bank_CashIn_SoundSet : INT_Bank_Base_SoundSet
	{
		soundShaders[] = {"INT_Bank_CashIn_SoundShader"};
	};
	class INT_Bank_Card_SoundSet : INT_Bank_Base_SoundSet
	{
		soundShaders[] = {"INT_Bank_Card_SoundShader"};
	};
	class INT_Bank_Notify_SoundSet : INT_Bank_Base_SoundSet
	{
		soundShaders[] = {"INT_Bank_Notify_SoundShader"};
	};
	class INT_Bank_Click_SoundSet : INT_Bank_Base_SoundSet
	{
		soundShaders[] = {"INT_Bank_Click_SoundShader"};
	};
	class INT_Bank_Tab_SoundSet : INT_Bank_Base_SoundSet
	{
		soundShaders[] = {"INT_Bank_Tab_SoundShader"};
	};
	class INT_Bank_Select_SoundSet : INT_Bank_Base_SoundSet
	{
		soundShaders[] = {"INT_Bank_Select_SoundShader"};
	};
	class INT_Bank_Confirm_SoundSet : INT_Bank_Base_SoundSet
	{
		soundShaders[] = {"INT_Bank_Confirm_SoundShader"};
	};
	class INT_Bank_Error_SoundSet : INT_Bank_Base_SoundSet
	{
		soundShaders[] = {"INT_Bank_Error_SoundShader"};
	};
	class INT_Bank_Open_SoundSet : INT_Bank_Base_SoundSet
	{
		soundShaders[] = {"INT_Bank_Open_SoundShader"};
	};
	class INT_Bank_Close_SoundSet : INT_Bank_Base_SoundSet
	{
		soundShaders[] = {"INT_Bank_Close_SoundShader"};
	};
};
