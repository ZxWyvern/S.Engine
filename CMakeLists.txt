cmake_minimum_required(VERSION 3.16)
project(psx-engine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 11)

# Options for SDL to speed up configure
set(SDL_TEST OFF CACHE BOOL "" FORCE)
set(SDL_TESTS OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SDL_INSTALL OFF CACHE BOOL "" FORCE)
# SDL2.30 still uses SDL_SHARED / SDL_STATIC variable names in some forks
set(SDL_SHARED ON CACHE BOOL "" FORCE)
set(SDL_STATIC OFF CACHE BOOL "" FORCE)

add_subdirectory(external/SDL)

# GLAD (generated for OpenGL 3.3 core)
add_library(glad STATIC external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)
set_target_properties(glad PROPERTIES LINKER_LANGUAGE C)

# GLM is header-only
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE external/glm)

set(SOURCES
    src/main.cpp
    src/platform/window.cpp
    src/platform/input.cpp
    src/platform/logger.cpp
    src/core/logger.cpp
    src/core/time.cpp
    src/core/application.cpp
    src/renderer/shader.cpp
    src/renderer/mesh.cpp
    src/renderer/render_target.cpp
    src/renderer/renderer.cpp
    src/scene/transform.cpp
    src/scene/camera.cpp
    src/scene/scene_node.cpp
    src/scene/scene.cpp
    src/gameplay/player_controller.cpp
    src/gameplay/collision_system.cpp
    src/gameplay/level.cpp
    src/gameplay/game.cpp
)

add_executable(psx-engine ${SOURCES})

target_include_directories(psx-engine PRIVATE
    src
    external/glad/include
    external/glm
)

target_link_libraries(psx-engine PRIVATE
    glad
    glm
    SDL2main
    SDL2
)

if(WIN32)
    target_link_libraries(psx-engine PRIVATE opengl32)
else()
    find_package(OpenGL REQUIRED)
    target_link_libraries(psx-engine PRIVATE OpenGL::GL)
endif()

if(MSVC)
    target_compile_options(psx-engine PRIVATE /W4)
else()
    target_compile_options(psx-engine PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Copy shaders to build directory next to executable
add_custom_command(TARGET psx-engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/shaders
            $<TARGET_FILE_DIR:psx-engine>/shaders
    COMMENT "Copying shaders to build directory"
)

# Also copy for multi-config generators (Debug/Release subfolder)
if(CMAKE_CONFIGURATION_TYPES)
    add_custom_command(TARGET psx-engine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/shaders
                ${CMAKE_SOURCE_DIR}/build/shaders
        COMMENT "Copying shaders to build/shaders"
    )
endif()
