// VB_EchoClothes — одежда семейства VB_Echo (ТЗ: modding/mod_vb_Echo/ТЗ_VB_EchoClothes.md, гайд: ГАЙД.md).
// Экзоскелет Эхо. Этап 1 (11-23): перенесён из наших модов под свои имена —
//   шлем   — IMP_Helmet_RoninHeavy_Black (IMPGMOD);
//   маска  — OOTS_Mask_MaskGhost; жилет, куртка, штаны, перчатки, ботинки — OOTS_*_Exo;
//   сервопривод — OOTS_Exo_Servoprivod_black_BeltSlot_120 (наша заплатка OOTS_ClothesPack_DayZavr_Expansion).
// Модели, текстуры, материалы — в exo_set (tools/ec_import.py: пути в моделях заменены строкой той же длины).
// От старых модов не зависит: вложения со слотами OOTS (батарея и ArtCase сервопривода, модули Clothes*Mod куртки,
// ArmorPlateFront/Back жилета) и логика OOTS (playerHandlerType="Exo") не перенесены.
// Этап 2 (11-23, ТЗ раздел 6): характеристики по схеме сетов одежды DayZavr (вещи _vb), на ступень выше — броня ×0,7
// от сетов, прочность ×1,5, вместимость +20; вес вещей нормальный (без отрицательного веса OOTS), помощь каркаса —
// скриптом (scripts/4_World/VB_EchoClothes/VB_EC_Exo.c, множители — class CfgVB_EchoClothes ниже). Полный комплект —
// 7 вещей с меткой vbExo=1 (шлем, маска, жилет, куртка, штаны, перчатки, ботинки). Сервопривод в комплект НЕ входит
// (решение пользователя 11-23): самостоятельный модуль на энергии Эхо, носится с любой одеждой (этап 4).

class CfgPatches
{
	class VB_EchoClothes
	{
		units[]={"VB_EC_Exo_Helmet","VB_EC_Exo_Mask","VB_EC_Exo_Vest","VB_EC_Exo_Servo","VB_EC_Exo_Jacket","VB_EC_Exo_Pants","VB_EC_Exo_Gloves","VB_EC_Exo_Boots"};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]={"DZ_Data","DZ_Scripts","DZ_Characters","DZ_Characters_Headgear","DZ_Characters_Masks","DZ_Characters_Vests","DZ_Characters_Tops","DZ_Characters_Pants","DZ_Characters_Gloves","DZ_Characters_Shoes","DZ_Characters_Backpacks","VB_EchoCore"};
	};
};
class CfgMods
{
	class VB_EchoClothes
	{
		dir="VB_EchoClothes";
		picture="";
		action="";
		hideName=1;
		hidePicture=1;
		name="VB_EchoClothes";
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
				files[]={"VB_EchoClothes/scripts/4_World"};
			};
		};
	};
};

// Бонусы экзо (скрипт VB_EC_Exo.c читает отсюда, баланс — без правки скриптов). Сеты _vb для сравнения:
// карманы 0,5; падение 0,6; голод/жажда 0,7; кровь/здоровье 1,5; выносливость 0,75.
class CfgVB_EchoClothes
{
	class Exo
	{
		contentsWeight=0.4;		// содержимое карманов вещи экзо весит 40 %
		setOwnWeight=0.4;		// в полном комплекте и сами надетые вещи комплекта весят 40 % (каркас держит себя)
		fallHeight=0.5;			// ботинки экзо: высота падения ×0,5
		setHungerThirst=0.6;	// полный комплект: голод и жажда ×0,6
		setBlood=1.75;			// полный комплект: восстановление крови ×1,75
		setHealth=1.75;			// полный комплект: восстановление здоровья ×1,75
		setStamina=0.6;			// полный комплект: расход выносливости ×0,6
		// слоты полного комплекта (сервопривода нет: он отдельный модуль)
		setSlots[]={"Headgear","Mask","Body","Vest","Legs","Gloves","Feet"};
	};
	// Сервопривод (этап 4, 11-23, ТЗ раздел 7): работает сам, когда надет на пояс и в гнёздах есть энергия Эхо
	// (энергоячейки ядра, синтез — бесконечно); с любой одеждой. Числа «чуть увеличены» по просьбе пользователя.
	class Servo
	{
		assistKg=50;			// снимает с нагрузки бойца (запас выносливости считается, как будто груза меньше), кг
		assistKgFullExo=85;		// с полным комплектом экзо («полный экзоскелет»)
		staminaUse=0.8;			// расход выносливости ×
		contentsWeight=0.4;		// включённый: сам весит 0, содержимое — 40 %
		fallHeightFullExo=0.3;	// полный экзоскелет: высота падения × (у ботинок экзо без сервопривода — 0,5)
		drainBase=2;			// kE/tick, пока работает
		drainPerKg=0.12;		// + kE/tick за каждый снятый килограмм (50 кг -> 8, 85 кг -> 12,2)
		sprintMul=1.5;			// при беге расход ×
		tickSeconds=10;			// как часто сервер списывает энергию (единица kE/tick — 10 с)
	};
};
class CfgNonAIVehicles
{
	class ProxyAttachment;
	// крепления на модели сервопривода (бывшие батареи OOTS; tools/ec_servo_proxies.py): энергоячейки видны на спине
	class Proxyvb_exocell_slot_1: ProxyAttachment
	{
		scope=2;
		inventorySlot[]={"VB_Echo_GearCell1"};
		model="VB_EchoClothes\exo_set\Backpacks\ExoPrivod\vb_exocell_slot_1.p3d";
	};
	class Proxyvb_exocell_slot_2: ProxyAttachment
	{
		scope=2;
		inventorySlot[]={"VB_Echo_GearCell2"};
		model="VB_EchoClothes\exo_set\Backpacks\ExoPrivod\vb_exocell_slot_2.p3d";
	};
};

class CfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base {};
	class PlateCarrierVest;
	class TacticalGloves_ColorBase;
	class MilitaryBoots_ColorBase;

	// ---------------- шлем (был IMP_Helmet_RoninHeavy_ColorBase / _Black): скрипт — тёмное стекло, хват в руках.
	// С маской совместим (11-23): noMask убран — в 1.30 он ещё запрещает маску (EntityAI.InitLegacySlotExclusionValuesDerived)
	class VB_EC_Exo_Helmet_Base: Clothing
	{
		scope=0;
		vbExo=1;
		displayName="$STR_VB_EC_Exo_Helmet";
		descriptionShort="$STR_VB_EC_Exo_Helmet_disc";
		model="VB_EchoClothes\exo_set\helmet\ronin_heavy\models\RoninHeavy_g.p3d";
		attachments[]={"NVG","helmetFlashlight"};
		repairableWithKits[]={2,3,5,8};
		repairCosts[]={25,25,25,25};
		inventorySlot[]={"Headgear","Helmet"};
		simulation="clothing";
		vehicleClass="Clothing";
		itemInfo[]={"Clothing","Headgear"};
		quickBarBonus=2;
		weight=2000;
		itemSize[]={4,4};
		noNVStrap=1;
		noEyewear=1;
		varWetMax=0.049;
		heatIsolation=1;
		visibilityModifier=0.75;
		hiddenSelections[]={"camoflage"};
		headSelectionsToHide[]={"Clipping_GP5GasMask"};
		class Protection
		{
			biological=1;
			chemical=1;
		};
		class ClothingTypes
		{
			male="VB_EchoClothes\exo_set\helmet\ronin_heavy\models\RoninHeavy.p3d";
			female="VB_EchoClothes\exo_set\helmet\ronin_heavy\models\RoninHeavy_f.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=600;
					transferToAttachmentsCoef=0.5;
					healthLevels[]={{1.0,{"VB_EchoClothes\exo_set\helmet\ronin_heavy\data\RoninHeavy.rvmat"}},{0.7,{"VB_EchoClothes\exo_set\helmet\ronin_heavy\data\RoninHeavy.rvmat"}},{0.5,{"VB_EchoClothes\exo_set\helmet\ronin_heavy\data\RoninHeavy_damage.rvmat"}},{0.3,{"VB_EchoClothes\exo_set\helmet\ronin_heavy\data\RoninHeavy_damage.rvmat"}},{0.0,{"VB_EchoClothes\exo_set\helmet\ronin_heavy\data\RoninHeavy_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health { damage=0.07; };
					class Blood { damage=0; };
					class Shock { damage=0.18; };
				};
				class Melee
				{
					class Health { damage=0.18; };
					class Blood { damage=0; };
					class Shock { damage=0.07; };
				};
				class Infected
				{
					class Health { damage=0.18; };
					class Blood { damage=0; };
					class Shock { damage=0.18; };
				};
				class FragGrenade
				{
					class Health { damage=0.07; };
					class Blood { damage=0; };
					class Shock { damage=0.07; };
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem { soundSet="pickUpPot_SoundSet"; id=797; };
				class drop { soundset="BallisticHelmet_drop_SoundSet"; id=898; };
			};
		};
	};
	class VB_EC_Exo_Helmet: VB_EC_Exo_Helmet_Base
	{
		scope=2;
		hiddenSelectionsTextures[]={"VB_EchoClothes\exo_set\helmet\ronin_heavy\data\RoninHeavy_Black.paa"};
	};

	// ---------------- маска (была OOTS_Mask_MaskGhost): дыхание — газ и зараза без фильтра; надевается под шлем
	class VB_EC_Exo_Mask_Base: Clothing
	{
		scope=0;
		vbExo=1;
		displayName="$STR_VB_EC_Exo_Mask";
		descriptionShort="$STR_VB_EC_Exo_Mask_disc";
		model="VB_EchoClothes\exo_set\Mask\MaskGhost\MaskGhost_g.p3d";
		inventorySlot[]={"Mask"};
		itemInfo[]={"Clothing","Mask"};
		rotationFlags=1;
		weight=170;
		itemSize[]={2,2};
		ragQuantity=1;
		varWetMax=0.1;
		heatIsolation=1;
		repairableWithKits[]={2,3,5,8};
		repairCosts[]={25,25,25,25};
		visibilityModifier=0.75;
		headSelectionsToHide[]={"Clipping_GP5GasMask"};
		hiddenSelections[]={"como"};
		class ClothingTypes
		{
			male="VB_EchoClothes\exo_set\Mask\MaskGhost\MaskGhost.p3d";
			female="VB_EchoClothes\exo_set\Mask\MaskGhost\MaskGhost.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=450;
					healthLevels[]={{1.0,{"VB_EchoClothes\exo_set\Mask\MaskGhost\Data\MaskGhost.rvmat"}},{0.7,{"VB_EchoClothes\exo_set\Mask\MaskGhost\Data\MaskGhost.rvmat"}},{0.5,{"VB_EchoClothes\exo_set\Mask\MaskGhost\Data\MaskGhost_damage.rvmat"}},{0.3,{"VB_EchoClothes\exo_set\Mask\MaskGhost\Data\MaskGhost_damage.rvmat"}},{0.0,{"VB_EchoClothes\exo_set\Mask\MaskGhost\Data\MaskGhost_destruct.rvmat"}}};
				};
			};
		};
		class Protection
		{
			biological=1;
			chemical=1;
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem { soundSet="Shirt_pickup_SoundSet"; id=797; };
				class drop { soundset="Shirt_drop_SoundSet"; id=898; };
			};
		};
	};
	class VB_EC_Exo_Mask: VB_EC_Exo_Mask_Base
	{
		scope=2;
		color="black";
		hiddenSelectionsTextures[]={"VB_EchoClothes\exo_set\Mask\MaskGhost\Data\MaskGhost_co.paa"};
		hiddenSelectionsMaterials[]={"VB_EchoClothes\exo_set\Mask\MaskGhost\Data\MaskGhost.rvmat"};
	};

	// ---------------- жилет (был OOTS_Vest_Exo): бронежилет от PlateCarrierVest, без чужих бронеплит
	class VB_EC_Exo_Vest_Base: PlateCarrierVest
	{
		scope=0;
		vbExo=1;
		displayName="$STR_VB_EC_Exo_Vest";
		descriptionShort="$STR_VB_EC_Exo_Vest_disc";
		model="VB_EchoClothes\exo_set\Vest\ExoVest\ExoPlate_g.p3d";
		inventorySlot[]={"Vest"};
		itemInfo[]={"Clothing","Vest"};
		attachments[]={"VestHolster","VestGrenadeA","VestGrenadeB","VestGrenadeC","VestGrenadeD"};
		weight=6000;
		itemSize[]={3,3};
		itemsCargoSize[]={10,8};
		quickBarBonus=5;
		varWetMax=0.1;
		heatIsolation=0.85;
		repairableWithKits[]={2,3,5,8};
		repairCosts[]={25,25,25,25};
		visibilityModifier=0.75;
		hiddenSelections[]={"como"};
		class Protection
		{
			biological=1;
			chemical=1;
		};
		class ClothingTypes
		{
			male="VB_EchoClothes\exo_set\Vest\ExoVest\ExoPlate_m.p3d";
			female="VB_EchoClothes\exo_set\Vest\ExoVest\ExoPlate_m.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=1200;
					transferToAttachmentsCoef=0.5;
					healthLevels[]={{1.0,{"VB_EchoClothes\exo_set\Vest\ExoVest\data\ExoPlate.rvmat"}},{0.7,{"VB_EchoClothes\exo_set\Vest\ExoVest\data\ExoPlate.rvmat"}},{0.5,{"VB_EchoClothes\exo_set\Vest\ExoVest\data\ExoPlate_damage.rvmat"}},{0.3,{"VB_EchoClothes\exo_set\Vest\ExoVest\data\ExoPlate_damage.rvmat"}},{0.0,{"VB_EchoClothes\exo_set\Vest\ExoVest\data\ExoPlate_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health { damage=0.1; };
					class Blood { damage=0; };
					class Shock { damage=0.3; };
				};
				class Melee
				{
					class Health { damage=0.14; };
					class Blood { damage=0; };
					class Shock { damage=0.14; };
				};
				class Infected
				{
					class Health { damage=0.14; };
					class Blood { damage=0; };
					class Shock { damage=0.14; };
				};
				class FragGrenade
				{
					class Health { damage=0.25; };
					class Blood { damage=0; };
					class Shock { damage=0.25; };
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem { soundSet="SmershVest_pickup_SoundSet"; id=797; };
				class drop { soundset="SmershVest_drop_SoundSet"; id=898; };
			};
		};
	};
	class VB_EC_Exo_Vest: VB_EC_Exo_Vest_Base
	{
		scope=2;
		color="black";
		hiddenSelectionsTextures[]={"VB_EchoClothes\exo_set\Vest\ExoVest\data\ExoVest_co.paa"};
		hiddenSelectionsMaterials[]={"VB_EchoClothes\exo_set\Vest\ExoVest\data\ExoPlate.rvmat"};
	};

	// ---------------- сервопривод (был OOTS_Exo_Servoprivod_black_BeltSlot_120): на поясе (решение 11-23: чтобы
	// оставался любой рюкзак), 100 ячеек, 4 кг. НЕ часть комплекта (vbExo нет): самостоятельный модуль на энергии Эхо,
	// бонусы — class CfgVB_EchoClothes Servo и скрипт VB_EC_Servo.c. Две энергоячейки видны на спине (крепления выше)
	class VB_EC_Exo_Servo_Base: Clothing
	{
		scope=0;
		vbExoServo=1;
		displayName="$STR_VB_EC_Exo_Servo";
		descriptionShort="$STR_VB_EC_Exo_Servo_disc";
		model="VB_EchoClothes\exo_set\Backpacks\ExoPrivod\ExoPrivod_g.p3d";
		debug_ItemCategory=9;
		attachments[]={"VB_Echo_GearCell1","VB_Echo_GearCell2"};
		inventorySlot[]={"Hips"};
		itemInfo[]={"Clothing","Back"};
		rotationFlags=16;
		weight=4000;
		itemSize[]={4,5};
		itemsCargoSize[]={10,10};
		quickBarBonus=5;
		varWetMax=0.1;
		heatIsolation=0.7;
		repairableWithKits[]={2,3,5,8};
		repairCosts[]={25,25,25,25};
		soundAttType="Outdoor";
		visibilityModifier=0.75;
		hiddenSelections[]={"Pr1","Pr2"};
		class Protection
		{
			biological=1;
			chemical=1;
		};
		class ClothingTypes
		{
			male="VB_EchoClothes\exo_set\Backpacks\ExoPrivod\ExoPrivod_m.p3d";
			female="VB_EchoClothes\exo_set\Backpacks\ExoPrivod\ExoPrivod_m.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=750;
					transferToAttachmentsCoef=0.5;
					healthLevels[]={{1.0,{"VB_EchoClothes\exo_set\Backpacks\ExoPrivod\data\ExoPrivod1.rvmat","VB_EchoClothes\exo_set\Backpacks\ExoPrivod\data\ExoPrivod2.rvmat"}},{0.7,{"VB_EchoClothes\exo_set\Backpacks\ExoPrivod\data\ExoPrivod1.rvmat","VB_EchoClothes\exo_set\Backpacks\ExoPrivod\data\ExoPrivod2.rvmat"}},{0.5,{"VB_EchoClothes\exo_set\Backpacks\ExoPrivod\data\ExoPrivod1_damage.rvmat","VB_EchoClothes\exo_set\Backpacks\ExoPrivod\data\ExoPrivod2_damage.rvmat"}},{0.3,{"VB_EchoClothes\exo_set\Backpacks\ExoPrivod\data\ExoPrivod1_damage.rvmat","VB_EchoClothes\exo_set\Backpacks\ExoPrivod\data\ExoPrivod2_damage.rvmat"}},{0.0,{"VB_EchoClothes\exo_set\Backpacks\ExoPrivod\data\ExoPrivod1_destruct.rvmat","VB_EchoClothes\exo_set\Backpacks\ExoPrivod\data\ExoPrivod2_destruct.rvmat"}}};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem_Light { soundSet="pickUpBackPack_Plastic_Light_SoundSet"; id=796; };
				class pickUpItem { soundSet="pickUpBackPack_Plastic_SoundSet"; id=797; };
				class drop { soundset="taloonbag_drop_SoundSet"; id=898; };
			};
		};
	};
	class VB_EC_Exo_Servo: VB_EC_Exo_Servo_Base
	{
		scope=2;
		hiddenSelectionsTextures[]={"VB_EchoClothes\exo_set\Backpacks\ExoPrivod\data\ExoPrivod1_co.paa","VB_EchoClothes\exo_set\Backpacks\ExoPrivod\data\ExoPrivod2_co.paa"};
		hiddenSelectionsMaterials[]={"VB_EchoClothes\exo_set\Backpacks\ExoPrivod\data\ExoPrivod1.rvmat","VB_EchoClothes\exo_set\Backpacks\ExoPrivod\data\ExoPrivod2.rvmat"};
	};

	// ---------------- куртка (была OOTS_Jacket_Exo); модули OOTS Clothes*Mod не перенесены
	class VB_EC_Exo_Jacket_Base: Clothing
	{
		scope=0;
		vbExo=1;
		displayName="$STR_VB_EC_Exo_Jacket";
		descriptionShort="$STR_VB_EC_Exo_Jacket_disc";
		model="VB_EchoClothes\exo_set\Tops\ExoJacket\ExoJacket_g.p3d";
		inventorySlot[]={"Body"};
		itemInfo[]={"Clothing","Body"};
		weight=4000;
		itemSize[]={4,3};
		itemsCargoSize[]={10,10};
		quickBarBonus=5;
		varWetMax=0.1;
		heatIsolation=0.9;
		ragQuantity=4;
		repairableWithKits[]={2,3,5,8};
		repairCosts[]={25,25,25,25};
		soundAttType="WoolShirt";
		visibilityModifier=0.75;
		hiddenSelections[]={"como"};
		class Protection
		{
			biological=1;
			chemical=1;
		};
		class ClothingTypes
		{
			male="VB_EchoClothes\exo_set\Tops\ExoJacket\ExoJacket_m.p3d";
			female="VB_EchoClothes\exo_set\Tops\ExoJacket\ExoJacket_m.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=750;
					transferToAttachmentsCoef=0.5;
					healthLevels[]={{1.0,{"VB_EchoClothes\exo_set\Tops\ExoJacket\data\ExoJacket.rvmat"}},{0.7,{"VB_EchoClothes\exo_set\Tops\ExoJacket\data\ExoJacket.rvmat"}},{0.5,{"VB_EchoClothes\exo_set\Tops\ExoJacket\data\ExoJacket_damage.rvmat"}},{0.3,{"VB_EchoClothes\exo_set\Tops\ExoJacket\data\ExoJacket_damage.rvmat"}},{0.0,{"VB_EchoClothes\exo_set\Tops\ExoJacket\data\ExoJacket_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health { damage=0.5; };
					class Blood { damage=0.5; };
					class Shock { damage=0.5; };
				};
				class Melee
				{
					class Health { damage=0.4; };
					class Blood { damage=0.4; };
					class Shock { damage=0.4; };
				};
				class Infected
				{
					class Health { damage=0.4; };
					class Blood { damage=0.4; };
					class Shock { damage=0.4; };
				};
				class FragGrenade
				{
					class Health { damage=0.5; };
					class Blood { damage=0.5; };
					class Shock { damage=0.5; };
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem { soundSet="Shirt_pickup_SoundSet"; id=797; };
				class drop { soundset="Shirt_drop_SoundSet"; id=898; };
			};
		};
	};
	class VB_EC_Exo_Jacket: VB_EC_Exo_Jacket_Base
	{
		scope=2;
		hiddenSelectionsTextures[]={"VB_EchoClothes\exo_set\Tops\ExoJacket\data\ExoJacket_co.paa"};
		hiddenSelectionsMaterials[]={"VB_EchoClothes\exo_set\Tops\ExoJacket\data\ExoJacket.rvmat"};
	};

	// ---------------- штаны (были OOTS_Pants_Exo); «personality» берёт текстуру модели
	class VB_EC_Exo_Pants_Base: Clothing
	{
		scope=0;
		vbExo=1;
		displayName="$STR_VB_EC_Exo_Pants";
		descriptionShort="$STR_VB_EC_Exo_Pants_disc";
		model="VB_EchoClothes\exo_set\Pants\ExoPants\ExoPants_g.p3d";
		ContinuouActions[]={"AT_WRING_CLOTHES"};
		inventorySlot[]={"Legs"};
		simulation="clothing";
		itemInfo[]={"Clothing","Legs"};
		weight=1500;
		itemSize[]={3,2};
		itemsCargoSize[]={10,9};
		quickBarBonus=5;
		ragQuantity=3;
		varWetMax=0.1;
		heatIsolation=0.9;
		repairableWithKits[]={2,3,5,8};
		repairCosts[]={25,25,25,25};
		visibilityModifier=0.75;
		hiddenSelections[]={"como","personality"};
		class Protection
		{
			biological=1;
			chemical=1;
		};
		class ClothingTypes
		{
			male="VB_EchoClothes\exo_set\Pants\ExoPants\ExoPants_m.p3d";
			female="VB_EchoClothes\exo_set\Pants\ExoPants\ExoPants_f.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=750;
					transferToAttachmentsCoef=0.5;
					healthLevels[]={{1.0,{"VB_EchoClothes\exo_set\Pants\ExoPants\data\ExoPants.rvmat"}},{0.7,{"VB_EchoClothes\exo_set\Pants\ExoPants\data\ExoPants.rvmat"}},{0.5,{"VB_EchoClothes\exo_set\Pants\ExoPants\data\ExoPants_damage.rvmat"}},{0.3,{"VB_EchoClothes\exo_set\Pants\ExoPants\data\ExoPants_damage.rvmat"}},{0.0,{"VB_EchoClothes\exo_set\Pants\ExoPants\data\ExoPants_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health { damage=0.5; };
					class Blood { damage=0.5; };
					class Shock { damage=0.5; };
				};
				class Melee
				{
					class Health { damage=0.4; };
					class Blood { damage=0.4; };
					class Shock { damage=0.4; };
				};
				class Infected
				{
					class Health { damage=0.4; };
					class Blood { damage=0.4; };
					class Shock { damage=0.4; };
				};
				class FragGrenade
				{
					class Health { damage=0.5; };
					class Blood { damage=0.5; };
					class Shock { damage=0.5; };
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem { soundSet="Shirt_pickup_SoundSet"; id=797; };
				class drop { soundset="Shirt_drop_SoundSet"; id=898; };
			};
		};
	};
	class VB_EC_Exo_Pants: VB_EC_Exo_Pants_Base
	{
		scope=2;
		color="black";
		hiddenSelectionsTextures[]={"VB_EchoClothes\exo_set\Pants\ExoPants\data\ExoPants_co.paa"};
		hiddenSelectionsMaterials[]={"VB_EchoClothes\exo_set\Pants\ExoPants\data\ExoPants.rvmat"};
	};

	// ---------------- перчатки (были OOTS_Gloves_Exo)
	class VB_EC_Exo_Gloves_Base: TacticalGloves_ColorBase
	{
		scope=0;
		vbExo=1;
		displayName="$STR_VB_EC_Exo_Gloves";
		descriptionShort="$STR_VB_EC_Exo_Gloves_disc";
		model="VB_EchoClothes\exo_set\Gloves\ExoGloves\ExoGloves_g.p3d";
		inventorySlot[]={"Gloves"};
		simulation="clothing";
		itemInfo[]={"Gloves"};
		rotationFlags=34;
		repairableWithKits[]={2,3,5,8};
		repairCosts[]={25,25,25,25};
		weight=454;
		itemSize[]={2,2};
		varWetMax=0.1;
		heatIsolation=0.8;
		visibilityModifier=0.75;
		hiddenSelections[]={"como","personality"};
		class Protection
		{
			biological=1;
			chemical=1;
		};
		class ClothingTypes
		{
			male="VB_EchoClothes\exo_set\Gloves\ExoGloves\ExoGloves_m.p3d";
			female="VB_EchoClothes\exo_set\Gloves\ExoGloves\ExoGloves_f.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=600;
					healthLevels[]={{1.0,{"VB_EchoClothes\exo_set\Gloves\ExoGloves\data\ExoGloves.rvmat"}},{0.7,{"VB_EchoClothes\exo_set\Gloves\ExoGloves\data\ExoGloves.rvmat"}},{0.5,{"VB_EchoClothes\exo_set\Gloves\ExoGloves\data\ExoGloves_damage.rvmat"}},{0.3,{"VB_EchoClothes\exo_set\Gloves\ExoGloves\data\ExoGloves_damage.rvmat"}},{0.0,{"VB_EchoClothes\exo_set\Gloves\ExoGloves\data\ExoGloves_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Melee
				{
					class Health { damage=0.4; };
					class Blood { damage=0.4; };
					class Shock { damage=0.4; };
				};
				class Infected
				{
					class Health { damage=0.4; };
					class Blood { damage=0.4; };
					class Shock { damage=0.4; };
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem { soundSet="WorkingGloves_pickup_SoundSet"; id=797; };
				class drop { soundset="WorkingGloves_drop_SoundSet"; id=898; };
			};
		};
	};
	class VB_EC_Exo_Gloves: VB_EC_Exo_Gloves_Base
	{
		scope=2;
		color="black";
		hiddenSelectionsTextures[]={"VB_EchoClothes\exo_set\Gloves\ExoGloves\data\ExoGloves_co.paa"};
		hiddenSelectionsMaterials[]={"VB_EchoClothes\exo_set\Gloves\ExoGloves\data\ExoGloves.rvmat"};
	};

	// ---------------- ботинки (были OOTS_Boots_Exo): ножны для ножа; мягкое падение — скрипт
	class VB_EC_Exo_Boots_Base: MilitaryBoots_ColorBase
	{
		scope=0;
		vbExo=1;
		displayName="$STR_VB_EC_Exo_Boots";
		descriptionShort="$STR_VB_EC_Exo_Boots_disc";
		model="VB_EchoClothes\exo_set\Boots\ExoBoots\ExoBoots_g.p3d";
		inventorySlot[]={"Feet"};
		itemInfo[]={"Clothing","Feet"};
		attachments[]={"knife"};
		itemSize[]={3,3};
		weight=900;
		durability=0.5;
		quickBarBonus=2;
		varWetMax=0.1;
		heatIsolation=0.9;
		visibilityModifier=0.75;
		repairableWithKits[]={2,3,5,8};
		repairCosts[]={25,25,25,25};
		soundAttType="Boots";
		soundImpactType="default";
		hiddenSelections[]={"como"};
		class Protection
		{
			biological=1;
			chemical=1;
		};
		class ClothingTypes
		{
			male="VB_EchoClothes\exo_set\Boots\ExoBoots\ExoBoots_m.p3d";
			female="VB_EchoClothes\exo_set\Boots\ExoBoots\ExoBoots_m.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=750;
					healthLevels[]={{1.0,{"VB_EchoClothes\exo_set\Boots\ExoBoots\data\ExoBoots.rvmat"}},{0.7,{"VB_EchoClothes\exo_set\Boots\ExoBoots\data\ExoBoots.rvmat"}},{0.5,{"VB_EchoClothes\exo_set\Boots\ExoBoots\data\ExoBoots_damage.rvmat"}},{0.3,{"VB_EchoClothes\exo_set\Boots\ExoBoots\data\ExoBoots_damage.rvmat"}},{0.0,{"VB_EchoClothes\exo_set\Boots\ExoBoots\data\ExoBoots_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Melee
				{
					class Health { damage=0.4; };
					class Blood { damage=0.4; };
					class Shock { damage=0.4; };
				};
				class Infected
				{
					class Health { damage=0.4; };
					class Blood { damage=0.4; };
					class Shock { damage=0.4; };
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem { soundSet="AthleticShoes_pickup_SoundSet"; id=797; };
				class drop { soundset="AthleticShoes_drop_SoundSet"; id=898; };
			};
		};
	};
	class VB_EC_Exo_Boots: VB_EC_Exo_Boots_Base
	{
		scope=2;
		color="black";
		hiddenSelectionsTextures[]={"VB_EchoClothes\exo_set\Boots\ExoBoots\data\ExoBoots_co.paa"};
		hiddenSelectionsMaterials[]={"VB_EchoClothes\exo_set\Boots\ExoBoots\data\ExoBoots.rvmat"};
	};
};
