class Anomaly_DetectorBase: ItemBase
{
	protected ref OpenableBehaviour m_Openable;
	protected SoundOnVehicle m_DetectorSound;
	protected const int ART_SPAWN_SOUND_RPC = 543643;
	
	protected int m_RangeState = 0;
	protected int m_CurRangeState = -1;
	
	
	
	void Anomaly_DetectorBase()
	{
		//m_Openable = new OpenableBehaviour(false);
		
		//RegisterNetSyncVariableBool("m_Openable.m_IsOpened");
		RegisterNetSyncVariableInt("m_RangeState", 0, 5);
		
		UpdateVisualState();
	}
	
	override void OnWorkStart()
	{
		Open();
	}
	
	override void OnWorkStop()
	{
		m_RangeState = DetectorDistancesSignals.DISTANCE_NONE;
		UpdateSoundsAndVisuals();
		Close();
	}
	
	override void OnWork( float consumed_energy ) 
	{
		super.OnWork(consumed_energy);
		CheckArt();
	}
	
	protected void CheckArt()
	{
		
	}
	
	override void Open()
	{
		
		/*if (GetCompEM().CanWork())
			GetCompEM().SwitchOn();*/
		
		if(m_Openable)
		{
			m_Openable.Open();
			#ifdef SERVER
			SetSynchDirty();
			#endif
		}
		
		UpdateVisualState();
	}

	override void Close()
	{
		/*if (GetCompEM().IsWorking())
			GetCompEM().SwitchOff();*/
		
		if(m_Openable)
		{
			m_Openable.Close();
			#ifdef SERVER
			SetSynchDirty();
			#endif
		}
		
		UpdateVisualState();
	}
	
	override bool IsOpen()
	{
		if(m_Openable && m_Openable.IsOpened())
			return true;
		
		return false;
	}

	protected void UpdateVisualState()
	{
		if ( IsOpen() )
		{
			SetAnimationPhase("top", 1.0);
		}
		else
		{
			SetAnimationPhase("top", 0.0);
		}
	}
	
    override void OnVariablesSynchronized()
    {
        super.OnVariablesSynchronized();

        UpdateVisualState();
		UpdateSoundsAndVisuals();
    }
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionTurnOnWhileInHands);
		AddAction(ActionTurnOffWhileInHands);
	}
	
	protected void UpdateSoundsAndVisuals()
	{
		
	}
	
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		
		#ifndef SERVER
		switch(rpc_type)
		{
			case ART_SPAWN_SOUND_RPC:
				Param1<vector> art_spawn_pos = new Param1<vector>(vector.Zero);
				if(ctx.Read(art_spawn_pos))
				{
					if(m_DetectorSound)
					{
						GetGame().ObjectDelete( m_DetectorSound );
					}
					
					EffectSound sound = SEffectManager.PlaySound("ArtSpawn_SoundSet", art_spawn_pos.param1);
					sound.SetAutodestroy(true);
				}
			break;
		}
		#endif;
	}
}

class OtklikLight: PointLightBase
{
	private const float BRIGHTNESS = 0.4;
	
	void OtklikLight()
	{
		SetVisibleDuringDaylight( true );
		SetRadiusTo( 1 );
		SetSpotLightAngle( 360 );
		SetCastShadow( true );
		EnableSpecular( true );
		SetBrightnessTo( 0.5 );
		SetFadeOutTime( 0.15 );
		SetAmbientColor( 0.0, 0.5, 0.9 );
		SetDiffuseColor( 0.0, 0.5, 0.9 );
	}
	
	void SetIntencity(float percent)
	{
		SetBrightnessTo( 4 * percent );
	}
}

class OkoLight: PointLightBase
{
	private const float BRIGHTNESS = 0.4;
	
	void OkoLight()
	{
		SetVisibleDuringDaylight( true );
		SetRadiusTo( 1 );
		SetSpotLightAngle( 360 );
		SetCastShadow( true );
		EnableSpecular( true );
		SetBrightnessTo( 0.5 );
		SetFadeOutTime( 0.15 );
		SetAmbientColor( 0.0, 0.5, 0.9 );
		SetDiffuseColor( 0.0, 0.5, 0.9 );
	}
	
	void SetIntencity(float percent)
	{
		SetBrightnessTo( 4 * percent );
	}
}

class Anomaly_Detector_Otklik: Anomaly_DetectorBase
{
	private const int LAMP_SEL_INDEX = 1;
	private const int LAMP_RVMAT_INDEX = 1;
	private const string LAMP_RVMAT_OFF = "OOTS_Anomaly_Client\\data\\objects\\Detectors\\Oklik\\date\\Oklik_lampa.rvmat";
	private const string LAMP_RVMAT_ON = "OOTS_Anomaly_Client\\data\\objects\\Detectors\\Oklik\\date\\Oklik_lampaLight.rvmat";
	private const string LAMP_OFF = "OOTS_Anomaly_Client\\data\\objects\\Detectors\\Oklik\\date\\Lampa_Defaultl_Empty_ca.paa";
	private const string LAMP_ON = "OOTS_Anomaly_Client\\data\\objects\\Detectors\\Oklik\\date\\Lampa_Defaultl_Blue_ca.paa";
	
	private OtklikLight m_Light;
	
	void Anomaly_Detector_Otklik()
	{
		m_Openable = new OpenableBehaviour(false);
		
		RegisterNetSyncVariableBool("m_Openable.m_IsOpened");
	}
	
	override protected void UpdateSoundsAndVisuals()
	{
		if(m_RangeState != m_CurRangeState)
		{
			if(m_DetectorSound)
			{
				GetGame().ObjectDelete( m_DetectorSound );
			}
			
			if(m_RangeState != DetectorDistancesSignals.DISTANCE_NONE)
			{
				SetObjectTexture(LAMP_SEL_INDEX, LAMP_ON);
				SetObjectMaterial(LAMP_RVMAT_INDEX, LAMP_RVMAT_ON);
				if(!m_Light)
				{
					m_Light = OtklikLight.Cast(  ScriptedLightBase.CreateLight(OtklikLight, "0 0 0", 0.08)  );
					m_Light.AttachOnMemoryPoint(this, "light", "");
				}
			}
			
			//GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(Blink);
			
			switch (m_RangeState)
			{
				case DetectorDistancesSignals.DISTANCE_NONE:
					if(m_Light)
						m_Light.Destroy();
					SetObjectTexture(LAMP_SEL_INDEX, LAMP_OFF);
					SetObjectMaterial(LAMP_RVMAT_INDEX, LAMP_RVMAT_OFF);
				break;
				case DetectorDistancesSignals.DISTANCE_FAR:
					//StartBlink(1200);
					m_Light.SetIntencity(0.25);
					m_DetectorSound = PlaySoundLoop("detector_middle", 10);
				break;
				case DetectorDistancesSignals.DISTANCE_MIDDLE:
					//StartBlink(650);
					m_Light.SetIntencity(0.5);
					m_DetectorSound = PlaySoundLoop("detector_close", 10);
				break;
				case DetectorDistancesSignals.DISTANCE_CLOSE:
					//StartBlink(216);
					m_Light.SetIntencity(1);
					m_DetectorSound = PlaySoundLoop("detector_spawn", 10);
				break;
			}
			
			m_CurRangeState = m_RangeState;
		}
	}
	
	private void StartBlink(int delay)
	{
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Blink, delay, true);
	}
	
	private void Blink()
	{
		//Print("Blink");
		SetObjectTexture(1, "OOTS_Anomaly_Client\\data\\objects\\Detectors\\Oklik\\date\\Lampa_Defaultl_Blue_ca.paa");
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Unblink, 100, false);
	}
	
	private void Unblink()
	{
		SetObjectTexture(1, "OOTS_Anomaly_Client\\data\\objects\\Detectors\\Oklik\\date\\Lampa_Defaultl_Empty_ca.paa");
	}
}


class Anomaly_Detector_Oko: Anomaly_DetectorBase
{
	private const int EKRAN_SEL_INDEX = 1;
	private const int LAMP_SEL_INDEX = 0;
	
	private const string EKRAN_RVMAT_OFF = "OOTS_Anomaly_Client\\data\\objects\\Detectors\\Oko\\date\\Oko.rvmat";
	private const string EKRAN_RVMAT_ON = "OOTS_Anomaly_Client\\data\\objects\\Detectors\\Oko\\date\\Oko_lampaLight.rvmat";
	private const string EKRAN_TEXTURE_OFF = "OOTS_Anomaly_Client\\data\\objects\\Detectors\\Oko\\date\\OkoOff_co.paa";
	private const string EKRAN_TEXTURE_ON = "OOTS_Anomaly_Client\\data\\objects\\Detectors\\Oko\\date\\Oko_ca.paa";
	
	private const string LAMP_RVMAT_OFF = "OOTS_Anomaly_Client\\data\\objects\\Detectors\\Oko\\date\\Oko.rvmat";
	private const string LAMP_RVMAT_ON = "OOTS_Anomaly_Client\\data\\objects\\Detectors\\Oko\\date\\Oko_lampaLight.rvmat";
	private const string LAMP_TEXTURE_OFF = "OOTS_Anomaly_Client\\data\\objects\\Detectors\\Oko\\date\\OkoOff_co.paa";
	private const string LAMP_TEXTURE_ON = "OOTS_Anomaly_Client\\data\\objects\\Detectors\\Oko\\date\\OkoOn_co.paa";
	
	
	
	private OkoLight m_Light;
	
	void Anomaly_Detector_Oko()
	{
		m_Openable = new OpenableBehaviour(false);
		
		RegisterNetSyncVariableBool("m_Openable.m_IsOpened");
	}
	
	
	override void OnWorkStart()
	{
		super.OnWorkStart();
		
		#ifndef SERVER
		SetObjectTexture(EKRAN_SEL_INDEX, EKRAN_TEXTURE_ON);
		SetObjectMaterial(EKRAN_SEL_INDEX, EKRAN_RVMAT_ON);
		#endif;
	}
	
	override void OnWorkStop()
	{
		super.OnWorkStop();
		
		#ifndef SERVER
		SetObjectTexture(EKRAN_SEL_INDEX, EKRAN_TEXTURE_OFF);
		SetObjectMaterial(EKRAN_SEL_INDEX, EKRAN_RVMAT_OFF);
		#endif;
	}
	
	override protected void UpdateSoundsAndVisuals()
	{
		if(m_RangeState != m_CurRangeState)
		{
			if(m_DetectorSound)
			{
				GetGame().ObjectDelete( m_DetectorSound );
			}
			
			if(m_RangeState != DetectorDistancesSignals.DISTANCE_NONE)
			{
				//SetObjectTexture(LAMP_SEL_INDEX, LAMP_TEXTURE_ON);
				SetObjectMaterial(LAMP_SEL_INDEX, LAMP_RVMAT_ON);
				if(!m_Light)
				{
					m_Light = OkoLight.Cast(  ScriptedLightBase.CreateLight(OkoLight, "0 0 0", 0.08)  );
					m_Light.AttachOnMemoryPoint(this, "light", "");
				}
			}
			
			//GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(Blink);
			
			switch (m_RangeState)
			{
				case DetectorDistancesSignals.DISTANCE_NONE:
					if(m_Light)
						m_Light.Destroy();
					//SetObjectTexture(LAMP_SEL_INDEX, LAMP_TEXTURE_OFF);
					SetObjectMaterial(LAMP_SEL_INDEX, LAMP_RVMAT_OFF);
				break;
				case DetectorDistancesSignals.DISTANCE_FAR:
					//StartBlink(1200);
					SetObjectMaterial(LAMP_SEL_INDEX, LAMP_RVMAT_ON);
					m_Light.SetIntencity(0.25);
					m_DetectorSound = PlaySoundLoop("detector_middle", 10);
				break;
				case DetectorDistancesSignals.DISTANCE_MIDDLE:
					//StartBlink(650);
					SetObjectMaterial(LAMP_SEL_INDEX, LAMP_RVMAT_ON);
					m_Light.SetIntencity(0.5);
					m_DetectorSound = PlaySoundLoop("detector_close", 10);
				break;
				case DetectorDistancesSignals.DISTANCE_CLOSE:
					//StartBlink(216);
					SetObjectMaterial(LAMP_SEL_INDEX, LAMP_RVMAT_ON);
					m_Light.SetIntencity(1);
					m_DetectorSound = PlaySoundLoop("detector_spawn", 10);
				break;
			}
			
			m_CurRangeState = m_RangeState;
		}
	}
	
	private void StartBlink(int delay)
	{
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Blink, delay, true);
	}
	
	private void Blink()
	{
		//Print("Blink");
		SetObjectMaterial(LAMP_SEL_INDEX, LAMP_RVMAT_ON);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Unblink, 100, false);
	}
	
	private void Unblink()
	{
		SetObjectMaterial(LAMP_SEL_INDEX, LAMP_RVMAT_OFF);
	}
}

class Anomaly_Detector_Medved: Anomaly_DetectorBase
{
	private const int SCREEN_SEL_INDEX = 3;
	private const int DISPLAY_SEL_INDEX = 4;
	private const string SCREEN_EMPTY = "OOTS_Anomaly_Client\\data\\objects\\Detectors\\Medved\\date\\EkranEmpty.paa";
	private const string SCREEN_OFF = "OOTS_Anomaly_Client\\data\\objects\\Detectors\\Medved\\date\\EkranOff.paa";
	private const string SCREEN_LOW = "OOTS_Anomaly_Client\\data\\objects\\Detectors\\Medved\\date\\EkranLow.paa";
	private const string SCREEN_MID = "OOTS_Anomaly_Client\\data\\objects\\Detectors\\Medved\\date\\EkranMid.paa";
	private const string SCREEN_HIGH = "OOTS_Anomaly_Client\\data\\objects\\Detectors\\Medved\\date\\EkranHigh.paa";
	
	private int m_AnimPhase;
	
	void Anomaly_Detector_Medved()
	{
		m_Openable = new OpenableBehaviour(false);
		
		RegisterNetSyncVariableBool("m_Openable.m_IsOpened");
		
		RegisterNetSyncVariableInt("m_AnimPhase", 0, 180);
		
		
		#ifndef SERVER
		UpdateSoundsAndVisuals();
		#endif;
	}

	override void OnWorkStart()
	{
		super.OnWorkStart();
		
		#ifndef SERVER
		SetObjectTexture(SCREEN_SEL_INDEX, SCREEN_EMPTY);
		SetObjectTexture(DISPLAY_SEL_INDEX, SCREEN_EMPTY);
		#endif;
	}
	
	override void OnWorkStop()
	{
		super.OnWorkStop();
		
		#ifndef SERVER
		SetObjectTexture(SCREEN_SEL_INDEX, SCREEN_OFF);
		SetObjectTexture(DISPLAY_SEL_INDEX, SCREEN_OFF);
		#endif;
	}
	
	override protected void UpdateSoundsAndVisuals()
	{
		if(m_RangeState != m_CurRangeState)
		{
			if(m_DetectorSound)
			{
				GetGame().ObjectDelete( m_DetectorSound );
			}
			
			switch (m_RangeState)
			{
				case DetectorDistancesSignals.DISTANCE_NONE:
					SetObjectTexture(SCREEN_SEL_INDEX, SCREEN_EMPTY);
					SetObjectTexture(DISPLAY_SEL_INDEX, SCREEN_EMPTY);
				break;
				case DetectorDistancesSignals.DISTANCE_FAR:
					SetObjectTexture(SCREEN_SEL_INDEX, SCREEN_LOW);
					SetObjectTexture(DISPLAY_SEL_INDEX, SCREEN_LOW);
					m_DetectorSound = PlaySoundLoop("detector_middle", 10);
				break;
				case DetectorDistancesSignals.DISTANCE_MIDDLE:
					SetObjectTexture(SCREEN_SEL_INDEX, SCREEN_MID);
					SetObjectTexture(DISPLAY_SEL_INDEX, SCREEN_MID);
					m_DetectorSound = PlaySoundLoop("detector_close", 10);
				break;
				case DetectorDistancesSignals.DISTANCE_CLOSE:
					SetObjectTexture(SCREEN_SEL_INDEX, SCREEN_HIGH);
					SetObjectTexture(DISPLAY_SEL_INDEX, SCREEN_HIGH);
					m_DetectorSound = PlaySoundLoop("detector_spawn", 10);
				break;
			}
			
			m_CurRangeState = m_RangeState;
		}
		
		if(m_RangeState != DetectorDistancesSignals.DISTANCE_NONE)
			SetAnimationPhase("ekran", m_AnimPhase / 180);
	}
}

class Anomaly_Detector_Svarog: Anomaly_DetectorBase
{

	private const int SVAROG_SEL_INDEX = 3;
	private const string SVAROG_RVMAT_OFF = "OOTS_Anomaly_Client\\data\\objects\\Detectors\\Svarog\\date\\SvarogMat.rvmat";
	private const string SVAROG_RVMAT_ON = "OOTS_Anomaly_Client\\data\\objects\\Detectors\\Svarog\\date\\Svadog_EkranOn.rvmat";
	private const string SVAROG_OFF = "OOTS_Anomaly_Client\\data\\objects\\Detectors\\Svarog\\date\\Svarog_default_co.paa";
	private const string SVAROG_ON = "OOTS_Anomaly_Client\\data\\objects\\Detectors\\Svarog\\date\\Svadog_EkranOn.paa";
	
	private float m_Art1Dist = -1;
	private float m_Art1Angle;
	
	private float m_Art2Dist = -1;
	private float m_Art2Angle;
	
	private float m_Art3Dist = -1;
	private float m_Art3Angle;
	
	private float m_Art4Dist = -1;
	private float m_Art4Angle;
	
	void Anomaly_Detector_Svarog()
	{
		RegisterNetSyncVariableFloat("m_Art1Dist", -1, 1, 2);
		RegisterNetSyncVariableFloat("m_Art1Angle", 0, 1, 2);
		
		RegisterNetSyncVariableFloat("m_Art2Dist", -1, 1, 2);
		RegisterNetSyncVariableFloat("m_Art2Angle", 0, 1, 2);
		
		RegisterNetSyncVariableFloat("m_Art3Dist", -1, 1, 2);
		RegisterNetSyncVariableFloat("m_Art3Angle", 0, 1, 2);
		
		RegisterNetSyncVariableFloat("m_Art4Dist", -1, 1, 2);
		RegisterNetSyncVariableFloat("m_Art4Angle", 0, 1, 2);
		
		#ifndef SERVER
		UpdateSoundsAndVisuals();
		#endif;
	}

	override void OnWorkStart()
	{
		super.OnWorkStart();
		
		#ifndef SERVER
		SetObjectTexture(SVAROG_SEL_INDEX, SVAROG_ON);
		SetObjectMaterial(SVAROG_SEL_INDEX, SVAROG_RVMAT_ON);
		#endif
	}
	
	override void OnWorkStop()
	{
		super.OnWorkStop();
		
		#ifndef SERVER
		SetObjectTexture(SVAROG_SEL_INDEX, SVAROG_OFF);
		SetObjectMaterial(SVAROG_SEL_INDEX, SVAROG_RVMAT_OFF);
		m_Art1Dist = -1;
		m_Art2Dist = -1;
		m_Art3Dist = -1;
		m_Art4Dist = -1;
		UpdateSoundsAndVisuals();
		#endif
	}
	
	override protected void UpdateSoundsAndVisuals()
	{

		HandleArt(1, m_Art1Dist, m_Art1Angle);
		HandleArt(2, m_Art2Dist, m_Art2Angle);
		HandleArt(3, m_Art3Dist, m_Art3Angle);
		HandleArt(4, m_Art4Dist, m_Art4Angle);
	}
	
	private void HandleArt(int index, float translate, float rotate)
	{
		string art = "art" + index.ToString();
		if(translate < 0)
		{
			SetAnimationPhase(art + "_hide", 1);
			return;
		}
		
		SetAnimationPhase(art + "_translate", translate);
		SetAnimationPhase(art + "_rotate", rotate);
		SetAnimationPhase(art + "_hide", 0);
	}
	
	
	/*override void SetActions()
	{
		super.SetActions();
		
		RemoveAction(ActionOpen);
		RemoveAction(ActionClose);
		AddAction(ActionTurnOnWhileInHands);
		AddAction(ActionTurnOffWhileInHands);
	}*/
}

class Anomaly_Detector_Veles: Anomaly_DetectorBase
{

	private const int VELES_SEL_INDEX = 3;
	private const string VELES_RVMAT_OFF = "OOTS_Anomaly_Client\\data\\objects\\Detectors\\Veles\\date\\VelesMat.rvmat";
	private const string VELES_RVMAT_ON = "OOTS_Anomaly_Client\\data\\objects\\Detectors\\Veles\\date\\Veles_EkranOn.rvmat";
	private const string VELES_OFF = "OOTS_Anomaly_Client\\data\\objects\\Detectors\\Veles\\date\\Veles_default_co.paa";
	private const string VELES_ON = "OOTS_Anomaly_Client\\data\\objects\\Detectors\\Veles\\date\\Veles_EkranOn.paa";
	
	private float m_Art1Dist = -1;
	private float m_Art1Angle;
	
	private float m_Art2Dist = -1;
	private float m_Art2Angle;
	
	private float m_Art3Dist = -1;
	private float m_Art3Angle;
	
	private float m_Art4Dist = -1;
	private float m_Art4Angle;
	
	void Anomaly_Detector_Veles()
	{
		RegisterNetSyncVariableFloat("m_Art1Dist", -1, 1, 2);
		RegisterNetSyncVariableFloat("m_Art1Angle", 0, 1, 2);
		
		RegisterNetSyncVariableFloat("m_Art2Dist", -1, 1, 2);
		RegisterNetSyncVariableFloat("m_Art2Angle", 0, 1, 2);
		
		RegisterNetSyncVariableFloat("m_Art3Dist", -1, 1, 2);
		RegisterNetSyncVariableFloat("m_Art3Angle", 0, 1, 2);
		
		RegisterNetSyncVariableFloat("m_Art4Dist", -1, 1, 2);
		RegisterNetSyncVariableFloat("m_Art4Angle", 0, 1, 2);
		
		#ifndef SERVER
		UpdateSoundsAndVisuals();
		#endif;
	}

	override void OnWorkStart()
	{
		super.OnWorkStart();
		
		#ifndef SERVER
		SetObjectTexture(VELES_SEL_INDEX, VELES_ON);
		SetObjectMaterial(VELES_SEL_INDEX, VELES_RVMAT_ON);
		#endif
	}
	
	override void OnWorkStop()
	{
		super.OnWorkStop();
		
		#ifndef SERVER
		SetObjectTexture(VELES_SEL_INDEX, VELES_OFF);
		SetObjectMaterial(VELES_SEL_INDEX, VELES_RVMAT_OFF);
		m_Art1Dist = -1;
		m_Art2Dist = -1;
		m_Art3Dist = -1;
		m_Art4Dist = -1;
		UpdateSoundsAndVisuals();
		#endif
	}
	
	override protected void UpdateSoundsAndVisuals()
	{

		HandleArt(1, m_Art1Dist, m_Art1Angle);
		HandleArt(2, m_Art2Dist, m_Art2Angle);
		HandleArt(3, m_Art3Dist, m_Art3Angle);
		HandleArt(4, m_Art4Dist, m_Art4Angle);
	}
	
	private void HandleArt(int index, float translate, float rotate)
	{
		string art = "art" + index.ToString();
		if(translate < 0)
		{
			SetAnimationPhase(art + "_hide", 1);
			return;
		}
		
		SetAnimationPhase(art + "_translate", translate);
		SetAnimationPhase(art + "_rotate", rotate);
		SetAnimationPhase(art + "_hide", 0);
	}
	
}