---
name: dayz-ai-patterns
description: >
  Use this skill when working with DayZ Expansion AI (eAI module): FSM states (Dormant, Fighting),
  combat hysteresis, threat/targeting, LOS/FOV, pathfinding, group formation, squad target
  distribution, or the NoiseSystem. Also trigger when a user asks why AI bots are not
  attacking, not patrolling, walking through doors, ignoring sounds, all focusing the same
  enemy, not spreading into formation, or when debugging eAIBase, eAIGroup, eAIState, or
  ExpansionPathHandler. Trigger on: eAI, DayZ AI, AI FSM, FSM pathfinding, group AI,
  squad AI, threat targeting, LOS FOV AI, Expansion AI, AI bots.
  DayZ 1.30: native AI noise (AIParams, GetEnvironmentNoiseReduction), sandstorm
  MINDSTATE_COWER, IsSurrendered, animal group AI, navmesh regeneration.
---

# DayZ Expansion AI Patterns (eAI)

> Material source: salutesh/DayZ-Expansion-Scripts @ 8f75d554fda209b257c00deb8f01c181e67c980a
> Research: dual-arm (Claude + Codex), adversarially verified. All path:line cites are confirmed.

> Target: DayZ 1.30 stable (1.30.164078). Version labels: ../_shared/dayz-version-policy.md. Porting a ≤1.29 mod: dayz-130-migration skill.

The PC-* patterns below cite that Expansion commit, which targets DayZ ≤1.29; no Expansion source is on
this machine, so those cites were **not** re-verified for 1.30. The vanilla parts (NoiseSystem, infected
mind state, animal AI) were re-verified on 1.30.164078. What an eAI-style mod must re-check on 1.30:
§ DayZ 1.30 changes and `references/dayz-1-30-ai.md`.

## Overview

12 patterns covering the irreducible AI architecture of Expansion eAI:

| ID   | Pattern                                  | File reference                          |
|------|------------------------------------------|-----------------------------------------|
| PC-5 | Dormant state: DisableSimulation via FSM | `eaistate_dormant.c:1-17`               |
| PC-11| Fighting FSM: hysteresis + guard clauses | `eaistate_fighting.c:18-38`             |
| PC-13| FSM loaded from XML + sub-FSM composition| `eAIBase.c:661-666`, `Master.xml:3-8`   |
| PC-9a| FOV pre-filter before LOS raycast        | `eAIBase.c:8156-8168`                   |
| PC-9b| LOS throttle adaptive to update interval | `eAIBase.c:8089-8097`                   |
| PC-14| Threat cache with jitter 250-300 ms      | `eAITargetInformationState.c:58-67`     |
| PC-7 | Staggered target scan + jitter 0.1-0.2 s | `eAIBase.c:2962-2966`                   |
| PC-2 | Backoff 5x: no players in visibility     | `ExpansionWorld.c:247-250`              |
| PC-15| Pathfinding: recalc thresholds + cost    | `ExpansionPathHandler.c:581-592`        |
| PC-16| Path filters: navmesh costs (door=10000) | `expansionpathfilters.c:131-147`        |
| PC-17| Group targets: distribution by tracking  | `eAIGroup.c:726`, `eAIBase.c:3497-3505` |
| PC-18| Formation positions: cached + jitter 2-4s| `eAIGroup.c:822-830`                    |

Server-side scheduling/throttling principles (budget-per-frame, FPS-adaptive interval, rolling average, modulo-gating): canonical server-perf write-up: `enforce-script-reference/references/server-performance.md` (PC-2/PC-7/PC-9b in THIS skill are the eAI-specific instances). For `#ifdef SERVER` vs `IsServer()` side checks and ScriptInvoker lifecycle, `enforce-script-reference` does apply (hard rules 17-20, 28).

---

## FSM Architecture

### PC-13 — FSM loaded from XML; sub-FSM composition

`eAIBase.c:661-666` + `Scripts/FSM/Master.xml:3-8`

`LoadFSM()` calls `ExpansionFSMType.LoadXML(...)`, then `Spawn(...)` and `StartDefault()`. Master.xml includes sub-FSMs for Vehicles, Fighting, and Reloading. Dormant is a state in the master.

Key implication: state composition changes via XML, not Enforce code. Adding or removing states does not require a code recompile.

Full detail: `references/eai-fsm-combat.md` — PC-13.

### PC-5 — Dormant state: DisableSimulation managed by FSM

`eaistate_dormant.c:1-17`

On enter: `unit.DisableSimulation(true)`. On exit: `unit.DisableSimulation(false)`. Entry conditions checked by FSM transitions: no combat, no players within visibility range, no pending waypoints, no active movement, leader is an AI unit.

Do not scatter `DisableSimulation` calls across logic — let the FSM state own that toggle. The FSM exit restores simulation; no caller needs to remember to re-enable it.

Full detail: `references/eai-fsm-combat.md` — PC-5.

### Vanilla infected mind-state: `MINDSTATE_COWER` (1.30)

This is **not** Expansion eAI. Vanilla `DayZInfected` gained `DayZInfectedConstants.MINDSTATE_COWER` after `MINDSTATE_FIGHT` (`P:\scripts\3_game\entities\dayzinfected.c:18`). `ZombieBase` syncs `m_MindState` with range `-1..5` (`P:\scripts\4_world\entities\creatures\infected\zombiebase.c:54`; before 1.30: `-1..4`). On the sandstorm cower state the infected holds the calm idle pose (`SetIdleState(0)`); native AI already stopped movement (`zombiebase.c:474-478`). Animals got a `SlotCowering` behaviour in `CfgAIBehaviours` too. `[EXACT]` blocks: `references/dayz-1-30-ai.md`.

---

## Combat FSM

### PC-11 — Fighting FSM: hysteresis thresholds and guard clause chain

`eaistate_fighting.c:18-38` (enter/exit logic)
`eaistate_fighting_fireweapon.c:78` (fire sub-FSM guards)

Enter combat when `GetThreatToSelf() >= 0.4`. Exit when it drops below `0.2`. The gap prevents oscillation when threat hovers around a single value.

Guard clauses before enter: `IsRestrained`, `IsUnconscious`, `IsInTransport`, `GetRunningAction`. The fire-weapon sub-FSM has its own chain: `IsRestrained -> GetTarget -> IsFighting`. `IsFightingFSM` flag is set so external systems can query combat state without inspecting the FSM directly.

Hysteresis rule: always set enter > exit. A single threshold creates a flip-flop bug in practice.

Full detail: `references/eai-fsm-combat.md` — PC-11.

---

## Threat & Targeting

### PC-14 — Threat cache with jitter 250-300 ms

`eAITargetInformationState.c:58-67`

```
if (force || diff > Math.RandomIntInclusive(250, 300))
{
    m_ThreatLevelUpdateTimestamp = time;
    m_ThreatLevel = m_Info.CalculateThreat(m_AI, this);
```

Each target information state carries its own timestamp and a per-call random cooldown. `CalculateThreat` runs at most once per ~275 ms on average. The jitter means AIs do not all recalculate threat on the same server tick.

Full detail: `references/eai-pathfinding-groups.md` — PC-14.

### PC-7 — Staggered target scan: persistent index + jitter 0.1-0.2 s

`eAIBase.c:2962-2966`

```
m_eAI_UpdateTargetsTick += pDt;
if (m_eAI_CurrentPotentialTargetIndex >= m_eAI_PotentialTargetEntities.Count()
    && m_eAI_UpdateTargetsTick > Math.RandomFloat(0.1, 0.2))
{
    m_eAI_UpdateTargetsTick = 0;
    m_eAI_CurrentPotentialTargetIndex = 0;
```

`UpdateTargets` advances one index per tick rather than scanning the entire list. A full refresh only happens when the index is exhausted AND the random interval has passed. Prevents all AIs from finishing their scan simultaneously.

Full detail: `references/eai-pathfinding-groups.md` — PC-7.

---

## LOS / FOV

### PC-9a — FOV pre-filter before LOS raycast

`eAIBase.c:8156-8168`

`eAI_CalculateFOVHalfAngleH` produces the effective half-angle (stance-adjusted). If `angleDiffH > threshAngleH`, the candidate is discarded before a raycast is issued. Angle comparison costs nanoseconds; a raycast costs microseconds to milliseconds depending on scene complexity.

Apply cheap spatial tests (angle, range band) before expensive ones (raycast, physics query).

Full detail: `references/eai-fsm-combat.md` — PC-9 (Codex angle).

### PC-9b — LOS throttle adaptive to current update interval

`eAIBase.c:8089-8097`

```
if (s_eAI_FTO < 2)
    interval = LOS_CHECK_INTERVAL;
else
    interval = LOS_CHECK_INTERVAL * (pDt / ExpansionWorld.AI_UPDATE_INTERVAL);
if (m_eAI_LOSCheckDT < interval && !m_eAI_TargetChanged)
    return state.m_LOS;
```

In scheduler mode (`s_eAI_FTO >= 2`) the LOS interval scales with `pDt`. An AI updated at 5x the base interval does not get 5x the LOS check rate — the effective check frequency remains constant. Setting `m_eAI_TargetChanged = true` forces a fresh check, bypassing the cache.

Full detail: `references/eai-pathfinding-groups.md` — PC-9 (Claude angle).

### PC-2 — Backoff 5x when no players within visibility distance (AI-specific)

`ExpansionWorld.c:247-250`

```
if (eAIBase.s_eAI_FTO >= 2 && ai.m_eAI_PlayersWithinVisibilityDistanceLimit.Count() == 0)
    updateIntervalCurrent = AI_UPDATE_INTERVAL * 5;
else
    updateIntervalCurrent = updateInterval;
```

When no player is within the AI's visibility range, its update interval degrades from ~50 ms to ~250 ms. Logic is not disabled — it runs at one-fifth frequency. Condition is AI-local: each unit decides its own backoff independently.

This is the AI-specific implementation of the backoff principle (degrade frequency by relevance). Canonical server-perf write-up: `enforce-script-reference/references/server-performance.md` (PC-2/PC-7/PC-9b are the eAI-specific instances).

---

## Pathfinding

### PC-15 — Path recalculation with thresholds and measured cost

`ExpansionPathHandler.c:581-592`

`ExpansionPathHandler` does not recalculate on every target update. Three guards:
- `PATH_RECALCULATE_THRESHOLD`: minimum positional delta before path is stale
- `m_MinTimeUntilNextUpdate`: cooldown between recalcs
- `m_TimeIt.GetElapsed()`: actual cost measurement per recalc (feedback for adaptive scheduling)

Legacy path: `eAIPathFinding`, activated with `#define EAI_USE_LEGACY_PATHFINDING`. Default is `ExpansionPathHandler`.

Full detail: `references/eai-pathfinding-groups.md` — PC-15.

### PC-16 — Path filters with navmesh costs; DOOR_OPENED = 10 000

`expansionpathfilters.c:131-147`

`SetFilterCost` per area type: CRAWL, CROUCH, JUMP, DEEP_WATER = 10. DOOR_OPENED = 10 000. Cost 10 000 is effectively prohibitive — the pathfinder routes around an open door rather than through it (avoids collision with door physics geometry).

Filters encode behavioral preferences, not just include/exclude flags. A cost of 10 000 is a soft prohibition without a hard constraint.

Full detail: `references/eai-pathfinding-groups.md` — PC-16.

---

## Group & Formation

### PC-17 — Group target distribution: prevents squad over-concentration

`eAIGroup.c:726` + `eAIBase.c:3497-3505`

`eAIGroup.AddTarget` distributes target data to members. Before assigning target X to a member, `eAIBase` checks `num_ai_in_group_not_tracking`: if it reaches 0 (everyone already tracks X), returns false. The squad naturally distributes across multiple enemies without explicit assignment logic.

If all bots are chasing the same player, check whether `num_ai_in_group_not_tracking` logic is intact and `AddTarget` is being called rather than bypassed.

Full detail: `references/eai-pathfinding-groups.md` — PC-17.

### PC-18 — Formation positions cached with per-AI jitter 2-4 s

`eAIGroup.c:822-830`

`GetFormationPosition` checks `m_eAI_FormationPositionUpdateTime` with `Math.RandomFloat(2.0, 4.0)`. Each member has its own jitter window. When the leader moves, members do not all recalculate simultaneously — they spread the recalc over a 2 s window.

Formation recalc is per-AI, not per-group. The group does not broadcast "recalculate now" to all members.

Full detail: `references/eai-pathfinding-groups.md` — PC-18.

---

## Animal & Group AI Templates (vanilla)

Native group-behaviour templates live in `GroupBehaviourTemplates`. These are **not** Expansion eAI FSMs.
On 1.30.164078 `P:\DZ\data\aiconfigs\config.cpp:77` has the same five as 1.29 (`DZAmbientLifeGroupBeh`,
`DZWolfGroupBeh`, `DZDeerGroupBeh`, `DZdomesticGroupBeh`, `DZSheepGroupBeh`); Bliss adds
`BlissBearGroupBeh` (`P:\DZ\data_bliss\aiconfigs\config.cpp:9`).

Seen on 1.30 Exp 164014 only, in the Nasdara data PBO (`data_takistan\aiconfigs\config.cpp`), which is
**not** unpacked on `P:\` — `[UNVERIFIED on 1.30.164078]`:

- `DZMouflonGroupBeh` — `type = "DomesticHerbivores"`: graze/rest lifecycle, `safeKeeperIntervalMin/Max`, catch-up radii.
- `DZDogGroupBeh` — `type = "Predators"`: `zoneType = "HuntingGround"`, `siegeAttackCountdownMin/Max`, `huntAttackCountdownMin/Max`, `safeKeeperIntervalMin/Max`.

The matching animal scripts (`Animal_OvisGmelini`, `Animal_CanisFamiliaris_Dobermann_*` /
`_GermanShepherd_*`, `Animal_VaranusGriseus`) exist in `P:\scripts\4_world\entities\creatures\animals\`
but have no config class in `P:\DZ`.

`[DESIGN]` Custom maps that want the same packs copy these classes (or inherit them) in their own `aiconfigs` rather than reimplementing predator/herbivore FSMs in script. Details: `references/dayz-1-30-ai.md`.

---

## References

- `references/eai-fsm-combat.md` — PC-5, PC-11, PC-13, PC-9a
- `references/eai-pathfinding-groups.md` — PC-14, PC-7, PC-2, PC-9b, PC-15, PC-16, PC-17, PC-18
- `references/dayz-1-30-ai.md` — 1.30: native noise attenuation, `MINDSTATE_COWER`, `AIParams`, animal group templates, eAI re-check list


## NoiseSystem — feeding noise the AI can hear (added 2026-06-06)

Re-verified on 1.30.164078 (`P:\scripts\3_game\noise.c:1-26`; first written against v1.24):

- Server-side API: `g_Game.GetNoiseSystem().AddNoise(entity, params, mult)` /
  `AddNoisePos(entity, pos, params, mult)` / `AddNoiseTarget(pos, lifetimeSec, params, mult)`.
- `AddNoiseTarget` creates a positional **decoy with a duration** — infected/AI investigate a
  position with no entity attached (distraction devices, impacts). Helper:
  `MiscGameplayFunctions.GenerateAINoiseAtPositionEx(source, lifeTime, params)`
  (`P:\scripts\4_world\static\miscgameplayfunctions.c:1886`); the old position-based
  `GenerateAINoiseAtPosition` is `[Obsolete]` (`:2072`).
- `NoiseParams.Load("CfgNoises_entry")` or `LoadFromPath(path)`.
- Weather dampening is native in 1.30. Tune rain/snow/fog/wind/sandstorm in `class AIParams`
  (`P:\DZ\data\aiconfigs\config.cpp:12-22`). Query what the AI applies at a point with
  `NoiseSystem.GetEnvironmentNoiseReduction(vector pos)` (`P:\scripts\3_game\noise.c:13`; an absolute
  strength reduction per its comment, not a 0..1 multiplier). `NoiseAIEvaluate.GetNoiseReduction` and
  `Weather.GetNoiseReductionByWeather` are `[Obsolete]` (`P:\scripts\4_world\static\sensesaievaluate.c:92-93`,
  `P:\scripts\3_game\weather.c:454-455`). Player steps pass only the stance/shoe/surface multiplier:
  `noiseMultiplier = NoiseAIEvaluate.GetNoiseMultiplier(this)` then `AddNoise(noiseParams, noiseMultiplier)`
  (`P:\scripts\4_world\entities\dayzplayerimplement.c:3472-3474`). Infected voice noise is
  `AddNoise(this, sound_event.m_NoiseParams)` with no climate multiplier (`zombiebase.c:597-600`). Do
  **not** also multiply `AddNoise` by a script weather factor — the engine attenuates natively and you
  would double-dampen.
  (Before 1.30: script did the dampening — `AddNoise(..., NoiseAIEvaluate.GetNoiseReduction(g_Game.GetWeather()))`
  on player steps, sound events, doors, infected voices; 1.29: `O:\…\scripts\4_world\entities\dayzplayerimplement.c:3344`.)
- Weapon shots are config-side: `class NoiseShoot { strength = 82; type = "shot"; }` in the weapon
  config (real mod example: IMPWMODPart2 MCXSpear `config.cpp:73-77`).

## DayZ 1.30 changes

Re-verified on 1.30.164078 (`P:\`). What changes, what breaks, and the migration checklist. `[EXACT]`
blocks and line numbers: `references/dayz-1-30-ai.md`.

**What changes**

- Native noise attenuation for native AI (infected, animals). New proto
  `NoiseSystem.GetEnvironmentNoiseReduction(vector pos)` (`P:\scripts\3_game\noise.c:12-13`); only the debug
  HUD reads it in vanilla script (`P:\scripts\4_world\plugins\pluginbase\pluginpresencenotifier.c:155-159`).
  No vanilla script multiplies weather into `AddNoise` any more.
- `class AIParams` gained `snowfallToNoiseMultiplier = 5`, `fogToNoiseMultiplier = 10`,
  `windToNoiseMultiplier = 10`, `sandstormToNoiseMultiplier = 10` (`P:\DZ\data\aiconfigs\config.cpp:16-20`;
  before 1.30: rain + sea only). `[CHANGELOG 1.30]` says `'Weather.GetNoiseReductionByWeather'` was removed;
  on 1.30.164078 it **still exists** as `[Obsolete]` (`P:\scripts\3_game\weather.c:454-467`), next to the
  new, also obsolete, `GetNoiseReductionByWeatherEx(object)` (`:407-420`).
- `MiscGameplayFunctions.GenerateAINoiseAtPosition` → `GenerateAINoiseAtPositionEx(Object, …)`;
  `CarScript.GenerateCarHornAINoise` removed (horn → `VehicleHornComponent`).
- Vanilla infected: `MINDSTATE_COWER` for sandstorm; net-sync range of `m_MindState` is now `-1..5`.
  Animals: `SlotCowering` added to their `CfgAIBehaviours`.
- Surrender: `Man.IsSurrendered()` / `PlayerBase.SetSurrenderState(bool)`; `EmoteManager.ForceSurrenderState`
  and `SurrenderDummyItem` are obsolete.
- New animal scripts (mouflon, dogs, monitor lizard) without configs in `P:\DZ`; the Nasdara group templates
  `DZMouflonGroupBeh` / `DZDogGroupBeh` were seen on Exp only (not in `P:\DZ`, unverified on stable).
- `[CHANGELOG 1.30]` navmesh file version has **no** backwards compatibility — regenerate navmesh for custom
  terrains (matters to eAI PC-15/PC-16). Navmesh can use a hide animation source via EntityType Navmesh config.

**What breaks for a ≤1.29 AI / stealth mod**

1. Compile warning `[Obsolete("Use Weather.GetNoiseReductionByWeatherEx instead!")]` /
   `[Obsolete("Use GetNoiseReductionEx instead!")]` if you still call `GetNoiseReductionByWeather` /
   `NoiseAIEvaluate.GetNoiseReduction`.
2. Overrides of `GetNoiseReductionByWeather` no longer change what native infected/animal AI hear — they
   use the native `AIParams` curve.
3. Passing a weather-derived multiplier into `AddNoise` **and** letting the engine apply native reduction
   double-damps the event.
4. Threat code that detects a surrendered player by `SurrenderDummyItem` in hands stops working.
5. Code that switches on infected mind state without a `MINDSTATE_COWER` case.
6. Custom terrains: old navmesh files will not load. `[CHANGELOG 1.30]`

**Migration checklist**

- [ ] For what native AI actually applies at a point: `GetGame().GetNoiseSystem().GetEnvironmentNoiseReduction(pos)`
      (absolute reduction, not a multiplier).
- [ ] For HUD / old script math on a specific object: `weather.GetNoiseReductionByWeatherEx(object)` (obsolete
      but sandstorm-aware).
- [ ] To change how rain/snow/fog/wind/sandstorm dampen zombie hearing: patch `class AIParams`
      (`rainToNoiseMultiplier`, `snowfallToNoiseMultiplier`, `sandstormToNoiseMultiplier`, …).
- [ ] Call `AddNoise(entity, params)` without folding climate into `external_strenght_multiplier`; use
      `GenerateAINoiseAtPositionEx`.
- [ ] If you spawn custom infected, keep `m_MindState` sync range covering `MINDSTATE_COWER` (5).
- [ ] Surrender checks: `IsSurrendered()`, not an item in hands.
- [ ] Custom maps: regenerate navmesh. Copy/inherit group templates only after checking they exist in your
      1.30 data.
- [ ] eAI-style `PlayerBase` bots: run the 8-point re-check in `references/dayz-1-30-ai.md` § "What an
      eAI-style mod must re-check" (hearing, emitted noise, surrender, cower, inventory/hands, weapons,
      `ProcessVariables(float)`/`OnCEIterate`/storage 144, navmesh).
- [ ] Expansion eAI PC-5..PC-18 were **not** re-verified for 1.30 (no Expansion source on this machine).
      Treat those cites as ≤1.29 Expansion research and re-check them against the Expansion build you run.
