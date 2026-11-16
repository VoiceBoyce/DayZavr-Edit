// Стражи рощи «Древнего корня» (ТЗ 2б.1): звери с перекраской — Лесовой волк (ваниль), Терновая заноза и Хранитель
// рощи (MSFZ). Классы VB_RootGuard_* в config.cpp, метка vbRootGuard = 1. Своих зомби у «Корня» нет.
// Здесь общее для клиента и сервера: кто страж, и разрешение разделки Хранителя — он на основе Энта MSFZ, а Энт
// по классу зомби (ZombieBase), у которых ванильное «Разделать» (ActionSkinning) выключено. Условие действия
// проверяют обе стороны. Что выпадает при разделке — сервер (VB_AR_GuardSkin.c, VB_RootGuards.VB_SkinServer).
// Волк и Заноза — животные, разделываются и так.

class VB_AR_Guard
{
	// VB_Is зовут из CanBeTargetedByAI очень часто: метку конфига читаем один раз на класс
	protected static ref map<string, bool> s_VB_ByType;

	static bool VB_Is(EntityAI e)
	{
		if (!e)
			return false;
		if (!s_VB_ByType)
			s_VB_ByType = new map<string, bool>;
		string type = e.GetType();
		bool isGuard;
		if (s_VB_ByType.Find(type, isGuard))
			return isGuard;
		isGuard = e.ConfigGetBool("vbRootGuard");
		s_VB_ByType.Set(type, isGuard);
		return isGuard;
	}
}

modded class ZombieBase
{
	override bool CanBeSkinned()
	{
		if (VB_AR_Guard.VB_Is(this))
			return true;
		return super.CanBeSkinned();
	}
}
