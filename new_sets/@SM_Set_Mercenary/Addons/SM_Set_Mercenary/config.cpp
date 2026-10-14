class CfgPatches
{
	class SM_Set_Mercenary
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Scripts"
		};
	};
};
class CfgMods
{
	class SM_Set_Mercenary
	{
		dir="SM_Set_Mercenary";
		hideName=1;
		hidePicture=1;
		name="SM_Set_Mercenary";
		credits="Twizicss";
		author="Twizicss";
		authorID="0";
		version="1.0";
		extra=0;
		type="mod";
		dependencies[]=
		{
			"World"
		};
		class defs
		{
			class worldScriptModule
			{
				value="";
				files[]=
				{
					"SM_Set_Mercenary\scripts\4_World"
				};
			};
		};
	};
};
class cfgWeapons
{
	class RifleCore;
	class Rifle_Base: RifleCore
	{
		inventorySlot[]+=
		{
			"Shoulder",
			"Shoulder1",
			"Melee"
		};
	};
};
class CfgSlots
{
	class Slot_Shoulder1
	{
		name="Shoulder1";
		displayName="#Shoulder";
		ghostIcon="set:dayz_inventory image:shoulderleft";
	};
};
class CfgNonAIVehicles
{
	class ProxyAttachment;
	class ProxyShoulder1: ProxyAttachment
	{
		scope=2;
		inventorySlot="Shoulder1";
		model="\SM_Set_Mercenary\bag\models\proxy\Shoulder1.p3d";
	};
};
