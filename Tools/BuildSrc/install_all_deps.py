from install_dep import install_package
import platform

def install_all_deps(prefix, verbose=False):
    # Add your Python dependencies here.
    install_package('wget', prefix, verbose=verbose)
    install_package('colorama', prefix, verbose=verbose)
    install_package('termcolor', prefix, verbose=verbose)
    install_package('ummalqura', prefix, verbose=verbose)
