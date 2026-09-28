class CfgPatches
{
	class FOG_GPNVG_Tarzier_stuff
	{
		units[]=
		{
			"FOG_GPNVG_Tan",
			"FOG_GPNVG_Black",
			"FOG_GPNVG_RG",
			"FOG_GPNVG_Tarzier",
			"FOG_GPNVG_Tarzier_Black"
		};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Characters_Glasses"
		};
	};
};
class CfgVehicles
{
	class NVGoggles;
	class FOG_GPNVG_Base: NVGoggles
	{
		scope=0;
		displayName="L3Harris GPNVG18";
		descriptionShort="The L3Harris Ground Panoramic Night Vision Goggle provides the operator with a 97-degree panoramic field-of-view that enables increased situational awareness and operational effectiveness.";
		model="\FOG_MOD\Gear\NVG\GPNVG\FOG_GPNVG.p3d";
		hiddenSelections[]=
		{
			"camo"
		};
		attachments[]=
		{
			"BatteryD",
			"FOG_TarsierSlot"
		};
	};
	class FOG_GPNVG_Tan: FOG_GPNVG_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\NVG\GPNVG\Data\GPNVG18_Tan_co.paa"
		};
	};
	class FOG_GPNVG_Black: FOG_GPNVG_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\NVG\GPNVG\Data\GPNVG18_Black_co.paa"
		};
	};
	class FOG_GPNVG_RG: FOG_GPNVG_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\NVG\GPNVG\Data\GPNVG18_RG_co.paa"
		};
	};
	class FOG_GPNVG_Tarzier: FOG_GPNVG_Tan
	{
	};
	class FOG_GPNVG_Tarzier_Black: FOG_GPNVG_Black
	{
	};
};
