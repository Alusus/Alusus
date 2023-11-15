#!/bin/bash

# We have migrated the build scripts to Python 3.4+ (we will not support any Python version lower than that).

SOURCE=${BASH_SOURCE[0]}
while [ -L "$SOURCE" ]; do # Resolve "$SOURCE" until the file is no longer a symlink.
    DIR=$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )
    SOURCE=$(readlink "$SOURCE")
    [[ $SOURCE != /* ]] && SOURCE=$DIR/$SOURCE # If "$SOURCE" was a relative symlink, we need to resolve it
                                               # relative to the path where the symlink file was located.
done
SCRIPT_DIR=$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )

shopt -s expand_aliases
if [ -z "$ALUSUS_PYTHON_EXECUTABLE" ]; then
    PYTHON_EXECUTABLE=python3
else
    PYTHON_EXECUTABLE="$ALUSUS_PYTHON_EXECUTABLE"
fi
if [ ! $(which $PYTHON_EXECUTABLE) ]; then
    PYTHON_EXECUTABLE=python
fi
$PYTHON_EXECUTABLE "$SCRIPT_DIR/BuildSrc/install_deps.py" &&\
$PYTHON_EXECUTABLE "$SCRIPT_DIR/BuildSrc/build.py" "$@"
exit $?
