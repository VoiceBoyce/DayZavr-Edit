// VB_EchoBase — энергия Эхо на базе (ТЗ: modding/mod_vb_Echo/ТЗ_VB_EchoBase.md, гайд: modding/mod_vb_Echo/ГАЙД.md).
// Цепочка: источники (ветряк, солнечная панель, печь) дают ток → преобразователь с кристаллом делает Эхо →
// буфер преобразователя и накопители → зарядная станция (вложенное в гнёздах) и беспроводная платформа (машина, игрок).
// Единицы (решение 11-20): kE и kE/tick, тик = 10 с, до 4096 степени двойки, выше круглые; шкала ×32 и буферы —
// ТЗ_VB_EchoBase_буферы.md. Вход/выход/радиус — class VB_EchoRating у
// каждого класса (Mk I; улучшенные уровни пользователю пока не нужны).
// Модели — свои (tools/eb_models.py, концепты одобрены 11-20). Неон (hiddenSelection «neon»): без энергии серый,
// у устройств с Эхо красный, у источников цвет уровня (Mk I — жёлтый); «tier» — метка уровня. Красит скрипт по
// состоянию из сети. Устройства ставятся из наборов (одна модель кейса на все, как кит станка DRL): голограмма —
// само устройство (projectionTypename), разборка ключом или отвёрткой обратно в набор.
// Сеть без проводов: всё в радиусе от преобразователя или ретранслятора. Счёт — один серверный таймер
// (scripts/4_World/VB_EchoBase/VB_EB_Network.c), цифры — Instance_1\VB_Mods\VB_EchoBase\settings.json.

class CfgPatches
{
	class VB_EchoBase
	{
		units[]={"VB_EB_WindTurbine","VB_EB_SolarPanel","VB_EB_Furnace","VB_EB_Converter","VB_EB_Storage","VB_EB_ChargeStation","VB_EB_ChargePad","VB_EB_Relay","VB_EB_Crystal","VB_EB_CrystalFusion","VB_EB_Kit_WindTurbine","VB_EB_Kit_SolarPanel","VB_EB_Kit_Furnace","VB_EB_Kit_Converter","VB_EB_Kit_Storage","VB_EB_Kit_ChargeStation","VB_EB_Kit_ChargePad","VB_EB_Kit_Relay"};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]={"DZ_Data","DZ_Scripts","VB_EchoCore"};
	};
};
class CfgMods
{
	class VB_EchoBase
	{
		dir="VB_EchoBase";
		picture="";
		action="";
		hideName=1;
		hidePicture=1;
		name="VB_EchoBase";
		credits="DayZavr";
		author="DayZavr";
		version="1";
		extra=0;
		type="mod";
		dependencies[]={"World","Mission"};
		class defs
		{
			class worldScriptModule
			{
				value="";
				files[]={"VB_EchoBase/scripts/4_World"};
			};
			class missionScriptModule
			{
				value="";
				files[]={"VB_EchoBase/scripts/5_Mission"};
			};
		};
	};
};
class CfgSlots
{
	class Slot_VB_EB_Crystal
	{
		name="VB_EB_Crystal";
		displayName="$STR_VB_EB_slot_crystal";
		ghostIcon="set:vb_echo_icons image:crystal";		// свои иконки — VB_EchoCore (tools/ec_icons.py)
	};
};
class CfgVehicles
{
	class Inventory_Base;
	// общее у всех устройств: ставится только из набора, в руки и рюкзак не берётся; разбирается обратно в набор
	class VB_EB_DeviceBase: Inventory_Base
	{
		scope=0;
		displayName="$STR_VB_EB_device";
		descriptionShort="$STR_VB_EB_device_disc";
		weight=30000;
		itemSize[]={10,10};
		itemBehaviour=0;
		heavyItem=1;
		carveNavmesh=1;
		rotationFlags=2;
		slopeTolerance=0.4;
		yawPitchRollLimit[]={45,45,45};
		physLayer="item_large";
		soundImpactType="metal";
		// у ванильного Inventory_Base varQuantityDestroyOnMin = 1: опустевший буфер/накопитель удалялся вместе с
		// устройством (E117). У устройств заряд — количество, ноль — не повод исчезать
		varQuantityDestroyOnMin=0;
		// голограмма установки красит первое скрытое выделение (неон) — зелёный/красный: можно ли поставить
		hologramMaterial="power_generator";
		hologramMaterialPath="dz\gear\camping\data";
		hiddenSelections[]={"neon","tier"};
		hiddenSelectionsTextures[]={"vb_echobase\data\er_neon_empty_co.paa","vb_echobase\data\er_neon_empty_co.paa"};
		hiddenSelectionsMaterials[]={"vb_echobase\data\er_neon_empty.rvmat","vb_echobase\data\er_neon_empty.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=1000;
				};
			};
		};
	};
	// ---------------- источники тока
	class VB_EB_WindTurbine: VB_EB_DeviceBase
	{
		scope=2;
		displayName="$STR_VB_EB_wind";
		descriptionShort="$STR_VB_EB_wind_disc";
		model="VB_EchoBase\data\eb_wind.p3d";
		vbKit="VB_EB_Kit_WindTurbine";
		class VB_EchoRating { tier=1; outMax=64; };	// kE/tick при максимальном ветре (шкала ×32, 11-20)
		// буфер тока (ТЗ_VB_EchoBase_буферы.md): энергия EnergyManager — ваниль сама сохраняет и шлёт клиенту;
		// пассивный — сам не включается и не тратит
		class EnergyManager
		{
			isPassiveDevice=1;
			energyStorageMax=1024;
			energyAtSpawn=0;
			convertEnergyToQuantity=0;
		};
		class AnimationSources
		{
			class rotor
			{
				source="user";
				animPeriod=0.01;
				initPhase=0;
			};
		};
	};
	class VB_EB_SolarPanel: VB_EB_DeviceBase
	{
		scope=2;
		displayName="$STR_VB_EB_solar";
		descriptionShort="$STR_VB_EB_solar_disc";
		model="VB_EchoBase\data\eb_solar.p3d";
		vbKit="VB_EB_Kit_SolarPanel";
		class VB_EchoRating { tier=1; outMax=32; };	// kE/tick при солнце в зените
		// буфер тока (ТЗ_VB_EchoBase_буферы.md): энергия EnergyManager — ваниль сама сохраняет и шлёт клиенту;
		// пассивный — сам не включается и не тратит
		class EnergyManager
		{
			isPassiveDevice=1;
			energyStorageMax=512;
			energyAtSpawn=0;
			convertEnergyToQuantity=0;
		};
	};
	class VB_EB_Furnace: VB_EB_DeviceBase
	{
		scope=2;
		displayName="$STR_VB_EB_furnace";
		descriptionShort="$STR_VB_EB_furnace_disc";
		model="VB_EchoBase\data\eb_furnace.p3d";
		vbKit="VB_EB_Kit_Furnace";
		itemsCargoSize[]={10,6};						// отсек топлива
		// в отсек — только топливо (точные классы; kE за штуку — в настройках сервера, fuel). Клиент настроек не знает,
		// поэтому список здесь: иначе предмет «ложился» бы у клиента и возвращался сервером. Новое топливо — сюда и в fuel
		vbFuel[]={"WoodenStick","Firewood","VB_RiftRootPiece","VB_RiftBlood","VB_RiftFuelCapsule","VB_RiftHeartChunk"};
		// kE, что осталось в горящей порции — количество предмета (ваниль его сохраняет: порция переживает рестарт).
		// Не показывается (quantityShow=0); ноль — не повод исчезать (varQuantityDestroyOnMin=0 у базы)
		varQuantityInit=0;
		varQuantityMin=0;
		varQuantityMax=100000;
		quantityShow=0;
		quantityBar=0;
		class VB_EchoRating { tier=1; outMax=64; };	// kE/tick: так быстро жжёт топливо (kE за штуку — в настройках)
		// буфер тока (ТЗ_VB_EchoBase_буферы.md): энергия EnergyManager — ваниль сама сохраняет и шлёт клиенту;
		// пассивный — сам не включается и не тратит
		class EnergyManager
		{
			isPassiveDevice=1;
			energyStorageMax=1024;
			energyAtSpawn=0;
			convertEnergyToQuantity=0;
		};
	};
	// ---------------- ядро сети
	class VB_EB_Converter: VB_EB_DeviceBase
	{
		scope=2;
		displayName="$STR_VB_EB_converter";
		descriptionShort="$STR_VB_EB_converter_disc";
		model="VB_EchoBase\data\eb_converter.p3d";
		vbKit="VB_EB_Kit_Converter";
		attachments[]={"VB_EB_Crystal"};
		// кристалл в колбе = вставленный в слот (11-20): crystal_glow красит скрипт (Эхо красный, синтез бирюзовый),
		// crystal_hide — без кристалла скрыт (начальная фаза 1), crystal_spin — вращение (скрипт клиента, model.cfg)
		hiddenSelections[]={"neon","tier","crystal_glow"};
		hiddenSelectionsTextures[]={"vb_echobase\data\er_neon_empty_co.paa","vb_echobase\data\er_neon_empty_co.paa","vb_echobase\data\er_neon_co.paa"};
		hiddenSelectionsMaterials[]={"vb_echobase\data\er_neon_empty.rvmat","vb_echobase\data\er_neon_empty.rvmat","vb_echobase\data\er_neon_idle.rvmat"};
		class AnimationSources
		{
			class crystal_spin
			{
				source="user";
				animPeriod=0.01;
				initPhase=0;
			};
			class crystal_hide
			{
				source="user";
				animPeriod=0.01;
				initPhase=1;
			};
		};
		varQuantityInit=0;
		varQuantityMin=0;
		varQuantityMax=10000;
		quantityBar=1;
		stackedUnit="percentage";		// заряд в процентах, не в штуках (11-20)
		// буфер Эхо преобразователя 10 000 kE — носитель ядра (kE в количестве предмета), первая часть запаса сети
		class VB_EchoRating { tier=1; inMax=256; outMax=256; range=32; };	// ток до 256 kE/tick → Эхо (ночью ×1,5); радиус 32 м
		// буфер тока (ТЗ_VB_EchoBase_буферы.md): энергия EnergyManager — ваниль сама сохраняет и шлёт клиенту;
		// пассивный — сам не включается и не тратит
		class EnergyManager
		{
			isPassiveDevice=1;
			energyStorageMax=2048;
			energyAtSpawn=0;
			convertEnergyToQuantity=0;
		};
		class VB_Echo
		{
			kind="echo";
			capacity=10000;
			infinite=0;
			transfer=1;
			storage="quantity";
		};
	};
	// ---------------- хранение и выдача
	class VB_EB_Storage: VB_EB_DeviceBase
	{
		scope=2;
		displayName="$STR_VB_EB_storage";
		descriptionShort="$STR_VB_EB_storage_disc";
		model="VB_EchoBase\data\eb_storage.p3d";
		vbKit="VB_EB_Kit_Storage";
		varQuantityInit=0;
		varQuantityMin=0;
		varQuantityMax=100000;
		quantityBar=1;
		stackedUnit="percentage";		// заряд в процентах, не в штуках (11-20)
		// уровень заряда (решение пользователя 11-20): секции колб lvl0..11 и лампы шкалы lamp0..7 — заполненное
		// красное, пустое серое (скрипт VB_EB_Storage.VB_Paint по количеству предмета)
		hiddenSelections[]={"neon","tier","lvl0","lvl1","lvl2","lvl3","lvl4","lvl5","lvl6","lvl7","lvl8","lvl9","lvl10","lvl11","lamp0","lamp1","lamp2","lamp3","lamp4","lamp5","lamp6","lamp7"};
		hiddenSelectionsTextures[]={"vb_echobase\data\er_neon_empty_co.paa","vb_echobase\data\er_neon_empty_co.paa","vb_echobase\data\er_neon_empty_co.paa","vb_echobase\data\er_neon_empty_co.paa","vb_echobase\data\er_neon_empty_co.paa","vb_echobase\data\er_neon_empty_co.paa","vb_echobase\data\er_neon_empty_co.paa","vb_echobase\data\er_neon_empty_co.paa","vb_echobase\data\er_neon_empty_co.paa","vb_echobase\data\er_neon_empty_co.paa","vb_echobase\data\er_neon_empty_co.paa","vb_echobase\data\er_neon_empty_co.paa","vb_echobase\data\er_neon_empty_co.paa","vb_echobase\data\er_neon_empty_co.paa","vb_echobase\data\er_neon_empty_co.paa","vb_echobase\data\er_neon_empty_co.paa","vb_echobase\data\er_neon_empty_co.paa","vb_echobase\data\er_neon_empty_co.paa","vb_echobase\data\er_neon_empty_co.paa","vb_echobase\data\er_neon_empty_co.paa","vb_echobase\data\er_neon_empty_co.paa","vb_echobase\data\er_neon_empty_co.paa"};
		hiddenSelectionsMaterials[]={"vb_echobase\data\er_neon_empty.rvmat","vb_echobase\data\er_neon_empty.rvmat","vb_echobase\data\er_neon_empty.rvmat","vb_echobase\data\er_neon_empty.rvmat","vb_echobase\data\er_neon_empty.rvmat","vb_echobase\data\er_neon_empty.rvmat","vb_echobase\data\er_neon_empty.rvmat","vb_echobase\data\er_neon_empty.rvmat","vb_echobase\data\er_neon_empty.rvmat","vb_echobase\data\er_neon_empty.rvmat","vb_echobase\data\er_neon_empty.rvmat","vb_echobase\data\er_neon_empty.rvmat","vb_echobase\data\er_neon_empty.rvmat","vb_echobase\data\er_neon_empty.rvmat","vb_echobase\data\er_neon_empty.rvmat","vb_echobase\data\er_neon_empty.rvmat","vb_echobase\data\er_neon_empty.rvmat","vb_echobase\data\er_neon_empty.rvmat","vb_echobase\data\er_neon_empty.rvmat","vb_echobase\data\er_neon_empty.rvmat","vb_echobase\data\er_neon_empty.rvmat","vb_echobase\data\er_neon_empty.rvmat"};
		// гнёзда разрядки ячеек на боку (идея пользователя 11-20, как ▲ у станции): энергия ячеек — в запас сети,
		// до io kE/tick (слоты ядра, прокси — те же vb_eb_st_cell_d*)
		attachments[]={"VB_Echo_DrainCell1","VB_Echo_DrainCell2"};
		class GUIInventoryAttachmentsProps
		{
			class Drain
			{
				name="$STR_VB_EB_cat_drain";
				description="";
				attachmentSlots[]={"VB_Echo_DrainCell1","VB_Echo_DrainCell2"};
				icon="set:vb_echo_icons image:cell_drain";
			};
		};
		class VB_EchoRating { tier=1; io=2048; };		// 100 000 kE, вход и выход до 2048 kE/tick (шкала ×32, 11-20)
		class VB_Echo
		{
			kind="echo";
			capacity=100000;
			infinite=0;
			transfer=1;
			storage="quantity";
		};
	};
	class VB_EB_ChargeStation: VB_EB_DeviceBase
	{
		scope=2;
		displayName="$STR_VB_EB_station";
		descriptionShort="$STR_VB_EB_station_disc";
		model="VB_EchoBase\data\eb_station.p3d";
		vbKit="VB_EB_Kit_ChargeStation";
		// станция v2 (идея пользователя 11-20): заряжаемое вешается на станцию и видно на модели (прокси), отсека нет.
		// Слоты — в ядре (VB_EchoCore): Charge* заряжает, Drain* разряжает ячейку в запас сети; полная ячейка сама
		// встаёт в разрядку, пустая — в зарядку (скрипт CanReceiveAttachment)
		attachments[]={"VB_Echo_ChargeTank1","VB_Echo_ChargeTank2","VB_Echo_ChargeBattery","VB_Echo_ChargeCell1","VB_Echo_ChargeCell2","VB_Echo_DrainCell1","VB_Echo_DrainCell2"};
		class GUIInventoryAttachmentsProps
		{
			class Charge
			{
				name="$STR_VB_EB_cat_charge";
				description="";
				attachmentSlots[]={"VB_Echo_ChargeTank1","VB_Echo_ChargeTank2","VB_Echo_ChargeBattery","VB_Echo_ChargeCell1","VB_Echo_ChargeCell2"};
				icon="set:vb_echo_icons image:cell_charge";
			};
			class Drain
			{
				name="$STR_VB_EB_cat_drain";
				description="";
				attachmentSlots[]={"VB_Echo_DrainCell1","VB_Echo_DrainCell2"};
				icon="set:vb_echo_icons image:cell_drain";
			};
		};
		// из сети в свой буфер до inMax; каждому гнезду: бак и аккумулятор chargeItem, ячейка chargeCell, разрядка ячейки
		// в сеть drainCell (kE/tick, ТЗ_VB_EchoBase_буферы.md 3.3: ячейки быстрее, разрядка ещё быстрее)
		class VB_EchoRating { tier=1; inMax=2048; chargeItem=256; chargeCell=1024; drainCell=2048; };
		// буфер Эхо (ТЗ_VB_EchoBase_буферы.md): количество предмета, из него станция заряжает вложенное
		varQuantityInit=0;
		varQuantityMin=0;
		varQuantityMax=4096;
		quantityBar=1;
		stackedUnit="percentage";
		class VB_Echo
		{
			kind="echo";
			capacity=4096;
			infinite=0;
			transfer=0;
			storage="quantity";
		};
	};
	class VB_EB_ChargePad: VB_EB_DeviceBase
	{
		scope=2;
		displayName="$STR_VB_EB_pad";
		descriptionShort="$STR_VB_EB_pad_disc";
		model="VB_EchoBase\data\eb_pad.p3d";
		vbKit="VB_EB_Kit_ChargePad";
		slopeTolerance=0.15;							// только на ровное (решение 11-20)
		yawPitchRollLimit[]={10,10,10};
		class VB_EchoRating { tier=1; inMax=256; outMax=128; };	// из сети 256, машине 128 kE/tick (потери 50 %, решение 11-20)
		// буфер Эхо (ТЗ_VB_EchoBase_буферы.md): количество предмета, из него платформа заряжает машины и игроков
		varQuantityInit=0;
		varQuantityMin=0;
		varQuantityMax=2048;
		quantityBar=1;
		stackedUnit="percentage";
		class VB_Echo
		{
			kind="echo";
			capacity=2048;
			infinite=0;
			transfer=0;
			storage="quantity";
		};
	};
	// ---------------- сеть
	class VB_EB_Relay: VB_EB_DeviceBase
	{
		scope=2;
		displayName="$STR_VB_EB_relay";
		descriptionShort="$STR_VB_EB_relay_disc";
		model="VB_EchoBase\data\eb_relay.p3d";
		vbKit="VB_EB_Kit_Relay";
		class VB_EchoRating { tier=1; range=32; };	// м
	};
	// ---------------- расходник преобразователя
	class VB_EB_Crystal: Inventory_Base
	{
		scope=2;
		displayName="$STR_VB_EB_crystal";
		descriptionShort="$STR_VB_EB_crystal_disc";
		model="VB_EchoBase\data\eb_crystal.p3d";
		weight=300;
		itemSize[]={1,2};
		inventorySlot[]={"VB_EB_Crystal"};
		varQuantityInit=100000;			// ресурс 100 000 kE Эхо (5 баков; шкала ×32, 11-20)
		varQuantityMin=0;
		varQuantityMax=100000;
		varQuantityDestroyOnMin=1;		// износился — рассыпался
		quantityBar=1;
		stackedUnit="percentage";		// заряд в процентах, не в штуках (11-20)
		hiddenSelections[]={"neon"};
		hiddenSelectionsTextures[]={"vb_echobase\data\er_neon_co.paa"};
		hiddenSelectionsMaterials[]={"vb_echobase\data\er_neon_idle.rvmat"};
	};
	// донат: не изнашивается (решение пользователя 11-20, вариант 1); бирюзовый
	class VB_EB_CrystalFusion: VB_EB_Crystal
	{
		scope=2;
		displayName="$STR_VB_EB_crystal_fusion";
		descriptionShort="$STR_VB_EB_crystal_fusion_disc";
		varQuantityDestroyOnMin=0;
		hiddenSelectionsTextures[]={"vb_echobase\data\er_neon_fusion_co.paa"};
	};
	// ---------------- наборы: одна модель кейса на все (как кит станка DRL); неон — цвет уровня устройства внутри
	class VB_EB_KitBase: Inventory_Base
	{
		scope=0;
		displayName="$STR_VB_EB_kit";
		descriptionShort="$STR_VB_EB_kit_disc";
		model="VB_EchoBase\data\eb_kit.p3d";
		weight=15000;
		// как ванильный WoodenCrate (решение пользователя 11-20): размер, тяжёлый предмет двумя руками, хват ящика —
		// scripts/4_World/VB_EchoBase/VB_EB_Hands.c (ik heavy/wooden_crate.anm)
		itemSize[]={10,5};
		itemBehaviour=0;
		rotationFlags=2;
		physLayer="item_large";
		soundImpactType="metal";
		hiddenSelections[]={"neon"};
		hiddenSelectionsTextures[]={"vb_echobase\data\eb_neon_t1_co.paa"};
		hiddenSelectionsMaterials[]={"vb_echobase\data\er_neon_idle.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=300;
				};
			};
		};
	};
	class VB_EB_Kit_WindTurbine: VB_EB_KitBase
	{
		scope=2;
		displayName="$STR_VB_EB_kit_wind";
		projectionTypename="VB_EB_WindTurbine";		// голограмма — само устройство (ваниль, Hologram.c)
	};
	class VB_EB_Kit_SolarPanel: VB_EB_KitBase
	{
		scope=2;
		displayName="$STR_VB_EB_kit_solar";
		projectionTypename="VB_EB_SolarPanel";
	};
	class VB_EB_Kit_Furnace: VB_EB_KitBase
	{
		scope=2;
		displayName="$STR_VB_EB_kit_furnace";
		projectionTypename="VB_EB_Furnace";
	};
	class VB_EB_Kit_Converter: VB_EB_KitBase
	{
		scope=2;
		displayName="$STR_VB_EB_kit_converter";
		projectionTypename="VB_EB_Converter";
	};
	class VB_EB_Kit_Storage: VB_EB_KitBase
	{
		scope=2;
		displayName="$STR_VB_EB_kit_storage";
		projectionTypename="VB_EB_Storage";
	};
	class VB_EB_Kit_ChargeStation: VB_EB_KitBase
	{
		scope=2;
		displayName="$STR_VB_EB_kit_station";
		projectionTypename="VB_EB_ChargeStation";
	};
	class VB_EB_Kit_ChargePad: VB_EB_KitBase
	{
		scope=2;
		displayName="$STR_VB_EB_kit_pad";
		projectionTypename="VB_EB_ChargePad";
	};
	class VB_EB_Kit_Relay: VB_EB_KitBase
	{
		scope=2;
		displayName="$STR_VB_EB_kit_relay";
		projectionTypename="VB_EB_Relay";
	};
};
// Прокси станции v2: где рисовать вложенное (модели-заглушки VB_EchoBase/proxy — tools/eb_proxies.py; места —
// tools/eb_models.py station_proxies). На каждый слот свой прокси и класс (E105).
class CfgNonAIVehicles
{
	class ProxyAttachment;
	class Proxyvb_eb_st_tank1: ProxyAttachment
	{
		scope=2;
		inventorySlot[]={"VB_Echo_ChargeTank1"};
		model="VB_EchoBase\proxy\vb_eb_st_tank1.p3d";
	};
	class Proxyvb_eb_st_tank2: ProxyAttachment
	{
		scope=2;
		inventorySlot[]={"VB_Echo_ChargeTank2"};
		model="VB_EchoBase\proxy\vb_eb_st_tank2.p3d";
	};
	class Proxyvb_eb_st_battery: ProxyAttachment
	{
		scope=2;
		inventorySlot[]={"VB_Echo_ChargeBattery"};
		model="VB_EchoBase\proxy\vb_eb_st_battery.p3d";
	};
	class Proxyvb_eb_st_cell_c1: ProxyAttachment
	{
		scope=2;
		inventorySlot[]={"VB_Echo_ChargeCell1"};
		model="VB_EchoBase\proxy\vb_eb_st_cell_c1.p3d";
	};
	class Proxyvb_eb_st_cell_c2: ProxyAttachment
	{
		scope=2;
		inventorySlot[]={"VB_Echo_ChargeCell2"};
		model="VB_EchoBase\proxy\vb_eb_st_cell_c2.p3d";
	};
	class Proxyvb_eb_st_cell_d1: ProxyAttachment
	{
		scope=2;
		inventorySlot[]={"VB_Echo_DrainCell1"};
		model="VB_EchoBase\proxy\vb_eb_st_cell_d1.p3d";
	};
	class Proxyvb_eb_st_cell_d2: ProxyAttachment
	{
		scope=2;
		inventorySlot[]={"VB_Echo_DrainCell2"};
		model="VB_EchoBase\proxy\vb_eb_st_cell_d2.p3d";
	};
};
