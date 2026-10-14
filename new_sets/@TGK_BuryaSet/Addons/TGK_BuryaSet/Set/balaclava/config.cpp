class CfgPatches
{
	class TGK_BuryaMask
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Characters",
			"DZ_Scripts",
			"DZ_Characters_Headgear"
		};
	};
};
class CfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base
	{
	};
	class BalaclavaMask_ColorBase;
	class TGK_BuryaMask_ColorBase: BalaclavaMask_ColorBase
	{
		displayName="$STR_TGK_BuryaMask_01";
		descriptionShort="$STR_TGK_BuryaMask_02";
		model="TGK_BuryaSet\Set\balaclava\models\Balaclava_g.p3d";
		inventorySlot[]=
		{
			"Mask"
		};
		itemInfo[]=
		{
			"Clothing",
			"Mask"
		};
		rotationFlags=17;
		weight=300;
		itemSize[]={2,2};
		ragQuantity=2;
		varWetMax=0.34999999;
		heatIsolation=0.5;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		headSelectionsToHide[]=
		{
			"Clipping_Gasmask",
			"Clipping_Balaclava_3holes"
		};
		hiddenSelections[]=
		{
			"camo"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=100;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"TGK_BuryaSet\Set\balaclava\data\Balaclava.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_BuryaSet\Set\balaclava\data\Balaclava.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_BuryaSet\Set\balaclava\data\Balaclava_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_BuryaSet\Set\balaclava\data\Balaclava_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_BuryaSet\Set\balaclava\data\Balaclava_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		class ClothingTypes
		{
			male="TGK_BuryaSet\Set\balaclava\models\Balaclava_m.p3d";
			female="TGK_BuryaSet\Set\balaclava\models\Balaclava_m.p3d";
		};
		class Protection
		{
			biological=0.25;
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
	class TGK_Burya_Balaclava_Black: TGK_BuryaMask_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\balaclava\data\Balaclava_Black_co.paa"
		};
	};
	class TGK_Burya_Balaclava_ClearSky: TGK_BuryaMask_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\balaclava\data\Balaclava_ClearSky_co.paa"
		};
	};
	class TGK_Burya_Balaclava_LitPat: TGK_BuryaMask_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\balaclava\data\Balaclava_LitPat_co.paa"
		};
	};
	class TGK_Burya_Balaclava_Monolith: TGK_BuryaMask_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\balaclava\data\Balaclava_Monolith_co.paa"
		};
	};
	class TGK_Burya_Balaclava_QC1: TGK_BuryaMask_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\balaclava\data\Balaclava_QC1_co.paa"
		};
	};
	class TGK_Burya_Balaclava_Sans: TGK_BuryaMask_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\balaclava\data\Balaclava_Sans_co.paa"
		};
	};
	class TGK_Burya_Balaclava_DarkGreen: TGK_BuryaMask_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\balaclava\data\Balaclava_DarkGreen_co.paa"
		};
	};
	class TGK_Burya_Balaclava_DarkRed: TGK_BuryaMask_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\balaclava\data\Balaclava_DarkRed_co.paa"
		};
	};
	class TGK_Burya_Balaclava_LightGreen: TGK_BuryaMask_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\balaclava\data\Balaclava_LightGreen_co.paa"
		};
	};
	class TGK_Burya_Balaclava_OSENNIY: TGK_BuryaMask_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\balaclava\data\Balaclava_OSENNIY_co.paa"
		};
	};
	class TGK_Burya_Balaclava_White: TGK_BuryaMask_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\balaclava\data\Balaclava_White_co.paa"
		};
	};
};
