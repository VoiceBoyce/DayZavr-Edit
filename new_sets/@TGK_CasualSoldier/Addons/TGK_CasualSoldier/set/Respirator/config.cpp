class CfgPatches
{
	class TGK_Casual_Respirator
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Characters",
			"DZ_Scripts",
			"DZ_Characters_Headgear",
			"DZ_Characters_Masks"
		};
	};
};
class cfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base
	{
	};
	class TGK_Casual_Respirator_base: Clothing
	{
		scope=0;
		displayName="$STR_Casual_Resp01";
		descriptionShort="$STR_Casual_Resp02";
		inventorySlot[]=
		{
			"Mask"
		};
		itemInfo[]=
		{
			"Clothing",
			"Mask"
		};
		repairableWithKits[]={8,6};
		repairCosts[]={30,25};
		rotationFlags=1;
		weight=730;
		weightPerQuantityUnit=0.2;
		itemSize[]={3,4};
		varWetMax=0.49000001;
		heatIsolation=0.60000002;
		visibilityModifier=0.89999998;
		stackedUnit="ml";
		model="TGK_CasualSoldier\set\Respirator\models\Respirator_g.p3d";
		headSelectionsToHide[]=
		{
			"Clipping_Gasmask"
		};
		hiddenSelections[]=
		{
			"camo"
		};
		class ClothingTypes
		{
			male="TGK_CasualSoldier\set\Respirator\models\Respirator_m.p3d";
			female="TGK_CasualSoldier\set\Respirator\models\Respirator_m.p3d";
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
								"TGK_CasualSoldier\set\Respirator\data\Respirator.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_CasualSoldier\set\Respirator\data\Respirator.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_CasualSoldier\set\Respirator\data\Respirator_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_CasualSoldier\set\Respirator\data\Respirator_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_CasualSoldier\set\Respirator\data\Respirator_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		class Protection
		{
			biological=1;
			chemical=1;
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem
				{
					soundSet="DarkMotoHelmet_pickup_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="DarkMotoHelmet_drop_SoundSet";
					id=898;
				};
			};
		};
		soundImpactType="plastic";
		soundVoiceType="gasmask";
		soundVoicePriority=5;
	};
	class TGK_Casual_Respirator_Black: TGK_Casual_Respirator_base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_CasualSoldier\set\Respirator\data\Respirator_Black.paa"
		};
	};
	class TGK_Casual_Respirator_White: TGK_Casual_Respirator_base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_CasualSoldier\set\Respirator\data\Respirator_White.paa"
		};
	};
};
