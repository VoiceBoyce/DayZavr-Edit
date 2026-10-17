# Deep-dive verified additions

Extracted from `enforce-script-reference/SKILL.md` on 2026-07-07 (F3 sectioning).

Source-verified vs vanilla v1.24 + real mods: recipes/crafting (PluginRecipesManager), ComponentEnergyManager, action-system additions, damage pipeline, player internals/sync.

---

## Deep-dive verified additions (added 2026-06-06)

> Source-verified vs vanilla v1.24 + real mods (digests in
> `LF_RollingStone_dev/research/deep-dive-2026-06-06/`). Line refs +-3.

### Recipes / crafting (PluginRecipesManager)
- `MAX_NUMBER_OF_INGREDIENTS = 2` (`recipebase.c:1`) — recipes are physically limited to 2
  ingredients; `MAXIMUM_RESULTS = 10`.
- Real API: `InsertIngredient(index, classname)` — **`AddIngredient` does not exist**.
- The official typo is `RegisterRecipies()` (double i). Mod pattern verified in production:
  `modded class PluginRecipesManagerBase { override void RegisterRecipies() { super.RegisterRecipies(); RegisterRecipe(new MyRecipe); } }`.
- Base `CanDo()` rejects ingredients with attachments — override required for recipes with
  weapons/items with attachments. An override copies `ingredients[0]` and `[1]` to locals first
  and does not call `super.CanDo`: measured in game, the array read back after that call held the
  same item in both slots and the craft cancelled itself (`SKILL.md`, Override Rules, rule 42).
- `m_ResultToInventory`: only `-1` (to inventory) works; the swap branch `>= 0` is commented out in
  `SpawnItems`. `SetIsCacheable` does not exist.
- 1.30: a result with `m_ResultReplacesIngredient` no longer takes the ingredient's health — vanilla calls `TransferItemProperties(ingr, res, true, true, false, false)` (`transfer_health = false`, `4_world\classes\recipes\recipebase.c:353`; signature unchanged, `4_world\static\miscgameplayfunctions.c:269`); set `m_ResultInheritsHealth` if the result must inherit it. Before 1.30: `TransferItemProperties(ingr, res)` copied health. `PluginRecipesManager.GetRecipeClassName(int recipe_id)` is at `pluginrecipesmanager.c:84`.

### ComponentEnergyManager (quick facts)
- `MAX_SOCKETS_COUNT = 4` hardcoded (`3_game\tools\component\componentenergymanager.c:73`).
- `energyStorageMax` is optional: if you only define `energyAtSpawn`, that value acts as maximum.
- The energy chain is recursive: `ConsumeEnergy()`/`CanWork()` traverse sources upwards
  (safety limit 500 cycles).
- `OnSwitchOn/Off` fires on BOTH sides; `OnWorkStart/OnWork/OnWorkStop` server/SP only — real
  synchronization goes through `SetSynchDirty`.
- `compatiblePlugTypes` absent in config => the socket accepts all plugs.

### Action system (additions)
- `AddAction(typename)` in `SetActions()` registers a **global singleton per input type** — there are no
  per-instance actions nor dynamic runtime registration.
- `CCTCursor` measures from the hit-pos of the `ObjIntersectView` raycast (requires View Geometry LOD);
  `CCTObject` measures from target's `GetPosition()`. If an action does not appear and the model lacks
  ViewGeo, check the LOD first (skill `dayz-physics-engine`, truth #2).
- 1.30: `CCTCursor` and `CCTCursorInherited` also measure from `MiscGameplayFunctions.GetPlayerHeadPosition` — a stance-based height, not the Head bone (`cctcursor.c:28`, `cctcursorinherited.c:28-30`, `4_world\static\miscgameplayfunctions.c:759-787`). New `CCTLiquid` (ctor `cctliquid.c:37`) replaces vanilla use of `CCTWaterSurfaceEx` in fill/drink/wash. `Can()` of a custom CCT keeps its signature. Before 1.30: distance from hit-pos / `GetPosition()` only.

### Damage pipeline (quick facts)
- `ProcessDirectDamage(damageType, source, componentName, ammoName, modelPos, damageCoef, flags)`
  (`object.c:1138`) — `componentName` is the name of the **DamageZone**, not the model component.
- `damageCoef` multiplies the base damage of CfgAmmo. In vanilla TransportHit, coef = speed in m/s
  (`entityai.c:4134-4153`, `RegisterTransportHit`).
- `EEHitBy` is server-only; `EEHitByRemote` runs on the client that hit. `DecreaseHealth` bypasses
  the pipeline (no EEHitBy/animation/bleeding).
- `ProcessIndirectDamage` DOES NOT exist in script; radius damage =
  `DamageSystem.ExplosionDamage(source, null, ammo, pos, DamageType.EXPLOSION)` (`damagesystem.c:25`).
- `GetProtectionLevel` covers only DEF_BIOLOGICAL/DEF_CHEMICAL (hazmat) — ballistic absorption of
  clothing is C++/config, with no script API.
- Full TransportHit flow line by line: skill `dayz-physics-engine`,
  `references/dano-transporthit.md`.

### Player internals — sync (quick facts)
- Stamina syncs via **SyncJuncture** (`SJ_STAMINA`), not via `SetSynchDirty`
  (`staminahandler.c:831-839`). `EStaminaModifiers.PUSH_CAR` and `EStaminaConsumers.PUSH` already exist
  (`estaminamodifiers.c:13`) -> `DepleteStaminaEx` for custom costs.
- `eModifierSyncIDs`: only 7 bits used — `0x80..0x80000000` free for custom synchronized
  modifiers (`emodifiers.c:3-17`).
- `PlayerStatsPCO` uses POSITIONAL indices: a custom stat via `modded PlayerStatsPCO_current`
  needs an index above the last vanilla stat or it corrupts serialization. 1.30: `PlayerStatsPCO_current`
  extends `PlayerStatsPCO_v130` (`4_world\classes\playerstats\playerstatspco.c:362`), whose
  `EPlayerStats_v130` ends at `THERMAL_BIAS` = 11 (`:312-326`), so a mod stat needs index ≥ 12. Before
  1.30: `_current` extended `PlayerStatsPCO_v115` (last index 10, so > 10). A 1.29 mod that used index 11
  now collides with `THERMAL_BIAS`. Load-side consequences for old characters: skill `dayz-persistence`.
- Custom modifiers: `ModifiersManager` `Init()` hardcodes the list -> pattern
  `modded class ModifiersManager`. `CfgAgents` DOES NOT exist: agents are script classes registered
  in `PluginTransmissionAgents`.
