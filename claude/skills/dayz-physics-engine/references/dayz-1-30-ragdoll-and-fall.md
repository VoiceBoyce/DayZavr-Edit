# DayZ 1.30 — ragdoll data-driven and fall damage

> Target: DayZ 1.30 stable (1.30.164078). Version labels: ../../_shared/dayz-version-policy.md. Porting a ≤1.29 mod: dayz-130-migration skill.

Lane P04 overflow for `dayz-physics-engine`. SKILL.md keeps the 90% working set; this file holds
1.30 physics-engine detail that would blow the 500-line cap. Character bind path (`ragdoll =` on
`SurvivorBase`) is `dayz-characters`. Animation-graph / surrender ASI is `dayz-animation-pipeline`.

Citations are `P:\…` (1.30.164078) unless marked `1.29:` (`O:\DayZ 1.29 Старый распакованный клиент\…`). This file was
first written against the 1.30 Exp 164014 extract; every `[EXACT]` block below was re-read on stable and holds
unless a correction says otherwise. `[UNVERIFIED]` = no script call-site or no in-game test.

## 1. `.ragdoll` / `RagdollDef`

1.30: vanilla ships a text `RagdollDef` (`P:\DZ\characters\bodies\human.ragdoll`, 202 lines) and binds it to the
player in config: `class SurvivorBase … class enfAnimSys { … ragdoll="DZ/characters/bodies/human.ragdoll"; }`
(`P:\DZ\characters\data\config.cpp:153-161`, key at `:160`). Before 1.30: no `.ragdoll` file and no `ragdoll`
key (1.29: `O:\…\DZ\characters\data\config.cpp:153-160`); player ragdoll collision/mass lived in native code and
scripts only saw `PhysicsSetRagdoll`.

[CHANGELOG 1.30] Workbench added a ragdoll editor for `.ragdoll` files.

There is **no** `LoadRagdollFile` (or similar) in `P:\scripts`. The config key is the only binding vanilla shows;
[UNVERIFIED] whether a custom creature picks up a `.ragdoll` without it. `dayz-characters` documents
the `SurvivorBase` config bind.

```c
// [EXACT] P:\DZ\characters\bodies\human.ragdoll:1
RagdollDef {
 Object "{D470A4E7581CCE23}DZ/anims/workspaces/player/Models/player_m_editorpreview.xob"
 {
  RagdollBone pelvis {
   Mass 17
   Offset -0.008 -0.011 0
   Geometries {
    PhysicsCapsuleGeometry Spine1 {
     SurfaceProperties "DZ/data/data/penetration/flesh.bisurf"
     Offset 0 0.914 0.015
     Orientation 0 0 90
     Radius 0.1
     Height 0.2
    }
    PhysicsCapsuleGeometry Spine2 {
     Offset 0 1.061 0
     Radius 0.1
     Height 0.1
    }
   }
   {
```

Head is a sphere, not a capsule:

```c
// [EXACT] P:\DZ\characters\bodies\human.ragdoll:38
      RagdollBone head {
       Mass 6.1
       Offset 0.094 0 0
       Geometries {
        PhysicsSphereGeometry Head {
         SurfaceProperties "DZ/data/data/penetration/flesh.bisurf"
         Offset 0 1.672 -0.009
         Radius 0.1
        }
       }
       JointMinLimits -70 -35 -40
       JointMaxLimits 70 35 45
       JointStiffness 0.8
       JointDamper 0.3
      }
```

[DESIGN] Author a custom humanoid ragdoll by copying this tree, matching `RagdollBone` names to
the skeleton, and keeping `SurfaceProperties` on a penetration `.bisurf`. Joint limits are degrees
relative to the parent. Masses in vanilla `human.ragdoll` (kg): pelvis 17 (`:5`), spine3 15 (`:23`),
head 6.1 (`:39`), leftarm 2.1 (`:54`), leftforearm 1.7 (`:69`), rightarm 2.1 (`:85`),
rightforearm 1.7 (`:100`), leftupleg 7.5 (`:118`), leftleg 6.6 (`:134`), rightupleg 7.5 (`:159`),
rightleg 6.6 (`:175`). Sum = 73.9 kg (corrected on 1.30.164078: this line used to say 73.8). (The 2026-06-11 live-player `dBodyGetMass` reading of 87.5 kg
is a different body — the CCT/player body, not this ragdoll def.)

## 2. `PhysicsSetSimpleDeath` vs `PhysicsSetRagdoll`

(Before 1.30: `PhysicsSetRagdoll` carried a comment about the `RAGDOLL` interaction layers (1.29: `scripts\3_game\human.c:1417-1418`)
and the non-command death branch called `PhysicsSetRagdoll(true)` (1.29: `scripts\4_world\entities\dayzplayerimplement.c:736`).
`PhysicsSetSimpleDeath` and `PhysicsIsRagdoll` did not exist.)

```c
// [EXACT] P:\scripts\3_game\human.c:1448
	//! Old system for handling player death state, was previously handled by PhysicsSetRagdoll
	proto native	void		PhysicsSetSimpleDeath(bool pEnable);
	
	proto native	void		PhysicsSetRagdoll(bool pEnable);
	proto native	bool		PhysicsIsRagdoll();
```

`PhysicsIsFalling` also documents ragdoll motion:

```c
// [EXACT] P:\scripts\3_game\human.c:1426
	//! returns true if physics controller is falling 
	//! returns true if the ragdoll is moving greater than 1m/s
	proto native 	bool		PhysicsIsFalling(bool pValidate);
```

Non-command death branch (1.30):

```c
// [EXACT] P:\scripts\4_world\entities\dayzplayerimplement.c:743
			else
			{
				PhysicsSetSimpleDeath(true);
			}
```

Unconscious ragdoll query lives on the command, not only on `Human`:

```c
// [EXACT] P:\scripts\3_game\human.c:644
	proto native void 	WakeUp(int targetStance = -1);
	proto native bool	IsWakingUp();

	proto native bool   IsRagdoll();
```

Vanilla clears vehicle re-attach while that ragdoll is active:

```c
// [EXACT] P:\scripts\4_world\entities\manbase\playerbase.c:3346
			if (hcu && hcu.IsRagdoll())
			{
				// Don't attach us back to the vehicle once we wake up
				m_TransportCache = null;
			}
```

## 3. Wake-up while falling

(Before 1.30: only `m_UnconsciousTime > 2`, then `hcu.WakeUp` (1.29: `playerbase.c:3184-3192`).
The 2026-06-11 0.5 s wake test, measured on 1.29, buried the player 40 m under terrain.)

```c
// [EXACT] P:\scripts\4_world\entities\manbase\playerbase.c:3419
						//! protection for a player being broken when attempting to wake them up too early into unconsciousness
						if (m_UnconsciousTime > 2)
						{
							// If the player is falling then prevent waking up
							if (false == PhysicsIsFalling(false))
							{
								int wakeUpStance = DayZPlayerConstants.STANCEIDX_PRONE;
	
								//! Don't set the stance if we are swimming or in a vehicle, stance change animation could play
								if (m_Swimming.m_bWasSwimming || m_LastCommandBeforeUnconscious == DayZPlayerConstants.COMMANDID_VEHICLE)
									wakeUpStance = -1;
	
								hcu.WakeUp(wakeUpStance);
```

[DESIGN] Custom uncon/ragdoll get-up MUST keep both guards. `PhysicsIsFalling(false)` is true while
the ragdoll moves greater than 1 m/s (`P:\scripts\3_game\human.c:1426-1428`), so a still-tumbling ragdoll will not wake.

## 4. Fall damage `CurveExp`

1.30 values below. Before 1.30: linear `Math.InverseLerp` with `HEALTH_HEIGHT_LOW = 5`, `HEALTH_HEIGHT_HIGH = 14`,
`SHOCK_HEIGHT_LOW = 3`, `SHOCK_HEIGHT_HIGH = 12`, `BROKENLEGS_HEIGHT_LOW = 5`,
`BROKENLEGS_HEIGHT_HIGH = 9` (1.29: `scripts\4_world\entities\dayzplayerimplementfalldamage.c:26-32`;
`HandleFallDamage` used `Math.InverseLerp` at `:97`, and `Randomize` had no `pValue == 1` early return, `:151-153`).

```c
// [EXACT] P:\scripts\4_world\entities\dayzplayerimplementfalldamage.c:26
	const float		HEALTH_HEIGHT_LOW 		= 2;
	const float		HEALTH_HEIGHT_HIGH 		= 12;

	private const float		SHOCK_HEIGHT_LOW 		= 0;
	private const float		SHOCK_HEIGHT_HIGH 		= 8;
	private const float		BROKENLEGS_HEIGHT_LOW 	= 3;
	private const float		BROKENLEGS_HEIGHT_HIGH 	= 8;
```

```c
// [EXACT] P:\scripts\4_world\entities\dayzplayerimplementfalldamage.c:92
	float CurveExp(float low, float high, float value, float power)
	{
	    float t = Math.InverseLerp(low, high, value);
	    t = Math.Clamp(t, 0, 1);
	    return Math.Pow(t, power);
	}
```

Powers used in `HandleFallDamage` (`P:\scripts\4_world\entities\dayzplayerimplementfalldamage.c:104-139`): health 2.0, shock 2.0, broken legs 2.5, bleeding
hands/feet 1.8, bleeding legs 2.0, gloves 2.0, shoes 2.2, pants 2.0, bleeding chance 2.0.

```c
// [EXACT] P:\scripts\4_world\entities\dayzplayerimplementfalldamage.c:160
	private float Randomize(int pType, float pValue)
	{
		if (pValue == 1)
			return pValue;
```

Ammo names unchanged: `FallDamageHealth` / `FallDamageShock` (`:20-21`), applied via
`ProcessDirectDamage` at `:155-156`.

[DESIGN] A 1.29 `modded` class that re-lists `HEALTH_HEIGHT_LOW = 5` and `InverseLerp` will compile
and then silently restore the old (weaker) curve. Replace the body with `CurveExp` or do not
override the constants.

## 5. Action cursor (View Geometry) + liquid surfaces

Truth #2 is still true. Line drift plus the liquid-surface target:

- `RaycastRVProxy`: `P:\scripts\4_world\classes\useractionscomponent\actiontargets.c:255` (1.29: `:214-219`; `RaycastRVParams` still defaults
  `ObjIntersectView` at `P:\scripts\3_game\global\dayzphysics.c:88`).
- Ground fallback `RayCastBullet` + `ROADWAY|TERRAIN|WATERLAYER`: `:383-384`.
- Liquid surface with no object (new in 1.30): utility 0.01 at `:532-534`. `ActionTarget` ctor gained a trailing
  `string surfaceName = ""` (`:125-138`; 1.29 had five parameters) and `GetSurfaceLiquidType()` (`:184-187`). Five-argument
  calls still compile; a subclass constructor override must take the new parameter.

```c
// [EXACT] P:\scripts\4_world\classes\useractionscomponent\actiontargets.c:255
		if (DayZPhysics.RaycastRVProxy(rayInput, results))
```

```c
// [EXACT] P:\scripts\4_world\classes\useractionscomponent\actiontargets.c:383
			PhxInteractionLayers collisionLayerMask = PhxInteractionLayers.ROADWAY|PhxInteractionLayers.TERRAIN|PhxInteractionLayers.WATERLAYER;
			DayZPhysics.RayCastBullet(m_RayStart,m_RayEnd,collisionLayerMask,null,hitObject,contact_pos,hitNormal,hitFraction);
```

```c
// [EXACT] P:\scripts\4_world\classes\useractionscomponent\actiontargets.c:532
		//! surfaces with liquid source
		if (m_SurfaceInfo && m_SurfaceInfo.GetLiquidType() != LIQUID_NONE)
			return 0.01;
```

## 6. Related 1.30 notes (physics-adjacent, not this skill's job)

- [CHANGELOG 1.30] `disableSimulation` on house-based entities — `dayz-model-pipeline`. Script API
  `Entity.DisableSimulation` / `GetIsSimulationDisabled` (`P:\scripts\3_game\entities\entity.c:3-6`).
- [CHANGELOG 1.30] `SurfaceInfo.GetSoundHitType` / `SoundHitType` — listed in the official changelog but **not present**
  in `P:\scripts` on 1.30.164078 (0 hits; `3_game\surfaceinfo.c` ends its getters at `:70`). Do not code against it.
- Motorbike branch inside `RegisterTransportHit` (`P:\scripts\3_game\entities\entityai.c:4166-4184`; it sat at `:4143-4160`
  in the Exp build) — see `dano-transporthit.md`. Vehicle authoring is `dayz-vehicles` / `dayz-motorbikes`.
- `DayZPlayer.IsFirstRenderFrame` (`P:\scripts\3_game\dayzplayer.c:1186-1189`) — catchup skip, not rigid-body API.
