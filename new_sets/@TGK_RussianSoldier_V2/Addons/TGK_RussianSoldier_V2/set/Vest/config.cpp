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
	class PlateCarrierVest;
	class TGK_RussianSoldier_v2_Vest_ColorBase: PlateCarrierVest
	{
		scope=0;
		displayName="$STR_RusV2_Vest01";
		descriptionShort="$STR_RusV2_Vest02";
		model="TGK_RussianSoldier_V2\set\Vest\models\6B45_g.p3d";
		inventorySlot[]=
		{
			"Vest"
		};
		itemInfo[]=
		{
			"Clothing",
			"Vest"
		};
		attachments[]=
		{
			"GroinPlate",
			"GroinPouch",
			"GranadePouch",
			"MagPouchBig",
			"MagPouchSmall",
			"MagPouchSmall1",
			"MagPouchSmall2",
			"MagPouchSmall21",
			"MedPouch2"
		};
		weight=7500;
		itemSize[]={4,4};
		quickBarBonus=1;
		varWetMax=0.49000001;
		heatIsolation=0.80000001;
		repairableWithKits[]={3,8};
		repairCosts[]={75,50};
		visibilityModifier=0.94999999;
		hiddenSelections[]=
		{
			"camo"
		};
		class ClothingTypes
		{
			male="TGK_RussianSoldier_V2\set\Vest\models\6B45_m.p3d";
			female="TGK_RussianSoldier_V2\set\Vest\models\6B45_m.p3d";
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
								"TGK_RussianSoldier_V2\set\Vest\data\6B45.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_RussianSoldier_V2\set\Vest\data\6B45.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_RussianSoldier_V2\set\Vest\data\6B45_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_RussianSoldier_V2\set\Vest\data\6B45_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_RussianSoldier_V2\set\Vest\data\6B45_destruct.rvmat"
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
						damage=0.31;
					};
					class Blood
					{
						damage=0.31;
					};
					class Shock
					{
						damage=0.31;
					};
				};
				class Melee
				{
					class Health
					{
						damage=0.20999999;
					};
					class Blood
					{
						damage=0.20999999;
					};
					class Shock
					{
						damage=0.20999999;
					};
				};
				class Infected
				{
					class Health
					{
						damage=0.20999999;
					};
					class Blood
					{
						damage=0.20999999;
					};
					class Shock
					{
						damage=0.20999999;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage=0.20999999;
					};
					class Blood
					{
						damage=0.20999999;
					};
					class Shock
					{
						damage=0.20999999;
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
	class TGK_RussianSoldier_v2_6B45_Alpine: TGK_RussianSoldier_v2_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V2\set\Vest\data\6B45_Alpine.paa"
		};
	};
	class TGK_RussianSoldier_v2_6B45_Black: TGK_RussianSoldier_v2_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V2\set\Vest\data\6B45_Black.paa"
		};
	};
	class TGK_RussianSoldier_v2_6B45_Black_MC: TGK_RussianSoldier_v2_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V2\set\Vest\data\6B45_Black_MC.paa"
		};
	};
	class TGK_RussianSoldier_v2_6B45_EMR: TGK_RussianSoldier_v2_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V2\set\Vest\data\6B45_EMR.paa"
		};
	};
	class TGK_RussianSoldier_v2_6B45_MC: TGK_RussianSoldier_v2_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V2\set\Vest\data\6B45_MC.paa"
		};
	};
};
