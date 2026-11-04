class RF_ESCAPE_FROM_DAYZ_CL_EyesAnimationService: Managed {

    // MARK: - Private Properties

    private PlayerBase delegate;
    // private bool isAnimationStopped;
    private float closeEyeAnimationTime;
    private float openEyeAnimationTime;

    private float closeEyesAnimationDuration = 3.0;
    private float openEyesAnimationDuration = 5.0;

    // MARK: - Init Methods

    void RF_ESCAPE_FROM_DAYZ_CL_EyesAnimationService() {
        auto game = GetDayZGame();
        if (game) game.Subscribe(this, "OnUpdate");
    }

    void ~RF_ESCAPE_FROM_DAYZ_CL_EyesAnimationService() {
        auto game = GetDayZGame();
        if (game) game.Unsubscribe(this, "OnUpdate");
    }
    
    void StartCloseEyesAnimation() {
        RF_ESCAPE_FROM_DAYZ_Log().Info("Start Close animation");
        this.closeEyeAnimationTime = 0.01;
    }

    void StartOpenEyesAnimation() {
        RF_ESCAPE_FROM_DAYZ_Log().Info("Start Open Animation");
        this.openEyeAnimationTime = 0.01;
    }

    void AnimateCloseEyes(float pDt) {
        if (this.closeEyeAnimationTime == 0) return;

        if (this.closeEyeAnimationTime > this.closeEyesAnimationDuration) {
            // ResetVignetteValue();
            this.closeEyeAnimationTime = 0;
            return;
        }

        float value = Math.Lerp(0, 2, closeEyeAnimationTime / closeEyesAnimationDuration);
        SetVignetteValue(value);
        closeEyeAnimationTime += pDt;
    }

    void AnimateOpenEyes(float pDt) {
        if (this.openEyeAnimationTime == 0) return;

        if (this.openEyeAnimationTime > this.openEyesAnimationDuration) {
            ResetVignetteValue();
            this.openEyeAnimationTime = 0;
            return;
        }

        float value = Math.Lerp(2, 0, openEyeAnimationTime / openEyesAnimationDuration);
        SetVignetteValue(value);
        openEyeAnimationTime += pDt;
    }

    void OnUpdate(bool doSim, float timeslice) {
        this.AnimateCloseEyes(timeslice);
        this.AnimateOpenEyes(timeslice);
    }

    void SetVignetteValue(float value) {
        float blur = value * 0.25;
        float vignette = value;
        float colorOverlayFactor = value * 0.15;

        auto requester = RF_ESCAPE_FROM_DAYZ_PPERequesterSleepEffect.Cast(PPERequesterBank.GetRequester(RF_ESCAPE_FROM_DAYZ_PPERequesterSleepEffect));
        requester.RF_ESCAPE_FROM_DAYZ_SetEffectValues(blur, vignette, colorOverlayFactor);
        // isAnimationStopped = false;
	}

    void ResetVignetteValue() {
        // if (isAnimationStopped) return;
        RF_ESCAPE_FROM_DAYZ_Log().Info("Resetting and clear vignette");
        PPERequesterBank.GetRequester(RF_ESCAPE_FROM_DAYZ_PPERequesterSleepEffect).Stop();
        // isAnimationStopped = true;
    }

    float GetOpenEyesAnimationDuration() {
        return this.openEyesAnimationDuration;
    }

    float GetCloseEyesAnimationDuration() {
        return this.closeEyesAnimationDuration;
    }
}