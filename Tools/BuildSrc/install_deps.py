from __future__ import print_function
import sys
import os

# fmt: off
# Alusus import(s).
sys.path.insert(0, os.path.dirname(__file__))
import common
# fmt: on


if __name__ == "__main__":
    exit(common.subprocess_run_hidden_except_on_error(
        [
            sys.executable, "-m", "pip", "install", "-r", os.path.join(
                common.ALUSUS_TOOLS_DIR, "BuildSrc", "requirements.txt")
        ]).returncode)
