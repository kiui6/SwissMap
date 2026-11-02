cmake_minimum_required(VERSION 3.12)

project ("SwissMap" VERSION 1.0.0 LANGUAGES CXX)

add_library(SwissMap INTERFACE)

target_include_directories(SwissMap INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(SwissMap INTERFACE cxx_std_20)

if(ENABLE_TESTS)
    message(STATUS "Tests enabled")
    add_subdirectory(tests)
endif()