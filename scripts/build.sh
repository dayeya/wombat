#!/bin/bash

set -e

DEBUG_DIR="../debug"
BUILD_DIR="../build"

cd $BUILD_DIR

# Inside ../build
cmake .
cmake --build .

echo "CMake build complete"