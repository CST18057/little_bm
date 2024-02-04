#!/bin/bash

cmake -S . -B build/ -D CMAKE_BUILD_TYPE=Release && cd build && make -j32 || exit
for file in bm*
do
  taskset -c 0 ./$file || exit
done