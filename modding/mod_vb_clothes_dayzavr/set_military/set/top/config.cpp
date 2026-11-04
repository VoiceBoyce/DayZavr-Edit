class CfgPatches
{
	class Vp_SET19
	{
		units[] = {"Vp_SET19_Top_Desert","Vp_SET19_Top_Black","Vp_SET19_Top_Black_Pink","Vp_SET19_Top_Black_Pixel"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Characters"};
	};
};
class cfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base{};
	class Vp_SET19_Top_ColorBase: Clothing
	{
		scope = 0;
		displayName = "$STR_VBC_Spetsnaz_Top";
		descriptionShort = "$STR_VBC_Spetsnaz_Top_Desc";
		model = "\set_military\set\top\model\Top_g.p3d";
		attachments[] = {};
		inventorySlot[] = {"Body"};
		itemInfo[] = {"Clothing","Body"};
		quickBarBonus = 0;
		weight = 1500;
		itemSize[] = {4,3};
		varWetMax = 0.249;
		heatIsolation = 0.7;
		itemsCargoSize[] = {10,6};
		repairableWithKits[] = {5,2};
		repairCosts[] = {75.0,50.0};
		hiddenSelections[] = {"camo","personality"};
		class ClothingTypes
		{
			male = "\set_military\set\top\model\Top_m.p3d";
			female = "\set_military\set\top\model\Top_f.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"\set_military\set\top\data\top.rvmat"}},{0.7,{"\set_military\set\top\data\top.rvmat"}},{0.5,{"\set_military\set\top\data\top_damage.rvmat"}},{0.3,{"\set_military\set\top\data\top_damage.rvmat"}},{0.0,{"\set_military\set\top\data\top_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health
					{
						damage = 0.85;
					};
					class Blood
					{
						damage = 0.85;
					};
					class Shock
					{
						damage = 0.85;
					};
				};
				class Melee
				{
					class Health
					{
						damage = 0.75;
					};
					class Blood
					{
						damage = 0.75;
					};
					class Shock
					{
						damage = 0.75;
					};
				};
				class Infected
				{
					class Health
					{
						damage = 0.75;
					};
					class Blood
					{
						damage = 0.75;
					};
					class Shock
					{
						damage = 0.75;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage = 0.85;
					};
					class Blood
					{
						damage = 0.85;
					};
					class Shock
					{
						damage = 0.85;
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
					soundSet = "Shirt_pickup_SoundSet";
					id = 797;
				};
				class drop
				{
					soundset = "Shirt_drop_SoundSet";
					id = 898;
				};
			};
		};
	};
	class Vp_SET19_Top_Desert: Vp_SET19_Top_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"\set_military\set\top\data\top_co.paa"};
	};
	class Vp_SET19_Top_Black: Vp_SET19_Top_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"\set_military\set\retexture\black\top_black.paa"};
	};
	class Vp_SET19_Top_Black_Pink: Vp_SET19_Top_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"\set_military\set\retexture\black_pink\top_black_pink.paa"};
	};
	class Vp_SET19_Top_Black_Pixel: Vp_SET19_Top_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"\set_military\set\retexture\black_pixel\Top_black_pixel.paa"};
	};
};
