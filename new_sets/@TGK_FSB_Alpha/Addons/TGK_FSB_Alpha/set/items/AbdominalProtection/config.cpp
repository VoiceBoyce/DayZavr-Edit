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
	class TGK_FSB_ALPHA_AbdominalProtection_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_TGK_FSB_ALPHA_AbdominalProtection01";
		descriptionShort="$STR_TGK_FSB_ALPHA_AbdominalProtection02";
		model="TGK_FSB_Alpha\set\items\AbdominalProtection\models\AbdominalProtection.p3d";
		inventorySlot[]=
		{
			"AbdominalProtection"
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
								"TGK_FSB_Alpha\set\items\AbdominalProtection\data\AbdominalProtection.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_FSB_Alpha\set\items\AbdominalProtection\data\AbdominalProtection.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_FSB_Alpha\set\items\AbdominalProtection\data\AbdominalProtection_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_FSB_Alpha\set\items\AbdominalProtection\data\AbdominalProtection_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_FSB_Alpha\set\items\AbdominalProtection\data\AbdominalProtection_destruct.rvmat"
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
	class TGK_FSB_ALPHA_AbdominalProtection_Alpine: TGK_FSB_ALPHA_AbdominalProtection_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\items\AbdominalProtection\data\AbdominalProtection_Alpine.paa"
		};
	};
	class TGK_FSB_ALPHA_AbdominalProtection_Black: TGK_FSB_ALPHA_AbdominalProtection_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\items\AbdominalProtection\data\AbdominalProtection_Black.paa"
		};
	};
	class TGK_FSB_ALPHA_AbdominalProtection_Black_MC: TGK_FSB_ALPHA_AbdominalProtection_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\items\AbdominalProtection\data\AbdominalProtection_Black_MC.paa"
		};
	};
	class TGK_FSB_ALPHA_AbdominalProtection_MC: TGK_FSB_ALPHA_AbdominalProtection_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\items\AbdominalProtection\data\AbdominalProtection_MC.paa"
		};
	};
	class TGK_FSB_ALPHA_AbdominalProtection_ODGreen: TGK_FSB_ALPHA_AbdominalProtection_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\items\AbdominalProtection\data\AbdominalProtection_ODGreen.paa"
		};
	};
	class TGK_FSB_ALPHA_AbdominalProtection_Tropic_MC: TGK_FSB_ALPHA_AbdominalProtection_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\items\AbdominalProtection\data\AbdominalProtection_Tropic_MC.paa"
		};
	};
};
class CfgNonAIVehicles
{
	class ProxyAttachment;
	class ProxyAbdominalProtection: ProxyAttachment
	{
		scope=2;
		inventorySlot="AbdominalProtection";
		model="TGK_FSB_Alpha\set\items\AbdominalProtection\models\AbdominalProtection.p3d";
	};
};
class CfgSlots
{
	class Slot_AbdominalProtection
	{
		name="AbdominalProtection";
		displayName="$STR_TGK_FSB_ALPHA_AbdominalProtection01";
		GhostIcon="set:TGK_CustomIcons image:AbdominalProtection";
	};
};
