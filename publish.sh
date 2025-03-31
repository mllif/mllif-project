#!/bin/bash

CC="$(which clang)"

rm -r cmake-build

cmake -B "cmake-build" -G "Ninja" -DCMAKE_BUILD_TYPE="Release" -DCMAKE_C_COMPILER="$CC" -DCMAKE_CXX_COMPILER="$CC++" || exit 1
cmake --build "cmake-build" || exit 1
cmake -DCMAKE_INSTALL_PREFIX="cmake-build/build" -P "cmake-build/cmake_install.cmake" || exit 1

tar -cJvf "mllif.tar.xz" "cmake-build/build" || exit 1
stat "mllif.tar.xz"
