// DayZavr: слоты Magazine4-6 для жилета Vp_SET17_Vest (attachments Magazine..Magazine6).
// Раньше лежал в set\script\4_world\config.bin и был сломан:
//  - переопределял ванильные Slot_magazine..3 (менял их иконки),
//  - добавлял магазинам слоты "slot_magazine1..6", которых не существует, поэтому 4-6 не принимали магазины,
//  - не зависел от DZ_Weapons_Magazines, и += мог примениться раньше ванильного inventorySlot.
class CfgPatches
{
	class set_Incite_MagazineSlots
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Scripts","DZ_Weapons_Magazines","DZ_Characters_Vests"};
		magazines[] = {};
	};
};
class CfgSlots
{
	class Slot_Magazine4
	{
		name = "Magazine4";
		displayName = "#STR_CfgMagazine0";
		ghostIcon = "set:dayz_inventory image:magazine2";
		autoAttach = "false";
	};
	class Slot_Magazine5
	{
		name = "Magazine5";
		displayName = "#STR_CfgMagazine0";
		ghostIcon = "set:dayz_inventory image:magazine2";
		autoAttach = "false";
	};
	class Slot_Magazine6
	{
		name = "Magazine6";
		displayName = "#STR_CfgMagazine0";
		ghostIcon = "set:dayz_inventory image:magazine2";
		autoAttach = "false";
	};
};
class CfgMagazines
{
	class DefaultMagazine;
	class Magazine_Base: DefaultMagazine
	{
		inventorySlot[] += {"Magazine4","Magazine5","Magazine6"};
	};
};
