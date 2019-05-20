#!/bin/bash

# We have migrated the build scripts to Python 3.4+ (we will not support any Python version lower than that).
ALUSUS_BUILD_SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
ALUSUS_BUILD_SCRIPT_DIR="$ALUSUS_BUILD_SCRIPT_DIR"
# Give users an option to select a different Python 3 interpreter.
if [ -z "${ALUSUS_PYTHON_INTERP}" ]; then
    export ALUSUS_PYTHON_INTERP=python3
fi
"$ALUSUS_PYTHON_INTERP" "$ALUSUS_BUILD_SCRIPT_DIR/BuildSrc/build.py" "$@"
exit $?
