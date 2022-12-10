#!/bin/bash

# We have migrated the build scripts to Python 3.4+ (we will not support any Python version lower than that).
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
SCRIPT_DIR="$SCRIPT_DIR"
shopt -s expand_aliases
PYTHON_EXECUTABLE=python3
if [ ! $(which $PYTHON_EXECUTABLE) ]; then
    PYTHON_EXECUTABLE=python
fi
$PYTHON_EXECUTABLE -m pip install -r "$SCRIPT_DIR/BuildSrc/requirements.txt" &&\
$PYTHON_EXECUTABLE "$SCRIPT_DIR/BuildSrc/build.py" "$@"
exit $?
