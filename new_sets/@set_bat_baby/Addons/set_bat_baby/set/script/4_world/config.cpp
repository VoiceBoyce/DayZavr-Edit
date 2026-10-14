class CfgPatches
{
	class Slot_Magazine_proxy
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Characters_Vests"
		};
		magazines[]={};
	};
};
class CfgSlots
{
	class Slot_Magazine
	{
		name="Magazine";
		displayName="#STR_CfgMagazine0";
		ghostIcon="set:dayz_inventory image:magazine2";
	};
	class Slot_Magazine2
	{
		name="Magazine2";
		displayName="#STR_CfgMagazine0";
		ghostIcon="set:dayz_inventory image:magazine2";
	};
	class Slot_Magazine3
	{
		name="Magazine3";
		displayName="#STR_CfgMagazine0";
		ghostIcon="set:dayz_inventory image:magazine2";
	};
};
class CfgMagazines
{
	class DefaultMagazine;
	class Magazine_Base: DefaultMagazine
	{
		scope=0;
		inventorySlot[]+=
		{
			"slot_magazine1",
			"slot_magazine2",
			"slot_magazine3"
		};
	};
};
