// VB_EchoCore — ядро семейства VB_Echo: энергия «Эхо» и холодный синтез (ТЗ: modding/mod_vb_Echo/ТЗ_VB_Echo.md).
// Носитель энергии — любой предмет любого мода с блоком VB_Echo в своём классе:
//   class VB_Echo
//   {
//       kind = "echo";        // "echo" — Эхо (красная, расходуется), "fusion" — холодный синтез (бирюзовая)
//       capacity = 100;       // Э (для storage = "energy" берётся energyStorageMax EnergyManager)
//       infinite = 0;         // 1 — всегда полный, расход не списывается
//       transfer = 1;         // 0 — энергию нельзя перелить в другой носитель / станцию / сеть
//       storage = "quantity"; // "quantity" — количество предмета (баки, ячейки), "energy" — EnergyManager (аккумуляторы)
//   };
// Логика — scripts/4_World/VB_EchoCore/VB_Echo.c.

class CfgPatches
{
	class VB_EchoCore
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]={"DZ_Data","DZ_Scripts"};
	};
};
class CfgMods
{
	class VB_EchoCore
	{
		dir="VB_EchoCore";
		picture="";
		action="";
		hideName=1;
		hidePicture=1;
		name="VB_EchoCore";
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
				files[]={"VB_EchoCore/scripts/4_World"};
			};
		};
	};
};
