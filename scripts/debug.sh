#!/bin/bash

set -e

GDBINIT="../debug/.gdbinit"
BUILD_DIR="../build"

cd $BUILD_DIR

# Inside ../build but running gdb with given arguments.
# gdb -x $GDBINIT --args $@
gdb -x $GDBINIT --args ./wombat --build ../examples/test.wo