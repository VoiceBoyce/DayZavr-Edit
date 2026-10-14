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
	class TGK_RussianSoldier_v2_GroinPlate_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_RusV2_GroinPlate01";
		descriptionShort="$STR_RusV2_GroinPlate02";
		model="TGK_RussianSoldier_V2\set\Items\GroinPlate\models\GroinPlate.p3d";
		inventorySlot[]=
		{
			"GroinPlate"
		};
		itemInfo[]=
		{
			"Clothing"
		};
		weight=1000;
		itemSize[]={3,3};
		quickBarBonus=0;
		varWetMax=0.048999999;
		heatIsolation=0.1;
		repairableWithKits[]={3,8};
		repairCosts[]={75,50};
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
					hitpoints=300;
					transferToAttachmentsCoef=0.5;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"TGK_RussianSoldier_V2\set\Items\GroinPlate\data\GroinPlate.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_RussianSoldier_V2\set\Items\GroinPlate\data\GroinPlate.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_RussianSoldier_V2\set\Items\GroinPlate\data\GroinPlate_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_RussianSoldier_V2\set\Items\GroinPlate\data\GroinPlate_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_RussianSoldier_V2\set\Items\GroinPlate\data\GroinPlate_destruct.rvmat"
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
	class TGK_RussianSoldier_v2_GroinPlate_Alpine: TGK_RussianSoldier_v2_GroinPlate_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V2\set\Items\GroinPlate\data\GroinPlate_Alpine.paa"
		};
	};
	class TGK_RussianSoldier_v2_GroinPlate_Black: TGK_RussianSoldier_v2_GroinPlate_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V2\set\Items\GroinPlate\data\GroinPlate_Black.paa"
		};
	};
	class TGK_RussianSoldier_v2_GroinPlate_Black_MC: TGK_RussianSoldier_v2_GroinPlate_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V2\set\Items\GroinPlate\data\GroinPlate_Black_MC.paa"
		};
	};
	class TGK_RussianSoldier_v2_GroinPlate_EMR: TGK_RussianSoldier_v2_GroinPlate_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V2\set\Items\GroinPlate\data\GroinPlate_EMR.paa"
		};
	};
	class TGK_RussianSoldier_v2_GroinPlate_MC: TGK_RussianSoldier_v2_GroinPlate_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V2\set\Items\GroinPlate\data\GroinPlate_MC.paa"
		};
	};
};
class CfgNonAIVehicles
{
	class ProxyAttachment;
	class ProxyGroinPlate: ProxyAttachment
	{
		scope=2;
		inventorySlot="GroinPlate";
		model="TGK_RussianSoldier_V2\set\Items\GroinPlate\models\GroinPlate.p3d";
	};
};
class CfgSlots
{
	class Slot_GroinPlate
	{
		name="GroinPlate";
		displayName="$STR_RusV2_GroinPlate01";
		GhostIcon="set:TGK_CustomIcons_RusV2 image:AbdominalProtection1";
	};
};
