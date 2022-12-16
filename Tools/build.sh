#!/bin/bash

# We have migrated the build scripts to Python 3.4+ (we will not support any Python version lower than that).

SOURCE=${BASH_SOURCE[0]}
while [ -L "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
    DIR=$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )
    SOURCE=$(readlink "$SOURCE")
    [[ $SOURCE != /* ]] && SOURCE=$DIR/$SOURCE # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
SCRIPT_DIR=$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )

shopt -s expand_aliases
PYTHON_EXECUTABLE=python3
if [ ! $(which $PYTHON_EXECUTABLE) ]; then
    PYTHON_EXECUTABLE=python
fi
$PYTHON_EXECUTABLE "$SCRIPT_DIR/BuildSrc/install_deps.py" &&\
$PYTHON_EXECUTABLE "$SCRIPT_DIR/BuildSrc/build.py" "$@"
exit $?
