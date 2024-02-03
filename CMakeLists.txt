cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(little_benchmark)

include(cmake/CPM.cmake)

set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS}  -g -O2 -fno-omit-frame-pointer")

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

add_executable(hamming_weight "hamming_weight/hamming_weight.cc")
target_link_libraries(hamming_weight benchmark)
target_compile_features(hamming_weight PRIVATE cxx_std_20)