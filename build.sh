#!/bin/bash

cmake -S . -B build/ -D CMAKE_BUILD_TYPE=Release && cd build && make -j32
for file in bm*
do
  ./$file
done