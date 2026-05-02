cmake_minimum_required(VERSION 3.25)

set(ProjectName "TwitchOverlay")

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

IF(EMSCRIPTEN)
    # Emscripten fournit SDL3, OpenGL ES et WebGL2 nativement
    # Pas besoin de find_package, tout passe par des flags de link
ELSEIF (WIN32)
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

if(EMSCRIPTEN)
    set_target_properties(${ProjectName} PROPERTIES SUFFIX ".html")
    
    target_include_directories(${ProjectName} PRIVATE
        "$ENV{EMSDK}/upstream/emscripten/cache/sysroot/include"
    )

    target_compile_options(${ProjectName} PRIVATE
        "-sUSE_SDL=3"
    )
    
    target_link_options(${ProjectName} PRIVATE
        "-sUSE_SDL=3"
        "-sUSE_WEBGL2=1"
        "-sFULL_ES3=1"
        "-sALLOW_MEMORY_GROWTH=1"
        "-sWASM=1"
    )
    target_link_libraries(${ProjectName}
        PRIVATE
            LibLabyrinth
            LibSimpleMaths
    )

    add_custom_command(
        TARGET ${ProjectName} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/rsc/minimal.html
            ${CMAKE_BINARY_DIR}/${ProjectName}/minimal.html
        COMMENT "Copie de minimal.html dans le répertoire de build"
    )
else()
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
endif()

add_dependencies(${ProjectName}
        LibLabyrinth
        LibSimpleMaths
)
