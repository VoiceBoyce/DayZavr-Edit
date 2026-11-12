

class SFP_StakanGranen extends SFP_Vodka_ColorBase
{
    private bool m_HornIsPlaying;
    protected bool m_HornSynchRemote;
    protected ref EffectSound m_HornSound;

    // Константы для звуковых эффектов
    const string HORN_SOUND_PATH = "";

    void SFP_StakanGranen()
    {
        RegisterNetSyncVariableBool("m_HornIsPlaying");
        RegisterNetSyncVariableBool("m_HornSynchRemote");
    }

    override void OnVariablesSynchronized()
    {
        super.OnVariablesSynchronized();

        if (IsSoundSynchRemote() && !m_HornIsPlaying)
        {
            BlowHornSoundPlay();
        }
        else if (!IsSoundSynchRemote() && m_HornIsPlaying)
        {
            BlowHornSoundStop();
        }
    }

    void BlowHornSoundPlay()
    {
        if (m_HornSound != null && m_HornSound.IsPlaying())
        {
            m_HornSound.SoundStop();
        }

        m_HornIsPlaying = true;
        m_HornSound = SEffectManager.PlaySoundOnObject(HORN_SOUND_PATH, this);
        if (m_HornSound != null)
        {
            m_HornSound.SetMaxVolume(0.7);
            m_HornSound.SetSoundAutodestroy(true);
        }
    }

    void BlowHornSoundStop()
    {
        m_HornIsPlaying = false;
        if (m_HornSound != null)
        {
            m_HornSound.SetSoundLoop(false);
            m_HornSound.SoundStop();
        }
    }

    void BlowHorn()
    {
        if (GetGame().IsServer())
        {
            ref NoiseParams npar = new NoiseParams();
            npar.LoadFromPath("CfgVehicles SFP_StakanGranen NoiseHornCall");
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

        // Сохраняем текущий набор бутылочных действий без лишнего FillBottleSnow.
        RemoveAction(ActionFillBottleSnow);
		RemoveAction(ActionOpenVodkaRoz);
        AddAction(ActionGoChockChockStakan);
    }
}
class SFP_USSR_Kruzka: SFP_StakanGranen {};
class SFP_StakanRumka: SFP_StakanGranen {};
