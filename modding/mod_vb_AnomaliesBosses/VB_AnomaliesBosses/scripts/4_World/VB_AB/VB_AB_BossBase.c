// Базовый класс всех боссов (config: VB_AB_BossBase). ТЗ, разделы 3–4.
//
// Здесь только то, что нужно игроку: масштаб, аура, голос, эффекты способностей, запреты ванили и данные
// для «Вырезать трофей». Вся логика (здоровье из bosses.json, поле, поводок, способности, щит, фазы, смерть,
// трофеи) — в серверном моде VB_AnomaliesBosses_Server (modded class VB_AB_BossBase), игрокам он не отдаётся.
//
// Масштаб — абсолютный, каждый кадр (анимация сбрасывает матрицу, умножение накапливалось бы, см. MSFZ_Scale).
// Значение синхронизируется: на сервере его может переопределить bosses.json (Scale).

class VB_AB_BossBase extends ZmbM_HermitSkinny_Base
{
	protected float m_VB_Scale = 1.0;			// синхр.
	protected float m_VB_CutTime = 15.0;		// синхр.: время «Вырезать трофей», с (Trophy.CutTimeSec)
	protected bool m_VB_Carved;					// синхр.: тело уже разрезано
	protected bool m_VB_Minion;					// синхр.: спутник из «Стаи» (без поля и трофея)
	protected bool m_VB_Enraged;				// синхр.: «Ярость»
	protected bool m_VB_CarvedSeen;				// клиент: распад уже показан

	void VB_AB_BossBase()
	{
		RegisterNetSyncVariableFloat("m_VB_Scale", 0.2, 5.0, 3);
		RegisterNetSyncVariableFloat("m_VB_CutTime", 0, 120, 1);
		RegisterNetSyncVariableBool("m_VB_Carved");
		RegisterNetSyncVariableBool("m_VB_Minion");
		RegisterNetSyncVariableBool("m_VB_Enraged");
	}

	override void EEInit()
	{
		super.EEInit();
		float s = GetGame().ConfigGetFloat("CfgVehicles " + GetType() + " vbABScale");
		if (s > 0)
			m_VB_Scale = s;
		VB_UpdateScaleMask();
		VB_AB_Aura.Attach(this);		// аура палитры (только клиент, VB_AB_Aura.c)
		VB_AB_Voice.Attach(this);		// голос модели (только клиент, VB_AB_Voice.c)
	}

	override void EEDelete(EntityAI parent)
	{
		VB_AB_Aura.Detach(this);
		VB_AB_Voice.Detach(this);
		super.EEDelete(parent);
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		VB_UpdateScaleMask();
		if (m_VB_Minion)
			VB_AB_Aura.Detach(this);		// спутникам «Стаи» ауры нет: меньше света и частиц
		else
			VB_AB_Aura.Attach(this);		// повторный вызов ничего не делает, мёртвому не включается
		if (m_VB_Carved && !m_VB_CarvedSeen)
		{
			m_VB_CarvedSeen = true;
			VB_AB_Aura.Dissolve(VB_AB_Aura.BodyCenter(this), VB_AB_Aura.GetPalette(this), m_VB_Scale);
		}
	}

	// Смерть на клиенте (EEKilled вызывается только на сервере): аура гаснет
	override void OnDamageDestroyed(int oldLevel)
	{
		super.OnDamageDestroyed(oldLevel);
		VB_AB_Aura.OnDeath(this);
		VB_AB_Voice.OnDeath(this, oldLevel);
	}

	// Клиент: эффекты способностей (VB_AB_Fx)
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		if (GetGame().IsDedicatedServer())
			return;
		if (rpc_type == VB_AB_Fx.RPC_FX)
		{
			Param3<string, string, vector> fx = new Param3<string, string, vector>("", "", vector.Zero);
			if (ctx.Read(fx))
				VB_AB_Fx.Play(fx.param1, fx.param2, fx.param3);
		}
		else if (rpc_type == VB_AB_Fx.RPC_PLAYER)
		{
			Param2<int, float> pf = new Param2<int, float>(0, 0);
			if (ctx.Read(pf))
				VB_AB_Fx.OnPlayerFx(pf.param1, pf.param2);
		}
		else if (rpc_type == VB_AB_Voice.RPC_VOICE)
			VB_AB_Voice.OnRPC(this, ctx);
	}

	protected void VB_UpdateScaleMask()
	{
		if (m_VB_Scale != 1.0)
			SetEventMask(EntityEvent.POSTFRAME);
	}

	override void EOnPostFrame(IEntity other, int extra)
	{
		if (m_VB_Scale != 1.0)
			VB_ApplyScale(m_VB_Scale);
	}

	protected void VB_ApplyScale(float scale)
	{
		vector transform[4];
		GetTransform(transform);
		for (int i = 0; i < 3; i++)
		{
			vector axis = transform[i];
			axis.Normalize();
			transform[i] = axis * scale;
		}
		SetTransform(transform);
		Update();
	}

	// Босса нельзя убить добиванием со спины.
	override bool CanBeBackstabbed()
	{
		return false;
	}

	// Попадания не сбивают босса анимацией оглушения.
	override bool EvaluateDamageHitAnimation(EntityAI pSource, string pComponent, string pAmmoType, out bool pHeavyHit, out int pAnimType, out float pAnimHitDir)
	{
		return false;
	}

	// Голос модели (VB_AB_Voice.c): ванильный обработчик искал бы наборы VB_AB_BossBase_<Состояние>_SoundSet
	// (по ClassName() — скриптовому классу, одному на всех боссов), поэтому свой проигрыватель
	override protected void HandleSoundEvents()
	{
		VB_AB_Voice.OnMindState(this, GetMindStateSynced());
	}

	// Голос из анимаций (удар, «зов», прыжок) — свой вместо голоса отшельника; super не вызывать
	override void OnSoundVoiceEvent(int event_id, string event_user_string)
	{
		VB_AB_Voice.OnAnimVoice(this, event_id);
	}

	// Ванильная разделка не нужна: трофей режется своим действием (ТЗ 2.7).
	override bool CanBeSkinned()
	{
		return false;
	}

	bool VB_IsMinion()
	{
		return m_VB_Minion;
	}

	bool VB_IsEnraged()
	{
		return m_VB_Enraged;
	}

	// «Вырезать трофей»: время полоски у клиента и сервера одно (синхронизируется)
	float VB_GetCutTime()
	{
		return m_VB_CutTime;
	}

	bool VB_IsCarved()
	{
		return m_VB_Carved;
	}
};
