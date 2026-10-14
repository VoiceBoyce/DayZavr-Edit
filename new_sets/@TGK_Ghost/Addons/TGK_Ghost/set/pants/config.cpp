class CfgPatches
{
	class TGK_Ghost_Pants
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
	class GorkaPants_ColorBase;
	class TGK_Ghost_Pants_ColorBase: GorkaPants_ColorBase
	{
		scope=0;
		displayName="$STR_Ghost_Pants01";
		descriptionShort="$STR_Ghost_Pants02";
		model="TGK_Ghost\set\pants\Pants_g.p3d";
		ContinuouActions[]=
		{
			"AT_WRING_CLOTHES"
		};
		inventorySlot[]=
		{
			"Legs"
		};
		simulation="clothing";
		itemInfo[]=
		{
			"Clothing",
			"Legs"
		};
		attachments[]=
		{
			"Pistol"
		};
		itemSize[]={3,4};
		itemsCargoSize[]={5,4};
		quickBarBonus=2;
		repairableWithKits[]={5,3};
		heatIsolation=0.69999999;
		ragQuantity=5;
		repairCosts[]={25,25};
		absorbency=0.5;
		varWetMax=0.85000002;
		weight=1000;
		hiddenSelections[]=
		{
			"camo",
			"personality"
		};
		rotationFlags=17;
		class ClothingTypes
		{
			male="TGK_Ghost\set\pants\Pants_m.p3d";
			female="TGK_Ghost\set\pants\Pants_m.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=155;
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
						damage=0.85000002;
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
						damage=0.64999998;
					};
					class Blood
					{
						damage=0.44999999;
					};
					class Shock
					{
						damage=0.5;
					};
				};
				class Infected
				{
					class Health
					{
						damage=0.64999998;
					};
					class Blood
					{
						damage=0.44999999;
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
	class TGK_Ghost_Pants_Blue: TGK_Ghost_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_Blue.paa"
		};
	};
	class TGK_Ghost_Pants_ClearSky: TGK_Ghost_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_ClearSky.paa"
		};
	};
	class TGK_Ghost_Pants_Black: TGK_Ghost_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_co.paa"
		};
	};
	class TGK_Ghost_Pants_DarkGreen: TGK_Ghost_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_DarkGreen.paa"
		};
	};
	class TGK_Ghost_Pants_DarkRed: TGK_Ghost_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_DarkRed.paa"
		};
	};
	class TGK_Ghost_Pants_EMRSummer: TGK_Ghost_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_EMRSummer.paa"
		};
	};
	class TGK_Ghost_Pants_Gray: TGK_Ghost_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_Gray_co.paa"
		};
	};
	class TGK_Ghost_Pants_GrayCamo: TGK_Ghost_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_GrayCamo.paa"
		};
	};
	class TGK_Ghost_Pants_LightGreen: TGK_Ghost_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_LightGreen.paa"
		};
	};
	class TGK_Ghost_Pants_LitPat: TGK_Ghost_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_LitPat.paa"
		};
	};
	class TGK_Ghost_Pants_Merc: TGK_Ghost_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_Merc_co.paa"
		};
	};
	class TGK_Ghost_Pants_Osenniy: TGK_Ghost_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_Osenniy.paa"
		};
	};
	class TGK_Ghost_Pants_QC1: TGK_Ghost_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_QC1.paa"
		};
	};
	class TGK_Ghost_Pants_Sans: TGK_Ghost_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_Sans.paa"
		};
	};
	class TGK_Ghost_Pants_White: TGK_Ghost_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_White.paa"
		};
	};
	class TGK_Ghost_Pants_Yellow: TGK_Ghost_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\texture\MainTexture_Yellow.paa"
		};
	};
};
