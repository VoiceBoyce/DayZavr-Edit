# Terrain holes: evidence (DayZ 1.30)

> Target: DayZ 1.30 stable (1.30.164078). Labels: ../../_shared/dayz-version-policy.md.

First measured 2026-09-24 comparing the 1.30.164.014 Exp install with 1.29 stable;
re-checked on stable 1.30.164078 on 2026-10-15 (`P:\` unpack, stable `DayZDiag_x64.exe`
1.30.164.078). What was re-checked says so; the RVAs and the disassembly window are from
the Exp build only. Nothing has been tested in game.

## How to reproduce it

1. On 1.30 the unpacked data is already on `P:\` (`P:\scripts`, `P:\DZ\worlds\<world>\world\config.cpp`,
   `P:\DZ\worlds\<world>\ce\`); 1.29 is on `O:\DayZ 1.29 Старый распакованный клиент\`.
   Otherwise extract `dta\scripts.pbo`, `Addons\worlds_enoch.pbo`, and
   `Addons\worlds_chernarusplus.pbo` (any PBO extractor; entries are uncompressed) and
   convert each `config.bin` with `CfgConvert.exe -txt` (DayZ Tools).
2. Diff the two versions.
3. Search for strings in `DayZDiag_x64.exe` in both versions (ASCII and UTF-16) and diff them.
4. To see which code uses each string, search for `lea`/`mov r64, [rip+disp32]` instructions
   in `.text` that point to its RVA and list other strings referenced in a window
   of ±0x600 bytes. The RVAs are specific to each build.

## New engine strings

In `DayZDiag_x64.exe` 1.30 four related strings appear that are not in the 1.29 executable
(remaining occurrences of "hole" are from Recast, the navmesh generator, and already
existed). Stable 1.30.164.078 still contains `\holes.cfg`, `SurfaceIsHole` and both
`out of range` messages (grep 2026-10-15; the retail `DayZ_x64.exe` also contains
`\holes.cfg` and `SurfaceIsHole`). The table below is the **Exp 164014** analysis; its RVAs
were not re-measured on stable:

| String | Where used (reference RVA) | Nearby strings in that function |
|---|---|---|
| `\holes.cfg` | 0x792cb4 | `CfgWorlds` (+1429), `DefaultWorld` (+1474): world loading |
| `tiles` | 0x6655e3 | `Holes` (+1308) |
| `Holes` | 0x665aff | `tiles` (−1308), `y (%d) out of range <0, %d)` (+1349), `x (%d) out of range <0, %d)` (+1374) |
| `Holes` | 0x71b1f2 | `minTreesInForestSquare` (−1358), `minRocksInRockSquare` (−1203): `CfgWorlds` parameters |
| `SurfaceIsHole` | 0xc20528 | native table of `CGame` (`SurfaceY`, `SurfaceRoadY`, `GetSurfaceInfoOn`…) |

In `.rdata`, `\holes.cfg` sits right before loading screen messages ("Preparing
surface materials", "Extruding hills and valleys"…) and right after Buldozer actions
`UABuldMarkUnderground` and `UABuldRemoveUnderground`. The stable executable keeps the
same neighbourhood: `UABuldRemoveUnderground`, `UABuldMarkUnderground`,
`UABuldTextureInfo`, "Config for island %s not found. Using DefaultWorld instead",
`\holes.cfg`, "Preparing surface materials", "Extruding hills and valleys" (grep 2026-10-15).

Interpretation, marked as inference: code iterates through `Holes`, reads `tiles`, and validates each
`x`/`z` against the grid size; `Holes` is also read as a world parameter in
`CfgWorlds`; and a path exists that loads a `holes.cfg` file when loading the world. It has not
been disassembled to see how that path is constructed.

## Livonia config: 1.29 vs 1.30

`P:\DZ\worlds\enoch\world\config.cpp` (1.30.164078) against
`1.29: O:\…\DZ\worlds\enoch\world\config.cpp`. Differences:

- New `class Holes` inside `CfgWorlds > Enoch`, between `OutsideTerrain` and `Grid`, with
  a single `Dambog` group of seven cells (`P:\DZ\worlds\enoch\world\config.cpp:303-318`;
  the block is in `SKILL.md`).
- New `hasOcean=0` in the same world class (`:409`).

`P:\DZ\worlds\chernarusplus\world\config.cpp` contains neither `Holes` nor `tiles`.
The Sakhal world is in `sakhal\addons\worlds_sakhal.ebo` and Nasdara's in
`nasdara\addons\worlds_nasdara.ebo`, both encrypted; neither could be read (there is no
`P:\DZ\worlds\sakhal\world\`, and Nasdara is not unpacked on `P:\`).
`P:\DZ\worlds\enoch\ce\cfgundergroundtriggers.json` is byte-for-byte identical to 1.29
(`cmp`, 2026-10-15): holes did not arrive with new triggers.

## Cell size in Livonia: 6.25 m

Livonia measures 12,800 m. With 2048 cells, each measures 6.25 m. Position of `Dambog`
cells according to assumed size, versus vanilla Dambog underground triggers
(`P:\DZ\worlds\enoch\ce\cfgundergroundtriggers.json`, identical in 1.29 and 1.30, 8 triggers
at X 584.6–749.7 and Z 1131.0–1229.1; re-checked 2026-10-15):

| Cell group | With 5 m | With 6.25 m | With 10 m |
|---|---|---|---|
| `{118, 195–197}` | X 590–595, Z 975–990 | X 737.5–743.75, Z 1218.75–1237.5 | X 1180–1190, Z 1950–1980 |
| `{94–95, 180–181}` | X 470–480, Z 900–910 | X 587.5–600, Z 1125–1137.5 | X 940–960, Z 1800–1820 |

Triggers matched by the 6.25 m column:

- Main entrance: triggers at (749.7; 533.5; 1228.5) and (735.0; 533.7; 1229.1), measuring
  15 × 5.6 × 10.8 m.
- Second entrance: trigger at (593.7; 588.75; 1131.8), measuring 11 × 22.3 × 11.6 m.

With 5 m and with 10 m neither group falls within the range of the triggers. The fit of two
independent groups is what supports the `cross_checked` level. The header of `.wrp`
(OPRW v32) was not parsed (its first fields are not the plain OPRW grid header), and cell
size of other maps is not measured.

## The `.wrp` changes identically with and without holes

Sizes and versions re-checked on stable: `P:\DZ\worlds\<world>\world\<world>.wrp` (1.30.164078,
same sizes as on Exp 164014) and `1.29: O:\…\DZ\worlds\<world>\world\<world>.wrp`.

| Map | 1.29 | 1.30 | Difference | Has `Holes`? |
|---|---|---|---|---|
| ChernarusPlus | OPRW v29, 223,653,986 B | OPRW v32, 235,121,528 B | +5.1% | no |
| Enoch (Livonia) | OPRW v29, 226,053,187 B | OPRW v32, 237,263,513 B | +5.0% | yes |

Both advance to v32 and grow by the same amount even though only one has holes: the `.wrp` change
is format-related. This supports holes living in the config; it does not rule out `.wrp`
also storing something related (for example, the "underground" flag per object).

## DayZ Tools (2026-10-15 update)

`<Steam>\steamapps\common\DayZ Tools\Bin\TerrainBuilder\terrainBuilder.exe`, updated
2026-10-15 (Steam build 25460346), string grep:

- Present: "Compute objects that are fully underground", "Attempting to compute
  underground flag for %d objects.", "Underground Objects: %d … Overground Objects: %d",
  `CBuldozerEnvelope::MessageMarkUnderground`, and "This file seems to be missing the
  underground flag on line %d. The importing will still happen, but please re-export all
  the files with the missing flag as they will become unsupported in the near future."
- Absent: any `hole`/`Holes`/`holes.cfg` string.

So the per-object underground flag is a Terrain Builder feature (an object-import column;
missing → warning for now), and holes are not authored in Terrain Builder. The April 2026
Tools had neither (measured 2026-09-24).

## External sources

- **[CHANGELOG 1.30] (Exp 1.30.164014 changelog).** The forum post is at
  `forums.dayz.com/topic/266385`, behind Cloudflare; copy from mydayz.eu was read.
  - MODDING section adds "Terrain holes", without further detail.
  - TERRAIN BUILDER section adds Buldozer camera untethered from terrain to
    edit below surface (key 8), an input to tether and untether it, having that
    toggle affect objects as well, and "underground" flag per object against
    occlusion. Key 8 is confirmed on stable by `UABuldLinkCamToTerrain` → `k8`
    (`P:\bin\preset_x1mousekey.xml:1058-1060`); the per-object flag by the Terrain Builder
    strings above.
- **Dev Blog Recap 2 (Steam announcement, 2026-09-22).** Among what arrives with Badlands:
  underground facilities and infrastructure with new terrain holes system,
  also available for other maps and modding community, and radio signals
  in Morse leading to underground shelters.
- **Flynn's Terrain Tools [reported: FTT guide] (third-party,
  `github.com/Naviata/FTT-Releases`, 2026-09-19/20).**
  Only publishes binaries, without source code. According to its guide `guide/holes.md`:
  - its "Hole Generator" marks heightfield grid cells;
  - writes `holes.cfg` next to `layers.cfg`, with one class per group;
  - adds `#include` to terrain `config.cpp`;
  - Buldozer rereads file on each alt-tab;
  - its general guide warns that a newer Terrain Builder requires an "underground"
    flag column in object imports (consistent with the 2026-10-15 Terrain Builder
    warning above, which still imports files without it).

  Apart from that column, none of this was verified here. Its binary was not run.
