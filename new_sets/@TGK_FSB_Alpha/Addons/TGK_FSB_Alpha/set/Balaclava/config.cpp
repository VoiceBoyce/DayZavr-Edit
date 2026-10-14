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
	class TGK_FSB_ALPHA_Balaclava_base: BalaclavaMask_ColorBase
	{
		scope=0;
		displayName="$STR_TGK_FSB_ALPHA_Balaclava_01";
		descriptionShort="$STR_TGK_FSB_ALPHA_Balaclava_02";
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
		model="TGK_FSB_Alpha\set\Balaclava\models\Balaclava_g.p3d";
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
								"TGK_FSB_Alpha\set\Balaclava\data\Balaclava_Black.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_FSB_Alpha\set\Balaclava\data\Balaclava_Black.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_FSB_Alpha\set\Balaclava\data\Balaclava_Black_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_FSB_Alpha\set\Balaclava\data\Balaclava_Black_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_FSB_Alpha\set\Balaclava\data\Balaclava_Black_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		class ClothingTypes
		{
			male="TGK_FSB_Alpha\set\Balaclava\models\Balaclava_m.p3d";
			female="TGK_FSB_Alpha\set\Balaclava\models\Balaclava_m.p3d";
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
	class TGK_FSB_ALPHA_Balaclava_Alpine: TGK_FSB_ALPHA_Balaclava_base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\Balaclava\data\Balaclava_Alpine.paa"
		};
	};
	class TGK_FSB_ALPHA_Balaclava_Black: TGK_FSB_ALPHA_Balaclava_base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\Balaclava\data\Balaclava_Black.paa"
		};
	};
	class TGK_FSB_ALPHA_Balaclava_Black_MC: TGK_FSB_ALPHA_Balaclava_base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\Balaclava\data\Balaclava_Black_MC.paa"
		};
	};
	class TGK_FSB_ALPHA_Balaclava_MC: TGK_FSB_ALPHA_Balaclava_base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\Balaclava\data\Balaclava_MC.paa"
		};
	};
	class TGK_FSB_ALPHA_Balaclava_ODGreen: TGK_FSB_ALPHA_Balaclava_base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\Balaclava\data\Balaclava_ODGreen.paa"
		};
	};
	class TGK_FSB_ALPHA_Balaclava_Tropic_MC: TGK_FSB_ALPHA_Balaclava_base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\Balaclava\data\Balaclava_Tropic_MC.paa"
		};
	};
};
