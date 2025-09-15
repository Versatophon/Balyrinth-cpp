# Balyrinth

## Prerequisites

You need to have these packages installed via vcpkg:
- For `LibLabyrinth`:
	- pcg:
		```
		vcpkg install pcg
		```

- For `BalyrinthGenerator`:
	- glew:
		```
		vcpkg install glew
		```
	- sdl3:
		```
		vcpkg install sdl3
		```
	- imgui[core,sdl3-binding,opengl3-binding]:
		```
		vcpkg.exe install imgui[core,sdl3-binding,opengl3-binding] --recurse
		```

## Build 

```
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=YourVcpkgPath/scripts/buildsystems/vcpkg.cmake
```

You project will be buildable from `build` directory
