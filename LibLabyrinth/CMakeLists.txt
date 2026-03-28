cmake_minimum_required(VERSION 3.25)

set(ProjectName "LibLabyrinth")

project(${ProjectName} VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    src/GridShape.h
    src/IndexProvider.h
    src/Labyrinth.h
    src/Labyrinth.cpp
    src/LabyrinthApi.h
    src/Node.h
    src/Node.cpp
    src/RoomNeighborhood.h
    src/Topology.h
    src/Topology.cpp
    src/Seed.h
    src/SeededRandGen.h
    src/SeededRandGen.cpp
    src/Shape.h
    src/Shape.cpp
    src/Generators/SquaresOnRectShape.cpp
    src/Generators/SquaresOnToreShape.cpp
    src/Generators/HexagonsOnRectShape.cpp
    src/Generators/HexagonsOnToreShape.cpp
)

if(EMSCRIPTEN)
    add_library(LibLabyrinth STATIC ${SOURCES})
else()
    add_compile_definitions(LABYRINTH_EXPORTS)
    add_library(LibLabyrinth SHARED ${SOURCES})
endif()

find_path(PCG_INCLUDE_DIRS "pcg_random.hpp") # Only if no build package is required

target_include_directories(${ProjectName}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    PRIVATE 
        ${PCG_INCLUDE_DIRS} # Only if no build package is required 
)

target_link_libraries(${ProjectName}
    PRIVATE 
        LibSimpleMaths
)

# cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake
