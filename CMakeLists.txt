cmake_minimum_required(VERSION 3.25)

project(Balyrinth CXX)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(LibLabyrinth)
add_subdirectory(BalyrinthGenerator)

#"CMAKE_TOOLCHAIN_FILE": "/opt/vcpkg/scripts/buildsystems/vcpkg.cmake",
#"CMAKE_TOOLCHAIN_FILE": "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake",

# cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=D:/CleanDev/vcpkg/scripts/buildsystems/vcpkg.cmake

# install imgui with backends with vcpkg
# vcpkg.exe install imgui[core,sdl3-binding,opengl3-binding] --recurse
