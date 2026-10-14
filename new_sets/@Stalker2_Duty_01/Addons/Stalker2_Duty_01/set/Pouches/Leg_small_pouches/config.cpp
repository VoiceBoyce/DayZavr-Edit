class CfgPatches
{
	class S2_Duty_01
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Weapons_Magazines",
			"DZ_Weapons_Firearms",
			"DZ_Data",
			"DZ_Characters",
			"DZ_Scripts"
		};
	};
};
class CfgVehicles
{
	class Container_Base;
	class Stalker_2_Duty_01_Legsmallpouches_Colorbase: Container_Base
	{
		scope=0;
		displayName="$STR_Duty_01_Leg_small_pouches_01";
		descriptionShort="$STR_Duty_01_Leg_small_pouches_02";
		model="Stalker2_Duty_01\set\Pouches\Leg_small_pouches\Leg_small_pouches.p3d";
		inventorySlot[]=
		{
			"Leg_small_pouches_a",
			"Leg_small_pouches_b"
		};
		weight=200;
		itemSize[]={1,3};
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		hiddenSelections[]=
		{
			"camo"
		};
		randomQuantity=2;
		rotationFlags=17;
		itemsCargoSize[]={5,5};
		varWetMax=0.0099999998;
		heatIsolation=0.0099999998;
		lootTag[]=
		{
			"Military_east",
			"Military_west"
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
								"Stalker2_Duty_01\set\pants\data\pants.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"Stalker2_Duty_01\set\pants\data\pants.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"Stalker2_Duty_01\set\pants\data\pants_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"Stalker2_Duty_01\set\pants\data\pants_damage.rvmat"
							}
						},
						
						{
							0.0099999998,
							
							{
								"Stalker2_Duty_01\set\pants\data\pants_destruct.rvmat"
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
				class pickUpItem_Light
				{
					soundSet="pickUpCourierBag_Light_SoundSet";
					id=796;
				};
				class pickUpItem
				{
					soundSet="pickUpCourierBag_SoundSet";
					id=797;
				};
			};
		};
	};
	class Stalker_2_Duty_01_Leg_small_pouches: Stalker_2_Duty_01_Legsmallpouches_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\pants\data\pants_co.paa"
		};
	};
	class Stalker_2_Duty_01_Leg_small_pouches_ClearSky: Stalker_2_Duty_01_Legsmallpouches_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\pants\data\pants_ClearSky.paa"
		};
	};
	class Stalker_2_Duty_01_Leg_small_pouches_Korpus: Stalker_2_Duty_01_Legsmallpouches_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\pants\data\pants_Korpus.paa"
		};
	};
	class Stalker_2_Duty_01_Leg_small_pouches_Monolith: Stalker_2_Duty_01_Legsmallpouches_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\pants\data\pants_Monolith.paa"
		};
	};
	class Stalker_2_Duty_01_Leg_small_pouches_MSOP: Stalker_2_Duty_01_Legsmallpouches_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\pants\data\pants_MSOP.paa"
		};
	};
	class Stalker_2_Duty_01_Leg_small_pouches_Warta: Stalker_2_Duty_01_Legsmallpouches_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\pants\data\pants_Warta.paa"
		};
	};
};
class CfgNonAIVehicles
{
	class ProxyAttachment;
	class ProxyLeg_small_pouches: ProxyAttachment
	{
		scope=2;
		inventorySlot="Leg_small_pouches_a";
		model="Stalker2_Duty_01\set\Pouches\Leg_small_pouches\Leg_small_pouches.p3d";
	};
	class ProxyLeg_small_pouches_Proxy: ProxyAttachment
	{
		scope=2;
		inventorySlot="Leg_small_pouches_b";
		model="Stalker2_Duty_01\set\Pouches\Leg_small_pouches\Leg_small_pouches_Proxy.p3d";
	};
};
class CfgSlots
{
	class Slot_Leg_small_pouches_a
	{
		name="Leg_small_pouches_a";
		displayName="$STR_Duty_01_Leg_small_pouches_01";
		GhostIcon="set:TGK_CustomIcons image:Leg_small_pouches";
	};
	class Slot_Leg_small_pouches_b
	{
		name="Leg_small_pouches_b";
		displayName="$STR_Duty_01_Leg_small_pouches_01";
		GhostIcon="set:TGK_CustomIcons image:Leg_small_pouches";
	};
};
