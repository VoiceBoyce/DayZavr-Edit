class CfgPatches
{
	class TGK_RussianSoldier_v2
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
class cfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base
	{
	};
	class BalaclavaMask_ColorBase;
	class TGK_RussianSoldier_v2_Balaclava_base: BalaclavaMask_ColorBase
	{
		scope=0;
		displayName="$STR_RusV2_Balaclava01";
		descriptionShort="$STR_RusV2_Balaclava02";
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
		model="TGK_RussianSoldier_V2\set\Balaclava\models\Balaclava_g.p3d";
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
								"TGK_RussianSoldier_V2\set\Balaclava\data\Balaclava.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_RussianSoldier_V2\set\Balaclava\data\Balaclava.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_RussianSoldier_V2\set\Balaclava\data\Balaclava_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_RussianSoldier_V2\set\Balaclava\data\Balaclava_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_RussianSoldier_V2\set\Balaclava\data\Balaclava_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		class ClothingTypes
		{
			male="TGK_RussianSoldier_V2\set\Balaclava\models\Balaclava_m.p3d";
			female="TGK_RussianSoldier_V2\set\Balaclava\models\Balaclava_m.p3d";
		};
	};
	class TGK_RussianSoldier_v2_Balaclava_Alpine: TGK_RussianSoldier_v2_Balaclava_base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V2\set\Balaclava\data\Balaclava_Alpine.paa"
		};
	};
	class TGK_RussianSoldier_v2_Balaclava_Black: TGK_RussianSoldier_v2_Balaclava_base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V2\set\Balaclava\data\Balaclava_Black.paa"
		};
	};
	class TGK_RussianSoldier_v2_Balaclava_Black_MC: TGK_RussianSoldier_v2_Balaclava_base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V2\set\Balaclava\data\Balaclava_Black_MC.paa"
		};
	};
	class TGK_RussianSoldier_v2_Balaclava_EMR: TGK_RussianSoldier_v2_Balaclava_base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V2\set\Balaclava\data\Balaclava_EMR.paa"
		};
	};
	class TGK_RussianSoldier_v2_Balaclava_MC: TGK_RussianSoldier_v2_Balaclava_base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V2\set\Balaclava\data\Balaclava_MC.paa"
		};
	};
};
