class CfgPatches
{
	class Vp_SET19
	{
		units[]=
		{
			"Vp_SET19_Vest_Desert",
			"Vp_SET19_Vest_Black",
			"Vp_SET19_Vest_Black_Pink",
			"Vp_SET19_Vest_Black_Pixel"
		};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Characters",
			"DZ_Characters_Vests"
		};
	};
};
class cfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base
	{
	};
	class Vp_SET19_Vest_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_set_military_Vest";
		descriptionShort="";
		model="\set_military\set\vest\model\Vest_g.p3d";
		attachments[]={};
		inventorySlot[]=
		{
			"Vest"
		};
		itemInfo[]=
		{
			"Clothing",
			"Vest"
		};
		quickBarBonus=2;
		weight=10000;
		itemSize[]={4,4};
		varWetMax=0.050000001;
		heatIsolation=1;
		itemsCargoSize[]={10,4};
		repairableWithKits[]={3,8};
		repairCosts[]={75,50};
		hiddenSelections[]=
		{
			"Vest"
		};
		class ClothingTypes
		{
			male="\set_military\set\vest\model\Vest_mf.p3d";
			female="\set_military\set\vest\model\Vest_mf.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=1000;
					transferToAttachmentsCoef=0.5;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"\set_military\set\vest\data\napleshniki.rvmat",
								"\set_military\set\vest\data\vest.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"\set_military\set\vest\data\napleshniki.rvmat",
								"\set_military\set\vest\data\vest.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"\set_military\set\vest\data\napleshniki_damage.rvmat",
								"\set_military\set\vest\data\vest_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"\set_military\set\vest\data\napleshniki_damage.rvmat",
								"\set_military\set\vest\data\vest_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"\set_military\set\vest\data\napleshniki_destruct.rvmat",
								"\set_military\set\vest\data\vest_destruct.rvmat"
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
						damage=0.1;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.1;
					};
				};
				class Melee
				{
					class Health
					{
						damage=0.1;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.1;
					};
				};
				class Infected
				{
					class Health
					{
						damage=0.050000001;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.0099999998;
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
						damage=0;
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
					soundSet="SmershVest_pickup_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="SmershVest_drop_SoundSet";
					id=898;
				};
			};
		};
	};
	class Vp_SET19_Vest_Desert: Vp_SET19_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"\set_military\set\vest\data\vest_co.paa"
		};
	};
	class Vp_SET19_Vest_Black: Vp_SET19_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"\set_military\set\retexture\black\Vest_black.paa"
		};
	};
	class Vp_SET19_Vest_Black_Pink: Vp_SET19_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"\set_military\set\retexture\black_pink\vest_black_pink.paa"
		};
	};
	class Vp_SET19_Vest_Black_Pixel: Vp_SET19_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"\set_military\set\retexture\black_pixel\Vest__black_pixel.paa"
		};
	};
};
