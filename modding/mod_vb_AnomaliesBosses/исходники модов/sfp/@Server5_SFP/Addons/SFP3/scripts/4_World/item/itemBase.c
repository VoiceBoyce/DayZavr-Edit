class SFP_item_mobile_Simen : ItemBase
{
    private bool m_HornIsPlaying;
	protected bool m_HornSynchRemote;
    ref protected EffectSound 	m_HornSound;

    void SFP_item_mobile_Simen()
    {
		RegisterNetSyncVariableBool( "m_HornIsPlaying" );
		RegisterNetSyncVariableBool( "m_HornSynchRemote" );
    }

    override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();		

		if ( IsSoundSynchRemote() && !m_HornIsPlaying )
		{
			BlowHornSoundPlay();
		} 
		else if ( !IsSoundSynchRemote() && m_HornIsPlaying )
		{
			BlowHornSoundStop();
		}
    }

    void BlowHornSoundPlay()
	{
		m_HornIsPlaying = true;
        m_HornSound = SEffectManager.PlaySoundOnObject( "", this );
        m_HornSound.SetMaxVolume(0.7);
		m_HornSound.SetSoundAutodestroy( true );
	}

	void BlowHornSoundStop()
	{
		m_HornIsPlaying = false;
		m_HornSound.SetSoundLoop( false );
		m_HornSound.SoundStop();
	}

    void BlowHorn()
	{	
		if (GetGame().IsServer())
		{
			ref NoiseParams npar = new NoiseParams();
			npar.LoadFromPath("CfgVehicles SFP_item_mobile_Simen NoiseHornCall");
			GetGame().GetNoiseSystem().AddNoisePos(this, GetPosition(), npar);
		}
		m_HornSynchRemote = true;
		m_HornIsPlaying = false;
		SetSynchDirty();
	}

	void StopBlowingHorn()
	{	
		m_HornSynchRemote = false;
		m_HornIsPlaying = true;
		SetSynchDirty();
	}

    override bool IsSoundSynchRemote()
	{	
		return m_HornSynchRemote;
	}

    override void SetActions()
    {
        super.SetActions();        
        AddAction(ActionPhoneNabor);
    }
};
//-----------------------------------
class SFP_item_Chetki_01 : ItemBase
{
    private bool m_HornIsPlaying;
	protected bool m_HornSynchRemote;
    ref protected EffectSound 	m_HornSound;

    void SFP_item_Chetki_01()
    {
		RegisterNetSyncVariableBool( "m_HornIsPlaying" );
		RegisterNetSyncVariableBool( "m_HornSynchRemote" );
    }

    override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();		

		if ( IsSoundSynchRemote() && !m_HornIsPlaying )
		{
			BlowHornSoundPlay();
		} 
		else if ( !IsSoundSynchRemote() && m_HornIsPlaying )
		{
			BlowHornSoundStop();
		}
    }

    void BlowHornSoundPlay()
	{
		m_HornIsPlaying = true;
        m_HornSound = SEffectManager.PlaySoundOnObject( "", this );
        m_HornSound.SetMaxVolume(0.7);
		m_HornSound.SetSoundAutodestroy( true );
	}

	void BlowHornSoundStop()
	{
		m_HornIsPlaying = false;
		m_HornSound.SetSoundLoop( false );
		m_HornSound.SoundStop();
	}

    void BlowHorn()
	{	
		if (GetGame().IsServer())
		{
			ref NoiseParams npar = new NoiseParams();
			npar.LoadFromPath("CfgVehicles SFP_item_Chetki_01 NoiseHornCall");
			GetGame().GetNoiseSystem().AddNoisePos(this, GetPosition(), npar);
		}
		m_HornSynchRemote = true;
		m_HornIsPlaying = false;
		SetSynchDirty();
	}

	void StopBlowingHorn()
	{	
		m_HornSynchRemote = false;
		m_HornIsPlaying = true;
		SetSynchDirty();
	}

    override bool IsSoundSynchRemote()
	{	
		return m_HornSynchRemote;
	}

    override void SetActions()
    {
        super.SetActions();        
        AddAction(ActionChetki);
    }
};
//-----------------------------------
class SFP_Knife_Skeleton extends CombatKnife
{
	
	private bool m_HornIsPlaying;
	protected bool m_HornSynchRemote;
    ref protected EffectSound 	m_HornSound;

    void SFP_Items_osmotr_base()
    {
		RegisterNetSyncVariableBool( "m_HornIsPlaying" );
		RegisterNetSyncVariableBool( "m_HornSynchRemote" );
    }

    override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();		

		if ( IsSoundSynchRemote() && !m_HornIsPlaying )
		{
			BlowHornSoundPlay();
		} 
		else if ( !IsSoundSynchRemote() && m_HornIsPlaying )
		{
			BlowHornSoundStop();
		}
    }

    void BlowHornSoundPlay()
	{
		m_HornIsPlaying = true;
        m_HornSound = SEffectManager.PlaySoundOnObject( "", this );
        m_HornSound.SetMaxVolume(0.7);
		m_HornSound.SetSoundAutodestroy( true );
	}

	void BlowHornSoundStop()
	{
		m_HornIsPlaying = false;
		m_HornSound.SetSoundLoop( false );
		m_HornSound.SoundStop();
	}

    void BlowHorn()
	{	
		if (GetGame().IsServer())
		{
			ref NoiseParams npar = new NoiseParams();
			npar.LoadFromPath("CfgVehicles SFP_Knife_Skeleton NoiseHornCall");
			GetGame().GetNoiseSystem().AddNoisePos(this, GetPosition(), npar);
		}
		m_HornSynchRemote = true;
		m_HornIsPlaying = false;
		SetSynchDirty();
	}

	void StopBlowingHorn()
	{	
		m_HornSynchRemote = false;
		m_HornIsPlaying = true;
		SetSynchDirty();
	}

    override bool IsSoundSynchRemote()
	{	
		return m_HornSynchRemote;
	}
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionSkeletonKrut);
		AddAction(ActionUnrestrainTarget);
		AddAction(ActionSkinning);
		AddAction(ActionMineBush);
		AddAction(ActionMineTreeBark);
		AddAction(ActionDigWorms);
		AddAction(ActionShaveTarget);
		AddAction(ActionDisarmMine);
		AddAction(ActionDisarmExplosive);
		AddAction(ActionShave);
		AddAction(ActionCraftBolts);
	}
}
class SFP_Knife_Kerambit extends CombatKnife
{
	
	private bool m_HornIsPlaying;
	protected bool m_HornSynchRemote;
    ref protected EffectSound 	m_HornSound;

    void SFP_Items_osmotr_base()
    {
		RegisterNetSyncVariableBool( "m_HornIsPlaying" );
		RegisterNetSyncVariableBool( "m_HornSynchRemote" );
    }

    override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();		

		if ( IsSoundSynchRemote() && !m_HornIsPlaying )
		{
			BlowHornSoundPlay();
		} 
		else if ( !IsSoundSynchRemote() && m_HornIsPlaying )
		{
			BlowHornSoundStop();
		}
    }

    void BlowHornSoundPlay()
	{
		m_HornIsPlaying = true;
        m_HornSound = SEffectManager.PlaySoundOnObject( "", this );
        m_HornSound.SetMaxVolume(0.7);
		m_HornSound.SetSoundAutodestroy( true );
	}

	void BlowHornSoundStop()
	{
		m_HornIsPlaying = false;
		m_HornSound.SetSoundLoop( false );
		m_HornSound.SoundStop();
	}

    void BlowHorn()
	{	
		if (GetGame().IsServer())
		{
			ref NoiseParams npar = new NoiseParams();
			npar.LoadFromPath("CfgVehicles SFP_Knife_Kerambit NoiseHornCall");
			GetGame().GetNoiseSystem().AddNoisePos(this, GetPosition(), npar);
		}
		m_HornSynchRemote = true;
		m_HornIsPlaying = false;
		SetSynchDirty();
	}

	void StopBlowingHorn()
	{	
		m_HornSynchRemote = false;
		m_HornIsPlaying = true;
		SetSynchDirty();
	}

    override bool IsSoundSynchRemote()
	{	
		return m_HornSynchRemote;
	}
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionKarambaKrut);
		AddAction(ActionUnrestrainTarget);
		AddAction(ActionSkinning);
		AddAction(ActionMineBush);
		AddAction(ActionMineTreeBark);
		AddAction(ActionDigWorms);
		AddAction(ActionShaveTarget);
		AddAction(ActionDisarmMine);
		AddAction(ActionDisarmExplosive);
		AddAction(ActionShave);
		AddAction(ActionCraftBolts);
	}
}
class SFP_AmbientBar_Sound extends House
{
	
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "SFP_AmbientBar_SoundSet";
	
	
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
		}
	}
	
	override void EEDelete(EntityAI parent)
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			
			if ( m_AmbientSoundLoop )
				SEffectManager.DestroyEffect(m_AmbientSoundLoop);
		}
	}
};
class SFP_NPC_DrZnahar_voiceX2_speed extends House
{
	
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "SFP_DrZnahar_diag_SoundSetx2";
	
	
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
		}
	}
	
	override void EEDelete(EntityAI parent)
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			
			if ( m_AmbientSoundLoop )
				SEffectManager.DestroyEffect(m_AmbientSoundLoop);
		}
	}
};
//-----------------------------------
//-----------------------------------
class SFP_NPC_DrZnahar_voice extends House
{
	
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "SFP_DrZnahar_diag_SoundSet";
	
	
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
		}
	}
	
	override void EEDelete(EntityAI parent)
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			
			if ( m_AmbientSoundLoop )
				SEffectManager.DestroyEffect(m_AmbientSoundLoop);
		}
	}
};
//-----------------------------------
//-----------------------------------
class SFP_NPC_Boroda_voice extends House
{
	
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "SFP_boroda_diag_SoundSet";
	
	
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
		}
	}
	
	override void EEDelete(EntityAI parent)
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			
			if ( m_AmbientSoundLoop )
				SEffectManager.DestroyEffect(m_AmbientSoundLoop);
		}
	}
};
//-----------------------------------
//-----------------------------------
class SFP_NPC_Holod_voice extends House
{
	
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "SFP_HolodTrader_diag_SoundSet";
	
	
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
		}
	}
	
	override void EEDelete(EntityAI parent)
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			
			if ( m_AmbientSoundLoop )
				SEffectManager.DestroyEffect(m_AmbientSoundLoop);
		}
	}
};
//-----------------------------------
class SFP_NPC_Sich_voice extends House
{
	
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "SFP_Sich_diag_SoundSet";
	
	
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
		}
	}
	
	override void EEDelete(EntityAI parent)
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			
			if ( m_AmbientSoundLoop )
				SEffectManager.DestroyEffect(m_AmbientSoundLoop);
		}
	}
};
//-----------------------------------
class SFP_NPC_Ecolog_voice extends House
{
	
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "SFP_Ecolog_diag_SoundSet";
	
	
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
		}
	}
	
	override void EEDelete(EntityAI parent)
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			
			if ( m_AmbientSoundLoop )
				SEffectManager.DestroyEffect(m_AmbientSoundLoop);
		}
	}
};

//-----------------------------------
/*class SFP_Karton_box: ItemBase
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionKartonPackOpen);
	}
};
//-----------------------------------
class SFP_VodkaPack12: ItemBase
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWaterPackOpen);
	}
};
//-----------------------------------
class SFP_VodkaSFPPack12: ItemBase
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWaterPackOpen);
	}
};
//-----------------------------------

//-----------------------------------
class SFP_CanTunaPack15: ItemBase
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWaterPackOpen);
	}
};
//-----------------------------------
class SFP_PipsiPack12: ItemBase
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWaterPackOpen);
	}
};
//-----------------------------------
class SFP_CanPack6: ItemBase
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWaterPackOpen);
	}
};
//-----------------------------------
class SFP_BootleWaterPack12: ItemBase
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWaterPackOpen);
	}
};
//-----------------------------------
//-----------------------------------
class SFP_BootleWaterPack6: ItemBase
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWaterPackOpen);
	}
};*/
//-----------------------------------
//-----------------------------------
class SFP_Item_Diplomat: ItemBase
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionOsmotrItem);
	}
};
//-----------------------------------
class SFP_CatBaun_lapa: Edible_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeed);
		AddAction(ActionEatBig);
		AddAction(ActionOsmotrItem);
	}
};
//-----------------------------------
class SFP_ratdog_Otrezano_Lapa: Edible_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeed);
		AddAction(ActionEatBig);
		AddAction(ActionOsmotrItem);
	}
};
//-----------------------------------
class SFP_ZombieCivilian_hand: Edible_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeed);
		AddAction(ActionEatBig);
		AddAction(ActionOsmotrItem);
	}
};
//-----------------------------------
class SFP_Krovosos_jaw: Edible_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeed);
		AddAction(ActionEatBig);
		AddAction(ActionOsmotrItem);
	}
};
//-----------------------------------
class SFP_Trupojor_Kogot: Edible_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeed);
		AddAction(ActionEatBig);
		AddAction(ActionOsmotrItem);
	}
};
//-----------------------------------
class SFP_Psedog_hvost: Edible_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeed);
		AddAction(ActionEatBig);
		AddAction(ActionOsmotrItem);
	}
};
//-----------------------------------
class SFP_Piyavka_Yazik: Edible_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeed);
		AddAction(ActionEatBig);
		AddAction(ActionOsmotrItem);
	}
};
//-----------------------------------
class SFP_tushkano_head: Edible_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeed);
		AddAction(ActionEatBig);
		AddAction(ActionOsmotrItem);
	}
};
//-----------------------------------
class SFP_Plot_eye: Edible_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeed);
		AddAction(ActionEatBig);
		AddAction(ActionOsmotrItem);
	}
};
//-----------------------------------
class SFP_slepo_hvost: Edible_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeed);
		AddAction(ActionEatBig);
		AddAction(ActionOsmotrItem);
	}
};
//-----------------------------------
class SFP_BearZMB_Kogot: Edible_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeed);
		AddAction(ActionEatBig);
		AddAction(ActionOsmotrItem);
	}
};
//-----------------------------------
class SFP_boar_leg: Edible_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeed);
		AddAction(ActionEatBig);
		AddAction(ActionOsmotrItem);
	}
};
//-----------------------------------
class SFP_izlome_hand: Edible_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeed);
		AddAction(ActionEatBig);
		AddAction(ActionOsmotrItem);
	}
};
//-----------------------------------
class SFP_himera_uho: Edible_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeed);
		AddAction(ActionEatBig);
		AddAction(ActionOsmotrItem);
	}
};
//-----------------------------------
class SFP_himera_kogot: Edible_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeed);
		AddAction(ActionEatBig);
		AddAction(ActionOsmotrItem);
	}
};
//-----------------------------------
class SFP_DikoDog_Uho: Edible_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeed);
		AddAction(ActionEatBig);
		AddAction(ActionOsmotrItem);
	}
};
//-----------------------------------
/*class SFP_Item_scanner_anom: ItemBase
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionTurnOnScan);
		AddAction(ActionTurnOffScan);
	}
};*/
//-----------------------------------
class SFP_Items_osmotr_base : ItemBase
{
    private bool m_HornIsPlaying;
	protected bool m_HornSynchRemote;
    ref protected EffectSound 	m_HornSound;

    void SFP_Items_osmotr_base()
    {
		RegisterNetSyncVariableBool( "m_HornIsPlaying" );
		RegisterNetSyncVariableBool( "m_HornSynchRemote" );
    }

    override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();		

		if ( IsSoundSynchRemote() && !m_HornIsPlaying )
		{
			BlowHornSoundPlay();
		} 
		else if ( !IsSoundSynchRemote() && m_HornIsPlaying )
		{
			BlowHornSoundStop();
		}
    }

    void BlowHornSoundPlay()
	{
		m_HornIsPlaying = true;
        m_HornSound = SEffectManager.PlaySoundOnObject( "", this );
        m_HornSound.SetMaxVolume(0.7);
		m_HornSound.SetSoundAutodestroy( true );
	}

	void BlowHornSoundStop()
	{
		m_HornIsPlaying = false;
		m_HornSound.SetSoundLoop( false );
		m_HornSound.SoundStop();
	}

    void BlowHorn()
	{	
		if (GetGame().IsServer())
		{
			ref NoiseParams npar = new NoiseParams();
			npar.LoadFromPath("CfgVehicles SFP_Items_osmotr_base NoiseHornCall");
			GetGame().GetNoiseSystem().AddNoisePos(this, GetPosition(), npar);
		}
		m_HornSynchRemote = true;
		m_HornIsPlaying = false;
		SetSynchDirty();
	}

	void StopBlowingHorn()
	{	
		m_HornSynchRemote = false;
		m_HornIsPlaying = true;
		SetSynchDirty();
	}

    override bool IsSoundSynchRemote()
	{	
		return m_HornSynchRemote;
	}

    override void SetActions()
    {
        super.SetActions();        
        AddAction(ActionOsmotrItem);
    }
};
//-----------------------------------
class SFP_ksiva_base : ItemBase
{
    private bool m_HornIsPlaying;
	protected ref OpenableBehaviour m_Openable;
	protected bool m_HornSynchRemote;
    ref protected EffectSound 	m_HornSound;

    void SFP_ksiva_base()
    {
		m_Openable = new OpenableBehaviour(false);
		RegisterNetSyncVariableBool("m_Openable.m_IsOpened");
		UpdateVisualState();
		RegisterNetSyncVariableBool( "m_HornIsPlaying" );
		RegisterNetSyncVariableBool( "m_HornSynchRemote" );
    }
	override void Open()
	{
		m_Openable.Open();
		SetSynchDirty();

		UpdateVisualState();
	}
	override void Close()
	{
		m_Openable.Close();
		SetSynchDirty();

		UpdateVisualState();
	}
	override bool IsOpen()
	{
		return m_Openable.IsOpened();
	}
	protected void UpdateVisualState()
	{
		if ( IsOpen() )
		{
			SetAnimationPhase("cover", 1.0);
		}
		else
		{
			SetAnimationPhase("cover", 0.0);
		}
	}
	

    override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		UpdateVisualState();		

		if ( IsSoundSynchRemote() && !m_HornIsPlaying )
		{
			BlowHornSoundPlay();
		} 
		else if ( !IsSoundSynchRemote() && m_HornIsPlaying )
		{
			BlowHornSoundStop();
		}
    }

    void BlowHornSoundPlay()
	{
		m_HornIsPlaying = true;
        m_HornSound = SEffectManager.PlaySoundOnObject( "", this );
        m_HornSound.SetMaxVolume(0.7);
		m_HornSound.SetSoundAutodestroy( true );
	}

	void BlowHornSoundStop()
	{
		m_HornIsPlaying = false;
		m_HornSound.SetSoundLoop( false );
		m_HornSound.SoundStop();
	}

    void BlowHorn()
	{	
		if (GetGame().IsServer())
		{
			ref NoiseParams npar = new NoiseParams();
			npar.LoadFromPath("CfgVehicles SFP_ksiva_base NoiseHornCall");
			GetGame().GetNoiseSystem().AddNoisePos(this, GetPosition(), npar);
		}
		m_HornSynchRemote = true;
		m_HornIsPlaying = false;
		SetSynchDirty();
	}

	void StopBlowingHorn()
	{	
		m_HornSynchRemote = false;
		m_HornIsPlaying = true;
		SetSynchDirty();
	}

    override bool IsSoundSynchRemote()
	{	
		return m_HornSynchRemote;
	}

    override void SetActions()
    {
        super.SetActions();        
        AddAction(ActionKsivaProvide);
		AddAction(ActionOpen);
		AddAction(ActionClose);
    }
};
class SFP_Item_tools_3 : ItemBase
{
    private bool m_HornIsPlaying;
	protected ref OpenableBehaviour m_Openable;
	protected bool m_HornSynchRemote;
    ref protected EffectSound 	m_HornSound;

    void SFP_Item_tools_3()
    {
		m_Openable = new OpenableBehaviour(false);
		RegisterNetSyncVariableBool("m_Openable.m_IsOpened");
		UpdateVisualState();
		RegisterNetSyncVariableBool( "m_HornIsPlaying" );
		RegisterNetSyncVariableBool( "m_HornSynchRemote" );
    }
	override void Open()
	{
		m_Openable.Open();
		SetSynchDirty();

		UpdateVisualState();
	}
	override void Close()
	{
		m_Openable.Close();
		SetSynchDirty();

		UpdateVisualState();
	}
	override bool IsOpen()
	{
		return m_Openable.IsOpened();
	}
	protected void UpdateVisualState()
	{
		if ( IsOpen() )
		{
			SetAnimationPhase("cover", 1.0);
		}
		else
		{
			SetAnimationPhase("cover", 0.0);
		}
	}
	

    override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		UpdateVisualState();		

		if ( IsSoundSynchRemote() && !m_HornIsPlaying )
		{
			BlowHornSoundPlay();
		} 
		else if ( !IsSoundSynchRemote() && m_HornIsPlaying )
		{
			BlowHornSoundStop();
		}
    }

    void BlowHornSoundPlay()
	{
		m_HornIsPlaying = true;
        m_HornSound = SEffectManager.PlaySoundOnObject( "", this );
        m_HornSound.SetMaxVolume(0.7);
		m_HornSound.SetSoundAutodestroy( true );
	}

	void BlowHornSoundStop()
	{
		m_HornIsPlaying = false;
		m_HornSound.SetSoundLoop( false );
		m_HornSound.SoundStop();
	}

    void BlowHorn()
	{	
		if (GetGame().IsServer())
		{
			ref NoiseParams npar = new NoiseParams();
			npar.LoadFromPath("CfgVehicles SFP_Item_tools_3 NoiseHornCall");
			GetGame().GetNoiseSystem().AddNoisePos(this, GetPosition(), npar);
		}
		m_HornSynchRemote = true;
		m_HornIsPlaying = false;
		SetSynchDirty();
	}

	void StopBlowingHorn()
	{	
		m_HornSynchRemote = false;
		m_HornIsPlaying = true;
		SetSynchDirty();
	}

    override bool IsSoundSynchRemote()
	{	
		return m_HornSynchRemote;
	}

    override void SetActions()
    {
        super.SetActions();        
		AddAction(ActionOpen);
		AddAction(ActionClose);
    }
};
