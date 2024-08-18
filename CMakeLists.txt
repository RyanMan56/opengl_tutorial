cmake_minimum_required(VERSION 3.15)

# Will cause cmake to define the variables OpenglTutorial_VERSION_MAJOR and OpenglTutorial_VERSION_MINOR
project(OpenglTutorial VERSION 1.0)

# Provide a single source of truth for compiler flags
add_library(opengl_tutorial_compiler_flags INTERFACE)
target_compile_features(opengl_tutorial_compiler_flags INTERFACE cxx_std_20)

# Compiler conditionals
set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
set(msvc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")

# Add compile warning flags based on the compiler used from the flags above
target_compile_options(opengl_tutorial_compiler_flags INTERFACE
  "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-fcolor-diagnostics;-fansi-escape-codes;-Wall;-Wextra;-Wconversion;-Wsign-conversion;-Wshadow;-Wformat=2;-Wunused>>"
  "$<${msvc_like_cxx}:$<BUILD_INTERFACE:-W3>>"
)

# Control where the static and shared libraries are built so that on Windows
# we don't need to mess with the PATH to run the executable
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

# Don't build the GLFW docs/tests/examples
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Copy shader files to build directory
set(SHADER_SOURCE_DIR "${CMAKE_SOURCE_DIR}/src/shaders")
set(SHADER_DEST_DIR "${CMAKE_BINARY_DIR}/shaders")

# Make sure the shader destination directory exists
file(MAKE_DIRECTORY ${SHADER_DEST_DIR})

# Copy shader files
file(GLOB SHADER_FILES "${SHADER_SOURCE_DIR}/*")
file(COPY ${SHADER_FILES} DESTINATION ${SHADER_DEST_DIR})

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

# Gathers all cpp files in the src directory recursively
# NOTE: I will need to regenerate build files after adding any new files,
# as these won't be automatically picked up
file(GLOB_RECURSE SOURCE_FILES "${CMAKE_SOURCE_DIR}/src/*.cpp")

add_executable(OpenglTutorial src/main.cpp ${SOURCE_FILES})

# Copy the assets directory into the build directory
add_custom_command(TARGET OpenglTutorial POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:OpenglTutorial>/../assets
)

# Add GLFW as a subdirectory
add_subdirectory(external/glfw)
add_subdirectory(external/glad)
add_subdirectory(external/stb_image)

# Find and link OpenGL (this will be provided by the graphics drivers and the operating system)
find_package(OpenGL REQUIRED)

# Link the compiler flags and libraries to the executable
target_link_libraries(OpenglTutorial PUBLIC opengl_tutorial_compiler_flags)
target_link_libraries(OpenglTutorial PRIVATE glfw OpenGL::GL glad stb_image)

# Ensure the include directories are correctly set
target_include_directories(OpenglTutorial PRIVATE
  "${PROJECT_SOURCE_DIR}/external"
)
target_include_directories(OpenglTutorial PUBLIC "${PROJECT_BINARY_DIR}/src")

# Install the executable to the bin directory
install(TARGETS OpenglTutorial DESTINATION bin)
