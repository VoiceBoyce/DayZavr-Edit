class CfgPatches
{
	class TGK_Burya_Pants
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
class cfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base
	{
	};
	class GorkaPants_ColorBase;
	class TGK_Burya_Pants_ColorBase: GorkaPants_ColorBase
	{
		scope=0;
		displayName="$STR_TGK_Burya_Pants_01";
		descriptionShort="$STR_TGK_Burya_Pants_02";
		model="TGK_BuryaSet\Set\pants\models\Pants_g.p3d";
		ContinuouActions[]=
		{
			"AT_WRING_CLOTHES"
		};
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
		itemsCargoSize[]={10,5};
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
			male="TGK_BuryaSet\Set\pants\models\Pants_m.p3d";
			female="TGK_BuryaSet\Set\pants\models\Pants_m.p3d";
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
								"TGK_BuryaSet\Set\pants\data\Pants.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_BuryaSet\Set\pants\data\Pants.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_BuryaSet\Set\pants\data\Pants_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_BuryaSet\Set\pants\data\Pants_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_BuryaSet\Set\pants\data\Pants_destruct.rvmat"
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
	class TGK_Pants_Burya_Berezka: TGK_Burya_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\pants\data\Pants_Berezka_co.paa"
		};
	};
	class TGK_Pants_Burya_Black: TGK_Burya_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\pants\data\Pants_Black_co.paa"
		};
	};
	class TGK_Pants_Burya_ClearSky: TGK_Burya_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\pants\data\Pants_ClearSky_co.paa"
		};
	};
	class TGK_Pants_Burya_LitPat: TGK_Burya_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\pants\data\Pants_LitPat_co.paa"
		};
	};
	class TGK_Pants_Burya_Monolith: TGK_Burya_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\pants\data\Pants_Monolith_co.paa"
		};
	};
	class TGK_Pants_Burya_QC1: TGK_Burya_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\pants\data\Pants_QC1_co.paa"
		};
	};
	class TGK_Pants_Burya_Sans: TGK_Burya_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\pants\data\Pants_Sans_co.paa"
		};
	};
	class TGK_Pants_Burya_DarkGreen: TGK_Burya_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\pants\data\Pants_DarkGreen_co.paa"
		};
	};
	class TGK_Pants_Burya_DarkRed: TGK_Burya_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\pants\data\Pants_DarkRed_co.paa"
		};
	};
	class TGK_Pants_Burya_LightGreen: TGK_Burya_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\pants\data\Pants_LightGreen_co.paa"
		};
	};
	class TGK_Pants_Burya_OSENNIY: TGK_Burya_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\pants\data\Pants_OSENNIY_co.paa"
		};
	};
	class TGK_Pants_Burya_White: TGK_Burya_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\pants\data\Pants_White_co.paa"
		};
	};
};
