# NovaClient GD

A multi-feature client mod for Geometry Dash, built on the [Geode](https://geode-sdk.org/) framework.

- **ID:** `vinz.novaclient_gd`
- **Version:** `1.0.0`
- **Developer:** Vinz

## Features

**Creator/Cheats tab**
- **Noclip** — disables player death (hooks `PlayLayer::destroyPlayer`).
- **Show Hitboxes** — draws collision rectangles for the player(s) and nearby objects.
- **Instant Restart** — skips the death animation delay and resets the level immediately.

**Bot System tab (macro record & replay)**
- **Start/Stop Recording** — captures every `pushButton`/`releaseButton` event with its
  logical frame number, target player, and position into an in-memory vector.
- **Start/Stop Replay** — replays the recorded macro frame-for-frame by programmatically
  invoking `pushButton`/`releaseButton` on the correct player object at the correct frame.
- **Clear Macro** — wipes the recorded data.
- **Save/Load Macro** — persists the macro to a small binary file in the mod's save
  directory (`macro.novamacro`) so it survives between sessions.

**Credits tab** — mod & author info.

Open the menu via the toolbar button added next to the settings gear on the main menu,
or press **H** while inside a level.

## Project Layout

```
NovaClientGD/
├── CMakeLists.txt
├── mod.json
├── LICENSE
├── .gitignore
├── .github/workflows/build.yml
└── src/
    ├── main.cpp             # hook registration (MenuLayer, PlayLayer, GJBaseGameLayer, PlayerObject)
    ├── GlobalState.hpp       # shared toggle state (noclip / hitboxes / instant restart)
    ├── BotSystem.hpp/.cpp    # macro record/replay engine
    ├── HitboxRenderer.hpp/.cpp
    ├── NovaMenu.hpp/.cpp     # tabbed in-game UI popup
```

## Building locally

1. Install the [Geode SDK & CLI](https://docs.geode-sdk.org/getting-started/) and set the
   `GEODE_SDK` environment variable.
2. From the project root:
   ```sh
   geode build
   ```
3. The compiled `.geode` file will be under `build/`.

## Building via GitHub Actions (CI)

1. Zip up this project folder (or push it as-is) so the repository root contains
   `mod.json`, `CMakeLists.txt`, `src/`, and `.github/workflows/build.yml`.
2. Create a new GitHub repository and push the contents:
   ```sh
   git init
   git add .
   git commit -m "Initial commit: NovaClient GD"
   git branch -M main
   git remote add origin https://github.com/<your-username>/NovaClientGD.git
   git push -u origin main
   ```
3. Go to the **Actions** tab on GitHub — the `Build` workflow triggers automatically on
   push. It builds Windows, macOS, Android32, and Android64 targets, then a final
   **Combine** job merges them into a single universal `.geode` package.
4. Once the run finishes (green check), open the workflow run and download the
   `NovaClientGD.geode` artifact from the **Artifacts** section at the bottom of the
   summary page.
5. Copy/import the downloaded `.geode` file into your Geode `mods/` folder (or drag it
   into the Geode mod loader UI) to install it.

## Notes & caveats

- GD/Geode binding signatures (e.g. `PlayLayer::init`, `PlayerObject::pushButton`) can
  shift slightly between Geode SDK versions as RobTop updates the game. If you hit a
  compile error about a mismatched override signature, check the current bindings in
  your local `$GEODE_SDK` install (or run `geode sdk update`) and adjust the affected
  function signature in `src/main.cpp` accordingly.
- The `geode` version pin in `mod.json` (`3.9.0`) may need bumping to match whatever
  Geode SDK version the CI runner resolves — check the
  [geode-sdk/build-geode-mod](https://github.com/geode-sdk/build-geode-mod) action's
  README for the latest recommended pin.
- The macro bot works by replaying raw button inputs, so it is exact and deterministic
  as long as the level/mod environment (game version, active mods affecting physics,
  etc.) matches the one used during recording.
