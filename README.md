# Emberfall

A tiny action RPG. The fires of Emberfall grow cold — slimes crawl from the
marshes, and the ancient **Ember Tyrant** stirs in the corrupted wastes to the
south-east. Slay, loot, level up, and save the village.

## Play the browser version

Open **`emberfall.html`** in any modern browser (Chrome, Edge, Firefox). No
install, no build, no assets — everything is generated in code.

## Features

- Procedurally generated world: lake, beaches, forests, a village and a corrupted wasteland
- Action combat: sword swings with knockback, arrows, hit flashes, screen shake
- 3 enemy types + a boss (**the Ember Tyrant**) with telegraphed slam attacks and an enrage phase
- Leveling (XP, +HP, +damage), gold, potions, gems, magnet-style loot pickup
- Quest chain from Elder Maren, a trading post run by Finn (potions, blade, armor)
- Minimap, quest tracker, boss health bar, floating damage numbers, particles
- Synthesized sound effects and ambient music (Web Audio)

## Controls

| Key | Action |
|---|---|
| `WASD` / arrows | move |
| `Space` / left click | attack (click aims the swing) |
| `E` | talk / interact / advance dialog |
| `1–9` | pick dialog options |
| `Q` | drink health potion |
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
