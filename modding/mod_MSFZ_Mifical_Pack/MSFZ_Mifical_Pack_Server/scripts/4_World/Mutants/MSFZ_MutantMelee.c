// Удар мутанта-зомби по цели. Заменяет ванильную ветку COMMANDID_ATTACK из ZombieBase.FightLogic (1.30).
// Отличия от ванили:
//  - удар по блоку не гасится полностью: урон берётся из класса "<урон>_Blocked", если он есть в CfgAmmo
//    (иначе как в ванили — Dummy_Light, без урона);
//  - в 1.30 у атак зомби m_IsHeavy всегда 0 (флаг закомментирован в DayZInfectedType.RegisterAttacks),
//    поэтому тяжёлый удар определяем по "_Heavy" в имени урона (тяжёлый берётся из AttackLong);
//  - толчок игрока от мутанта.
class MSFZ_MutantMelee
{
	// pushHit — сила толчка при обычном попадании, pushBlocked — по блоку, heavyOnly — толкать только тяжёлым ударом
	static bool AttackHit(ZombieBase zombie, float pushHit, float pushBlocked, bool heavyOnly)
	{
		DayZInfectedCommandAttack attackCommand = zombie.GetCommand_Attack();
		if (!attackCommand || !attackCommand.WasHit())
			return true;

		EntityAI target = zombie.m_ActualTarget;
		DayZInfectedAttackType attackType = zombie.m_ActualAttackType;
		if (!target || !attackType)
			return true;

		if (target.GetMeleeTargetType() == EMeleeTargetType.NONALIGNABLE)
			return false;

		vector targetPos = target.GetPosition();
		vector zombiePos = zombie.GetPosition();
		if (vector.DistanceSq(targetPos, zombiePos) > attackType.m_Distance * attackType.m_Distance)
			return true;

		// блок засчитывается, только если игрок стоит лицом к мутанту (как в ванили)
		PlayerBase player = PlayerBase.Cast(target);
		bool blocked = false;
		if (player && player.GetMeleeFightLogic() && player.GetMeleeFightLogic().IsInBlock())
		{
			float angle = Math.RAD2DEG * Math.AbsFloat(Math3D.AngleFromPosition(targetPos, MiscGameplayFunctions.GetHeadingVector(player), zombiePos));
			blocked = angle <= GameConstants.AI_MAX_BLOCKABLE_ANGLE;
		}

		string ammo = attackType.m_AmmoType;
		bool isHeavy = ammo.Contains("_Heavy");
		if (blocked)
		{
			string blockedAmmo = ammo + "_Blocked";
			if (GetGame().ConfigIsExisting("CfgAmmo " + blockedAmmo))
				ammo = blockedAmmo;
			else
				ammo = "Dummy_Light";
		}

		vector hitPosWS = target.ModelToWorld(target.GetDefaultHitPosition());
		DamageSystem.CloseCombatDamageName(zombie, target, target.GetHitComponentForAI(), ammo, hitPosWS);

		if (player && (!heavyOnly || isHeavy))
		{
			float force = pushHit;
			if (blocked)
				force = pushBlocked;
			Push(player, zombiePos, force);
		}
		return true;
	}

	// Толчок игрока от точки fromPos по горизонтали с небольшим подбросом
	static void Push(PlayerBase player, vector fromPos, float force)
	{
		if (force <= 0)
			return;

		vector dir = player.GetPosition() - fromPos;
		dir[1] = 0;
		dir.Normalize();
		vector impulse = dir * force;
		impulse[1] = 300;
		dBodyApplyImpulse(player, impulse);
	}
};
