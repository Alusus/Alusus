import os
import sys
import subprocess

if __name__ == "__main__":
    alusus_defines_include_dir = sys.argv[1]
    host_tripple = subprocess.check_output([
        (os.environ["CC"] if "CC" in os.environ else "clang"),
        "-dumpmachine"
    ]).strip()
    install_include_dir = sys.argv[2]
    install_bin_dir = sys.argv[3]
    install_lib_dir = sys.argv[4]

    to_write_text =\
        "#ifndef ALUSUS_COMPILE_DEFINES_H{}".format(os.linesep) +\
        "#define ALUSUS_COMPILE_DEFINES_H{}".format(os.linesep) +\
        "#define ALUSUS_HOST_TRIPPLE \"{0}\"{1}".format(host_tripple, os.linesep) +\
        "#define ALUSUS_INSTALL_INCLUDE_DIR \"{0}\"{1}".format(install_include_dir, os.linesep) +\
        "#define ALUSUS_INSTALL_BIN_DIR \"{0}\"{1}".format(install_bin_dir, os.linesep) +\
        "#define ALUSUS_INSTALL_LIB_DIR \"{0}\"{1}".format(install_lib_dir, os.linesep) +\
        "#endif{}".format(os.linesep) +\
        "{}".format(os.linesep)

    os.makedirs(alusus_defines_include_dir, exist_ok=True)
    header_path = os.path.join(os.path.join(
        alusus_defines_include_dir, "AlususCompileDefines.h"))
    write_new_header = False
    if not os.path.exists(header_path):
        write_new_header = True
    else:
        with open(header_path, "r") as fd:
            write_new_header = fd.read() != to_write_text
    if write_new_header:
        with open(header_path, "w") as fd:
            fd.write(to_write_text)
