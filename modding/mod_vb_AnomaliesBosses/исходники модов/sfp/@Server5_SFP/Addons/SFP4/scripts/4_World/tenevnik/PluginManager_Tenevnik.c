// Регистрация клиентского плагина Теневника.
// Не трогаем 5_Mission — используем PluginManager.

modded class PluginManager
{
	override void Init()
	{
		super.Init();

		// Клиентский менеджер эффектов (звук/взгляд/дрожь)
		RegisterPlugin("PluginSFP_TenevnikClient", true, false);
	}
}
