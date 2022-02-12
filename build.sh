#!/bin/sh

# cmake -B build -DCMAKE_CROSSCOMPILING=True -DCMAKE_TOOLCHAIN_FILE=win.toolchain.cmake src
export CC=gcc
export CXX=g++
#export CC=clang
#export CXX=clang++

cmake -DCMAKE_BUILD_TYPE=Debug -B build src
VERBOSE=1 cmake --build build --parallel  # --clean-first
