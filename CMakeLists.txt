# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.5)

# Set the project name and language
project(FallingSand C)

# Set the C standard to use
set(CMAKE_C_STANDARD 11)

# Set the output directory for the executable files
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Find the SDL2 library
find_package(SDL2 REQUIRED)

# Set the source files 
file(GLOB SRC_FILES "src/*.c")

# Add the source files to the executable target
add_executable(FallingSand ${SRC_FILES})

# Link the executable target with the SDL2 library
target_link_libraries(FallingSand PRIVATE SDL2)

# Add compile options for the executable target
target_compile_options(FallingSand PRIVATE 
    -Wall 
    -Wextra 
    -Wpedantic
)

# Add a separate executable target for debugging
add_executable(FallingSand_debug ${SRC_FILES})

# Link the debugging executable target with the SDL2 library
target_link_libraries(FallingSand_debug PRIVATE SDL2)

# Add compile options for the debugging executable target
target_compile_options(FallingSand_debug PRIVATE 
    -Wall 
    -Wextra 
    -Wpedantic 
    -g
    -DDEBUG
)

# Add include directories for the targets
include_directories(FallingSand PRIVATE src/include)
include_directories(FallingSand_debug PRIVATE src/include)
