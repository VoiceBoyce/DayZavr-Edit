// SFP_Cig_Sys
// Action: smoke cigarette (hold LMB)

// Tunables
static const float SFP_CIG_SMOKE_TOTAL_TIME = 48.0;     // seconds per smoking session (hold LMB)     // seconds to smoke full 100 -> 0 ("чуть дольше")
static const float SFP_CIG_CONSUME_PER_SEC = 2.1;       //правильно расчитать потребление и таймер экшена. 100кол в сиге /20 (секунд экшн) =5 очков в сек будет потреблять
static const float SFP_CIG_COUGH_CHANCE = 0.40;         // 30% шанс 1 раз кашлянуть после завершения/прерывания

class CAContinuousSFP_SmokeCigarette : CAContinuousBase
{
	protected float m_TimeElapsed;
	protected float m_TimeToComplete;
	protected float m_ConsumePerSecond;

	void CAContinuousSFP_SmokeCigarette(float time_to_complete_action, float consume_per_second)
	{
		m_TimeToComplete = time_to_complete_action;
		m_ConsumePerSecond = consume_per_second;
	}

	override void Setup(ActionData action_data)
	{
		m_TimeElapsed = 0;
	}

	override int Execute(ActionData action_data)
	{
		if (!action_data || !action_data.m_Player || !action_data.m_MainItem)
			return UA_ERROR;

		float dt = action_data.m_Player.GetDeltaT();
		m_TimeElapsed += dt;

		// consume on server
		if (GetGame().IsServer())
		{
			SFP_SigaPack_Single cig;
			if (Class.CastTo(cig, action_data.m_MainItem) && cig.HasQuantity())
			{
				float q = cig.GetQuantity();
				float minq = cig.GetQuantityMin();
				if (q > (minq + 0.001))
				{
					float consume = m_ConsumePerSecond * dt;
					float newq = q - consume;

					// Clamp to min and finish gracefully so the "out" animation plays
					if (newq <= (minq + 0.001))
					{
						cig.SetQuantity(minq, false, false);
						cig.SFP_UpdateVisualStage_Server(true);
						return UA_FINISHED;
					}

					cig.AddQuantity(-consume, false, false);
					// force our visual stage sync (quantity sync alone may not trigger client callbacks reliably)
					cig.SFP_UpdateVisualStage_Server();
				}
			}
		}

		// stop if depleted
		ItemBase it = action_data.m_MainItem;
		if (it && it.HasQuantity() && it.GetQuantity() <= (it.GetQuantityMin() + 0.001))
			return UA_FINISHED;

		// or if session finished
		if (m_TimeElapsed >= m_TimeToComplete)
			return UA_FINISHED;

		return UA_PROCESSING;
	}

	override int Cancel(ActionData action_data)
	{
		return UA_CANCEL;
	}

	override float GetProgress()
	{
		if (m_TimeToComplete <= 0)
			return 0;
		return Math.Clamp(m_TimeElapsed / m_TimeToComplete, 0, 1);
	}
};

class ActionSFP_SmokeCigaretteCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousSFP_SmokeCigarette(SFP_CIG_SMOKE_TOTAL_TIME, SFP_CIG_CONSUME_PER_SEC);
	}
};

class ActionSFP_SmokeCigarette : ActionContinuousBase
{
	void ActionSFP_SmokeCigarette()
	{
		m_CallbackClass = ActionSFP_SmokeCigaretteCB;

		// Thermometer animation (in/loop/out)
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_TAKETEMPSELF;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_TAKETEMPSELF;

		// Stand / crouch only (no prone)
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;

		m_Text = "#STR_SFP_SIGAused";
	}

	// 1 раз кашлянуть как при болезни (через SymptomManager)
	protected void SFP_TryCoughOnce(PlayerBase player)
	{
		if (!GetGame() || !GetGame().IsServer())
			return;
		if (!player)
			return;
		if (Math.RandomFloat01() > SFP_CIG_COUGH_CHANCE)
			return;

		SymptomManager sm = player.GetSymptomManager();
		if (sm)
			sm.QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_COUGH);
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNone;
	}

	override bool HasTarget()
	{
		return false;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		SFP_SigaPack_Single cig;
		if (!Class.CastTo(cig, item))
			return false;

		return cig.HasQuantity() && cig.GetQuantity() > (cig.GetQuantityMin() + 0.001);
	}

	override bool ActionConditionContinue(ActionData action_data)
	{
		// IMPORTANT:
		// Do not stop the action by condition when quantity reaches 0.
		// Let the action component return UA_FINISHED so the "out" animation plays.
		if (!action_data || !action_data.m_Player || !action_data.m_MainItem)
			return false;

		if (action_data.m_Player.GetItemInHands() != action_data.m_MainItem)
			return false;

		return true;
	}

	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);
		SFP_SigaPack_Single cig;
		if (Class.CastTo(cig, action_data.m_MainItem))
		{
			cig.SetSmokingState(true);
			cig.SFP_UpdateVisualStage_Server();
		}
	}

	override void OnStartClient(ActionData action_data)
	{
		super.OnStartClient(action_data);
		SFP_SigaPack_Single cig;
		if (Class.CastTo(cig, action_data.m_MainItem))
			cig.StartSmokingParticle();
	}

	override void OnEndServer(ActionData action_data)
	{
		super.OnEndServer(action_data);

		SFP_SigaPack_Single cig;
		if (!Class.CastTo(cig, action_data.m_MainItem))
			return;

		cig.SetSmokingState(false);
		cig.SFP_UpdateVisualStage_Server();

		if (cig.HasQuantity() && cig.GetQuantity() <= (cig.GetQuantityMin() + 0.001))
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(cig.DeleteSafe, 1200, false);
		}

		// После завершения или прерывания экшена: 30% шанс кашлянуть 1 раз
		// Делаем небольшой делэй, чтобы не конфликтовать с "out" анимацией.
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.SFP_TryCoughOnce, 250, false, action_data.m_Player);
	}

	override void OnEndClient(ActionData action_data)
	{
		super.OnEndClient(action_data);
		SFP_SigaPack_Single cig;
		if (Class.CastTo(cig, action_data.m_MainItem))
			cig.StopSmokingParticle();
	}
};
