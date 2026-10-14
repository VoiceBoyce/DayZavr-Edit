class CfgPatches
{
	class TGK_Ghost_Top
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
	class GorkaEJacket_ColorBase;
	class TGK_Ghost_Top_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_Ghost_Top01";
		descriptionShort="$STR_Ghost_Top02";
		model="TGK_Ghost\set\top\Top_g.p3d";
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
		itemsCargoSize[]={10,5};
		repairableWithKits[]={2,3};
		repairCosts[]={75,50};
		hiddenSelections[]=
		{
			"camo"
		};
		rotationFlags=17;
		class ClothingTypes
		{
			male="TGK_Ghost\set\top\Top_m.p3d";
			female="TGK_Ghost\set\top\Top_m.p3d";
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
								"TGK_Ghost\set\texture\MainTexture.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_Ghost\set\texture\MainTexture.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_Ghost\set\texture\MainTexture_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_Ghost\set\texture\MainTexture_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_Ghost\set\texture\MainTexture_destruct.rvmat"
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
						damage=0.89999998;
					};
					class Blood
					{
						damage=0.89999998;
					};
					class Shock
					{
						damage=0.89999998;
					};
				};
				class Melee
				{
					class Health
					{
						damage=0.44999999;
					};
					class Blood
					{
						damage=0.44999999;
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
						damage=0.5;
					};
					class Blood
					{
						damage=0.5;
					};
					class Shock
					{
						damage=0.5;
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
	class TGK_Ghost_Top_Blue: TGK_Ghost_Top_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_Blue.paa"
		};
	};
	class TGK_Ghost_Top_ClearSky: TGK_Ghost_Top_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_ClearSky.paa"
		};
	};
	class TGK_Ghost_Top_Black: TGK_Ghost_Top_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_co.paa"
		};
	};
	class TGK_Ghost_Top_DarkGreen: TGK_Ghost_Top_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_DarkGreen.paa"
		};
	};
	class TGK_Ghost_Top_DarkRed: TGK_Ghost_Top_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_DarkRed.paa"
		};
	};
	class TGK_Ghost_Top_EMRSummer: TGK_Ghost_Top_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_EMRSummer.paa"
		};
	};
	class TGK_Ghost_Top_Gray: TGK_Ghost_Top_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_Gray_co.paa"
		};
	};
	class TGK_Ghost_Top_GrayCamo: TGK_Ghost_Top_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_GrayCamo.paa"
		};
	};
	class TGK_Ghost_Top_LightGreen: TGK_Ghost_Top_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_LightGreen.paa"
		};
	};
	class TGK_Ghost_Top_LitPat: TGK_Ghost_Top_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_LitPat.paa"
		};
	};
	class TGK_Ghost_Top_Merc: TGK_Ghost_Top_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_Merc_co.paa"
		};
	};
	class TGK_Ghost_Top_Osenniy: TGK_Ghost_Top_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_Osenniy.paa"
		};
	};
	class TGK_Ghost_Top_QC1: TGK_Ghost_Top_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_QC1.paa"
		};
	};
	class TGK_Ghost_Top_Sans: TGK_Ghost_Top_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_Sans.paa"
		};
	};
	class TGK_Ghost_Top_White: TGK_Ghost_Top_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_White.paa"
		};
	};
	class TGK_Ghost_Top_Yellow: TGK_Ghost_Top_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_Yellow.paa"
		};
	};
};
