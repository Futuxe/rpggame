# Emberfall

A tiny action RPG. The fires of Emberfall grow cold — slimes crawl from the
marshes, and the ancient **Ember Tyrant** stirs in the corrupted wastes to the
south-east. Slay, loot, level up, and save the village.

## Play the browser version

Open **`index.html`** in any modern browser (Chrome, Edge, Firefox). No
install, no build, no assets — everything is generated in code.

## Features

- Procedural world (1000×1000) with **9 biomes**: Verdant Fields, Corruption, Cinder
  Caldera, Ashen Wastes, Frostpeak Tundra, Golden Desert, Murk Swamp, Fungal Hollow,
  Crimson Scar, Amber Savanna, Emerald Jungle, Rusted Badlands, Blossom Grove,
  Mistwood — each with its own terrain texture, props (pines, cacti, palms, cherry
  trees, crystals, mushrooms…), palette and monster table. The main village
  always stands on Verdant Fields plains.
- **24 enemy types** across surface & dungeons: slimes, skeletons, wraiths, necromancers,
  zombies, wolves, dire wolves, spiders, scorpions, yetis, sporecaps, imps, muck blobs,
  bandits, lava hounds, shades, vultures, hornets, apes, sprites, wisps, emberites…
  plus **mimics** — chest-monsters that ambush greedy looters (floor 2+)
- 5 dungeon floors of **real rooms and arenas** (pillared halls, plazas, vaults,
  boss arenas behind the Ember Seal) — no mazes
- **Ray-traced lighting on every floor, surface included**: walls cast real shadows,
  colored torch/lava/shrine light, line-of-sight checks — dusk on the surface lights
  altar sigils, cottage windows and lantern-lit villagers. Sword swings can no longer
  cut through walls.
- **Fortnite-style potion chugging**: consumables open a drink channel (health flask
  1.6s, buffs 2.2s) — you can't move while drinking, and taking a hit cancels it
  without consuming the potion. Active effects show as stacked chips with icons and
  mm:ss timers above the health bar.
- **Health flasks are boss loot only** — no shop, chest or quest will sell you one;
  they drop from biome guardians and dungeon bosses.
- **Biome guardians with unlockable abilities** — four hidden altars (Frostpeak
  Tundra, Murk Swamp, Golden Desert, Cinder Caldera). Wake the guardian with `E`
  and slay it to claim its power: **Rime Dash** (`Shift`), **Deep Mend** (faster
  regen), **Dune Whirl** (`G`), **Magma Pierce** (piercing arrows).
- **Wandering gear vendors** — smiths camp beside every biome altar with one-of-a-kind
  relics (Rimeforged Blade, Serpentfang, Obsidian Plate, Magma Bowstring…), and
  Quartermaster Brom sells floor-scaled weapons and armor on every dungeon floor.
- **Bow with a real draw** — hold the shot 0.38s, then the arrow looses.
- **Item tooltips** — hover any quick-bar or inventory slot to read what it is.
- **Inventory**: 27-slot satchel (9×3) + Fortnite-style 5-slot quick bar at the
  top-right, keys `1`–`5` or click, `F` uses the selected item
- Hand-drawn-feeling sprites: caped hero, robed villagers with per-kind props,
  shingled huts with smoking chimneys and glowing windows, top-lit enemies
- 5 bosses, shrine trials, hidden relic vaults, elites, chests, minimap, quests,
  synthesized audio

## Controls

| Key | Action |
|---|---|
| `WASD` / arrows | move |
| `Space` / left click | attack (click aims the swing) |
| `Shift` | Rime Dash (after slaying Frostmaw) |
| `G` | Dune Whirl (after slaying the Dune Serpent) |
| `E` | talk / interact / awaken altars / advance dialog |
| `1–5` / click | select quick-bar slot |
| `F` | use the selected quick-bar item |
| `Q` | quick-drink a health potion |
| `I` | open the inventory (click to move stacks) |
| `P` / `M` | pause / mute |

## C version (native, raylib)

A native C port built with [raylib](https://www.raylib.com) is in progress
(`emberfall.c`) — pixel-art sprites baked at runtime, procedural audio, same
world and gameplay. Build:

```
gcc -O2 -o emberfall.exe emberfall.c -lraylib -lopengl32 -lgdi32 -lwinmm
```

## License

MIT
