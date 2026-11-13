// Материалы аномалий (ТЗ 2.6, 8.14–8.15). Только клиент: сервер про подсветку ничего не знает.
//
// На земле материал светится цветом своей стихии, как артефакты: мягкий пульсирующий свет и мелкие частицы ауры
// палитры (те же .ptc, что у боссов, только уменьшенные). В руках, в инвентаре и в контейнерах подсветки нет.
// Стихия и цвет — из config.cpp (cfg/materials.hpp: vbABPalette, vbABGlow; генератор tools/ab_materials.py).
// Источников света одновременно не больше MAX_LIGHTS (остальные — только частицы): база с кучей материалов на земле
// не должна просаживать FPS.
//
// Находка (детектор превратил запись в предмет): звук «найдено», всплеск частиц распада ауры и вспышка света —
// VB_AB_FoundFx.Play по RPC_FOUND (детекторы мода и писк чужих детекторов).

class VB_AB_Light_MatGlow extends PointLightBase
{
	void VB_AB_Light_MatGlow()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(1.4);
		SetCastShadow(false);
		EnableSpecular(false);
		SetBrightnessTo(0.9);
		SetFadeOutTime(0.4);
		SetFlickerAmplitude(0.25);
		SetFlickerSpeed(0.7);
	}

	void VB_SetColor(vector c)
	{
		SetAmbientColor(c[0], c[1], c[2]);
		SetDiffuseColor(c[0], c[1], c[2]);
	}
};

class VB_AB_Mat_Base extends Inventory_Base
{
	static const int MAX_LIGHTS = 12;
	protected static int s_VB_Lights;
	protected VB_AB_Light_MatGlow m_VB_Light;
	protected Particle m_VB_Ptc;

	override void EEInit()
	{
		super.EEInit();
		if (!GetGame().IsDedicatedServer())
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_UpdateGlow, 300, false);
	}

	override void EEItemLocationChanged(notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc)
	{
		super.EEItemLocationChanged(oldLoc, newLoc);
		if (!GetGame().IsDedicatedServer())
			VB_UpdateGlow();
	}

	override void EEDelete(EntityAI parent)
	{
		if (GetGame())
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_UpdateGlow);
		VB_GlowOff();
		super.EEDelete(parent);
	}

	void VB_UpdateGlow()
	{
		if (!GetGame() || GetGame().IsDedicatedServer())
			return;
		if (!GetHierarchyParent() && !IsSetForDeletion())
			VB_GlowOn();
		else
			VB_GlowOff();
	}

	protected void VB_GlowOn()
	{
		if (!m_VB_Ptc)
		{
			string pal = ConfigGetString("vbABPalette");
			int id = 0;
			if (pal != "")
				id = ParticleList.GetParticleIDByName("vb_ab_aura_" + pal);
			if (id > 0)
			{
				m_VB_Ptc = Particle.PlayOnObject(id, this, "0 0.1 0");
				if (m_VB_Ptc)
				{
					m_VB_Ptc.ScaleParticleParamFromOriginal(EmitorParam.SIZE, 0.35);
					m_VB_Ptc.ScaleParticleParamFromOriginal(EmitorParam.VELOCITY, 0.3);
					m_VB_Ptc.ScaleParticleParamFromOriginal(EmitorParam.BIRTH_RATE, 0.35);
				}
			}
		}
		if (!m_VB_Light && s_VB_Lights < MAX_LIGHTS)
		{
			m_VB_Light = VB_AB_Light_MatGlow.Cast(ScriptedLightBase.CreateLight(VB_AB_Light_MatGlow, "0 0 0", 0.5));
			if (m_VB_Light)
			{
				s_VB_Lights++;
				m_VB_Light.VB_SetColor(VB_GlowColor(GetType()));
				m_VB_Light.AttachOnObject(this, "0 0.15 0");
			}
		}
	}

	protected void VB_GlowOff()
	{
		if (m_VB_Ptc)
			m_VB_Ptc.Stop();
		m_VB_Ptc = null;
		if (m_VB_Light)
		{
			m_VB_Light.FadeOut();
			s_VB_Lights = Math.Max(0, s_VB_Lights - 1);
		}
		m_VB_Light = null;
	}

	static vector VB_GlowColor(string type)
	{
		TFloatArray c = new TFloatArray;
		GetGame().ConfigGetFloatArray("CfgVehicles " + type + " vbABGlow", c);
		if (c.Count() >= 3)
			return Vector(c[0], c[1], c[2]);
		return "0.6 0.8 1";
	}
};

// Находка: звук, всплеск частиц стихии и вспышка её цвета. pal пустая (не материал) — только звук.
class VB_AB_FoundFx
{
	static void Play(vector pos, string pal, vector color)
	{
		if (!GetGame() || GetGame().IsDedicatedServer())
			return;
		EffectSound s = SEffectManager.PlaySound("VB_AB_Found_SoundSet", pos);
		if (s)
			s.SetAutodestroy(true);
		if (pal == "")
			return;
		int id = ParticleList.GetParticleIDByName("vb_ab_aura_" + pal + "_dissolve");
		if (id > 0)
		{
			Particle p = Particle.PlayInWorld(id, pos + "0 0.1 0");
			if (p)
			{
				p.ScaleParticleParamFromOriginal(EmitorParam.SIZE, 0.45);
				p.ScaleParticleParamFromOriginal(EmitorParam.VELOCITY, 0.45);
			}
		}
		VB_AB_Light_MatGlow flash = VB_AB_Light_MatGlow.Cast(ScriptedLightBase.CreateLight(VB_AB_Light_MatGlow, pos + "0 0.3 0", 0));
		if (flash)
		{
			flash.VB_SetColor(color);
			flash.SetRadiusTo(3.5);
			flash.SetBrightnessTo(3);
			flash.FadeOut(1.2);
		}
	}

	// разбор RPC_FOUND: позиция, стихия, цвет (сервер — VB_AB_Finds.FoundParam)
	static void OnRpc(ParamsReadContext ctx)
	{
		Param3<vector, string, vector> d = new Param3<vector, string, vector>(vector.Zero, "", vector.Zero);
		if (ctx.Read(d))
			Play(d.param1, d.param2, d.param3);
	}
};
