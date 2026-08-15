# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

"Powerslide Remake" is a C++ remake of the 1998 arcade racer *Powerslide*: 3D off-road racing with neural-network AI and LAN/web multiplayer. Built on Ogre 1.9 (static, OpenGL), cross-platform (Linux, Windows, Android/GLES2). The engine reads the **original game's binary asset files** (`.pf` packed files) directly rather than shipping converted assets.

Version 1.21. MIT licensed.

## Build & Run

Requires a **pre-built static Ogre 1.9** (OpenGL render system) and (optionally) OpenAL Soft. Dependencies (SFML, Lua, SimpleINI, JSON++, ogg/vorbis, multislider) are vendored as subdirectories and built by the top-level `CMakeLists.txt`.

```bash
# Configure — PowerslideRemake_OGREHOME must point to your static Ogre build
# (containing CMake/ and lib/OGRE/cmake/). OpenAL is on by default.
cmake -S . -B build \
  -DPowerslideRemake_OGREHOME=/path/to/ogre-1.9-static \
  [-DPowerslideRemake_OPENAL=OFF] \
  [-DPowerslideRemake_OPENALDIR=/path/to/openal]

cmake --build build -j
# Installs the executable to the repo root (CMAKE_INSTALL_PREFIX = source dir)
cmake --install build
```

Local machine setup (already configured): the build tree lives at `~/PowerslideRemake/build` with `-DPowerslideRemake_OGREHOME=/home/dm/ogre/bin`. Rebuild with `cmake --build ~/PowerslideRemake/build -j` then `cmake --install ~/PowerslideRemake/build`. The only warnings produced are benign: deprecated `std::binary_function` in `SimpleIni.h` and `tmpnam` in `loslib.c` (vendored Lua).

CMake options:
- `PowerslideRemake_OPENAL` (default `ON`) — builds `src/sound/*` and links OpenAL + ogg/vorbis + sfml-audio. When `OFF`, defines `NO_OPENAL` and all sound code is excluded via `#ifndef NO_OPENAL`.
- `PowerslideRemake_OGREHOME` — path to static Ogre includes & libraries.
- `PowerslideRemake_OPENALDIR` — path to OpenAL (only when OpenAL ON).

The application executable is `ogre/CMakeLists.txt`'s `PowerslideRemake` target (built as a shared lib on Android, executable elsewhere). It uses a precompiled header (`ogre/src/pcheader.h`).

There are no tests or linters wired into the build. `gtest/` and `tools/pfunpacker` exist but are only enabled under MSVC and are commented out.

### Running

Original game assets are required at runtime (from CD or GOG): `data.pf`, `gameshell.pf`, `store.pf` (`patch.pf` optional). Place them next to the executable. `resources.cfg` maps the `[PF]` resource group to `.` (cwd), so the binary must be run from the repo root.

```bash
./run.sh   # sets LD_LIBRARY_PATH=. and runs ./PowerslideRemake
```

Optional music: drop `track_0.ogg` (main theme) and `track_2`–`track_8.ogg` next to the `.pf` files (`track_1`/`track_9` unused). Only with OpenAL on.

Runtime save files (encoded, gitignored but present locally): `player.str`, `hiscores.str`, `hiscores.dat`.

## Architecture

Entry point is `ogre/src/Main.cpp` (on Android it exposes JNI `OgreActivityJNI`). The object graph:

```
BaseApp (Ogre FrameListener / WindowEventListener)
 ├── GameState            — central state: loads original data, holds player/hiscores/track params
 ├── InputHandler         — OIS keyboard/mouse/touch → actions
 ├── DMLuaManager         — Lua pipeline (assets/scripts/*.lua) bound to C++ via parseFile()
 ├── SoundsProcesser / MusicProcessor  — OpenAL, #ifndef NO_OPENAL
 └── GameModeSwitcher     — owns the active game mode, drives frame callbacks
      ├── MenuMode / MenuMultiMode      (inherit BaseMenuMode)
      └── SinglePlayerMode / DeathmatchMode / MultiPlayerMode / championship / timetrial (inherit BaseRaceMode)
```

All concrete modes derive from `BaseMode` (`ogre/src/gamemodes/`), which defines the `initData / initCamera / clearData / frameStarted / frameRenderingQueued` lifecycle. `GameModeSwitcher` transitions between `GameMode` enum values (`ModeMenu*`, `ModeRace*` in `Enums.h`) and forwards Ogre frame events to the active mode. `ModeContext` is the struct that threads `BaseApp`/`GameState`/`InputHandler`/tray manager/Lua state/sound processors into every mode.

### Key subsystems (all under `ogre/src/`)

- **`loaders/`** — parsers for original Powerslide binary formats. `PFLoader` reads the `.pf` packed-file filesystem (returns `Ogre::DataStreamPtr` for a given virtual path). `STRLoader` decodes XOR-encoded `.str` strings; `OriginalSettings` wraps `.str`/`SimpleINI` parsing for game settings. `TerrainLoader`, `PHYLoader`, `AILoader`, `DE2Loader` (mesh), `TRALoader`, `TEXLoader`/`LUTs` (HKLUT texture upscaling), `SUSLoader`, `ParticlesLoader`, etc. each parse a specific original asset type. New asset-format work belongs here.
- **`physics/`** — custom vehicle physics (not Bullet). `Physics` steps the world; `PhysicsVehicle`/`PhysicsWheels`/`PhysicsBody`/`PhysicsRoofs`/`TerrainData` model the car. `InitialVehicleSetup` holds per-vehicle starting config. See `docs/physics/` for the reference papers.
- **`pscar/`** — vehicle hierarchy: `PSBaseVehicle` → `PSBaseCar` → `PSControllableCar` → `PSPlayerCar`; plus `PSAICar`, `PSMultiplayerCar`, `PSBaseGraphicsVehicle`, `PSCarEngine`.
- **`gamelogic/`** — `Championship`, `LapController`/`LapUtils`, `RacingGridGeneration`, `AIUtils`, `TrialGhost`, `FinishBoard`.
- **`ai`** — neural-network driving AI; data loaded via `AILoader` (see `docs/ai/`). `AIUtils` also owns the **anti-stuck** subsystem (see [AI stuck detection below](#ai-stuck-detection-and-reverse-recovery)).
- **`ui/`** — MyGUI/SdkTrays-based menus and race HUD (`UIBase` → `UIMainMenu`, `UIRace`, multiplayer variants, `UIBackground`). `ui/elements/` and `ui/routines/`. The tacho dashboard (`UIRace`) supports 3-digit position/total displays for massacre (see [Race HUD 3-digit tacho below](#race-hud-3-digit-tacho-massacre)).
- **`multiplayer/`** + top-level **`multislider/`** — networking. `MultiplayerController` splits into `Master`/`Slave`; the `multislider/client` library wraps SFML network (TCP 8800 lobby, UDP 8800 room list, 8700 race data). `multislider/server` is a separate deployable web/lobby server.
- **`lua/`** — `DMLuaManager` binds C++ functions into the Lua state. `assets/scripts/Main.lua` is the entry script; it calls `parseFile("vehicle.lua")` / `parseFile("Scene.lua")` to configure the rendering pipeline (terrain material/shader selection, mesh options). Lua scripts are loaded as the Ogre `[Lua]` resource group (see `resources.cfg`).
- **`cheats/`** — in-game cheats (nitro, bomb, burn, sticky, spider, etc.), triggered from `BaseApp::enable*` / `InputHandler`. `CheatBomb`/`CheatBurn` carry an `mIsDeathmatch` flag (set via `Cheats::setDeathmatch`) that switches their blast into a life-draining weapon in deathmatch (see [Deathmatch mode](#deathmatch-mode)).
- **`customs/`** — custom Ogre pieces (`CustomOverlaySystem`, `MovableText`, `CustomSceneManagerFactory`, `CustomTrayManager`).
- **`includes/`** — shared precompiled-header-friendly includes (`OgreInclude.h`, `OISInclude.h`, `CommonIncludes.h`).

`GameState` is the single source of truth for loaded original data: it owns four `PFLoader` instances (data/gameshell/store/patch) and the parsed `.str` structs (`STRPowerslide`, `STRRacecrud`, `STRRacetimes`, `STRHiscores`). Most subsystems receive a `GameState&` and read original data through it.

### Android

`Main.cpp` wraps everything in JNI (`com.powerslide.remake.OgreActivityJNI`); the Android build links GLES2 and reads `.pf` files from `<powerslide>` in external storage. `#if defined(__ANDROID__)` guards are common throughout (touch input, texture reload, storage paths).

### Deathmatch mode

A last-car-standing mode (`DeathmatchMode : public SinglePlayerMode`, in `gamemodes/`; enum `ModeMenuDeathmatch`/`ModeRaceDeathmatch` in `Enums.h`; menu entry wired in `UIMainMenu`/`UIMainMenuLabels`). It reuses the single-race pipeline — no separate physics or rendering path — and gates every behavioral difference behind one flag, so normal modes are byte-for-byte unaffected.

- **Master gate: `GameState::isDeathmatch()` / `setDeathmatch(bool)`.** Set in `DeathmatchMode::initData` (before `BaseRaceMode::initData`) and cleared in `clearData`. Every deathmatch-specific code path checks this flag (or a downstream `mIsDeathmatch` propagated to `Cheats`/`CheatBomb`/`CheatBurn` via `Cheats::setDeathmatch`). When adding deathmatch behavior, gate it here rather than on the `GameMode` enum.
- **Life/death system.** `PhysicsVehicle::mLife` starts at `1.0` and never changes in normal modes. In deathmatch it is drained by several sources, all flag-gated: collision damage (`Physics::processCarsCollisions` — the player is never the victim; AI-vs-AI only bleeds when the victim is already low), bomb/burn blasts (`CheatBomb`/`CheatBurn`, both skip the player), and a self-destruction bleed (`Physics::internalTimeStep` when `life <= 0.5`). **Death is unified at one chokepoint**: `PhysicsVehicle::timeStep` checks `mLife <= 0.0f`, and on the first such step fires a one-shot explosion impulse, calls `mPhysics->onCarDead(this)` (broadcasts to `PhysicsListener::carDead`), and hides the car's wheels. Any new source of damage only needs to drain `mLife`; the death FX/callback follow automatically.
- **Session end.** `DeathmatchMode::carDead` (overrides the base no-op) decrements `mAliveCars` and, when only the player remains (`mAliveCars <= 1`), triggers the same finish sequence as a final-lap single race — fixed finish camera, hidden rear-view mirror, finish sign, `resetAfterFinishTimer()`, `setRaceFinished(true)`. The existing `GameModeSwitcher` teardown (`ModeRaceDeathmatch → ModeMenu` / podium, after a 10 s countdown) then runs unmodified. The player cannot die in deathmatch (no source drains player life), so the survivor is always the player.
- **Input allowlist.** `BaseApp::enable*` / cheat creation functions (`createBombByPlayer`, `createBurnByPlayer`, `switchRenderType`, etc.) check `getMode() == ModeRaceDeathmatch` alongside the other race modes to control which actions are permitted during a deathmatch.
- **Per-vehicle death FX** (black smoke + orange embers) in `PSControllableCar`, gated on `isDeathmatch()` and null-guarded.
- **Massacre sub-mode** — a deathmatch where the field is built in *batches* from a "Racing grid batches" slider (`GameState::mBatchesMin=1` to `mBatchesMax=25`), gated by `GameState::isMassacreEnabled()`. The opponents slider is ignored; the AI count is always `12 × batches − 1`. At 25 batches this hits the `mAIMax=299` ceiling.
  - **Batch size**: `mBatchSize = mRaceGridCarsMax = 12` (12 AI per batch). The last batch is uneven — 11 AI + the **player** fills the 12th slot.
  - **Launch sequence** (`BaseRaceMode::beforeStartSequence`): single-grid case (batches=1 → 11 AI) releases player + all AI at GO. For batches ≥ 2, GO releases only the first 12 AI; remaining batches release on a timer compressed for large fields (`batchTime /= 1.5` at 100 cars, again at 200). The player rides in the last batch (`batchAmount = (aiCount + 1) / mBatchSize`), always at the very back.
  - **PHYLoader cloning**: the original game ships only 12 `.phy` files. Slots beyond `mRaceGridCarsMax` clone an existing `.phy` entry with `mTrackPosition[0][7] += 16.0f` cumulative stagger, so every car has its own grid position.
  - **AI stuck guard**: batched cars are parked on the grid (physics `integrate()` gated on per-vehicle `mIsRaceStarted`). The anti-stuck detector is guarded on `physicsAICar->getRaceStarted()`, so parked batched cars don't trigger false stuck detection.
  - **Engine/limit details**: `mAIMax=299` (non-Android), `mRaceGridCarsMax=12`. `PSAICar mPSCar` is `std::vector<PSAICar>` sized at construction. RacingGridGeneration rewrites its hardcoded 12-permutation path to fill `0..aiCount-1` for anything above 12. Dashboard car icons (`mDashboardCars`) are resized to `aiCount` at dashboard load. The deathmatch results table and in-race AI dashboard icons are capped at 12 (acceptable for massacre). The tacho pos/total display supports up to 300 cars with 3-digit digits (see [Race HUD 3-digit tacho](#race-hud-3-digit-tacho-massacre)).

### AI stuck detection and reverse recovery

All AI (every mode, not deathmatch-only) carry an anti-stuck system in `AIUtils::performAICorrection` (state: `mTimerAIStuck`, `mTimerReverse`, `mIsReverseEnabled`, `mAIDistanceLength`, `mPrevPos`). The original Powerslide logic sat `#if 0`'d out for years; it was restored and modernized (branch `ai-stuck-recovery`, merged into `deathmatch-massacre`). Key facts a future editor needs:

- **Detection** — every AI step, the step's displacement is projected onto the car's *live* flat heading and its magnitude accumulates. If a 2000 ms window expires with < 20 units of travel, the car is wedged.
- **Recovery** — the gearbox is driven into R (auto `gearDown()` jumps any forward gear straight to N, then to R), wheels straight, full throttle, no brake. Recovery ends after 30 units of heading-travel (the same accumulator counts reverse travel) or 3000 ms, then back to 1st gear.
- **`setBrake`/`setAcceleration` on cars are particle FX only.** `PSControllableCar::setBrake(bool)`/`setAcceleration(bool)` set `mBrakeEnabled`/`mAccelEnabled` which only gate particle emission. Real control inputs go through `physicsAICar->setAcceleration(Ogre::Real)` (→ `mThrottle`) and `setBreaks(Ogre::Real)` (→ `mBreaks`).
- **Reverse needs a wheel kick.** The engine's torque curve at idle revs is near-zero, so a standing car can't start reversing from engine power alone. `PhysicsVehicle::gearDown()` issues `setBackVelocity(...)` when the gearbox hits R (kick that builds RPM so the curve takes over). It applies to *all* vehicle types — make sure it stays ungated if you touch it.
- **Use live rotation, not `getForwardAxis()`.** `PSBaseCar::getForwardAxis()` reads `PSBaseVehicle::mInitialVehicleSetup`, a by-value copy made at construction that never updates. `AIUtils` measures heading from the live `initialVehicleSetup.mCarRot` (the shared object `PhysicsVehicle` references).
- **Gate on per-vehicle `getRaceStarted()`, not game state.** In the batched massacre start, the game-level `getRaceStarted()` turns true at GO while still-parked cars have per-vehicle `mIsRaceStarted == false`. Running the detector on a parked car (zero displacement for seconds) triggers a false stuck → it launches reversing. The whole measurement/detection block is guarded by `physicsAICar->getRaceStarted()`.
- **Reset NN state after recovery.** Reversing walks `mPrevClosestSplineIndex` backward along the track spline and bakes the reverse heading into `mPrevRot`, so resuming forward control produces erratic steering. On recovery exit the code sets `mIsPrevClosestSplineIndexInited = false` and `mPrevRot = ZERO` to force a full spline re-scan. The RNN hidden state (`slotMatrix` recurrence terms) is deliberately not touched.

### Race HUD 3-digit tacho (massacre)

The tacho dashboard `pos / totalcars` display (`UIRace::setCarPos`) supports up to 300 cars. Key facts for a future editor:

- **Widened params.** `setCarPos(size_t pos, size_t totalcars)` (was `unsigned char` — overflowed past 255). Callers pass `LapController`/`mAliveCars` `size_t` values directly, no truncating cast.
- **Third digit panels.** `mTachoTotalCarsDigit3` / `mTachoPosDigit3` (hundreds) are created hidden in `UIRace::load` at base multiplier slots 28.5 / 31.5 and shown only when the value ≥ 100. `mDashDigitLeftTotalCars*` / `mDashDigitLeftPos*` members store the slot x positions.
- **Depth-based layout.** `setCarPos` places every panel from a rightmost anchor: `left = anchor - depth * tightStep`. `shiftDepth = +1` when `totalcars ≥ 100` inserts the extra slot for the total hundreds digit (moves div + pos one step further left). Normal races never hit 99+ so the layout reduces exactly to the original 5-slot 2-digit arrangement.
- **Two compression knobs active only when a value needs 3 digits:** a tighter `tightStep` (`0.75 × dashDigitWidth`) and a panel width scale `5/7 ≈ 0.714`. Position and total scale independently (`scaleTotal` vs `scalePos`).
- **Offset invariants preserved by explicit compensation** (so 2→3-digit transitions don't shift the readout): the rightmost digit's *right edge* stays put via `total1RightCorr` added to the anchor; each digit's *bottom edge* stays put via `setTop(origTop + (origHeight − scaledHeight))`. Dimension/top members (`mDashDigitWidth{,Small}`, `mDashDigitHeight{,Small}`, `mDashDigitTop{,Small}`) are stored at `load` time for this.
- `setDimensions`/`setLeft`/`setTop`/`show`/`hide` are all issued **every frame** with deterministic values — no getter-based deltas, so transitions (massacre cars dying: 300 → 3) collapse cleanly in both directions.

## Conventions

- Headers are **co-located** with their `.cpp` files inside `ogre/src/<subsystem>/` — there is no separate `include/` tree (the only `include` dir is `ogre/src/includes/` for shared PCH includes).
- Source files are added to the build via `file(GLOB ...)` per subsystem in `ogre/CMakeLists.txt`. **New `.cpp`/`.h` files are picked up automatically** only if placed in an already-globbed directory; adding a new top-level subdir under `ogre/src/` requires a new glob + `list(APPEND ...)` entry in `ogre/CMakeLists.txt`.
- C++03-compatible code style (the README targets a C++03 compiler), though modern compilers are used in practice.
- Rendering/material configuration is data-driven through Lua (`assets/scripts/`) and Ogre material scripts (`assets/materials/`), not hardcoded — prefer extending those over touching engine code.
- `.pf` and `.ogg` files are gitignored (large original assets); `.str`/`.dat` save files and `*.log` are also gitignored.

## Useful references

- Class diagram: `docs/ClassDiagram.graphml` (open with [yEd](http://www.yworks.com/products/yed)).
- Physics reference papers: `docs/physics/`.
- AI reference: `docs/ai/`.
- `readme.md` covers asset sourcing, multiplayer server deployment, and port details.
