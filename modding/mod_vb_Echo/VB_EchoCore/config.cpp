// VB_EchoCore — ядро семейства VB_Echo: энергия «Эхо» и холодный синтез (ТЗ: modding/mod_vb_Echo/ТЗ_VB_Echo.md).
// Носитель энергии — любой предмет любого мода с блоком VB_Echo в своём классе:
//   class VB_Echo
//   {
//       kind = "echo";        // "echo" — Эхо (красная, расходуется), "fusion" — холодный синтез (бирюзовая)
//       capacity = 20000;     // kE (для storage = "energy" берётся energyStorageMax EnergyManager)
//       infinite = 0;         // 1 — всегда полный, расход не списывается
//       transfer = 1;         // 0 — энергию нельзя перелить в другой носитель / станцию / сеть
//       storage = "quantity"; // "quantity" — количество предмета (баки, ячейки), "energy" — EnergyManager (аккумуляторы)
//       perE = 1;             // единиц заряда в 1 kE (необязательно, по умолчанию 1; аккумулятор Эхо — 0.1)
//   };
// Логика — scripts/4_World/VB_EchoCore/VB_Echo.c.
// Ещё в ядре (11-20): энергоячейки VB_EchoCell_* (одна модель, кольцо на крышке — уровень, ядро — заряд), общие
// слоты зарядки и разрядки (станция базы, баки, аккумуляторы, ячейки) и свои иконки слотов (imageset vb_echo_icons,
// генератор tools/ec_icons.py): ghostIcon="set:vb_echo_icons image:<crystal|tank|pipes|pipes_engine|battery|cell|
// cell_charge|cell_drain>".

class CfgPatches
{
	class VB_EchoCore
	{
		units[]={"VB_EchoCell_Mk1","VB_EchoCell_Mk2","VB_EchoCell_Mk3","VB_EchoCell_Mk4","VB_EchoCell_Fusion"};
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
			class imageSets
			{
				files[]={"VB_EchoCore/gui/vb_echo_icons.imageset"};
			};
		};
	};
};
// Слоты зарядной станции базы (VB_EchoBase): Charge* — заряжает, Drain* — разряжает ячейку в запас сети.
// Носители сами объявляют, куда встают (inventorySlot[]): бак Echo Runner, аккумулятор Эхо, энергоячейки.
class CfgSlots
{
	class Slot_VB_Echo_ChargeTank1
	{
		name="VB_Echo_ChargeTank1";
		displayName="$STR_VB_Echo_slot_tank";
		ghostIcon="set:vb_echo_icons image:tank";
	};
	class Slot_VB_Echo_ChargeTank2
	{
		name="VB_Echo_ChargeTank2";
		displayName="$STR_VB_Echo_slot_tank";
		ghostIcon="set:vb_echo_icons image:tank";
	};
	class Slot_VB_Echo_ChargeBattery
	{
		name="VB_Echo_ChargeBattery";
		displayName="$STR_VB_Echo_slot_battery";
		ghostIcon="set:vb_echo_icons image:battery";
	};
	class Slot_VB_Echo_ChargeCell1
	{
		name="VB_Echo_ChargeCell1";
		displayName="$STR_VB_Echo_slot_cell_charge";
		ghostIcon="set:vb_echo_icons image:cell_charge";
	};
	class Slot_VB_Echo_ChargeCell2
	{
		name="VB_Echo_ChargeCell2";
		displayName="$STR_VB_Echo_slot_cell_charge";
		ghostIcon="set:vb_echo_icons image:cell_charge";
	};
	class Slot_VB_Echo_DrainCell1
	{
		name="VB_Echo_DrainCell1";
		displayName="$STR_VB_Echo_slot_cell_drain";
		ghostIcon="set:vb_echo_icons image:cell_drain";
	};
	class Slot_VB_Echo_DrainCell2
	{
		name="VB_Echo_DrainCell2";
		displayName="$STR_VB_Echo_slot_cell_drain";
		ghostIcon="set:vb_echo_icons image:cell_drain";
	};
};
class CfgVehicles
{
	class Inventory_Base;
	// Энергоячейка: заряд = количество предмета (kE). Новая — пустая. Неон «neon»: пустая серая, Эхо красная,
	// синтез бирюзовая (скрипт VB_EchoCell.c); «tier» — кольцо уровня на крышке (Mk I–IV: жёлтый, зелёный, синий,
	// фиолетовый). Ёмкости: 2048 / 4096 / 10 000 / 20 000 kE — шкала ×32, выше 4096 круглые (решение 11-20).
	class VB_EchoCell_Base: Inventory_Base
	{
		scope=0;
		displayName="$STR_VB_Echo_cell_mk1";
		descriptionShort="$STR_VB_Echo_cell_disc";
		model="VB_EchoCore\data\eb_cell.p3d";
		weight=400;
		itemSize[]={1,2};
		rotationFlags=0;
		canBeSplit=0;
		varQuantityInit=0;
		varQuantityMin=0;
		varQuantityMax=2048;
		varQuantityDestroyOnMin=0;		// пустая ячейка не исчезает (E117)
		quantityBar=1;
		stackedUnit="percentage";		// заряд в процентах, не в штуках (11-20)
		inventorySlot[]={"VB_Echo_ChargeCell1","VB_Echo_ChargeCell2","VB_Echo_DrainCell1","VB_Echo_DrainCell2"};
		hiddenSelections[]={"neon","tier"};
		hiddenSelectionsTextures[]={"vb_echocore\data\er_neon_empty_co.paa","vb_echocore\data\eb_neon_t1_co.paa"};
		hiddenSelectionsMaterials[]={"vb_echocore\data\er_neon_empty.rvmat","vb_echocore\data\er_neon_idle.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=100;
				};
			};
		};
		class VB_Echo
		{
			kind="echo";
			capacity=2048;
			infinite=0;
			transfer=1;
			storage="quantity";
		};
	};
	class VB_EchoCell_Mk1: VB_EchoCell_Base
	{
		scope=2;
		displayName="$STR_VB_Echo_cell_mk1";
		descriptionShort="$STR_VB_Echo_cell_mk1_disc";
		varQuantityInit=0;
		varQuantityMax=2048;
		hiddenSelectionsTextures[]={"vb_echocore\data\er_neon_empty_co.paa","vb_echocore\data\eb_neon_t1_co.paa"};
		class VB_Echo
		{
			kind="echo";
			capacity=2048;
			infinite=0;
			transfer=1;
			storage="quantity";
		};
	};
	class VB_EchoCell_Mk2: VB_EchoCell_Base
	{
		scope=2;
		displayName="$STR_VB_Echo_cell_mk2";
		descriptionShort="$STR_VB_Echo_cell_mk2_disc";
		varQuantityInit=0;
		varQuantityMax=4096;
		hiddenSelectionsTextures[]={"vb_echocore\data\er_neon_empty_co.paa","vb_echocore\data\eb_neon_t2_co.paa"};
		class VB_Echo
		{
			kind="echo";
			capacity=4096;
			infinite=0;
			transfer=1;
			storage="quantity";
		};
	};
	class VB_EchoCell_Mk3: VB_EchoCell_Base
	{
		scope=2;
		displayName="$STR_VB_Echo_cell_mk3";
		descriptionShort="$STR_VB_Echo_cell_mk3_disc";
		varQuantityInit=0;
		varQuantityMax=10000;
		hiddenSelectionsTextures[]={"vb_echocore\data\er_neon_empty_co.paa","vb_echocore\data\eb_neon_t3_co.paa"};
		class VB_Echo
		{
			kind="echo";
			capacity=10000;
			infinite=0;
			transfer=1;
			storage="quantity";
		};
	};
	class VB_EchoCell_Mk4: VB_EchoCell_Base
	{
		scope=2;
		displayName="$STR_VB_Echo_cell_mk4";
		descriptionShort="$STR_VB_Echo_cell_mk4_disc";
		varQuantityInit=0;
		varQuantityMax=20000;
		hiddenSelectionsTextures[]={"vb_echocore\data\er_neon_empty_co.paa","vb_echocore\data\eb_neon_t4_co.paa"};
		class VB_Echo
		{
			kind="echo";
			capacity=20000;
			infinite=0;
			transfer=1;
			storage="quantity";
		};
	};
	// донат (решение пользователя 11-20): не разряжается, энергию не отдаёт (синтез не переливается) — только
	// питание снаряжения Эхо; на станции не заряжается и не разряжается (слотов станции нет)
	class VB_EchoCell_Fusion: VB_EchoCell_Base
	{
		scope=2;
		displayName="$STR_VB_Echo_cell_fusion";
		descriptionShort="$STR_VB_Echo_cell_fusion_disc";
		varQuantityInit=20000;
		varQuantityMax=20000;
		inventorySlot[]={};
		hiddenSelectionsTextures[]={"vb_echocore\data\er_neon_fusion_co.paa","vb_echocore\data\er_neon_fusion_co.paa"};
		hiddenSelectionsMaterials[]={"vb_echocore\data\er_neon_idle.rvmat","vb_echocore\data\er_neon_idle.rvmat"};
		class VB_Echo
		{
			kind="fusion";
			capacity=20000;
			infinite=1;
			transfer=0;
			storage="quantity";
		};
	};
};
