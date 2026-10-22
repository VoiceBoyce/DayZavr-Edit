class CfgPatches
{
	class FixtureMod164078
	{
		requiredAddons[] = {"DZ_Data","DZ_Structures_Takistan_Residential"};
	};
};
class CfgMods
{
	class FixtureMod164078
	{
		type = "mod";
		class defs
		{
			class worldScriptModule
			{
				value = "";
				files[] = {"FixtureMod164078/scripts/4_World"};
			};
		};
	};
};
class CfgVehicles
{
	class HouseNoDestruct;
	class Fixture_RebuildHouse: HouseNoDestruct
	{
		class Construction
		{
			class house
			{
				class fixture_wall
				{
					name = "Fixture wall";
					is_base = 0;
					utility_build[] = {"fixture_roof"};
				};
				class fixture_roof
				{
					name = "Fixture roof";
					is_base = 0;
					toggle_utility_part_update = 1;
					utility_build[] = {"fixture_wall"};
				};
			};
		};
	};
	class NavmeshTest_A;
	class Fixture_Nav: NavmeshTest_A
	{
	};
};
