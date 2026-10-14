class CfgPatches
{
	class TGK_Burya_Jacket
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
class cfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base
	{
	};
	class GorkaEJacket_ColorBase;
	class TGK_Burya_Jacket_Colorbase: Clothing
	{
		scope=0;
		displayName="$STR_TGK_Burya_Jacket_01";
		descriptionShort="$STR_TGK_Burya_Jacket_02";
		model="TGK_BuryaSet\Set\jacket\models\Top_g.p3d";
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
		rotationFlags=17;
		hiddenSelections[]=
		{
			"camo",
			"camo1"
		};
		class ClothingTypes
		{
			male="TGK_BuryaSet\Set\jacket\models\Top_m.p3d";
			female="TGK_BuryaSet\Set\jacket\models\Top_m.p3d";
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
								"TGK_BuryaSet\Set\jacket\data\Jacket.rvmat",
								"TGK_BuryaSet\Set\kapushon\data\Hood.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_BuryaSet\Set\jacket\data\Jacket.rvmat",
								"TGK_BuryaSet\Set\kapushon\data\Hood.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_BuryaSet\Set\jacket\data\Jacket_damage.rvmat",
								"TGK_BuryaSet\Set\kapushon\data\Hood_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_BuryaSet\Set\jacket\data\Jacket_damage.rvmat",
								"TGK_BuryaSet\Set\kapushon\data\Hood_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_BuryaSet\Set\jacket\data\Jacket_destruct.rvmat",
								"TGK_BuryaSet\Set\kapushon\data\Hood_destruct.rvmat"
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
	class TGK_Burya_Jacket_Berezka: TGK_Burya_Jacket_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\jacket\data\Jacket_Berezka_co.paa",
			"TGK_BuryaSet\Set\kapushon\data\Hood_Berezka_co.paa"
		};
	};
	class TGK_Burya_Jacket_Black: TGK_Burya_Jacket_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\jacket\data\Jacket_Black_co.paa",
			"TGK_BuryaSet\Set\kapushon\data\Hood_Black_co.paa"
		};
	};
	class TGK_Burya_Jacket_ClearSky: TGK_Burya_Jacket_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\jacket\data\Jacket_ClearSky_co.paa",
			"TGK_BuryaSet\Set\kapushon\data\Hood_ClearSky_co.paa"
		};
	};
	class TGK_Burya_Jacket_LitPat: TGK_Burya_Jacket_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\jacket\data\Jacket_LitPat_co.paa",
			"TGK_BuryaSet\Set\kapushon\data\Hood_LitPat_co.paa"
		};
	};
	class TGK_Burya_Jacket_Monolith: TGK_Burya_Jacket_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\jacket\data\Jacket_Monolith_co.paa",
			"TGK_BuryaSet\Set\kapushon\data\Hood_Monolith_co.paa"
		};
	};
	class TGK_Burya_Jacket_QC1: TGK_Burya_Jacket_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\jacket\data\Jacket_QC1_co.paa",
			"TGK_BuryaSet\Set\kapushon\data\Hood_QC1_co.paa"
		};
	};
	class TGK_Burya_Jacket_Sans: TGK_Burya_Jacket_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\jacket\data\Jacket_Sans_co.paa",
			"TGK_BuryaSet\Set\kapushon\data\Hood_Sans_co.paa"
		};
	};
	class TGK_Burya_Jacket_DarkGreen: TGK_Burya_Jacket_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\jacket\data\Jacket_DarkGreen_co.paa",
			"TGK_BuryaSet\Set\kapushon\data\Hood_DarkGreen_co.paa"
		};
	};
	class TGK_Burya_Jacket_DarkRed: TGK_Burya_Jacket_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\jacket\data\Jacket_DarkRed_co.paa",
			"TGK_BuryaSet\Set\kapushon\data\Hood_DarkRed_co.paa"
		};
	};
	class TGK_Burya_Jacket_LightGreen: TGK_Burya_Jacket_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\jacket\data\Jacket_LightGreen_co.paa",
			"TGK_BuryaSet\Set\kapushon\data\Hood_LightGreen_co.paa"
		};
	};
	class TGK_Burya_Jacket_OSENNIY: TGK_Burya_Jacket_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\jacket\data\Jacket_OSENNIY_co.paa",
			"TGK_BuryaSet\Set\kapushon\data\Hood_OSENNIY_co.paa"
		};
	};
	class TGK_Burya_Jacket_White: TGK_Burya_Jacket_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_BuryaSet\Set\jacket\data\Jacket_White_co.paa",
			"TGK_BuryaSet\Set\kapushon\data\Hood_White_co.paa"
		};
	};
};
