cmake_minimum_required(VERSION 3.12)
project(FallingSand LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED TRUE) 

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build (Debug, Release)" FORCE)
endif()

add_subdirectory(libs/ZINC)
find_package(SDL2 REQUIRED)

add_executable(${PROJECT_NAME} 
    src/main.c
    src/chunk.c
    src/cell.c
    src/move_list.c
    src/world.c
    src/camera.c
    src/utils.c
    src/particle.c
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ZINC
    ${SDL2_LIBRARIES} 
    m
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(${PROJECT_NAME} PRIVATE 
        -g 
        -DDEBUG
    )
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(${PROJECT_NAME} PRIVATE -O3)
endif()

