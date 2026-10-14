class CfgPatches
{
	class Stalker_2_Monolith_GranadePouch
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
	class Stalker_2_Monolith_GranadePouch_Colorbase: Container_Base
	{
		scope=0;
		displayName="$STR_Stalker_2_Monolith_GranadePouch_01";
		descriptionShort="$STR_Stalker_2_Monolith_GranadePouch_02";
		model="Stalker2_Monolith_01\set\items\GranadePouch\models\GranadePouch.p3d";
		inventorySlot[]=
		{
			"GranadePouch_a",
			"GranadePouch_b",
			"GranadePouch_c"
		};
		attachments[]=
		{
			"VestGrenadeA"
		};
		weight=200;
		itemSize[]={2,2};
		itemsCargoSize[]={3,2};
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		hiddenSelections[]=
		{
			"camo"
		};
		randomQuantity=2;
		rotationFlags=17;
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
								"Stalker2_Monolith_01\set\items\GranadePouch\data\GranadePouch.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"Stalker2_Monolith_01\set\items\GranadePouch\data\GranadePouch.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"Stalker2_Monolith_01\set\items\GranadePouch\data\GranadePouch_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"Stalker2_Monolith_01\set\items\GranadePouch\data\GranadePouch_damage.rvmat"
							}
						},
						
						{
							0.0099999998,
							
							{
								"Stalker2_Monolith_01\set\items\GranadePouch\data\GranadePouch_destruct.rvmat"
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
	class S2_Monolith_01_GranadePouch: Stalker_2_Monolith_GranadePouch_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\items\GranadePouch\data\GranadePouch_co.paa"
		};
	};
	class S2_Monolith_01_GranadePouch_ClearSky: Stalker_2_Monolith_GranadePouch_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\items\GranadePouch\data\GranadePouch_ClearSky.paa"
		};
	};
	class S2_Monolith_01_GranadePouch_Duty: Stalker_2_Monolith_GranadePouch_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\items\GranadePouch\data\GranadePouch_Duty.paa"
		};
	};
	class S2_Monolith_01_GranadePouch_Freedom: Stalker_2_Monolith_GranadePouch_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\items\GranadePouch\data\GranadePouch_Freedom.paa"
		};
	};
	class S2_Monolith_01_GranadePouch_Korpus: Stalker_2_Monolith_GranadePouch_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\items\GranadePouch\data\GranadePouch_Korpus.paa"
		};
	};
	class S2_Monolith_01_GranadePouch_Merc: Stalker_2_Monolith_GranadePouch_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\items\GranadePouch\data\GranadePouch_Merc.paa"
		};
	};
	class S2_Monolith_01_GranadePouch_MSOP: Stalker_2_Monolith_GranadePouch_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\items\GranadePouch\data\GranadePouch_MSOP.paa"
		};
	};
	class S2_Monolith_01_GranadePouch_Neutrals: Stalker_2_Monolith_GranadePouch_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\items\GranadePouch\data\GranadePouch_Neutrals.paa"
		};
	};
	class S2_Monolith_01_GranadePouch_Warta: Stalker_2_Monolith_GranadePouch_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\items\GranadePouch\data\GranadePouch_Warta.paa"
		};
	};
};
class CfgNonAIVehicles
{
	class ProxyAttachment;
	class ProxyGranadePouch_proxy_a: ProxyAttachment
	{
		scope=2;
		inventorySlot="GranadePouch_a";
		model="Stalker2_Monolith_01\set\items\GranadePouch\models\GranadePouch_proxy_a.p3d";
	};
	class ProxyGranadePouch_proxy_b: ProxyAttachment
	{
		scope=2;
		inventorySlot="GranadePouch_b";
		model="Stalker2_Monolith_01\set\items\GranadePouch\models\GranadePouch_proxy_b.p3d";
	};
	class ProxyGranadePouch_proxy_c: ProxyAttachment
	{
		scope=2;
		inventorySlot="GranadePouch_c";
		model="Stalker2_Monolith_01\set\items\GranadePouch\models\GranadePouch_proxy_c.p3d";
	};
};
class CfgSlots
{
	class Slot_GranadePouch_a
	{
		name="GranadePouch_a";
		displayName="$STR_Stalker_2_Monolith_GranadePouch_01";
		GhostIcon="set:TGK_CustomIconsS2MONOLITH image:GranadePouch";
	};
	class Slot_GranadePouch_b
	{
		name="GranadePouch_b";
		displayName="$STR_Stalker_2_Monolith_GranadePouch_01";
		GhostIcon="set:TGK_CustomIconsS2MONOLITH image:GranadePouch";
	};
	class Slot_GranadePouch_c
	{
		name="GranadePouch_c";
		displayName="$STR_Stalker_2_Monolith_GranadePouch_01";
		GhostIcon="set:TGK_CustomIconsS2MONOLITH image:GranadePouch";
	};
};
