modded class MSFZ_Troll
{
    float Ricar_Spawn_radius = 10;
    int Ricar_Spawn_Count = 6;
    int Ricar_Spawn_Cooldown = 120;
    bool Ricar_spawned = false;
    override bool ModCommandHandlerAfter(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
    {
        if (pCurrentCommandID == DayZInfectedConstants.COMMANDID_MOVE)
        {
            DayZInfectedInputController inputController = GetInputController();
            int mindState = inputController.GetMindState();
            
            if (mindState == DayZInfectedConstants.MINDSTATE_CHASE)
            {
                if(!Ricar_spawned)
                {
                    vector Ricar_spawn_place = this.GetPosition();
                    for (int l = 0; l < Ricar_Spawn_Count; l++)
                    {
                        vector Spider_spawnPosAll = MSFZ_GetRandomSpawnPosition(Ricar_spawn_place, Ricar_Spawn_radius);
                        GetGame().CreateObject("MSFZ_Ricar", Spider_spawnPosAll, false, true);
                        Ricar_spawned = true;
                        if(IsAlive())
                        {
                            GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(RespawnCooldownRestart, Ricar_Spawn_Cooldown*1000, false);
                        }
                    }
                }
            }
        }

        return super.ModCommandHandlerAfter(pDt, pCurrentCommandID, pCurrentCommandFinished);
    }

    void RespawnCooldownRestart()
    {
        Ricar_spawned = false;
    }

    vector MSFZ_GetRandomSpawnPosition(vector Ricar_spawn_place, float Ricar_Spawn_radius)
    {
        float x = Ricar_spawn_place[0] + Math.RandomFloat(-Ricar_Spawn_radius, Ricar_Spawn_radius);
        float z = Ricar_spawn_place[2] + Math.RandomFloat(-Ricar_Spawn_radius, Ricar_Spawn_radius);
        float y = GetGame().SurfaceY(x, z);
        return Vector(x, y, z);
    }

    override bool FightLogic(int pCurrentCommandID, DayZInfectedInputController pInputController, float pDt)
	{
		if (pCurrentCommandID == DayZInfectedConstants.COMMANDID_MOVE)
		{
			// we attack only in chase & fight state
			int mindState = pInputController.GetMindState();
			if (mindState == DayZInfectedConstants.MINDSTATE_CHASE)
			{
				return ChaseAttackLogic(pCurrentCommandID, pInputController, pDt);
			}
			else if (mindState == DayZInfectedConstants.MINDSTATE_FIGHT)
			{
				return FightAttackLogic(pCurrentCommandID, pInputController, pDt);
			}
		}
		else if (pCurrentCommandID == DayZInfectedConstants.COMMANDID_ATTACK)
		{
			DayZInfectedCommandAttack attackCommand = GetCommand_Attack();
			if (attackCommand && attackCommand.WasHit())
			{
				if (m_ActualTarget != null)
				{
					if (m_ActualTarget.GetMeleeTargetType() == EMeleeTargetType.NONALIGNABLE)
						return false;

					bool playerInBlockStance = false;
					vector targetPos = m_ActualTarget.GetPosition();
					vector hitPosWS = targetPos;
					vector zombiePos = GetPosition();

					PlayerBase playerTarget = PlayerBase.Cast(m_ActualTarget);
					if (playerTarget)
					{
						playerInBlockStance = playerTarget.GetMeleeFightLogic() && playerTarget.GetMeleeFightLogic().IsInBlock();
					}

					if (vector.DistanceSq(targetPos, zombiePos) <= m_ActualAttackType.m_Distance * m_ActualAttackType.m_Distance)
					{
						//! player is in block stance and facing the infected
						if (playerInBlockStance && (Math.RAD2DEG * Math.AbsFloat(Math3D.AngleFromPosition(targetPos, MiscGameplayFunctions.GetHeadingVector(playerTarget), zombiePos))) <= GameConstants.AI_MAX_BLOCKABLE_ANGLE)
						{
							//! infected is playing heavy attack - decrease the dmg to light
							if (m_ActualAttackType.m_IsHeavy == 1)
							{
								hitPosWS = m_ActualTarget.ModelToWorld(m_ActualTarget.GetDefaultHitPosition()); //! override hit pos by pos defined in type
								DamageSystem.CloseCombatDamageName(this, m_ActualTarget, m_ActualTarget.GetHitComponentForAI(), "MeleeZombie", hitPosWS);
                                PushPlayer(playerTarget, 1);
							}
							else
							{
								//! infected is playing light attack - do not send damage, play animation instead
								hitPosWS = m_ActualTarget.ModelToWorld(m_ActualTarget.GetDefaultHitPosition());
								DamageSystem.CloseCombatDamageName(this, m_ActualTarget, m_ActualTarget.GetHitComponentForAI(), "Dummy_Light", hitPosWS);
                                PushPlayer(playerTarget, 1);
							}
						}
						else
						{
							hitPosWS = m_ActualTarget.ModelToWorld(m_ActualTarget.GetDefaultHitPosition()); //! override hit pos by pos defined in type
							DamageSystem.CloseCombatDamageName(this, m_ActualTarget, m_ActualTarget.GetHitComponentForAI(), m_ActualAttackType.m_AmmoType, hitPosWS);
                            PushPlayer(playerTarget, 2);
						}
					}
				}
			}
			
			return true;
		}
				
		return false;
	}

    void PushPlayer(PlayerBase playerTarget, int logic)
    {
        if (playerTarget)
        {
            vector impulseDir = GetDirection();
            impulseDir[1] = 0; // Обнуляем вертикальную составляющую
            impulseDir.Normalize();
            impulseDir = impulseDir * -1;
            float impulseForce = 1;
            if (logic == 1)
            {
                impulseForce = 3000;
            }
            else if (logic == 2)
            {
                impulseForce = 1500;
            }
            vector impulse = impulseDir * impulseForce;
            impulse[1] = 300; // Устанавливаем фиксированное значение для оси Y
            dBodyApplyImpulse(playerTarget, impulse);
            Print("Pushed player " + impulse);
        }
    }
};
