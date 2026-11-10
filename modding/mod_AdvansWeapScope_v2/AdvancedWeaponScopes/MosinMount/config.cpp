class CfgPatches
{
	class AD_MosinMount
	{
		units[] = {"AD_MosinMount","AD_Pilad_MosinMount","AD_LeupoldMk4_MosinMount","AD_DHF5_MosinMount","AD_PVS4_MosinMount","AD_OKP7_MosinMount","AD_RMR_LM_MosinMount","AD_RMR_TM_MosinMount","AD_ACOG_MosinMount","AD_ACOG_RMR_MosinMount","AD_MRS_MosinMount","AD_XPS34_MosinMount","AD_XPS34_HHS_MosinMount","AD_G33_MosinMount","AD_NFATACR_MosinMount","AD_NFATACR_1_8_MosinMount","AD_SpecterDR_MosinMount"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Weapons_Optics","AD_Pilad","AD_DHF5","AD_PVS4","AD_OKP7","AD_RMR","AD_ACOG","AD_MRS","AD_NFATACR","AD_NFATACR_1_8","AD_SpecterDR","AD_LeupoldMk4","AD_XPS34"};
	};
};
class cfgVehicles
{
	class Inventory_Base;
	class AD_MosinMount: Inventory_Base
	{
		scope = 2;
		displayName = "$STR_AD_MosinMount0";
		descriptionShort = "$STR_AD_MosinMount1";
		model = "AdvancedWeaponScopes\MosinMount\mosinmount.p3d";
		rotationFlags = 4;
		reversed = 0;
		weight = 175;
		itemSize[] = {2,1};
		inventorySlot[] = {"weaponOpticsMosin"};
		repairableWithKits[] = {1};
		repairCosts[] = {25.0};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 50;
					healthLevels[] = {{1.0,{"AdvancedWeaponScopes\MosinMount\data\mosinmount.rvmat"}},{0.7,{"AdvancedWeaponScopes\MosinMount\data\mosinmount.rvmat"}},{0.5,{"AdvancedWeaponScopes\MosinMount\data\mosinmount_damaged.rvmat"}},{0.3,{"AdvancedWeaponScopes\MosinMount\data\mosinmount_damaged.rvmat"}},{0.0,{"AdvancedWeaponScopes\MosinMount\data\mosinmount_destruct.rvmat"}}};
				};
			};
		};
	};
	class AD_Pilad_Base;
	class AD_Pilad_MosinMount: AD_Pilad_Base
	{
		scope = 2;
		displayName = "$STR_AD_Pilad8";
		descriptionShort = "$STR_AD_Pilad9";
		model = "AdvancedWeaponScopes\MosinMount\Pilad_mosinmount.p3d";
		inventorySlot[] = {"weaponOpticsMosin"};
		weight = 1075;
		itemSize[] = {4,2};
	};
	class AD_LeupoldMk4_Base;
	class AD_LeupoldMk4_MosinMount: AD_LeupoldMk4_Base
	{
		scope = 2;
		displayName = "$STR_AD_LeupoldMk4_4";
		descriptionShort = "$STR_AD_LeupoldMk4_5";
		model = "\AdvancedWeaponScopes\MosinMount\LeupoldMk4_mosinmount.p3d";
		inventorySlot[] = {"weaponOpticsMosin"};
		weight = 875;
		itemSize[] = {4,2};
	};
	class AD_DHF5_Base;
	class AD_DHF5_MosinMount: AD_DHF5_Base
	{
		scope = 2;
		displayName = "$STR_AD_DHF58";
		descriptionShort = "$STR_AD_DHF59";
		model = "AdvancedWeaponScopes\MosinMount\dhf5_mosinmount.p3d";
		inventorySlot[] = {"weaponOpticsMosin"};
		weight = 1000;
		itemSize[] = {5,2};
	};
	class AD_PVS4_Base;
	class AD_PVS4_MosinMount: AD_PVS4_Base
	{
		scope = 2;
		displayName = "$STR_AD_PVS48";
		descriptionShort = "$STR_AD_PVS49";
		model = "AdvancedWeaponScopes\MosinMount\PVS4_mosinmount.p3d";
		inventorySlot[] = {"weaponOpticsMosin"};
		weight = 1975;
		itemSize[] = {4,4};
	};
	class AD_OKP7_Base;
	class AD_OKP7_MosinMount: AD_OKP7_Base
	{
		scope = 2;
		displayName = "$STR_AD_OKP76";
		descriptionShort = "$STR_AD_OKP77";
		model = "AdvancedWeaponScopes\MosinMount\OKP7_mosinmount.p3d";
		inventorySlot[] = {"weaponOpticsMosin"};
		weight = 400;
		itemSize[] = {2,2};
	};
	class AD_RMR_LM_Base;
	class AD_RMR_LM_MosinMount: AD_RMR_LM_Base
	{
		scope = 2;
		displayName = "$STR_AD_RMR7";
		descriptionShort = "$STR_AD_RMR8";
		model = "AdvancedWeaponScopes\MosinMount\RMRLM_mosinmount.p3d";
		inventorySlot[] = {"weaponOpticsMosin"};
		weight = 400;
		itemSize[] = {2,2};
	};
	class AD_RMR_TM_Base;
	class AD_RMR_TM_MosinMount: AD_RMR_TM_Base
	{
		scope = 2;
		displayName = "$STR_AD_RMR6";
		descriptionShort = "$STR_AD_RMR1";
		model = "AdvancedWeaponScopes\MosinMount\RMRTM_mosinmount.p3d";
		inventorySlot[] = {"weaponOpticsMosin"};
		weight = 420;
		itemSize[] = {2,2};
	};
	class AD_ACOG_Base;
	class AD_ACOG_MosinMount: AD_ACOG_Base
	{
		scope = 2;
		displayName = "$STR_AD_ACOG6";
		descriptionShort = "$STR_AD_ACOG2";
		model = "AdvancedWeaponScopes\MosinMount\ACOG_mosinmount.p3d";
		inventorySlot[] = {"weaponOpticsMosin"};
		weight = 800;
		itemSize[] = {2,2};
	};
	class AD_ACOG_RMR_Base;
	class AD_ACOG_RMR_MosinMount: AD_ACOG_RMR_Base
	{
		scope = 2;
		displayName = "$STR_AD_ACOG5";
		descriptionShort = "$STR_AD_ACOG2";
		model = "AdvancedWeaponScopes\MosinMount\ACOGWRMR_mosinmount.p3d";
		inventorySlot[] = {"weaponOpticsMosin"};
		weight = 850;
		itemSize[] = {2,2};
	};
	class AD_MRS_Base;
	class AD_MRS_MosinMount: AD_MRS_Base
	{
		scope = 2;
		displayName = "$STR_AD_MRS2";
		descriptionShort = "$STR_AD_MRS3";
		model = "AdvancedWeaponScopes\MosinMount\MRS_mosinmount.p3d";
		inventorySlot[] = {"weaponOpticsMosin"};
		weight = 400;
		itemSize[] = {2,2};
	};
	class AD_XPS34_Base;
	class AD_XPS34_MosinMount: AD_XPS34_Base
	{
		scope = 2;
		displayName = "$STR_AD_XPS344";
		descriptionShort = "$STR_AD_XPS345";
		model = "AdvancedWeaponScopes\MosinMount\xps34_mosinmount.p3d";
		inventorySlot[] = {"weaponOpticsMosin"};
		weight = 325;
		itemSize[] = {2,2};
	};
	class AD_XPS34_HHS_Base;
	class AD_XPS34_HHS_MosinMount: AD_XPS34_HHS_Base
	{
		scope = 2;
		displayName = "$STR_AD_XPS34HHS4";
		descriptionShort = "$STR_AD_XPS34HHS5";
		model = "AdvancedWeaponScopes\MosinMount\xps34_g33_mosinmount.p3d";
		inventorySlot[] = {"weaponOpticsMosin"};
		weight = 425;
		itemSize[] = {2,2};
	};
	class AD_G33_Base;
	class AD_G33_MosinMount: AD_G33_Base
	{
		scope = 2;
		displayName = "$STR_AD_G334";
		descriptionShort = "$STR_AD_G335";
		model = "AdvancedWeaponScopes\MosinMount\g33_mosinmount.p3d";
		inventorySlot[] = {"weaponOpticsMosin"};
		weight = 325;
		itemSize[] = {2,2};
	};
	class AD_NFATACR_Base;
	class AD_NFATACR_MosinMount: AD_NFATACR_Base
	{
		scope = 2;
		displayName = "$STR_AD_NFATACR2";
		descriptionShort = "$STR_AD_NFATACR1";
		model = "AdvancedWeaponScopes\MosinMount\NFATACR_mosinmount.p3d";
		inventorySlot[] = {"weaponOpticsMosin"};
		weight = 1300;
		itemSize[] = {5,2};
	};
	class AD_NFATACR_1_8_Base;
	class AD_NFATACR_1_8_MosinMount: AD_NFATACR_1_8_Base
	{
		scope = 2;
		displayName = "$STR_AD_NFATACR_1_82";
		descriptionShort = "$STR_AD_NFATACR1";
		model = "AdvancedWeaponScopes\MosinMount\NFATACR_1_8_mosinmount.p3d";
		inventorySlot[] = {"weaponOpticsMosin"};
		weight = 800;
		itemSize[] = {2,2};
	};
	class AD_SpecterDR_Base;
	class AD_SpecterDR_MosinMount: AD_SpecterDR_Base
	{
		scope = 2;
		displayName = "$STR_AD_SpecterDR6";
		descriptionShort = "$STR_AD_SpecterDR7";
		model = "AdvancedWeaponScopes\MosinMount\SpecterDR_mosinmount.p3d";
		inventorySlot[] = {"weaponOpticsMosin"};
		weight = 700;
		itemSize[] = {2,2};
	};
};
