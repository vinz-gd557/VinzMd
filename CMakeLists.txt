cmake_minimum_required(VERSION 3.21)

# ---------------------------------------------------------------------------
# NovaClient GD - Geode Mod
# ---------------------------------------------------------------------------

# PAKSA MATI DI LEVEL GLOBAL SEBELUM GEODE DI-LOAD
if(ANDROID)
    add_compile_definitions(FMT_USE_CONSTEVAL=0)
    add_compile_options(-DFMT_USE_CONSTEVAL=0)
endif()

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define the GEODE_SDK environment variable to point to your Geode SDK installation.")
else()
    message(STATUS "Found Geode SDK: $ENV{GEODE_SDK}")
endif()

project(NovaClientGD VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Load Geode SDK Subdirectory
add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

# Setup the Geode Mod Packaging
setup_geode_mod(${PROJECT_NAME})
