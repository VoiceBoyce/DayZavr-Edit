enum SFP_TenevnikAuraPhase
{
	OFF = 0,
	ACTIVE = 1,
	RELOAD = 2
};

class SFP_Tenevnik extends ZombieBase
{
	// RPC (удалённые вызовы)
	static const int SFP_RPC_CLONE_DEATH_FX = 721501;

	// --- синхронизация (сервер → клиент) ---
	protected int	m_SFP_AuraPhaseSync = SFP_TenevnikAuraPhase.OFF;
	protected bool	m_SFP_AggressiveSync;
	protected float	m_SFP_AuraRadiusSync;

	// --- серверное состояние ---
	protected bool			m_SFP_IsAggressive;
	protected ref Timer		m_SFP_ServerTick;
	protected ref Timer		m_SFP_AuraTimer;
	protected ref Timer		m_SFP_CloneRespawnTimer;
	protected ref Timer		m_SFP_InitialCloneSpawnTimer;
	protected int			m_SFP_InitialClonesPending;
	protected bool			m_SFP_InitialSpawning;
	protected PlayerBase	m_SFP_TargetPlayer;
	protected ref array<SFP_Tenevnik_clone> m_SFP_Clones;

	// --- клиентские эффекты ---
	protected int		m_SFP_AuraPhaseLast = -1;
	protected bool		m_SFP_AggressiveLast;
	protected EffectSound m_SFP_SndAura;
	protected EffectSound m_SFP_SndAuraReload;
	// ПРИМЕЧАНИЕ: Scripted/Point-свет нельзя хранить в strong ref (сильная ссылка) (их деструктор приватный).
	// Нужно хранить обычный указатель (без ref), а очистку делать через FadeOut() — дальше движок сам.
	protected SFP_TenevnikAuraLight m_SFP_AuraLight;

	// версия 10: имена FX (эффекты) в массивах (не в json) — чтобы легко менять
	static ref array<string> FX_SoundsetsAuraActive = { "SFP_ShumF_TeleArmor_Soundset" };
	static ref array<string> FX_SoundsetsAuraReload = { "SFP_ShumF_TeleArmorReload_Soundset" };
	// Пси-аура (звук в "наушниках" всем игрокам в радиусе)
	static ref array<string> FX_SoundsetsPsiAura = { "SFP_MozgoTras_PsiAura_Soundset" };
	static ref array<int> FX_ParticlesCloneDeath = { ParticleList.SFP_PSEDOG_CLONED };
	static ref array<string> FX_SoundsetsCloneDeath = { "SFP_PseDog_DeadClone_Soundset" };
	override bool CanBeSkinned()
	{
		return true;
	}

	void SFP_Tenevnik()
	{
		RegisterNetSyncVariableInt("m_SFP_AuraPhaseSync", 0, 2);
		RegisterNetSyncVariableBool("m_SFP_AggressiveSync");
		RegisterNetSyncVariableFloat("m_SFP_AuraRadiusSync", 0.0, 500.0, 2);

		m_SFP_Clones = new array<SFP_Tenevnik_clone>();
	}

	override void EEInit()
	{
		super.EEInit();

		if (GetGame().IsServer())
		{
			SFP_TenevnikSettings st = SFP_TenevnikSettings.Get();
			m_SFP_AuraRadiusSync = st.m_AuraRadius;
			SetHealth("", "Health", st.m_TenevnikHealth);
			SFP_SetAggressive(false);
			SFP_SetAuraPhase(SFP_TenevnikAuraPhase.OFF);
			SetSynchDirty();

			m_SFP_ServerTick = new Timer(CALL_CATEGORY_GAMEPLAY);
			m_SFP_ServerTick.Run(0.5, this, "SFP_ServerTick", null, true);
		}

		if (GetGame().IsClient() && !GetGame().IsDedicatedServer())
		{
			SFP_CreateClientLight();
		}
	}

	override void EEDelete(EntityAI parent)
	{
		if (GetGame())
		{
			if (m_SFP_ServerTick) m_SFP_ServerTick.Stop();
			if (m_SFP_AuraTimer) m_SFP_AuraTimer.Stop();
			if (m_SFP_CloneRespawnTimer) m_SFP_CloneRespawnTimer.Stop();
			if (m_SFP_InitialCloneSpawnTimer) m_SFP_InitialCloneSpawnTimer.Stop();
		}

		if (GetGame() && GetGame().IsServer())
		{
			SFP_DeleteAllClones();
		}

		if (GetGame() && GetGame().IsClient() && !GetGame().IsDedicatedServer())
		{
			StopSoundSet(m_SFP_SndAura);
			StopSoundSet(m_SFP_SndAuraReload);
			if (m_SFP_AuraLight)
			{
				m_SFP_AuraLight.FadeOut();
				m_SFP_AuraLight = null;
			}
		}

		super.EEDelete(parent);
	}

	// версия 10: гарантированно гасим всё при смерти (сервер/клиент)
	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);

		if (GetGame().IsServer())
		{
			// стоп ауры, отмена респавна, удаление клонов
			SFP_OnAggroEnd();
			SFP_SetAggressive(false);
			SFP_SetAuraPhase(SFP_TenevnikAuraPhase.OFF);
			SetSynchDirty();
			
					}

		if (GetGame().IsClient() && !GetGame().IsDedicatedServer())
		{
			// локально выключаем звук/искажение сразу, не дожидаясь синхронизации
			StopSoundSet(m_SFP_SndAura);
			StopSoundSet(m_SFP_SndAuraReload);
			if (m_SFP_AuraLight)
				m_SFP_AuraLight.ApplyOff();

			// принудительно гасим клиентские эффекты (звук/тинт/дрожь/взгляд), чтобы ничего не "залипало"
			PluginSFP_TenevnikClient pl = PluginSFP_TenevnikClient.Cast(GetPlugin(PluginSFP_TenevnikClient));
			if (pl)
				pl.SFP_ForceClearAllNow();
		}
	}

	// Принудительно отправить игрокам команду сбросить локальные эффекты.
	// Не в json — чисто тех. страховка от залипаний звука/эффектов.

	// ------------------------------------------------------------
	// Серверный тик (агр, аура, клоны)
	// ------------------------------------------------------------
	void SFP_ServerTick()
	{
		if (!IsAlive())
		{
			SFP_SetAggressive(false);
			SFP_SetAuraPhase(SFP_TenevnikAuraPhase.OFF);
			SFP_DeleteAllClones();
			
			return;
		}

		// "Агрессия" для наших эффектов:
// 1) если цель - игрок (m_ActualTarget) — считаем агро (даже если mindstate ещё не обновился)
// 2) либо mindstate CHASE/FIGHT
// 3) либо ALERTED + цель игрок
PlayerBase target_pb = PlayerBase.Cast(m_ActualTarget);
bool aggressive_now = false;

if (target_pb)
{
    aggressive_now = true;
}
else
{
    aggressive_now = (m_MindState == DayZInfectedConstants.MINDSTATE_CHASE || m_MindState == DayZInfectedConstants.MINDSTATE_FIGHT);
    if (!aggressive_now && m_MindState == DayZInfectedConstants.MINDSTATE_ALERTED)
    {
        // на всякий случай: у некоторых кастомных заражённых target может синхриться чуть позже
        if (PlayerBase.Cast(m_ActualTarget))
            aggressive_now = true;
    }
}
if (aggressive_now != m_SFP_IsAggressive)
		{
			SFP_SetAggressive(aggressive_now);
			if (aggressive_now)
				SFP_OnAggroStart();
			else
				SFP_OnAggroEnd();
		}

		if (!m_SFP_IsAggressive)
		{
			// если агро нет - выключаем пси-ауру (если включали)
			
			return;
		}

		PlayerBase currentTarget = PlayerBase.Cast(m_ActualTarget);
		if (currentTarget && currentTarget != m_SFP_TargetPlayer)
		{
			m_SFP_TargetPlayer = currentTarget;
			SFP_DeleteAllClones();
			SFP_SpawnInitialClones();
		}

		// пси-ауру обновляем ВСЕГДА, пока теневник агрессивен, даже если цель уже пропала
		

		// если цель пропала/умерла - клоны не спавним
		if (!m_SFP_TargetPlayer || !m_SFP_TargetPlayer.IsAlive())
			return;

		// при необходимости доспавниваем до максимума (но не во время первичного пачечного спавна)
		if (!m_SFP_InitialSpawning)
			SFP_TrySpawnCloneIfNeeded();

		// обновление пси-ауры уже сделано выше
	}

	// --- пси-аура: звук в наушниках игрокам в радиусе, пока теневник агрессивен ---

	protected void SFP_OnAggroStart()
	{
		// аура начинает цикл
		SFP_StartAuraCycle();

		// клоны
		m_SFP_TargetPlayer = PlayerBase.Cast(m_ActualTarget);
		SFP_SpawnInitialClones();
	}

	protected void SFP_OnAggroEnd()
	{
		SFP_StopAuraCycle();
		
		// выключаем пси-ауру всем игрокам, кому включали
		
SFP_DeleteAllClones();
		m_SFP_TargetPlayer = null;
	}

	protected void SFP_SetAggressive(bool state)
	{
		m_SFP_IsAggressive = state;
		m_SFP_AggressiveSync = state;
		SetSynchDirty();
	}

	protected void SFP_SetAuraPhase(int phase)
	{
		m_SFP_AuraPhaseSync = phase;
		SetSynchDirty();
	}

	protected float SFP_RandInRange(float minVal, float maxVal)
	{
		return Math.RandomFloatInclusive(minVal, maxVal);
	}

	// ---------------- цикл ауры ----------------
	protected void SFP_StartAuraCycle()
	{
		if (m_SFP_AuraTimer)
			m_SFP_AuraTimer.Stop();
		else
			m_SFP_AuraTimer = new Timer(CALL_CATEGORY_GAMEPLAY);

		SFP_TenevnikSettings st = SFP_TenevnikSettings.Get();
		SFP_SetAuraPhase(SFP_TenevnikAuraPhase.ACTIVE);
		m_SFP_AuraTimer.Run(SFP_RandInRange(st.m_AuraActiveMin, st.m_AuraActiveMax), this, "SFP_OnAuraActiveFinished", null, false);
	}

	protected void SFP_OnAuraActiveFinished()
	{
		if (!m_SFP_IsAggressive)
		{
			SFP_SetAuraPhase(SFP_TenevnikAuraPhase.OFF);
			return;
		}

		SFP_TenevnikSettings st = SFP_TenevnikSettings.Get();
		SFP_SetAuraPhase(SFP_TenevnikAuraPhase.RELOAD);
		m_SFP_AuraTimer.Run(SFP_RandInRange(st.m_AuraReloadMin, st.m_AuraReloadMax), this, "SFP_OnAuraReloadFinished", null, false);
	}

	protected void SFP_OnAuraReloadFinished()
	{
		if (!m_SFP_IsAggressive)
		{
			SFP_SetAuraPhase(SFP_TenevnikAuraPhase.OFF);
			return;
		}

		SFP_TenevnikSettings st = SFP_TenevnikSettings.Get();
		SFP_SetAuraPhase(SFP_TenevnikAuraPhase.ACTIVE);
		m_SFP_AuraTimer.Run(SFP_RandInRange(st.m_AuraActiveMin, st.m_AuraActiveMax), this, "SFP_OnAuraActiveFinished", null, false);
	}

	protected void SFP_StopAuraCycle()
	{
		if (m_SFP_AuraTimer)
			m_SFP_AuraTimer.Stop();
		SFP_SetAuraPhase(SFP_TenevnikAuraPhase.OFF);
	}

	bool SFP_IsAuraActiveServer()
	{
		return (m_SFP_AuraPhaseSync == SFP_TenevnikAuraPhase.ACTIVE);
	}

	// ---------------- клоны ----------------
	protected void SFP_SpawnInitialClones()
	{
		if (!GetGame().IsServer())
			return;

		// если уже шёл первичный спавн клонов - сбросим очередь
		SFP_StopInitialCloneSpawning(false);

		if (!m_SFP_TargetPlayer || !m_SFP_TargetPlayer.IsAlive())
			return;

		SFP_TenevnikSettings st = SFP_TenevnikSettings.Get();
		int initial = Math.RandomIntInclusive(st.m_CloneSpawnInitialMin, st.m_CloneSpawnInitialMax);
		if (initial < 0) initial = 0;
		if (initial > st.m_CloneCountMax) initial = st.m_CloneCountMax;

		if (initial <= 0)
		{
			SFP_ScheduleCloneRespawn();
			return;
		}

		// ВАЖНО: не спавним пачкой, чтобы не подвешивать сервер.
		// Спавним по 1 клону в секунду (первого сразу, остальные - с интервалом 1s).
		m_SFP_InitialSpawning = true;
		m_SFP_InitialClonesPending = initial - 1;

		SFP_SpawnOneClone();

		if (m_SFP_InitialClonesPending > 0)
		{
			if (!m_SFP_InitialCloneSpawnTimer)
				m_SFP_InitialCloneSpawnTimer = new Timer(CALL_CATEGORY_GAMEPLAY);
			m_SFP_InitialCloneSpawnTimer.Run(1.0, this, "SFP_InitialCloneSpawnTick", null, true);
		}
		else
		{
			// все клоны уже созданы
			m_SFP_InitialSpawning = false;
			SFP_ScheduleCloneRespawn();
		}
	}



	// Тик первичного спавна: 1 клон в секунду
	void SFP_InitialCloneSpawnTick()
	{
		if (!GetGame().IsServer())
			return;

		if (!m_SFP_IsAggressive || !m_SFP_TargetPlayer || !m_SFP_TargetPlayer.IsAlive())
		{
			SFP_StopInitialCloneSpawning(false);
			return;
		}

		if (m_SFP_InitialClonesPending <= 0)
		{
			SFP_StopInitialCloneSpawning(true);
			return;
		}

		SFP_SpawnOneClone();
		m_SFP_InitialClonesPending--;

		if (m_SFP_InitialClonesPending <= 0)
			SFP_StopInitialCloneSpawning(true);
	}

	protected void SFP_StopInitialCloneSpawning(bool scheduleRespawn)
	{
		if (!GetGame() || !GetGame().IsServer())
			return;

		if (m_SFP_InitialCloneSpawnTimer)
			m_SFP_InitialCloneSpawnTimer.Stop();

		m_SFP_InitialClonesPending = 0;
		m_SFP_InitialSpawning = false;

		if (scheduleRespawn && m_SFP_IsAggressive && m_SFP_TargetPlayer && m_SFP_TargetPlayer.IsAlive())
		{
			SFP_ScheduleCloneRespawn();
		}
	}

	protected void SFP_ScheduleCloneRespawn()
	{
		if (!GetGame().IsServer())
			return;

		if (m_SFP_CloneRespawnTimer)
			m_SFP_CloneRespawnTimer.Stop();
		else
			m_SFP_CloneRespawnTimer = new Timer(CALL_CATEGORY_GAMEPLAY);

		SFP_TenevnikSettings st = SFP_TenevnikSettings.Get();
		m_SFP_CloneRespawnTimer.Run(SFP_RandInRange(st.m_CloneRespawnMin, st.m_CloneRespawnMax), this, "SFP_TrySpawnCloneIfNeeded", null, false);
	}

	void SFP_TrySpawnCloneIfNeeded()
	{
		if (!GetGame().IsServer())
			return;

		if (!m_SFP_IsAggressive)
		{
			// если агро нет - выключаем пси-ауру (если включали)
			
			return;
		}

		if (!m_SFP_TargetPlayer || !m_SFP_TargetPlayer.IsAlive())
			return;

		SFP_TenevnikSettings st = SFP_TenevnikSettings.Get();
		SFP_CleanupDeadClones();

		if (m_SFP_Clones.Count() < st.m_CloneCountMax)
		{
			SFP_SpawnOneClone();
			// если всё ещё не добрали - снова планируем
			if (m_SFP_Clones.Count() < st.m_CloneCountMax)
				SFP_ScheduleCloneRespawn();
		}
	}

	protected void SFP_CleanupDeadClones()
	{
		for (int i = m_SFP_Clones.Count() - 1; i >= 0; i--)
		{
			SFP_Tenevnik_clone c = m_SFP_Clones[i];
			if (!c || !c.IsAlive())
				m_SFP_Clones.Remove(i);
		}
	}

	protected void SFP_DeleteAllClones()
	{
		if (!GetGame().IsServer())
			return;

		// останавливаем первичный спавн, если был
		SFP_StopInitialCloneSpawning(false);

		if (m_SFP_CloneRespawnTimer)
			m_SFP_CloneRespawnTimer.Stop();

		for (int i = 0; i < m_SFP_Clones.Count(); i++)
		{
			SFP_Tenevnik_clone c = m_SFP_Clones[i];
			if (c)
				GetGame().ObjectDelete(c);
		}
		m_SFP_Clones.Clear();
	}

	protected void SFP_SpawnOneClone()
	{
		SFP_TenevnikSettings st = SFP_TenevnikSettings.Get();
		if (m_SFP_Clones.Count() >= st.m_CloneCountMax)
			return;

		vector spawnPos = SFP_FindCloneSpawnPos(m_SFP_TargetPlayer, st.m_CloneSpawnDistMin, st.m_CloneSpawnDistMax);
		if (spawnPos == vector.Zero)
			return;

		EntityAI ent = EntityAI.Cast(GetGame().CreateObjectEx("SFP_Tenevnik_clone", spawnPos, ECE_SETUP | ECE_PLACE_ON_SURFACE | ECE_INITAI | ECE_NOLIFETIME));
		SFP_Tenevnik_clone clone = SFP_Tenevnik_clone.Cast(ent);
		if (!clone)
		{
			if (ent)
				GetGame().ObjectDelete(ent);
			return;
		}

		clone.SFP_SetMaster(this);
		clone.SetHealth("", "Health", st.m_CloneHealth);

		// версия 13: сразу "подталкиваем" ИИ клона к агре на цель (чтобы не бродил, пока не увидит)
		clone.SFP_SetAggroTarget(m_SFP_TargetPlayer);

		// Небольшая задержка на инициализацию инвентаря/прокси у заражённого (ZombieBase) (иначе иногда Attachments не создаются).
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.SFP_EquipCloneFromPlayer_Deferred, 50, false, m_SFP_TargetPlayer, clone);

		m_SFP_Clones.Insert(clone);
	}

	protected void SFP_EquipCloneFromPlayer_Deferred(PlayerBase pb, SFP_Tenevnik_clone clone)
	{
		SFP_EquipCloneFromPlayer(pb, clone);
	}

	protected vector SFP_FindCloneSpawnPos(PlayerBase pb, float distMin, float distMax)
	{
		if (!pb)
			return vector.Zero;

		vector ppos = pb.GetPosition();
		vector pfwd = pb.GetDirection();
		pfwd.Normalize();

		for (int i = 0; i < 18; i++)
		{
			float dist = Math.RandomFloatInclusive(distMin, distMax);
			float a = Math.RandomFloatInclusive(0.0, Math.PI2);
			vector dir = Vector(Math.Cos(a), 0, Math.Sin(a));
			vector pos = ppos + dir * dist;
			pos[1] = GetGame().SurfaceY(pos[0], pos[2]);

			vector to = pos - ppos;
			to[1] = 0;
			to.Normalize();
			float dot = vector.Dot(pfwd, to);

			// dot>0 => спереди. Нам нужно НЕ на камеру -> чаще всего сзади/сбоку
			if (dot > 0.2)
				continue;

			return pos;
		}

		return vector.Zero;
	}

	// --- копирование экипировки игрока на клона ---
	
// --- копирование экипировки игрока на клона ---
protected void SFP_EquipCloneFromPlayer(PlayerBase pb, SFP_Tenevnik_clone clone)
{
	if (!pb || !clone)
		return;

	// ВАЖНО для заражённых клонов с proxy-одеждой на p3d:
	// НЕ удаляйте все вложения целиком. Меняйте только управляемые слоты и аттачьте предметы в конкретные slotId.
	SFP_TenevnikUtils.CopyPlayerToClone(pb, clone);
}

protected void SFP_ClearCloneAttachments(EntityAI clone)
{
	if (!clone)
		return;

	int ac = clone.GetInventory().AttachmentCount();
	for (int i = ac - 1; i >= 0; i--)
	{
		EntityAI a = clone.GetInventory().GetAttachmentFromIndex(i);
		if (a)
			GetGame().ObjectDelete(a);
	}
}

protected int SFP_GetCurrentSlotId(EntityAI ent)
{
	if (!ent)
		return -1;
	InventoryLocation il = new InventoryLocation();
	if (!ent.GetInventory().GetCurrentInventoryLocation(il))
		return -1;
	return il.GetSlot();
}

protected EntityAI SFP_TryCreateAttachmentByInventorySlots(EntityAI parent, EntityAI src)
{
	if (!parent || !src)
		return null;

	if (!src.ConfigIsExisting("inventorySlot"))
		return null;

	array<string> invSlots = new array<string>();
	src.ConfigGetTextArray("inventorySlot", invSlots);
	if (invSlots.Count() == 0)
		return null;

	for (int i = 0; i < invSlots.Count(); i++)
	{
		int slotId = InventorySlots.GetSlotIdFromString(invSlots[i]);
		if (slotId < 0)
			continue;
		if (!parent.GetInventory().HasInventorySlot(slotId))
			continue;

		EntityAI dst = parent.GetInventory().CreateAttachmentEx(src.GetType(), slotId);
		if (dst)
			return dst;
	}

	return null;
}

protected void SFP_TryCopyHandsItem(EntityAI clone, EntityAI srcHands)
{
	if (!clone || !srcHands)
		return;

	array<string> trySlots = { "Hands", "LeftHand", "RightHand", "Melee", "Shoulder" };
	for (int i = 0; i < trySlots.Count(); i++)
	{
		int slotId = InventorySlots.GetSlotIdFromString(trySlots[i]);
		if (slotId < 0)
			continue;
		if (!clone.GetInventory().HasInventorySlot(slotId))
			continue;
		if (clone.GetInventory().FindAttachment(slotId))
			continue;

		EntityAI dst = clone.GetInventory().CreateAttachmentEx(srcHands.GetType(), slotId);
		if (!dst)
			continue;

		SFP_CopyEntityState(srcHands, dst);
		SFP_CopyEntityRecursive(srcHands, dst, 0);
		return;
	}
}

protected void SFP_CopyEntityState(EntityAI src, EntityAI dst)
{
	if (!src || !dst)
		return;

	// Ванильный хелпер: переносит переменные/агенты/здоровье/состояния магазина/оружия/энергии и т.д.
	// Это лучше, чем вручную выставлять пару полей — и даёт максимально "похожий" предмет.
	MiscGameplayFunctions.TransferItemProperties(src, dst, true, true, true, false);
}

protected void SFP_CopyEntityRecursive(EntityAI src, EntityAI dst, int depth)
	{
		if (!src || !dst)
			return;
		if (depth > 4)
			return;

		// вложения (attachments)
		int ac = src.GetInventory().AttachmentCount();
		for (int i = 0; i < ac; i++)
		{
			EntityAI sAtt = src.GetInventory().GetAttachmentFromIndex(i);
			if (!sAtt)
				continue;

			InventoryLocation il = new InventoryLocation();
			if (!sAtt.GetInventory().GetCurrentInventoryLocation(il))
				continue;
			int slotId = il.GetSlot();
			if (slotId < 0)
				continue;

			if (!dst.GetInventory().HasInventorySlot(slotId))
				continue;

			EntityAI dAtt = dst.GetInventory().CreateAttachmentEx(sAtt.GetType(), slotId);
			if (!dAtt)
				continue;

			SFP_CopyEntityState(sAtt, dAtt);
			SFP_CopyEntityRecursive(sAtt, dAtt, depth + 1);
		}

		// карго (cargo)
		CargoBase c = src.GetInventory().GetCargo();
		if (c)
		{
			int cnt = c.GetItemCount();
			for (int j = 0; j < cnt; j++)
			{
				EntityAI sItem = c.GetItem(j);
				if (!sItem)
					continue;

				EntityAI dItem = dst.GetInventory().CreateInInventory(sItem.GetType());
				if (!dItem)
					continue;

				SFP_CopyEntityState(sItem, dItem);
				SFP_CopyEntityRecursive(sItem, dItem, depth + 1);
			}
		}
	}

	void SFP_OnCloneKilled(SFP_Tenevnik_clone clone, vector pos)
	{
		// вызывается клоном на сервере
		int idx = m_SFP_Clones.Find(clone);
		if (idx != -1)
			m_SFP_Clones.Remove(idx);

		// версия 13: FX (эффекты) делаем на клиентах через RPC (удалённые вызовы) (как у Wreck_Mi8/CrashBase в ваниле),
		// чтобы частица НЕ была привязана к клону и не зависела от регистрации на сервере.
		// версия 14: переносим RPC (удалённые вызовы) на сам КЛОН (см. SFP_Tenevnik_clone.OnRPC (удалённые вызовы)),
		// потому что мастер может быть вне сетевого пузыря у игрока, который убил клона.
		// Здесь FX (эффекты) больше не рассылаем.

		// планируем доспавн
		SFP_ScheduleCloneRespawn();
	}

	// ------------------------------------------------------------
	// Партикл смерти клона (серверный effecter)
	// ------------------------------------------------------------
	// версия 12/версия 13: оставлено как fallback (не используется по умолчанию)

	// ------------------------------------------------------------
	// Отражение урона
	// ------------------------------------------------------------
	override bool EEOnDamageCalculated(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		if (GetGame().IsServer() && SFP_IsAuraActiveServer() && damageType == DamageType.FIRE_ARM)
		{
			PlayerBase shooter;
			if (source)
				shooter = PlayerBase.Cast(source.GetHierarchyRootPlayer());

			if (shooter)
			{
				SFP_TenevnikSettings st = SFP_TenevnikSettings.Get();
				shooter.AddHealth("", "Health", -st.m_ReflectedDamageHP);
			}

			// мутант не получает урон
			return false;
		}

		return super.EEOnDamageCalculated(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
	}

	// ------------------------------------------------------------
	// Клиентские визуальные эффекты (звук + тепловое искажение)
	// ------------------------------------------------------------
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();

		if (GetGame().IsDedicatedServer())
			return;

		// на всякий случай: иногда EEInit на клиенте может прийти раньше/позже синка
		SFP_CreateClientLight();

		if (m_SFP_AuraPhaseSync != m_SFP_AuraPhaseLast || m_SFP_AggressiveSync != m_SFP_AggressiveLast)
		{
			m_SFP_AuraPhaseLast = m_SFP_AuraPhaseSync;
			m_SFP_AggressiveLast = m_SFP_AggressiveSync;
			SFP_UpdateClientFX();
		}
	}

	protected void SFP_CreateClientLight()
	{
		if (m_SFP_AuraLight)
			return;
		m_SFP_AuraLight = SFP_TenevnikAuraLight.Cast(ScriptedLightBase.CreateLight(SFP_TenevnikAuraLight, "0 0 0"));
		if (m_SFP_AuraLight)
			// версия 14: смещаем ещё ниже (уровень груди), чтобы heat haze не "улетал" над головой
			m_SFP_AuraLight.AttachOnObject(this, "0 1.1 0");
	}

	protected void SFP_UpdateClientFX()
	{
		SFP_TenevnikSettings st = SFP_TenevnikSettings.Get();

		// звуки
		if (!m_SFP_AggressiveSync || m_SFP_AuraPhaseSync == SFP_TenevnikAuraPhase.OFF)
		{
			StopSoundSet(m_SFP_SndAura);
			StopSoundSet(m_SFP_SndAuraReload);
			if (m_SFP_AuraLight) m_SFP_AuraLight.ApplyOff();
			return;
		}

		if (m_SFP_AuraPhaseSync == SFP_TenevnikAuraPhase.ACTIVE)
		{
			StopSoundSet(m_SFP_SndAuraReload);
			if (!m_SFP_SndAura)
				PlaySoundSetLoop(m_SFP_SndAura, FX_SoundsetsAuraActive[0], 0.2, 0.5);

			if (m_SFP_AuraLight)
				m_SFP_AuraLight.ApplyActive(st.m_HeatHazeRadiusActive, st.m_HeatHazePowerActive);
		}
		else if (m_SFP_AuraPhaseSync == SFP_TenevnikAuraPhase.RELOAD)
		{
			StopSoundSet(m_SFP_SndAura);
			if (!m_SFP_SndAuraReload)
				PlaySoundSetLoop(m_SFP_SndAuraReload, FX_SoundsetsAuraReload[0], 0.2, 0.5);

			if (m_SFP_AuraLight)
				m_SFP_AuraLight.ApplyReload(st.m_HeatHazeRadiusReload, st.m_HeatHazePowerReload);
		}
	}

	// ------------------------------------------------------------
	// Вспомогательные методы для клиентских менеджеров
	// ------------------------------------------------------------
	bool SFP_IsAggressive()
	{
		// версия 10: даже если синк не успел обновиться — на трупе аура/фильтры не должны работать
		return m_SFP_AggressiveSync && IsAlive();
	}

	float SFP_GetAuraRadius()
	{
		return m_SFP_AuraRadiusSync;
	}

	int SFP_GetAuraPhase()
	{
		return m_SFP_AuraPhaseSync;
	}

	float SFP_GetAuraPhaseMultiplier()
	{
		// множитель фазы ауры (можно использовать для любых визуальных эффектов)
		if (!m_SFP_AggressiveSync)
			return 0.0;
		if (m_SFP_AuraPhaseSync == SFP_TenevnikAuraPhase.ACTIVE)
			return 1.0;
		if (m_SFP_AuraPhaseSync == SFP_TenevnikAuraPhase.RELOAD)
			return 0.35; // заметно слабее на перезарядке
		return 0.0;
	}

// ------------------------------------------------------------
// Репликация FX (эффекты) смерти клона
// ------------------------------------------------------------




}
