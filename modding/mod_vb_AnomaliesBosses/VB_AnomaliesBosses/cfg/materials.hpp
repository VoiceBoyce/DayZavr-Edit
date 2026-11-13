// СГЕНЕРИРОВАНО tools/ab_materials.py — руками не править
// Материалы аномалий (ТЗ 2.6): простые предметы без свойств и скриптов, не еда, не портятся.
// Стопка до 20: параметры количества как у ванильного ChickenFeather (DZ/gear/crafting/config.cpp):
// canBeSplit, varQuantityInit = 1, varQuantityMin = 0, varQuantityMax = 20. weight — вес ОДНОЙ штуки:
// у стакающегося предмета вес = weight x количество (ItemBase.GetWeightSpecialized).
// rotationFlags: 2 — лежит как смоделирован (кристалл, комок, мешочек); 17 — плашмя (осколок, галька,
// капсула смоделированы стоя на ребре), см. E53. Модели, атлас и материал — VB_AB/i/mat.
	class VB_AB_Mat_Base: Inventory_Base
	{
		scope = 0;
		itemSize[] = {2, 2};
		weight = 50;
		canBeSplit = 1;
		varQuantityInit = 1;
		varQuantityMin = 0;
		varQuantityMax = 20;
		varQuantityDestroyOnMin = 1;
		rotationFlags = 2;
		soundImpactType = "default";
	};
	// Electra — Разрядный кварц (crystal)
	class VB_AB_Mat_Electra: VB_AB_Mat_Base
	{
		scope = 2;
		displayName = "$STR_VB_AB_Mat_Electra";
		descriptionShort = "$STR_VB_AB_Mat_Electra_Desc";
		model = "VB_AB\i\mat\vb_ab_mat_electra.p3d";
		weight = 90;
		rotationFlags = 2;
		soundImpactType = "glass";
		vbABPalette = "storm";
		vbABGlow[] = {0.4, 0.75, 1};
	};
	// EMP — Ионная пыль (pouch)
	class VB_AB_Mat_EMP: VB_AB_Mat_Base
	{
		scope = 2;
		displayName = "$STR_VB_AB_Mat_EMP";
		descriptionShort = "$STR_VB_AB_Mat_EMP_Desc";
		model = "VB_AB\i\mat\vb_ab_mat_emp.p3d";
		weight = 25;
		rotationFlags = 2;
		soundImpactType = "textile";
		vbABPalette = "storm";
		vbABGlow[] = {0.42, 0.42, 1};
	};
	// Groza — Грозовой кристалл (crystal)
	class VB_AB_Mat_Groza: VB_AB_Mat_Base
	{
		scope = 2;
		displayName = "$STR_VB_AB_Mat_Groza";
		descriptionShort = "$STR_VB_AB_Mat_Groza_Desc";
		model = "VB_AB\i\mat\vb_ab_mat_groza.p3d";
		weight = 100;
		rotationFlags = 2;
		soundImpactType = "glass";
		vbABPalette = "storm";
		vbABGlow[] = {0.3, 0.65, 1};
	};
	// MlechniPut — Звёздный песок (pouch)
	class VB_AB_Mat_MlechniPut: VB_AB_Mat_Base
	{
		scope = 2;
		displayName = "$STR_VB_AB_Mat_MlechniPut";
		descriptionShort = "$STR_VB_AB_Mat_MlechniPut_Desc";
		model = "VB_AB\i\mat\vb_ab_mat_mlechniput.p3d";
		weight = 30;
		rotationFlags = 2;
		soundImpactType = "textile";
		vbABPalette = "storm";
		vbABGlow[] = {0.75, 0.85, 1};
	};
	// Zharka — Окалина (lump)
	class VB_AB_Mat_Zharka: VB_AB_Mat_Base
	{
		scope = 2;
		displayName = "$STR_VB_AB_Mat_Zharka";
		descriptionShort = "$STR_VB_AB_Mat_Zharka_Desc";
		model = "VB_AB\i\mat\vb_ab_mat_zharka.p3d";
		weight = 120;
		rotationFlags = 2;
		soundImpactType = "default";
		vbABPalette = "fire";
		vbABGlow[] = {1, 0.48, 0.07};
	};
	// FireShtorm — Пепельное стекло (shard)
	class VB_AB_Mat_FireShtorm: VB_AB_Mat_Base
	{
		scope = 2;
		displayName = "$STR_VB_AB_Mat_FireShtorm";
		descriptionShort = "$STR_VB_AB_Mat_FireShtorm_Desc";
		model = "VB_AB\i\mat\vb_ab_mat_fireshtorm.p3d";
		weight = 50;
		rotationFlags = 17;
		soundImpactType = "glass";
		vbABPalette = "fire";
		vbABGlow[] = {1, 0.3, 0.03};
	};
	// FireIskra — Искровой уголь (lump)
	class VB_AB_Mat_FireIskra: VB_AB_Mat_Base
	{
		scope = 2;
		displayName = "$STR_VB_AB_Mat_FireIskra";
		descriptionShort = "$STR_VB_AB_Mat_FireIskra_Desc";
		model = "VB_AB\i\mat\vb_ab_mat_fireiskra.p3d";
		weight = 90;
		rotationFlags = 2;
		soundImpactType = "default";
		vbABPalette = "fire";
		vbABGlow[] = {1, 0.7, 0.2};
	};
	// Par — Горячий конденсат (capsule)
	class VB_AB_Mat_Par: VB_AB_Mat_Base
	{
		scope = 2;
		displayName = "$STR_VB_AB_Mat_Par";
		descriptionShort = "$STR_VB_AB_Mat_Par_Desc";
		model = "VB_AB\i\mat\vb_ab_mat_par.p3d";
		weight = 70;
		rotationFlags = 17;
		soundImpactType = "metal";
		vbABPalette = "fire";
		vbABGlow[] = {1, 0.55, 0.28};
	};
	// Snow — Вечный лёд (crystal)
	class VB_AB_Mat_Snow: VB_AB_Mat_Base
	{
		scope = 2;
		displayName = "$STR_VB_AB_Mat_Snow";
		descriptionShort = "$STR_VB_AB_Mat_Snow_Desc";
		model = "VB_AB\i\mat\vb_ab_mat_snow.p3d";
		weight = 80;
		rotationFlags = 2;
		soundImpactType = "glass";
		vbABPalette = "ice";
		vbABGlow[] = {0.5, 0.85, 1};
	};
	// Steklo — Острый осколок (shard)
	class VB_AB_Mat_Steklo: VB_AB_Mat_Base
	{
		scope = 2;
		displayName = "$STR_VB_AB_Mat_Steklo";
		descriptionShort = "$STR_VB_AB_Mat_Steklo_Desc";
		model = "VB_AB\i\mat\vb_ab_mat_steklo.p3d";
		weight = 40;
		rotationFlags = 17;
		soundImpactType = "glass";
		vbABPalette = "ice";
		vbABGlow[] = {0.45, 0.85, 1};
	};
	// Zastoy — Застывший пузырь (pebble)
	class VB_AB_Mat_Zastoy: VB_AB_Mat_Base
	{
		scope = 2;
		displayName = "$STR_VB_AB_Mat_Zastoy";
		descriptionShort = "$STR_VB_AB_Mat_Zastoy_Desc";
		model = "VB_AB\i\mat\vb_ab_mat_zastoy.p3d";
		weight = 50;
		rotationFlags = 17;
		soundImpactType = "default";
		vbABPalette = "ice";
		vbABGlow[] = {0.4, 0.85, 1};
	};
	// Voronka — Сжатый камень (pebble)
	class VB_AB_Mat_Voronka: VB_AB_Mat_Base
	{
		scope = 2;
		displayName = "$STR_VB_AB_Mat_Voronka";
		descriptionShort = "$STR_VB_AB_Mat_Voronka_Desc";
		model = "VB_AB\i\mat\vb_ab_mat_voronka.p3d";
		weight = 150;
		rotationFlags = 17;
		soundImpactType = "default";
		vbABPalette = "gravity";
		vbABGlow[] = {0.62, 0.22, 1};
	};
	// Lomka — Скрученный камень (lump)
	class VB_AB_Mat_Lomka: VB_AB_Mat_Base
	{
		scope = 2;
		displayName = "$STR_VB_AB_Mat_Lomka";
		descriptionShort = "$STR_VB_AB_Mat_Lomka_Desc";
		model = "VB_AB\i\mat\vb_ab_mat_lomka.p3d";
		weight = 130;
		rotationFlags = 2;
		soundImpactType = "default";
		vbABPalette = "gravity";
		vbABGlow[] = {0.68, 0.3, 1};
	};
	// Tramplin — Гравитационная галька (pebble)
	class VB_AB_Mat_Tramplin: VB_AB_Mat_Base
	{
		scope = 2;
		displayName = "$STR_VB_AB_Mat_Tramplin";
		descriptionShort = "$STR_VB_AB_Mat_Tramplin_Desc";
		model = "VB_AB\i\mat\vb_ab_mat_tramplin.p3d";
		weight = 110;
		rotationFlags = 17;
		soundImpactType = "default";
		vbABPalette = "gravity";
		vbABGlow[] = {0.72, 0.38, 1};
	};
	// Roy — Хитиновая пыль (pouch)
	class VB_AB_Mat_Roy: VB_AB_Mat_Base
	{
		scope = 2;
		displayName = "$STR_VB_AB_Mat_Roy";
		descriptionShort = "$STR_VB_AB_Mat_Roy_Desc";
		model = "VB_AB\i\mat\vb_ab_mat_roy.p3d";
		weight = 20;
		rotationFlags = 2;
		soundImpactType = "textile";
		vbABPalette = "acid";
		vbABGlow[] = {0.48, 1, 0.12};
	};
	// Studen — Студенистая слизь (capsule)
	class VB_AB_Mat_Studen: VB_AB_Mat_Base
	{
		scope = 2;
		displayName = "$STR_VB_AB_Mat_Studen";
		descriptionShort = "$STR_VB_AB_Mat_Studen_Desc";
		model = "VB_AB\i\mat\vb_ab_mat_studen.p3d";
		weight = 70;
		rotationFlags = 17;
		soundImpactType = "metal";
		vbABPalette = "acid";
		vbABGlow[] = {0.42, 1, 0.08};
	};
	// Holera — Ядовитая плесень (lump)
	class VB_AB_Mat_Holera: VB_AB_Mat_Base
	{
		scope = 2;
		displayName = "$STR_VB_AB_Mat_Holera";
		descriptionShort = "$STR_VB_AB_Mat_Holera_Desc";
		model = "VB_AB\i\mat\vb_ab_mat_holera.p3d";
		weight = 60;
		rotationFlags = 2;
		soundImpactType = "organic";
		vbABPalette = "acid";
		vbABGlow[] = {0.58, 1, 0.1};
	};
	// Flash — Световая пыль (pouch)
	class VB_AB_Mat_Flash: VB_AB_Mat_Base
	{
		scope = 2;
		displayName = "$STR_VB_AB_Mat_Flash";
		descriptionShort = "$STR_VB_AB_Mat_Flash_Desc";
		model = "VB_AB\i\mat\vb_ab_mat_flash.p3d";
		weight = 20;
		rotationFlags = 2;
		soundImpactType = "textile";
		vbABPalette = "psi";
		vbABGlow[] = {1, 0.95, 0.88};
	};
	// LinceAgr — Линзовое стекло (shard)
	class VB_AB_Mat_LinceAgr: VB_AB_Mat_Base
	{
		scope = 2;
		displayName = "$STR_VB_AB_Mat_LinceAgr";
		descriptionShort = "$STR_VB_AB_Mat_LinceAgr_Desc";
		model = "VB_AB\i\mat\vb_ab_mat_linceagr.p3d";
		weight = 60;
		rotationFlags = 17;
		soundImpactType = "glass";
		vbABPalette = "psi";
		vbABGlow[] = {1, 0.45, 0.75};
	};
	// Lince — Пустое стекло (shard)
	class VB_AB_Mat_Lince: VB_AB_Mat_Base
	{
		scope = 2;
		displayName = "$STR_VB_AB_Mat_Lince";
		descriptionShort = "$STR_VB_AB_Mat_Lince_Desc";
		model = "VB_AB\i\mat\vb_ab_mat_lince.p3d";
		weight = 50;
		rotationFlags = 17;
		soundImpactType = "glass";
		vbABPalette = "psi";
		vbABGlow[] = {0.75, 0.82, 0.9};
	};
	// DeadZone — Чёрный пепел (pouch)
	class VB_AB_Mat_DeadZone: VB_AB_Mat_Base
	{
		scope = 2;
		displayName = "$STR_VB_AB_Mat_DeadZone";
		descriptionShort = "$STR_VB_AB_Mat_DeadZone_Desc";
		model = "VB_AB\i\mat\vb_ab_mat_deadzone.p3d";
		weight = 25;
		rotationFlags = 2;
		soundImpactType = "textile";
		vbABPalette = "rad";
		vbABGlow[] = {0.72, 0.95, 0.12};
	};
	// Atom — Светящийся шлак (lump)
	class VB_AB_Mat_Atom: VB_AB_Mat_Base
	{
		scope = 2;
		displayName = "$STR_VB_AB_Mat_Atom";
		descriptionShort = "$STR_VB_AB_Mat_Atom_Desc";
		model = "VB_AB\i\mat\vb_ab_mat_atom.p3d";
		weight = 140;
		rotationFlags = 2;
		soundImpactType = "default";
		vbABPalette = "rad";
		vbABGlow[] = {0.76, 1, 0.1};
	};
