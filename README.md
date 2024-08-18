# OpenGL Tutorial

Project created following the LearnOpenGL tutorial: https://learnopengl.com/

The CMake build system has been put together largely inspired by the official CMake tutorial which I have followed in this repository: https://github.com/RyanMan56/cmake_tutorial

## How to build and run

1. This project uses git submodules to include dependencies, so first make sure to run `git submodule update --init --recursive` if you haven't done a deep clone already
2. Create a `build` directory, e.g. `mkdir build`
3. Generate a native build system, `cd build && cmake ..`
4. Compile/link the project, `cmake --build .`
5. Run the project `./bin/OpenglTutorial` (assuming you're on mac/linux. You'll probably just want to open and run the project in VS if you're on windows)

There are a couple of handy scripts for mac/linux (I haven't created these for windows yet). Make sure to cd into the scripts directory and run them from there:

- `scripts/build.sh` - cds into the build directory, generates a native build system, compiles/links the project, and runs the executable
- `scripts/clean_build.sh` - removes the `build` directory and runs `build.sh`

### Note

When adding a new .cpp file make sure to run `cmake ..` e.g. `cmake .. && cmake --build . && ./bin/OpenglTutorial` otherwise the change to the sources won't get picked up by CMake
