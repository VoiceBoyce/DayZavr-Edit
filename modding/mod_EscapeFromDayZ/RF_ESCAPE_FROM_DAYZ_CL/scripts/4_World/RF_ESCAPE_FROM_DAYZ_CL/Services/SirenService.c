class RF_ESCAPE_FROM_DAYZ_CL_SirenService: Managed {

    // MARK: - Public Properties

    autoptr RF_ESCAPE_FROM_DAYZ_CL_EffectSoundsArray soundSets = {};

    // MARK: - Public methods

    void ConfigureWith(RF_ESCAPE_FROM_DAYZ_CL_SirenDTO config) {
        StopSiren();

        foreach(auto position: config.positions) {
            auto rfSound = SEffectManager.PlaySound("RF_ESCAPE_FROM_DAYZ_CL_Sirena_SoundSet", position);
            rfSound.SetSoundLoop(false);
            rfSound.SetSoundAutodestroy(true);
            soundSets.Insert(rfSound);
        }
    }

    void StopSiren() {
        foreach(auto soundSet: soundSets) {
            if (!soundSet) continue;
            SEffectManager.DestroySound(soundSet);
        }
    }
}

typedef array<EffectSound> RF_ESCAPE_FROM_DAYZ_CL_EffectSoundsArray;