#!/bin/bash

# Give users an option to select a different Python 3 interpreter.
if [ -z "$ALUSUS_PYTHON_INTERP" ]; then
    export ALUSUS_PYTHON_INTERP=python3
fi

# Set the the mode to "CC".
export ALUSUS_CC_CXX_BIN_TYPE="CC"

"$ALUSUS_PYTHON_INTERP" "$ALUSUS_CUSTOM_CC_CXX_PYTHON_SCRIPT_PATH" "$@"

exit $?
