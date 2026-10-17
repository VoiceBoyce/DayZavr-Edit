# Effects & Lights

Seaplane water spray effects, custom navigation lights and per-aircraft headlights.

_Extracted from `dayz-aviation/SKILL.md` 2026-07-07 (F3). Target: DayZ 1.30 stable (1.30.164078); labels `../../_shared/dayz-version-policy.md`._

### Custom nav lights

```cpp
class LlamaPlaneScript_NavLightLeft  extends PointLightBase { /* red wing tip */ };
class LlamaPlaneScript_NavLightRight extends PointLightBase { /* green wing tip */ };
class LlamaPlaneScript_NavLightTail  extends PointLightBase { /* white tail */ };

// In LlamaPlaneScript:
protected void CreateNavLights()
{
    vector lightPos = m_CachedLightWingLeft;  // from memory point light_wing_left
    m_NavLightLeft = LlamaPlaneScript_NavLightLeft.Cast(ScriptedLightBase.CreateLight(LlamaPlaneScript_NavLightLeft, lightPos));
    m_NavLightLeft.AttachOnObject(this, lightPos);
    // ... right, tail
}

protected void SetNavLightsVisible(bool visible)
{
    if (m_NavLightLeft) m_NavLightLeft.SetEnabled(visible);
    // ...
}
```

Per-aircraft headlights — **Before 1.30** (LM_Planes, 1.29 community pattern; on 1.30 `CreateFrontLight` is never called, so this class is dead code — use the profile form below):

```cpp
class LM_TigermothFrontLight extends CarLightBase
{
    void LM_TigermothFrontLight()
    {
        m_SegregatedBrightness  = 6;  m_SegregatedRadius  = 60; m_SegregatedAngle  = 80;
        m_SegregatedColorRGB    = Vector(0.9, 0.9, 1);
        m_AggregatedBrightness  = 10; m_AggregatedRadius  = 85; m_AggregatedAngle  = 90;
        m_AggregatedColorRGB    = Vector(0.9, 0.9, 1);
        FadeIn(0.3); SetFadeOutTime(0.25); SegregateLight();
    }
}
```

**1.30:** vehicle headlights are `VehicleLightBase` configured by a `VehicleLightProfileBase` subclass registered in the
aircraft constructor with `m_LightsComponent.RegisterLight("Front", new VehicleLightData(new LM_TigermothLightProfileFront()))`
(vanilla pattern `P:\scripts\4_world\entities\vehicles\inheritedcars\offroad_02.c:21-22`; profile fields
`m_SegregatedBrightness/Radius/Angle/ColorRGB`, `m_AggregatedBrightness/...`, `m_FadeInTime`/`m_FadeOutTime`,
`vehiclelightprofiles\vehiclelightprofilebase.c:2-29`, subclass `VehicleLightProfileFront` at `:31`). The same numbers as
the `CarLightBase` above move into the profile constructor. Vanilla `CivilianSedanFrontLight` is
`[Obsolete("use VehicleLightBase and VehicleLightProfileBase instead")]`
(`P:\scripts\4_world\entities\scriptedlightbase\spotlightbase\carlightbase\civiliansedanfrontlight.c:1`). Nav lights that
subclass `PointLightBase` are unchanged. Full migration: `dayz-1-30-aviation.md`.

## Effects

### Seaplane water effects

Three emitter classes per position (Front=9 max emitters, Back=5, Side=3) extending `EffectSeaplaneWaterBase : EffectParticle`. Speed states (Slow/Medium/Fast) via `enum ESeaplaneSpeed`. `UpdateSpeedState(speed)` enables/disables emitters dynamically. `Update()` lerps intensity between states. Memory point positions updated via `m_PosUpdateTimer` + `CoordToParent`. Reusable for boats, amphibious vehicles, hovercraft.
