cmake_minimum_required(VERSION 3.30)

project(condo)

add_executable(condo main.cpp thread_pool.h thread_pool.cpp)
target_compile_features(condo PRIVATE cxx_std_23)
