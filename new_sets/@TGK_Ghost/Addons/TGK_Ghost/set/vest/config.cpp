class CfgPatches
{
	class TGK_Ghost_Vest
	{
		units[]={};
		weapons[]={};
		requiredVersion=1;
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
	class TGK_Ghost_Vest_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_Ghost_Vest01";
		descriptionShort="$STR_Ghost_Vest02";
		model="TGK_Ghost\set\vest\models\Vest_g.p3d";
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
			"WalkieTalkie",
			"Chemlight"
		};
		weight=3500;
		itemSize[]={4,4};
		quickBarBonus=2;
		varWetMax=0.048999999;
		heatIsolation=0.40000001;
		itemsCargoSize[]={7,4};
		repairableWithKits[]={3,8};
		repairCosts[]={75,50};
		hiddenSelections[]=
		{
			"camo"
		};
		class ClothingTypes
		{
			male="TGK_Ghost\set\vest\models\Vest_m.p3d";
			female="TGK_Ghost\set\vest\models\Vest_m.p3d";
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
								"TGK_Ghost\set\vest\data\Vest.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_Ghost\set\vest\data\Vest.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_Ghost\set\vest\data\Vest_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_Ghost\set\vest\data\Vest_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_Ghost\set\vest\data\Vest_destruct.rvmat"
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
						damage=0.20999999;
					};
					class Shock
					{
						damage=0.30000001;
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
	class TGK_Ghost_Vest_Blue: TGK_Ghost_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\vest\data\Vest_Blue.paa"
		};
	};
	class TGK_Ghost_Vest_ClearSky: TGK_Ghost_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\vest\data\Vest_ClearSky.paa"
		};
	};
	class TGK_Ghost_Vest_Black: TGK_Ghost_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\vest\data\Vest_co.paa"
		};
	};
	class TGK_Ghost_Vest_DarkGreen: TGK_Ghost_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\vest\data\Vest_DarkGreen.paa"
		};
	};
	class TGK_Ghost_Vest_DarkRed: TGK_Ghost_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\vest\data\Vest_Red.paa"
		};
	};
	class TGK_Ghost_Vest_EMRSummer: TGK_Ghost_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\vest\data\Vest_EMRSummer.paa"
		};
	};
	class TGK_Ghost_Vest_Gray: TGK_Ghost_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\vest\data\Vest_Gray_co.paa"
		};
	};
	class TGK_Ghost_Vest_GrayCamo: TGK_Ghost_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\vest\data\Vest_GrayCamo.paa"
		};
	};
	class TGK_Ghost_Vest_LightGreen: TGK_Ghost_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\vest\data\Vest_LightGreen.paa"
		};
	};
	class TGK_Ghost_Vest_LitPat: TGK_Ghost_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\vest\data\Vest_LitPat.paa"
		};
	};
	class TGK_Ghost_Vest_Merc: TGK_Ghost_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\vest\data\Vest_Merc_co.paa"
		};
	};
	class TGK_Ghost_Vest_Osenniy: TGK_Ghost_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\vest\data\Vest_OSENNIY.paa"
		};
	};
	class TGK_Ghost_Vest_QC1: TGK_Ghost_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\vest\data\Vest_QC1.paa"
		};
	};
	class TGK_Ghost_Vest_Sans: TGK_Ghost_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\vest\data\Vest_Sans.paa"
		};
	};
	class TGK_Ghost_Vest_White: TGK_Ghost_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\vest\data\Vest_White.paa"
		};
	};
	class TGK_Ghost_Vest_Yellow: TGK_Ghost_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\vest\data\Vest_Yellow.paa"
		};
	};
};
