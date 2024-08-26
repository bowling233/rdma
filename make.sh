#!/bin/bash
#rm -rf build
#mkdir build
cmake -S . -B build
cmake --build build

cd build

for i in '0' '64' '1k' '2k' '4k' '8k'
do
    ./pt_${i} > ../result/pt_${i}.txt
done
