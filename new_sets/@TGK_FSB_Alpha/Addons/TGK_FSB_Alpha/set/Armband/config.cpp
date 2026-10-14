class CfgPatches
{
	class TGK_FSB_ALPHA
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
	class TGK_FSB_ALPHA_ArmBand_base: BalaclavaMask_ColorBase
	{
		scope=0;
		displayName="$STR_TGK_FSB_ALPHA_ArmBand_01";
		descriptionShort="$STR_TGK_FSB_ALPHA_ArmBand_02";
		inventorySlot[]=
		{
			"Armband"
		};
		itemInfo[]=
		{
			"Clothing",
			"Armband"
		};
		rotationFlags=17;
		weight=300;
		itemSize[]={2,2};
		ragQuantity=2;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		varWetMax=0.0099999998;
		heatIsolation=0.0099999998;
		model="TGK_FSB_Alpha\set\Armband\models\ArmBand_g.p3d";
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
					hitpoints=200;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"TGK_FSB_Alpha\set\Armband\data\ArmBand.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_FSB_Alpha\set\Armband\data\ArmBand.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_FSB_Alpha\set\Armband\data\ArmBand_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_FSB_Alpha\set\Armband\data\ArmBand_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_FSB_Alpha\set\Armband\data\ArmBand_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		class ClothingTypes
		{
			male="TGK_FSB_Alpha\set\Armband\models\ArmBand_m.p3d";
			female="TGK_FSB_Alpha\set\Armband\models\ArmBand_m.p3d";
		};
	};
	class TGK_FSB_ALPHA_ArmBand_Yellow: TGK_FSB_ALPHA_ArmBand_base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\Armband\data\ArmBand_Yellow.paa"
		};
	};
	class TGK_FSB_ALPHA_ArmBand_Orange: TGK_FSB_ALPHA_ArmBand_base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\Armband\data\ArmBand_Orange.paa"
		};
	};
};
