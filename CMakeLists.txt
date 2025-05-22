cmake_minimum_required(VERSION 3.20)
project(SCUMM-Redux VERSION 1.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Include directories
include_directories(
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/libs/imgui
        ${PROJECT_SOURCE_DIR}/libs/imgui/backends
        ${PROJECT_SOURCE_DIR}/libs/glfw/include
)

# Gather ImGui sources
file(GLOB IMGUI_SOURCES
        ${PROJECT_SOURCE_DIR}/libs/imgui/*.cpp
        ${PROJECT_SOURCE_DIR}/libs/imgui/backends/imgui_impl_glfw.cpp
        ${PROJECT_SOURCE_DIR}/libs/imgui/backends/imgui_impl_opengl3.cpp
)

# Gather project sources
file(GLOB_RECURSE PROJECT_SOURCES
        ${PROJECT_SOURCE_DIR}/src/*.cpp
        ${PROJECT_SOURCE_DIR}/src/*.h
)

# Create executable
add_executable(${PROJECT_NAME}
        ${PROJECT_SOURCES}
        ${IMGUI_SOURCES}
)

# Add GLFW subdirectory
add_subdirectory(libs/glfw)

# Link libraries
target_link_libraries(${PROJECT_NAME}
        PRIVATE
        glfw
        OpenGL::GL
)

# Platform-specific libraries
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE opengl32)
elseif(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    target_link_libraries(${PROJECT_NAME} PRIVATE
            ${COCOA_LIBRARY}
            ${IOKIT_LIBRARY}
            ${COREVIDEO_LIBRARY}
    )
elseif(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE
            GL
            X11
            pthread
            Xrandr
            Xi
            dl
    )
endif()

# Compiler-specific options
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
            _CRT_SECURE_NO_WARNINGS
            NOMINMAX
    )
else()
    target_compile_options(${PROJECT_NAME} PRIVATE
            -Wall -Wextra -Wpedantic
            -Wno-unused-parameter
    )
endif()

# Debug/Release configurations
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG=1)
    if(NOT MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE -g -O0)
    endif()
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE NDEBUG=1)
    if(NOT MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE -O3)
    endif()
endif()

# Set output directories
set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin
)

# Copy resources to build directory (if exists)
if(EXISTS ${PROJECT_SOURCE_DIR}/res)
    file(COPY ${PROJECT_SOURCE_DIR}/res
            DESTINATION ${CMAKE_BINARY_DIR}/bin)
    message(STATUS "Resources: Copied to build directory")
else()
    message(STATUS "Resources: Directory not found, skipping")
endif()

# Installation
install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin
)

# Install resources if they exist
if(EXISTS ${PROJECT_SOURCE_DIR}/res)
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/res/
            DESTINATION bin/res
    )
endif()

# Print build information
message(STATUS "SCUMM Redux Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "  Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "  Architecture: ${CMAKE_SYSTEM_PROCESSOR}")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "  Debug Features: Enabled")
    message(STATUS "  Optimization: Disabled")
else()
    message(STATUS "  Debug Features: Disabled")
    message(STATUS "  Optimization: Enabled")
endif()