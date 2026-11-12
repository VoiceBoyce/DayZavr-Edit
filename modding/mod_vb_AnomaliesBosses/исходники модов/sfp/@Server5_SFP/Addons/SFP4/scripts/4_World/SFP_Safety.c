// Общие проверки безопасности для мобов и устройств SFP (порт под DayZ 1.30).
// Цель: мобы не должны трогать физику игрока в "чужих" состояниях
// и не должны ставить/телепортировать ИИ в воду или внутрь геометрии.
class SFP_Safety
{
	// Толкать игрока физикой (SetVelocity/dBodyApplyImpulse) можно только когда он сам стоит/идёт:
	// не в машине и не "на ней" (без сознания в машине у игрока есть родитель), не плывёт,
	// не на лестнице, не падает, не лезет, не без сознания и не связан.
	static bool CanPushPlayer(PlayerBase player)
	{
		if (!player || !player.IsAlive())
			return false;
		if (player.GetParent())
			return false;
		if (player.IsUnconscious() || player.IsRestrained())
			return false;
		if (player.GetCommand_Vehicle())
			return false;
		if (!player.GetCommand_Move())
			return false;
		if (!dBodyIsSet(player))
			return false;
		return true;
	}

	static bool IsWater(vector pos)
	{
		return GetGame().SurfaceIsSea(pos[0], pos[2]) || GetGame().SurfaceIsPond(pos[0], pos[2]);
	}

	// Точка на земле пригодна для ИИ: не вода и в ней нет геометрии (стены, дома, камни).
	// boxSize - полный размер коробки (x, y, z), коробка стоит на земле.
	static bool IsFreeGroundPos(vector pos, vector boxSize, array<Object> exclude = null)
	{
		if (IsWater(pos))
			return false;

		array<Object> excluded = exclude;
		if (!excluded)
			excluded = new array<Object>;

		vector center = pos;
		center[1] = center[1] + boxSize[1] * 0.5 + 0.1;
		return !GetGame().IsBoxCollidingGeometry(center, vector.Zero, boxSize, ObjIntersectView, ObjIntersectGeom, excluded, null);
	}
}
