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
	class PlateCarrierVest;
	class TGK_FSB_ALPHA_Vest_ColorBase: PlateCarrierVest
	{
		scope=0;
		displayName="$STR_FSB_VEST_01";
		descriptionShort="$STR_FSB_VEST_02";
		model="TGK_FSB_Alpha\set\Vest\models\Vest_g.p3d";
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
			"AbdominalProtection",
			"TGK_Magpouch_a",
			"TGK_Magpouch_b",
			"BackPanel",
			"TGK_IFAK",
			"TGK_MedPouch"
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
			male="TGK_FSB_Alpha\set\Vest\models\Vest_m.p3d";
			female="TGK_FSB_Alpha\set\Vest\models\Vest_m.p3d";
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
								"TGK_FSB_Alpha\set\Vest\data\Vest.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_FSB_Alpha\set\Vest\data\Vest.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_FSB_Alpha\set\Vest\data\Vest_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_FSB_Alpha\set\Vest\data\Vest_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_FSB_Alpha\set\Vest\data\Vest_destruct.rvmat"
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
	class TGK_FSB_ALPHA_Vest_Alpine: TGK_FSB_ALPHA_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\Vest\data\Vest_Alpine.paa"
		};
	};
	class TGK_FSB_ALPHA_Vest_Black: TGK_FSB_ALPHA_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\Vest\data\Vest_Black.paa"
		};
	};
	class TGK_FSB_ALPHA_Vest_Black_MC: TGK_FSB_ALPHA_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\Vest\data\Vest_Black_MC.paa"
		};
	};
	class TGK_FSB_ALPHA_Vest_MC: TGK_FSB_ALPHA_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\Vest\data\Vest_MC.paa"
		};
	};
	class TGK_FSB_ALPHA_Vest_ODGreen: TGK_FSB_ALPHA_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\Vest\data\Vest_ODGreen.paa"
		};
	};
	class TGK_FSB_ALPHA_Vest_Tropic_MC: TGK_FSB_ALPHA_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\Vest\data\Vest_Tropic_MC.paa"
		};
	};
};
