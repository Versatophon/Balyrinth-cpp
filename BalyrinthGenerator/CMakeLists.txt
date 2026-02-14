cmake_minimum_required(VERSION 3.25)

set(ProjectName "BalyrinthGenerator")

project(${ProjectName} VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ImGuiDir "../imgui/")
set(ImGuiBackendsDir "../imgui/backends/")
set(ImGuiSrcs
    "${ImGuiDir}imgui.cpp"
    "${ImGuiDir}imgui_draw.cpp"
    "${ImGuiDir}imgui_tables.cpp"
    "${ImGuiDir}imgui_widgets.cpp"
    "${ImGuiBackendsDir}imgui_impl_opengl3.cpp"
    "${ImGuiBackendsDir}imgui_impl_sdl3.cpp"
)

IF (WIN32)
  find_package(OpenGL REQUIRED)
  find_package(GLEW CONFIG REQUIRED)
  find_package(SDL3 REQUIRED CONFIG REQUIRED)
ELSE()
  find_package(PkgConfig REQUIRED) 
  find_package(OpenGL)
  find_package(SDL3 REQUIRED CONFIG REQUIRED)

  find_package(GLEW REQUIRED)
  include_directories(${GLEW_INCLUDE_DIRS})
  link_libraries(${GLEW_LIBRARIES})
ENDIF()

add_executable(${ProjectName}
    src/main.cpp
    src/BalyrinthGeneratorWindow.h
    src/BalyrinthGeneratorWindow.cpp
    src/ManagedWindow.h
    src/ManagedWindow.cpp
    src/GL/Binder.h
    src/GL/Binder.cpp
    src/GL/Buffers.h
    src/GL/Buffers.cpp
    src/GL/Renderable.h
    src/GL/Renderable.cpp
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
    ${ImGuiSrcs}
)

target_include_directories(${ProjectName} 
    PRIVATE
        ${ImGuiDir}
        ${ImGuiBackendsDir}
)

target_link_libraries(${ProjectName}
    PRIVATE 
        OpenGL::GL
        GLEW::GLEW
        SDL3::SDL3
        LibLabyrinth
        LibSimpleMaths
)

if(WIN32)
    add_custom_command(TARGET ${ProjectName} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:${ProjectName}>
            $<TARGET_FILE_DIR:${ProjectName}>
        COMMAND_EXPAND_LISTS
    )
endif()
