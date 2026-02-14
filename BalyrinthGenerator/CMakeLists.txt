cmake_minimum_required(VERSION 3.25)

set(ProjectName "BalyrinthGenerator")

project(${ProjectName} VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ImGuiDir "../imgui/")
set(ImGuiBackendsDir "../imgui/backends/")
set(ImGuiSrcs
    "${ImGuiDir}imconfig.h"
    "${ImGuiDir}imgui.cpp"
    "${ImGuiDir}imgui.h"
    "${ImGuiDir}imgui_draw.cpp"
    "${ImGuiDir}imgui_internal.h"
    "${ImGuiDir}imgui_tables.cpp"
    "${ImGuiDir}imgui_widgets.cpp"
    "${ImGuiDir}imstb_rectpack.h"
    "${ImGuiDir}imstb_textedit.h"
    "${ImGuiDir}imstb_truetype.h"
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

set(ProjectSrcs
    src/main.cpp
    src/BalyrinthGeneratorWindow.h
    src/BalyrinthGeneratorWindow.cpp
    src/ManagedWindow.h
    src/ManagedWindow.cpp
    src/Drawing/Color.h
    src/GL/Binder.h
    src/GL/Binder.cpp
    src/GL/Enums.h
    src/GL/Renderable.h
    src/GL/Renderable.cpp
    src/GL/RenderableMesh.h
    src/GL/RenderableMesh.cpp
    src/GL/Shader.h
    src/GL/Shader.cpp
    src/GL/ShaderProgram.h
    src/GL/ShaderProgram.cpp
    src/GL/Texture.h
    src/GL/Texture.cpp
    src/GL/Viewport.h
    src/GL/Viewport.cpp
    src/GL/Buffers/ArrayBuffer.h
    src/GL/Buffers/ArrayBuffer.cpp
    src/GL/Buffers/ArrayBufferList.h
    src/GL/Buffers/ArrayBufferList.cpp
    src/GL/Buffers/Buffer.h
    src/GL/Buffers/Buffer.cpp
    src/GL/Buffers/IndexBuffer.h
    src/GL/Buffers/IndexBuffer.cpp
    src/GL/Buffers/Ubo.h
    src/GL/Buffers/Ubo.cpp
    src/GL/Buffers/Vao.h
    src/GL/Buffers/Vao.cpp
    src/Graphics/GraphicsState.h
    src/Graphics/GraphicsState.cpp
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

set(AllFiles
    ${ProjectSrcs}
    ${ImGuiSrcs}
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source" FILES ${ProjectSrcs})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/${ImGuiDir}" PREFIX "Externals/ImGui" FILES ${ImGuiSrcs})

add_executable(${ProjectName}
    ${AllFiles}
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
