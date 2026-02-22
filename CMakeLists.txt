cmake_minimum_required(VERSION 3.30)

project(condo)

add_library(thread_pool INTERFACE)
target_compile_features(thread_pool INTERFACE cxx_std_23)
target_include_directories(thread_pool INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(condo main.cpp)
target_compile_features(condo PRIVATE cxx_std_23)
target_link_libraries(condo PRIVATE thread_pool)
