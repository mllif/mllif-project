#!/usr/bin/env bash

# Goto test environment
PREDIR="$(pwd)"
cd "$(dirname $(realpath $0))"

# Compile test source with plugin
(clang++ -fplugin="$1/build/lib/MLLIFCFront.so" -g -S -emit-llvm -Werror main.cxx -o main.ll) || exit 1

# Restore environment
cd "$PREDIR"
