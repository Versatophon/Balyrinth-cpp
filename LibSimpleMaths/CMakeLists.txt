cmake_minimum_required(VERSION 3.25)
project(LibSimpleMaths VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(SIMPLE_MATHS_EXPORTS)

add_library(LibSimpleMaths SHARED 
    src/SimpleMathsApi.h
    src/Matrix4f.h
    src/Matrix4f.cpp
    src/Quaternionf.h
    src/Quaternionf.cpp
    src/Rectanglei.h
    src/Transformf.h
    src/Transformf.cpp
    src/Trigonometry.h
    src/Vector2f.h
    src/Vector2f.cpp
    src/Vector2i.h
    src/Vector2i.cpp
    src/Vector2us.h
    src/Vector2us.cpp
    src/Vector3f.h
    src/Vector3f.cpp
    src/Vector3i.h
    src/Vector3i.cpp
)

target_include_directories(LibSimpleMaths
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    PRIVATE 
        ${PCG_INCLUDE_DIRS} # Only if no build package is required 
)
