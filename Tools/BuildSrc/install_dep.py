try:
    from importlib import reload, import_module
except ImportError:
    from importlib import import_module
import sys
import subprocess
import site
import os


# Install Pip and the requested package for the user (no root or admin privileges required).
def install_package(package, verbose=False):
    # Install pip.
    try:
        import pip
    except ImportError:
        if not verbose:
            with open(os.devnull, "w") as f:
                subprocess.call([sys.executable, os.path.join(os.path.realpath(os.path.dirname(os.path.realpath(__file__))), 'get-pip.py'), '--user'], stdout=f, stderr=subprocess.STDOUT)
                subprocess.call([sys.executable, '-m', 'pip', 'install', '--upgrade', 'pip' '--user'], stdout=f, stderr=subprocess.STDOUT)
        else:
            subprocess.call([sys.executable, os.path.join(os.path.realpath(os.path.dirname(os.path.realpath(__file__))), 'get-pip.py'), '--user'])
            subprocess.call([sys.executable, '-m', 'pip', 'install', '--upgrade', 'pip' '--user'])
        reload(site)
    
    # Install the package.
    try:
        import_module(package)
    except ImportError:
        if not verbose:
            with open(os.devnull, "w") as f:
                subprocess.call([sys.executable, '-m', 'pip', 'install', '--user', package], stdout=f, stderr=subprocess.STDOUT)
        else:
            subprocess.call([sys.executable, '-m', 'pip', 'install', '--user', package])
        reload(site)
