cmake_minimum_required(VERSION 3.22)
project(superball LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS ON)
set(SFML_BUILD_NETWORK OFF)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON)
FetchContent_MakeAvailable(SFML)

add_library(superball SHARED main.cpp)
target_include_directories(superball PRIVATE headers)
target_precompile_headers(superball PRIVATE headers/pch.hpp)
target_link_libraries(superball PRIVATE
    SFML::Graphics SFML::Audio SFML::Window SFML::System SFML::Main)

if(ANDROID)
    target_link_options(superball PRIVATE -u ANativeActivity_onCreate)
endif()
