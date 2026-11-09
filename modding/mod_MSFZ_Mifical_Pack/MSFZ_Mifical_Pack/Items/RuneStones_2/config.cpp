class CfgPatches
{
	class MSFZ_RuneStone2
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
	class MSFZ_RuneStone_Base;
	class MSFZ_RuneStone_Bereza: MSFZ_RuneStone_Base
	{
		scope = 2;
		displayName = "$STR_RuneStone_Bereza";
		descriptionShort = "$STR_RuneStone_Bereza1";
		model = "MSFZ_Mifical_Pack\Items\RuneStones_2\MSFZ_RuneStone_2.p3d";
		animClass = "NoFireClass";
		weight = 300;
		itemSize[] = {2,2};
		canBeSplit = 1;
		inventorySlot[] = {"Runes","Material_FPole_Runes"};
		isMeleeWeapon = 1;
		hiddenSelections[] = {"camo1"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_2.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Bereza.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 200;
					healthLevels[] = {{1,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Bereza.rvmat"}},{0.7,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Bereza.rvmat"}},{0.5,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_2_damage.rvmat"}},{0.3,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_2_damage.rvmat"}},{0,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_2_destruct.rvmat"}}};
				};
			};
		};
	};
	class MSFZ_RuneStone_Bog: MSFZ_RuneStone_Base
	{
		scope = 2;
		displayName = "$STR_RuneStone_Bog";
		descriptionShort = "$STR_RuneStone_Bog1";
		model = "MSFZ_Mifical_Pack\Items\RuneStones_2\MSFZ_RuneStone_2.p3d";
		animClass = "NoFireClass";
		weight = 300;
		itemSize[] = {2,2};
		canBeSplit = 1;
		inventorySlot[] = {"Runes","Material_FPole_Runes"};
		isMeleeWeapon = 1;
		hiddenSelections[] = {"camo1"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_2.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Bog.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 200;
					healthLevels[] = {{1,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Bog.rvmat"}},{0.7,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Bog.rvmat"}},{0.5,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_2_damage.rvmat"}},{0.3,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_2_damage.rvmat"}},{0,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_2_destruct.rvmat"}}};
				};
			};
		};
	};
	class MSFZ_RuneStone_Chelovek: MSFZ_RuneStone_Base
	{
		scope = 2;
		displayName = "$STR_RuneStone_Chelovek";
		descriptionShort = "$STR_RuneStone_Chelovek1";
		model = "MSFZ_Mifical_Pack\Items\RuneStones_2\MSFZ_RuneStone_2.p3d";
		animClass = "NoFireClass";
		weight = 300;
		itemSize[] = {2,2};
		canBeSplit = 1;
		inventorySlot[] = {"Runes","Material_FPole_Runes"};
		isMeleeWeapon = 1;
		hiddenSelections[] = {"camo1"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_2.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Chelovek.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 200;
					healthLevels[] = {{1,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Chelovek.rvmat"}},{0.7,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Chelovek.rvmat"}},{0.5,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_2_damage.rvmat"}},{0.3,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_2_damage.rvmat"}},{0,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_2_destruct.rvmat"}}};
				};
			};
		};
	};
	class MSFZ_RuneStone_Day: MSFZ_RuneStone_Base
	{
		scope = 2;
		displayName = "$STR_RuneStone_Day";
		descriptionShort = "$STR_RuneStone_Day1";
		model = "MSFZ_Mifical_Pack\Items\RuneStones_2\MSFZ_RuneStone_2.p3d";
		animClass = "NoFireClass";
		weight = 300;
		itemSize[] = {2,2};
		canBeSplit = 1;
		inventorySlot[] = {"Runes","Material_FPole_Runes"};
		isMeleeWeapon = 1;
		hiddenSelections[] = {"camo1"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_2.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Day.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 200;
					healthLevels[] = {{1,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Day.rvmat"}},{0.7,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Day.rvmat"}},{0.5,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_2_damage.rvmat"}},{0.3,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_2_damage.rvmat"}},{0,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_2_destruct.rvmat"}}};
				};
			};
		};
	};
	class MSFZ_RuneStone_KladesPamyati: MSFZ_RuneStone_Base
	{
		scope = 2;
		displayName = "$STR_RuneStone_KladesPamyati";
		descriptionShort = "$STR_RuneStone_KladesPamyati1";
		model = "MSFZ_Mifical_Pack\Items\RuneStones_2\MSFZ_RuneStone_2.p3d";
		animClass = "NoFireClass";
		weight = 300;
		itemSize[] = {2,2};
		canBeSplit = 1;
		inventorySlot[] = {"Runes","Material_FPole_Runes"};
		isMeleeWeapon = 1;
		hiddenSelections[] = {"camo1"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_2.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_KladesPamyati.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 200;
					healthLevels[] = {{1,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_KladesPamyati.rvmat"}},{0.7,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_KladesPamyati.rvmat"}},{0.5,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_2_damage.rvmat"}},{0.3,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_2_damage.rvmat"}},{0,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_2_destruct.rvmat"}}};
				};
			};
		};
	};
	class MSFZ_RuneStone_Los: MSFZ_RuneStone_Base
	{
		scope = 2;
		displayName = "$STR_RuneStone_Los";
		descriptionShort = "$STR_RuneStone_Los1";
		model = "MSFZ_Mifical_Pack\Items\RuneStones_2\MSFZ_RuneStone_2.p3d";
		animClass = "NoFireClass";
		weight = 300;
		itemSize[] = {2,2};
		canBeSplit = 1;
		inventorySlot[] = {"Runes","Material_FPole_Runes"};
		isMeleeWeapon = 1;
		hiddenSelections[] = {"camo1"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_2.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Los.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 200;
					healthLevels[] = {{1,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Los.rvmat"}},{0.7,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Los.rvmat"}},{0.5,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_2_damage.rvmat"}},{0.3,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_2_damage.rvmat"}},{0,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_2_destruct.rvmat"}}};
				};
			};
		};
	};
	class MSFZ_RuneStone_Nasledie: MSFZ_RuneStone_Base
	{
		scope = 2;
		displayName = "$STR_RuneStone_Nasledie";
		descriptionShort = "$STR_RuneStone_Nasledie1";
		model = "MSFZ_Mifical_Pack\Items\RuneStones_2\MSFZ_RuneStone_2.p3d";
		animClass = "NoFireClass";
		weight = 300;
		itemSize[] = {2,2};
		canBeSplit = 1;
		inventorySlot[] = {"Runes","Material_FPole_Runes"};
		isMeleeWeapon = 1;
		hiddenSelections[] = {"camo1"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_2.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Nasledie.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 200;
					healthLevels[] = {{1,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Nasledie.rvmat"}},{0.7,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Nasledie.rvmat"}},{0.5,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_2_damage.rvmat"}},{0.3,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_2_damage.rvmat"}},{0,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_2_destruct.rvmat"}}};
				};
			};
		};
	};
	class MSFZ_RuneStone_Puti: MSFZ_RuneStone_Base
	{
		scope = 2;
		displayName = "$STR_RuneStone_Puti";
		descriptionShort = "$STR_RuneStone_Puti1";
		model = "MSFZ_Mifical_Pack\Items\RuneStones_2\MSFZ_RuneStone_2.p3d";
		animClass = "NoFireClass";
		weight = 300;
		itemSize[] = {2,2};
		canBeSplit = 1;
		inventorySlot[] = {"Runes","Material_FPole_Runes"};
		isMeleeWeapon = 1;
		hiddenSelections[] = {"camo1"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_2.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Puti.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 200;
					healthLevels[] = {{1,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Puti.rvmat"}},{0.7,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Puti.rvmat"}},{0.5,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_2_damage.rvmat"}},{0.3,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_2_damage.rvmat"}},{0,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_2_destruct.rvmat"}}};
				};
			};
		};
	};
	class MSFZ_RuneStone_Solnce: MSFZ_RuneStone_Base
	{
		scope = 2;
		displayName = "$STR_RuneStone_Solnce";
		descriptionShort = "$STR_RuneStone_Solnce1";
		model = "MSFZ_Mifical_Pack\Items\RuneStones_2\MSFZ_RuneStone_2.p3d";
		animClass = "NoFireClass";
		weight = 300;
		itemSize[] = {2,2};
		canBeSplit = 1;
		inventorySlot[] = {"Runes","Material_FPole_Runes"};
		isMeleeWeapon = 1;
		hiddenSelections[] = {"camo1"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_2.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Solnce.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 200;
					healthLevels[] = {{1,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Solnce.rvmat"}},{0.7,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Solnce.rvmat"}},{0.5,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_2_damage.rvmat"}},{0.3,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_2_damage.rvmat"}},{0,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_2_destruct.rvmat"}}};
				};
			};
		};
	};
	class MSFZ_RuneStone_Tis: MSFZ_RuneStone_Base
	{
		scope = 2;
		displayName = "$STR_RuneStone_Tis";
		descriptionShort = "$STR_RuneStone_Tis1";
		model = "MSFZ_Mifical_Pack\Items\RuneStones_2\MSFZ_RuneStone_2.p3d";
		animClass = "NoFireClass";
		weight = 300;
		itemSize[] = {2,2};
		canBeSplit = 1;
		inventorySlot[] = {"Runes","Material_FPole_Runes"};
		isMeleeWeapon = 1;
		hiddenSelections[] = {"camo1"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_2.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Tis.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 200;
					healthLevels[] = {{1,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Tis.rvmat"}},{0.7,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Tis.rvmat"}},{0.5,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_2_damage.rvmat"}},{0.3,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_2_damage.rvmat"}},{0,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_2_destruct.rvmat"}}};
				};
			};
		};
	};
	class MSFZ_RuneStone_Busk: MSFZ_RuneStone_Base
	{
		scope = 2;
		displayName = "$STR_RuneStone_Busk";
		descriptionShort = "$STR_RuneStone_Busk1";
		model = "MSFZ_Mifical_Pack\Items\RuneStones_2\MSFZ_RuneStone_3.p3d";
		animClass = "NoFireClass";
		weight = 300;
		itemSize[] = {2,2};
		canBeSplit = 1;
		inventorySlot[] = {"Runes","Material_FPole_Runes"};
		isMeleeWeapon = 1;
		hiddenSelections[] = {"camo1"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_3.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Busk.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 200;
					healthLevels[] = {{1,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Busk.rvmat"}},{0.7,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Busk.rvmat"}},{0.5,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_3_damage.rvmat"}},{0.3,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_3_damage.rvmat"}},{0,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_3_destruct.rvmat"}}};
				};
			};
		};
	};
	class MSFZ_RuneStone_Cherepaha: MSFZ_RuneStone_Base
	{
		scope = 2;
		displayName = "$STR_RuneStone_Cherepaha";
		descriptionShort = "$STR_RuneStone_Cherepaha1";
		model = "MSFZ_Mifical_Pack\Items\RuneStones_2\MSFZ_RuneStone_3.p3d";
		animClass = "NoFireClass";
		weight = 300;
		itemSize[] = {2,2};
		canBeSplit = 1;
		inventorySlot[] = {"Runes","Material_FPole_Runes"};
		isMeleeWeapon = 1;
		hiddenSelections[] = {"camo1"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_3.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Cherepaha.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 200;
					healthLevels[] = {{1,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Cherepaha.rvmat"}},{0.7,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Cherepaha.rvmat"}},{0.5,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_3_damage.rvmat"}},{0.3,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_3_damage.rvmat"}},{0,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_3_destruct.rvmat"}}};
				};
			};
		};
	};
	class MSFZ_RuneStone_Kaban: MSFZ_RuneStone_Base
	{
		scope = 2;
		displayName = "$STR_RuneStone_Kaban";
		descriptionShort = "$STR_RuneStone_Kaban1";
		model = "MSFZ_Mifical_Pack\Items\RuneStones_2\MSFZ_RuneStone_3.p3d";
		animClass = "NoFireClass";
		weight = 300;
		itemSize[] = {2,2};
		canBeSplit = 1;
		inventorySlot[] = {"Runes","Material_FPole_Runes"};
		isMeleeWeapon = 1;
		hiddenSelections[] = {"camo1"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_3.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Kaban.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 200;
					healthLevels[] = {{1,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Kaban.rvmat"}},{0.7,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Kaban.rvmat"}},{0.5,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_3_damage.rvmat"}},{0.3,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_3_damage.rvmat"}},{0,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_3_destruct.rvmat"}}};
				};
			};
		};
	};
	class MSFZ_RuneStone_Medved: MSFZ_RuneStone_Base
	{
		scope = 2;
		displayName = "$STR_RuneStone_Medved";
		descriptionShort = "$STR_RuneStone_Medved1";
		model = "MSFZ_Mifical_Pack\Items\RuneStones_2\MSFZ_RuneStone_3.p3d";
		animClass = "NoFireClass";
		weight = 300;
		itemSize[] = {2,2};
		canBeSplit = 1;
		inventorySlot[] = {"Runes","Material_FPole_Runes"};
		isMeleeWeapon = 1;
		hiddenSelections[] = {"camo1"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_3.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Medved.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 200;
					healthLevels[] = {{1,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Medved.rvmat"}},{0.7,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Medved.rvmat"}},{0.5,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_3_damage.rvmat"}},{0.3,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_3_damage.rvmat"}},{0,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_3_destruct.rvmat"}}};
				};
			};
		};
	};
	class MSFZ_RuneStone_Orel: MSFZ_RuneStone_Base
	{
		scope = 2;
		displayName = "$STR_RuneStone_Orel";
		descriptionShort = "$STR_RuneStone_Orel1";
		model = "MSFZ_Mifical_Pack\Items\RuneStones_2\MSFZ_RuneStone_3.p3d";
		animClass = "NoFireClass";
		weight = 300;
		itemSize[] = {2,2};
		canBeSplit = 1;
		inventorySlot[] = {"Runes","Material_FPole_Runes"};
		isMeleeWeapon = 1;
		hiddenSelections[] = {"camo1"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_3.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Orel.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 200;
					healthLevels[] = {{1,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Orel.rvmat"}},{0.7,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Orel.rvmat"}},{0.5,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_3_damage.rvmat"}},{0.3,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_3_damage.rvmat"}},{0,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_3_destruct.rvmat"}}};
				};
			};
		};
	};
	class MSFZ_RuneStone_Osa: MSFZ_RuneStone_Base
	{
		scope = 2;
		displayName = "$STR_RuneStone_Osa";
		descriptionShort = "$STR_RuneStone_Osa1";
		model = "MSFZ_Mifical_Pack\Items\RuneStones_2\MSFZ_RuneStone_3.p3d";
		animClass = "NoFireClass";
		weight = 300;
		itemSize[] = {2,2};
		canBeSplit = 1;
		inventorySlot[] = {"Runes","Material_FPole_Runes"};
		isMeleeWeapon = 1;
		hiddenSelections[] = {"camo1"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_3.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Osa.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 200;
					healthLevels[] = {{1,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Osa.rvmat"}},{0.7,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Osa.rvmat"}},{0.5,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_3_damage.rvmat"}},{0.3,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_3_damage.rvmat"}},{0,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_3_destruct.rvmat"}}};
				};
			};
		};
	};
	class MSFZ_RuneStone_Tigr: MSFZ_RuneStone_Base
	{
		scope = 2;
		displayName = "$STR_RuneStone_Tigr";
		descriptionShort = "$STR_RuneStone_Tigr1";
		model = "MSFZ_Mifical_Pack\Items\RuneStones_2\MSFZ_RuneStone_3.p3d";
		animClass = "NoFireClass";
		weight = 300;
		itemSize[] = {2,2};
		canBeSplit = 1;
		inventorySlot[] = {"Runes","Material_FPole_Runes"};
		isMeleeWeapon = 1;
		hiddenSelections[] = {"camo1"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_3.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Tigr.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 200;
					healthLevels[] = {{1,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Tigr.rvmat"}},{0.7,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Tigr.rvmat"}},{0.5,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_3_damage.rvmat"}},{0.3,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_3_damage.rvmat"}},{0,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_3_destruct.rvmat"}}};
				};
			};
		};
	};
	class MSFZ_RuneStone_Wolf: MSFZ_RuneStone_Base
	{
		scope = 2;
		displayName = "$STR_RuneStone_Wolf";
		descriptionShort = "$STR_RuneStone_Wolf1";
		model = "MSFZ_Mifical_Pack\Items\RuneStones_2\MSFZ_RuneStone_3.p3d";
		animClass = "NoFireClass";
		weight = 300;
		itemSize[] = {2,2};
		canBeSplit = 1;
		inventorySlot[] = {"Runes","Material_FPole_Runes"};
		isMeleeWeapon = 1;
		hiddenSelections[] = {"camo1"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_3.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Wolf.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 200;
					healthLevels[] = {{1,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Wolf.rvmat"}},{0.7,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Wolf.rvmat"}},{0.5,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_3_damage.rvmat"}},{0.3,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_3_damage.rvmat"}},{0,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_3_destruct.rvmat"}}};
				};
			};
		};
	};
	class MSFZ_RuneStone_Zayac: MSFZ_RuneStone_Base
	{
		scope = 2;
		displayName = "$STR_RuneStone_Zayac";
		descriptionShort = "$STR_RuneStone_Zayac1";
		model = "MSFZ_Mifical_Pack\Items\RuneStones_2\MSFZ_RuneStone_3.p3d";
		animClass = "NoFireClass";
		weight = 300;
		itemSize[] = {2,2};
		canBeSplit = 1;
		inventorySlot[] = {"Runes","Material_FPole_Runes"};
		isMeleeWeapon = 1;
		hiddenSelections[] = {"camo1"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_3.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Zayac.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 200;
					healthLevels[] = {{1,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Zayac.rvmat"}},{0.7,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Zayac.rvmat"}},{0.5,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_3_damage.rvmat"}},{0.3,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_3_damage.rvmat"}},{0,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_3_destruct.rvmat"}}};
				};
			};
		};
	};
	class MSFZ_RuneStone_Zmey: MSFZ_RuneStone_Base
	{
		scope = 2;
		displayName = "$STR_RuneStone_Zmey";
		descriptionShort = "$STR_RuneStone_Zmey1";
		model = "MSFZ_Mifical_Pack\Items\RuneStones_2\MSFZ_RuneStone_3.p3d";
		animClass = "NoFireClass";
		weight = 300;
		itemSize[] = {2,2};
		canBeSplit = 1;
		inventorySlot[] = {"Runes","Material_FPole_Runes"};
		isMeleeWeapon = 1;
		hiddenSelections[] = {"camo1"};
		hiddenSelectionsTextures[] = {"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_3.paa"};
		hiddenSelectionsMaterials[] = {"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Zmey.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 200;
					healthLevels[] = {{1,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Zmey.rvmat"}},{0.7,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_Zmey.rvmat"}},{0.5,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_3_damage.rvmat"}},{0.3,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_3_damage.rvmat"}},{0,{"MSFZ_Mifical_Pack\Items\RuneStones_2\data\MSFZ_RuneStone_3_destruct.rvmat"}}};
				};
			};
		};
	};
};
