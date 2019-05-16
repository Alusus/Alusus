from install_dep import install_package

def install_all_deps(prefix):
    # Add your Python dependencies here.
    install_package('wget', prefix)
    install_package('colorama', prefix)
    install_package('termcolor', prefix)
    install_package('ummalqura', prefix)
