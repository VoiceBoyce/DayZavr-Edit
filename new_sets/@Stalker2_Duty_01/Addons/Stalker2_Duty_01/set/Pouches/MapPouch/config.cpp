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
			"DZ_Scripts"
		};
		magazines[]={};
	};
};
class cfgVehicles
{
	class Container_Base;
	class Stalker_2_Duty_01_MapPouch_Colorbase: Container_Base
	{
		scope=0;
		displayName="$STR_Duty_01_MapPouch_01";
		descriptionShort="$STR_Duty_01_MapPouch_02";
		model="Stalker2_Duty_01\set\Pouches\MapPouch\MapPouch.p3d";
		itemSize[]={2,3};
		itemsCargoSize[]={5,4};
		inventorySlot[]=
		{
			"MapPouch"
		};
		hiddenSelections[]=
		{
			"camo"
		};
		weight=400;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		rotationFlags=17;
		varWetMax=0.0099999998;
		heatIsolation=0.0099999998;
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
							0,
							
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
	class Stalker_2_Duty_01_MapPouch: Stalker_2_Duty_01_MapPouch_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\pants\data\pants_co.paa"
		};
	};
	class Stalker_2_Duty_01_MapPouch_ClearSky: Stalker_2_Duty_01_MapPouch_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\pants\data\pants_ClearSky.paa"
		};
	};
	class Stalker_2_Duty_01_MapPouch_Korpus: Stalker_2_Duty_01_MapPouch_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\pants\data\pants_Korpus.paa"
		};
	};
	class Stalker_2_Duty_01_MapPouch_Monolith: Stalker_2_Duty_01_MapPouch_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\pants\data\pants_Monolith.paa"
		};
	};
	class Stalker_2_Duty_01_MapPouch_MSOP: Stalker_2_Duty_01_MapPouch_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\pants\data\pants_MSOP.paa"
		};
	};
	class Stalker_2_Duty_01_MapPouch_Warta: Stalker_2_Duty_01_MapPouch_Colorbase
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
	class ProxyMapPouch: ProxyAttachment
	{
		scope=2;
		inventorySlot="MapPouch";
		model="Stalker2_Duty_01\set\Pouches\MapPouch\MapPouch.p3d";
	};
};
class CfgSlots
{
	class Slot_MapPouch
	{
		name="MapPouch";
		displayName="$STR_Duty_01_MapPouch_01";
		GhostIcon="set:TGK_CustomIcons image:MapPouch";
	};
};
