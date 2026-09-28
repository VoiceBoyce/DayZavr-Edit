/*
modded class GameplayEffectWidgets
{
    // Register custom FOG occluder widget layouts for future use
    override protected void InitLayouts()
    {
        super.InitLayouts();
        RegisterLayouts("FOG_MOD/Scripts/GUI/Layouts/CameraEffects.layout", {
            FOG_EFFECT_NVG_DUAL_OCCLUDER,
            FOG_EFFECT_NVG_GPNVG_OCCLUDER
        });
    }

    override protected void InitWidgetSets()
    {
        super.InitWidgetSets();
        InitWidgetSet(FOG_EFFECT_NVG_DUAL_OCCLUDER, false, FOG_EFFECT_NVG_DUAL_OCCLUDER);
        InitWidgetSet(FOG_EFFECT_NVG_GPNVG_OCCLUDER, false, FOG_EFFECT_NVG_GPNVG_OCCLUDER);
        UpdateVisibility();
    }
}
*/