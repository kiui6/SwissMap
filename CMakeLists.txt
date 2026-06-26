cmake_minimum_required(VERSION 3.12)

project ("FlatHashMap" VERSION 1.0.0 LANGUAGES CXX)

add_library(FlatHashMap INTERFACE)

target_include_directories(FlatHashMap INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(FlatHashMap INTERFACE cxx_std_20)

if(ENABLE_TESTS)
    add_subdirectory(tests)
endif()