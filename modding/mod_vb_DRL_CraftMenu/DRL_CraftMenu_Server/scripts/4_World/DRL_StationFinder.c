// Поиск станции крафта рядом с точкой (сервер).
//
// GetObjectsAtPosition отдаёт только объекты, лежащие в мире. Станция, поставленная
// обвесом в другую (швейная машинка на швейном столе, стол на верстаке), туда не
// попадает - её ищем во вложениях найденных объектов.
class DRL_StationFinder
{
	static const int MAX_DEPTH = 3;

	// Подходит ли объект под список классов станций рецепта (пустой список - любой).
	static bool MatchesClasses(EntityAI ent, array<string> classes)
	{
		if (!ent)
			return false;
		if (!classes || classes.Count() == 0)
			return true;
		string t = ent.GetType();
		for (int i = 0; i < classes.Count(); i++)
		{
			string req = classes[i];
			req.Trim();
			if (req == "")
				continue;
			if (t == req || ent.IsKindOf(req))
				return true;
		}
		return false;
	}

	// Первая станция из списка классов в радиусе от точки, включая вложения.
	static EntityAI FindNear(vector pos, float radius, array<string> classes)
	{
		if (!classes || classes.Count() == 0)
			return null;
		array<Object> objs = new array<Object>();
		array<CargoBase> prx = new array<CargoBase>();
		GetGame().GetObjectsAtPosition(pos, radius, objs, prx);
		for (int i = 0; i < objs.Count(); i++)
		{
			EntityAI ent = EntityAI.Cast(objs[i]);
			if (!ent || ent.IsInherited(Man))
				continue;
			EntityAI found = FindInHierarchy(ent, classes, 0);
			if (found)
				return found;
		}
		return null;
	}

	// Тот же поиск для одного класса (восстановление процесса после рестарта).
	static EntityAI FindNearByClass(vector pos, float radius, string cls)
	{
		if (cls == "")
			return null;
		array<string> classes = new array<string>();
		classes.Insert(cls);
		return FindNear(pos, radius, classes);
	}

	protected static EntityAI FindInHierarchy(EntityAI ent, array<string> classes, int depth)
	{
		if (!ent)
			return null;
		if (MatchesClasses(ent, classes))
			return ent;
		if (depth >= MAX_DEPTH || !ent.GetInventory())
			return null;
		int ac = ent.GetInventory().AttachmentCount();
		for (int i = 0; i < ac; i++)
		{
			EntityAI att = ent.GetInventory().GetAttachmentFromIndex(i);
			EntityAI found = FindInHierarchy(att, classes, depth + 1);
			if (found)
				return found;
		}
		return null;
	}
}
