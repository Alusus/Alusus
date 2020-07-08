#!/bin/bash

# We have migrated the build scripts to Python 3.4+ (we will not support any Python version lower than that).

# Set the script directory.
ALUSUS_BUILD_SCRIPT_DIR="$(dirname $0)"

# Give users an option to select a different Python 3 interpreter.
if [ -z "$ALUSUS_PYTHON_INTERP" ]; then
    export ALUSUS_PYTHON_INTERP=python3
fi

"$ALUSUS_PYTHON_INTERP" "$ALUSUS_BUILD_SCRIPT_DIR/BuildSrc/create_dylib.py" "$@"

exit $?
