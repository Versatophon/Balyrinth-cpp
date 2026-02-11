cmake_minimum_required(VERSION 3.25)
set(ProjectName "Mazemerizer")

project(${ProjectName} VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(GLEW CONFIG REQUIRED)
find_package(SDL3 REQUIRED CONFIG REQUIRED)
find_package(imgui REQUIRED)

add_executable(${ProjectName}
    src/main.cpp
)

target_link_libraries(${ProjectName}
    PRIVATE 
        OpenGL::GL
        GLEW::GLEW
        SDL3::SDL3
        imgui::imgui
        LibLabyrinth
        LibSimpleMaths
)

if(WIN32)
    add_custom_command(TARGET ${ProjectName} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:${ProjectName}
            $<TARGET_FILE_DIR:${ProjectName}>
        COMMAND_EXPAND_LISTS
    )
endif()
