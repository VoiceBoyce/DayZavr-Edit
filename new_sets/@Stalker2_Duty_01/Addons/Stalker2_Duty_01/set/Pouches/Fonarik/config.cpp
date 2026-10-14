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
			"DZ_Weapons_Lights"
		};
	};
};
class cfgVehicles
{
	class Inventory_Base;
	class Switchable_Base;
	class UniversalLight;
	class Stalker_2_Duty_01_UniversalLight: UniversalLight
	{
		scope=2;
		displayName="$STR_Duty_01_UniversalLight_01";
		descriptionShort="$STR_Duty_01_UniversalLight_02";
		model="Stalker2_Duty_01\set\Pouches\Fonarik\Fonarik.p3d";
		debug_ItemCategory=3;
		rotationFlags=17;
		reversed=1;
		weight=80;
		itemSize[]={2,1};
		inventorySlot[]=
		{
			"helmetFlashlight"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=120;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"Stalker2_Duty_01\set\helmet\main\Main.rvmat",
								"Stalker2_Duty_01\set\Pouches\Fonarik\Main.rvmat"
							}
						},
						
						{
							0.69999999,
							{}
						},
						
						{
							0.5,
							
							{
								"Stalker2_Duty_01\set\helmet\main\Main_damage.rvmat",
								"Stalker2_Duty_01\set\Pouches\Fonarik\Main_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							{}
						},
						
						{
							0,
							
							{
								"Stalker2_Duty_01\set\helmet\main\Main_destruct.rvmat",
								"Stalker2_Duty_01\set\Pouches\Fonarik\Main_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		repairableWithKits[]={7};
		repairCosts[]={25};
		hiddenSelections[]=
		{
			"camo",
			"glass",
			"reflector"
		};
		hiddenSelectionsTextures[]=
		{
			"dz\weapons\attachments\data\m4_flashlight_co.paa",
			"dz\gear\tools\data\flashlight_glass.paa",
			"dz\weapons\attachments\data\m4_flashlight_co.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"dz\weapons\attachments\data\m4_flashlight.rvmat",
			"dz\gear\tools\data\flashlight_glass.rvmat",
			"dz\weapons\attachments\data\m4_flashlight.rvmat"
		};
		class EnergyManager
		{
			hasIcon=1;
			autoSwitchOffWhenInCargo=1;
			energyUsagePerSecond=0.0099999998;
			plugType=1;
			attachmentAction=1;
			updateInterval=40;
		};
		soundImpactType="plastic";
	};
};
class CfgNonAIVehicles
{
	class ProxyAttachment;
	class ProxyFonarik: ProxyAttachment
	{
		scope=2;
		inventorySlot="helmetFlashlight";
		model="Stalker2_Duty_01\set\Pouches\Fonarik\Fonarik.p3d";
	};
};
class CfgSlots
{
	class Slot_S2_Duty_01_UniversalLight
	{
		name="S2_Duty_01_UniversalLight";
		displayName="$STR_Duty_01_UniversalLight_01";
		GhostIcon="set:TGK_CustomIcons image:Fonarik";
	};
};
