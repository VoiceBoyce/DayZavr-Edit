class CfgPatches
{
	class OOTS_Anomaly_Client
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Scripts","DZ_Data","DZ_Characters_Glasses"};
	};
};
class CfgMods
{
	class OOTS_Anomaly_Client
	{
		author = "";
		type = "mod";
		dependencies[] = {"Game","World"};
		class defs
		{
			class imageSets
			{
				files[] = {"OOTS_Anomaly_Client\GUI\imagesets\ootspsy.imageset"};
			};
		};
	};
};
class CfgSlots
{
	class Slot_PsyTower_osnova
	{
		name = "PsyTower_osnova";
		displayName = "Основа ПСИ-Установки";
		ghostIcon = "set:ootspsy image:osnova";
	};
	class Slot_PsyTower_battery
	{
		name = "PsyTower_battery";
		displayName = "Батарея ПСИ-Установки";
		ghostIcon = "set:ootspsy image:battery";
	};
};
class CfgVehicles
{
	class Inventory_Base;
	class Container_Base;
	class OOTS_item_PsyDecoder: Inventory_Base
	{
		scope = 2;
		displayName = "Декодер ПСИ-Установки";
		descriptionShort = "При помощи этого устройства можно отключить ПСИ-Установку";
		model = "OOTS_Anomaly_Client\PsyTower\Model\PsyDecoder.p3d";
		weight = 250;
		itemSize[] = {1,2};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 70;
					healthLevels[] = {{1,{"DZ\gear\tools\data\flashlight_old.rvmat"}},{0.7,{"DZ\gear\tools\data\flashlight_old.rvmat"}},{0.5,{"DZ\gear\tools\data\flashlight_old_damage.rvmat"}},{0.3,{"DZ\gear\tools\data\flashlight_old_damage.rvmat"}},{0,{"DZ\gear\tools\data\flashlight_old_destruct.rvmat"}}};
				};
			};
		};
		class MeleeModes
		{
			class Default
			{
				ammo = "MeleeLightBlunt";
				range = 1;
			};
			class Heavy
			{
				ammo = "MeleeLightBlunt_Heavy";
				range = 1;
			};
			class Sprint
			{
				ammo = "MeleeLightBlunt_Heavy";
				range = 2.8;
			};
		};
		attachments[] = {"BatteryD"};
		repairableWithKits[] = {7};
		repairCosts[] = {25};
		class EnergyManager
		{
			hasIcon = 1;
			autoSwitchOffWhenInCargo = 0;
			energyUsagePerSecond = 0.07;
			plugType = 1;
			attachmentAction = 1;
			updateInterval = 40;
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class Attack_flashlight_Light1
				{
					soundSet = "Char_Attack_pipemetal_light1_SoundSet";
					id = 806;
				};
				class Attack_flashlight_Light2
				{
					soundSet = "Char_Attack_pipemetal_light2_SoundSet";
					id = 807;
				};
				class Attack_flashlight_Heavy1
				{
					soundSet = "Char_Attack_pipemetal_heavy1_SoundSet";
					id = 808;
				};
				class Attack_flashlight_Heavy2
				{
					soundSet = "Char_Attack_pipemetal_heavy2_SoundSet";
					id = 809;
				};
			};
		};
	};
	class OOTS_item_PsyHelmet: Inventory_Base
	{
		scope = 2;
		displayName = "Пси-шлем";
		descriptionShort = "Прототип устройства защиты от пси-излучения. Внешне пси-шлем выглядит как большие наушники с несколькими электродами, касающимися лба, висков и затылка.";
		model = "OOTS_Anomaly_Client\data\objects\PsiShlem\model\PsiHelmet_g.p3d";
		inventorySlot[] = {"HeadGear"};
		rotationFlags = 17;
		weight = 250;
		absorbency = 0.5;
		itemSize[] = {3,3};
		hiddenSelections[] = {"Cloth","Latex","Leather","Metal","Plastic"};
		hiddenSelectionsTextures[] = {"OOTS_Anomaly_Client\data\objects\PsiShlem\date\Save_co.paa","OOTS_Anomaly_Client\data\objects\PsiShlem\date\Save_co.paa","OOTS_Anomaly_Client\data\objects\PsiShlem\date\Save_co.paa","OOTS_Anomaly_Client\data\objects\PsiShlem\date\Save_co.paa","OOTS_Anomaly_Client\data\objects\PsiShlem\date\Save_co.paa"};
		hiddenSelectionsMaterials[] = {"OOTS_Anomaly_Client\data\objects\PsiShlem\date\Cloth.rvmat","OOTS_Anomaly_Client\data\objects\PsiShlem\date\Latex.rvmat","OOTS_Anomaly_Client\data\objects\PsiShlem\date\Leather.rvmat","OOTS_Anomaly_Client\data\objects\PsiShlem\date\Metal.rvmat","OOTS_Anomaly_Client\data\objects\PsiShlem\date\plastic.rvmat"};
		class ClothingTypes
		{
			male = "OOTS_Anomaly_Client\data\objects\PsiShlem\model\PsiHelmet.p3d";
			female = "OOTS_Anomaly_Client\data\objects\PsiShlem\model\PsiHelmet.p3d";
		};
		oldpower = 0;
		simulation = "itemTransmitter";
		inputRange = 8;
		range = 0;
		repairableWithKits[] = {5,7};
		repairCosts[] = {30,25};
		class EnergyManager
		{
			hasIcon = 1;
			autoSwitchOff = 1;
			energyUsagePerSecond = 0;
			plugType = 5;
			attachmentAction = 1;
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 100;
					healthLevels[] = {{1,{"DZ\characters\headgear\data\HelmetMich.rvmat"}},{0.7,{"DZ\characters\headgear\data\HelmetMich.rvmat"}},{0.5,{"DZ\characters\headgear\data\HelmetMich_damage.rvmat"}},{0.3,{"DZ\characters\headgear\data\HelmetMich_damage.rvmat"}},{0,{"DZ\characters\headgear\data\HelmetMich_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health
					{
						damage = 0.75;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0.5;
					};
				};
				class Melee
				{
					class Health
					{
						damage = 0.25;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0.25;
					};
				};
				class Infected
				{
					class Health
					{
						damage = 0.25;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0.25;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage = 0.5;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0.45999998;
					};
				};
			};
		};
	};
	class OOTS_item_PsyBox: Container_Base
	{
		scope = 2;
		displayName = "Ящик для ПСИ-Установки";
		descriptionShort = "В ящике находятся части для размещения ПСИ-Установки";
		model = "OOTS_Anomaly_Client\PsyTower\Model\PsyBox.p3d";
		hologramMaterial = "PsyTower";
		hologramMaterialPath = "OOTS_Anomaly_Client\PsyTower\data";
		weight = 12000;
		itemBehaviour = 0;
		carveNavmesh = 1;
		canBeDigged = 1;
		physLayer = "item_large";
		lootTag[] = {"Work"};
		itemSize[] = {5,8};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 600;
					transferToAttachmentsCoef = 0.06;
					healthLevels[] = {{1,{"DZ\gear\camping\data\sea_chest.rvmat"}},{0.7,{"DZ\gear\camping\data\sea_chest.rvmat"}},{0.5,{"DZ\gear\camping\data\sea_chest_damage.rvmat"}},{0.3,{"DZ\gear\camping\data\sea_chest_damage.rvmat"}},{0,{"DZ\gear\camping\data\sea_chest_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class FragGrenade
				{
					class Health
					{
						damage = 8;
					};
					class Blood
					{
						damage = 8;
					};
					class Shock
					{
						damage = 8;
					};
				};
			};
		};
		class Cargo
		{
			itemsCargoSize[] = {0,0};
			openable = 1;
			allowOwnedCargoManipulation = 1;
		};
		attachments[] = {"PsyTower_osnova","PsyTower_battery"};
	};
	class OOTS_item_PsyTower: Inventory_Base
	{
		scope = 2;
		displayName = "ПСИ-Установка";
		descriptionShort = "Элемент вышек Монолита, а точнее ПСИ-Установка.";
		model = "OOTS_Anomaly_Client\PsyTower\Model\PsyTower.p3d";
		allowOwnedCargoManipulation = 1;
		overrideDrawArea = 8;
		forceFarBubble = "true";
		destroyOnEmpty = 0;
		quantityBar = 0;
		varQuantityMax = 0;
		carveNavmesh = 1;
		canBeDigged = 0;
		heavyItem = 1;
		itemBehaviour = 0;
		physLayer = "item_large";
		lootTag[] = {"Work"};
		weight = 10000;
		itemSize[] = {4,7};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 10000;
					transferToAttachmentsCoef = 0.06;
					healthLevels[] = {{1,{"OOTS_Anomaly_Client\PsyTower\data\PsyTower.rvmat"}},{0.7,{"OOTS_Anomaly_Client\PsyTower\data\PsyTower.rvmat"}},{0.5,{"OOTS_Anomaly_Client\PsyTower\data\PsyTower_damage.rvmat"}},{0.3,{"OOTS_Anomaly_Client\PsyTower\data\PsyTower_damage.rvmat"}},{0,{"OOTS_Anomaly_Client\PsyTower\data\PsyTower_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class FragGrenade
				{
					class Health
					{
						damage = 8;
					};
					class Blood
					{
						damage = 8;
					};
					class Shock
					{
						damage = 8;
					};
				};
			};
		};
	};
	class OOTS_item_PsyTower_osnova: Inventory_Base
	{
		scope = 2;
		displayName = "Основание ПСИ-Установки";
		descriptionShort = "Элемент ПСИ-Установки";
		model = "OOTS_Anomaly_Client\PsyTower\Model\osnova.p3d";
		allowOwnedCargoManipulation = 1;
		overrideDrawArea = 8;
		forceFarBubble = "true";
		destroyOnEmpty = 0;
		quantityBar = 0;
		varQuantityMax = 0;
		carveNavmesh = 1;
		canBeDigged = 0;
		heavyItem = 1;
		itemBehaviour = 0;
		physLayer = "item_large";
		lootTag[] = {"Work"};
		weight = 6000;
		itemSize[] = {3,6};
		inventorySlot[] = {"PsyTower_osnova"};
	};
	class OOTS_item_PsyTower_battery: Inventory_Base
	{
		scope = 2;
		displayName = "Батарея ПСИ-Установки";
		descriptionShort = "Элемент ПСИ-Установки";
		model = "OOTS_Anomaly_Client\PsyTower\Model\battery.p3d";
		allowOwnedCargoManipulation = 1;
		overrideDrawArea = 8;
		forceFarBubble = "true";
		destroyOnEmpty = 0;
		quantityBar = 0;
		varQuantityMax = 0;
		carveNavmesh = 1;
		canBeDigged = 0;
		heavyItem = 1;
		itemBehaviour = 0;
		physLayer = "item_large";
		lootTag[] = {"Work"};
		weight = 4000;
		itemSize[] = {3,3};
		inventorySlot[] = {"PsyTower_battery"};
	};
	class OOTS_Item_Bolt: Inventory_Base
	{
		scope = 2;
		displayName = "Болт";
		descriptionShort = "металлическое крепёжное изделие в виде стержня с наружной резьбой и шестигранной головкой под гаечный ключ. Используется сталкерами для обнаружения, определения границ и разрядки аномалий, а также для обнаружения мин.";
		model = "OOTS_Anomaly_Client\Bolt\bolt.p3d";
		weight = 30;
		count = 20;
		itemSize[] = {1,1};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 80;
					healthLevels[] = {{1,{"OOTS_Anomaly_Client\Bolt\bolt.rvmat"}},{0.7,{"OOTS_Anomaly_Client\Bolt\bolt.rvmat"}},{0.5,{"OOTS_Anomaly_Client\Bolt\bolt.rvmat"}},{0.3,{"OOTS_Anomaly_Client\Bolt\bolt.rvmat"}},{0,{"OOTS_Anomaly_Client\Bolt\bolt.rvmat"}}};
				};
			};
		};
	};
	class HouseNoDestruct;
	class OOTS_AnomalyBase: HouseNoDestruct
	{
		scope = 0;
		model = "\OOTS_Anomaly_Client\graphics\particles\EmptySphere2.p3d";
	};
	class Anomaly_Electra: OOTS_AnomalyBase
	{
		scope = 1;
	};
	class Anomaly_Studen: OOTS_AnomalyBase
	{
		scope = 1;
	};
	class Anomaly_Tramplin: OOTS_AnomalyBase
	{
		scope = 1;
	};
	class Anomaly_Voronka: OOTS_AnomalyBase
	{
		scope = 1;
	};
	class Anomaly_Zharka: OOTS_AnomalyBase
	{
		scope = 1;
	};
	class Anomaly_Teleport_Entry: OOTS_AnomalyBase
	{
		scope = 1;
	};
	class Anomaly_Teleport_Output: OOTS_AnomalyBase
	{
		scope = 1;
	};
	class Anomaly_Teleport_Dot: OOTS_AnomalyBase
	{
		scope = 1;
	};
	class Anomaly_EMP_Blast: OOTS_AnomalyBase
	{
		scope = 1;
	};
	class Anomaly_Steklo: OOTS_AnomalyBase
	{
		scope = 1;
	};
	class Anomaly_DeadZone: OOTS_AnomalyBase
	{
		scope = 1;
	};
	class Anomaly_Zastoy: OOTS_AnomalyBase
	{
		scope = 1;
	};
	class Anomaly_Snow: OOTS_AnomalyBase
	{
		scope = 1;
	};
	class Anomaly_Par: OOTS_AnomalyBase
	{
		scope = 1;
	};
	class Anomaly_Groza: OOTS_AnomalyBase
	{
		scope = 1;
	};
	class Anomaly_BlackHole: OOTS_AnomalyBase
	{
		scope = 1;
	};
	class Anomaly_Atom: OOTS_AnomalyBase
	{
		scope = 1;
	};
	class Anomaly_Lince: OOTS_AnomalyBase
	{
		scope = 1;
	};
	class Anomaly_LinceAgr: OOTS_AnomalyBase
	{
		scope = 1;
	};
	class Anomaly_Holera: OOTS_AnomalyBase
	{
		scope = 1;
	};
	class Anomaly_FireShtorm: OOTS_AnomalyBase
	{
		scope = 0;
	};
	class Anomaly_Lomka: OOTS_AnomalyBase
	{
		scope = 0;
	};
	class Anomaly_Roy: OOTS_AnomalyBase
	{
		scope = 1;
	};
	class Anomaly_Flash: OOTS_AnomalyBase
	{
		scope = 1;
	};
	class Anomaly_FireIskra: OOTS_AnomalyBase
	{
		scope = 1;
	};
	class Anomaly_MlechniPut: OOTS_AnomalyBase
	{
		scope = 1;
	};
	class VitaminBottle;
	class Edible_Base;
	class PainkillerTablets;
	class OOTS_Medicine_PsiBlocade: PainkillerTablets
	{
		scope = 2;
		displayName = "Пси-блокада";
		descriptionShort = "Наркотический препарат, блокирующий лавинообразные нервные импульсы. Интересен побочный эффект: после приёма препарата наблюдается кратковременное исчезновение каких-либо эмоций. В Зоне используется для противодействия аномальным полям повышенной пси-активности; за её пределами применения не нашёл. Время действия в течении 30 сек.";
		model = "OOTS_Anomaly_Client\Data\Objects\PsyBlocade\PsyBlocade.p3d";
		rotationFlags = 17;
		itemSize[] = {1,2};
		varQuantityInit = 10;
		varQuantityMin = 0;
		varQuantityMax = 10;
		varQuantityDestroyOnMin = 1;
		weight = 20;
		stackedUnit = "pills";
		quantityBar = 0;
		absorbency = 0.7;
		hiddenSelections[] = {"como","glass"};
		hiddenSelectionsTextures[] = {"OOTS_Anomaly_Client\data\objects\PsyBlocade\PsyBlocade_co.paa","OOTS_Anomaly_Client\data\objects\PsyBlocade\PsyBlocade_ca.paa"};
		hiddenSelectionsMaterials[] = {"OOTS_Anomaly_Client\data\objects\PsyBlocade\PsyBlocade.rvmat","OOTS_Anomaly_Client\data\objects\PsyBlocade\PsyBlocade.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 30;
					healthLevels[] = {{1,{"DZ\gear\medical\data\painkiller2.rvmat"}},{0.7,{"DZ\gear\medical\data\painkiller2.rvmat"}},{0.5,{"DZ\gear\medical\data\painkiller2_damage.rvmat"}},{0.3,{"DZ\gear\medical\data\painkiller2_damage.rvmat"}},{0,{"DZ\gear\medical\data\painkiller2_destruct.rvmat"}}};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class Tablets_open
				{
					soundSet = "Tablets_open_SoundSet";
					id = 201;
				};
				class Tablets_catch
				{
					soundSet = "Tablets_catch_SoundSet";
					id = 202;
				};
				class Tablets_close
				{
					soundSet = "Tablets_close_SoundSet";
					id = 203;
				};
				class pickup
				{
					soundSet = "purificationtablets_pickup_SoundSet";
					id = 797;
				};
				class drop
				{
					soundset = "purificationtablets_drop_SoundSet";
					id = 898;
				};
			};
		};
	};
	class OOTS_Medicine_Anabiotik: VitaminBottle
	{
		scope = 2;
		displayName = "Анабиотик";
		descriptionShort = "Экспериментальный препарат, разработанный для научно-исследовательских групп, действующих в Зоне. Входящий в формулу препарата тетрадотоксин, известный как «порошок зомби», вызывает полный паралич центральной нервной системы; предположительно, это может обеспечить выживание вне укрытия во время выброса. Препарат не испытывался в полевых условиях и не прошёл клинических испытаний, что ставит под сомнение его эффективность и заставляет задуматься о возможных побочных эффектах. Время действия препарата в пределах 10 минут.";
		model = "OOTS_Anomaly_Client\Data\Objects\Anabiotik\Anabiotik.p3d";
		varQuantityInit = 5;
		varQuantityMin = 0;
		varQuantityMax = 5;
		varQuantityDestroyOnMin = 1;
		itemSize[] = {1,2};
		weight = 20;
		stackedUnit = "pills";
		quantityBar = 0;
		hiddenSelections[] = {"como","glass","nak"};
		hiddenSelectionsTextures[] = {"OOTS_Anomaly_Client\data\objects\Anabiotik\Anabiotik_co.paa","OOTS_Anomaly_Client\data\objects\Anabiotik\Anabiotik_ca.paa","OOTS_Anomaly_Client\data\objects\Anabiotik\Nak_co.paa"};
		hiddenSelectionsMaterials[] = {"OOTS_Anomaly_Client\data\objects\Anabiotik\Anabiotik.rvmat","OOTS_Anomaly_Client\data\objects\Anabiotik\Anabiotik.rvmat","OOTS_Anomaly_Client\data\objects\Anabiotik\nak.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 50;
					healthLevels[] = {{1,{"DZ\gear\medical\data\vitamin_bottle.rvmat"}},{0.7,{"DZ\gear\medical\data\vitamin_bottle.rvmat"}},{0.5,{"DZ\gear\medical\data\vitamin_bottle_damage.rvmat"}},{0.3,{"DZ\gear\medical\data\vitamin_bottle_damage.rvmat"}},{0,{"DZ\gear\medical\data\vitamin_bottle_destruct.rvmat"}}};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class VitaminBottle_open
				{
					soundSet = "VitaminBottle_open_SoundSet";
					id = 201;
				};
				class VitaminBottle_shake
				{
					soundSet = "VitaminBottle_shake_SoundSet";
					id = 202;
				};
				class VitaminBottle_close
				{
					soundSet = "VitaminBottle_close_SoundSet";
					id = 203;
				};
				class pickUpItem
				{
					soundSet = "vitaminbottle_pickup_SoundSet";
					id = 797;
				};
			};
		};
	};
	class AviatorGlasses;
	class BlowOut_DebugGlasses: AviatorGlasses
	{
		scope = 2;
		displayName = "BlowOut_DebugGlasses";
		descriptionShort = "Put on yourseff to see BlowOut HideZone Triggers";
	};
	class BlowOut_HideZoneBase: HouseNoDestruct
	{
		scope = 0;
	};
	class BlowOut_HideZone_2m: BlowOut_HideZoneBase
	{
		scope = 2;
	};
	class BlowOut_HideZone_5m: BlowOut_HideZoneBase
	{
		scope = 2;
	};
	class BlowOut_HideZone_10m: BlowOut_HideZoneBase
	{
		scope = 2;
	};
	class BlowOut_DebugStone: Inventory_Base
	{
		scope = 1;
		displayName = "Small Trigger Debug Stone";
		descriptionShort = "Displays trigger vertices";
		model = "\dz\gear\consumables\SmallStone.p3d";
		animClass = "NoFireClass";
		weight = 500;
		itemSize[] = {1,1};
		rotationFlags = 17;
		storageCategory = 10;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 100;
					healthLevels[] = {{1,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0,{}}};
				};
			};
		};
	};
	class BlowOut_Set_Phase: HouseNoDestruct
	{
		scope = 0;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 100;
					healthLevels[] = {{1,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0,{}}};
				};
			};
		};
	};
	class BlowOut_Set_Warning: BlowOut_Set_Phase
	{
		scope = 2;
	};
	class BlowOut_Set_Active: BlowOut_Set_Phase
	{
		scope = 2;
	};
	class BlowOut_Set_Final: BlowOut_Set_Phase
	{
		scope = 2;
	};
	class PsiStorm_Set_Phase: HouseNoDestruct
	{
		scope = 0;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 100;
					healthLevels[] = {{1,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0,{}}};
				};
			};
		};
	};
	class PsiStorm_Set_Warning: PsiStorm_Set_Phase
	{
		scope = 2;
	};
	class PsiStorm_Set_Active: PsiStorm_Set_Phase
	{
		scope = 2;
	};
	class PsiStorm_Set_Final: PsiStorm_Set_Phase
	{
		scope = 2;
	};
};
class CfgSounds
{
	class default
	{
		name = "";
		titles[] = {};
	};
	class teleport_idle: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\anomalies\teleport_idle",3,1,80};
	};
	class teleport_incoming: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\anomalies\teleport_incoming",3,1,50};
	};
	class teleport_work: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\anomalies\teleport_work_2",3,1,50};
	};
	class electra_active: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\anomalies\electra_active",3,1,50};
	};
	class electra_idle: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\anomalies\electra_idle",3,1,30};
	};
	class studen_idle: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\anomalies\studen_idle",2,1,30};
	};
	class studen_active: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\anomalies\studen_active",3,1,30};
	};
	class zharka_idle: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\anomalies\zharka_idle",3,1,50};
	};
	class zharka_active: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\anomalies\zharka_active",5,1,50};
	};
	class tramplin_idle: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\anomalies\tramplin_idle",1,1,50};
	};
	class tramplin_active: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\anomalies\tramplin_active",6,1,50};
	};
	class voronka_idle: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\anomalies\voronka_idle",1,1,50};
	};
	class voronka_active: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\anomalies\voronka_active",6,1,100};
	};
	class emp_blast: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\anomalies\emp_blast",10,1,30};
	};
	class emp_blast_IDLE: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\anomalies\emp_blast_IDLE",10,1,100};
	};
	class Steklo_IDLE: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\anomalies\Steklo_IDLE",10,1,30};
	};
	class Steklo_ACTIVE: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\anomalies\Steklo_ACTIVE",10,1,25};
	};
	class DeadZone_IDLE: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\anomalies\DeadZone_IDLE",10,1,70};
	};
	class DeadZone_ACTIVE: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\anomalies\DeadZone_ACTIVE",10,1,50};
	};
	class Zastoy_IDLE: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\anomalies\Zastoy_IDLE",5,0.5,30};
	};
	class Zastoy_ACTIVE: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\anomalies\Zastoy_ACTIVE",7,0.75,50};
	};
	class Snow_IDLE: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\anomalies\Snow_IDLE",15,1,50};
	};
	class Snow_ACTIVE: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\anomalies\Snow_ACTIVE",10,1,70};
	};
	class par_idle: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\anomalies\par_idle",5,1,60};
	};
	class par_active: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\anomalies\par_active",7,1,70};
	};
	class Groza_IDLE: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\anomalies\Groza_IDLE",15,1,40};
	};
	class Groza_ACTIVE: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\anomalies\Groza_ACTIVE",10,1,60};
	};
	class BlackHole_IDLE: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\anomalies\BlackHole_IDLE",15,1,40};
	};
	class BlackHole_ACTIVE: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\anomalies\BlackHole_ACTIVE",10,1,60};
	};
	class Atom_IDLE: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\anomalies\Atom_IDLE",15,1,15};
	};
	class Atom_ACTIVE: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\anomalies\Atom_ACTIVE",25,1,100};
	};
	class Lince_idle: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\anomalies\Lince_idle",5,1,30};
	};
	class Lince_active: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\anomalies\Lince_active",7,1,30};
	};
	class LinceAgr_Active: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\anomalies\LinceAgr_Active",7,1,30};
	};
	class Holera_idle: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\anomalies\Holera_idle",5,1,30};
	};
	class Holera_active: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\anomalies\Holera_active",7,1,30};
	};
	class Roy_idle: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\anomalies\Roy_idle",5,1,30};
	};
	class Roy_active: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\anomalies\Roy_active",7,1,30};
	};
	class Flash_idle: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\anomalies\Flash_idle",5,1,30};
	};
	class Flash_active: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\anomalies\Flash_active",7,1,30};
	};
	class FireIskra_idle: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\anomalies\FireIskra_idle",5,1,30};
	};
	class FireIskra_active: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\anomalies\FireIskra_active",7,1,30};
	};
	class MlechniPut_idle: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\anomalies\MlechniPut_idle",5,1,30};
	};
	class MlechniPut_active: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\anomalies\MlechniPut_active",7,1,30};
	};
	class detector_spawn: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\detectors\detector_spawn",6,1,10};
	};
	class detector_close: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\detectors\detector_close",6,1,10};
	};
	class detector_middle: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\detectors\detector_middle",6,1,10};
	};
	class detector_spawn_2: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\detectors\detector_spawn_2",6,1,10};
	};
	class detector_close_2: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\detectors\detector_close_2",6,1,10};
	};
	class detector_middle_2: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\detectors\detector_middle_2",6,1,10};
	};
	class artefact_spawn: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\detectors\artefact_spawn",6,1,12};
	};
	class bo_warning_1: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\blowout\warning_1",10,1,30};
	};
	class bo_warning_2: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\blowout\warning_2",10,1,60};
	};
	class bo_warning_3: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\blowout\warning_3",10,1,60};
	};
	class bo_active_1: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\blowout\start_1",10,1,60};
	};
	class bo_active_2: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\blowout\start_2",10,1,60};
	};
	class bo_active_3: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\blowout\start_3",10,1,60};
	};
	class bo_final: default
	{
		sound[] = {"\OOTS_Anomaly_Client\data\sounds\blowout\final",10,1,60};
	};
};
class CfgSoundSets
{
	class PsiStorm_1_SoundSet
	{
		soundShaders[] = {"PsiStorm_1_SoundShader"};
		sound3DProcessingType = "infected3DProcessingType";
		volumeCurve = "infectedAttenuationCurve";
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class PsiStorm_2_SoundSet
	{
		soundShaders[] = {"PsiStorm_2_SoundShader"};
		sound3DProcessingType = "infected3DProcessingType";
		volumeCurve = "infectedAttenuationCurve";
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class PsiStorm_HeadVoice_1_SoundSet
	{
		soundShaders[] = {"PsiStorm_HeadVoice_1_SoundShader"};
		sound3DProcessingType = "infected3DProcessingType";
		volumeCurve = "infectedAttenuationCurve";
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class PsiStorm_HeadVoice_2_SoundSet
	{
		soundShaders[] = {"PsiStorm_HeadVoice_2_SoundShader"};
		sound3DProcessingType = "infected3DProcessingType";
		volumeCurve = "infectedAttenuationCurve";
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class BlowOutWarning_SoundSet
	{
		soundShaders[] = {"BlowOutWarning_SoundShader"};
		sound3DProcessingType = "infected3DProcessingType";
		volumeCurve = "infectedAttenuationCurve";
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class BlowOutWarning1_SoundSet
	{
		soundShaders[] = {"BlowOutWarning1_SoundShader"};
		sound3DProcessingType = "infected3DProcessingType";
		volumeCurve = "infectedAttenuationCurve";
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class BlowOutWarning2_SoundSet
	{
		soundShaders[] = {"BlowOutWarning2_SoundShader"};
		sound3DProcessingType = "infected3DProcessingType";
		volumeCurve = "infectedAttenuationCurve";
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class BlowOutWarning3_SoundSet
	{
		soundShaders[] = {"BlowOutWarning3_SoundShader"};
		sound3DProcessingType = "infected3DProcessingType";
		volumeCurve = "infectedAttenuationCurve";
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class BlowOutWarningMonolith_SoundSet
	{
		soundShaders[] = {"BlowOutWarningMonolith_SoundShader"};
		sound3DProcessingType = "infected3DProcessingType";
		volumeCurve = "infectedAttenuationCurve";
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class BlowOutActive_SoundSet
	{
		soundShaders[] = {"BlowOutActive_SoundShader"};
		sound3DProcessingType = "infected3DProcessingType";
		volumeCurve = "infectedAttenuationCurve";
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class BlowOutActive1_SoundSet
	{
		soundShaders[] = {"BlowOutActive1_SoundShader"};
		sound3DProcessingType = "infected3DProcessingType";
		volumeCurve = "infectedAttenuationCurve";
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class BlowOutActive2_SoundSet
	{
		soundShaders[] = {"BlowOutActive2_SoundShader"};
		sound3DProcessingType = "infected3DProcessingType";
		volumeCurve = "infectedAttenuationCurve";
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class BlowOutActive3_SoundSet
	{
		soundShaders[] = {"BlowOutActive3_SoundShader"};
		sound3DProcessingType = "infected3DProcessingType";
		volumeCurve = "infectedAttenuationCurve";
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class BlowOutActiveMonolith_SoundSet
	{
		soundShaders[] = {"BlowOutActiveMonolith_SoundShader"};
		sound3DProcessingType = "infected3DProcessingType";
		volumeCurve = "infectedAttenuationCurve";
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class BlowOutFinal_SoundSet
	{
		soundShaders[] = {"BlowOutFinal_SoundShader"};
		sound3DProcessingType = "infected3DProcessingType";
		volumeCurve = "infectedAttenuationCurve";
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class BlowOutFinalMonolith_SoundSet
	{
		soundShaders[] = {"BlowOutFinalMonolith_SoundShader"};
		sound3DProcessingType = "infected3DProcessingType";
		volumeCurve = "infectedAttenuationCurve";
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class BlowOutMonolithPray1_SoundSet
	{
		soundShaders[] = {"BlowOutMonolithPray1_SoundShader"};
		sound3DProcessingType = "infected3DProcessingType";
		volumeCurve = "infectedAttenuationCurve";
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class BlowOutMonolithPray2_SoundSet
	{
		soundShaders[] = {"BlowOutMonolithPray2_SoundShader"};
		sound3DProcessingType = "infected3DProcessingType";
		volumeCurve = "infectedAttenuationCurve";
		spatial = 1;
		doppler = 0;
		loop = 0;
	};
	class BlowOutStart_SoundSet
	{
		soundShaders[] = {"BlowOutStart_SoundShader"};
		sound3DProcessingType = "infected3DProcessingType";
		volumeCurve = "infectedAttenuationCurve";
		spatial = 1;
		doppler = 0;
		loop = 0;
		volumeFactor = 0.95;
		volumeRandomizer = 0.2;
		frequencyFactor = 1;
		frequencyRandomizer = 2;
	};
	class EarthCollapse_SoundSet
	{
		soundShaders[] = {"EarthCollapse_SoundShader"};
		sound3DProcessingType = "infected3DProcessingType";
		volumeCurve = "infectedAttenuationCurve";
		spatial = 1;
		doppler = 0;
		loop = 1;
		volumeFactor = 0.95;
		volumeRandomizer = 0.2;
		frequencyFactor = 1;
		frequencyRandomizer = 2;
	};
	class WarningRumble_SoundSet
	{
		soundShaders[] = {"WarningRumble_SoundShader"};
		sound3DProcessingType = "infected3DProcessingType";
		volumeCurve = "infectedAttenuationCurve";
		spatial = 1;
		doppler = 0;
		loop = 1;
		volumeFactor = 0.95;
		volumeRandomizer = 0.2;
		frequencyFactor = 1;
		frequencyRandomizer = 2;
	};
	class RumbleHit_1_SoundSet
	{
		soundShaders[] = {"RumbleHit_1_SoundShader"};
		sound3DProcessingType = "infected3DProcessingType";
		volumeCurve = "infectedAttenuationCurve";
		spatial = 1;
		doppler = 0;
		loop = 0;
		volumeFactor = 0.95;
		volumeRandomizer = 0.2;
		frequencyFactor = 1;
		frequencyRandomizer = 2;
	};
	class RumbleHit_2_SoundSet
	{
		soundShaders[] = {"RumbleHit_2_SoundShader"};
		sound3DProcessingType = "infected3DProcessingType";
		volumeCurve = "infectedAttenuationCurve";
		spatial = 1;
		doppler = 0;
		loop = 0;
		volumeFactor = 0.95;
		volumeRandomizer = 0.2;
		frequencyFactor = 1;
		frequencyRandomizer = 2;
	};
	class ThunderHit_SoundSet
	{
		soundShaders[] = {"ThunderHit_SoundShader"};
		sound3DProcessingType = "infected3DProcessingType";
		volumeCurve = "infectedAttenuationCurve";
		spatial = 1;
		doppler = 0;
		loop = 0;
		volumeFactor = 0.95;
		volumeRandomizer = 0.2;
		frequencyFactor = 1;
		frequencyRandomizer = 2;
	};
	class PsiInfluence_SoundSet
	{
		soundShaders[] = {"PsiInfluence_SoundShader"};
	};
	class ArtSpawn_SoundSet
	{
		soundShaders[] = {"ArtSpawn_SoundShader"};
	};
};
class CfgSoundShaders
{
	class PsiStorm_1_SoundShader
	{
		samples[] = {{"\OOTS_Anomaly_Client\data\sounds\psistorm\storm1",1}};
		volume = 1;
		range = 2000;
	};
	class PsiStorm_2_SoundShader
	{
		samples[] = {{"\OOTS_Anomaly_Client\data\sounds\psistorm\storm2",1}};
		volume = 1;
		range = 2000;
	};
	class PsiStorm_HeadVoice_1_SoundShader
	{
		samples[] = {{"\OOTS_Anomaly_Client\data\sounds\psistorm\headvoice_1",1}};
		volume = 1;
		range = 10;
	};
	class PsiStorm_HeadVoice_2_SoundShader
	{
		samples[] = {{"\OOTS_Anomaly_Client\data\sounds\psistorm\headvoice_2",1}};
		volume = 1;
		range = 10;
	};
	class BlowOutWarning_SoundShader
	{
		samples[] = {{"\OOTS_Anomaly_Client\data\sounds\blowout\warning",1}};
		volume = 0.7;
		range = 2000;
	};
	class BlowOutWarning1_SoundShader
	{
		samples[] = {{"\OOTS_Anomaly_Client\data\sounds\blowout\warning_1",1}};
		volume = 10;
		range = 2000;
	};
	class BlowOutWarning2_SoundShader
	{
		samples[] = {{"\OOTS_Anomaly_Client\data\sounds\blowout\warning_2",1}};
		volume = 10;
		range = 2000;
	};
	class BlowOutWarning3_SoundShader
	{
		samples[] = {{"\OOTS_Anomaly_Client\data\sounds\blowout\warning_3",1}};
		volume = 10;
		range = 2000;
	};
	class BlowOutWarningMonolith_SoundShader
	{
		samples[] = {{"\OOTS_Anomaly_Client\data\sounds\blowout\warning_monolith",1}};
		volume = 0.7;
		range = 2000;
	};
	class BlowOutActive_SoundShader
	{
		samples[] = {{"\OOTS_Anomaly_Client\data\sounds\blowout\active",1}};
		volume = 0.7;
		range = 200;
	};
	class BlowOutActive1_SoundShader
	{
		samples[] = {{"\OOTS_Anomaly_Client\data\sounds\blowout\start_1",1}};
		volume = 10;
		range = 200;
	};
	class BlowOutActive2_SoundShader
	{
		samples[] = {{"\OOTS_Anomaly_Client\data\sounds\blowout\start_2",1}};
		volume = 10;
		range = 200;
	};
	class BlowOutActive3_SoundShader
	{
		samples[] = {{"\OOTS_Anomaly_Client\data\sounds\blowout\start_3",1}};
		volume = 10;
		range = 200;
	};
	class BlowOutActiveMonolith_SoundShader
	{
		samples[] = {{"\OOTS_Anomaly_Client\data\sounds\blowout\active_monolith",1}};
		volume = 0.7;
		range = 200;
	};
	class BlowOutFinal_SoundShader
	{
		samples[] = {{"\OOTS_Anomaly_Client\data\sounds\blowout\final",1}};
		volume = 0.7;
		range = 200;
	};
	class BlowOutFinalMonolith_SoundShader
	{
		samples[] = {{"\OOTS_Anomaly_Client\data\sounds\blowout\final_monolith",1}};
		volume = 0.7;
		range = 200;
	};
	class BlowOutMonolithPray1_SoundShader
	{
		samples[] = {{"\OOTS_Anomaly_Client\data\sounds\blowout\monolith_pray_1",1}};
		volume = 1.5;
		range = 200;
	};
	class BlowOutMonolithPray2_SoundShader
	{
		samples[] = {{"\OOTS_Anomaly_Client\data\sounds\blowout\monolith_pray_2",1}};
		volume = 1.5;
		range = 200;
	};
	class BlowOutStart_SoundShader
	{
		samples[] = {{"\OOTS_Anomaly_Client\data\sounds\blowout\start",1}};
		volume = 2;
		range = 2000;
	};
	class EarthCollapse_SoundShader
	{
		samples[] = {{"\OOTS_Anomaly_Client\data\sounds\blowout\earth_collapse",1}};
		volume = 0.8;
		range = 200;
	};
	class WarningRumble_SoundShader
	{
		samples[] = {{"\OOTS_Anomaly_Client\data\sounds\blowout\warning_rumble",1}};
		volume = 0.6;
		range = 200;
	};
	class RumbleHit_1_SoundShader
	{
		samples[] = {{"\OOTS_Anomaly_Client\data\sounds\blowout\rumble_hit1",1}};
		volume = 0.98;
		range = 200;
	};
	class RumbleHit_2_SoundShader
	{
		samples[] = {{"\OOTS_Anomaly_Client\data\sounds\blowout\rumble_hit2",1}};
		volume = 0.98;
		range = 200;
	};
	class ThunderHit_SoundShader
	{
		samples[] = {{"\OOTS_Anomaly_Client\data\sounds\blowout\thunder",1}};
		volume = 1.5;
		range = 200;
	};
	class PsiInfluence_SoundShader
	{
		samples[] = {{"\OOTS_Anomaly_Client\data\sounds\blowout\psi_influence",1}};
		range = 30;
		volume = 5.5;
	};
	class ArtSpawn_SoundShader
	{
		samples[] = {{"\OOTS_Anomaly_Client\data\sounds\detectors\artefact_spawn",1}};
		range = 12;
		volume = 6;
	};
};
