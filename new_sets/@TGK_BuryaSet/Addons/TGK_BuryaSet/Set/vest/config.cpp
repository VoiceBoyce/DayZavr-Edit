class CfgPatches
{
	class TGK_Burya_vest
	{
		units[]={};
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
class CfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base
	{
	};
	class PlateCarrierVest;
	class TGK_Burya_vest_Colorbase: PlateCarrierVest
	{
		scope=0;
		displayName="$STR_TGK_Burya_vest_01";
		descriptionShort="$STR_TGK_Burya_vest_02";
		model="TGK_BuryaSet\Set\vest\models\Vest_g.p3d";
		inventorySlot[]=
		{
			"Vest"
		};
		itemInfo[]=
		{
			"Clothing",
			"Vest"
		};
		weight=3500;
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
			male="TGK_BuryaSet\Set\vest\models\Vest_m.p3d";
			female="TGK_BuryaSet\Set\vest\models\Vest_m.p3d";
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
								"TGK_BuryaSet\Set\vest\data\Vest.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_BuryaSet\Set\vest\data\Vest.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_BuryaSet\Set\vest\data\Vest_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_BuryaSet\Set\vest\data\Vest_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_BuryaSet\Set\vest\data\Vest_destruct.rvmat"
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
						damage=0.30000001;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.60000002;
					};
				};
				class Melee
				{
					class Health
					{
						damage=0.30000001;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.25;
					};
				};
				class Infected
				{
					class Health
					{
						damage=0.30000001;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.25;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage=0.5;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.25999999;
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
	class TGK_BURYA_Vest_Berezka: TGK_Burya_vest_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\vest\data\Vest_Berezka_co.paa"
		};
	};
	class TGK_BURYA_Vest_Black: TGK_Burya_vest_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\vest\data\Vest_Black_co.paa"
		};
	};
	class TGK_BURYA_Vest_ClearSky: TGK_Burya_vest_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\vest\data\Vest_ClearSky_co.paa"
		};
	};
	class TGK_BURYA_Vest_LitPat: TGK_Burya_vest_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\vest\data\Vest_LitPat_co.paa"
		};
	};
	class TGK_BURYA_Vest_Monolith: TGK_Burya_vest_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\vest\data\Vest_Monolith_co.paa"
		};
	};
	class TGK_BURYA_Vest_QC1: TGK_Burya_vest_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\vest\data\Vest_QC1_co.paa"
		};
	};
	class TGK_BURYA_Vest_Sans: TGK_Burya_vest_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\vest\data\Vest_Sans_co.paa"
		};
	};
	class TGK_BURYA_Vest_DarkGreen: TGK_Burya_vest_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\vest\data\Vest_DarkGreen_co.paa"
		};
	};
	class TGK_BURYA_Vest_DarkRed: TGK_Burya_vest_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\vest\data\Vest_DarkRed_co.paa"
		};
	};
	class TGK_BURYA_Vest_LightGreen: TGK_Burya_vest_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\vest\data\Vest_LightGreen_co.paa"
		};
	};
	class TGK_BURYA_Vest_OSENNIY: TGK_Burya_vest_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\vest\data\Vest_OSENNIY_co.paa"
		};
	};
	class TGK_BURYA_Vest_White: TGK_Burya_vest_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\vest\data\Vest_White_co.paa"
		};
	};
};
