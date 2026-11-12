class SFP_PseDOG_Brown_Cloned extends SFP_PseDOG_Base
{
	void SFP_PseDOG_Brown_Cloned()
	{
		// Клоны НЕ призывают других клонов
		m_SFP_CanSummonClones = false;
	}
	
	override void EEKilled(Object killer)
	{
		// На сервере помечаем, что нужно проиграть FX смерти клона
		if (GetGame() && GetGame().IsServer())
		{
			m_SFP_CloneDeathFx = true;
			SetSynchDirty();
		}
		
		// Базовая логика смерти (для животного)
		super.EEKilled(killer);
		
		// Удаляем модельку клона, чтобы не оставалось трупа
		if (GetGame() && GetGame().IsServer())
		{
			// небольшая задержка, чтобы дать время клиентам получить FX
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.SFP_DeleteCloneCorpse, 200, false);
		}
	}
	
	// Удаление именно клона (не влияет на главного мутанта)
	void SFP_DeleteCloneCorpse()
	{
		if (this && !IsAlive())
		{
			// Полностью удаляем объект клона из мира
			GetGame().ObjectDelete(this);
		}
	}
}
