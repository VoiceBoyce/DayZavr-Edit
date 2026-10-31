// Disable_Vehicle_Water_Damage — серверный мод: машины и мотоциклы не руинятся от воды (утопленный двигатель).
// Урон от столкновений и всё остальное — как в ванили. Ставить через -serverMod.

class CfgPatches
{
	class Disable_Vehicle_Water_Damage
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Scripts"};
	};
};

class CfgMods
{
	class Disable_Vehicle_Water_Damage
	{
		dir = "Disable_Vehicle_Water_Damage";
		hideName = 1;
		hidePicture = 1;
		name = "Disable_Vehicle_Water_Damage";
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
				files[] = {"Disable_Vehicle_Water_Damage/scripts/4_World"};
			};
		};
	};
};
