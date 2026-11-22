// VB_EchoClothes — одежда семейства VB_Echo (ТЗ: modding/mod_vb_Echo/ТЗ_VB_EchoClothes.md, гайд: ГАЙД.md).
// Экзоскелет Эхо (этап 1, 11-23): перенесён из наших модов под свои имена, вид и числа — как были на сервере 16:
//   шлем   — IMP_Helmet_RoninHeavy_Black (IMPGMOD);
//   маска  — OOTS_Mask_MaskGhost; жилет, куртка, штаны, перчатки, ботинки — OOTS_*_Exo (оригинал OOTS + наша заплатка
//            OOTS_ClothesPack_DayZavr_Expansion); сервопривод — OOTS_Exo_Servoprivod_black_BeltSlot_120 (заплатка).
// Модели, текстуры, материалы — в exo_set (tools/ec_import.py: пути в моделях заменены строкой той же длины).
// От старых модов не зависит: вложения со слотами OOTS (батарея и ArtCase сервопривода, модули Clothes*Mod куртки,
// ArmorPlateFront/Back жилета) и логика OOTS (playerHandlerType="Exo") не перенесены — механика экзо будет своя (этап 4).
// «Грузоподъёмность» пока как была — отрицательный вес предмета (сервопривод −120 кг, куртка и штаны по −30 кг).

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
class CfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base {};
	class PlateCarrierVest;
	class TacticalGloves_ColorBase;
	class MilitaryBoots_ColorBase;

	// ---------------- шлем (был IMP_Helmet_RoninHeavy_ColorBase / _Black): скрипт — тёмное стекло, хват в руках
	class VB_EC_Exo_Helmet_Base: Clothing
	{
		scope=0;
		displayName="$STR_VB_EC_Exo_Helmet";
		descriptionShort="$STR_VB_EC_Exo_Helmet_disc";
		model="VB_EchoClothes\exo_set\helmet\ronin_heavy\models\RoninHeavy_g.p3d";
		attachments[]={"NVG","helmetFlashlight"};
		repairableWithKits[]={8};
		repairCosts[]={25.0};
		inventorySlot[]={"Headgear","Helmet"};
		simulation="clothing";
		vehicleClass="Clothing";
		itemInfo[]={"Clothing","Headgear"};
		quickBarBonus=2;
		weight=2000;
		itemSize[]={4,4};
		noMask=1;
		noNVStrap=1;
		noEyewear=1;
		varWetMax=0.049;
		heatIsolation=1;
		hiddenSelections[]={"camoflage"};
		headSelectionsToHide[]={"Clipping_GP5GasMask"};
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
					hitpoints=160;
					healthLevels[]={{1.0,{"VB_EchoClothes\exo_set\helmet\ronin_heavy\data\RoninHeavy.rvmat"}},{0.7,{"VB_EchoClothes\exo_set\helmet\ronin_heavy\data\RoninHeavy.rvmat"}},{0.5,{"VB_EchoClothes\exo_set\helmet\ronin_heavy\data\RoninHeavy_damage.rvmat"}},{0.3,{"VB_EchoClothes\exo_set\helmet\ronin_heavy\data\RoninHeavy_damage.rvmat"}},{0.0,{"VB_EchoClothes\exo_set\helmet\ronin_heavy\data\RoninHeavy_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health { damage=0.12; };
					class Blood { damage=0.12; };
					class Shock { damage=0.12; };
				};
				class Melee
				{
					class Health { damage=0.12; };
					class Blood { damage=0.12; };
					class Shock { damage=0.12; };
				};
				class Infected
				{
					class Health { damage=0.12; };
					class Blood { damage=0.12; };
					class Shock { damage=0.12; };
				};
				class FragGrenade
				{
					class Health { damage=0.12; };
					class Blood { damage=0.12; };
					class Shock { damage=0.05; };
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

	// ---------------- маска (была OOTS_Mask_MaskGhost): защита от газа и заразы без фильтра — как была
	class VB_EC_Exo_Mask_Base: Clothing
	{
		scope=0;
		displayName="$STR_VB_EC_Exo_Mask";
		descriptionShort="$STR_VB_EC_Exo_Mask_disc";
		model="VB_EchoClothes\exo_set\Mask\MaskGhost\MaskGhost_g.p3d";
		inventorySlot[]={"Mask"};
		itemInfo[]={"Clothing","Mask"};
		rotationFlags=1;
		weight=170;
		itemSize[]={2,2};
		ragQuantity=1;
		varWetMax=1;
		heatIsolation=1;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		visibilityModifier=0.85;
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
					hitpoints=100;
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
		repairableWithKits[]={3,8};
		repairCosts[]={75,50};
		visibilityModifier=0.85;
		hiddenSelections[]={"como"};
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
					hitpoints=3500;
					transferToAttachmentsCoef=0.5;
					healthLevels[]={{1.0,{"VB_EchoClothes\exo_set\Vest\ExoVest\data\ExoPlate.rvmat"}},{0.7,{"VB_EchoClothes\exo_set\Vest\ExoVest\data\ExoPlate.rvmat"}},{0.5,{"VB_EchoClothes\exo_set\Vest\ExoVest\data\ExoPlate_damage.rvmat"}},{0.3,{"VB_EchoClothes\exo_set\Vest\ExoVest\data\ExoPlate_damage.rvmat"}},{0.0,{"VB_EchoClothes\exo_set\Vest\ExoVest\data\ExoPlate_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health { damage=0.13; };
					class Blood { damage=0.1; };
					class Shock { damage=0.1; };
				};
				class Melee
				{
					class Health { damage=0.13; };
					class Blood { damage=0.1; };
					class Shock { damage=0.2; };
				};
				class Infected
				{
					class Health { damage=0.13; };
					class Blood { damage=0; };
					class Shock { damage=0.15; };
				};
				class FragGrenade
				{
					class Health { damage=0.5; };
					class Blood { damage=0; };
					class Shock { damage=0.26; };
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

	// ---------------- сервопривод (был OOTS_Exo_Servoprivod_black_BeltSlot_120): на поясе, отсек 10×12, −120 кг.
	// Батарея и ArtCase — слоты OOTS, не перенесены; питание будет от энергоячейки Эхо (этап 4)
	class VB_EC_Exo_Servo_Base: Clothing
	{
		scope=0;
		displayName="$STR_VB_EC_Exo_Servo";
		descriptionShort="$STR_VB_EC_Exo_Servo_disc";
		model="VB_EchoClothes\exo_set\Backpacks\ExoPrivod\ExoPrivod_g.p3d";
		debug_ItemCategory=9;
		inventorySlot[]={"Hips"};
		itemInfo[]={"Clothing","Back"};
		rotationFlags=16;
		weight=-120000;
		itemSize[]={4,5};
		itemsCargoSize[]={10,12};
		quickBarBonus=5;
		varWetMax=0.1;
		heatIsolation=0.7;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		soundAttType="Outdoor";
		visibilityModifier=0.85;
		hiddenSelections[]={"Pr1","Pr2"};
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
					hitpoints=5000;
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

	// ---------------- куртка (была OOTS_Jacket_Exo): −30 кг, отсек 10×10; модули OOTS Clothes*Mod не перенесены
	class VB_EC_Exo_Jacket_Base: Clothing
	{
		scope=0;
		displayName="$STR_VB_EC_Exo_Jacket";
		descriptionShort="$STR_VB_EC_Exo_Jacket_disc";
		model="VB_EchoClothes\exo_set\Tops\ExoJacket\ExoJacket_g.p3d";
		inventorySlot[]={"Body"};
		itemInfo[]={"Clothing","Body"};
		weight=-30000;
		itemSize[]={4,3};
		itemsCargoSize[]={10,10};
		quickBarBonus=5;
		varWetMax=0.1;
		heatIsolation=0.9;
		ragQuantity=4;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		soundAttType="WoolShirt";
		visibilityModifier=0.85;
		hiddenSelections[]={"como"};
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
					hitpoints=2500;
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
					class Health { damage=0.5; };
					class Blood { damage=0.5; };
					class Shock { damage=0.5; };
				};
				class Infected
				{
					class Health { damage=0.5; };
					class Blood { damage=0.5; };
					class Shock { damage=0.5; };
				};
				class FragGrenade
				{
					class Health { damage=0.7; };
					class Blood { damage=0.7; };
					class Shock { damage=0.7; };
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

	// ---------------- штаны (были OOTS_Pants_Exo): −30 кг, отсек 10×10; «personality» берёт текстуру модели
	class VB_EC_Exo_Pants_Base: Clothing
	{
		scope=0;
		displayName="$STR_VB_EC_Exo_Pants";
		descriptionShort="$STR_VB_EC_Exo_Pants_disc";
		model="VB_EchoClothes\exo_set\Pants\ExoPants\ExoPants_g.p3d";
		ContinuouActions[]={"AT_WRING_CLOTHES"};
		inventorySlot[]={"Legs"};
		simulation="clothing";
		itemInfo[]={"Clothing","Legs"};
		weight=-30000;
		itemSize[]={3,2};
		itemsCargoSize[]={10,10};
		quickBarBonus=5;
		ragQuantity=3;
		varWetMax=0.1;
		heatIsolation=0.9;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		visibilityModifier=0.85;
		hiddenSelections[]={"como","personality"};
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
					hitpoints=2500;
					healthLevels[]={{1.0,{"VB_EchoClothes\exo_set\Pants\ExoPants\data\ExoPants.rvmat"}},{0.7,{"VB_EchoClothes\exo_set\Pants\ExoPants\data\ExoPants.rvmat"}},{0.5,{"VB_EchoClothes\exo_set\Pants\ExoPants\data\ExoPants_damage.rvmat"}},{0.3,{"VB_EchoClothes\exo_set\Pants\ExoPants\data\ExoPants_damage.rvmat"}},{0.0,{"VB_EchoClothes\exo_set\Pants\ExoPants\data\ExoPants_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health { damage=0.13; };
					class Blood { damage=0.1; };
					class Shock { damage=0.1; };
				};
				class Melee
				{
					class Health { damage=0.13; };
					class Blood { damage=0.1; };
					class Shock { damage=0.2; };
				};
				class Infected
				{
					class Health { damage=0.13; };
					class Blood { damage=0; };
					class Shock { damage=0.15; };
				};
				class FragGrenade
				{
					class Health { damage=0.5; };
					class Blood { damage=0; };
					class Shock { damage=0.26; };
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
		displayName="$STR_VB_EC_Exo_Gloves";
		descriptionShort="$STR_VB_EC_Exo_Gloves_disc";
		model="VB_EchoClothes\exo_set\Gloves\ExoGloves\ExoGloves_g.p3d";
		inventorySlot[]={"Gloves"};
		simulation="clothing";
		itemInfo[]={"Gloves"};
		rotationFlags=34;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		weight=454;
		itemSize[]={2,2};
		varWetMax=0.1;
		heatIsolation=0.8;
		visibilityModifier=0.85;
		hiddenSelections[]={"como","personality"};
		class Protection
		{
			biological=0;
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
					hitpoints=1250;
					healthLevels[]={{1.0,{"VB_EchoClothes\exo_set\Gloves\ExoGloves\data\ExoGloves.rvmat"}},{0.7,{"VB_EchoClothes\exo_set\Gloves\ExoGloves\data\ExoGloves.rvmat"}},{0.5,{"VB_EchoClothes\exo_set\Gloves\ExoGloves\data\ExoGloves_damage.rvmat"}},{0.3,{"VB_EchoClothes\exo_set\Gloves\ExoGloves\data\ExoGloves_damage.rvmat"}},{0.0,{"VB_EchoClothes\exo_set\Gloves\ExoGloves\data\ExoGloves_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Melee
				{
					class Health { damage=0.65; };
					class Blood { damage=0.3; };
					class Shock { damage=0.3; };
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

	// ---------------- ботинки (были OOTS_Boots_Exo): ножны для ножа
	class VB_EC_Exo_Boots_Base: MilitaryBoots_ColorBase
	{
		scope=0;
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
		visibilityModifier=0.85;
		repairableWithKits[]={3};
		repairCosts[]={25};
		soundAttType="Boots";
		soundImpactType="default";
		hiddenSelections[]={"como"};
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
					hitpoints=2250;
					healthLevels[]={{1.0,{"VB_EchoClothes\exo_set\Boots\ExoBoots\data\ExoBoots.rvmat"}},{0.7,{"VB_EchoClothes\exo_set\Boots\ExoBoots\data\ExoBoots.rvmat"}},{0.5,{"VB_EchoClothes\exo_set\Boots\ExoBoots\data\ExoBoots_damage.rvmat"}},{0.3,{"VB_EchoClothes\exo_set\Boots\ExoBoots\data\ExoBoots_damage.rvmat"}},{0.0,{"VB_EchoClothes\exo_set\Boots\ExoBoots\data\ExoBoots_destruct.rvmat"}}};
				};
			};
			class GlobalArmor
			{
				class Melee
				{
					class Health { damage=0.5; };
					class Blood { damage=0.6; };
					class Shock { damage=0.6; };
				};
				class Infected
				{
					class Health { damage=0.5; };
					class Blood { damage=0.6; };
					class Shock { damage=0.6; };
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
