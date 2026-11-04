

class RF_ESCAPE_FROM_DAYZ_AirstrikeEffectArea: ContaminatedArea_Base {

	const float RF_ESCAPE_FROM_DAYZ_SE_TICK_RATE = 1;

	private autoptr Timer RF_ESCAPE_FROM_DAYZ_SE_eventTimer = new Timer();
	private float RF_ESCAPE_FROM_DAYZ_SE_Lifetime = 60 * 1; // 1 minute

	
	void RF_ESCAPE_FROM_DAYZ_AirstrikeEffectArea() {
		RF_ESCAPE_FROM_DAYZ_Log().Info("[AIRSTRIKE AREA]: init");
	}

	void ~RF_ESCAPE_FROM_DAYZ_AirstrikeEffectArea() {
		RF_ESCAPE_FROM_DAYZ_SE_eventTimer.Stop();
	}

	override void EEInit() {
		super.EEInit();
		RF_ESCAPE_FROM_DAYZ_Log().Info("[AIRSTRIKE AREA]: EEInit");
		if (!GetGame().IsServer()) return;
		RF_ESCAPE_FROM_DAYZ_SE_eventTimer.Run(RF_ESCAPE_FROM_DAYZ_SE_TICK_RATE, this, "RF_ESCAPE_FROM_DAYZ_SE_updateLifeTime", NULL, true);
	}

	override void EEDelete(EntityAI parent) {
		super.EEDelete(parent);
		RF_ESCAPE_FROM_DAYZ_Log().Info("[AIRSTRIKE AREA]: EEDelete");
	}

	// override void EEOnCECreate() {
	// 	RF_ESCAPE_FROM_DAYZ_Log().Info("[AIRSTRIKE AREA]: EEOnCECreate");

	// 	// We get the PPE index for future usage and synchronization ( we must do it here for dynamic as it is not read through file )
	// 	// m_PPERequesterIdx = GetRequesterIndex(m_PPERequesterType);
	// 	RF_ESCAPE_FROM_DAYZ_Log().Info(string.Format("[AIRSTRIKE AREA]: m_PPERequesterIdx: %1", m_PPERequesterIdx));
	// }
	
	override void SetupZoneData(EffectAreaParams params) {
		RF_ESCAPE_FROM_DAYZ_Log().Info("[AIRSTRIKE AREA]: SetupZoneData");

		params.m_ParamName			= string.Format("AIRSTRIKE area (%1)", m_Position.ToString());
		params.m_ParamPartId 		= ParticleList.CONTAMINATED_AREA_GAS_BIGASS;
		params.m_ParamAroundPartId 	= ParticleList.CONTAMINATED_AREA_GAS_AROUND;
		params.m_ParamTinyPartId 	= ParticleList.CONTAMINATED_AREA_GAS_TINY;
		params.m_ParamPosHeight 	= 5;
		params.m_ParamNegHeight 	= 3;
		params.m_ParamRadius 		= 50;
		params.m_ParamInnerRings 	= 1;
		// params.m_ParamInnerSpace 	= 40;
		// params.m_ParamOuterSpace 	= 30;
		// params.m_ParamOuterOffset 	= 0;
		params.m_ParamTriggerType 	= "ContaminatedTrigger_Dynamic";
		
		super.SetupZoneData(params);

		InitZone();
	}
	
	override void DeferredInit() {
		super.DeferredInit();
		RF_ESCAPE_FROM_DAYZ_Log().Info("[AIRSTRIKE AREA]: DeferredInit");
		SetupZoneData(new EffectAreaParams);
	}

	override void InitZoneClient() {
		super.InitZoneClient();
		RF_ESCAPE_FROM_DAYZ_Log().Info("[AIRSTRIKE AREA]: InitZoneClient");
		PlaceParticles(m_Position, m_Radius, m_InnerRings, m_InnerSpacing, m_OuterRingToggle, m_OuterSpacing, m_OuterRingOffset, m_ParticleID);		
	}

	override void InitZoneServer() {
		super.InitZoneServer();
		RF_ESCAPE_FROM_DAYZ_Log().Info("[AIRSTRIKE AREA]: InitZoneServer");
		
		// We create the trigger on server
		if (m_TriggerType != "") CreateTrigger(m_PositionTrigger, m_Radius);
	}
	
	override void CreateTrigger(vector pos, int radius) {
		super.CreateTrigger(pos, radius);
		RF_ESCAPE_FROM_DAYZ_Log().Info(string.Format("[AIRSTRIKE AREA]: Create trigger (%1)", m_Position.ToString(false)));
		
		// This handles the specific case of dynamic triggers as some additionnal parameters are present
		// ContaminatedTrigger_Dynamic dynaTrigger = ContaminatedTrigger_Dynamic.Cast(m_Trigger);
		// if (!dynaTrigger) return;
		// RF_ESCAPE_FROM_DAYZ_Log().Info("[AIRSTRIKE AREA]: dynaTrigger");
		// Print(dynaTrigger)
		// dynaTrigger.SetLocalEffects(m_AroundParticleID, m_TinyParticleID, m_PPERequesterIdx);
		// dynaTrigger.SetAreaState(eAreaDecayStage.LIVE);
	}

	private void RF_ESCAPE_FROM_DAYZ_SE_updateLifeTime() {
		RF_ESCAPE_FROM_DAYZ_SE_Lifetime -= RF_ESCAPE_FROM_DAYZ_SE_TICK_RATE;
		if (RF_ESCAPE_FROM_DAYZ_SE_Lifetime > 0) return;
		RF_ESCAPE_FROM_DAYZ_Log().Info(string.Format("[AIRSTRIKE AREA]: Remove trigger (%1)", m_Position.ToString(false)));
		this.DeleteSafe();
		RF_ESCAPE_FROM_DAYZ_SE_eventTimer.Stop();
	}
}