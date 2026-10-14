class CfgPatches
{
	class TGK_Ghost_Boots
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Characters",
			"DZ_Characters_Shoes"
		};
	};
};
class CfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base
	{
	};
	class CombatBoots_ColorBase;
	class TGK_Ghost_Boots_ColorBase: CombatBoots_ColorBase
	{
		displayName="$STR_Ghost_Boots01";
		descriptionShort="$STR_Ghost_Boots02";
		model="TGK_Ghost\set\boots\Boots_g.p3d";
		inventorySlot[]=
		{
			"Feet"
		};
		itemInfo[]=
		{
			"Clothing",
			"Feet"
		};
		itemSize[]={4,2};
		weight=1200;
		durability=0.34999999;
		varWetMax=0.25;
		heatIsolation=0.64999998;
		visibilityModifier=0.89999998;
		repairableWithKits[]={3};
		repairCosts[]={25};
		soundAttType="Sneakers";
		soundImpactType="default";
		hiddenSelections[]=
		{
			"camo"
		};
		rotationFlags=16;
		class ClothingTypes
		{
			male="TGK_Ghost\set\boots\Boots_m.p3d";
			female="TGK_Ghost\set\boots\Boots_m.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=350;
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
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem
				{
					soundSet="AthleticShoes_pickup_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="AthleticShoes_drop_SoundSet";
					id=898;
				};
			};
		};
	};
	class TGK_Ghost_Boots_Blue: TGK_Ghost_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_Blue.paa"
		};
	};
	class TGK_Ghost_Boots_ClearSky: TGK_Ghost_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_ClearSky.paa"
		};
	};
	class TGK_Ghost_Boots_Black: TGK_Ghost_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_co.paa"
		};
	};
	class TGK_Ghost_Boots_DarkGreen: TGK_Ghost_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_DarkGreen.paa"
		};
	};
	class TGK_Ghost_Boots_DarkRed: TGK_Ghost_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_DarkRed.paa"
		};
	};
	class TGK_Ghost_Boots_EMRSummer: TGK_Ghost_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_EMRSummer.paa"
		};
	};
	class TGK_Ghost_Boots_Gray: TGK_Ghost_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_Gray_co.paa"
		};
	};
	class TGK_Ghost_Boots_GrayCamo: TGK_Ghost_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_GrayCamo.paa"
		};
	};
	class TGK_Ghost_Boots_LightGreen: TGK_Ghost_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_LightGreen.paa"
		};
	};
	class TGK_Ghost_Boots_LitPat: TGK_Ghost_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_LitPat.paa"
		};
	};
	class TGK_Ghost_Boots_Merc: TGK_Ghost_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_Merc_co.paa"
		};
	};
	class TGK_Ghost_Boots_Osenniy: TGK_Ghost_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_Osenniy.paa"
		};
	};
	class TGK_Ghost_Boots_QC1: TGK_Ghost_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_QC1.paa"
		};
	};
	class TGK_Ghost_Boots_Sans: TGK_Ghost_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_Sans.paa"
		};
	};
	class TGK_Ghost_Boots_White: TGK_Ghost_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_White.paa"
		};
	};
	class TGK_Ghost_Boots_Yellow: TGK_Ghost_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_Yellow.paa"
		};
	};
};
