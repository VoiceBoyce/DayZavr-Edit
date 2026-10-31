// Disable_Vehicle_Damage — серверный мод: машины и мотоциклы не получают урон от столкновений (ДТП) и от воды
// (утопленный двигатель). Ставить через -serverMod.

class CfgPatches
{
	class Disable_Vehicle_Damage
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Scripts"};
	};
};

class CfgMods
{
	class Disable_Vehicle_Damage
	{
		dir = "Disable_Vehicle_Damage";
		hideName = 1;
		hidePicture = 1;
		name = "Disable_Vehicle_Damage";
		author = "DayZavr";
		version = "1";
		extra = 0;
		type = "mod";
		dependencies[] = {"World"};
		class defs
		{
			class worldScriptModule
			{
				value = "";
				files[] = {"Disable_Vehicle_Damage/scripts/4_World"};
			};
		};
	};
};
