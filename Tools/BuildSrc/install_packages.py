from __future__ import print_function
import subprocess
from alusus_common import ALUSUS_TOOLS_DIR
import sys
import os


def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)


if __name__ == "__main__":
    output = subprocess.run([sys.executable, "-m", "pip", "install",
                            "-r", os.path.join(ALUSUS_TOOLS_DIR, "BuildSrc", "requirements.txt")],
                            stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if output.returncode:
        eprint(output.stderr.decode(), end='')
        print(output.stdout.decode(), end='')
    exit(output.returncode)
