class CfgPatches
{
	class TGK_Ghost_Poncho
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
	class TGK_Ghost_Poncho_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_Ghost_Poncho_01";
		descriptionShort="$STR_Ghost_Poncho_02";
		model="TGK_Ghost\set\poncho\models\poncho_g.p3d";
		varWetMax=0.60000002;
		inventorySlot[]=
		{
			"Back"
		};
		itemInfo[]=
		{
			"Clothing",
			"Back"
		};
		weight=700;
		itemSize[]={4,6};
		repairCosts[]={30,25};
		heatIsolation=0.1;
		repairableWithKits[]={2,3};
		absorbency=0.80000001;
		hiddenSelections[]=
		{
			"camo"
		};
		rotationFlags=17;
		class ClothingTypes
		{
			male="TGK_Ghost\set\poncho\models\poncho_m.p3d";
			female="TGK_Ghost\set\poncho\models\poncho_m.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=500;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"TGK_Ghost\set\poncho\data\Poncho.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_Ghost\set\poncho\data\Poncho.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_Ghost\set\poncho\data\Poncho_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_Ghost\set\poncho\data\Poncho_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_Ghost\set\poncho\data\Poncho_destruct.rvmat"
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
						damage=0.75;
					};
					class Blood
					{
						damage=1;
					};
					class Shock
					{
						damage=1;
					};
				};
				class Melee
				{
					class Health
					{
						damage=0.89999998;
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
				class Infected
				{
					class Health
					{
						damage=0.89999998;
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
				class FragGrenade
				{
					class Health
					{
						damage=1;
					};
					class Blood
					{
						damage=0.5;
					};
					class Shock
					{
						damage=1;
					};
				};
			};
			class AnimEvents
			{
				class SoundWeapon
				{
					class pickUpItem
					{
						soundSet="Shirt_pickup_SoundSet";
						id=797;
					};
					class drop
					{
						soundset="Shirt_drop_SoundSet";
						id=898;
					};
				};
			};
		};
	};
	class TGK_Ghost_Poncho_Blue: TGK_Ghost_Poncho_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\poncho\data\Poncho_Blue_co.paa"
		};
	};
	class TGK_Ghost_Poncho_ClearSky: TGK_Ghost_Poncho_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\poncho\data\Poncho_ClearSky_co.paa"
		};
	};
	class TGK_Ghost_Poncho_Black: TGK_Ghost_Poncho_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\poncho\data\Poncho_Black_co.paa"
		};
	};
	class TGK_Ghost_Poncho_DarkGreen: TGK_Ghost_Poncho_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\poncho\data\Poncho_DarkGreen_co.paa"
		};
	};
	class TGK_Ghost_Poncho_DarkRed: TGK_Ghost_Poncho_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\poncho\data\Poncho_Red_co.paa"
		};
	};
	class TGK_Ghost_Poncho_EMRSummer: TGK_Ghost_Poncho_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\poncho\data\Poncho_EMRSummer_co.paa"
		};
	};
	class TGK_Ghost_Poncho_Gray: TGK_Ghost_Poncho_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\poncho\data\Poncho_Gray_co.paa"
		};
	};
	class TGK_Ghost_Poncho_GrayCamo: TGK_Ghost_Poncho_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\poncho\data\Poncho_GrayCamo_co.paa"
		};
	};
	class TGK_Ghost_Poncho_LightGreen: TGK_Ghost_Poncho_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\poncho\data\Poncho_LightGreen_co.paa"
		};
	};
	class TGK_Ghost_Poncho_LitPat: TGK_Ghost_Poncho_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\poncho\data\Poncho_LitPat_co.paa"
		};
	};
	class TGK_Ghost_Poncho_Merc: TGK_Ghost_Poncho_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\poncho\data\Poncho_Merc_co.paa"
		};
	};
	class TGK_Ghost_Poncho_Osenniy: TGK_Ghost_Poncho_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\poncho\data\Poncho_OSENNIY_co.paa"
		};
	};
	class TGK_Ghost_Poncho_QC1: TGK_Ghost_Poncho_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\poncho\data\Poncho_QC1_co.paa"
		};
	};
	class TGK_Ghost_Poncho_Sans: TGK_Ghost_Poncho_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\poncho\data\Poncho_Sans_co.paa"
		};
	};
	class TGK_Ghost_Poncho_White: TGK_Ghost_Poncho_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\poncho\data\Poncho_White_co.paa"
		};
	};
	class TGK_Ghost_Poncho_Yellow: TGK_Ghost_Poncho_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\poncho\data\Poncho_Yellow_co.paa"
		};
	};
};
