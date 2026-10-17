# AnswerOverflow community findings — Particles (mined 2026-05-17)

Source: DayZ Modders Discord (serverId 452035973786632194), via AnswerOverflow MCP. Cross-reference with the existing `script-api-reference.md` and `vanilla-particle-catalog.md` in this skill.

---

## PRT-1. `PlayOnObject` pattern with client guard — confirmed against vanilla

**[VERIFIED vs P:\scripts]** — `ParticleManager.GetInstance().PlayOnObject(...)` + `ParticleList.SMOKING_HELI_WRECK` confirmed at `3_game\particles\particlelist.c`, `4_world\entities\building\wrecks\wreck_mi8.c`, `4_world\entities\building\wrecks\wreck_uh1y.c`, and 20 other vanilla files.

Standard pattern for attaching a particle effect to an entity, gated to client-side only:

```cs
class Land_Wreck_C130J extends CrashBase
{
    void Land_Wreck_C130J()
    {
        if (!g_Game.IsDedicatedServer())
        {
            m_ParticleEfx = ParticleManager.GetInstance().PlayOnObject(
                ParticleList.SMOKING_HELI_WRECK,
                this,
                Vector(-3.51846, -2.09741, -5.57666)   // local offset from object origin
            );
        }
    }
}
```

Key points (matches existing `script-api-reference.md`):

- **Client-only.** Particles are client-side. Server should NEVER call `PlayOnObject` — wastes pool slots and renders nothing. Gate with `!g_Game.IsDedicatedServer()` in the original 1.29 snippet; on 1.30 use `!g_Game.IsHeadlessOrDedicatedServer()` (see below) (NOT `IsClient()` — see ENF rule about IsClient/IsServer in load).
- **`ParticleList` class constants** — use the named constants from `P:\scripts\3_game\particles\particlelist.c` (e.g. `ParticleList.SMOKING_HELI_WRECK`, `ParticleList.FIRE_SMALL_CAMP`, etc.). Don't hard-code integer IDs.
- **Offset vector** is in the parent object's local space.
- **Hold the return** — `ParticleSource` return value. Store as `m_ParticleEfx` so you can `Stop()`/`Delete()` it later in `EEDelete`/`OnDisconnect`.

Source: Leo/STimon (Land_Wreck_C130J) — https://www.answeroverflow.com/m/1504612151212511303

Before 1.30: vanilla wrecks used `if (!g_Game.IsDedicatedServer())` — the snippet above matches 1.29 `wreck_mi8.c:6`.
1.30: most vanilla wrecks, FireplaceBase, ParticleManager and boats gate with `if (!g_Game.IsHeadlessOrDedicatedServer())`
— `P:\scripts\4_world\entities\building\wrecks\wreck_mi8.c:6`, `wreck_uh1y.c:5`,
`P:\scripts\4_world\entities\itembase\fireplacebase.c:1112`, `P:\scripts\4_world\entities\vehicles\boatscript.c:382`,
`P:\scripts\3_game\particles\particlemanager\particlemanager.c:65` (`wreck_chinook.c:6` and `wreck_mi24_tk.c:5` were
not switched). An earlier digest citation `BoatScript.c:38` points at an enum, not the VFX guard.

[DESIGN] Keep the community snippet only as a 1.29 pattern. On 1.30 `ParticleManager.GetInstance()` returns null on a
headless client, so this snippet (gate `!IsDedicatedServer()`, no null-check) dereferences null there. New 1.30 code:

```cs
if (!g_Game.IsHeadlessOrDedicatedServer())
{
    ParticleManager pm = ParticleManager.GetInstance();
    if (pm)
        m_ParticleEfx = pm.PlayOnObject(ParticleList.SMOKING_HELI_WRECK, this, Vector(-3.51846, -2.09741, -5.57666));
}
```

---

---

## Mining metadata

- Server: DayZ Modders (id 452035973786632194), 14498 members.
- Particles-specific signal was thin (4 threads in this bucket, mostly reposts of vanilla patterns). The pattern above just confirms what `script-api-reference.md` already covers — useful as an external citation.
- Recommended: when documenting new particle patterns, use this finding as the citation template (HIGH confidence, full code, verified-vs-vanilla).
