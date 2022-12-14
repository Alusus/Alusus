from __future__ import print_function
import subprocess
import sys
import os

# Alusus imports.
import alusus_common


if __name__ == "__main__":
    output = subprocess.run([sys.executable, "-m", "pip", "install",
                            "-r", os.path.join(alusus_common.ALUSUS_TOOLS_DIR, "BuildSrc", "requirements.txt")],
                            stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if output.returncode:
        alusus_common.eprint(output.stderr.decode(), end='')
        print(output.stdout.decode(), end='')
    exit(output.returncode)
