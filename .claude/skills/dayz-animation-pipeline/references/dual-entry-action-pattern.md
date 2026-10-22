# Dual entry action — kill the >180° yaw spin (Layer 1.5: scripted)

A scripted pattern that snaps the player to the correct side of a vehicle seat **before** the engine plays the get-in animation, so approaching from the "wrong" side does not produce a 180+° spin in the air. Layer 1.5: it is Enforce Script (not raw config, but no SEAnim/Workbench either).

> Target: DayZ 1.30 stable (1.30.164200). On 1.30, single-track vehicles can use the native directional get-in instead (§DayZ 1.30: native directional in/out, below). The LFQuad measurements in this file were made before 1.30; the call order they rely on is unchanged in 1.30 (next section).

## The bug this fixes

The get-in command forces the player's yaw to point along `dir(pos_<role>) → pos_<role>_dir`. The command is started in `ActionGetInTransport.Start` by `StartCommand_Vehicle` (`P:\scripts\4_world\classes\useractionscomponent\actions\interact\actiongetintransport.c:104`), right after `super.Start` has run `ActionBase.Start`, which calls `OnStartServer` / `OnStartClient` (`P:\scripts\4_world\classes\useractionscomponent\actionbase.c:775-799`). So an `OnStartServer`/`OnStartClient` override runs before the command starts. (corrected on 1.30.164078: this sentence used to say the engine forces the yaw "on `OnStartServer`". In 1.29 too the command started in `Start`, 1.29: `actiongetintransport.c:82-98`; 1.29's own `OnStartServer` override only called `car.ForceUpdateLightsStart()`, `:100-109`, and 1.30 removed that override.) That direction is computed from a single pair of memory points (`pos_driver` and `pos_driver_dir`). If the player approaches from the opposite side of the vehicle, the rotation needed to align with that direction is greater than 180°. The engine spins the player through the long way, producing an ugly mid-air rotation right before the climb-on animation starts.

The vanilla car configs work around this by placing the seat memory points on the side the player is "expected" to approach. That breaks the moment a player approaches from the other side — common on bikes, quads, and any vehicle with symmetric ingress.

## The fix (LL-getin-dual-entry)

Place **four** memory points per seat instead of two:

```
pos_driver_L      pos_driver_dir_L
pos_driver_R      pos_driver_dir_R
```

For a passenger seat, the same with `codriver` / `co` / whatever your role string is. The `_L` and `_R` are mirrored across the X axis of the vehicle so the seat itself doesn't move — only the snap point and facing direction change.

LFQuad reference layout (`LFQuad_dev/handoff_2026-05-28.md`):

```
pos_driver_L      = (+0.65, 0.05, -0.58)
pos_driver_dir_L  = (+0.65, 0.05, +0.42)   # 1.0 m forward of _L
pos_driver_R      = (-0.65, 0.05, -0.58)   # mirror in X
pos_driver_dir_R  = (-0.65, 0.05, +0.42)
```

Same shape for codriver, with the X offsets adjusted to the passenger seat (LFQuad uses the same `±0.65` because the quad has a single shared bench).

Then `modded class ActionGetInTransport` overrides `OnStartServer` AND `OnStartClient` to call a helper after the vanilla path runs. Verified against the LFQuad ground truth (`LFQuad_dev/task4_handoff/LFQuad_ActionGetInTransport.c`). On 1.30 `ActionGetInTransport` declares neither method itself, so `super` reaches `ActionBase.OnStartServer` / `OnStartClient` (`actionbase.c:1388`, `:1391`); the override still compiles and still runs before `StartCommand_Vehicle` (previous section). Not re-tested in game on 1.30.

```c
modded class ActionGetInTransport
{
    override void OnStartServer(ActionData action_data)
    {
        super.OnStartServer(action_data);   // vanilla path runs first
        TryRealignForLFQuad(action_data);    // then snap if this is our vehicle
    }

    override void OnStartClient(ActionData action_data)
    {
        super.OnStartClient(action_data);
        TryRealignForLFQuad(action_data);
    }

    // Bail-out branch keeps every non-LFQuad vehicle untouched.
    protected void TryRealignForLFQuad(ActionData action_data)
    {
        if (!action_data || !action_data.m_Player) return;
        PlayerBase player = PlayerBase.Cast(action_data.m_Player);
        if (!player) return;

        Object target = action_data.m_Target.GetObject();
        if (!target) return;

        // Class name is "LFQuad_base" (lowercase b) — verified in config.cpp.
        LFQuad_base quad;
        if (!Class.CastTo(quad, target)) return;

        vector worldP = player.GetPosition();
        vector localP = quad.WorldToModel(worldP);

        // 1) Which seat is closer to the player? Use crewdriver / crewcodriver
        //    memory points (NOT the action's slot — the action does not always
        //    carry that info reliably).
        bool isDriver = true;
        if (quad.MemoryPointExists("crewdriver") && quad.MemoryPointExists("crewcodriver"))
        {
            vector mpD  = quad.GetMemoryPointPos("crewdriver");
            vector mpCo = quad.GetMemoryPointPos("crewcodriver");
            isDriver = vector.Distance(localP, mpD) <= vector.Distance(localP, mpCo);
        }

        // 2) Which side? +X = left in the model frame; at exactly x = 0 we
        //    route to _L by convention (symmetric vehicles never care; for
        //    asymmetric vehicles this edge case is documented in this file).
        string role   = isDriver ? "driver" : "codriver";
        string suffix = (localP[0] >= 0) ? "_L" : "_R";
        string memPos = "pos_" + role + suffix;
        string memDir = "pos_" + role + "_dir" + suffix;

        // 3) If the memory points are missing, bail out and let vanilla run.
        if (!quad.MemoryPointExists(memPos) || !quad.MemoryPointExists(memDir)) return;

        // 4) Snap position + yaw. Keep the player's Y to avoid fighting
        //    terrain height.
        vector worldPos = quad.ModelToWorld(quad.GetMemoryPointPos(memPos));
        vector worldDir = quad.ModelToWorld(quad.GetMemoryPointPos(memDir));
        vector look = worldDir - worldPos;
        look[1] = 0;
        if (look.LengthSq() < 0.0001) return;  // dir == pos guard
        look.Normalize();
        float yaw = look.VectorToAngles()[0];

        vector newPos = worldPos;
        newPos[1] = worldP[1];
        player.SetPosition(newPos);
        player.SetOrientation(Vector(yaw, player.GetOrientation()[1], player.GetOrientation()[2]));
    }
}
```

(Full reference: `LFQuad_dev/task4_handoff/LFQuad_ActionGetInTransport.c`.)

## Why `super.OnStartServer` runs **first**, helper runs **second**

The vanilla path needs to advance the mount state machine; the helper just nudges the player into the right seat side AFTER the vanilla action has accepted the input. Snapping before `super` would teleport the player out of the action's raycast envelope and the climb animation would not advance. Snapping after lets the engine's own forced yaw eat the small residual rotation as a believable step into the seat (<30°), instead of swinging through 180+°.

On 1.30 `super.OnStartServer` / `super.OnStartClient` reach the empty `ActionBase` hooks (`actionbase.c:1388-1394`), so the order relative to `super` changes nothing there; what matters is that both hooks run before `StartCommand_Vehicle` (`actiongetintransport.c:73` then `:104`). The raycast-envelope explanation above is the LFQuad project's reading, not something the script shows.

## `config.cpp` still keeps the vanilla fallback

```cpp
class CfgVehicles {
    class LFQuad_Body: Car {
        class Crew {
            class Driver {
                getInPos    = "pos_driver";       // vanilla fallback
                getInDir    = "pos_driver_dir";   // used if our action is somehow not overridden
                action      = "ActionGetInTransport";
                attendant   = 0;
            };
            class CoDriver: Driver {
                getInPos    = "pos_codriver";
                getInDir    = "pos_codriver_dir";
            };
        };
    };
};
```

`pos_driver` (no suffix) still exists in the `.p3d` — it points to either the canonical center or the same as `pos_driver_L`. The `getInPos` / `getInDir` references are NOT removed: they are the fallback for any code path that bypasses the modded action.

## Bail-out for non-LFQuad vehicles

The override must check whether the target vehicle is one of yours before doing the snap. Otherwise every car on the server gets your dual-entry logic, which interferes with vanilla balance. The pattern:

```c
if (!vehicle || !vehicle.IsInherited(LFQuad_Body)) {
    super.OnStartServer(action_data);
    return;
}
```

`IsInherited` matches your mod's base class plus every subclass (paint variants, special editions). Use it; do not whitelist by class name.

## Cross-dependency — selection painter for `seat_*`

The modded action only works if the player can actually trigger the action from both sides. The action's raycast looks for a `seat_driver` / `seat_codriver` selection on the vehicle surface; if your selection only covers the top of the seat, the raycast from the side misses, and the player can't even invoke the action from the wrong side.

Mitigation: ensure the `seat_*` named selection covers **all visible faces of the seat** including the side panels of the tank/fender that a player can ray-cast at from outside the vehicle. The painter tool (`references/selection-painter-for-actions.md`) is what makes that practical — heuristic bounds typically miss the side faces.

## Edge case: player exactly at x = 0

The check `localP[0] >= 0` routes to `_L` when the player is exactly on the vehicle's center line (head-on or directly behind, perfectly aligned). For symmetric vehicles this is irrelevant — both sides snap to mirrored positions. For asymmetric vehicles (different seat heights or a single-side ingress), the bias toward `_L` may be wrong. Document the choice in your project's `assumptions.md` if you ever ship an asymmetric vehicle with this pattern. Switching the bias is a one-line change (`> 0` instead of `>= 0`).

## Memory point math — the mirror

Mirror in X (the vehicle's left/right axis): `pos = (-pos.x, pos.y, pos.z)`. The Z (height) and Y (forward) stay the same. If your vehicle is **not symmetric** (different seat heights or unusual offsets), measure both sides independently — do not blindly mirror.

If you change `pos_*_L` you must change `pos_*_R` to match, or one side feels different. Treat the four points as a unit: a single decision about where the player sits relative to the seat geometry.

## Validation

In-game:

1. Approach the seat from the LEFT and trigger "Subir" — player snaps onto the seat with <30° residual rotation, climb animation plays.
2. Approach the seat from the RIGHT and trigger "Subir" — same as above, mirrored. No 180° spin in the air.
3. Repeat for the codriver seat.
4. Approach a vanilla Sedan / Hatchback / Olga — vanilla get-in behavior is unchanged (bail-out branch).

If step 4 misbehaves, the bail-out check is wrong — likely you whitelisted a too-narrow class instead of `IsInherited(<your base>)`. Fix before shipping; breaking vanilla vehicles is a player-visible regression.

## Reference case

`LFQuad_dev/task4_handoff/LFQuad_ActionGetInTransport.c` and `LFQuad_dev/handoff_2026-05-28.md`. The pattern generalizes to any vehicle with bilateral ingress.

## DayZ 1.30: native directional in/out (alternative) [EXACT]

1.30 adds a native mechanism for directional get-in / get-out on single-track vehicles (motorcycles), which removes the need for scripted player snaps on vehicles implementing this contract. Before 1.30: `ActionGetInTransport.Start` always used `GetSeatAnimationType(crew_index)` (1.29: `actiongetintransport.c:90`), so the snap above was the only way.

### Engine flow in `ActionGetInTransport.c`
When `transport.HasDirectionalInOutAction()` returns `true` (`P:\scripts\3_game\vehicles\transport.c:680`), `ActionGetInTransport.Start` computes the approach side using the dot product between the target direction vector and the vehicle's right-normal vector:

```c
// [EXACT] P:\scripts\4_world\classes\useractionscomponent\actions\interact\actiongetintransport.c:81-102
		if (transport.HasDirectionalInOutAction())
		{
			vector fwd = transport.GetDirection();
			fwd[1] = 0;
			fwd.Normalize();
	
			vector playerPosition = action_data.m_Player.GetPosition();
			vector toTarget = transport.GetPosition() - playerPosition;
			toTarget[1] = 0;
			toTarget.Normalize();
	
			vector right = Vector(fwd[2], 0.0, -fwd[0]);
	
			float side = vector.Dot(toTarget, right);
	
			if (side < 0.0)
			    direction = 1;
		
			seat = transport.GetSeatAnimationTypeDirectional(crewIndex, direction);
		}
		else
			seat = transport.GetSeatAnimationType(crewIndex);
```

### Transport interface methods in `Transport.c`
```c
// [EXACT] P:\scripts\3_game\vehicles\transport.c:680-683
	bool HasDirectionalInOutAction()
	{
		return false;
	}
```
and:
```c
// [EXACT] P:\scripts\3_game\vehicles\transport.c:693-699
	int GetSeatAnimationTypeDirectional(int posIdx, int directionIndex)
	{
#ifndef CFGMODS_DEFINE_TEST
		Error("GetSeatAnimationTypeDirectional() not implemented");
#endif
		return 0;
	}
```

[DESIGN] For a custom vehicle (such as a quad or motorbike) on 1.30, overriding `HasDirectionalInOutAction()` to return `true` and implementing `GetSeatAnimationTypeDirectional(posIdx, directionIndex)` allows mapping left (`direction = 0`) and right (`direction = 1`) boarding animations natively through the animation graph (`GetIn_L` on `GetCommandI(CMD_Vehicle_GetIn) == 0`, `GetIn_R` on `== 1`, `MotorBikeSTM` in `P:\DZ\anims\workspaces\player\player_main\vehicles.agf:1582-1751`), serving as a clean alternative to the manual `pos_*_L/_R` snap script. Note that the motorbike states are reached only for `VehicleType` 10 or 11 (`vehicles.agf:1570`), so a four-wheeler that returns another anim instance keeps the car graph. Not measured on a custom vehicle.
