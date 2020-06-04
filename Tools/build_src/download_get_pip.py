import contextlib
import urllib.request
import sys
import os


_GET_PIP_URL = "https://bootstrap.pypa.io/get-pip.py"


def download_get_pip(location):
    global _GET_PIP_URL

    with contextlib.closing(urllib.request.urlopen(_GET_PIP_URL)) as get_pip_url_fd:
        with open(os.path.join(location, "get-pip.py"), "w") as get_pip_script_file_fd:
            get_pip_script_file_fd.write(get_pip_url_fd.read().decode())


if __name__ == "__main__":
    # Pass the location to download get-pip.py.
    download_get_pip(sys.argv[1])
