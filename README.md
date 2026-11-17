# Emberfall

A tiny action RPG. The fires of Emberfall grow cold — slimes crawl from the
marshes, and the ancient **Ember Tyrant** stirs in the corrupted wastes to the
south-east. Slay, loot, level up, and save the village.

## Play the browser version

Open **`emberfall.html`** in any modern browser (Chrome, Edge, Firefox). No
install, no build, no assets — everything is generated in code.

## Features

- Procedural world (1000×1000) with **9 biomes**: Verdant Fields, Corruption, Cinder
  Caldera, Ashen Wastes, Frostpeak Tundra, Golden Desert, Murk Swamp, Fungal Hollow,
  Crimson Scar, Amber Savanna, Emerald Jungle, Rusted Badlands, Blossom Grove,
  Mistwood — each with its own terrain texture, props (pines, cacti, palms, cherry
  trees, crystals, mushrooms…), palette and monster table
- **24 enemy types** across surface & dungeons: slimes, skeletons, wraiths, necromancers,
  zombies, wolves, dire wolves, spiders, scorpions, yetis, sporecaps, imps, muck blobs,
  bandits, lava hounds, shades, vultures, hornets, apes, sprites, wisps, emberites…
  plus **mimics** — chest-monsters that ambush greedy looters (floor 2+)
- 5 dungeon floors of **real rooms and arenas** (pillared halls, plazas, vaults,
  boss arenas behind the Ember Seal) — no mazes
- **Ray-traced lighting**: walls cast real shadows, colored torch/lava/shrine light,
  line-of-sight checks — and sword swings can no longer cut through walls
- Potions with **Minecraft-style timed effects**: Speed, Strength, Regeneration,
  Fire Resistance (and Poison from monsters) with HUD timers
- **Inventory**: 27-slot satchel (9×3) + Fortnite-style quick bar, keys `1`–`5`,
  click-to-move stacks, `F` uses the selected item
- 5 bosses, shrine trials, hidden relic vaults, elites, chests, minimap, quests,
  synthesized audio

## Controls

| Key | Action |
|---|---|
| `WASD` / arrows | move |
| `Space` / left click | attack (click aims the swing) |
| `E` | talk / interact / advance dialog |
| `1–5` | select quick-bar slot (sword / bow / potions) |
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
