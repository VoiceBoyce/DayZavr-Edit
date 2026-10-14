class CfgPatches
{
	class S2_Duty_01
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Characters",
			"DZ_Scripts",
			"DZ_Characters_Headgear",
			"DZ_Characters_Masks"
		};
	};
};
class CfgVehicles
{
	class Inventory_Base;
	class Clothing_Base;
	class Clothing: Clothing_Base
	{
	};
	class GP5GasMask;
	class Stalker_2_Duty_01_Helmet_Colorbase: GP5GasMask
	{
		scope=0;
		displayName="$STR_Duty_01_Helmet_01";
		descriptionShort="$STR_Duty_01_Helmet_02";
		model="Stalker2_Duty_01\set\helmet\models\Helmet_g.p3d";
		inventorySlot[]=
		{
			"Headgear",
			"Helmet"
		};
		attachments[]=
		{
			"helmetFlashlight",
			"NVG",
			"S2_Duty_01_GasMaskFilter"
		};
		simulation="clothing";
		vehicleClass="Clothing";
		itemInfo[]=
		{
			"Clothing",
			"Headgear"
		};
		rotationFlags=1;
		itemSize[]={3,3};
		weight=1090;
		varWetMax=0.249;
		heatIsolation=0.69999999;
		repairableWithKits[]={8,6};
		repairCosts[]={30,25};
		visibilityModifier=0.94999999;
		headSelectionsToHide[]=
		{
			"Clipping_GP5GasMask"
		};
		hiddenSelections[]=
		{
			"camo_balaclava",
			"camo_main",
			"camo_glass"
		};
		class ClothingTypes
		{
			male="Stalker2_Duty_01\set\helmet\models\Helmet_m.p3d";
			female="Stalker2_Duty_01\set\helmet\models\Helmet_m.p3d";
		};
		class Protection
		{
			biological=1;
			chemical=1;
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
								"Stalker2_Duty_01\set\helmet\balaclava\balaclava.rvmat",
								"Stalker2_Duty_01\set\helmet\main\Main.rvmat",
								"Stalker2_Duty_01\set\helmet\glass\Glass.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"Stalker2_Duty_01\set\helmet\balaclava\balaclava.rvmat",
								"Stalker2_Duty_01\set\helmet\main\Main.rvmat",
								"Stalker2_Duty_01\set\helmet\glass\Glass.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"Stalker2_Duty_01\set\helmet\balaclava\balaclava_damage.rvmat",
								"Stalker2_Duty_01\set\helmet\main\Main_damage.rvmat",
								"Stalker2_Duty_01\set\helmet\glass\Glass_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"Stalker2_Duty_01\set\helmet\balaclava\balaclava_damage.rvmat",
								"Stalker2_Duty_01\set\helmet\main\Main_damage.rvmat",
								"Stalker2_Duty_01\set\helmet\glass\Glass_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"Stalker2_Duty_01\set\helmet\balaclava\balaclava_destruct.rvmat",
								"Stalker2_Duty_01\set\helmet\main\Main_destruct.rvmat",
								"Stalker2_Duty_01\set\helmet\glass\Glass_destruct.rvmat"
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
	class Stalker_2_Duty_01_Helmet: Stalker_2_Duty_01_Helmet_Colorbase
	{
		scope=2;
		attachments[]=
		{
			"helmetFlashlight",
			"NVG",
			"S2_Duty_01_GasMaskFilter"
		};
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\helmet\balaclava\balaclava_co.paa",
			"Stalker2_Duty_01\set\helmet\main\Main_ca.paa",
			"Stalker2_Duty_01\set\helmet\glass\Glass1_ca.paa"
		};
	};
	class Stalker_2_Duty_01_Helmet_ClearSky: Stalker_2_Duty_01_Helmet_Colorbase
	{
		scope=2;
		attachments[]=
		{
			"helmetFlashlight",
			"NVG",
			"S2_Duty_01_GasMaskFilter"
		};
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\helmet\balaclava\balaclava_ClearSky.paa",
			"Stalker2_Duty_01\set\helmet\main\Main_ClearSky.paa",
			"Stalker2_Duty_01\set\helmet\glass\Glass1_ca.paa"
		};
	};
	class Stalker_2_Duty_01_Helmet_Korpus: Stalker_2_Duty_01_Helmet_Colorbase
	{
		scope=2;
		attachments[]=
		{
			"helmetFlashlight",
			"NVG",
			"S2_Duty_01_GasMaskFilter"
		};
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\helmet\balaclava\balaclava_Korpus.paa",
			"Stalker2_Duty_01\set\helmet\main\Main_Korpus.paa",
			"Stalker2_Duty_01\set\helmet\glass\Glass1_ca.paa"
		};
	};
	class Stalker_2_Duty_01_Helmet_Monolith: Stalker_2_Duty_01_Helmet_Colorbase
	{
		scope=2;
		attachments[]=
		{
			"helmetFlashlight",
			"NVG",
			"S2_Duty_01_GasMaskFilter"
		};
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\helmet\balaclava\balaclava_Monolith.paa",
			"Stalker2_Duty_01\set\helmet\main\Main_Monolith.paa",
			"Stalker2_Duty_01\set\helmet\glass\Glass1_ca.paa"
		};
	};
	class Stalker_2_Duty_01_Helmet_MSOP: Stalker_2_Duty_01_Helmet_Colorbase
	{
		scope=2;
		attachments[]=
		{
			"helmetFlashlight",
			"NVG",
			"S2_Duty_01_GasMaskFilter"
		};
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\helmet\balaclava\balaclava_MSOP.paa",
			"Stalker2_Duty_01\set\helmet\main\Main_MSOP.paa",
			"Stalker2_Duty_01\set\helmet\glass\Glass1_ca.paa"
		};
	};
	class Stalker_2_Duty_01_Helmet_Warta: Stalker_2_Duty_01_Helmet_Colorbase
	{
		scope=2;
		attachments[]=
		{
			"helmetFlashlight",
			"NVG",
			"S2_Duty_01_GasMaskFilter"
		};
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\helmet\balaclava\balaclava_Warta.paa",
			"Stalker2_Duty_01\set\helmet\main\Main_Warta.paa",
			"Stalker2_Duty_01\set\helmet\glass\Glass1_ca.paa"
		};
	};
	class GP5GasMask_Filter;
	class Stalker_2_Duty_01_Helmet_Filter_Colorbase: GP5GasMask_Filter
	{
		scope=0;
		displayName="$STR_Duty_01_Helmet_filter_01";
		descriptionShort="$STR_Duty_01_Helmet_filter_02";
		model="Stalker2_Duty_01\set\helmet\models\Helmet_filter.p3d";
		inventorySlot[]=
		{
			"S2_Duty_01_GasMaskFilter"
		};
		rotationFlags=2;
		itemSize[]={2,2};
		weight=150;
		varQuantityInit=150;
		varQuantityMin=0;
		varQuantityMax=150;
		quantityBar=1;
		varQuantityDestroyOnMin=0;
		varWetMax=0.249;
		heatIsolation=0;
		visibilityModifier=0.94999999;
		hiddenSelections[]=
		{
			"camo_main"
		};
		class Protection
		{
			biological=1;
			chemical=1;
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=100;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"Stalker2_Duty_01\set\helmet\main\Main.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"Stalker2_Duty_01\set\helmet\main\Main.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"Stalker2_Duty_01\set\helmet\main\Main_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"Stalker2_Duty_01\set\helmet\main\Main_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"Stalker2_Duty_01\set\helmet\main\Main_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		soundImpactType="metal";
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem
				{
					soundSet="DarkMotoHelmet_pickup_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="DarkMotoHelmet_drop_SoundSet";
					id=898;
				};
			};
		};
	};
	class Stalker_2_Duty_01_Helmet_Filter: Stalker_2_Duty_01_Helmet_Filter_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\helmet\main\Main_ca.paa"
		};
	};
	class Stalker_2_Duty_01_Helmet_Filter_ClearSky: Stalker_2_Duty_01_Helmet_Filter_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\helmet\main\Main_ClearSky.paa"
		};
	};
	class Stalker_2_Duty_01_Helmet_Filter_Korpus: Stalker_2_Duty_01_Helmet_Filter_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\helmet\main\Main_Korpus.paa"
		};
	};
	class Stalker_2_Duty_01_Helmet_Filter_Monolith: Stalker_2_Duty_01_Helmet_Filter_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\helmet\main\Main_Monolith.paa"
		};
	};
	class Stalker_2_Duty_01_Helmet_Filter_MSOP: Stalker_2_Duty_01_Helmet_Filter_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\helmet\main\Main_MSOP.paa"
		};
	};
	class Stalker_2_Duty_01_Helmet_Filter_Warta: Stalker_2_Duty_01_Helmet_Filter_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\helmet\main\Main_Warta.paa"
		};
	};
};
class CfgNonAIVehicles
{
	class ProxyAttachment;
	class ProxyHelmet_filter: ProxyAttachment
	{
		scope=2;
		inventorySlot="S2_Duty_01_GasMaskFilter";
		model="Stalker2_Duty_01\set\helmet\models\Helmet_filter.p3d";
	};
};
class CfgSlots
{
	class Slot_S2_Duty_01_GasMaskFilter
	{
		name="S2_Duty_01_GasMaskFilter";
		displayName="$STR_Duty_01_Helmet_filter_01";
		GhostIcon="set:dayz_inventory image:gasmaskfilter";
	};
};
