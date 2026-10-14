class CfgPatches
{
	class TGK_RussianSoldier_v2
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Characters",
			"DZ_Data"
		};
	};
};
class CfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base
	{
	};
	class TGK_RussianSoldier_v2_Top_Base: Clothing
	{
		scope=0;
		displayName="$STR_RusV2_Top01";
		descriptionShort="$STR_RusV2_Top02";
		model="TGK_RussianSoldier_V2\set\Top\models\Top_g.p3d";
		inventorySlot[]=
		{
			"Body"
		};
		itemInfo[]=
		{
			"Clothing",
			"Body"
		};
		weight=1000;
		itemSize[]={4,6};
		quickBarBonus=2;
		ragQuantity=3;
		varWetMax=0.249;
		heatIsolation=0.40000001;
		itemsCargoSize[]={5,5};
		repairableWithKits[]={2,3};
		repairCosts[]={75,50};
		hiddenSelections[]=
		{
			"camo",
			"pads"
		};
		rotationFlags=17;
		class ClothingTypes
		{
			male="TGK_RussianSoldier_V2\set\Top\models\Top_m.p3d";
			female="TGK_RussianSoldier_V2\set\Top\models\Top_m.p3d";
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
								"TGK_RussianSoldier_V2\set\Top\data\Top.rvmat",
								"TGK_RussianSoldier_V2\set\pads\Pads.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_RussianSoldier_V2\set\Top\data\Top.rvmat",
								"TGK_RussianSoldier_V2\set\pads\Pads.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_RussianSoldier_V2\set\Top\data\Top_damage.rvmat",
								"TGK_RussianSoldier_V2\set\pads\Pads_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_RussianSoldier_V2\set\Top\data\Top_damage.rvmat",
								"TGK_RussianSoldier_V2\set\pads\Pads_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_RussianSoldier_V2\set\Top\data\Top_destruct.rvmat",
								"TGK_RussianSoldier_V2\set\pads\Pads_destruct.rvmat"
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
	class TGK_RussianSoldier_v2_VKPO_Alpine: TGK_RussianSoldier_v2_Top_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V2\set\Top\data\Top_Alpine.paa",
			"TGK_RussianSoldier_V2\set\pads\Pads_Alpine.paa"
		};
	};
	class TGK_RussianSoldier_v2_VKPO_Black: TGK_RussianSoldier_v2_Top_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V2\set\Top\data\Top_Black.paa",
			"TGK_RussianSoldier_V2\set\pads\Pads_Black.paa"
		};
	};
	class TGK_RussianSoldier_v2_VKPO_Black_MC: TGK_RussianSoldier_v2_Top_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V2\set\Top\data\Top_Black_MC.paa",
			"TGK_RussianSoldier_V2\set\pads\Pads_Black_MC.paa"
		};
	};
	class TGK_RussianSoldier_v2_VKPO_EMR: TGK_RussianSoldier_v2_Top_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V2\set\Top\data\Top_EMR.paa",
			"TGK_RussianSoldier_V2\set\pads\Pads_EMR.paa"
		};
	};
	class TGK_RussianSoldier_v2_VKPO_MC: TGK_RussianSoldier_v2_Top_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V2\set\Top\data\Top_MC.paa",
			"TGK_RussianSoldier_V2\set\pads\Pads_MC.paa"
		};
	};
};
