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
	class TGK_FSB_ALPHA_Helmet_ColorBase: Mich2001Helmet
	{
		scope=0;
		displayName="$STR_FSB_ALPHA_Helmet_01";
		descriptionShort="$STR_FSB_ALPHA_Helmet_02";
		model="TGK_FSB_Alpha\set\Helmet\Helmet_g.p3d";
		repairableWithKits[]={8};
		repairCosts[]={25};
		inventorySlot[]=
		{
			"Headgear",
			"Helmet"
		};
		attachments[]=
		{
			"NVG",
			"helmetFlashlight",
			"Helmet_Cover",
			"BackPouch",
			"Helmet_Headphones"
		};
		simulation="clothing";
		vehicleClass="Clothing";
		itemInfo[]=
		{
			"Clothing",
			"Headgear"
		};
		weight=2600;
		quickBarBonus=0;
		itemSize[]={4,4};
		noMask=0;
		noNVStrap=0;
		noEyewear=0;
		varWetMax=0.1;
		heatIsolation=0.34999999;
		headSelectionsToHide[]=
		{
			"Clipping_Mich2001"
		};
		hiddenSelections[]=
		{
			"camo"
		};
		rotationFlags=8;
		class ClothingTypes
		{
			male="TGK_FSB_Alpha\set\Helmet\Helmet_m.p3d";
			female="TGK_FSB_Alpha\set\Helmet\Helmet_m.p3d";
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
								"TGK_FSB_Alpha\set\Helmet\Helmet\LSHZ_Helmet.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_FSB_Alpha\set\Helmet\Helmet\LSHZ_Helmet.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_FSB_Alpha\set\Helmet\Helmet\LSHZ_Helmet_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_FSB_Alpha\set\Helmet\Helmet\LSHZ_Helmet_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_FSB_Alpha\set\Helmet\Helmet\LSHZ_Helmet_destruct.rvmat"
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
	class TGK_FSB_ALPHA_Helmet_Black: TGK_FSB_ALPHA_Helmet_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\Helmet\Helmet\LSHZ_Helmet_Black.paa"
		};
	};
};
