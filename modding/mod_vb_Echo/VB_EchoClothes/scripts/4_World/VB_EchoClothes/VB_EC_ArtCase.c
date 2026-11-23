// Капсула для артефактов экзо Эхо (VB_EchoClothes, этап 3б; ТЗ_VB_EchoClothes.md раздел 8.4).
// Слот VB_EC_ArtCase на сервоприводе (видна на левом бедре — прокси vb_ec_artcase_box). Груз — только артефакты:
// классы с началом из vbArtPrefixes[] конфига, кроме vbArtExclude[] (кейсы-экраны OOTS). Артефакты в капсуле действуют,
// как в любом кармане (экраном она не является). Окна и кольцо на крышке (vbglow) горят, когда внутри есть артефакт.

class VB_EC_ArtCase extends Container_Base
{
	protected static ref TStringArray s_VB_Prefixes;
	protected static ref TStringArray s_VB_Exclude;
	protected bool m_VB_GlowShown;
	protected bool m_VB_GlowInit;

	protected static void VB_LoadLists()
	{
		if (s_VB_Prefixes)
			return;
		s_VB_Prefixes = new TStringArray;
		s_VB_Exclude = new TStringArray;
		GetGame().ConfigGetTextArray("CfgVehicles VB_EC_ArtCase vbArtPrefixes", s_VB_Prefixes);
		GetGame().ConfigGetTextArray("CfgVehicles VB_EC_ArtCase vbArtExclude", s_VB_Exclude);
	}

	static bool VB_IsArtifact(EntityAI item)
	{
		if (!item)
			return false;
		VB_LoadLists();
		string t = item.GetType();
		foreach (string ex : s_VB_Exclude)
		{
			if (t.IndexOf(ex) == 0)
				return false;
		}
		foreach (string pre : s_VB_Prefixes)
		{
			if (t.IndexOf(pre) == 0)
				return true;
		}
		return false;
	}

	override bool CanReceiveItemIntoCargo(EntityAI item)
	{
		if (!super.CanReceiveItemIntoCargo(item))
			return false;
		return VB_IsArtifact(item);
	}

	override bool CanSwapItemInCargo(EntityAI child_entity, EntityAI new_entity)
	{
		if (!super.CanSwapItemInCargo(child_entity, new_entity))
			return false;
		return VB_IsArtifact(new_entity);
	}

	// ---------------------------------------------------------------- свет окон (клиент)
	protected void VB_UpdateGlow()
	{
		if (GetGame().IsDedicatedServer())
			return;
		bool on = false;
		CargoBase cargo = GetInventory().GetCargo();
		if (cargo && cargo.GetItemCount() > 0)
			on = true;
		if (m_VB_GlowInit && on == m_VB_GlowShown)
			return;
		m_VB_GlowInit = true;
		m_VB_GlowShown = on;
		int idx = GetHiddenSelectionIndex("vbglow");
		if (idx < 0)
			return;
		if (on)
		{
			SetObjectTexture(idx, VB_EC_Exo_Servo_Base.GLOW_TEX_ECHO);
			SetObjectMaterial(idx, VB_EC_Exo_Servo_Base.GLOW_MAT_ON);
		}
		else
		{
			SetObjectTexture(idx, VB_EC_Exo_Servo_Base.GLOW_TEX_OFF);
			SetObjectMaterial(idx, VB_EC_Exo_Servo_Base.GLOW_MAT_OFF);
		}
	}

	override void EEInit()
	{
		super.EEInit();
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(VB_UpdateGlow, 500, false);	// груз приходит после создания
	}

	override void EECargoIn(EntityAI item)
	{
		super.EECargoIn(item);
		VB_UpdateGlow();
	}

	override void EECargoOut(EntityAI item)
	{
		super.EECargoOut(item);
		VB_UpdateGlow();
	}

	override void EEHealthLevelChanged(int oldLevel, int newLevel, string zone)
	{
		super.EEHealthLevelChanged(oldLevel, newLevel, zone);
		m_VB_GlowInit = false;
		VB_UpdateGlow();
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);
		if (!GetGame().IsDedicatedServer())
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(VB_UpdateGlow);
	}
};
