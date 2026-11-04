class CfgPatches
{
	class Vp_SET19
	{
		units[] = {"Vp_SET19_Pants_Desert","Vp_SET19_Pants_Black","Vp_SET19_Pants_Black_Pink","Vp_SET19_Pants_Black_Pixel"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Characters","DZ_Scripts"};
	};
};
class cfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base{};
	class Vp_SET19_Pants_ColorBase: Clothing
	{
		scope = 0;
		displayName = "$STR_VBC_Spetsnaz_Pants";
		descriptionShort = "$STR_VBC_Spetsnaz_Pants_Desc";
		model = "\set_military\set\pants\model\Pants_g.p3d";
		attachments[] = {};
		inventorySlot[] = {"Legs"};
		itemInfo[] = {"Clothing","Legs"};
		quickBarBonus = 0;
		weight = 1200;
		ragQuantity = 6;
		itemSize[] = {4,3};
		varWetMax = 0.249;
		heatIsolation = 0.6;
		itemsCargoSize[] = {10,5};
		repairableWithKits[] = {5,2};
		repairCosts[] = {75.0,50.0};
		hiddenSelections[] = {"camo","personality"};
		class ClothingTypes
		{
			male = "\set_military\set\pants\model\Pants_m.p3d";
			female = "\set_military\set\pants\model\Pants_f.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					transferToAttachmentsCoef = 0.5;
					healthLevels[] = {{1.0,{"\set_military\set\pants\data\Pants.rvmat"}},{0.7,{"\set_military\set\pants\data\Pants.rvmat"}},{0.5,{"\set_military\set\pants\data\Pants_damage.rvmat"}},{0.3,{"\set_military\set\pants\data\Pants_damage.rvmat"}},{0.0,{"\set_military\set\pants\data\Pants_destruct.rvmat"}}};
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
	class Vp_SET19_Pants_Desert: Vp_SET19_Pants_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"\set_military\set\pants\data\Pants_co.paa"};
	};
	class Vp_SET19_Pants_Black: Vp_SET19_Pants_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"\set_military\set\retexture\black\pants_black.paa"};
	};
	class Vp_SET19_Pants_Black_Pink: Vp_SET19_Pants_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"\set_military\set\retexture\black_pink\pants_black_pink.paa"};
	};
	class Vp_SET19_Pants_Black_Pixel: Vp_SET19_Pants_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"\set_military\set\retexture\black_pixel\pants_black_pixel.paa"};
	};
};
