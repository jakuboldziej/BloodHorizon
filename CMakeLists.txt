cmake_minimum_required(VERSION 3.16)
project(BloodHorizon)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)

# Find SDL3
pkg_check_modules(SDL3 REQUIRED sdl3)
pkg_check_modules(SDL3_IMAGE REQUIRED sdl3-image)
pkg_check_modules(SDL3_TTF REQUIRED sdl3-ttf)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${SDL3_INCLUDE_DIRS})
include_directories(${SDL3_IMAGE_INCLUDE_DIRS})
include_directories(${SDL3_TTF_INCLUDE_DIRS})

# Collect all source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME} 
    ${SDL3_LIBRARIES}
    ${SDL3_IMAGE_LIBRARIES}
    ${SDL3_TTF_LIBRARIES}
)

# Compiler flags
target_compile_options(${PROJECT_NAME} PRIVATE ${SDL3_CFLAGS_OTHER})
target_compile_options(${PROJECT_NAME} PRIVATE ${SDL3_IMAGE_CFLAGS_OTHER})
target_compile_options(${PROJECT_NAME} PRIVATE ${SDL3_TTF_CFLAGS_OTHER})

# Enable debug symbols
set(CMAKE_BUILD_TYPE Debug)

# Windows-specific settings
if(WIN32)
    # Link mingw32 for Windows
    target_link_libraries(${PROJECT_NAME} mingw32)
    
    # Set console subsystem
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_FLAGS "-mconsole"
    )
endif()

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
)
