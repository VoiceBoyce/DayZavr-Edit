// Объекты Изнанки: сердце (VB_RiftHeart) и корни/трещины без сердца (конфиг-классы на VB_RiftBase).
// Всё ниже работает только на клиенте: сервер объект просто держит.
// Удар = переключение материалов свечения (4 уровня, vb_rift_*_0..3.rvmat); ритм задаёт VB_RiftPulse.
// Траву выжигают локальные срезатели: ClutterCutter6x6 в точке cut_big (у сердца)
// и ClutterCutter2x2 по точкам cut_1..cut_N из Memory LOD (вдоль корней и трещин).

class VB_RiftBase extends House
{
	static const int LEVEL_IDLE = 1;		// уровень свечения, когда пульс стоит
	static const int LEVEL_DAY = -2;		// день: сухие текстуры *_day_co, материалы *_day без свечения
	static const string MAT_PATH = "VB_Rift\\data\\";

	float m_VB_Phase;						// сдвиг удара в цикле, с (корням его задаёт VB_RiftPulse)
	float m_VB_Clock;						// свои «часы» пульса, с: идут быстрее, когда сердце рядом потревожено
	float m_VB_Rate = 1.0;					// текущая скорость часов (плавно тянется к цели)
	float m_VB_LastT = -1;					// момент цикла на прошлом тике (для запуска звука удара)
	VB_RiftBase m_VB_LinkHeart;				// сердце, в ритме которого бьётся (задаёт VB_RiftPulse / сердце для обвивок)
	protected int m_VB_IdxCrust = -1;
	protected int m_VB_IdxVeins = -1;
	protected int m_VB_Level = -100;
	protected bool m_VB_DayTex;				// сейчас стоят дневные текстуры
	protected bool m_VB_Active;
	protected VB_RiftLight m_VB_Light;
	protected ParticleSource m_VB_Spores;
	protected EffectSound m_VB_Hum;			// низкий гул-петля у сердца (вместе со спорами)
	protected ref array<Object> m_VB_Cutters;

	bool VB_IsHeart()
	{
		return false;
	}

	// сердце вырезано (только у VB_RiftHeart)
	bool VB_IsDead()
	{
		return false;
	}

	// погас, как днём: само сердце вырезано или вырезано сердце, в ритме которого он бьётся
	bool VB_IsDormant()
	{
		return m_VB_LinkHeart && m_VB_LinkHeart.VB_IsDead();
	}

	// сердце режут прямо сейчас (только у живого VB_RiftHeart): всё рядом бьётся чаще и просыпается даже днём
	bool VB_IsDisturbed()
	{
		return false;
	}

	override void EEInit()
	{
		super.EEInit();

		VB_AlignToTerrain();	// и на сервере, и на клиенте: у локальных копий результат тот же

		if (g_Game.IsDedicatedServer())
			return;

		m_VB_IdxCrust = GetHiddenSelectionIndex("crust");
		m_VB_IdxVeins = GetHiddenSelectionIndex("veins");
		m_VB_Phase = Math.RandomFloat(0, VB_RiftPulse.BEAT_PERIOD);	// без сердца рядом бьются не в такт
		m_VB_Clock = g_Game.GetTime() * 0.001;
		VB_CreateCutters();
		m_VB_Active = true;
		VB_RiftPulse.VB_Register(this);
	}

	// Очистка только здесь. Деструктора нет намеренно: при выходе из игры он вызывался после выгрузки
	// скриптовых модулей и ронял клиент (Virtual Machine Exception в ~VB_RiftBase, 2026-11-11).
	override void EEDelete(EntityAI parent)
	{
		VB_Cleanup();
		super.EEDelete(parent);
	}

	// Наклон по склону: плоскость по высотам рельефа в 9 точках (сетка 3x3 с шагом vbAlignRadius),
	// поворот по горизонтали сохраняется. Если наклон уже задан вручную (в редакторе), не трогаем.
	protected void VB_AlignToTerrain()
	{
		if (ConfigGetInt("vbAlignToTerrain") != 1)
			return;

		vector ori = GetOrientation();
		if (Math.AbsFloat(ori[1]) > 0.5 || Math.AbsFloat(ori[2]) > 0.5)
			return;

		float r = ConfigGetFloat("vbAlignRadius");
		if (r <= 0)
			r = 3.0;

		vector pos = GetPosition();
		float sumY = 0;
		float sumXY = 0;
		float sumZY = 0;
		float h;
		for (int ix = -1; ix <= 1; ix++)
		{
			for (int iz = -1; iz <= 1; iz++)
			{
				h = g_Game.SurfaceY(pos[0] + ix * r, pos[2] + iz * r);
				sumY += h;
				sumXY += ix * r * h;
				sumZY += iz * r * h;
			}
		}

		// на симметричной сетке МНК даёт: наклон = сумма(d*h) / сумма(d^2), сумма(d^2) = 6 r^2
		float slopeX = sumXY / (6.0 * r * r);
		float slopeZ = sumZY / (6.0 * r * r);
		vector up = Vector(-slopeX, 1.0, -slopeZ).Normalized();

		vector dir = GetDirection();
		dir = dir - up * vector.Dot(dir, up);
		if (dir.Length() < 0.001)
			return;
		dir.Normalize();

		vector mat[4];
		Math3D.DirectionAndUpMatrix(dir, up, mat);
		vector rot[3];
		rot[0] = mat[0];
		rot[1] = mat[1];
		rot[2] = mat[2];
		vector angles = Math3D.MatrixToAngles(rot);

		SetPosition(Vector(pos[0], sumY / 9.0, pos[2]));
		SetOrientation(angles);
	}

	protected void VB_Cleanup()
	{
		if (!m_VB_Active)
			return;

		m_VB_Active = false;
		VB_RiftPulse.VB_Unregister(this);
		VB_SetLight(false);
		if (m_VB_Spores)
		{
			m_VB_Spores.StopParticle(StopParticleFlags.IMMEDIATE);
			m_VB_Spores = null;
		}
		VB_SetHum(false);
		VB_DeleteCutters();
	}

	// гул: петля на объекте, плавно появляется и гаснет
	void VB_SetHum(bool on)
	{
		if (on && !m_VB_Hum)
		{
			m_VB_Hum = SEffectManager.PlaySoundOnObject(VB_RiftPulse.HUM_SET, this, 2.0, 2.0, true);
		}
		else if (!on && m_VB_Hum)
		{
			m_VB_Hum.SetAutodestroy(true);
			m_VB_Hum.SoundStop();
			m_VB_Hum = null;
		}
	}

	// споры: включает менеджер ночью у ближайших сердец; поднимаются строго вверх, даже если объект наклонён
	void VB_SetSpores(bool on)
	{
		VB_SetHum(on);
		if (on && !m_VB_Spores)
		{
			ParticleManager pm = ParticleManager.GetInstance();
			if (pm)
				m_VB_Spores = pm.PlayOnObject(ParticleList.VB_RIFT_SPORES, this, "0 0 0", "0 0 0", true);
		}
		else if (!on && m_VB_Spores)
		{
			m_VB_Spores.StopParticle();
			m_VB_Spores = null;
		}
	}

	void VB_SetLevel(int level)
	{
		if (level == m_VB_Level)
			return;

		m_VB_Level = level;
		bool day = (level == LEVEL_DAY);
		if (day != m_VB_DayTex)
		{
			m_VB_DayTex = day;
			string tex = "_co.paa";
			if (day)
				tex = "_day_co.paa";
			if (m_VB_IdxCrust >= 0)
				SetObjectTexture(m_VB_IdxCrust, MAT_PATH + "vb_rift_crust" + tex);
			if (m_VB_IdxVeins >= 0)
				SetObjectTexture(m_VB_IdxVeins, MAT_PATH + "vb_rift_vein" + tex);
		}

		string suffix = "_" + level.ToString();
		if (day)
			suffix = "_day";
		if (m_VB_IdxCrust >= 0)
			SetObjectMaterial(m_VB_IdxCrust, MAT_PATH + "vb_rift_crust" + suffix + ".rvmat");
		if (m_VB_IdxVeins >= 0)
			SetObjectMaterial(m_VB_IdxVeins, MAT_PATH + "vb_rift_vein" + suffix + ".rvmat");
	}

	void VB_PlayBeat()
	{
		EffectSound snd = SEffectManager.PlaySoundOnObject(VB_RiftPulse.SOUND_SET, this);
		if (snd)
			snd.SetAutodestroy(true);
	}

	void VB_SetLight(bool on)
	{
		if (on && !m_VB_Light)
		{
			vector pos = GetPosition() + "0 1 0";
			if (MemoryPointExists("light"))
				pos = ModelToWorld(GetMemoryPointPos("light"));

			m_VB_Light = VB_RiftLight.Cast(ScriptedLightBase.CreateLight(VB_RiftLight, pos));
		}
		else if (!on && m_VB_Light)
		{
			m_VB_Light.Destroy();
			m_VB_Light = null;
		}
	}

	void VB_SetLightPulse(float k, float night)
	{
		if (!m_VB_Light)
			return;

		m_VB_Light.SetBrightnessTo((VB_RiftPulse.LIGHT_BASE + VB_RiftPulse.LIGHT_BEAT * k) * night);
		m_VB_Light.SetRadiusTo(VB_RiftPulse.LIGHT_RADIUS * (0.85 + 0.3 * k));
	}

	protected void VB_CreateCutters()
	{
		m_VB_Cutters = new array<Object>;
		if (MemoryPointExists("cut_big"))
			VB_AddCutter("ClutterCutter6x6", ModelToWorld(GetMemoryPointPos("cut_big")));

		for (int i = 1; i < 200; i++)
		{
			string name = "cut_" + i.ToString();
			if (!MemoryPointExists(name))
				break;

			VB_AddCutter("ClutterCutter2x2", ModelToWorld(GetMemoryPointPos(name)));
		}
	}

	protected void VB_AddCutter(string type, vector pos)
	{
		Object cutter = g_Game.CreateObjectEx(type, pos, ECE_LOCAL | ECE_PLACE_ON_SURFACE);
		if (cutter)
			m_VB_Cutters.Insert(cutter);
	}

	protected void VB_DeleteCutters()
	{
		if (!m_VB_Cutters || !g_Game)
			return;

		foreach (Object cutter : m_VB_Cutters)
		{
			if (cutter)
				g_Game.ObjectDelete(cutter);
		}
		m_VB_Cutters.Clear();
	}
}

// Сердце. На клиенте «заражает» ближайшие деревья: меряет ствол лучами и надевает обвивку подходящего
// размера локальным объектом (VB_RiftWrap_*). Обвивки — тоже VB_RiftBase: пульсируют в ритме сердца,
// днём засыпают. Удаляются вместе с сердцем.
class VB_RiftHeart extends VB_RiftBase
{
	static const string CHUNK_TYPE = "VB_RiftHeartChunk";
	static const int CALM_MS = 90000;		// столько после последней попытки резать сердце остаётся потревоженным

	// Тепло: живое сердце ночью греет, как слабый костёр (ванильный UniversalTemperatureSource, как у Bonfire).
	// На морозном Сахале к нему тянет погреться — а там споры, корни и зомби. Сахал сам срезает потолок на 3.
	static const float HEAT_FULL = 2.5;		// м: полное тепло
	static const float HEAT_MAX = 7.0;		// м: дальше не греет
	static const float HEAT_CAP = 18.0;		// потолок температуры для игрока (у костра Bonfire 25)
	static const int HEAT_CHECK_MS = 15000;	// как часто сервер сверяет «ночь/жив»

	protected ref array<Object> m_VB_Wraps;
	protected ref UniversalTemperatureSource m_VB_Heat;
	protected ref UniversalTemperatureSourceSettings m_VB_HeatSettings;
	protected ref UniversalTemperatureSourceLambdaConstant m_VB_HeatLambda;
	protected bool m_VB_Dead;				// вырезано (до рестарта), синхронизируется всем клиентам
	protected bool m_VB_Disturbed;			// его режут или резали недавно, синхронизируется
	protected bool m_VB_HordeCalled;		// сервер: волна зомби уже была (одна на сердце до рестарта)

	void VB_RiftHeart()
	{
		RegisterNetSyncVariableBool("m_VB_Dead");
		RegisterNetSyncVariableBool("m_VB_Disturbed");
	}

	override bool VB_IsDisturbed()
	{
		return m_VB_Disturbed && !m_VB_Dead;
	}

	// сервер: игрок начал резать — сердце потревожено, шум тянет зомби, первая попытка поднимает волну
	void VB_DisturbServer(PlayerBase cutter)
	{
		if (m_VB_Dead)
			return;

		if (!m_VB_Disturbed)
		{
			m_VB_Disturbed = true;
			SetSynchDirty();
			VB_UpdateHeatServer();
		}
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_CalmServer);
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_CalmServer, CALM_MS, false);

		VB_RiftHorde.VB_CallNoise(GetPosition());
		if (!m_VB_HordeCalled)
		{
			m_VB_HordeCalled = true;
			VB_RiftHorde.VB_Spawn(GetPosition());
		}
	}

	void VB_CalmServer()
	{
		if (!m_VB_Disturbed)
			return;

		m_VB_Disturbed = false;
		SetSynchDirty();
	}

	override bool VB_IsHeart()
	{
		return true;
	}

	override bool VB_IsDead()
	{
		return m_VB_Dead;
	}

	override bool VB_IsDormant()
	{
		return m_VB_Dead;
	}

	// сервер: сердце вырезано — гаснет до рестарта, вырезавшему «Сердце Изнанки», всем игрокам удар молнии
	void VB_KillServer(PlayerBase cutter)
	{
		if (m_VB_Dead)
			return;

		m_VB_Dead = true;
		m_VB_Disturbed = false;
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_CalmServer);
		SetSynchDirty();
		VB_UpdateHeatServer();

		// для админов: кто и где вырезал (script_*.log сервера)
		string who = "?";
		if (cutter && cutter.GetIdentity())
			who = cutter.GetIdentity().GetName() + " (" + cutter.GetIdentity().GetPlainId() + ")";
		Print("[VB_Rift] сердце вырезано: " + who + " в " + GetPosition().ToString());

		if (cutter)
		{
			EntityAI chunk = cutter.GetHumanInventory().CreateInInventory(CHUNK_TYPE);
			if (!chunk)
				g_Game.CreateObjectEx(CHUNK_TYPE, cutter.GetPosition(), ECE_PLACE_ON_SURFACE);
		}

		VB_RiftStrike.VB_BroadcastServer();
	}

	override void EEInit()
	{
		super.EEInit();

		if (!g_Game.IsDedicatedServer())
			VB_InfectTrees();

		if (g_Game.IsServer() || !g_Game.IsMultiplayer())
		{
			m_VB_HeatSettings = new UniversalTemperatureSourceSettings();
			m_VB_HeatSettings.m_UpdateInterval = 2;
			m_VB_HeatSettings.m_Updateable = true;
			m_VB_HeatSettings.m_TemperatureItemCap = GameConstants.ITEM_TEMPERATURE_NEUTRAL_ZONE_MIDDLE;
			m_VB_HeatSettings.m_TemperatureCap = HEAT_CAP;
			m_VB_HeatSettings.m_RangeFull = HEAT_FULL;
			m_VB_HeatSettings.m_RangeMax = HEAT_MAX;
			m_VB_HeatSettings.m_ItemDryModifier = 2.0;
			m_VB_HeatLambda = new UniversalTemperatureSourceLambdaConstant();
			m_VB_Heat = new UniversalTemperatureSource(this, m_VB_HeatSettings, m_VB_HeatLambda);
			VB_UpdateHeatServer();
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_UpdateHeatServer, HEAT_CHECK_MS, true);
		}
	}

	// греет только живое сердце ночью (или пока его режут)
	void VB_UpdateHeatServer()
	{
		if (!m_VB_Heat)
			return;

		bool on = !m_VB_Dead && (m_VB_Disturbed || VB_RiftPulse.VB_NightFactor() >= VB_RiftPulse.NIGHT_LIGHT_ABOVE);
		if (m_VB_Heat.IsActive() != on)
			m_VB_Heat.SetActive(on);
	}

	override protected void VB_Cleanup()
	{
		if (g_Game)
		{
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_CalmServer);
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_UpdateHeatServer);
		}
		if (m_VB_Heat)
			m_VB_Heat.SetActive(false);
		VB_DeleteWraps();
		super.VB_Cleanup();
	}

	protected void VB_InfectTrees()
	{
		m_VB_Wraps = new array<Object>;
		float radius = ConfigGetFloat("vbInfectRadius");
		int maxTrees = ConfigGetInt("vbInfectMax");
		if (radius <= 0 || maxTrees <= 0)
			return;

		vector hp = GetPosition();
		array<Object> found = new array<Object>;
		g_Game.GetObjectsAtPosition(hp, radius, found, null);

		array<Object> trees = new array<Object>;
		foreach (Object obj : found)
		{
			if (obj && obj.IsTree() && !obj.IsDamageDestroyed())
				trees.Insert(obj);
		}

		// ближайшие к сердцу первыми (по горизонтали: позиция дерева — центр модели, высоко над землёй)
		Object pick;
		float pickSq;
		float dSq;
		vector tp;
		for (int n = 0; n < maxTrees; n++)
		{
			if (trees.Count() == 0)
				break;

			pick = null;
			pickSq = 1000000000.0;
			foreach (Object tree : trees)
			{
				tp = tree.GetPosition();
				dSq = (tp[0] - hp[0]) * (tp[0] - hp[0]) + (tp[2] - hp[2]) * (tp[2] - hp[2]);
				if (dSq < pickSq)
				{
					pickSq = dSq;
					pick = tree;
				}
			}

			trees.RemoveItem(pick);
			VB_WrapTree(pick);
		}
	}

	// шесть лучей на высоте 1 м над землёй к точке aim; в hits — попадания именно в это дерево
	protected int VB_ProbeTrunk(Object tree, vector aim, array<vector> hits)
	{
		hits.Clear();
		vector mid = Vector(aim[0], g_Game.SurfaceY(aim[0], aim[2]) + 1.0, aim[2]);
		set<Object> results = new set<Object>;
		vector cpos;
		vector cdir;
		int ccomp;
		float a;
		vector off;
		for (int i = 0; i < 6; i++)
		{
			a = i * Math.PI2 / 6.0;
			off = Vector(Math.Cos(a), 0, Math.Sin(a)) * 1.5;
			results.Clear();
			if (DayZPhysics.RaycastRV(mid + off, mid - off * 0.3, cpos, cdir, ccomp, results, null, null, false, false, ObjIntersectGeom))
			{
				if (results.Find(tree) >= 0)
					hits.Insert(cpos);
			}
		}
		return hits.Count();
	}

	// Ствол: позиция дерева — центр всей модели с кроной, у наклонных деревьев ствол в стороне.
	// Поэтому сначала ищем сеткой +-1.8 м точку, где лучи чаще всего попадают в дерево, потом уточняем:
	// центр — среднее попаданий, радиус — среднее расстояние до них.
	protected void VB_WrapTree(Object tree)
	{
		vector tp = tree.GetPosition();
		array<vector> hits = new array<vector>;
		vector bestAim = tp;
		int bestCount = VB_ProbeTrunk(tree, tp, hits);
		int cnt;
		vector aim;
		for (int gx = -3; gx <= 3 && bestCount < 5; gx++)
		{
			for (int gz = -3; gz <= 3 && bestCount < 5; gz++)
			{
				if (gx == 0 && gz == 0)
					continue;
				aim = Vector(tp[0] + gx * 0.6, tp[1], tp[2] + gz * 0.6);
				cnt = VB_ProbeTrunk(tree, aim, hits);
				if (cnt > bestCount)
				{
					bestCount = cnt;
					bestAim = aim;
				}
			}
		}

		if (bestCount < 3)
			return;

		float cx = tp[0];
		float cz = tp[2];
		float r = 0;
		for (int pass = 0; pass < 2; pass++)		// второй проход — лучи к найденному центру, попадания со всех сторон
		{
			if (VB_ProbeTrunk(tree, bestAim, hits) < 3)
				return;
			if (!VB_FitCircle(hits, bestAim, cx, cz, r))
				return;
			bestAim = Vector(cx, tp[1], cz);
		}
		float gy = g_Game.SurfaceY(cx, cz);
		if (r < 0.04 || r > 1.0)
			return;

		// размер обвивки: ближайший не меньше ствола, остальное подгоняем масштабом
		TFloatArray sizes = {0.10, 0.15, 0.20, 0.27, 0.35, 0.45};
		TStringArray types = {"VB_RiftWrap_10", "VB_RiftWrap_15", "VB_RiftWrap_20", "VB_RiftWrap_27", "VB_RiftWrap_35", "VB_RiftWrap_45"};
		int bi = sizes.Count() - 1;
		for (int b = 0; b < sizes.Count(); b++)
		{
			if (sizes[b] >= r)
			{
				bi = b;
				break;
			}
		}
		float scale = Math.Clamp(r / sizes[bi], 0.6, 1.6);

		Object wrap = g_Game.CreateObjectEx(types[bi], Vector(cx, gy, cz), ECE_LOCAL);
		if (!wrap)
			return;

		wrap.SetOrientation(Vector(Math.ModFloat(cx * 7.31 + cz * 3.17, 360.0), 0, 0));	// у всех игроков одинаково
		wrap.SetScale(scale);
		VB_RiftBase wrapRift = VB_RiftBase.Cast(wrap);
		if (wrapRift)
			wrapRift.m_VB_LinkHeart = this;
		m_VB_Wraps.Insert(wrap);
	}

	static float VB_Det3(float a, float b, float c, float d, float e, float f, float g, float h, float i)
	{
		return a * (e * i - f * h) - b * (d * i - f * g) + c * (d * h - e * g);
	}

	// окружность по точкам (метод Kasa, наименьшие квадраты) в координатах относительно origin:
	// x^2 + z^2 + D x + E z + F = 0. false — точки почти на одной прямой
	protected bool VB_FitCircle(array<vector> pts, vector origin, out float cx, out float cz, out float r)
	{
		float sxx = 0;
		float sxz = 0;
		float szz = 0;
		float sx = 0;
		float sz = 0;
		float sxw = 0;
		float szw = 0;
		float sw = 0;
		float x;
		float z;
		float w;
		foreach (vector p : pts)
		{
			x = p[0] - origin[0];
			z = p[2] - origin[2];
			w = x * x + z * z;
			sxx += x * x;
			sxz += x * z;
			szz += z * z;
			sx += x;
			sz += z;
			sxw += x * w;
			szw += z * w;
			sw += w;
		}
		float n = pts.Count();
		float det = VB_Det3(sxx, sxz, sx, sxz, szz, sz, sx, sz, n);
		if (Math.AbsFloat(det) < 0.0000001)
			return false;

		float d = VB_Det3(-sxw, sxz, sx, -szw, szz, sz, -sw, sz, n) / det;
		float e = VB_Det3(sxx, -sxw, sx, sxz, -szw, sz, sx, -sw, n) / det;
		float f = VB_Det3(sxx, sxz, -sxw, sxz, szz, -szw, sx, sz, -sw) / det;
		float rr = d * d * 0.25 + e * e * 0.25 - f;
		if (rr <= 0)
			return false;

		cx = origin[0] - d * 0.5;
		cz = origin[2] - e * 0.5;
		r = Math.Sqrt(rr);
		return true;
	}

	protected void VB_DeleteWraps()
	{
		if (!m_VB_Wraps || !g_Game)
			return;

		foreach (Object wrap : m_VB_Wraps)
		{
			if (wrap)
				g_Game.ObjectDelete(wrap);
		}
		m_VB_Wraps.Clear();
	}
}
