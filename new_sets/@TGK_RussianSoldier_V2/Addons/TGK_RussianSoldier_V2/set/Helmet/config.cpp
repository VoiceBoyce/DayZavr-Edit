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
	class Mich2001Helmet;
	class TGK_RussianSoldier_v2_Helmet_ColorBase: Mich2001Helmet
	{
		scope=0;
		displayName="$STR_RusV2_Helmet01";
		descriptionShort="$STR_RusV2_Helmet02";
		model="TGK_RussianSoldier_V2\set\Helmet\models\Helmet_g.p3d";
		repairableWithKits[]={8};
		repairCosts[]={25};
		inventorySlot[]=
		{
			"Headgear",
			"Helmet"
		};
		attachments[]=
		{
			"helmetFlashlight",
			"NVG"
		};
		simulation="clothing";
		vehicleClass="Clothing";
		itemInfo[]=
		{
			"Clothing",
			"Headgear"
		};
		weight=1300;
		quickBarBonus=0;
		itemSize[]={4,4};
		noMask=0;
		noNVStrap=1;
		noGlasses=1;
		varWetMax=0.1;
		heatIsolation=0.34999999;
		headSelectionsToHide[]=
		{
			"Clipping_Mich2001"
		};
		hiddenSelections[]=
		{
			"camo",
			"chinstrap"
		};
		rotationFlags=8;
		class ClothingTypes
		{
			male="TGK_RussianSoldier_V2\set\Helmet\models\Helmet_m.p3d";
			female="TGK_RussianSoldier_V2\set\Helmet\models\Helmet_m.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=200;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"TGK_RussianSoldier_V2\set\Helmet\data\6B47.rvmat",
								"TGK_RussianSoldier_V2\set\Helmet\data\Chinstrap\Chinstrap.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_RussianSoldier_V2\set\Helmet\data\6B47.rvmat",
								"TGK_RussianSoldier_V2\set\Helmet\data\Chinstrap\Chinstrap.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_RussianSoldier_V2\set\Helmet\data\6B47_damage.rvmat",
								"TGK_RussianSoldier_V2\set\Helmet\data\Chinstrap\Chinstrap_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_RussianSoldier_V2\set\Helmet\data\6B47_damage.rvmat",
								"TGK_RussianSoldier_V2\set\Helmet\data\Chinstrap\Chinstrap_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_RussianSoldier_V2\set\Helmet\data\6B47_destruct.rvmat",
								"TGK_RussianSoldier_V2\set\Helmet\data\Chinstrap\Chinstrap_destruct.rvmat"
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
				class Infected
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
				class FragGrenade
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
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem
				{
					soundSet="pickUpPot_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="BallisticHelmet_drop_SoundSet";
					id=898;
				};
			};
		};
	};
	class TGK_RussianSoldier_v2_Helmet_Alpine: TGK_RussianSoldier_v2_Helmet_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V2\set\Helmet\data\6B47_Alpine.paa",
			"TGK_RussianSoldier_V2\set\Helmet\data\Chinstrap\Chinstrap_Alpine.paa"
		};
	};
	class TGK_RussianSoldier_v2_Helmet_Black: TGK_RussianSoldier_v2_Helmet_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V2\set\Helmet\data\6B47_Black.paa",
			"TGK_RussianSoldier_V2\set\Helmet\data\Chinstrap\Chinstrap_Black.paa"
		};
	};
	class TGK_RussianSoldier_v2_Helmet_Black_MC: TGK_RussianSoldier_v2_Helmet_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V2\set\Helmet\data\6B47_Black_MC.paa",
			"TGK_RussianSoldier_V2\set\Helmet\data\Chinstrap\Chinstrap_Black_MC.paa"
		};
	};
	class TGK_RussianSoldier_v2_Helmet_EMR: TGK_RussianSoldier_v2_Helmet_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V2\set\Helmet\data\6B47_EMR.paa",
			"TGK_RussianSoldier_V2\set\Helmet\data\Chinstrap\Chinstrap_EMR.paa"
		};
	};
	class TGK_RussianSoldier_v2_Helmet_MC: TGK_RussianSoldier_v2_Helmet_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V2\set\Helmet\data\6B47_MC.paa",
			"TGK_RussianSoldier_V2\set\Helmet\data\Chinstrap\Chinstrap_MC.paa"
		};
	};
};
