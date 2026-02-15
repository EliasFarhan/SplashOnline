# Plan: Extract a Reusable Rollback Engine Library from SplashOnline

## Context

SplashOnline is a C++23 rollback multiplayer water-fighting game used as a CppCon 2024 demo. The goal is to refactor it into **two layers**: a reusable library ("RollbackEngine") that students can use to build any rollback multiplayer game, and the game-specific "Splash" code that uses it. The library should enforce an MVC pattern and be independently testable.

**Core problem**: `PlayerInput`, `MaxPlayerNmb`, `GameSystems`, and packet types are hardcoded throughout the rollback/network/engine layers, preventing reuse.

---

## Library vs Game Boundary

### RollbackEngine library (reusable)
| Component | Current file | What moves |
|-----------|-------------|------------|
| System interface | `include/engine/system.h` | `SystemInterface` as-is |
| Window management | `include/engine/window.h` | `OnEventInterface`, window functions |
| Engine loop | `include/engine/engine.h` | `RunEngine`, job scheduling |
| Graphics pipeline | `include/graphics/graphics_manager.h` | `DrawInterface`, renderer management |
| GUI layer | `include/graphics/gui_renderer.h` | `OnGuiInterface`, ImGui integration |
| Rollback primitives | `include/rollback/rollback_system.h` | `Checksum<N>`, `RollbackInterface<T,N>` |
| Rollback manager | `include/rollback/rollback_manager.h` | **Templatized** on `InputT` |
| Utilities | `include/utils/timer.h`, `log.h`, `adler32.h` | As-is (remove `PlayerInput` overload from adler32) |
| **New** MVC interfaces | — | `GameModelInterface`, `GameViewInterface`, `GameControllerBase` |
| **New** Network transport | — | `NetworkTransportInterface` (abstract) |

### Splash game (stays game-specific)
All of: `game/`, `game/graphics/`, `audio/`, `network/client.h`, `network/packet.h`, `splash.h`, `engine/input_manager.h` (the `PlayerInput` struct + SDL controller mapping), texture/spine managers, FMOD, Photon integration.

---

## Key Abstractions

### 1. GameInput concept (replaces hardcoded `PlayerInput`)

```cpp
// rollback_engine/include/rollback_engine/input_concept.h
template<typename T>
concept GameInput = std::is_trivially_copyable_v<T>
    && std::equality_comparable<T>
    && (sizeof(T) <= 64);
```

Every place that currently uses `PlayerInput` in the library becomes a template parameter `InputT` constrained by `GameInput`.

### 2. MVC Interfaces

**Model** — `GameModelInterface<InputT, ChecksumSize>` (`include/rollback_engine/mvc/model.h`)
- `Begin()`, `End()`, `Tick()`
- `SetInputs(span<const InputT>)`, `SetPreviousInputs(span<const InputT>)`
- `CalculateChecksum() -> Checksum<ChecksumSize>`
- `CopyStateFrom(const GameModelInterface&)` (replaces current `RollbackFrom`)
- `GetMaxPlayers() -> int`

Current `GameSystems` (`include/game/game_systems.h`) implements this. It already has `Begin/Tick/End`, `SetPlayerInput`, `CalculateChecksum`, `RollbackFrom` — just needs interface conformance.

**View** — `GameViewInterface<InputT, ChecksumSize>` (`include/rollback_engine/mvc/view.h`)
- `Begin()`, `End()`, `Update(float dt)`, `Tick()`, `Draw()`
- `SetModel(const GameModelInterface*)` — read-only access to model

Current `GameView` already holds `const GameSystems*`. Minimal change.

**Controller** — `GameControllerBase<InputT, ChecksumSize>` (`include/rollback_engine/mvc/controller.h`)
- Owns Model + View + RollbackManager
- Implements the fixed-timestep loop (currently in `GameManager::Update` lines 52-106)
- Implements the rollback update loop (currently in `GameManager::RollbackUpdate` lines 252-394)
- Virtual hooks: `PollLocalInput() -> InputT`, `OnDesync(int frame)`, `OnGameOver()`

Current `GameManager` becomes `SplashGameController : GameControllerBase<PlayerInput, 7>`, keeping only game-specific logic (intro timer, countdown sounds, end screen).

### 3. Templated RollbackManager

Current `RollbackManager` (`include/rollback/rollback_manager.h`) becomes `RollbackManager<InputT, ChecksumSize>`:
- `PlayerInput` → `InputT`
- `MaxPlayerNmb` → runtime `maxPlayers` from config
- `GameSystems confirmFrameGameSystems_` → `GameModelInterface<InputT, ChecksumSize>*` (non-owning, controller creates both models)
- `SetInputs(const InputPacket&)` → `SetInputsFromNetwork(int playerNumber, span<const InputT>, uint16_t frame, uint8_t size)`
- Becomes header-only (template class)

### 4. Network Transport Interface

```cpp
// rollback_engine/include/rollback_engine/network/transport.h
class NetworkTransportInterface {
    virtual void SendReliable(span<const uint8_t> data) = 0;
    virtual void SendUnreliable(span<const uint8_t> data) = 0;
    virtual bool IsConnected() const = 0;
    virtual bool IsMaster() const = 0;
    virtual int GetLocalPlayerIndex() const = 0;
};
```

Current Photon code (`network/client.h/cpp`) becomes `PhotonTransport : NetworkTransportInterface` in the game project.

---

## Directory Structure

```
SplashOnline/
  externals/
    RollbackEngine/                    # NEW - reusable library
      CMakeLists.txt
      include/rollback_engine/
        input_concept.h                # GameInput concept
        mvc/
          model.h                      # GameModelInterface<InputT, N>
          view.h                       # GameViewInterface<InputT, N>
          controller.h                 # GameControllerBase<InputT, N>
        engine/
          system.h                     # SystemInterface (from engine/system.h)
          window.h                     # Window management (from engine/window.h)
          engine.h                     # RunEngine (from engine/engine.h)
        graphics/
          graphics_manager.h           # DrawInterface (from graphics/graphics_manager.h)
          gui_renderer.h               # OnGuiInterface (from graphics/gui_renderer.h)
        rollback/
          rollback_system.h            # Checksum<N>, RollbackInterface<T,N>
          rollback_manager.h           # RollbackManager<InputT, N> (header-only template)
        network/
          transport.h                  # NetworkTransportInterface
        utils/
          timer.h, adler32.h, log.h
      src/
        engine/engine.cpp, window.cpp
        graphics/graphics_manager.cpp, gui_renderer.cpp
        utils/log.cpp
      test/                            # Library-level tests
        rollback_test.cpp              # Tests with SimpleInput mock
        checksum_test.cpp
        model_contract_test.cpp
        controller_test.cpp
  include/splash/                      # Game-specific headers (renamed from current layout)
    game/ (const.h, player_input.h, player_character.h, bullet.h, level.h, game_systems.h)
    graphics/ (game_view.h, player_view.h, bullet_view.h, level_view.h, end_screen_view.h)
    network/ (photon_transport.h, packet.h)
    audio/ (audio_manager.h, sound_manager.h, music_manager.h, player_sound.h)
    input/ (input_manager.h with PlayerInput + SDL reading)
    splash_controller.h               # SplashGameController (from game_manager.h)
    splash.h
  src/splash/                          # Game-specific sources
  test/                                # Game-level tests
```

### CMake structure

**`externals/RollbackEngine/CMakeLists.txt`**: Builds `RollbackEngine` static library, links SDL2, imgui, NekoCore. Exports include path.

**Root `CMakeLists.txt`**: `add_subdirectory(externals/RollbackEngine)`, builds `SplashLib` linking `RollbackEngine` + NekoPhysics + Spine + FMOD + NekoNet.

### Student project usage
```cmake
# StudentGame/CMakeLists.txt
add_subdirectory(externals/RollbackEngine)
target_link_libraries(MyGame PUBLIC RollbackEngine NekoPhysics)
```

Student defines their own `FighterInput` struct, `FightingModel : GameModelInterface<FighterInput, 3>`, `FightingView`, `FightingController : GameControllerBase<FighterInput, 3>`.

---

## Migration Phases

### Phase 1: Create RollbackEngine directory and move pure utilities
- Create `externals/RollbackEngine/` with CMakeLists.txt
- Move `timer.h`, `log.h/cpp`, `adler32.h` (remove `PlayerInput` overload)
- Move `system.h`, `window.h/cpp`, `engine.h/cpp`
- Move `graphics_manager.h/cpp`, `gui_renderer.h/cpp`
- Move `rollback_system.h`
- Update all includes in game code
- **Verify**: project compiles, existing tests pass

### Phase 2: Create MVC interfaces
- Write `input_concept.h`, `model.h`, `view.h`, `controller.h`
- Make `GameSystems` implement `GameModelInterface<PlayerInput, 7>` (rename `RollbackFrom` → `CopyStateFrom`, add `GetMaxPlayers`)
- Make `GameView` implement `GameViewInterface<PlayerInput, 7>`
- **Verify**: project compiles

### Phase 3: Templatize RollbackManager
- Convert `RollbackManager` to `RollbackManager<InputT, ChecksumSize>` in the library
- Replace `GameSystems` member with `GameModelInterface*`
- Replace `InputPacket` dependency with `SetInputsFromNetwork()` method
- Replace `MaxPlayerNmb`/`MaxPlayerInputNmb` with runtime config
- Move to library as header-only
- Update `GameManager` to pass the confirm model pointer
- **Verify**: existing `rollback_test.cpp` passes (updated for new API)

### Phase 4: Extract GameControllerBase
- Extract fixed-timestep loop and rollback update loop from `GameManager` into `GameControllerBase`
- Create `SplashGameController : GameControllerBase<PlayerInput, 7>` with game-specific logic (intro timer, sounds, end screen)
- **Verify**: game runs correctly with the same behavior

### Phase 5: Abstract network transport
- Create `NetworkTransportInterface` in library
- Create `PhotonTransport` wrapping current `client.h/cpp`
- Wire `GameControllerBase` to use `NetworkTransportInterface*`
- **Verify**: online play still works

### Phase 6: Separate PlayerInput from library
- Extract `PlayerInput` struct from `input_manager.h` into `splash/game/player_input.h`
- Move `Adler32::Add(PlayerInput)` into game-specific `input_checksum.h`
- Library's `input_manager` becomes raw SDL event polling only
- **Verify**: full compilation, tests pass

---

## Test Plan

### Library-level tests (`externals/RollbackEngine/test/`)

**Test fixtures:**
```cpp
struct SimpleInput { int8_t x=0, y=0; uint8_t buttons=0; auto operator<=>(const SimpleInput&) const = default; };

class SimpleModel : public GameModelInterface<SimpleInput, 1> {
    int posX=0, posY=0;
    // Begin, Tick (apply inputs to position), CalculateChecksum (adler32 of pos), CopyStateFrom
};
```

| Test file | What it tests |
|-----------|--------------|
| `checksum_test.cpp` | Adler32 with ints, floats, Vec2, ranges; Checksum<N> aggregation and comparison |
| `rollback_test.cpp` | Input prediction (replicate last), SetInputsFromNetwork, ConfirmLastFrame with matching/mismatching checksums |
| `model_contract_test.cpp` | Determinism: same inputs → same checksum; CopyStateFrom produces identical checksums |
| `controller_test.cpp` | Fixed timestep accumulation; rollback triggering on late input; frame confirmation progression |

### Game-level tests (`test/`)

| Test file | What it tests |
|-----------|--------------|
| `game_systems_test.cpp` | GameSystems implements GameModelInterface correctly; physics determinism |
| `integration_test.cpp` | Full Model + RollbackManager + mock network: simulate 2-player game, confirm frames, detect desync |

### Verification after each phase
1. `cmake --build . --target SplashTest && ctest` — all tests pass
2. Run the game executable — visual verification that gameplay is unchanged
3. Run `network_test` — online play still works (after Phase 5)
