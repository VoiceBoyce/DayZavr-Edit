// Custom FOG PPE modes without vignetting/edge darkening
// These are used instead of NV_DEFAULT_GLASSES to achieve clean green screen
// Follows vanilla NVG effect profile but excludes edge vignetting
/*
modded class PPERequester_CameraNV
{
    // TODO: uncomment when ready to finalize FOG NVG visual appearance
    // FOG_NV_CLEAN_GREEN (mode 1000) = custom green NVG with no vignette
    protected override void SetNVMode(int mode)
    {
        if (mode == FOG_NV_CLEAN_GREEN)
        {
            // Green colorization
            SetTargetValueColor(PostProcessEffectType.Glow, PPEGlow.PARAM_COLORIZATIONCOLOR, {0.0, 0.7, 0.0, 1.0}, PPEGlow.L_23_NVG, PPOperators.MULTIPLICATIVE);
            // Saturation
            SetTargetValueFloat(PostProcessEffectType.Glow, PPEGlow.PARAM_SATURATION, true, 0.2, PPEGlow.L_23_NVG, PPOperators.SET);
            // Film grain
            SetTargetValueFloat(PostProcessEffectType.FilmGrain, PPEFilmGrain.PARAM_SHARPNESS, false, 6.0, PPEFilmGrain.L_1_NVG, PPOperators.SET);
            SetTargetValueFloat(PostProcessEffectType.FilmGrain, PPEFilmGrain.PARAM_GRAINSIZE, false, 0.5, PPEFilmGrain.L_2_NVG, PPOperators.SET);
            // Zero vignette
            SetTargetValueFloat(PostProcessEffectType.Glow, PPEGlow.PARAM_VIGNETTE, false, 0.0, PPEGlow.L_25_SHOCK, PPOperators.SET);
            SetTargetValueFloat(PostProcessEffectType.Glow, PPEGlow.PARAM_VIGNETTE, false, 0.0, PPEGlow.L_23_NVG, PPOperators.SET);
            return;
        }
        super.SetNVMode(mode);
    }
}
*/