#!/bin/bash

# We have migrated the build scripts to Python 3.4+ (we will not support any Python version lower than that).

# Set the script directory.
ALUSUS_BUILD_SCRIPT_DIR="$(dirname $0)"

# Give users an option to select a different Python 3 interpreter.
if [ -z "$ALUSUS_PYTHON_INTERP" ]; then
    export ALUSUS_PYTHON_INTERP=python3
fi

# Give users an option to select a different path to cache the build script dependencies.
if [ -z "$ALUSUS_PYTHONPATH" ]; then
    export ALUSUS_PYTHONPATH=$HOME/.alusus_pythonpath
fi

# Give users an option to select a different directory to put temporary files.
if [ -z "$ALUSUS_TMP_DIR" ]; then
    export ALUSUS_TMP_DIR=$HOME/.alusus_tmp_dir
fi

mkdir -p "$ALUSUS_PYTHONPATH" &> /dev/null

# Set PYTHONPATH.
if [ -z "$PYTHONPATH" ]; then
    export PYTHONPATH="$ALUSUS_PYTHONPATH"
else
    export PYTHONPATH="$ALUSUS_PYTHONPATH:$PYTHONPATH"
fi

# Install Pip.
"$ALUSUS_PYTHON_INTERP" -c "import pkg_resources; pkg_resources.require(\"pip==20.1.1\")" &> /dev/null
if [ $? -ne 0 ]; then
    # Download the "get-pip.py" script
    "$ALUSUS_PYTHON_INTERP" "$ALUSUS_BUILD_SCRIPT_DIR/BuildSrc/download_get_pip.py" "$ALUSUS_PYTHONPATH"
    
    # Install Pip 20.1.1.
    "$ALUSUS_PYTHON_INTERP" "$ALUSUS_PYTHONPATH/get-pip.py" "pip==20.1.1" --target "$ALUSUS_PYTHONPATH"
    
    # Delete "get-pip.py".
    rm "$ALUSUS_PYTHONPATH/get-pip.py"
fi

# Install all other dependencies.
"$ALUSUS_PYTHON_INTERP" -c "import pkg_resources; pkg_resources.require(\"ummalqura==2.0.1\")" &> /dev/null
if [ $? -ne 0 ]; then
    "$ALUSUS_PYTHON_INTERP" -m pip install ummalqura==2.0.1 --target "$ALUSUS_PYTHONPATH"
fi
"$ALUSUS_PYTHON_INTERP" -c "import pkg_resources; pkg_resources.require(\"termcolor==1.1.0\")" &> /dev/null
if [ $? -ne 0 ]; then
    "$ALUSUS_PYTHON_INTERP" -m pip install termcolor==1.1.0 --target "$ALUSUS_PYTHONPATH"
fi
"$ALUSUS_PYTHON_INTERP" -c "import pkg_resources; pkg_resources.require(\"wget==3.2\")" &> /dev/null
if [ $? -ne 0 ]; then
    "$ALUSUS_PYTHON_INTERP" -m pip install wget==3.2 --target "$ALUSUS_PYTHONPATH"
fi
"$ALUSUS_PYTHON_INTERP" -c "import pkg_resources; pkg_resources.require(\"whichcraft==0.6.1\")" &> /dev/null
if [ $? -ne 0 ]; then
    "$ALUSUS_PYTHON_INTERP" -m pip install whichcraft==0.6.1 --target "$ALUSUS_PYTHONPATH"
fi

"$ALUSUS_PYTHON_INTERP" "$ALUSUS_BUILD_SCRIPT_DIR/BuildSrc/build.py" "$@"

exit $?
