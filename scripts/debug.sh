#!/bin/bash

set -e

GDBINIT="../debug/.gdbinit"
BUILD_DIR="../build"

cd $BUILD_DIR

# Inside ../build but running gdb with given arguments.
gdb -x $GDBINIT --args $@