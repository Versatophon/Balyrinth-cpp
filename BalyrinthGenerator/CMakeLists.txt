cmake_minimum_required(VERSION 3.25)
project(BalyrinthGenerator VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(GLEW CONFIG REQUIRED)
find_package(SDL3 REQUIRED CONFIG REQUIRED)
find_package(imgui REQUIRED)

add_executable(BalyrinthGenerator
    src/main.cpp
    src/BalyrinthGeneratorWindow.h
    src/BalyrinthGeneratorWindow.cpp
    src/ManagedWindow.h
    src/ManagedWindow.cpp
    src/GL/Binder.h
    src/GL/Binder.cpp
    src/GL/Buffers.h
    src/GL/Buffers.cpp
    src/GL/Shader.h
    src/GL/Shader.cpp
    src/GL/Tex.h
    src/GL/Tex.cpp
    src/GL/Viewport.h
    src/GL/Viewport.cpp
    src/Drawers/GeometryContainer.h
    src/Drawers/ContiguousMazeDrawer.h
    src/Drawers/ContiguousMazeDrawer.cpp
    src/Drawers/MazeDrawer.h
    src/Drawers/MazeDrawer.cpp
    src/Drawers/ShapeMazeDrawer.h
    src/Drawers/ShapeMazeDrawer.cpp
    src/Resources/InlineShaders.h
    src/Tools/SettingsLoadSave.h
    src/Tools/SettingsLoadSave.cpp
)

target_link_libraries(BalyrinthGenerator
    PRIVATE 
        OpenGL::GL
        GLEW::GLEW
        SDL3::SDL3
        imgui::imgui
        LibLabyrinth
        LibSimpleMaths
)

if(WIN32)
    add_custom_command(TARGET BalyrinthGenerator POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:BalyrinthGenerator>
            $<TARGET_FILE_DIR:BalyrinthGenerator>
        COMMAND_EXPAND_LISTS
    )
endif()
