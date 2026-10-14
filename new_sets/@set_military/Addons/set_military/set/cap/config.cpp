class CfgPatches
{
	class Vp_SET19
	{
		units[]=
		{
			"Vp_SET19_cap_Desert",
			"Vp_SET19_cap_Black",
			"Vp_SET19_cap_Black_Pink",
			"Vp_SET19_cap_Black_Pixel"
		};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Characters",
			"DZ_Scripts"
		};
	};
};
class cfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base
	{
	};
	class Vp_SET19_cap_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_set_military_cap";
		descriptionShort="";
		model="\set_military\set\cap\model\cap_g.p3d";
		attachments[]={};
		inventorySlot[]=
		{
			"Headgear"
		};
		simulation="clothing";
		vehicleClass="Clothing";
		itemInfo[]=
		{
			"Clothing",
			"Helmet"
		};
		ragQuantity=6;
		quickBarBonus=2;
		weight=310;
		itemSize[]={4,4};
		varWetMax=0.050000001;
		heatIsolation=1;
		repairableWithKits[]={3,8};
		repairCosts[]={25};
		hiddenSelections[]=
		{
			"cap",
			"Headgears"
		};
		headSelectionsToHide[]=
		{
			"Clipping_Balaclava"
		};
		class ClothingTypes
		{
			male="\set_military\set\cap\model\cap_mf.p3d";
			female="\set_military\set\cap\model\cap_mf.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=500;
					transferToAttachmentsCoef=0.5;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"\set_military\set\cap\data\cap.rvmat",
								"\set_military\set\cap\data\Headgears.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"\set_military\set\cap\data\cap.rvmat",
								"\set_military\set\cap\data\Headgears.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"\set_military\set\cap\data\cap_damage.rvmat",
								"\set_military\set\cap\data\Headgears_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"\set_military\set\cap\data\cap_damage.rvmat",
								"\set_military\set\cap\data\Headgears_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"\set_military\set\cap\data\cap_destruct.rvmat",
								"\set_military\set\cap\data\Headgears_damage.rvmat"
							}
						}
					};
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health
					{
						damage=0.85000002;
					};
					class Blood
					{
						damage=0.80000001;
					};
					class Shock
					{
						damage=0.80000001;
					};
				};
				class Melee
				{
					class Health
					{
						damage=0.85000002;
					};
					class Blood
					{
						damage=0.80000001;
					};
					class Shock
					{
						damage=0.80000001;
					};
				};
				class Infected
				{
					class Health
					{
						damage=0.85000002;
					};
					class Blood
					{
						damage=0.80000001;
					};
					class Shock
					{
						damage=0.80000001;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage=0.2;
					};
					class Blood
					{
						damage=0.2;
					};
					class Shock
					{
						damage=0.2;
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
					soundSet="Shirt_pickup_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="Shirt_drop_SoundSet";
					id=898;
				};
			};
		};
	};
	class Vp_SET19_cap_Desert: Vp_SET19_cap_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"\set_military\set\cap\data\cap_co.paa",
			"\set_military\set\cap\data\Headgears_co.paa"
		};
	};
	class Vp_SET19_cap_Black: Vp_SET19_cap_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"\set_military\set\retexture\black\cap_black.paa",
			"\set_military\set\retexture\black\headgears_black.paa"
		};
	};
	class Vp_SET19_cap_Black_Pink: Vp_SET19_cap_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"\set_military\set\retexture\black_pink\cap_black_pink.paa",
			"\set_military\set\retexture\black_pink\headgears_black_pink.paa"
		};
	};
	class Vp_SET19_cap_Black_Pixel: Vp_SET19_cap_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"\set_military\set\retexture\black_pixel\cap_black_pixel.paa",
			"\set_military\set\retexture\black_pixel\headgears_black_pixel.paa"
		};
	};
};
