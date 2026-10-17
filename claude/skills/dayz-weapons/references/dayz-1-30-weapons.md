# DayZ 1.30 — firearm entity layer

Companion to `dayz-weapons`. First written against 1.30 Experimental (164014, "digest K"); every claim and
`[EXACT]` block below was **re-verified on 1.30.164078 stable** by opening the file under `P:\` (paths
below are relative to `P:\scripts\` unless they start with `P:\DZ`). `[VERIFIED-vanilla]` = read on disk.
`Before 1.30:` lines cite the 1.29 extraction (`O:\DayZ 1.29 Старый распакованный клиент\`, shortened to
`O:\…`). Anim motion stays with `dayz-animation-pipeline`. Labels: `../../_shared/dayz-version-policy.md`.

## W-STAGING1 — scripts without CfgWeapons / `.p3d`

1.30.164078 ships four new firearm scripts plus a bayonet that compile, register IK (packed `.asi` /
`.anm` under `P:\DZ\anims\`), and in two cases have dedicated bone remaps — but there is **no** model
folder for them under `P:\DZ\weapons\firearms\` / `P:\DZ\weapons\pistols\`, and no `CfgWeapons` /
`CfgMagazines` class for them in any `P:\DZ\**\config.cpp` (grep, 2026-10-15). Debinarize a **shipped**
peer instead — SCAR-H for a SCAR-L-shaped rifle, P1/Glock for a pistol, CZ527/Mosin for a bolt gun,
CZ61/PM73 for an open-bolt SMG. (Before 1.30: every vanilla firearm with a script under
`4_world\entities\firearms\` also shipped a debinarizable `.p3d` + `CfgWeapons` class.)

| Script class | Script path | Script parent | CfgWeapons / `.p3d` on 1.30.164078 |
|---|---|---|---|
| `SCARL_Base` | `4_world\entities\firearms\automaticrifle\scarl.c:1` | `SCARH_Base` (not `RifleBoltFree_Base`) | None. Shipped peer: `P:\DZ\weapons\firearms\scarh\scarh.p3d` + `class SCARH_Base: Rifle_Base` (`P:\DZ\weapons\firearms\scarh\config.cpp:26`). |
| `Luger_Base`, `Luger` | `4_world\entities\firearms\pistol\luger.c:1,25` | `Pistol_Base` | None (no `luger` folder under `P:\DZ\weapons\pistols\`). |
| `LeeEnfield_Base` | `4_world\entities\firearms\rifle\leeenfield.c:1` | `BoltActionRifle_ExternalMagazine_Base` | None. |
| `MP18_Base` | `4_world\entities\firearms\smg\mp18.c:1` | `OpenBolt_Base` | None. |
| `P1907_Bayonet` | `4_world\entities\itembase\inventory_base\p1907_bayonet.c:1` | `Inventory_Base` | Script only; no config class found. |

Also script-only on 1.30.164078 (no config class in `P:\DZ`): `AKM_Gold : AKM_Base`
(`4_world\entities\firearms\automaticrifle\akm.c:61`), `Magnum_Gold`, `SawedoffMagnum_Gold`,
`Magnum_Cylinder_Gold` (`4_world\entities\firearms\pistol\magnum.c:402-435`). `Deagle_Gold` **does** have a
config class (`P:\DZ\weapons\pistols\de\config.cpp:236`).

Magazine script stubs exist (`Mag_Luger_8Rnd`, `Mag_LeeEnfield_10Rnd`, `Mag_MP18_Drum32Rnd` in
`4_world\entities\itembase\magazine\magazines.c:65-67`) but `P:\DZ\weapons\attachments\magazine\config.cpp`
has **no** matching `CfgMagazines` classes. Do not put these classnames in `types.xml` or trader lists.

IK / reload `.anm` **are** packed (entity side only records the registration path; motion is anim-skill).
Registrations in `4_world\entities\manbase\dayzplayer\dayzplayercfgbase.c`:

- `Luger_Base` IK: `:395` → `player_main_luger08.asi` + `lugerP08.anm` + `w_LugerP08_states.anm`.
- `MP18_Base` IK: `:432` → `player_main_mp18.asi` + `mp18_IK.anm` + `MP18_states.anm`.
- `SCARL_Base` IK: `:445` — **same** ASI / IK / states as `SCARH_Base` (`:444`: `player_main_SCARH.asi`, `SCAR_ik.anm`, `w_SCAR_states.anm`).
- `LeeEnfield_Base` IK: `:446` → `player_main_leeEnfield.asi` + `enfield.anm` + `w_enfield_states.anm`.
- Magazines: `Mag_Luger_8Rnd` `:580`, `Mag_LeeEnfield_10Rnd` `:599`, `Mag_MP18_Drum32Rnd` `:630`.

Files present: `P:\DZ\anims\workspaces\player\player_main\weapons\player_main_{luger08,mp18,leeenfield,scarh}.asi`,
`P:\DZ\anims\anm\player\reloads\{lugerp08,mp18,leeenfield}\`.

```
// [EXACT] P:\scripts\4_world\entities\firearms\automaticrifle\scarl.c:1
class SCARL_Base : SCARH_Base
{
    
    override RecoilBase SpawnRecoilObject()
    {
        return new SCARLRecoil(this);
    }
	
	override void OnDebugSpawn()
    {
        GameInventory inventory = GetInventory();

        inventory.CreateInInventory( "SCAR_StockBttstck" );
        inventory.CreateInInventory( "M4_T3NRDSOptic" );
        inventory.CreateInInventory( "Battery9V" );

        SpawnAttachedMagazine("Mag_STANAG_30Rnd");
    }
};
```

`SCARH_Base` itself is `RifleBoltLock_Base` (`4_world\entities\firearms\automaticrifle\scarh.c:1`) and
config `SCARH_Base: Rifle_Base` (`P:\DZ\weapons\firearms\scarh\config.cpp:26`). A 5.56 SCAR-L-shaped mod
that wants this family inherits **SCAR-H config + `RifleBoltLock_Base` script**, then overrides
ammo/mags/recoil — there is no SCAR-L body to debinarize on 1.30.164078. Note 1.30 also changed
`SCARH_Base` config: `WeaponLength` 0.8→0.95, `ObstructionDistance` 0.5→0.7, `barrelArmor` 2.4→2.6,
`initSpeedMultiplier` 0.975→0.925 (config-delta; `P:\DZ\weapons\firearms\scarh\config.cpp:37`).

`P1907_Bayonet` is dual-use as attachment + melee tool: `CanPutAsAttachment` refuses the slot when
`suppressorImpro` or `weaponMuzzle` is reserved/occupied (`p1907_bayonet.c:13-26`), and
`OnWasAttached` / `OnWasDetached` call `parent.SetBayonetAttached(...)` when `parent.IsWeapon()`
(`:28-46`). `[DESIGN]` treat it as a bayonet-slot pattern, not as a shipped item in 1.30.164078.

## Pistol FSM — virtual anim-state getters (do not duplicate the FSM)

1.30: the stable-state constructors in `Pistol_Base.InitStateMachine()` take
`GetDischargedPistolAnimState()` / `GetChargedPistolAnimState()` / `GetOpenPistolAnimState()` /
`GetJammedPistolAnimState()`. Override those on the pistol class. The stable-state classes still return
`PistolStableStateID.*` from `GetCurrentStateID()` — they were not rewritten into `PistolDischarged`
helpers. (Before 1.30: the constructors took `PistolAnimState.*` constants — 1.29:
`O:\…\scripts\4_world\entities\firearms\pistol_base.c:203-212`.)

`enum PistolAnimState` (`4_world\entities\firearms\pistol_base.c:2-8`): `DEFAULT=0`, `OPENED_DISCHARGED=1`,
`CLOSED_CHARGED=2`, `JAMMED=3`. Base getters (`:611-629`): discharged → `DEFAULT`, charged →
`CLOSED_CHARGED`, open → `OPENED_DISCHARGED`, jammed → `JAMMED`.

```
// [EXACT] P:\scripts\4_world\entities\firearms\pistol_base.c:199
		// setup state machine
		// basic weapon states
		// open-closed | discharged-charged | nobullet-bullet | nomag-mag
		// regexp: [OC][CDJ][01][01]
		CD00 = new Pistol_CLO_DIS_BU0_MA0(this, NULL, GetDischargedPistolAnimState());
		CC00 = new Pistol_CLO_CHG_BU0_MA0(this, NULL, GetChargedPistolAnimState());
		CC10 = new Pistol_CLO_CHG_BU1_MA0(this, NULL, GetChargedPistolAnimState());
		CJF0 = new Pistol_CLO_JAM_BU1_MA0(this, NULL, GetJammedPistolAnimState());
		OD00 = new Pistol_OPE_DIS_BU0_MA0(this, NULL, GetOpenPistolAnimState());
		CD01 = new Pistol_CLO_DIS_BU0_MA1(this, NULL, GetDischargedPistolAnimState());
		CC01 = new Pistol_CLO_CHG_BU0_MA1(this, NULL, GetChargedPistolAnimState());
		CC11 = new Pistol_CLO_CHG_BU1_MA1(this, NULL, GetChargedPistolAnimState());
		CJF1 = new Pistol_CLO_JAM_BU1_MA1(this, NULL, GetJammedPistolAnimState());
		OD01 = new Pistol_OPE_DIS_BU0_MA1(this, NULL, GetOpenPistolAnimState());
```

Luger (toggle-lock) overrides **three** getters — not discharged, and not magic integers 1/2/3/4:

```
// [EXACT] P:\scripts\4_world\entities\firearms\pistol\luger.c:8
	override int GetChargedPistolAnimState()
	{
		return PistolAnimState.DEFAULT;
	}

	override int GetOpenPistolAnimState()
	{
		return PistolAnimState.OPENED_DISCHARGED;
	}
	
	override int GetJammedPistolAnimState()
	{
		return PistolAnimState.CLOSED_CHARGED;
	}
```

`[DESIGN]` a modded pistol with a non-Colt slide maps custom `PistolAnimState` (or a parallel enum the
weapon-states `.anm` understands) through these getters. Do not fork `Pistol_CLO_*` state classes unless
the FSM graph itself must change.

## OpenBolt magazine detach

1.30: `OpenBolt_Base.SetActions()` also `AddAction(FirearmActionDetachMagazine)`. (Before 1.30: it only
removed `FirearmActionLoadBullet` + `FirearmActionLoadBulletQuick` — 1.29:
`O:\…\scripts\4_world\entities\firearms\openbolt_base.c:382-388`.)

```
// [EXACT] P:\scripts\4_world\entities\firearms\openbolt_base.c:382
	override void SetActions()
	{
		super.SetActions();
		
		RemoveAction(FirearmActionLoadBullet);
		RemoveAction(FirearmActionLoadBulletQuick); // Easy reload
		AddAction(FirearmActionDetachMagazine);
	}
```

`OpenBolt_Base extends Rifle_Base` at `openbolt_base.c:122`. MP18 inherits this. Do not invent a
bolt-phase workaround to allow mag detach on open-bolt SMGs — the detach action is registered on the base.

`FirearmActionDetachMagazine` lives at
`4_world\classes\useractionscomponent\actions\weapons\firearmactiondetachmagazine.c:151`
(`ActionSequentialBase`, already so in 1.29). 1.30 removed its empty explicit constructor (1.29: `:149`)
and added `CanBePerformedFromInventory()` returning `true` (`:262-265`). The legacy class
`FirearmActionDetachMagazine_Old` is `[Obsolete("1.30: Use class FirearmActionDetachMagazine instead")]`
(`:13-14`).

Attach is stricter: `FirearmActionAttachMagazine` `ActionCondition` now requires
`weapon.TestAttachMagazine(weapon.GetCurrentMuzzle(), mag, false, true)`
(`firearmactionattachmagazine.c:256`). A 1.29 override of `TestAttachMagazine` that returned true too
early will now block the reload animation. `[DESIGN]`, not tested in-game here.

## Weapon_Base — mag detach validates FSM; `EEFired` is client FX

```
// [EXACT] P:\scripts\4_world\entities\firearms\weapon_base.c:1115
	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);

		GetPropertyModifierObject().UpdateModifiers();

		if(item.IsMagazine()) {ValidateAndRepair();}
	}
```

(Before 1.30: `EEItemDetached` stopped after `UpdateModifiers()` — 1.29:
`O:\…\scripts\4_world\entities\firearms\weapon_base.c:1123-1128`.)

`EEFired(int muzzleType, int mode, string ammoType)` is declared on `EntityAI` in 1.30
(`3_game\entities\entityai.c:4927`, empty body) and `Weapon_Base` overrides it (`weapon_base.c:341`),
wrapping muzzle-flash / overheat in `if ( !g_Game.IsHeadlessOrDedicatedServer() )` (`:345`). Before 1.30
`Weapon_Base` declared it itself without `override` (1.29: `weapon_base.c:341`). A `modded class
Weapon_Base` / subclass override keeps compiling. `[UNVERIFIED]` whether a non-weapon `EntityAI`
subclass that declared its own `EEFired(int,int,string)` without `override` now fails to compile —
add `override` when you port one.

Magazine drop orientation when a swap cannot fit inventory: 1.29 passed an uninitialized `float dir[4]`
into `SetGroundEx` (1.29: `O:\…\scripts\4_world\classes\weapons\weaponmanager.c:1362-1365`). 1.30 fills
it from the player yaw (`m_player` here is `WeaponManager.m_player`, `weaponmanager.c:5`):

```
// [EXACT] P:\scripts\4_world\classes\weapons\weaponmanager.c:1364
					vector rotationMatrix[3];
					float dir[4];
					Math3D.YawPitchRollMatrix(m_player.GetOrientation(), rotationMatrix);
					Math3D.MatrixToQuat(rotationMatrix, dir);
					new_il.SetGroundEx( old_mag,  m_player.GetPosition(), dir);
```

Cartridge pull in the FSM helper (1.30: `LocalAcquireCartridgeEx`; before 1.30: `LocalAcquireCartridge`).
The `*Ex` methods (`4_world\entities\itembase\magazine\magazine.c:180-199`) call the native
`LocalAcquireCartridge` / `ServerAcquireCartridge` / `ServerStoreCartridge` and then the new
`protected void UpdateAnimation()` (`:134`), which the MP18 drum overrides. The natives still exist
(`magazine.c:79-88`), so old calls compile but skip the mag animation update.

```
// [EXACT] P:\scripts\4_world\entities\firearms\fsm\weapon_utils.c:7
		float damage;
		string type;
		if (mag && mag.LocalAcquireCartridgeEx(damage, type))
```

## Recoil profiles (script `RecoilBase`, not config `recoil=`)

All four new recoils subclass `RecoilBase` and fill `m_HandsCurvePoints` + mouse/cam offsets — they do
**not** use a `m_HandsOffsetRelativePoseModel` string. Path: `4_world\classes\recoilbase\recoils\`.

| Class | Hands curve peak Y (point_1/2) | `m_MouseOffsetDistance` | `m_CamOffsetDistance` | Notes |
|---|---|---|---|---|
| `SCARLRecoil` | 1.3 / 1.6 | 1.5 | 0.01 | Curve values are identical to `SCARHRecoil.c` on 1.30.164078. |
| `LugerRecoil` | 3 / 4 | 0.95 | 0.02 | Sharp vertical hands spline. |
| `LeeEnfieldRecoil` | 1.75 / 2 | 1.6 | 0.02 | Short relative times (`m_HandsOffsetRelativeTime = 0.125`). |
| `MP18Recoil` | 0.9 / 0.85 | 0.9 | 0.005 | Lowest cam kick of the four. |

```
// [EXACT] P:\scripts\4_world\classes\recoilbase\recoils\leeenfieldrecoil.c:18
		m_HandsOffsetRelativeTime = 0.125;
		
		m_MouseOffsetRangeMin = 80;//in degrees min
		m_MouseOffsetRangeMax = 100;//in degrees max
		m_MouseOffsetDistance = 1.6;//how far should the mouse travel
		m_MouseOffsetRelativeTime = 0.0625;//[0..1] a time it takes to move the mouse the required distance relative to the reload time of the weapon(firing mode)
	
		m_CamOffsetDistance = 0.02;
```

Weapons spawn them via `SpawnRecoilObject()` (`scarl.c:4-7`, `luger.c:3-6`, `leeenfield.c:3-6`,
`mp18.c:7-10`). Config `recoil = "recoil_fal"` on `SCARH_Base` (`P:\DZ\weapons\firearms\scarh\config.cpp:94`)
is a **different** channel (CfgRecoils) from this script object.

## Ammo / explosion FX API (breaking)

1.30: `AmmoTypesAPI.GetExplosionParticleID` is gone. `ExplosiveEffectData` +
`GetExplosiveEffectData(ammoName, surfaceName)` return particle **and** optional sound set. Signature is
**two strings in, object out** — not `(ammoType, outData)`. (Before 1.30:
`static int GetExplosionParticleID(string ammoName, string surfaceName)` — 1.29:
`O:\…\scripts\3_game\global\ammotypes.c:57`, called from `dayzgame.c:3534` only when
`source.ShootsExplosiveAmmo()`.)

```
// [EXACT] P:\scripts\3_game\global\ammotypes.c:1
class ExplosiveEffectData
{
	int m_ParticleID;
	string m_SoundSetName;
	
	void ExplosiveEffectData(int particleID, string soundSetName = "")
	{
		m_ParticleID = particleID;
		m_SoundSetName = soundSetName;
	}
}
```

```
// [EXACT] P:\scripts\3_game\global\ammotypes.c:90
	static ExplosiveEffectData GetExplosiveEffectData(string ammoName, string surfaceName)
	{
		ExplosiveEffectData result;
		
		#ifdef NO_GUI
		return result;
		#endif
```

So on a `NO_GUI` build (dedicated server) the result is `null` — null-check it before reading fields.
Resolution order (`ammotypes.c:98-130`): exact `(ammo, surface)` entry → the ammo's `"default"` entry →
`CfgAmmo <ammoName> particle` (resolved with `ParticleList.GetParticleIDByName`) cached as `"default"`.
Register your own per-surface effect with `AmmoTypesAPI.AddExplosionParticleEffect(string ammoName,
string surfaceName, int particleID, string soundSetName = "")` (`:78`; 1.30 added the 4th parameter).
The vanilla table is built in `AmmoTypesAPI.Init()` (`:45-76`), called from `MissionBase`
(`5_mission\mission\missionbase.c:40`); it maps `Hit_Snow` / `Hit_Ice` / `Hit_Desert_Sand` / `Hit_Sand`
for `Explosion_40mm_Ammo`, `LandMineExplosion_Ammo`, `RGD5Grenade_Ammo`, `M67Grenade_Ammo`; the RGD5/M67
sand entries pass `"ExplosionDustEffect_SoundSet"` (`:66-72`).

Vanilla callers:

```
// [EXACT] P:\scripts\3_game\ammoeffects.c:49
		string surfaceType;
		g_Game.SurfaceGetType3D(pos[0], pos[1], pos[2], surfaceType);
		ExplosiveEffectData explosiveEffectData = AmmoTypesAPI.GetExplosiveEffectData(ammoType, surfaceType);
		//int particleID = GetAmmoParticleID(ammoType);
		
		if (ParticleList.IsValidId(explosiveEffectData.m_ParticleID))
		{
			return ParticleManager.GetInstance().PlayInWorld(explosiveEffectData.m_ParticleID, pos) != null;
		}
```

`DayZGame.ExplosionEffects` (`3_game\dayzgame.c:3634-3639`, inside `#ifndef SERVER` / `#ifndef NO_GUI`)
plays `explosiveEffectData.m_ParticleID` and `m_SoundSetName` via `SetStartSoundParams` for **every**
explosion source — the 1.29 `ShootsExplosiveAmmo()` gate is gone from that path (it remains on the
projectile-impact path, `dayzgame.c:3773`). `Ammo_40mm_Explosive.ShootsExplosiveAmmo()` was removed; the
base `Object.ShootsExplosiveAmmo()` (`3_game\entities\object.c:669`) and `M79` override remain.

`MagazineTypeToAmmoType` still exists at `ammotypes.c:26` (1.29: `:14`) because `ExplosiveEffectData`
was prepended. Vanilla uses it in `weapon_base.c:840` (1.29: `:848`).

`ExplosivesBase` (`4_world\entities\explosivesbase.c`): `[Obsolete("1.30: Handled in AmmoTypes instead")]`
on `m_TypeToSurfaceParticleIDMap` / `m_SurfaceToExplosionEffectsData` (`:40-43`),
`SetParticleExplosion(int particle)` (`:306-310`), `InitSpecificsExplosionEffectForSurface` (`:380-384`),
both `AddExplosionEffectForSurface` overloads — `(ExplosionEffectsData)` at `:386-387` and
`(string, int)` at `:432-433` — `GetParticleExplosionID(string)` (`:394`) and the new-and-already-obsolete
`GetExplosionSoundSetName(string)` (`:419`). More important than the attributes: 1.30
`ExplosivesBase.OnExplosionEffects` only calls `super` and `CreateLight()` (`:66-70`) — it **no longer
plays** `m_ParticleExplosionId` (Before 1.30 it did, via `GetParticleExplosionID(surface)` +
`PlayOnObject`). Vanilla `ClaymoreMine` / `Plastic_Explosive` have their `SetParticleExplosion` calls
commented out (`4_world\entities\explosivesbase\claymoremine.c:18`, `plastic_explosive.c:15`).
Put `particle = "..."` on the `CfgAmmo` class your explosive uses as `ammoType`; do not keep a
script-side surface→particle map. (Static reading of `P:\`; not tested in-game here.)

## CfgAmmo deltas a weapon/explosive mod must know

From `dayz-130-migration/references/config-delta-1.29-to-1.30.md`, each spot-checked in `P:\DZ`:

- **Renamed:** `CfgAmmo LandMineExplosion` → `LandMineExplosion_Ammo` (`P:\DZ\gear\traps\config.cpp:17`;
  1.29: `O:\…\DZ\gear\traps\config.cpp:17`). `LandMineExplosion_CarWheel` now inherits it (`:75`) and the
  landmine trap uses `ammoType="LandMineExplosion_Ammo"` (`:257`). A mod config `class X: LandMineExplosion`,
  `ammoType="LandMineExplosion"` or a script `Explode(..., "LandMineExplosion")` must be renamed.
- **New `particle=` keys** on nine explosive ammo classes in `P:\DZ\weapons\explosives\config.cpp`:
  `ClaymoreMine_Ammo` (`explosion_claymore_01`), `Explosion_40mm_Ammo` (`explosion_landmine_01`),
  `FlashGrenade_Ammo` (`explosion_M84_01`), `GasCanister_Ammo` (`explosion_RGD5_01`), `M67Grenade_Ammo`
  (`explosion_M67_01`), `Plastic_Explosive_Ammo` (`explosion_plastic_01`), `RGD5Grenade_Ammo`
  (`explosion_RGD5_01`), and empty `""` on `Fireworks_Ammo` / `LandFuelFeed_Ammo`. A modded ammo that
  **inherits** one of these gets the particle; one rooted at `DefaultAmmo` needs its own `particle=` or it
  explodes without a particle on 1.30 (see above).
- **New surface hit-sound keys** on `Bullet_Base` (`P:\DZ\weapons\projectiles\config.cpp:16`):
  `hitBrick[]` (`:1205`), `hitSand[]` (`:1224`), `hitGrass[]` (`:1243`), `hitMudDry[]` (`:1262`) plus their
  `soundBrick1-8` / `soundSand1-8` / `soundGrass1-8` / `soundMudDry1-8` arrays (samples under
  `DZ\sounds_takistan\Weapons\Hits\`). ~160 melee ammo classes (`P:\DZ\weapons\melee\config.cpp`, e.g.
  `MeleeDamage: Bullet_Base` `:19`) gained `hitBricks[]` / `hitMudDry[]`. Bullets inheriting `Bullet_Base`
  get them for free; `[UNVERIFIED]` what a bullet rooted elsewhere plays on those surfaces.
- **Explosive items declare `ammoType=`:** `ClaymoreMine`, `FlashGrenade`, `Grenade_ChemGas`, `M67Grenade`,
  `Plastic_Explosive`, `RGD5Grenade` gained a CfgVehicles `ammoType` key in 1.30
  (`P:\DZ\weapons\explosives\config.cpp`). It is the default `Object.Explode(damageType, ammoType = "")`
  falls back to (`3_game\entities\object.c:144-150`); script `SetAmmoType(...)` on `ExplosivesBase`
  (`explosivesbase.c:295-304`) is still what `InitiateExplosion` uses.
- **Value changes:** `Bullet_Flare` damage (Health 10→30, Shock 0→10), `MeleeWolf` `armorDamage=1.2`,
  `Rocket_RPG7_Base` / `Rocket_LAW_Base` / `GrenadeM4` `proxyShape` moved from `\dz\weapons\projectiles\`
  to `\dz\weapons\ammunition\rocket_rpg7_inflight.p3d`. A mod that hard-codes the old proxy path must
  update it.

## MP18 drum magazine (script only)

`Mag_MP18_Drum32Rnd` is not an empty stub. It drives AnimationSource `"drum"`, stops the anim at 12
rounds (`m_AmmoOffset = 12`), plays `wpn_mp18_drum_mag_SoundSet` at memory point `"drum_sound"`, and
persists `m_LastAnimPhase` from save version `>= 144` (`4_world\entities\itembase\magazine\magazines.c:67-121`;
144 = `GAME_STORAGE_VERSION` on 1.30). Still no `CfgMagazines` entry on 1.30.164078 — W-STAGING1 applies.

## Changelog T189758 vs scripts

`[CHANGELOG 1.30]` the official 1.30 modding notes claim `enum SoundHitType` and
`SurfaceInfo.GetSoundHitType()`. A grep of `P:\scripts` (1.30.164078) finds **neither**.
`3_game\surfaceinfo.c:8-70` adds vehicle/dust/cloud/ceiling particle and sound-set proto getters, not a
hit type. A weapon mod MUST NOT call `GetSoundHitType()` in Enforce on this build (compile fail).
Surfaces/impact particles belong to `dayz-surfaces` if that skill is patched separately.

## Out of this skill (digest K §5)

- `GunCase` / `GunCaseKeys` / color recipes → `dayz-containers`.
- PoliceBaton (1.30 config-delta, `P:\DZ\weapons\melee\blunt\config.cpp`): melee `ammo` `MeleeBlunt` /
  `MeleeBlunt_Heavy` → `MeleeBluntLight_2` for all modes, range 1.6→1.8, `inventorySlot[]`
  `{"Shoulder","Melee"}` → `{"Backpack_1"}`, weight 540→680. (The Exp digest's "health 18→13 / 40→30"
  is not reflected in the stable config delta — do not quote it.) `hitBricks` / `hitMudDry` melee sound
  arrays → `dayz-melee`.
- `Hit_Mud_Brick` / `Hit_Desert_Sand` registration → `dayz-surfaces`.
