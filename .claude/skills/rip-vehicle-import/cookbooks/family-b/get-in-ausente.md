# Cookbook B — get-in ausente

> Family B. This body was moved without rewriting in CAMBIO-1; status notes and paths remain exactly as they were in the origin.
>
> **Target: DayZ 1.30 stable (1.30.164200); labels `../../../_shared/dayz-version-policy.md`.** The hashed block below quotes
> **1.29** line numbers and is kept byte-exact; read it with the "1.30 deltas" section that follows it.

<!-- MOVED-EXACT source="dayz-vehicles/SKILL.md:344" sha256="E7A04C3A464FDD6735E3D7D564267E9464762A811903E504F50E02147BA727F1" -->
## GET-IN DOESN'T APPEAR — name the guard BEFORE touching the model (SP-141, added 2026-07-29)

Four vehicles in this vault have burned iterations on "the get-in prompt does not appear"
(LFQuad, MercedesAMGLF, LFHeli OH-1 R3, LFHeli HH-60G). The prompt is gated by **five ordered
guards** inside one function, and a *necessary* chain is not a *measured* cause: knowing the
prompt must pass through `CanReachSeatFromDoors` says nothing about which guard is firing.
Name the guard first; the fix follows in minutes.

`ActionGetInTransport.ActionCondition` (`actiongetintransport.c:50-79`) has **exactly one path
to `true`**, and rejects in this order:

1. `CrewPositionIndex(componentIndex) < 0` — the ViewGeometry component under the cursor is not
   dual-tagged `componentNN`, or its selection is not the seat's `actionSel` (preflight #4).
2. `CrewMember(crew_index)` non-null — seat occupied.
3. `!CrewCanGetThrough(crew_index)` — door state / seat-fold gate. ★ Base
   `OffroadHatchback.CrewCanGetThrough` covers only posIdx 0..3 and then **`return false`**
   (`offroadhatchback.c:212-250`), so ANY vehicle with more than four seats must override it or
   seats 4+ are dead. A `true` on posIdx >= 4 is proof your override is running.
4. `!IsAreaAtDoorFree(crew_index)` — engine-side door area.
5. `!CanReachSeatFromDoors(selection, player.GetPosition(), 1.0)` — and this one has three
   sub-conditions, all silent (`carscript.c:2708-2731`):
   - `GetDoorConditionPointFromSelection(sel)` must return a non-empty name. ★ **The trap**:
     base `CarScript` knows only FOUR cases, all lowercase — `seat_driver`, `seat_codriver`,
     `seat_cargo1`, `seat_cargo2` (`carscript.c:2673-2692`) — and `OffroadHatchback` the same
     six lowercase ones (`offroadhatchback.c:351-365`). Any other seat selection name returns
     `""` and the seat can NEVER be boarded, with config, bones, proxies and componentNN all
     correct. A custom seat set REQUIRES overriding this method.
   - `MemoryPointExists(conPointName)` — the point must be in the **Memory LOD** of the
     shipped model.
   - distance **IN PLAN** (height is zeroed) `<= pDistance`, and the action passes **1.0 m**.
     Vanilla places its condition points ~0.26 m OUTSIDE the hull at the door station
     (measured on `offroadhatchback` MLOD: `seat_con_1_1` x=1.1586 against a half-width of
     0.900) and REUSES two points for four seats. On a long fuselage two points cannot cover
     ten seats.

**The instrument** (DayZ-MCP): `query_get_in_condition` with a `component` index returns
`first_block` = exactly one of `componentNN` / `occupied` / `crew_can_get_through` /
`area_blocked` / `unreachable` / `""`, plus per-seat `crew_can_get_through`, `area_free`,
`occupied`, `reachable` — the `reachable` loop being the same `GetActionComponentNameList` ->
`CanReachSeatFromDoors` the action runs. **That names the guard in one call, offline of the
user's eye.** Pass `component=-1` for the whole crew bank (note: `reachable` is hardcoded false
in that mode — only the per-component call measures it).

Measured case, HH-60G 2026-07-29: all ten seats `first_block="unreachable"` with guards 1-4
GREEN on all ten, so the block is guard 5 alone — and that killed two plausible sub-causes at
once, because neither camelCase nor radius can explain a lowercase seat whose point was 1 mm
from the player.

★ **Discipline that this cost**: a plan that declared "measured mechanism" on the strength of
the chain being necessary was rejected by review for exactly that. Measure `first_block` per
seat BEFORE editing the model, the config or the script.

<!-- END MOVED-EXACT -->

## 1.30 deltas to the block above (re-verified on `P:\scripts`, 1.30.164078, 2026-10-15)

- 1.30 adds a guard **before** guard 1: `if (!transport.CanGetIn()) return false;`
  (`actiongetintransport.c:36`; default `true`, `transport.c:675-678`). Vanilla bikes override it (`motorbikescript.c:225-231`).
- Guard 4 is called as `IsAreaAtDoorFreeDiag(crewIndex)` (`actiongetintransport.c:54`), which only adds a DayZDiag bypass
  switch in front of `IsAreaAtDoorFree` (`transport.c:910-924`); guards 3 and 4 share that line. Variables are renamed
  `transport` / `crewIndex`.
- Line numbers on 1.30: `ActionCondition` `actiongetintransport.c:26-69` (guards at `:45-66`, reach loop `:62-66`, still
  passing `1.0`); `OffroadHatchback.CrewCanGetThrough` `offroadhatchback.c:225-263`; `CarScript.GetDoorConditionPointFromSelection`
  `carscript.c:2141-2160`, `CarScript.CanReachSeatFromDoors` `carscript.c:2177-2198`; `OffroadHatchback.GetDoorConditionPointFromSelection`
  `offroadhatchback.c:364-378`. The guard order and every sub-condition are otherwise unchanged.
- Separate 1.30 trap for a ripped car on a bare `CarScript` class: get-in works once the guards pass, but the engine never
  starts unless the script class also overrides `GetElectricitySourceDevice()` (base returns `null`, `transport.c:835-838`;
  pattern `civiliansedan.c:59-74`). See `dayz-vehicles/references/dayz-1-30-vehicles.md` §2.

## Fourth subcause of guard 5 (measured, WRX STI B-01): the base seat->door table is LEFT-HAND-DRIVE

[EXACT] On a right-hand-drive car with memory points named by physical side, base
`CarScript.GetDoorConditionPointFromSelection` pairs every seat with the FAR door
(`carscript.c:2141-2160`, re-checked on 1.30.164078): `CanReachSeatFromDoors` fails the 1.0 m plan test
from the correct door and no get-in appears, with names, points and `componentNN` all correct.
`query_get_in_condition` signature: `unreachable` from the correct door and `available` from the
opposite one - the exact mirror of the healthy sweep. Override crossing the sides
(driver->`seat_con_2_1`, codriver->`seat_con_1_1`, cargo1->`seat_con_2_2`, cargo2->`seat_con_1_2`),
as vanilla does for its own layouts (`offroadhatchback.c:364-378`, `van_01.c:269-287`). On RHD also
cross `GetSeatAnimationType` of the rear seats; the driver keeps `VEHICLESEAT_DRIVER`, whose get-in
animation will look LHD-sided - an engine limit, not a mod bug. (Measured on 1.30 Exp 164014;
first measured on 1.29 Diag, 2026-09-04.)
