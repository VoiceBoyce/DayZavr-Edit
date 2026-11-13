// Аура живого босса и распад тела после разделки (ТЗ 3 п.3, 2.7, 8 п.4). Только клиент: на сервере
// (и на безголовом клиенте) все методы сразу выходят, ничего не создают.
//
// Палитра — из config.cpp класса босса (vbABPalette, пишет tools/ab_skins.py); если поля нет — хвост имени
// класса (VB_AB_Ent_Storm -> storm). Ресурсы генерирует tools/ab_aura.py:
//   частицы vb_ab_aura_<палитра>.ptc (петля) и vb_ab_aura_<палитра>_dissolve.ptc (распад), регистрация —
//   VB_AB_AuraParticlesGen.c; свет VB_AB_Light_Aura<Палитра> — VB_AB_AuraLightsGen.c.
//
// Что на живом боссе:
//   - частицы палитры на кости spine2 (середина спины скелета отшельника; кость есть у всех 7 моделей).
//     Излучатели — шары с разлётом во все стороны, поэтому оси кости не важны. Масштаб Энта приходит
//     от родителя сам (AddChild к кости масштабированной модели);
//   - цветной точечный свет перед грудью (только ночью, без теней), у Энта радиус × vbABScale;
//   - тихая петля звука аномалии своей палитры (SoundSet из cfg/soundsets.hpp, если он есть, иначе CfgSounds).
// Частицы и звук включены, только пока камера ближе NEAR_ON м; свет — всё время, пока босс есть у клиента.
// Один таймер на всех боссов (раз в секунду): дальность, смерть (запасной путь, если OnDeath не пришёл),
// боссы, удалённые без Detach.

class VB_AB_AuraState
{
	VB_AB_BossBase m_Boss;				// слабая ссылка: станет null, если босса удалили без Detach
	string m_Pal;
	float m_Scale = 1;
	int m_PtcId;						// 0 — частиц для палитры нет
	bool m_Near;						// частицы и звук сейчас включены
	ParticleSource m_Ptc;
	VB_AB_Light_AuraBase m_Lamp;
	EffectSound m_SndSet;				// звук через SoundSet (с плавным затуханием); владеет SEffectManager
	SoundOnVehicle m_SndLoop;			// или через CfgSounds
};

class VB_AB_Aura
{
	static const string BONE = "spine2";				// середина спины (имена костей без учёта регистра)
	static const string BONE_ALT = "spine3";
	static const vector BONE_MISS_OFFSET = "0 1.2 0";	// кости нет — от корня модели, на высоте груди
	static const vector LIGHT_OFFSET = "0 1.75 0.45";	// свет перед грудью/лицом, в осях модели
	static const float LIGHT_FADE_IN = 1.5;			// с: свет разгорается при появлении босса
	static const float DEATH_FADE = 2.5;				// с: свет гаснет после смерти
	static const float NEAR_ON = 250;					// м: ближе — частицы и звук включаются
	static const float NEAR_OFF = 280;					// м: дальше — выключаются (зазор от мигания на границе)
	static const int TICK_MS = 1000;
	static const bool SOUND_ON = true;
	static const float SOUND_VOLUME = 0.45;			// доля громкости SoundSet аномалии: аура тише самой аномалии
	static const float SOUND_FADE = 0.0;				// без плавности: короткие петли аномалий (Жарка ~1 с) дают SoundError «FadeIn & FadeOut are longer than sound wave length»
	static const float SOUND_RANGE = 20;				// м: дальность запасного звука CfgSounds
	static const float DISSOLVE_UP = 0.15;				// м: распад чуть выше центра лежащего тела
	static const float DISSOLVE_FALLBACK_UP = 0.4;		// м: если кости нет — от позиции тела

	protected static ref array<ref VB_AB_AuraState> s_States;
	protected static ref VB_AB_Aura s_Ticker;
	protected static bool s_TickOn;
	protected static int s_LastTick;

	// ================================================================ вызовы из VB_AB_BossBase
	// Включить ауру (клиент; повторный вызов ничего не делает, мёртвому боссу не включается).
	static void Attach(VB_AB_BossBase boss)
	{
		if (!boss || !GetGame() || GetGame().IsHeadlessOrDedicatedServer())
			return;
		if (!boss.IsAlive() || boss.ToDelete())
			return;
		if (!s_States)
			s_States = new array<ref VB_AB_AuraState>;
		if (FindIdx(boss) >= 0)
			return;

		VB_AB_AuraState st = new VB_AB_AuraState;
		st.m_Boss = boss;
		st.m_Pal = GetPalette(boss);
		st.m_Scale = GetBossScale(boss);
		st.m_PtcId = PtcId(st.m_Pal, "");
		s_States.Insert(st);

		StartLight(st);
		UpdateNear(st, GetGame().GetCurrentCameraPosition());
		EnsureTicker();
	}

	// Босс удаляется у клиента (EEDelete): всё убрать сразу.
	static void Detach(VB_AB_BossBase boss)
	{
		if (!boss || !GetGame() || GetGame().IsHeadlessOrDedicatedServer() || !s_States)
			return;
		int i = FindIdx(boss);
		if (i < 0)
			return;
		Shutdown(s_States[i], true);
		s_States.Remove(i);
	}

	// Смерть: частицы перестают рождаться (летящие догорают), звук затухает, свет плавно гаснет.
	static void OnDeath(VB_AB_BossBase boss)
	{
		if (!boss || !GetGame() || GetGame().IsHeadlessOrDedicatedServer() || !s_States)
			return;
		int i = FindIdx(boss);
		if (i < 0)
			return;
		Shutdown(s_States[i], false);
		s_States.Remove(i);
	}

	// Распад тела: одноразовый всплеск частиц палитры (~2,5 с, пик к 1,5 с) и вспышка света.
	// scale — масштаб босса (Энт 1,6): шире разлёт и крупнее частицы; необязательный.
	static void Dissolve(vector pos, string palette, float scale = 1.0)
	{
		if (!GetGame() || GetGame().IsHeadlessOrDedicatedServer())
			return;
		string pal = palette;
		pal.ToLower();

		int id = PtcId(pal, "_dissolve");
		ParticleManager pm = ParticleManager.GetInstance();
		if (pm && id > 0)
		{
			ParticleSource ps = pm.PlayInWorld(id, pos);
			if (ps && scale > 1.05)
			{
				ps.ScaleParticleParamFromOriginal(EmitorParam.VELOCITY, scale);
				ps.ScaleParticleParamFromOriginal(EmitorParam.SIZE, scale);
			}
		}

		typename lt = VB_LightTypeFor(pal);
		if (lt)
		{
			VB_AB_Light_AuraBase flash = VB_AB_Light_AuraBase.Cast(ScriptedLightBase.CreateLight(lt, pos + Vector(0, 0.7, 0), 0));
			if (flash)
				flash.VB_Flash(scale);
		}
	}

	// То же для тела босса: центр — по кости spine2 лежащего тела, палитра и масштаб — из конфига класса.
	static void DissolveBoss(VB_AB_BossBase boss)
	{
		if (!boss || !GetGame() || GetGame().IsHeadlessOrDedicatedServer())
			return;
		// аргументы — через локальные (затирание результата вызова в списке аргументов Enforce)
		vector c = BodyCenter(boss);
		string pal = GetPalette(boss);
		float s = GetBossScale(boss);
		Dissolve(c, pal, s);
	}

	// ================================================================ помощники
	static string GetPalette(Object boss)
	{
		if (!boss)
			return "";
		string pal = boss.ConfigGetString("vbABPalette");
		if (pal == "")
		{
			string tn = boss.GetType();
			int k = tn.LastIndexOf("_");
			if (k >= 0)
				pal = tn.Substring(k + 1, tn.Length() - k - 1);
		}
		pal.ToLower();
		return pal;
	}

	static float GetBossScale(Object boss)
	{
		if (!boss)
			return 1;
		float s = boss.ConfigGetFloat("vbABScale");
		if (s <= 0)
			return 1;
		return s;
	}

	static vector BodyCenter(VB_AB_BossBase boss)
	{
		int b = boss.GetBoneIndexByName(BONE);
		if (b >= 0)
			return boss.GetBonePositionWS(b) + Vector(0, DISSOLVE_UP, 0);
		return boss.GetPosition() + Vector(0, DISSOLVE_FALLBACK_UP, 0);
	}

	protected static int FindIdx(VB_AB_BossBase boss)
	{
		if (!s_States)
			return -1;
		for (int i = 0; i < s_States.Count(); i++)
		{
			if (s_States[i] && s_States[i].m_Boss == boss)
				return i;
		}
		return -1;
	}

	// Номер частицы. Неизвестную палитру отсекаем по классу света (свет и частицы генерируются из одного
	// списка палитр), чтобы GetParticleIDByName не писал WARNING в лог на каждый вызов.
	protected static int PtcId(string pal, string suffix)
	{
		if (!VB_LightTypeFor(pal))
			return 0;
		return ParticleList.GetParticleIDByName("vb_ab_aura_" + pal + suffix);
	}

	protected static typename VB_LightTypeFor(string pal)
	{
		typename none;
		if (pal == "")
			return none;
		string first = pal.Substring(0, 1);
		first.ToUpper();
		string cls = "VB_AB_Light_Aura" + first + pal.Substring(1, pal.Length() - 1);
		return cls.ToType();
	}

	// звук ауры: тихая петля «своей» аномалии палитры
	protected static string SoundOf(string pal)
	{
		if (pal == "fire")
			return "vb_ab_zharka_idle";
		if (pal == "storm")
			return "vb_ab_electra_idle";
		if (pal == "ice")
			return "vb_ab_zastoy_idle";
		if (pal == "gravity")
			return "vb_ab_voronka_idle";
		if (pal == "acid")
			return "vb_ab_studen_idle";
		if (pal == "psi")
			return "vb_ab_lince_idle";
		if (pal == "rad")
			return "vb_ab_holera_idle";
		return "";
	}

	// ================================================================ части ауры
	protected static void StartLight(VB_AB_AuraState st)
	{
		typename lt = VB_LightTypeFor(st.m_Pal);
		if (!lt)
			return;
		VB_AB_Light_AuraBase lamp = VB_AB_Light_AuraBase.Cast(ScriptedLightBase.CreateLight(lt, st.m_Boss.GetPosition(), LIGHT_FADE_IN));
		if (!lamp)
			return;
		lamp.VB_SetScale(st.m_Scale);
		lamp.AttachOnObject(st.m_Boss, LIGHT_OFFSET);
		st.m_Lamp = lamp;
	}

	protected static void StartParticles(VB_AB_AuraState st)
	{
		if (st.m_Ptc || st.m_PtcId <= 0)
			return;
		ParticleManager pm = ParticleManager.GetInstance();
		if (!pm)
			return;
		int bone = st.m_Boss.GetBoneIndexByName(BONE);
		if (bone < 0)
			bone = st.m_Boss.GetBoneIndexByName(BONE_ALT);
		vector offs = "0 0 0";
		if (bone < 0)
			offs = BONE_MISS_OFFSET;
		// создать без запуска, прицепить к кости и только потом запустить: иначе первый кадр родится в точке 0 0 0
		ParticleSource ps = pm.CreateParticle(st.m_PtcId, offs, false);
		if (!ps)
			return;
		st.m_Boss.AddChild(ps, bone);
		ps.PlayParticle();
		st.m_Ptc = ps;
	}

	// мягкая остановка: новые частицы не рождаются, летящие догорают; источник сам вернётся в пул менеджера
	protected static void StopParticles(VB_AB_AuraState st)
	{
		if (st.m_Ptc)
			st.m_Ptc.StopParticle();
		st.m_Ptc = null;
	}

	protected static void StartSound(VB_AB_AuraState st)
	{
		if (!SOUND_ON || st.m_SndSet || st.m_SndLoop)
			return;
		string snd = SoundOf(st.m_Pal);
		if (snd == "")
			return;
		string sset = snd + "_SoundSet";
		if (GetGame().ConfigIsExisting("CfgSoundSets " + sset))
		{
			st.m_SndSet = SEffectManager.PlaySoundOnObject(sset, st.m_Boss, SOUND_FADE, SOUND_FADE, true);
			// потолок громкости: плавный старт (fade-in EffectSound) поднимает громкость до него, не до 1
			if (st.m_SndSet)
				st.m_SndSet.SetSoundMaxVolume(SOUND_VOLUME);
		}
		else if (GetGame().ConfigIsExisting("CfgSounds " + snd))
		{
			st.m_SndLoop = st.m_Boss.PlaySoundLoop(snd, SOUND_RANGE);
		}
	}

	// now = true: без затухания (босс удаляется)
	protected static void StopSound(VB_AB_AuraState st, bool now = false)
	{
		if (st.m_SndSet)
		{
			if (now)
				st.m_SndSet.SetSoundFadeOut(0);
			st.m_SndSet.SetSoundAutodestroy(true);
			st.m_SndSet.SoundStop();
		}
		st.m_SndSet = null;
		if (st.m_SndLoop && GetGame())
			GetGame().ObjectDelete(st.m_SndLoop);
		st.m_SndLoop = null;
	}

	// deleted = true: босс удаляется (свет сразу), false: смерть (свет гаснет плавно)
	protected static void Shutdown(VB_AB_AuraState st, bool deleted)
	{
		StopParticles(st);
		StopSound(st, deleted);
		if (st.m_Lamp)
		{
			if (deleted)
				st.m_Lamp.Destroy();
			else
				st.m_Lamp.FadeOut(DEATH_FADE);
		}
		st.m_Lamp = null;
		st.m_Near = false;
	}

	protected static void UpdateNear(VB_AB_AuraState st, vector cam)
	{
		float d2 = vector.DistanceSq(cam, st.m_Boss.GetPosition());
		if (st.m_Near)
		{
			if (d2 > NEAR_OFF * NEAR_OFF)
			{
				st.m_Near = false;
				StopParticles(st);
				StopSound(st);
			}
		}
		else if (d2 < NEAR_ON * NEAR_ON)
		{
			st.m_Near = true;
			StartParticles(st);
			StartSound(st);
		}
	}

	// ================================================================ таймер
	protected static void EnsureTicker()
	{
		if (!s_Ticker)
			s_Ticker = new VB_AB_Aura;
		// очередь вызовов могла очиститься при смене миссии — тогда таймер давно молчит, перезапускаем
		if (s_TickOn && GetGame().GetTime() - s_LastTick < TICK_MS * 3)
			return;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(s_Ticker.VB_Tick);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(s_Ticker.VB_Tick, TICK_MS, true);
		s_TickOn = true;
		s_LastTick = GetGame().GetTime();
	}

	protected static void StopTicker()
	{
		if (s_Ticker && GetGame())
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(s_Ticker.VB_Tick);
		s_TickOn = false;
	}

	void VB_Tick()
	{
		s_LastTick = GetGame().GetTime();
		if (!s_States || s_States.Count() == 0)
		{
			StopTicker();
			return;
		}
		vector cam = GetGame().GetCurrentCameraPosition();
		for (int i = s_States.Count() - 1; i >= 0; i--)
		{
			VB_AB_AuraState st = s_States[i];
			if (!st)
			{
				s_States.Remove(i);
				continue;
			}
			if (!st.m_Boss || st.m_Boss.ToDelete())
			{
				Shutdown(st, true);
				s_States.Remove(i);
				continue;
			}
			if (!st.m_Boss.IsAlive())
			{
				Shutdown(st, false);
				s_States.Remove(i);
				continue;
			}
			UpdateNear(st, cam);
		}
	}
};
