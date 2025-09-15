# Balyrinth

This project provides tools to generate labyrinths with several generation modes:
 
- Stack backtracking
- Queue backtracking
- Random backtracking
- Bloom

All genereations are reproductible from a seed.

Some rendered samples:

<div style="padding: 20px;">
	<img src="./images/BloomSquare.png" alt="BloomSquare" height="100"/>
	<img src="./images/BloomSquareTore.png" alt="BloomSquareTore" height="100"/>
	<img src="./images/BloomSquareToreContiguous.png" alt="BloomSquareToreContiguous" height="100"/>
	<img src="./images/BloomSquareToreWithNeighbors.png" alt="BloomSquareToreWithNeighbors" height="100"/>
	<img src="./images/BloomSqureToreWithNeighborsWithoutCell.png" alt="BloomSqureToreWithNeighborsWithoutCell" height="100"/>
</div>

## Build 

### Prerequisites

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

### Build project

```
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=YourVcpkgPath/scripts/buildsystems/vcpkg.cmake
```

You project will be buildable from `build` directory
