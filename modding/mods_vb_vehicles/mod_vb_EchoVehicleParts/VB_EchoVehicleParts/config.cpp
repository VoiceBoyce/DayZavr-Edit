// VB_EchoVehicleParts — общие комплектующие машин семейства VB_Echo (в моде @VB_EchoVehicles).
// Подходят к любой машине, и к ванильной (слоты ванили). ТЗ: modding/mod_vb_Echo/ТЗ_VB_Echo.md, раздел 3.2.
// Аккумуляторы: Эхо (красный, заряжается от мотора) и холодного синтеза (бирюзовый, всегда полный, донат).
// Только благодаря такому аккумулятору антиграв-модули держат заглушенную машину на подушке (решение пользователя 11-20).

class CfgPatches
{
	class VB_EchoVehicleParts
	{
		units[]={"VB_EchoBattery","VB_EchoBatteryFusion"};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]={"DZ_Data","DZ_Vehicles_Parts","VB_EchoCore"};
	};
};
class CfgMods
{
	class VB_EchoVehicleParts
	{
		dir="VB_EchoVehicleParts";
		picture="";
		action="";
		hideName=1;
		hidePicture=1;
		name="VB_EchoVehicleParts";
		credits="DayZavr";
		author="DayZavr";
		version="1";
		extra=0;
		type="mod";
		dependencies[]={"World"};
		class defs
		{
			class worldScriptModule
			{
				value="";
				files[]={"VB_EchoVehicleParts/scripts/4_World"};
			};
		};
	};
};
class CfgVehicles
{
	class CarBattery;
	class VB_EchoBattery: CarBattery
	{
		scope=2;
		displayName="$STR_VB_EP_battery_name";
		descriptionShort="$STR_VB_EP_battery_disc";
		model="VB_EchoVehicleParts\data\vb_echo_battery.p3d";
		attachments[]={};
		weight=12000;
		hiddenSelections[]={"neon"};
		hiddenSelectionsTextures[]={"vb_echovehicleparts\data\er_neon_co.paa"};
		hiddenSelectionsMaterials[]={"vb_echovehicleparts\data\er_neon_soft.rvmat"};
		class VB_Echo
		{
			kind="echo";
			capacity=0;
			infinite=0;
			transfer=1;
			storage="energy";
		};
	};
	class VB_EchoBatteryFusion: VB_EchoBattery
	{
		scope=2;
		displayName="$STR_VB_EP_battery_fusion_name";
		descriptionShort="$STR_VB_EP_battery_fusion_disc";
		hiddenSelectionsTextures[]={"vb_echovehicleparts\data\er_neon_fusion_co.paa"};
		class VB_Echo
		{
			kind="fusion";
			capacity=0;
			infinite=1;
			transfer=0;
			storage="energy";
		};
	};
};
