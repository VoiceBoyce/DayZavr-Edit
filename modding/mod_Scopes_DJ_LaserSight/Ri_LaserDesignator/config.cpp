class CfgPatches
{
	class Ri_LaserDesignator
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Scripts"
		};
		worlds[]={};
	};
};
class CfgMods
{
	class Ri_LaserDesignator
	{
		dir="Ri_LaserDesignator";
		hideName=0;
		hidePicture=0;
		name="Ri_LaserDesignator";
		version="Version 1.0";
		extra=0;
		type="mod";
		inputs="Ri_LaserDesignator/Inputs.xml";
		dependencies[]=
		{
			"Game",
			"World"
		};
		class defs
		{
			class gameScriptModule
			{
				value="";
				files[]=
				{
					"Ri_LaserDesignator/scripts/3_Game"
				};
			};
			class worldScriptModule
			{
				value="";
				files[]=
				{
					"Ri_LaserDesignator/scripts/4_World"
				};
			};
		};
	};
};
class cfgVehicles
{
	class Switchable_Base;
	class Inventory_Base;
	class Ri_Laser: Inventory_Base
	{
		scope=1;
		model="Ri_LaserDesignator\Lasers\model\Luch.p3d";
		hiddenSelections[]=
		{
			"zbytek"
		};
		simpleHiddenSelections[]=
		{
			"zbytek"
		};
		hiddenSelectionsTextures[]=
		{
			"#(argb,8,8,3)color(0,0,0,0,co)"
		};
		hiddenSelectionsMaterials[]=
		{
			"Ri_LaserDesignator\Lasers\data\l.rvmat"
		};
	};
	class Ri_LaserDesignator_Base: Switchable_Base
	{
		scope=1;
		rotationFlags=17;
		model="Ri_LaserDesignator\Lasers\model\Luch.p3d";
		reversed=1;
		weight=150;
		itemSize[]={2,1};
		inventorySlot[]=
		{
			"weaponFlashlight"
		};
		attachments[]=
		{
			"BatteryD"
		};
		repairableWithKits[]={7};
		repairCosts[]={25};
		soundImpactType="plastic";
		class EnergyManager
		{
			hasIcon=1;
			autoSwitchOffWhenInCargo=1;
			energyUsagePerSecond=0.0024999999;
			plugType=1;
			attachmentAction=1;
			updateInterval=30;
		};
		hiddenSelections[]=
		{
			"zbytek",
			"Beam1",
			"glass"
		};
		simpleHiddenSelections[]=
		{
			"Beam1"
		};
	};
};
