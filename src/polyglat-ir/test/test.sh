#!/usr/bin/env bash

# Goto test environment
PREDIR="$(pwd)"
cd "$(dirname $(realpath $0))"

# Compile test source with plugin
(clang++ -fplugin="$1/build/lib/polyglat-c.so" -emit-llvm -c main.cxx -o main.bc && llvm-dis main.bc && rm main.bc) || exit 1
(opt -load-pass-plugin="$1/build/lib/polyglat-ir.so" --passes=polyglat --polyglat-output main.psm main.ll -o main.bc && llvm-dis main.bc && rm main.bc) || exit 1

# Restore environment
cd "$PREDIR"
