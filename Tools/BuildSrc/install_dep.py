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
        sys.path.insert(0, local_deps_path)

def install_pip(prefix, verbose=False):
    try:
        add_local_site_packages_to_path(prefix)
        import_module('pip')
        return True
    except (ModuleNotFoundError, ImportError):
        install_location_args = ['--prefix', prefix]
        script_dir = os.path.dirname(os.path.realpath(__file__))
        cmd = [sys.executable, os.path.join(script_dir, 'get-pip.py'), '--ignore-installed'] + install_location_args
        ret = 0
        if not verbose:
            with open(os.devnull, "w") as f:
                ret = subprocess.call(cmd, stdout=f, stderr=subprocess.STDOUT)
        else:
            ret = subprocess.call(cmd)
        if ret != 0:
            raise subprocess.CalledProcessError(ret, cmd)
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
        original_path = os.getcwd()
        os.chdir(local_deps_path)
        cmd = [sys.executable, '-m', 'pip', 'install', '--ignore-installed'] + install_location_args + [package]
        ret = 0
        if not verbose:
            with open(os.devnull, "w") as f:
                p = subprocess.Popen(cmd, stdout=f, stderr=subprocess.STDOUT)
                ret = p.wait()
        else:
            p = subprocess.Popen(cmd)
            ret = p.wait()
        if ret != 0:
            os.chdir(original_path)
            raise subprocess.CalledProcessError(ret, cmd)
        add_local_site_packages_to_path(prefix)
        os.chdir(original_path)
        return False
