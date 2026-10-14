class CfgPatches
{
	class Stalker_2_Monolith_Balaclava
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
	class BalaclavaMask_ColorBase;
	class Stalker_2_Monolith_Balaclava_ColorBase: BalaclavaMask_ColorBase
	{
		scope=0;
		displayName="$STR_Stalker_2_Monolith_Balaclava_01";
		descriptionShort="$STR_Stalker_2_Monolith_Balaclava_02";
		inventorySlot[]=
		{
			"Mask"
		};
		itemInfo[]=
		{
			"Clothing",
			"Mask"
		};
		rotationFlags=17;
		weight=300;
		itemSize[]={2,2};
		ragQuantity=2;
		varWetMax=0.34999999;
		heatIsolation=0.5;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		model="Stalker2_Monolith_01\set\balaclava\models\balaclava_g.p3d";
		headSelectionsToHide[]=
		{
			"Clipping_Gasmask",
			"Clipping_Balaclava_3holes"
		};
		hiddenSelections[]=
		{
			"camo"
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
								"Stalker2_Monolith_01\set\balaclava\data\balaclava.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"Stalker2_Monolith_01\set\balaclava\data\balaclava.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"Stalker2_Monolith_01\set\balaclava\data\balaclava_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"Stalker2_Monolith_01\set\balaclava\data\balaclava_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"Stalker2_Monolith_01\set\balaclava\data\balaclava_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		class ClothingTypes
		{
			male="Stalker2_Monolith_01\set\balaclava\models\balaclava_m.p3d";
			female="Stalker2_Monolith_01\set\balaclava\models\balaclava_m.p3d";
		};
		class Protection
		{
			biological=0.25;
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
	class S2_Monolith_01_Balaclava: Stalker_2_Monolith_Balaclava_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\balaclava\data\balaclava_co.paa"
		};
	};
	class S2_Monolith_01_Balaclava_ClearSky: Stalker_2_Monolith_Balaclava_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\balaclava\data\balaclava_ClearSky.paa"
		};
	};
	class S2_Monolith_01_Balaclava_Duty1: Stalker_2_Monolith_Balaclava_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\balaclava\data\balaclava_Duty1.paa"
		};
	};
	class S2_Monolith_01_Balaclava_Duty2: Stalker_2_Monolith_Balaclava_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\balaclava\data\balaclava_Duty2.paa"
		};
	};
	class S2_Monolith_01_Balaclava_Freedom1: Stalker_2_Monolith_Balaclava_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\balaclava\data\balaclava_Freedom1.paa"
		};
	};
	class S2_Monolith_01_Balaclava_Freedom2: Stalker_2_Monolith_Balaclava_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\balaclava\data\balaclava_Freedom2.paa"
		};
	};
	class S2_Monolith_01_Balaclava_Korpus: Stalker_2_Monolith_Balaclava_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\balaclava\data\balaclava_Korpus.paa"
		};
	};
	class S2_Monolith_01_Balaclava_Merc: Stalker_2_Monolith_Balaclava_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\balaclava\data\balaclava_Merc.paa"
		};
	};
	class S2_Monolith_01_Balaclava_MSOP: Stalker_2_Monolith_Balaclava_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\balaclava\data\balaclava_MSOP.paa"
		};
	};
	class S2_Monolith_01_Balaclava_Neutrals: Stalker_2_Monolith_Balaclava_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\balaclava\data\balaclava_Neutrals.paa"
		};
	};
	class S2_Monolith_01_Balaclava_Warta1: Stalker_2_Monolith_Balaclava_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\balaclava\data\balaclava_Warta1.paa"
		};
	};
	class S2_Monolith_01_Balaclava_Warta2: Stalker_2_Monolith_Balaclava_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\balaclava\data\balaclava_Warta2.paa"
		};
	};
};
