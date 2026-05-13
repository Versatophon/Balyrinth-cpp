cmake_minimum_required(VERSION 3.25)

project(Balyrinth CXX)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(LibSimpleMaths)
add_subdirectory(LibLabyrinth)
add_subdirectory(BalyrinthGenerator)
add_subdirectory(TwitchOverlay)
#add_subdirectory(TwitchOverlayServer)

#add_subdirectory(Mazemerizer)

#"CMAKE_TOOLCHAIN_FILE": "/opt/vcpkg/scripts/buildsystems/vcpkg.cmake",
#"CMAKE_TOOLCHAIN_FILE": "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake",

# cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=D:/CleanDev/vcpkg/scripts/buildsystems/vcpkg.cmake

# install imgui with backends with vcpkg
# vcpkg.exe install imgui[core,sdl3-binding,opengl3-binding] --recurse

# install uwebsockets
# vcpkg install uwebsockets

# install emscripten
# winget install Ninja-build.Ninja
# git clone https://github.com/emscripten-core/emsdk.git
# cd emsdk
# Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
# .\emsdk install latest
# .\emsdk activate latest --permanent
# D:\CleanDev\emscripten\emsdk\emsdk_env.ps1

# build emscripten
# emcmake cmake -B build_web -G Ninja -DCMAKE_TOOLCHAIN_FILE='emscripten.cmake' -DSDL_EMSCRIPTEN_PERSISTENT_PATH=/libsdl -DPCG_INCLUDE_DIRS="D:/CleanDev/vcpkg/installed/x64-windows/include"
# cmake --build build_web
# npx serve build_web
