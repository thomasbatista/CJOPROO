cmake_minimum_required(VERSION 3.16)
project(LastWick LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Desativar construcao de testes e exemplos do raylib
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(CUSTOMIZE_BUILD OFF CACHE BOOL "" FORCE)

# Gerenciamento de dependencia externa via FetchContent
include(FetchContent)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
)
FetchContent_MakeAvailable(raylib)

add_executable(LastWick
    src/main.cpp
    src/core/GameConfig.h
    src/core/RunState.h
    src/core/HighScoreStore.h
    src/core/HighScoreStore.cpp
    src/core/AudioManager.h
    src/core/AudioManager.cpp
    src/core/EventBus.h
    src/core/EventBus.cpp
    src/core/ParticleSystem.h
    src/core/ParticleSystem.cpp
    src/core/Scene.h
    src/core/SceneManager.h
    src/core/SceneManager.cpp
    src/core/RaylibResource.h
    src/entities/Entity.h
    src/entities/Entity.cpp
    src/entities/Lantern.h
    src/entities/Lantern.cpp
    src/entities/Player.h
    src/entities/Player.cpp
    src/entities/Enemy.h
    src/entities/Enemy.cpp
    src/entities/Crawler.h
    src/entities/Crawler.cpp
    src/entities/Wisp.h
    src/entities/Wisp.cpp
    src/entities/Giant.h
    src/entities/Giant.cpp
    src/entities/EnemySpawner.h
    src/entities/EnemySpawner.cpp
    src/entities/Pickup.h
    src/entities/Pickup.cpp
    src/entities/OilFlask.h
    src/entities/OilFlask.cpp
    src/entities/Amulet.h
    src/entities/Amulet.cpp
    src/entities/Boots.h
    src/entities/Boots.cpp
    src/entities/PickupSpawner.h
    src/entities/PickupSpawner.cpp
    src/scenes/HUD.h
    src/scenes/HUD.cpp
    src/scenes/MenuScene.h
    src/scenes/MenuScene.cpp
    src/scenes/NightIntroScene.h
    src/scenes/NightIntroScene.cpp
    src/scenes/GameScene.h
    src/scenes/GameScene.cpp
    src/scenes/GameOverScene.h
    src/scenes/GameOverScene.cpp
)

target_include_directories(LastWick PRIVATE src)

# Ativacao de avisos rigorosos de compilacao
target_compile_options(LastWick PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra>
)

target_link_libraries(LastWick PRIVATE raylib)

# Suporte a testes automatizados com CTest
enable_testing()

add_executable(LastWickTests
    tests/test_main.cpp
    src/core/GameConfig.h
    src/core/RunState.h
    src/core/HighScoreStore.h
    src/core/HighScoreStore.cpp
    src/core/EventBus.h
    src/core/EventBus.cpp
    src/entities/Entity.h
    src/entities/Entity.cpp
    src/entities/Lantern.h
    src/entities/Lantern.cpp
    src/entities/Player.h
    src/entities/Player.cpp
    src/entities/Enemy.h
    src/entities/Enemy.cpp
    src/entities/Crawler.h
    src/entities/Crawler.cpp
    src/entities/Wisp.h
    src/entities/Wisp.cpp
    src/entities/Giant.h
    src/entities/Giant.cpp
    src/entities/EnemySpawner.h
    src/entities/EnemySpawner.cpp
)

target_include_directories(LastWickTests PRIVATE src)

target_compile_options(LastWickTests PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra>
)

target_link_libraries(LastWickTests PRIVATE raylib)

add_test(NAME UnitTests COMMAND LastWickTests)
