#!/bin/bash

# We have migrated the build scripts to Python 3.4+ (we will not support any Python version lower than that).
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
SCRIPT_DIR="$SCRIPT_DIR"
shopt -s expand_aliases
python3 "$SCRIPT_DIR/BuildSrc/install_dep.py" "$@"
python3 "$SCRIPT_DIR/BuildSrc/build.py" "$@"
exit $?
