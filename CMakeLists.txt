cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(little_benchmark)

include(cmake/CPM.cmake)

if (NOT EXISTS ${CMAKE_BINARY_DIR}/CMakeCache.txt)
  if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
  endif()
endif()

set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS}  -g -O3 -ffast-math -Wall -march=native -fno-omit-frame-pointer")

CPMAddPackage(
  NAME benchmark
  GITHUB_REPOSITORY google/benchmark
  VERSION 1.8.3
  OPTIONS "BENCHMARK_ENABLE_TESTING Off"
)

if(benchmark_ADDED)
  # enable c++11 to avoid compilation errors
  set_target_properties(benchmark PROPERTIES CXX_STANDARD 17)
endif()

# ---- Executable ----

add_executable(bm_hamming_weight "hamming_weight/hamming_weight.cc")
target_link_libraries(bm_hamming_weight benchmark)
target_compile_features(bm_hamming_weight PRIVATE cxx_std_20)

add_executable(bm_hamming_weight32 "hamming_weight/hamming_weight32.cc")
target_link_libraries(bm_hamming_weight32 benchmark)
target_compile_features(bm_hamming_weight32 PRIVATE cxx_std_20)

add_executable(bm_histgram "histgram/histgram.cc")
target_link_libraries(bm_histgram benchmark)
target_compile_features(bm_histgram PRIVATE cxx_std_20)