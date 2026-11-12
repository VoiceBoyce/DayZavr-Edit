// ------------------------------------------------------------
// SFP_Rifle_Case_Base01
// Пример второго варианта кейса с собственной крышкой.
// ------------------------------------------------------------
// [НАСТРАИВАЕТСЯ] Здесь обычно меняют только classname крышки.
// Если в модели другие имена selection/memory — переопределяйте методы ниже.

class SFP_Case_woden01_beige : SFP_OpenableCase_Base
{
	// [НАСТРАИВАЕТСЯ] classname отдельной крышки
	override protected string GetLidClassName()
	{
		return "SFP_Case_woden01_lid_beige";
	}

	// [ЗАВИСИТ ОТ МОДЕЛИ] Если у вашей модели другие имена — раскомментируйте и исправьте:
	//override protected string GetLidSelectionName() { return "case_lid"; }
	//override protected string GetSpawnItemMemoryPoint() { return "spawn_item"; }
}
