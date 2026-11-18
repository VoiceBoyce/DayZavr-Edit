// VB_MiningResources_Veles: «Велесова кирка» (ТЗ_Велесова_кирка.md).
// Наследник Магической кирки MSFZ: все её действия (руны с алтарей, рубка, разборка) остаются.
// Добыча руды, износ и «Дар Велеса» (янтарь) считает сервер мода руды по записи MineToolsData в settings.json.
//
// «Чует жилу» — только у игрока, который держит кирку в руках, и только у него (другим не видно и не слышно):
// раз в 2 с ищется ближайшая рудная жила в радиусе vbmrSenseRadius (config.cpp). Жила рядом — золотые прожилки
// и руна светятся (материал секции vbmr_glow), у навершия загорается золотой свет, кирка гудит, как роща.
// Чем ближе жила, тем ярче свет и громче гул.

class VB_MR_VelesLight extends PointLightBase
{
	void VB_MR_VelesLight()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(1.5);
		SetBrightnessTo(0.2);
		SetCastShadow(false);
		SetFlareVisible(false);
		SetAmbientColor(1.0, 0.72, 0.25);
		SetDiffuseColor(1.0, 0.72, 0.25);
	}
}

class VB_MR_Pickaxe_Veles extends MSFZ_Magic_Kirka
{
	protected const int SENSE_MS = 2000;

	protected bool m_VB_Sensing;
	protected float m_VB_Near;				// 0 — жилы рядом нет, 1 — вплотную
	protected bool m_VB_GlowOn;
	protected VB_MR_VelesLight m_VB_Light;
	protected EffectSound m_VB_Hum;

	override void EEInit()
	{
		super.EEInit();
		// зашёл в игру с киркой в руках: смены места не будет, руки у игрока заполняются чуть позже
		if (g_Game && !g_Game.IsDedicatedServer())
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_CheckHands, 1500, false);
	}

	void VB_CheckHands()
	{
		VB_SetSensing(VB_InLocalHands());
	}

	// взял в руки, убрал, выронил — включаем или выключаем чутьё (место предмета меняется и у этого игрока)
	override void EEItemLocationChanged(notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc)
	{
		super.EEItemLocationChanged(oldLoc, newLoc);
		if (!g_Game || g_Game.IsDedicatedServer())
			return;

		EntityAI owner = newLoc.GetParent();
		bool inHands = newLoc.GetType() == InventoryLocationType.HANDS && owner && owner == g_Game.GetPlayer();
		VB_SetSensing(inHands);
	}

	override void EEDelete(EntityAI parent)
	{
		if (g_Game)
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_CheckHands);
		VB_SetSensing(false);
		super.EEDelete(parent);
	}

	protected bool VB_InLocalHands()
	{
		if (!g_Game || g_Game.IsDedicatedServer())
			return false;

		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		return player && player.GetItemInHands() == this;
	}

	protected void VB_SetSensing(bool on)
	{
		if (on == m_VB_Sensing || !g_Game)
			return;

		m_VB_Sensing = on;
		if (on)
		{
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_Sense, SENSE_MS, true);
			VB_Sense();
		}
		else
		{
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_Sense);
			m_VB_Near = 0;
			VB_Apply();
		}
	}

	void VB_Sense()
	{
		// кирка ушла из рук без смены места у этого клиента (смерть, выход) — гасим
		if (!VB_InLocalHands())
		{
			VB_SetSensing(false);
			return;
		}

		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		float radius = ConfigGetFloat("vbmrSenseRadius");
		if (radius <= 0)
			radius = 30;

		vector pos = player.GetPosition();
		array<Object> objects = new array<Object>;
		g_Game.GetObjectsAtPosition3D(pos, radius, objects, null);
		float best = radius + 1;
		foreach (Object obj : objects)
		{
			if (!VB_MR_Vein_Base.Cast(obj))
				continue;

			vector veinPos = obj.GetPosition();
			float dist = vector.Distance(veinPos, pos);
			if (dist < best)
				best = dist;
		}

		m_VB_Near = 0;
		if (best <= radius)
			m_VB_Near = Math.Clamp(1.0 - best / radius, 0.05, 1.0);
		VB_Apply();
	}

	protected void VB_Apply()
	{
		bool on = m_VB_Near > 0;

		// свечение прожилок и руны: материал секции vbmr_glow (hiddenSelections, индекс 0) — только у этого игрока
		if (on != m_VB_GlowOn)
		{
			m_VB_GlowOn = on;
			string mat = ConfigGetString("vbmrGlowOff");
			if (on)
				mat = ConfigGetString("vbmrGlowOn");
			SetObjectMaterial(0, mat);
		}

		// золотой свет у навершия
		if (on && !m_VB_Light)
		{
			m_VB_Light = VB_MR_VelesLight.Cast(ScriptedLightBase.CreateLight(VB_MR_VelesLight, GetPosition()));
			if (m_VB_Light)
				m_VB_Light.AttachOnMemoryPoint(this, "vbmr_light");
		}
		else if (!on && m_VB_Light)
		{
			m_VB_Light.Destroy();
			m_VB_Light = null;
		}
		if (m_VB_Light)
		{
			m_VB_Light.SetBrightnessTo(0.15 + 0.85 * m_VB_Near);
			m_VB_Light.SetRadiusTo(1.0 + 2.0 * m_VB_Near);
		}

		// гул рощи: громче ближе к жиле
		if (on)
		{
			if (!m_VB_Hum)
				PlaySoundSetLoop(m_VB_Hum, "VB_MR_Veles_Hum_SoundSet", 0.5, 0.5);
			if (m_VB_Hum)
				m_VB_Hum.SetSoundVolume(0.25 + 0.75 * m_VB_Near);
		}
		else if (m_VB_Hum)
		{
			StopSoundSet(m_VB_Hum);
		}
	}
}
