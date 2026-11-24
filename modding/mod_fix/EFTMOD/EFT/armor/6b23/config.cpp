class CfgPatches
{
	class CfgPatches_6b23_Vest
	{
		units[] = {"EFT_Vest_6b23_ColorBase","EFT_Vest_6b23_Heavy_ColorBase" };
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Characters"};
	};
};
class CfgVehicles
{
	class Clothing;
	class EFT_Vest_6b23_ColorBase: Clothing
	{
		scope = 0;
		displayName = "#EFT_Vest_6b23_dig_displayName";
		descriptionShort = "#EFT_Vest_6b23_dig_descriptionShort";
		model = "EFT\armor\6b23\6b23_g.p3d";
		inventorySlot[] = {"Vest"};
		itemInfo[] = {"Clothing","Vest"};
		weight = 7900;
		itemSize[] = {3,4};
		itemsCargoSize[] = {6, 4};
		varWetMax = 0.09;
		heatIsolation = 0.8;
		repairableWithKits[] = {3,8};
		repairCosts[] = {75,50};
		hiddenSelections[] = {"zbytek"};
		class ClothingTypes
		{
			male = "EFT\armor\6b23\6b23.p3d";
			female = "EFT\armor\6b23\6b23.p3d";
			
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 325;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1,{"EFT\armor\6b23\Data\armor.rvmat"}},{0.45,{"EFT\armor\6b23\Data\armor.rvmat"}},{0.35,{"EFT\armor\6b23\Data\armor_damage.rvmat"}},{0.3,{"EFT\armor\6b23\Data\armor_damage.rvmat"}},{0,{"EFT\armor\6b23\Data\armor_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health
					{
						damage = 0.17;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0.17;
					};
				};
				class Melee
				{
					class Health
					{
						damage = 0.17;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0.17;
					};
				};
				class Infected
				{
					class Health
					{
						damage = 0.17;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0.17;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage = 0.3;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0.5;
					};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem
				{
					soundSet = "SmershVest_pickup_SoundSet";
					id = 797;
				};
				class drop
				{
					soundset = "SmershVest_drop_SoundSet";
					id = 898;
				};
			};
		};
	};
	
	class EFT_Vest_6b23_dig: EFT_Vest_6b23_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"EFT\armor\6b23\Data\armor_dig.paa"};
	};
	
	class EFT_Vest_6b23_Heavy_ColorBase: Clothing
	{
		scope = 0;
		displayName = "#EFT_Vest_6b23_Heavy_displayName";
		descriptionShort = "#EFT_Vest_6b23_Heavy_descriptionShort";
		model = "EFT\armor\6b23\6b23_Heavy_g.p3d";
		inventorySlot[] = {"Vest"};
		itemInfo[] = {"Clothing","Vest"};
		weight = 7200;
		itemSize[] = {3,4};
		itemsCargoSize[] = {6, 4};
		varWetMax = 0.09;
		heatIsolation = 0.8;
		repairableWithKits[] = {3,8};
		repairCosts[] = {75,50};
		hiddenSelections[] = {"zbytek"};
		class ClothingTypes
		{
			male = "EFT\armor\6b23\6b23_Heavy.p3d";
			female = "EFT\armor\6b23\6b23_Heavy.p3d";
			
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 325;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1,{"EFT\armor\6b23\Data\armor.rvmat"}},{0.45,{"EFT\armor\6b23\Data\armor.rvmat"}},{0.35,{"EFT\armor\6b23\Data\armor_damage.rvmat"}},{0.3,{"EFT\armor\6b23\Data\armor_damage.rvmat"}},{0,{"EFT\armor\6b23\Data\armor_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health
					{
						damage = 0.17;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0.17;
					};
				};
				class Melee
				{
					class Health
					{
						damage = 0.15;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0.15;
					};
				};
				class Infected
				{
					class Health
					{
						damage = 0.15;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0.15;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage = 0.3;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0.5;
					};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem
				{
					soundSet = "SmershVest_pickup_SoundSet";
					id = 797;
				};
				class drop
				{
					soundset = "SmershVest_drop_SoundSet";
					id = 898;
				};
			};
		};
	};
	
	class EFT_Vest_6b23_Heavy: EFT_Vest_6b23_Heavy_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"EFT\armor\6b23\Data\armor_flora.paa"};
	};
};
