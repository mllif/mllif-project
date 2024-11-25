#!/usr/bin/env bash

# Goto test environment
PREDIR="$(pwd)"
cd "$(dirname $(realpath $0))"

# Compile test source with plugin
(clang++ -fplugin="$1/build/lib/lilac-c.so" -emit-llvm -c main.cxx -o main.bc && llvm-dis main.bc && rm main.bc) || exit 1
clang++ -fuse-ld=lld -fpass-plugin="$1/build/lib/lilac-ir.so" -Werror main.ll -o main || exit 1

# Restore environment
cd "$PREDIR"
