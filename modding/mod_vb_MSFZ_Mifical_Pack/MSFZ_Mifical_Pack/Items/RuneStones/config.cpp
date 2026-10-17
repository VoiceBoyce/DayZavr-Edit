class CfgPatches
{
	class MSFZ_RuneStone
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Gear_Medical"};
	};
};
class CfgVehicles
{
	class Inventory_Base;
	class Box_Base;
	class MSFZ_RuneStone_Base: Inventory_Base
	{
		scope = 0;
	};
	class MSFZ_RuneStone_Fire: MSFZ_RuneStone_Base
	{
		scope = 2;
		displayName = "$STR_RuneStone_Fire";
		descriptionShort = "$STR_RuneStone_Fire1";
		model = "MSFZ_Mifical_Pack\Items\RuneStones\MSFZ_RuneStone_Fire.p3d";
		animClass = "NoFireClass";
		weight = 300;
		itemSize[] = {2,2};
		canBeSplit = 1;
		inventorySlot[] = {"Runes","Material_FPole_Runes"};
		isMeleeWeapon = 1;
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Items\RuneStones\data\MSFZ_RuneStone_Fire.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Items\RuneStones\data\MSFZ_RuneStone_Fire.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 200;
					healthLevels[] = {{1,{"MSFZ_Mifical_Pack\Items\RuneStones\data\MSFZ_RuneStone_Fire.rvmat"}},{0.7,{"MSFZ_Mifical_Pack\Items\RuneStones\data\MSFZ_RuneStone_Fire.rvmat"}},{0.5,{"MSFZ_Mifical_Pack\Items\RuneStones\data\MSFZ_RuneStone_Fire_damage.rvmat"}},{0.3,{"MSFZ_Mifical_Pack\Items\RuneStones\data\MSFZ_RuneStone_Fire_damage.rvmat"}},{0,{"MSFZ_Mifical_Pack\Items\RuneStones\data\MSFZ_RuneStone_Fire_destruct.rvmat"}}};
				};
			};
		};
	};
	class MSFZ_RuneStone_FireSmall: MSFZ_RuneStone_Base
	{
		scope = 2;
		displayName = "$STR_RuneStone_FireSmall";
		descriptionShort = "$STR_RuneStone_FireSmall1";
		model = "MSFZ_Mifical_Pack\Items\RuneStones\MSFZ_RuneStone_FireSmall.p3d";
		animClass = "NoFireClass";
		weight = 150;
		itemSize[] = {2,2};
		canBeSplit = 1;
		inventorySlot[] = {"Runes","Material_FPole_Runes"};
		isMeleeWeapon = 1;
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Items\RuneStones\data\MSFZ_RuneStone_Fire_Small.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Items\RuneStones\data\MSFZ_RuneStone_FireSmall.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 200;
					healthLevels[] = {{1,{"MSFZ_Mifical_Pack\Items\RuneStones\data\MSFZ_RuneStone_FireSmall.rvmat"}},{0.7,{"MSFZ_Mifical_Pack\Items\RuneStones\data\MSFZ_RuneStone_FireSmall.rvmat"}},{0.5,{"MSFZ_Mifical_Pack\Items\RuneStones\data\MSFZ_RuneStone_FireSmall_damage.rvmat"}},{0.3,{"MSFZ_Mifical_Pack\Items\RuneStones\data\MSFZ_RuneStone_FireSmall_damage.rvmat"}},{0,{"MSFZ_Mifical_Pack\Items\RuneStones\data\MSFZ_RuneStone_FireSmall_destruct.rvmat"}}};
				};
			};
		};
	};
	class MSFZ_RuneStone_Ice: MSFZ_RuneStone_Base
	{
		scope = 2;
		displayName = "$STR_RuneStone_Ice";
		descriptionShort = "$STR_RuneStone_Ice1";
		model = "MSFZ_Mifical_Pack\Items\RuneStones\MSFZ_RuneStone_Ice.p3d";
		animClass = "NoFireClass";
		weight = 300;
		itemSize[] = {2,2};
		canBeSplit = 1;
		inventorySlot[] = {"Runes","Material_FPole_Runes"};
		isMeleeWeapon = 1;
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Items\RuneStones\data\MSFZ_RuneStone_Ice.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Items\RuneStones\data\MSFZ_RuneStone_Ice.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 200;
					healthLevels[] = {{1,{"MSFZ_Mifical_Pack\Items\RuneStones\data\MSFZ_RuneStone_Ice.rvmat"}},{0.7,{"MSFZ_Mifical_Pack\Items\RuneStones\data\MSFZ_RuneStone_Ice.rvmat"}},{0.5,{"MSFZ_Mifical_Pack\Items\RuneStones\data\MSFZ_RuneStone_Ice_damage.rvmat"}},{0.3,{"MSFZ_Mifical_Pack\Items\RuneStones\data\MSFZ_RuneStone_Ice_damage.rvmat"}},{0,{"MSFZ_Mifical_Pack\Items\RuneStones\data\MSFZ_RuneStone_Ice_destruct.rvmat"}}};
				};
			};
		};
	};
	class MSFZ_RuneStone_IceSmall: MSFZ_RuneStone_Base
	{
		scope = 2;
		displayName = "$STR_RuneStone_IceSmall";
		descriptionShort = "$STR_RuneStone_Ice1Small";
		model = "MSFZ_Mifical_Pack\Items\RuneStones\MSFZ_RuneStone_IceSmall.p3d";
		animClass = "NoFireClass";
		weight = 150;
		itemSize[] = {2,2};
		canBeSplit = 1;
		inventorySlot[] = {"Runes","Material_FPole_Runes"};
		isMeleeWeapon = 1;
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Items\RuneStones\data\MSFZ_RuneStone_Ice_small.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Items\RuneStones\data\MSFZ_RuneStone_IceSmall.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 200;
					healthLevels[] = {{1,{"MSFZ_Mifical_Pack\Items\RuneStones\data\MSFZ_RuneStone_IceSmall.rvmat"}},{0.7,{"MSFZ_Mifical_Pack\Items\RuneStones\data\MSFZ_RuneStone_IceSmall.rvmat"}},{0.5,{"MSFZ_Mifical_Pack\Items\RuneStones\data\MSFZ_RuneStone_IceSmall_damage.rvmat"}},{0.3,{"MSFZ_Mifical_Pack\Items\RuneStones\data\MSFZ_RuneStone_IceSmall_damage.rvmat"}},{0,{"MSFZ_Mifical_Pack\Items\RuneStones\data\MSFZ_RuneStone_IceSmall_destruct.rvmat"}}};
				};
			};
		};
	};
	class MSFZ_RuneStone_Storm: MSFZ_RuneStone_Base
	{
		scope = 2;
		displayName = "$STR_RuneStone_Storm";
		descriptionShort = "$STR_RuneStone_Storm1";
		model = "MSFZ_Mifical_Pack\Items\RuneStones\MSFZ_RuneStone_Storm.p3d";
		animClass = "NoFireClass";
		weight = 300;
		itemSize[] = {2,2};
		canBeSplit = 1;
		inventorySlot[] = {"Runes","Material_FPole_Runes"};
		isMeleeWeapon = 1;
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Items\RuneStones\data\MSFZ_RuneStone_Storm.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Items\RuneStones\data\MSFZ_RuneStone_Storm.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 200;
					healthLevels[] = {{1,{"MSFZ_Mifical_Pack\Items\RuneStones\data\MSFZ_RuneStone_Storm.rvmat"}},{0.7,{"MSFZ_Mifical_Pack\Items\RuneStones\data\MSFZ_RuneStone_Storm.rvmat"}},{0.5,{"MSFZ_Mifical_Pack\Items\RuneStones\data\MSFZ_RuneStone_Storm_damage.rvmat"}},{0.3,{"MSFZ_Mifical_Pack\Items\RuneStones\data\MSFZ_RuneStone_Storm_damage.rvmat"}},{0,{"MSFZ_Mifical_Pack\Items\RuneStones\data\MSFZ_RuneStone_Storm_destruct.rvmat"}}};
				};
			};
		};
	};
	class MSFZ_RuneStone_StormSmall: MSFZ_RuneStone_Base
	{
		scope = 2;
		displayName = "$STR_RuneStone_StormSmall";
		descriptionShort = "$STR_RuneStone_StormSmall1";
		model = "MSFZ_Mifical_Pack\Items\RuneStones\MSFZ_RuneStone_StormSmall.p3d";
		animClass = "NoFireClass";
		weight = 150;
		itemSize[] = {2,2};
		canBeSplit = 1;
		inventorySlot[] = {"Runes","Material_FPole_Runes"};
		isMeleeWeapon = 1;
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Items\RuneStones\data\MSFZ_RuneStone_Storm_small.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Items\RuneStones\data\MSFZ_RuneStone_StormSmall.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 200;
					healthLevels[] = {{1,{"MSFZ_Mifical_Pack\Items\RuneStones\data\MSFZ_RuneStone_StormSmall.rvmat"}},{0.7,{"MSFZ_Mifical_Pack\Items\RuneStones\data\MSFZ_RuneStone_StormSmall.rvmat"}},{0.5,{"MSFZ_Mifical_Pack\Items\RuneStones\data\MSFZ_RuneStone_StormSmall_damage.rvmat"}},{0.3,{"MSFZ_Mifical_Pack\Items\RuneStones\data\MSFZ_RuneStone_StormSmall_damage.rvmat"}},{0,{"MSFZ_Mifical_Pack\Items\RuneStones\data\MSFZ_RuneStone_StormSmall_destruct.rvmat"}}};
				};
			};
		};
	};
};
class CfgNonAIVehicles
{
	class ProxyAttachment;
	class ProxyMSFZ_RuneStone_Proxy: ProxyAttachment
	{
		scope = 2;
		inventorySlot[] = {"Runes","Material_FPole_Runes"};
		model = "MSFZ_Mifical_Pack\Items\RuneStones\proxy\MSFZ_RuneStone_Proxy.p3d";
	};
};
