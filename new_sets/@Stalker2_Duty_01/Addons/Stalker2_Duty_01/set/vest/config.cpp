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
	class Clothing_Base;
	class Clothing: Clothing_Base
	{
	};
	class Stalker_2_Duty_01_VestPouch_Colorbase: Clothing
	{
		scope=0;
		displayName="$STR_Duty_01_Vest_01";
		descriptionShort="$STR_Duty_01_Vest_02";
		model="Stalker2_Duty_01\set\vest\Vest_Duty_01.p3d";
		itemSize[]={2,3};
		itemsCargoSize[]={5,4};
		inventorySlot[]=
		{
			"VestPouch_Duty_01"
		};
		hiddenSelections[]=
		{
			"camo",
			"anm"
		};
		weight=1200;
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
								"Stalker2_Duty_01\set\pants\data\pants.rvmat",
								"Stalker2_Duty_01\set\vest\data\anm.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"Stalker2_Duty_01\set\pants\data\pants.rvmat",
								"Stalker2_Duty_01\set\vest\data\anm.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"Stalker2_Duty_01\set\pants\data\pants_damage.rvmat",
								"Stalker2_Duty_01\set\vest\data\anm_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"Stalker2_Duty_01\set\pants\data\pants_damage.rvmat",
								"Stalker2_Duty_01\set\vest\data\anm_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"Stalker2_Duty_01\set\pants\data\pants_destruct.rvmat",
								"Stalker2_Duty_01\set\vest\data\anm_destruct.rvmat"
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
	class Stalker_2_Duty_01_VestPouch: Stalker_2_Duty_01_VestPouch_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\pants\data\pants_co.paa",
			"Stalker2_Duty_01\set\vest\data\anm_co.paa"
		};
	};
	class Stalker_2_Duty_01_VestPouch_ClearSky: Stalker_2_Duty_01_VestPouch_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\pants\data\pants_ClearSky.paa",
			"Stalker2_Duty_01\set\vest\data\anm_co.paa"
		};
	};
	class Stalker_2_Duty_01_VestPouch_Korpus: Stalker_2_Duty_01_VestPouch_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\pants\data\pants_Korpus.paa",
			"Stalker2_Duty_01\set\vest\data\anm_co.paa"
		};
	};
	class Stalker_2_Duty_01_VestPouch_Monolith: Stalker_2_Duty_01_VestPouch_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\pants\data\pants_Monolith.paa",
			"Stalker2_Duty_01\set\vest\data\anm_co.paa"
		};
	};
	class Stalker_2_Duty_01_VestPouch_MSOP: Stalker_2_Duty_01_VestPouch_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\pants\data\pants_MSOP.paa",
			"Stalker2_Duty_01\set\vest\data\anm_co.paa"
		};
	};
	class Stalker_2_Duty_01_VestPouch_Warta: Stalker_2_Duty_01_VestPouch_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\pants\data\pants_Warta.paa",
			"Stalker2_Duty_01\set\vest\data\anm_co.paa"
		};
	};
	class Stalker_2_Duty_01_VestPouch_INVISIBLEammo: Stalker_2_Duty_01_VestPouch_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\pants\data\pants_co.paa",
			"Stalker2_Duty_01\set\vest\data\anm_ca.paa"
		};
	};
	class Stalker_2_Duty_01_VestPouch_ClearSky_INVISIBLEammo: Stalker_2_Duty_01_VestPouch_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\pants\data\pants_ClearSky.paa",
			"Stalker2_Duty_01\set\vest\data\anm_ca.paa"
		};
	};
	class Stalker_2_Duty_01_VestPouch_Korpus_INVISIBLEammo: Stalker_2_Duty_01_VestPouch_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\pants\data\pants_Korpus.paa",
			"Stalker2_Duty_01\set\vest\data\anm_ca.paa"
		};
	};
	class Stalker_2_Duty_01_VestPouch_Monolith_INVISIBLEammo: Stalker_2_Duty_01_VestPouch_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\pants\data\pants_Monolith.paa",
			"Stalker2_Duty_01\set\vest\data\anm_ca.paa"
		};
	};
	class Stalker_2_Duty_01_VestPouch_MSOP_INVISIBLEammo: Stalker_2_Duty_01_VestPouch_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\pants\data\pants_MSOP.paa",
			"Stalker2_Duty_01\set\vest\data\anm_ca.paa"
		};
	};
	class Stalker_2_Duty_01_VestPouch_Warta_INVISIBLEammo: Stalker_2_Duty_01_VestPouch_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\pants\data\pants_Warta.paa",
			"Stalker2_Duty_01\set\vest\data\anm_ca.paa"
		};
	};
};
class CfgNonAIVehicles
{
	class ProxyAttachment;
	class ProxyVest_Duty_01: ProxyAttachment
	{
		scope=2;
		inventorySlot="VestPouch_Duty_01";
		model="Stalker2_Duty_01\set\vest\Vest_Duty_01.p3d";
	};
};
class CfgSlots
{
	class Slot_VestPouch_Duty_01
	{
		name="VestPouch_Duty_01";
		displayName="$STR_Duty_01_Vest_01";
		GhostIcon="set: image:";
	};
};
