#!/bin/bash
module load cmake
mkdir build
cd build
cmake -D IRIDIS=1 -D STATICitppLib=1 ../
make
