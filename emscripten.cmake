set(CMAKE_SYSTEM_NAME Emscripten)
set(CMAKE_SYSTEM_VERSION 1)

set(CMAKE_C_COMPILER "D:/CleanDev/emscripten/emsdk/upstream/emscripten/emcc.bat")
set(CMAKE_CXX_COMPILER "D:/CleanDev/emscripten/emsdk/upstream/emscripten/em++.bat")
set(CMAKE_AR "D:/CleanDev/emscripten/emsdk/upstream/bin/llvm-ar.exe")
set(CMAKE_RANLIB "D:/CleanDev/emscripten/emsdk/upstream/bin/llvm-ranlib.exe")

set(CMAKE_EXECUTABLE_SUFFIX ".js")

# Empêche CMake de tester les libs système Windows
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
