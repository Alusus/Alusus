from __future__ import print_function
import subprocess
import sys
import os

# Alusus import(s).
import alusus_common


if __name__ == "__main__":
    exit(alusus_common.subprocess_run_hidden_except_on_error(
        [
            sys.executable, "-m", "pip", "install", "-r", os.path.join(
                alusus_common.ALUSUS_TOOLS_DIR, "BuildSrc", "requirements.txt")
        ]).returncode)
