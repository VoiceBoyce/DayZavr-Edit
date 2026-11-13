// Маркеры точек, призрак под курсором и кольца радиусов редактора (ТЗ 6). Это отладочные фигуры Shape:
// существуют только на клиенте админа, на сервер и другим игрокам не уходят. Видны сквозь рельеф и дома.
//
// Цвет маркера (луч вверх и кольцо у земли): выбранная — голубой и выше, под курсором — жёлтый,
// босс на точке — красный, точка этого рестарта — зелёный, убит/убран — фиолетовый, выключена — серый,
// без известного босса — оранжевый, остальные — белый. Призрак — розовый, вокруг него радиус поля выбранного босса.
// На месте призрака стоит голограмма — локальная бочка (ECE_LOCAL, есть только у админа) с ванильным материалом
// голограммы постройки: по линиям было непонятно, куда встанет босс (пользователь, 2026-11-13).
// «Показать радиусы» у выбранной точки: поле — оранжевое, поводок — жёлтое (оба от центра поля, если оно раскрыто),
// пробуждение — синее (от точки).

class VB_AB_EditorMarks
{
	static const float MARK_DIST = 2500.0;		// м: дальше маркеры не рисуем
	static const float REBUILD_MOVE = 100.0;	// м: камера ушла — пересобрать (новые точки в MARK_DIST)
	static const float BEAM = 20.0;				// м: луч маркера
	static const float BEAM_SEL = 40.0;
	static const float MARK_R = 2.5;			// м: кольцо маркера у земли
	static const float LIFT = 0.4;				// м: кольца чуть над землёй

	static const int C_IDLE = 0xFFE6E6E6;
	static const int C_OFF = 0xFF7A7A7A;
	static const int C_NOBOSS = 0xFFFFA020;
	static const int C_LIVE = 0xFF3BE07A;
	static const int C_AWAKE = 0xFFFF3B3B;
	static const int C_DONE = 0xFFA77BE0;
	static const int C_SEL = 0xFF30D0FF;
	static const int C_TARGET = 0xFFFFE030;
	static const int C_GHOST = 0xFFFF6EC8;
	static const int C_FIELD = 0xFFFF8C1A;
	static const int C_LEASH = 0xFFFFD21A;
	static const int C_WAKE = 0xFF3AA0FF;

	static const string GHOST_CLASS = "Barrel_Blue";
	static const string GHOST_TEX = "#(argb,8,8,3)color(0.5,0.5,0.5,0.75,ca)";		// как Hologram.RefreshVisual
	static const string GHOST_MAT = "dz\\gear\\containers\\data\\barrel_deployable.rvmat";

	protected ref array<Shape> m_VB_Shapes = new array<Shape>;
	protected string m_VB_Key;
	protected vector m_VB_BuiltAt;
	protected EntityAI m_VB_GhostObj;
	protected float m_VB_GhostLift;				// от низа модели до её начала координат

	void VB_Clear()
	{
		foreach (Shape s : m_VB_Shapes)
		{
			if (s)
				s.Destroy();
		}
		m_VB_Shapes.Clear();
		m_VB_Key = "";
		VB_GhostObjHide();
	}

	void VB_Tick(VB_AB_Editor ed)
	{
		vector cam = g_Game.GetCurrentCameraPosition();
		string key = VB_AB_EditorClient.s_Version.ToString() + "|" + VB_AB_EditorClient.s_LiveVersion.ToString();
		key += "|" + ed.m_VB_CurPoint.ToString() + "|" + ed.VB_TargetId().ToString();
		key += "|" + ed.m_VB_ShowRadii.ToString() + "|" + ed.m_VB_CurBoss + "|" + VB_AB_EditorClient.s_WakeRadius.ToString();
		if (key != m_VB_Key || VB_AB_Editor.VB_DistXZ(cam, m_VB_BuiltAt) > REBUILD_MOVE)
		{
			VB_Clear();
			m_VB_Key = key;
			m_VB_BuiltAt = cam;
			VB_Build(ed, cam);
		}
		VB_DrawGhost(ed);
	}

	protected void VB_Build(VB_AB_Editor ed, vector cam)
	{
		int target = ed.VB_TargetId();
		int color;
		float beam;
		foreach (VB_AB_EdPoint p : VB_AB_EditorClient.s_Points)
		{
			if (VB_AB_Editor.VB_DistXZ(p.Position, cam) > MARK_DIST)
				continue;
			color = VB_PointColor(p);
			beam = BEAM;
			if (p.Id == ed.m_VB_CurPoint)
			{
				color = C_SEL;
				beam = BEAM_SEL;
			}
			if (p.Id == target)
				color = C_TARGET;
			VB_Beam(p.Position, beam, color, false);
			VB_Ring(p.Position, MARK_R, color, false, false);
			if (p.Id == ed.m_VB_CurPoint)
				VB_Ring(p.Position, MARK_R * 1.8, color, false, false);
		}
		if (ed.m_VB_ShowRadii)
			VB_Radii(ed);
	}

	protected int VB_PointColor(VB_AB_EdPoint p)
	{
		if (!VB_AB_EditorPanels.VB_HasKnownBoss(p))
			return C_NOBOSS;
		VB_AB_EdLive lv = VB_AB_EditorClient.VB_FindLive(p.Id);
		if (lv && (lv.Kind == VB_AB_EditorNet.LIVE_AWAKE || lv.Kind == VB_AB_EditorNet.LIVE_FIELD))
			return C_AWAKE;
		if (p.Disabled != 0)
			return C_OFF;
		if (lv && (lv.Kind == VB_AB_EditorNet.LIVE_DONE || lv.Kind == VB_AB_EditorNet.LIVE_RESPAWN))
			return C_DONE;
		if (lv)
			return C_LIVE;
		return C_IDLE;
	}

	// кольца поля и поводка (от центра раскрытого поля, иначе от точки) и пробуждения (от точки)
	protected void VB_Radii(VB_AB_Editor ed)
	{
		VB_AB_EdPoint p = VB_AB_EditorClient.VB_FindPoint(ed.m_VB_CurPoint);
		if (!p)
			return;
		vector center = p.Position;
		VB_AB_EdLive lv = VB_AB_EditorClient.VB_FindLive(p.Id);
		if (lv && lv.Kind == VB_AB_EditorNet.LIVE_FIELD)
			center = lv.Center;
		VB_AB_EdBoss b = ed.VB_RadiiBoss(p);
		if (b)
		{
			VB_Ring(center, b.Radius, C_FIELD, true, false);
			VB_Ring(center, b.Leash, C_LEASH, true, false);
		}
		VB_Ring(p.Position, VB_AB_EditorClient.s_WakeRadius, C_WAKE, true, false);
	}

	// призрак рисуется каждый кадр (фигура на один кадр)
	protected void VB_DrawGhost(VB_AB_Editor ed)
	{
		if (!ed.VB_GhostOk())
		{
			VB_GhostObjHide();
			return;
		}
		vector g = ed.VB_GhostPos();
		VB_GhostObjShow(g);
		vector ground = Vector(g[0], g_Game.SurfaceY(g[0], g[2]), g[2]);
		VB_Beam(ground, g[1] - ground[1] + BEAM, C_GHOST, true);
		VB_Ring(g, MARK_R, C_GHOST, false, true);
		VB_AB_EdBoss b = VB_AB_EditorClient.VB_FindBoss(ed.m_VB_Pick);
		if (b)
			VB_Ring(g, b.Radius, C_FIELD, true, true);
	}

	// голограмма-бочка низом на месте призрака
	protected void VB_GhostObjShow(vector pos)
	{
		if (!m_VB_GhostObj)
		{
			m_VB_GhostObj = EntityAI.Cast(g_Game.CreateObjectEx(GHOST_CLASS, pos, ECE_LOCAL));
			if (!m_VB_GhostObj)
				return;
			m_VB_GhostObj.SetObjectTexture(0, GHOST_TEX);
			m_VB_GhostObj.SetObjectMaterial(0, GHOST_MAT);
			vector box[2];
			if (m_VB_GhostObj.GetCollisionBox(box))
				m_VB_GhostLift = -box[0][1];
		}
		m_VB_GhostObj.SetPosition(pos + Vector(0, m_VB_GhostLift, 0));
	}

	protected void VB_GhostObjHide()
	{
		if (m_VB_GhostObj)
			g_Game.ObjectDelete(m_VB_GhostObj);
		m_VB_GhostObj = null;
	}

	protected void VB_Beam(vector base, float h, int color, bool once)
	{
		vector pts[2];
		pts[0] = base;
		pts[1] = base + Vector(0, h, 0);
		VB_Lines(color, pts, 2, once);
	}

	// follow — по рельефу (большие кольца), иначе ровно на высоте центра
	protected void VB_Ring(vector c, float r, int color, bool follow, bool once)
	{
		if (r <= 0)
			return;
		vector pts[129];
		int n = Math.Clamp(Math.Round(r * 0.5), 24, 128);
		float a;
		float x;
		float z;
		float y = c[1] + LIFT;
		for (int i = 0; i <= n; i++)
		{
			a = Math.PI2 * i / n;
			x = c[0] + Math.Sin(a) * r;
			z = c[2] + Math.Cos(a) * r;
			if (follow)
				y = g_Game.SurfaceY(x, z) + LIFT;
			pts[i] = Vector(x, y, z);
		}
		VB_Lines(color, pts, n + 1, once);
	}

	// статический массив точек ломаной — как в ванильном Debug.DrawLines (3_Game/tools/Debug.c)
	protected void VB_Lines(int color, vector[] pts, int n, bool once)
	{
		int flags = ShapeFlags.NOZBUFFER | ShapeFlags.TRANSP;
		if (once)
		{
			Shape.CreateLines(color, flags | ShapeFlags.ONCE, pts, n);
			return;
		}
		Shape s = Shape.CreateLines(color, flags, pts, n);
		if (s)
			m_VB_Shapes.Insert(s);
	}
}
