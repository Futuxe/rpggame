# TODO — Emberfall v3.1

## SHIPPED v3.1 (this session)
- ✅ **Fortnite-style potion chugging** — consumables open a drink channel
  (health flask 1.6s, buff potions 2.2s). You cannot move while drinking,
  taking a hit cancels the channel and the item is NOT consumed. Progress
  floods the hotbar slot + a progress ring above the hero's head.
- ✅ **Effect chips with icons + mm:ss timers** stacked above the health bar
  (REGEN 0:22 style), including the drink channel chip and ability chips
  (SHIFT dash cooldown, G whirl cooldown, MEND, PIERCE).
- ✅ **5-slot quickbar top-right** (Fortnite-style), keys 1–5 + LMB; clicking a
  slot selects it and chugs consumables; minimap moved beneath it.
- ✅ **Biome guardians + unlockable abilities** — four hidden altars stamped into
  the world (Frostpeak Tundra, Murk Swamp, Golden Desert, Cinder Caldera ring).
  Press E at an altar to wake its guardian:
  - FROSTMAW, THE RIME TYRANT → **Rime Dash** (SHIFT)
  - THE BOG QUEEN → **Deep Mend** (regen after 15s idle, +2 HP/s)
  - THE DUNE SERPENT → **Dune Whirl** (G, 360° sweep)
  - THE MAGMA TYRANT → **Magma Pierce** (arrows pierce every foe in line)
  Guardian kills do NOT trip the floor seal; each altar shows done on the minimap.
- ✅ **Bow draw delay** — pulling the string takes 0.38s before the arrow looses;
  the hero visibly holds the warden bow, nocks and draws an arrow.
- ✅ **Health flasks are boss-only loot** — removed from the merchant, chests,
  relic vaults, shrines, regular enemies, mimics and quest rewards. Merchant
  replaced the health slot with Fire Resist; quests now reward buff potions.
- ✅ **Item hover tooltips** — hovering any hotbar or inventory slot spells out
  the item name + what it does (every IDEF got a `desc`).
- ✅ **Wandering gear vendors** — a smith camps beside every biome altar
  (Rimeforged Blade +14, Serpentfang +18, Bogward Charm, Obsidian Plate,
  Magma Bowstring… one-of-a-kind relics + standard sharpening), and
  Quartermaster Brom sells floor-scaled gear on every dungeon floor.
- ✅ **Village always on plains** — the climate picker no longer overrides tiles
  within the village radius+12; Emberfall village is always Verdant Fields.
- ✅ **Richer sprites** — hero got a cape, gradient tunic, belt, stepping boots,
  skin shading; villagers got robes/hems/arms, elder's hood + staff, merchant's
  sash + coin pouch, smiths' packs + bandanas; huts got plank walls, shingled
  roofs, warm glowing windows, framed doors and smoking chimneys; every enemy
  gets a top-light sheen pass for volume.
- ✅ **Finished the ray tracing on the surface** — drawDarkness used to bail on
  floor 0; now the surface gets a dusk fog with the full ray-traced pipeline
  (LOS-verified visibility polygons): flickering torches, lava glow, altar
  sigils, lit cottage windows, lantern-lit villagers, chests, elites and bosses.
- ✅ Fixed pre-existing crash: `makeAshTex` was missing → ReferenceError that
  froze rendering whenever Ashen Wastes tiles came on screen.
- ✅ Fixed bow draw reading the dead `player.arrows` field (now `countItem`).
- ✅ Fixed inverted pierce logic that despawned every arrow instantly.
- ✅ Tested in Chromium: drink/lockout/interrupt, bow draw+fire, all four
  guardian altar→boss→ability flows, vendor purchase + one-time flags,
  dungeon vendor, tooltips, surface lighting — 0 console errors.

## SHIPPED v3 (previous session)
- 14 zones/biomes driven by a climate field; 20+ mobs with new AI patterns;
  room-based dungeon floors; LOS-checked sword swings; Minecraft-style timed
  potion effects; 27-slot inventory + 5-slot hotbar.

## Still open
- Native **C/raylib port** (`emberfall.c`) — raylib 6.2 win64 mingw bundle
  download looked truncated (1.7 MB), re-download before building.
- `wip/p01–p20.html` parts are historical only — `index.html` is the game.

> **Note to future session (opencode):** `index.html` is the single source of
> truth (~5000 lines, self-contained). Drink channel = `startDrink/finishDrink/
> cancelDrink` + `player.drink`. Abilities = `player.abilities` +
> `state.biomeAltars`/`state.biomeDead`. Vendors = `npcs` with `kind:'vendor'`
> + `stock[]` + `vendorDialog()`. Surface lighting lives in `drawDarkness`
> (`const surface=state.floor===0`). Repo: `git@github.com:Futuxe/rpggame.git`
> (SSH key at `~/.ssh/id_ed25519`).

— created by opencode, to be finished when the user is home 🎮
