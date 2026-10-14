class CfgPatches
{
	class TGK_S2_Warta
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
	class TGK_S2_Warta_GasHelmet_Colorbase: Clothing
	{
		scope=0;
		displayName="$STR_WartaHelmet_01";
		descriptionShort="$STR_WartaHelmet_02";
		model="TGK_S2_Warta\set\Gasmask\models\Warta_Helmet_g.p3d";
		repairableWithKits[]={8};
		repairCosts[]={25};
		inventorySlot[]=
		{
			"Headgear",
			"Helmet"
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
		varWetMax=0.048999999;
		heatIsolation=0.85000002;
		headSelectionsToHide[]=
		{
			"Clipping_Mich2001",
			"Clipping_Gasmask",
			"Clipping_Balaclava_3holes"
		};
		hiddenSelections[]=
		{
			"balaclava",
			"filters",
			"gazik",
			"helmet",
			"glass"
		};
		class ClothingTypes
		{
			male="TGK_S2_Warta\set\Gasmask\models\Warta_Helmet_m.p3d";
			female="TGK_S2_Warta\set\Gasmask\models\Warta_Helmet_m.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=300;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"TGK_S2_Warta\set\Gasmask\balaclava\balaclava.rvmat",
								"TGK_S2_Warta\set\Gasmask\filters\filters.rvmat",
								"TGK_S2_Warta\set\Gasmask\gazik\gazik.rvmat",
								"TGK_S2_Warta\set\Gasmask\helmet\helmet.rvmat",
								"TGK_S2_Warta\set\Gasmask\glass\glass.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_S2_Warta\set\Gasmask\balaclava\balaclava.rvmat",
								"TGK_S2_Warta\set\Gasmask\filters\filters.rvmat",
								"TGK_S2_Warta\set\Gasmask\gazik\gazik.rvmat",
								"TGK_S2_Warta\set\Gasmask\helmet\helmet.rvmat",
								"TGK_S2_Warta\set\Gasmask\glass\glass.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_S2_Warta\set\Gasmask\balaclava\balaclava_damage.rvmat",
								"TGK_S2_Warta\set\Gasmask\filters\filters_damage.rvmat",
								"TGK_S2_Warta\set\Gasmask\gazik\gazik_damage.rvmat",
								"TGK_S2_Warta\set\Gasmask\helmet\helmet_damage.rvmat",
								"TGK_S2_Warta\set\Gasmask\glass\glass_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_S2_Warta\set\Gasmask\balaclava\balaclava_damage.rvmat",
								"TGK_S2_Warta\set\Gasmask\filters\filters_damage.rvmat",
								"TGK_S2_Warta\set\Gasmask\gazik\gazik_damage.rvmat",
								"TGK_S2_Warta\set\Gasmask\helmet\helmet_damage.rvmat",
								"TGK_S2_Warta\set\Gasmask\glass\glass_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_S2_Warta\set\Gasmask\balaclava\balaclava_destruct.rvmat",
								"TGK_S2_Warta\set\Gasmask\filters\filters_destruct.rvmat",
								"TGK_S2_Warta\set\Gasmask\gazik\gazik_destruct.rvmat",
								"TGK_S2_Warta\set\Gasmask\helmet\helmet_destruct.rvmat",
								"TGK_S2_Warta\set\Gasmask\glass\glass_destruct.rvmat"
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
	class TGK_S2_Warta_GasHelmet: TGK_S2_Warta_GasHelmet_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_S2_Warta\set\Gasmask\balaclava\balaclava_co.paa",
			"TGK_S2_Warta\set\Gasmask\filters\filters_ca.paa",
			"TGK_S2_Warta\set\Gasmask\gazik\gazik_co.paa",
			"TGK_S2_Warta\set\Gasmask\helmet\helmet_co.paa",
			"TGK_S2_Warta\set\Gasmask\glass\glass_ca.paa"
		};
	};
};
