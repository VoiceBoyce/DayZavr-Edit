# CfgWeapons config contract

Authored 2026-07-07 (F4). `[VERIFIED in-game]` = a real in-game test in a project handoff;
`[VERIFIED-vanilla]` = read off disk; `[UNVERIFIED]` = inferred. Owns config inheritance, chamber / mag /
muzzle / optics slots, fire modes, recoil, dispersion, and jam CONFIG. The MOTION of the mechanism
(bolt/reload/fire/jam-unjam anims) is delegated → `dayz-animation-pipeline`.

## Inheritance chain — NOT a straight line `[VERIFIED-vanilla]`

Script-side firearm hierarchy (`scripts\4_world\entities\firearms\`). **Cite-then-verify caught a spec
error here:** `RifleBoltFree_Base` descends from `Rifle_Base`, but `BoltActionRifle_Base` and
`Pistol_Base` extend `Weapon_Base` **directly**, not `Rifle_Base`.

```
Weapon (native)
 └─ Weapon_Base            (weapon_base.c:38)
     ├─ Rifle_Base         (rifle_base.c:10  — extends Weapon_Base)
     │   └─ RifleBoltFree_Base   (rifleboltfree_base.c:92 — extends Rifle_Base)
     ├─ BoltActionRifle_Base     (boltactionrifle_base.c:5 — extends Weapon_Base DIRECTLY)
     └─ Pistol_Base              (pistol_base.c:149 — extends Weapon_Base DIRECTLY)
```
Verified: `boltactionrifle_base.c:5` `class BoltActionRifle_Base extends Weapon_Base`;
`pistol_base.c:149` `class Pistol_Base extends Weapon_Base`. Do not assume a common `Rifle_Base` parent.

Further branches, `[VERIFIED-vanilla]` on 1.30.164078 (same `Weapon_Base` root; the 1.30 additions'
configs and models are Badlands DLC data under `P:\DZ\weapons_takistan\` — W-STAGING1 in
`dayz-1-30-weapons.md`, corrected 2026-10-21):

- `RifleBoltLock_Base extends Rifle_Base` — `rifleboltlock_base.c:119`. `SCARH_Base : RifleBoltLock_Base`
  (`automaticrifle\scarh.c:1`); `SCARL_Base : SCARH_Base` (`automaticrifle\scarl.c:1`, new in 1.30).
  Config for SCAR-H is `class SCARH_Base: Rifle_Base` (`P:\DZ\weapons\firearms\scarh\config.cpp:26`).
  There is **no** `class SCARL` in any `P:\DZ` config on 1.30.164078.
- `OpenBolt_Base extends Rifle_Base` — `openbolt_base.c:122`. `MP18_Base extends OpenBolt_Base`
  (`smg\mp18.c:1`, new in 1.30, no config).
- `BoltActionRifle_ExternalMagazine_Base : BoltActionRifle_Base` —
  `boltactionrifleexternalmagazine_base.c:112`. `LeeEnfield_Base : BoltActionRifle_ExternalMagazine_Base`
  (`rifle\leeenfield.c:1`, new in 1.30, no config).
- `Luger_Base : Pistol_Base` (`pistol\luger.c:1`, new in 1.30, no config) — pistol anim IDs via
  `Get*PistolAnimState()` (`pistol_base.c:611-629`), not a second FSM.

**`Weapon_Base` owns the FSM + jam state** `[VERIFIED-vanilla]` (`weapon_base.c`): `m_fsm` (`:49`),
`m_isJammed` (`:50`), `m_Charged` (`:54`), `m_WeaponOpen` (`:55`), `m_ChanceToJam` (`:72`),
`m_abilities` (`:48`).

**`RifleBoltFree_Base` builds the state machine** `[VERIFIED-vanilla]` (`rifleboltfree_base.c`):
`InitStateMachine()` (`:106`) with stable states `C00/C10/C11/C01/JF0/JF1` (`:94-99`), regexp
`[CLJ][01][01]` = closed/locked/jammed × nobullet/bullet × nomag/mag (comment `:128-129`), and ability
records for RELOAD (`:109-113`), CHAMBERING (`:115`), MECHANISM (`:117`), UNJAMMING START/END
(`:119-120`), FIRE JAM/NORMAL/DRY (`:122-124`).

### Choosing the config base

Inherit the config base of the vanilla / pack weapon whose ammo / mags / recoil / sounds / anims you
want for free; override only what differs.

- **MK47** `[VERIFIED in-game]`: config `A6_MK47_Base : A6_AKM_Base` + script `RifleBoltFree_Base`
  (`A6_MK47_dev\research\F0-herencia-decision.md:22-30`; `A6_MK47_dev\CLAUDE.md` "Decisiones fijas").
- **SR2M** `[VERIFIED in-game]`: config `A6_SR2M_Base : A6_PP19_Base` (`A6_SR2M\config.cpp:783`) — an SMG
  inheriting the A6 PP19 base, NOT an AK base. The base you pick is per-weapon; don't copy MK47's.

## Config fields `[VERIFIED-vanilla]` — read off the des-rapificado A6 AK config

Source `A6_MK47_dev\research\a6_ak_config\config.cpp` (unRap of `A6_Weapons\...\AK\config.bin`). Line
numbers are the 762×39 AKM block (the one the MK47 inherits from):

| Field | Purpose | Line |
|---|---|---|
| `initSpeedMultiplier` | weapon's factor on muzzle velocity (effective MV = weapon × ammo) | `:364` (also `:33`) |
| `chamberableFrom[]` | ammo types the chamber accepts (`Ammo_762x39` + variants) | `:367` |
| `reloadAction` | reload action (e.g. `"ReloadAKM"`) | `:374` |
| `modes[] = {"SemiAuto","FullAuto"}` | fire modes list | `:375` |
| `weaponStateAnim` | the weapon-states `.anm` (bolt/bullet driver) — anim-side artifact, path lives in config | `:378` |
| `class SemiAuto : Mode_SemiAuto` / `class FullAuto : Mode_FullAuto` | per-mode `dispersion`, rpm, recoil, soundsets | (per-mode blocks) |
| `dispersion` | per-mode cone | `:386` (in the mode block) |
| `magazines[]` | accepted magazine classes | `F0-herencia-decision.md:14` |
| `simpleHiddenSelections[]` | script-toggled simple selections (`hide_pistolgrip`/`hide_sidemount` on A6 AK) | `F0-herencia-decision.md:17` |
| `attachments[]` | exposed slots (`weaponOptics`, `weaponMuzzleAK`, `weaponButtstockM4`, `weaponForegrip`, `weaponLight*`…) | `F0-herencia-decision.md:18` |
| `inventorySlot` (proxy/mag) | the slot an attachment/proxy occupies (e.g. `"magazine"`) | (attachment/proxy blocks) |

SR2M adds `discreteDistance[] = {100,200}` for irons (product-spec B7); `class OpticsInfoRifle` is
forward-declared before `CfgWeapons` (`A6_SR2M\config.cpp:772`). Verify a field's exact line by grep
before quoting — the table above is the AKM block; your inherited base may differ.

## W-CFG1 — `Mode_SemiAuto` / `Mode_FullAuto` live at ROOT config scope `[VERIFIED in-game]`

SR2M bug #10 (`A6_SR2M\config.cpp:773-778`, the comment IS the root-cause note):
```
class OpticsInfoRifle;
// bug #10 ROOT CAUSE: vanilla Mode_SemiAuto/Mode_FullAuto live at ROOT config
// scope (dtain.pbo config.cpp:273,307; Mode_FullAuto has autoFire=1), NOT inside
// CfgWeapons. Declaring them inside CfgWeapons shadowed the real ones with empty stubs
// -> FullAuto lost autoFire -> engine dropped it -> single mode. Keep these at ROOT.
class Mode_SemiAuto;
class Mode_FullAuto;
class CfgWeapons
{ ... }
```
Forward-declaring `Mode_FullAuto` INSIDE `CfgWeapons` makes an empty stub without `autoFire=1` → the mode
is dropped → the weapon is stuck in single. The forward declarations MUST be at ROOT scope, before
`class CfgWeapons`. `[VERIFIED in-game]` a6-sr2m memory 2026-06-28.

`[UNVERIFIED]` the exact vanilla `bin.pbo` lines `dta\bin.pbo config.cpp:273,307` for `Mode_FullAuto
autoFire=1` — that comes from the SR2M config comment, not independently read (vanilla `bin.pbo` is
binarized). `[UNVERIFIED]` the vault note `dayz-weapon-config-crossproject.md` (INV-W1, SP-031), cited by
a6-sr2m memory as the durable home of this lesson, was not located this session — read it before
extending the cross-project config record.

## The jam / unjam system — CONFIG side

- Jam is a `Weapon_Base` state: `m_isJammed` (`weapon_base.c:50`), `m_ChanceToJam[]` per health level
  (`:72`). `[VERIFIED-vanilla]`
- `RifleBoltFree_Base` inserts the `WeaponActions.UNJAMMING` START/END ability records
  (`rifleboltfree_base.c:119-120`) and the `WeaponActions.FIRE` `FIRE_JAM` action (`:122`).
  `[VERIFIED-vanilla]`
- Vanilla firearm actions live in
  `scripts\4_world\classes\useractionscomponent\actions\weapons\` (`firearmactionunjam.c`,
  `firearmactionmechanicmanipulate.c`, etc.).

**Config side = you inherit the base's abilities; you supply the jammed selection state via
`weaponStateAnim`.** The MOTION of the unjam is delegated — the unjam-loop root cause (timer-driven ~5 s
hold, needs `LoopStart`/`LoopEnd`) is documented in `dayz-animation-pipeline` (a6-sr2m memory
2026-07-02). Do not try to fix a jam-motion bug from config here.

## Attachments / optics slots

- `attachments[]` declares the slots the weapon exposes (`weaponOptics`, `weaponMuzzleAK`,
  `weaponButtstockM4`, `weaponForegrip`, `weaponLight*`…). An attachment attaches only if its own
  `inventorySlot` matches a slot in `attachments[]`.
- SR2M `[VERIFIED in-game]` v1 2026-06-25: an optics **mount** (`A6_SR2M_RailMount`) is a child slot —
  the mount occupies the weapon's optics slot and itself exposes a slot that accepts M4 optics
  (mount-gated optics). Two side slots use A6 switchables (`A6_SR2M\CLAUDE.md` "Decisiones fijas").
- `[UNVERIFIED]` the exact switchable rail flags (`hasRailFunctionality`,
  `CanAcceptLeft/RightFlashlight`) come from the SR2M product-spec changelog citing
  `A6_Switchable_Base.c:35-40` — re-read that file before writing an attachment section that quotes them.

## Cite-then-verify

The firearm hierarchy is not linear (above) and per-mode blocks vary by base. Grep the field in
`P:\scripts\4_world\entities\firearms\` or in the des-rapificado config before writing it; keep the
`[VERIFIED in-game]` / `[VERIFIED-vanilla]` / `[UNVERIFIED]` labels.

## DayZ 1.30 changes — config / script contract deltas

Entity-side only, re-verified on 1.30.164078 (`P:\scripts\4_world\…` unless noted). Details and
`[EXACT]` blocks: `dayz-1-30-weapons.md`.

- **Script recoil vs CfgRecoils.** `SpawnRecoilObject()` returns `SCARLRecoil` / `LugerRecoil` /
  `LeeEnfieldRecoil` / `MP18Recoil` (`classes\recoilbase\recoils\*.c`). Shipped `SCARH_Base` still sets
  `recoil = "recoil_fal"` in config (`P:\DZ\weapons\firearms\scarh\config.cpp:94`) — that is a different
  channel. Matching a 5.56 SCAR-shaped mod to SCAR-H means inheriting that config block, then swapping
  `chamberableFrom` / `magazines[]` / script recoil; there is no SCAR-L `CfgWeapons` class to unRap.
- **Inherited config values moved.** If you inherit a vanilla base, re-read it on `P:\DZ`: 1.30 changed
  `SCARH_Base` (`WeaponLength` 0.8→0.95, `ObstructionDistance` 0.5→0.7, `barrelArmor` 2.4→2.6,
  `initSpeedMultiplier` 0.975→0.925), `FAL_Base` `initSpeedMultiplier` 0.9→0.95, `Winchester70_Base`
  gained `ObstructionDistance=0.955`, and `AKM_Base` / `Magnum_Base` `hiddenSelections[]` became
  `{"camo","camo2"}` (`P:\DZ\weapons\firearms\akm\config.cpp:72-76`) — a skin that sets one
  `hiddenSelectionsTextures[]` entry now leaves `camo2` on the vanilla texture. Source: config-delta.
- **Magazine attach/detach.** `FirearmActionAttachMagazine` requires
  `TestAttachMagazine(GetCurrentMuzzle(), mag, false, true)`
  (`classes\useractionscomponent\actions\weapons\firearmactionattachmagazine.c:256`).
  `FirearmActionDetachMagazine_Old` is `[Obsolete("1.30: Use class FirearmActionDetachMagazine instead")]`
  (`firearmactiondetachmagazine.c:13`). Open-bolt bases register the detach action in `SetActions()`
  (`entities\firearms\openbolt_base.c:388`).
- **FSM repair on mag yank.** `Weapon_Base.EEItemDetached` → `ValidateAndRepair()` if
  `item.IsMagazine()` (`weapon_base.c:1115-1121`).
- **Explosives.** Do not call `ExplosivesBase.SetParticleExplosion` / `AddExplosionEffectForSurface`
  (`[Obsolete("1.30: Handled in AmmoTypes instead")]`, `explosivesbase.c:306-310`, `:386-387`, `:432-433`);
  1.30 `ExplosivesBase` no longer plays that particle at all. Use `CfgAmmo` `particle` (+
  `AmmoTypesAPI.AddExplosionParticleEffect` for per-surface variants) and read it with
  `AmmoTypesAPI.GetExplosiveEffectData`. `CfgAmmo LandMineExplosion` is now `LandMineExplosion_Ammo`.
