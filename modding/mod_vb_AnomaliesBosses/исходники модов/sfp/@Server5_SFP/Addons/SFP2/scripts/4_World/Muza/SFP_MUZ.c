class SFP_MUZ extends Canteen
{
	void SFP_MUZ()
	{

	}
	
	void ~SFP_MUZ()
	{
		
	}
	
	override bool IsContainer()
	{
		return true;
	}
	
	override string GetPouringSoundset()
	{
		return "emptyVessle_Canteen_SoundSet";
	}
	
		override string GetEmptyingLoopSoundsetHard()
	{
		return "pour_HardGround_Canteen_SoundSet";
	}
	
	override string GetEmptyingLoopSoundsetSoft()
	{
		return "pour_SoftGround_Canteen_SoundSet";
	}
	
	override string GetEmptyingLoopSoundsetWater()
	{
		return "pour_Water_Canteen_SoundSet";
	}
	
	override string GetEmptyingEndSoundsetHard()
	{
		return "pour_End_HardGround_Canteen_SoundSet";
	}
	
	override string GetEmptyingEndSoundsetSoft()
	{
		return "pour_End_SoftGround_Canteen_SoundSet";
	}
	
	override string GetEmptyingEndSoundsetWater()
	{
		return "pour_End_Water_Canteen_SoundSet";
	}
	
	override bool CanPutInCargo( EntityAI parent )
	{
		if ( !super.CanPutInCargo(parent) ) {return false;}	
		if ( !(parent.IsKindOf("SFP_MUZ"))/* && !(parent.IsKindOf("Container_Base"))*/)
		{
			return true;
		}
		
		return false;
	}
	
	override bool IsOpen()
	{
		return true;
	}
	
	override void EEOnCECreate()
	{
		super.EEOnCECreate();

		int rand = Math.RandomInt(0, 10);
		if (rand > 5)
		{
			InsertAgent(eAgents.CHOLERA, 1);
		}
	}
}
//-----------------------------------
class SFP_Game_VolkKura : ItemBase
{
    private bool m_HornIsPlaying;
	protected bool m_HornSynchRemote;
    ref protected EffectSound 	m_HornSound;
	
	private int m_TextureIndex = 0;
    private autoptr array<string> m_TexturePaths;
    private autoptr array<string> m_MaterialPaths;
    private string m_SelectionName = "Display";

    void SFP_Game_VolkKura()
    {
		RegisterNetSyncVariableBool( "m_HornIsPlaying" );
		RegisterNetSyncVariableBool( "m_HornSynchRemote" );
		
		// Инициализация списка текстур
        m_TexturePaths = new array<string>;
        m_TexturePaths.Insert("SFP2\\1_item\\portable_games\\ussrVolk\\data\\gif\\sfp_animated_display_volk01.paa");
		m_TexturePaths.Insert("SFP2\\1_item\\portable_games\\ussrVolk\\data\\gif\\sfp_animated_display_volk02.paa");
		m_TexturePaths.Insert("SFP2\\1_item\\portable_games\\ussrVolk\\data\\gif\\sfp_animated_display_volk03.paa");
		m_TexturePaths.Insert("SFP2\\1_item\\portable_games\\ussrVolk\\data\\gif\\sfp_animated_display_volk04.paa");
		m_TexturePaths.Insert("SFP2\\1_item\\portable_games\\ussrVolk\\data\\gif\\sfp_animated_display_volk05.paa");
		m_TexturePaths.Insert("SFP2\\1_item\\portable_games\\ussrVolk\\data\\gif\\sfp_animated_display_volk06.paa");
		m_TexturePaths.Insert("SFP2\\1_item\\portable_games\\ussrVolk\\data\\gif\\sfp_animated_display_volk07.paa");
		m_TexturePaths.Insert("SFP2\\1_item\\portable_games\\ussrVolk\\data\\gif\\sfp_animated_display_volk08.paa");
		m_TexturePaths.Insert("SFP2\\1_item\\portable_games\\ussrVolk\\data\\gif\\sfp_animated_display_volk09.paa");
		m_TexturePaths.Insert("SFP2\\1_item\\portable_games\\ussrVolk\\data\\gif\\sfp_animated_display_volk10.paa");
		m_TexturePaths.Insert("SFP2\\1_item\\portable_games\\ussrVolk\\data\\gif\\sfp_animated_display_volk11.paa");
		m_TexturePaths.Insert("SFP2\\1_item\\portable_games\\ussrVolk\\data\\gif\\sfp_animated_display_volk12.paa");
       
		//----------------------------------------------------------------------------------------------------------------
        // Инициализация списка материалов (оставьте пустой, если хотите использовать стандартный) 
        m_MaterialPaths = new array<string>;
        //m_MaterialPaths.Insert("path\\to\\material1.rvmat");
        m_MaterialPaths.Insert("");  // Пустая строка для стандартного материала
        //m_MaterialPaths.Insert("path\\to\\material3.rvmat");
        
        // Установить начальные текстуру и материал
        SetObjectTexture(GetHiddenSelectionIndex(m_SelectionName), m_TexturePaths.Get(m_TextureIndex));
        SetObjectMaterial(GetHiddenSelectionIndex(m_SelectionName), m_MaterialPaths.Get(m_TextureIndex));
        
        // Запуск таймера
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(ChangeTexture, 407, true);
    }
	 void ChangeTexture()
    {
        // Сменить текстуру
        SetObjectTexture(GetHiddenSelectionIndex(m_SelectionName), m_TexturePaths.Get(m_TextureIndex));
        
        // Сменить материал, если путь указан
		//Здесь проверяется, если путь к материалу в m_MaterialPaths не пустой, то устанавливается новый материал. В противном случае, материал не изменяется 
        if (m_MaterialPaths.Get(m_TextureIndex) != "")
        {
            SetObjectMaterial(GetHiddenSelectionIndex(m_SelectionName), m_MaterialPaths.Get(m_TextureIndex));
        }
        
        // Обновить индекс для следующей текстуры и материала
        m_TextureIndex++;
        
        // Зациклить списки текстур и материалов, если достигнут конец
        if (m_TextureIndex >= m_TexturePaths.Count())
        {
            m_TextureIndex = 0;
        }
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
        m_HornSound = SEffectManager.PlaySoundOnObject( "SFP_SoundGameVolkYaca_SoundSet", this );
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
			npar.LoadFromPath("CfgVehicles SFP_Game_VolkKura NoiseHornCall");
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
        AddAction(ActionPlayGameVolk);
    }
};
//-----------------------------------
//-----------------------------------
class SFP_PDA_NoFunc : ItemBase
{
    private bool m_HornIsPlaying;
	protected bool m_HornSynchRemote;
    ref protected EffectSound 	m_HornSound;

    void SFP_PDA_NoFunc()
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
			npar.LoadFromPath("CfgVehicles SFP_PDA_NoFunc NoiseHornCall");
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
        AddAction(ActionWorkPDA);
    }
};
//-----------------------------------
class SFP_MusicPlayerDmgMYZ : ItemBase
{
    private bool m_HornIsPlaying;
	protected bool m_HornSynchRemote;
    ref protected EffectSound 	m_HornSound;

    void SFP_MusicPlayerDmgMYZ()
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
        m_HornSound = SEffectManager.PlaySoundOnObject( "SFP_MusicPlayerRandomNauka_SoundSet", this );
        m_HornSound.SetMaxVolume(0.5);
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
			npar.LoadFromPath("CfgVehicles SFP_MusicPlayerDmgMYZ NoiseHornCall");
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
        AddAction(ActionPlayNaukaMUZ);
    }
};	
//-----------------------------------------------------------------------------------------------
//-----------------------------------
class SFP_MusicPlayerKidsMYZ : ItemBase
{
    private bool m_HornIsPlaying;
	protected bool m_HornSynchRemote;
    ref protected EffectSound 	m_HornSound;

    void SFP_MusicPlayerKidsMYZ()
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
        m_HornSound = SEffectManager.PlaySoundOnObject( "SFP_MusicPlayerRandomTrek_SoundSet", this );
        m_HornSound.SetMaxVolume(0.5);
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
			npar.LoadFromPath("CfgVehicles SFP_MusicPlayerKidsMYZ NoiseHornCall");
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
        AddAction(ActionPlayMyzMusicPlayer);
    }
};	
//-----------------------------------------------------------------------------------------------
class SFP_BayanMYZ : ItemBase
{
    private bool m_HornIsPlaying;
	protected bool m_HornSynchRemote;
    ref protected EffectSound 	m_HornSound;

    void SFP_BayanMYZ()
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
        m_HornSound = SEffectManager.PlaySoundOnObject( "SFP_Bayan_SoundSet", this );
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
			npar.LoadFromPath("CfgVehicles SFP_BayanMYZ NoiseHornCall");
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
        AddAction(ActionPlayMyzBayan);
    }
};
//-----------------------------------------------------------------------------------------------

//-----------------------------------
//-----------------------------------------------------------------------------------------------
class SFP_ActionPopisat : ItemBase
{
    private bool m_HornIsPlaying;
	protected bool m_HornSynchRemote;
    ref protected EffectSound 	m_HornSound;
	Particle 			m_ParticleEfx;

    void SFP_ActionPopisat()
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
		m_ParticleEfx = ParticleManager.GetInstance().PlayOnObject(ParticleList.SFP_WATER_PIS_PART, this, Vector(0, 0, 0));
	}

	void BlowHornSoundStop()
	{
		m_HornIsPlaying = false;
		m_HornSound.SetSoundLoop( false );
		m_HornSound.SoundStop();
		m_ParticleEfx.Stop();
	}

    void BlowHorn()
	{	
		if (GetGame().IsServer())
		{
			ref NoiseParams npar = new NoiseParams();
			npar.LoadFromPath("CfgVehicles SFP_ActionPopisat NoiseHornCall");
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
        AddAction(ActionActionPopisat);
    }
};
//-----------------------------------

//-----------------------------------------------------------------------------------------------
class SFP_LapTop_work : ItemBase
{
    private bool m_HornIsPlaying;
	protected bool m_HornSynchRemote;
    ref protected EffectSound 	m_HornSound;

    void SFP_LapTop_work()
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
        m_HornSound = SEffectManager.PlaySoundOnObject( "SFP_gubGarmo_SoundSet1", this );
        m_HornSound.SetMaxVolume(0.5);
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
			npar.LoadFromPath("CfgVehicles SFP_LapTop_work NoiseHornCall");
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
        AddAction(ActionPlayWorkNout);
    }
};
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------

class SFP_Collection_base : ItemBase
{
    private bool m_HornIsPlaying;
	protected bool m_HornSynchRemote;
    ref protected EffectSound 	m_HornSound;

    void SFP_Collection_base()
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
        m_HornSound = SEffectManager.PlaySoundOnObject( "SFP_gubGarmo_SoundSet1", this );
        m_HornSound.SetMaxVolume(0.5);
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
			npar.LoadFromPath("CfgVehicles SFP_Collection_base NoiseHornCall");
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
        AddAction(ActionInspactJurnal);
    }
};
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
class SFP_GubGarmonMYZ : ItemBase
{
    private bool m_HornIsPlaying;
	protected bool m_HornSynchRemote;
    ref protected EffectSound 	m_HornSound;

    void SFP_GubGarmonMYZ()
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
        m_HornSound = SEffectManager.PlaySoundOnObject( "SFP_gubGarmo_SoundSet", this );
        m_HornSound.SetMaxVolume(0.5);
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
			npar.LoadFromPath("CfgVehicles SFP_GubGarmonMYZ NoiseHornCall");
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
        AddAction(ActionPlayMyzGarmon);
    }
};
//-----------------------------------------------------------------------------------------------
class SFP_GitaraMYZ_NoMusic : ItemBase
{
    private bool m_HornIsPlaying;
	protected bool m_HornSynchRemote;
    ref protected EffectSound 	m_HornSound;

    void SFP_GitaraMYZ_NoMusic()
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
			npar.LoadFromPath("CfgVehicles SFP_GitaraMYZ_NoMusic NoiseHornCall");
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
        AddAction(ActionPlayMyzGitaraNoSong);
    }
};
//-----------------------------------------------------------------------------------------------
class SFP_GitaraMYZ : ItemBase
{
    private bool m_HornIsPlaying;
	protected bool m_HornSynchRemote;
    ref protected EffectSound 	m_HornSound;

    void SFP_GitaraMYZ()
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
        m_HornSound = SEffectManager.PlaySoundOnObject( "SFP_GitaraMUZ_SoundSet", this );
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
			npar.LoadFromPath("CfgVehicles SFP_GitaraMYZ NoiseHornCall");
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
        AddAction(ActionPlayMyzGitara);
    }
};
//-----------------------------------------------------------------------------------------------
class SFP_BalalaikaMYZ : ItemBase
{
    private bool m_HornIsPlaying;
	protected bool m_HornSynchRemote;
    ref protected EffectSound 	m_HornSound;

    void SFP_BalalaikaMYZ()
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
        m_HornSound = SEffectManager.PlaySoundOnObject( "SFP_Balaika_SoundSet", this );
        m_HornSound.SetMaxVolume(1.0);
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
			npar.LoadFromPath("CfgVehicles SFP_BalalaikaMYZ NoiseHornCall");
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
        AddAction(ActionPlayMyzBalalaika);
    }
};
//-----------------------------------------------------------------------------------------------
class SFP_SvistokMYZ : ItemBase
{
    private bool m_HornIsPlaying;
	protected bool m_HornSynchRemote;
    ref protected EffectSound 	m_HornSound;

    void SFP_SvistokMYZ()
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
        m_HornSound = SEffectManager.PlaySoundOnObject( "SFP_svist_svestka_SoundSet", this );
        m_HornSound.SetMaxVolume(1.0);
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
			npar.LoadFromPath("CfgVehicles SFP_SvistokMYZ NoiseHornCall");
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
        AddAction(ActionPlayMyzSvistok);
    }
};
//-----------------------------------------------------------------------------------------------
class SFP_SIGARETA_SMOKE : ItemBase
{
    private bool m_HornIsPlaying;
	protected bool m_HornSynchRemote;
    ref protected EffectSound 	m_HornSound;
	Particle 			m_ParticleEfx;

    void SFP_SIGARETA_SMOKE()
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
		m_ParticleEfx = ParticleManager.GetInstance().PlayOnObject(ParticleList.SFP_SMOKE_SIGA_PART, this, Vector(0, 0, 0));
	}

	void BlowHornSoundStop()
	{
		m_HornIsPlaying = false;
		m_HornSound.SetSoundLoop( false );
		m_HornSound.SoundStop();
		m_ParticleEfx.Stop();
	}

    void BlowHorn()
	{	
		if (GetGame().IsServer())
		{
			ref NoiseParams npar = new NoiseParams();
			npar.LoadFromPath("CfgVehicles SFP_SIGARETA_SMOKE NoiseHornCall");
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
        AddAction(ActionPlaySigaSmoke);
    }
};
/*
class SFP_SIGARETA_SMOKE : ItemBase
{
    Particle 			m_ParticleEfx;
	private bool m_HornIsPlaying;
	protected bool m_HornSynchRemote;
    ref protected EffectSound 	m_HornSound;
	
	override void EEInit()
	{

	}
	
	void ShowSmoke()
	{
		m_ParticleEfx = ParticleManager.GetInstance().PlayOnObject(ParticleList.SFP_SMOKE_SIGA_PART, this, Vector(0, 0, 0));
	}
	
	void HideSmoke()
	{
		m_ParticleEfx.Stop();
	}


    void SFP_SIGARETA_SMOKE()
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
        m_HornSound.SetMaxVolume(1.0);
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
			npar.LoadFromPath("CfgVehicles SFP_SIGARETA_SMOKE NoiseHornCall");
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
        AddAction(ActionPlaySigaSmoke);
    }
};*/
//-----------------------------------------------------------------------------------------------
/*class SFP_VAPE_Red_SMOKE : ItemBase
{
    Particle 			m_ParticleEfx;
	private bool m_HornIsPlaying;
	protected bool m_HornSynchRemote;
    ref protected EffectSound 	m_HornSound;
	
	override void EEInit()
	{

	}
	
	void ShowSmoke()
	{
		m_ParticleEfx = ParticleManager.GetInstance().PlayOnObject(ParticleList.SFP_SMOKE_VAPE_PART, this, Vector(0, 0.084, 0.000));
	}
	
	void HideSmoke()
	{
		m_ParticleEfx.Stop();
	}


    void SFP_VAPE_Red_SMOKE()
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
        m_HornSound = SEffectManager.PlaySoundOnObject( "SFP_VapeSmokeProces_SoundSet", this );
        m_HornSound.SetMaxVolume(1.0);
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
			npar.LoadFromPath("CfgVehicles SFP_VAPE_Red_SMOKE NoiseHornCall");
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
        AddAction(ActionPlayVapeSmoke);
    }
};*/
//-----------------------------------------------------------------------------------------------
class SFP_VAPE_Red_SMOKE : ItemBase
{
    private bool m_HornIsPlaying;
	protected bool m_HornSynchRemote;
    ref protected EffectSound 	m_HornSound;
	Particle 			m_ParticleEfx;

    void SFP_VAPE_Red_SMOKE()
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
        m_HornSound = SEffectManager.PlaySoundOnObject( "SFP_VapeSmokeProces_SoundSet", this );
        m_HornSound.SetMaxVolume(0.7);
		m_HornSound.SetSoundAutodestroy( true );
		m_ParticleEfx = ParticleManager.GetInstance().PlayOnObject(ParticleList.SFP_SMOKE_VAPE_PART, this, Vector(0, 0.084, 0.000));
	}

	void BlowHornSoundStop()
	{
		m_HornIsPlaying = false;
		m_HornSound.SetSoundLoop( false );
		m_HornSound.SoundStop();
		m_ParticleEfx.Stop();
	}

    void BlowHorn()
	{	
		if (GetGame().IsServer())
		{
			ref NoiseParams npar = new NoiseParams();
			npar.LoadFromPath("CfgVehicles SFP_VAPE_Red_SMOKE NoiseHornCall");
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
        AddAction(ActionPlayVapeSmoke);
    }
};