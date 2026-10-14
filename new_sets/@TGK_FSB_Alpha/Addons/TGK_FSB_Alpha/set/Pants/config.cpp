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
			"DZ_Characters_Pants"
		};
	};
};
class CfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base
	{
	};
	class GorkaPants_ColorBase;
	class TGK_FSB_ALPHA_Pants_ColorBase: GorkaPants_ColorBase
	{
		scope=0;
		displayName="$STR_FSB_Pants_01";
		descriptionShort="$STR_FSB_Pants_02";
		model="TGK_FSB_Alpha\set\Pants\models\G3_Pants_g.p3d";
		inventorySlot[]=
		{
			"Legs"
		};
		itemInfo[]=
		{
			"Clothing",
			"Legs"
		};
		weight=1000;
		itemSize[]={4,6};
		quickBarBonus=2;
		varWetMax=0.249;
		heatIsolation=0.40000001;
		itemsCargoSize[]={10,15};
		repairableWithKits[]={2,3};
		repairCosts[]={75,50};
		hiddenSelections[]=
		{
			"camo",
			"personality"
		};
		rotationFlags=17;
		class ClothingTypes
		{
			male="TGK_FSB_Alpha\set\Pants\models\G3_Pants_m.p3d";
			female="TGK_FSB_Alpha\set\Pants\models\G3_Pants_m.p3d";
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
								"TGK_FSB_Alpha\set\Pants\data\Pants.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_FSB_Alpha\set\Pants\data\Pants.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_FSB_Alpha\set\Pants\data\Pants_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_FSB_Alpha\set\Pants\data\Pants_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_FSB_Alpha\set\Pants\data\Pants_destruct.rvmat"
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
						damage=0.94999999;
					};
					class Blood
					{
						damage=0.94999999;
					};
					class Shock
					{
						damage=0;
					};
				};
				class Melee
				{
					class Health
					{
						damage=0.94999999;
					};
					class Blood
					{
						damage=0.94999999;
					};
					class Shock
					{
						damage=0.44999999;
					};
				};
				class Infected
				{
					class Health
					{
						damage=0.1;
					};
					class Blood
					{
						damage=0.5;
					};
					class Shock
					{
						damage=0.15000001;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage=1;
					};
					class Blood
					{
						damage=0.1;
					};
					class Shock
					{
						damage=0.1;
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
	class TGK_FSB_ALPHA_Pants_Alpine: TGK_FSB_ALPHA_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\Pants\data\Pants_Alpine.paa"
		};
	};
	class TGK_FSB_ALPHA_Pants_Black: TGK_FSB_ALPHA_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\Pants\data\Pants_Black.paa"
		};
	};
	class TGK_FSB_ALPHA_Pants_Black_MC: TGK_FSB_ALPHA_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\Pants\data\Pants_Black_MC.paa"
		};
	};
	class TGK_FSB_ALPHA_Pants_MC: TGK_FSB_ALPHA_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\Pants\data\Pants_MC.paa"
		};
	};
	class TGK_FSB_ALPHA_Pants_ODGreen: TGK_FSB_ALPHA_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\Pants\data\Pants_ODGreen.paa"
		};
	};
	class TGK_FSB_ALPHA_Pants_Tropic_MC: TGK_FSB_ALPHA_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\Pants\data\Pants_Tropic_MC.paa"
		};
	};
};
