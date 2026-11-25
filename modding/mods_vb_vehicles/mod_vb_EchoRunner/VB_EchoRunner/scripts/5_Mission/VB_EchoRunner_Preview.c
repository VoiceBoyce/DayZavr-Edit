// Баги №61: модуль на машине повёрнут своей костью hub (гасит угол колеса), а превью инвентаря рисует сам предмет со
// всеми анимациями — иконки модулей в «Шасси» показывались повёрнутыми. Иконка навески показывает локальную копию
// модуля того же класса без поворота (как превью ванильной консоли, scriptconsoleitemstab.c SetPreviewObject:
// локально, без физики и симуляции). Действия с иконкой идут к настоящему предмету (m_Obj), меняется только картинка.
class VB_ER_PreviewCache
{
	protected static ref map<string, EntityAI> s_Items;

	static EntityAI Get(string type)
	{
		if (!GetGame() || GetGame().IsDedicatedServer())
			return null;
		if (!s_Items)
			s_Items = new map<string, EntityAI>();
		EntityAI e;
		if (s_Items.Find(type, e) && e)
			return e;
		e = EntityAI.Cast(GetGame().CreateObjectEx(type, "0 0 0", ECE_LOCAL | ECE_CREATEPHYSICS | ECE_TRACE));
		if (!e)
			return null;
		dBodyDestroy(e);
		e.DisableSimulation(true);
		e.SetAllowDamage(false);
		s_Items.Set(type, e);
		return e;
	}
}

modded class SlotsIconBase
{
	override void SetItemPreview()
	{
		super.SetItemPreview();
		if (!m_Obj || !m_Obj.IsInherited(VB_EchoRunner_GravModule) || !m_Obj.GetHierarchyParent())
			return;
		EntityAI copy = VB_ER_PreviewCache.Get(m_Obj.GetType());
		if (copy)
			m_ItemPreview.SetItem(copy);
	}
}
