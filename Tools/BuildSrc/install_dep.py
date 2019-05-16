import sys
from importlib import import_module
import subprocess
import site
import os
import platform

def get_local_site_packages(prefix):
    local_deps_path = os.path.join(prefix, 'Lib','site-packages')
    if platform.system() != "Windows":
        local_deps_path = os.path.join(prefix, 'lib','python{major}.{minor}'.format(
            major=platform.python_version_tuple()[0], minor=platform.python_version_tuple()[1]
        ), 'site-packages')
    return local_deps_path

def add_local_site_packages_to_path(prefix):
    local_deps_path = get_local_site_packages(prefix)
    if not os.path.exists(local_deps_path):
        os.makedirs(local_deps_path)
    if local_deps_path not in sys.path:
        sys.path.append(local_deps_path)

def install_pip(prefix, verbose=False):
    local_deps_path = get_local_site_packages(prefix)
    try:
        add_local_site_packages_to_path(prefix)
        import_module('pip')
        return True
    except (ModuleNotFoundError, ImportError):
        install_location_args = ['--prefix', prefix]
        script_dir = os.path.dirname(os.path.realpath(__file__))
        cmd = [sys.executable, os.path.join(script_dir, 'get-pip.py'), '--ignore-installed'] + install_location_args
        if not verbose:
            with open(os.devnull, "w") as f:
                subprocess.call(cmd, stdout=f, stderr=subprocess.STDOUT)
        else:
            subprocess.call(cmd)
        add_local_site_packages_to_path(prefix)
        return False

def install_package(package, prefix, verbose=False):
    local_deps_path = get_local_site_packages(prefix)
    try:
        add_local_site_packages_to_path(prefix)
        import_module(package)
        return True
    except (ModuleNotFoundError, ImportError):
        install_pip(prefix, verbose=verbose)
        install_location_args = ['--prefix', prefix]
        cmd = [sys.executable, '-m', 'pip', 'install', '--ignore-installed'] + install_location_args + [package]
        if not verbose:
            with open(os.devnull, "w") as f:
                p = subprocess.Popen(cmd, stdout=f, stderr=subprocess.STDOUT, env={'PYTHONPATH': local_deps_path})
                p.wait()
        else:
            p = subprocess.Popen(cmd, env={'PYTHONPATH': local_deps_path})
            p.wait()
        add_local_site_packages_to_path(prefix)
        return False
