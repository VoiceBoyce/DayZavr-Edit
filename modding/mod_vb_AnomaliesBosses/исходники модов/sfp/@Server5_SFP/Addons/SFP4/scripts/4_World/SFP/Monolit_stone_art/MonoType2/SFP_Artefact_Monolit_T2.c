/**
    SFP_Artefact_Monolit_T2
    - Client-side light: SFP_MonolitStoneLight (attached to memory point "monolit_light")
    - Client-side particle: ParticleList.SFP_MONOLIT_ARTE_PART (как в ваниле у разбитого вертолёта)
    - Client-side proximity sound (2D/in-headphones effect): plays on the local player while within 300m
    - НИКАКИХ экранных эффектов (PPE) и НИКАКОГО JSON — этот тип остаётся как есть.

    Place this file in:
        scripts\4_World\SFP\Monolit_stone_art\MonoType2\SFP_Artefact_Monolit_T2.c
*/
class SFP_Artefact_Monolit_T2 extends House
{
    static const string SFP_MEM_POINT_LIGHT = "monolit_light";

    static const float  SFP_SOUND_RADIUS_M = 300.0;
    static const float  SFP_SOUND_RADIUS_SQ = SFP_SOUND_RADIUS_M * SFP_SOUND_RADIUS_M;
    static const int    SFP_PROX_CHECK_MS = 500; // how often to check distance on client (ms)

#ifndef SERVER
    protected ScriptedLightBase m_SFP_Light;
    protected Particle         m_SFP_Particle;
    protected EffectSound      m_SFP_ProxSound;
    protected PlayerBase       m_SFP_ProxSoundOwner;
#endif

    override void EEInit()
    {
        super.EEInit();

#ifndef SERVER
        SFP_CreateClientEffects();
        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(SFP_ProximityUpdate, SFP_PROX_CHECK_MS, true);
#endif
    }

    override void EEDelete(EntityAI parent)
    {
        super.EEDelete(parent);

#ifndef SERVER
        // stop periodic checks first
        if (GetGame())
            GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(SFP_ProximityUpdate);

        // cleanup all effects
        SFP_DestroyClientEffects();
#endif
    }

#ifndef SERVER
    protected void SFP_CreateClientEffects()
    {
        // --- Light (client only) ---
        // Делаем как ванильный Bonfire: CreateLight -> AttachOnMemoryPoint -> FadeBrightness/Radius
        if (!m_SFP_Light)
        {
            // NOTE: SFP_MonolitStoneLight must exist in your mod (ScriptedLightBase child).
            m_SFP_Light = ScriptedLightBase.Cast(ScriptedLightBase.CreateLight(SFP_MonolitStoneLight, Vector(0, 0, 0), 1));

            if (!m_SFP_Light)
            {
				#ifdef DIAG_DEVELOPER
				Print("[SFP Monolit T2] CreateLight(SFP_MonolitStoneLight) failed for " + GetType());
				#endif
            }
            else
            {
                if (MemoryPointExists(SFP_MEM_POINT_LIGHT))
                {
                    m_SFP_Light.AttachOnMemoryPoint(this, SFP_MEM_POINT_LIGHT);
                }
                else
                {
                    // Fallback if model is missing the memory point (prevents engine error spam)
					#ifdef DIAG_DEVELOPER
				Print("[SFP Monolit T2] Memory point not found on model: " + SFP_MEM_POINT_LIGHT + " for " + GetType());
				#endif
                    m_SFP_Light.AttachOnObject(this, Vector(0, 0, 0), Vector(0, 0, 0));
                }

                // гарантируем, что свет реально применил радиус/яркость (как в ваниле)
                m_SFP_Light.FadeBrightnessTo(SFP_MonolitStone_Config.LIGHT_BRIGHTNESS, 0);
                m_SFP_Light.FadeRadiusTo(SFP_MonolitStone_Config.LIGHT_RADIUS, 0);
            }
        }

        // --- Particle (client only) ---
        if (!m_SFP_Particle)
        {
            // Ванильный паттерн (см. Wreck_Mi8_Crashed): PlayOnObject(ParticleList.CONST, this, localPos)
            // Важно: использовать CONST из ParticleList, чтобы не зависеть от filename и чтобы регистрация точно была подхвачена.
            if (!GetGame().IsDedicatedServer())
            {
                vector localPos = Vector(0, 0, 0);

                if (MemoryPointExists(SFP_MEM_POINT_LIGHT))
                    localPos = GetMemoryPointPos(SFP_MEM_POINT_LIGHT);

                m_SFP_Particle = ParticleManager.GetInstance().PlayOnObject(ParticleList.SFP_MONOLIT_ARTE_PART, this, localPos);
            }
        }
    }

    protected void SFP_DestroyClientEffects()
    {
        SFP_StopProximitySound(true);

        if (m_SFP_Particle)
        {
            m_SFP_Particle.Stop();
            m_SFP_Particle = null;
        }

        if (m_SFP_Light)
        {
            m_SFP_Light.FadeOut();
            m_SFP_Light = null;
        }
    }

    protected void SFP_ProximityUpdate()
    {
        // dedicated servers don't run this file thanks to #ifndef SERVER, but keep it safe anyway
        if (!GetGame() || GetGame().IsDedicatedServer())
            return;

        PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
        if (!player)
        {
            SFP_StopProximitySound();
            return;
        }

        float distSq = vector.DistanceSq(player.GetPosition(), GetPosition());

        if (distSq <= SFP_SOUND_RADIUS_SQ)
            SFP_StartProximitySound(player);
        else
            SFP_StopProximitySound();
    }

    protected void SFP_StartProximitySound(PlayerBase player)
    {
        if (!player)
            return;

        // if the local player object changed (respawn), move the sound
        if (m_SFP_ProxSoundOwner && m_SFP_ProxSoundOwner != player)
            SFP_StopProximitySound(true);

        m_SFP_ProxSoundOwner = player;

        // Sound "in headphones": play it ON the player (2D-ish in the center of the listener).
        if (!m_SFP_ProxSound)
        {
            player.PlaySoundSetLoop(m_SFP_ProxSound, "SFP_AnomalyMonolitStone_SoundSet", 0.0, 0.0);
            // Do not autodestroy looped sounds; we'll stop it manually.
            if (m_SFP_ProxSound)
                m_SFP_ProxSound.SetAutodestroy(false);
        }
    }

    protected void SFP_StopProximitySound(bool force = false)
    {
        if (m_SFP_ProxSound)
        {
            m_SFP_ProxSound.SoundStop();
            m_SFP_ProxSound = null;
        }

        m_SFP_ProxSoundOwner = null;
    }
#endif
}