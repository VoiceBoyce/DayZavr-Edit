// Сцепление машин на снегу и льду Сахала как на обычном грунте других карт.
// Снег переводим на покрытие Dirt: сцепление 0.91 вместо 0.61 и сопротивление 0.07 вместо 0.45.
// Лёду поднимаем сцепление с 0.2 до 0.9, покрытие Ice используется только на Сахале.
// Мод нужен и серверу, и клиенту: физику машины считают обе стороны.

class CfgPatches
{
	class VB_SakhalGrip
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Surfaces", "DZ_Surfaces_Sakhal"};
	};
};

class CfgVehicleSurfaces
{
	class Ice
	{
		friction = 0.9;
	};
};

class CfgSurfaces
{
	class DZ_SurfacesExt;
	class sakhal_snow: DZ_SurfacesExt
	{
		vpSurface = "Dirt";
	};
	class sakhal_snow_forest: DZ_SurfacesExt
	{
		vpSurface = "Dirt";
	};
};
