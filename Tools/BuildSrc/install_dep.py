try:
    from importlib import reload, import_module
except ImportError:
    from importlib import import_module
import sys
import subprocess
import site
import os
import pip


# Install Pip and the requested package for the user (no root or admin privileges required).
def install_package(package, verbose=False):
    # Install the package.
    try:
        import_module(package)
    except ImportError:
        if not verbose:
            with open(os.devnull, "w") as f:
                subprocess.call([sys.executable, '-m', 'pip', 'install',
                                 '--user', package], stdout=f, stderr=subprocess.STDOUT)
        else:
            subprocess.call([sys.executable, '-m', 'pip',
                             'install', '--user', package])
        reload(site)


install_package('argparse')
install_package('termcolor')
install_package('wget')
install_package('colorama')
