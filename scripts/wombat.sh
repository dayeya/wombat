#!/bin/bash

set -e

EXEC="./wombat"
BUILD_DIR="../build"

cd $BUILD_DIR

$EXEC $@