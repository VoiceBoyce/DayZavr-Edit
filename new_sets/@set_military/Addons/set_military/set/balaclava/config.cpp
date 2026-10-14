class CfgPatches
{
	class Vp_Set19
	{
		units[]=
		{
			"Vp_Set19_Mask_Desert",
			"Vp_Set19_Mask_Black",
			"Vp_Set19_Mask_Black_Pink",
			"Vp_Set19_Mask_Black_Pixel"
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
	class Vp_Set19_Mask_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_set_military_Mask";
		descriptionShort="";
		model="\set_military\set\balaclava\model\mask_g.p3d";
		attachments[]={};
		inventorySlot[]=
		{
			"Mask"
		};
		itemInfo[]=
		{
			"Clothing",
			"Mask"
		};
		ragQuantity=6;
		quickBarBonus=2;
		weight=310;
		itemSize[]={3,3};
		varWetMax=0.050000001;
		heatIsolation=0.5;
		repairableWithKits[]={3,8};
		repairCosts[]={75,50};
		hiddenSelections[]=
		{
			"camo"
		};
		headSelectionsToHide[]=
		{
			"Clipping_Balaclava"
		};
		class ClothingTypes
		{
			male="\set_military\set\balaclava\model\mask_mf.p3d";
			female="\set_military\set\balaclava\model\mask_mf.p3d";
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
								"\set_military\set\balaclava\data\Mask.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"\set_military\set\balaclava\data\Mask.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"\set_military\set\balaclava\data\Mask_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"\set_military\set\balaclava\data\Mask_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"\set_military\set\balaclava\data\Mask_destruct.rvmat"
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
	class Vp_Set19_Mask_Desert: Vp_Set19_Mask_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"\set_military\set\balaclava\data\Mask_co.paa"
		};
	};
	class Vp_Set19_Mask_Black: Vp_Set19_Mask_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"\set_military\set\retexture\black\mask_black.paa"
		};
	};
	class Vp_Set19_Mask_Black_Pink: Vp_Set19_Mask_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"\set_military\set\retexture\black_pink\mask_black_pink.paa"
		};
	};
	class Vp_Set19_Mask_Black_Pixel: Vp_Set19_Mask_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"\set_military\set\retexture\black_pixel\mask_black_pixel.paa"
		};
	};
};
