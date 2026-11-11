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
	float m_VB_LastT = -1;					// момент цикла на прошлом тике (для запуска звука удара)
	protected int m_VB_IdxCrust = -1;
	protected int m_VB_IdxVeins = -1;
	protected int m_VB_Level = -100;
	protected bool m_VB_DayTex;				// сейчас стоят дневные текстуры
	protected bool m_VB_Active;
	protected VB_RiftLight m_VB_Light;
	protected ParticleSource m_VB_Spores;
	protected ref array<Object> m_VB_Cutters;

	bool VB_IsHeart()
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
		VB_DeleteCutters();
	}

	// споры: включает менеджер ночью у ближайших сердец; поднимаются строго вверх, даже если объект наклонён
	void VB_SetSpores(bool on)
	{
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

class VB_RiftHeart extends VB_RiftBase
{
	override bool VB_IsHeart()
	{
		return true;
	}
}
