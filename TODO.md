# TODO — Resume Emberfall v2

> **Note to future session (opencode):** the user asked you to FINISH THIS when they are home.
> Read this file fully, then continue exactly where things stopped. Repo: `git@github.com:Futuxe/rpggame.git`
> (SSH key already set up on this machine at `~/.ssh/id_ed25519`, portable git at
> `C:\Users\Admin\AppData\Local\Temp\opencode\gitportable\git\cmd\git.exe` — set `$env:HOME=$env:USERPROFILE` before calling it.)

## Status

- ✅ `emberfall.html` — v1 playable browser game, live on GitHub (commit 9c86baf)
- 🚧 **v2 upgrade in progress** — user request: "fix movement bugs and make it bigger: more levels, more enemies"
- 🚧 `wip/emberfall-v2-part1.html` — PART 1 of the v2 rewrite is DONE (core engine, audio, surface + dungeon worldgen, collision). It is a standalone partial file ending with a `PART2_PLACEHOLDER` script tag — the parts were meant to be concatenated into the final `emberfall.html`.

## What v2 adds (agreed design)

### Movement fixes
1. Clear `keys{}` + `mouse.down` on `window blur` (stuck keys after alt-tab)
2. Block player movement & attacks while `state.dialog` is open
3. Camera centering when viewport is larger than the map:
   `maxX=mapW*TS-VW; if(maxX<=0) tx=(mapW*TS-VW)/2 else clamp(...)` (same for Y)
4. NPCs + unopened chests are solid (already in part1's `circleFree`)
5. Wall-slide assist in `moveEntity` (already in part1: retry at 35% speed)
6. Hold-mouse = continuous attacks toward cursor
7. Spawn via `findFreeSpot()` after every floor transition (already in part1)

### The Ember Depths (dungeon)
- 3 procedural floors via `genDungeon(fl)` in part1 (rooms + wide L corridors, 60+fl*6 tiles)
- Floor 3 has a boss arena (20x14) and the **Ember Tyrant** as final boss (moved off the surface)
- Surface corrupt-waste center now has `T_STAIRSDOWN` + purple portal swirl (`entrance`), leads to Floor 1
- `T_STAIRSUP` in start room of each floor; press E on stairs → fade transition (`state.trans={t,phase:'out'|'in',cb}`)
- Floor 2+: lava pools (`T_LAVA`) — burn the player (8 dmg tick, reuse hurtPlayer), enemies refuse to step in unless flying (bats)
- Wall torches (`torches[]`) + darkness overlay on floors ≥1:
  offscreen light canvas, fill `rgba(2,2,10,0.88)`, punch holes with `destination-out` radial gradients
  at player (r≈190+45*lightLvl), torches (r≈140 flicker), lava, drops, boss
- Chests: `chests[]`, E to open near one → coins/potion/gem burst; remember opened via `openedChests` Set keyed `c.id`
- Merchant sells "Lantern" upgrade (price 80*2^lvl, max 3) → bigger light radius

### New enemies (add to EDEFS table in part2)
| type | hp | spd | dmg | xp | notes |
|---|---|---|---|---|---|
| bat | 18 | 135 | 6 | 10 | erratic sine strafe, flies over lava, r=8 |
| golem | 170 | 38 | 22 | 65 | knockback resist 0.25, r=14 |
| necro | 72 | 55 | 12 | 55 | keeps 140–240 range, casts purple bolt (enemy arrow), summons ≤2 skeletons (tag `summonedBy`) |
- **Elites**: 8%+2%/floor chance → scale 1.4, hp×2.2, dmg×1.5, xp×2.5, purple aura, guaranteed potion+gem drops
- Spawn tables per floor: F1 bat/dark slime/skeleton, F2 skeleton/bat/golem/wraith, F3 wraith/necro/golem
- Pre-populate 8+3*floor enemies on floor load, away from start room

### Rendering (part3 work)
- New tiles: `T_WALL` (bricks + lighter top edge when floor above), `T_FLOOR` (jitter+pebbles), `T_LAVA` (animated), `T_STAIRSDOWN` (dark hole+steps), `T_STAIRSUP`
- Depth-sorted drawables now include: trees (surface), buildings, torches, chests, portal swirl, NPCs, enemies, player, boss
- Minimap colors for new tiles; HUD floor label ("Surface — Emberfall" / "The Ember Depths · Floor N")
- Victory: after boss dies, spawn return portal at arena center (E → back to village), Elder turn-in unchanged

## Remaining steps
1. Write part 2 (state, quests/dialogs incl. lantern option, enemy AI for bat/golem/necro, update loop, floor transitions)
2. Write part 3 (rendering + HUD + darkness + screens + main loop)
3. Concatenate: part1 (replace the `PART2_PLACEHOLDER` marker) + part2 + part3 → overwrite `emberfall.html`
4. Test in browser (console check), fix bugs
5. Also still owed: native **C/raylib port** (`emberfall.c`) — raylib 6.2 win64 mingw bundle download looked truncated (1.7 MB), re-download before building
6. Commit + push everything to `Futuxe/rpggame`

— created by opencode, to be finished when the user is home 🎮
