# This is a Python script to add library paths and include paths from
# "LIBRARY_PATH" and "C_INCLUDE_PATH"/"CPLUS_INCLUDE_PATH"/"CPATH" environment variables,
# respectively, if the original binary fails to read the previously mentioned environment
# variables (such as the case with LLVM targetting a platform different from the host
# platform).

import sys
import os
import subprocess
import shutil
import platform
SOURCE_LOCATION = os.path.abspath(__file__)
sys.path.insert(0, os.path.dirname(SOURCE_LOCATION))
from parse_path_envvar import parse_path_envvar  # noqa


def _get_cc_cxx_wrappers_tmp_dir():
    return os.path.join(os.environ["ALUSUS_TMP_DIR"], "cc_cxx_wrappers")


def copy_cc_cxx_wrapppers_to_tmp_dir(this_platform=platform.system()):
    wrapper_ext = "cmd" if this_platform == "Windows" else "sh"
    os.makedirs(_get_cc_cxx_wrappers_tmp_dir(), exist_ok=True)
    original_cc_cxx_wrappers_dir = os.path.join(os.path.dirname(
        SOURCE_LOCATION), "..", "cc_cxx_wrappers")
    new_cc_cxx_wrappers_dir = _get_cc_cxx_wrappers_tmp_dir()
    shutil.copy2(
        os.path.join(original_cc_cxx_wrappers_dir,
                     "cc_wrapper.{}".format(wrapper_ext)),
        os.path.join(new_cc_cxx_wrappers_dir,
                     "cc_wrapper.{}".format(wrapper_ext))
    )
    shutil.copy2(
        os.path.join(original_cc_cxx_wrappers_dir,
                     "cxx_wrapper.{}".format(wrapper_ext)),
        os.path.join(new_cc_cxx_wrappers_dir,
                     "cxx_wrapper.{}".format(wrapper_ext))
    )


def create_new_environ_with_custom_cc_cxx(environ, target_system=None, this_platform=platform.system()):
    global SOURCE_LOCATION

    new_environ = environ.copy()
    if not (target_system == "windows" or this_platform == "Windows" and not target_system):
        new_environ["ALUSUS_CC_CXX_USE_FPIC"] = "1"
    else:
        new_environ["ALUSUS_CC_CXX_USE_FPIC"] = "0"
    new_environ["ALUSUS_CUSTOM_CC_CXX_PYTHON_SCRIPT_PATH"] = SOURCE_LOCATION
    new_environ["ALUSUS_CC_BIN"] = new_environ["CC"] if "CC" in new_environ else "clang"
    new_environ["ALUSUS_CXX_BIN"] = new_environ["CXX"] if "CXX" in new_environ else "clang++"
    wrapper_ext = "cmd" if this_platform == "Windows" else "sh"
    new_cc_cxx_wrappers_dir = _get_cc_cxx_wrappers_tmp_dir()
    new_environ["CC"] = os.path.join(
        new_cc_cxx_wrappers_dir, "cc_wrapper.{}".format(wrapper_ext))
    new_environ["CXX"] = os.path.join(
        new_cc_cxx_wrappers_dir, "cxx_wrapper.{}".format(wrapper_ext))
    return new_environ


if __name__ == "__main__":
    bin_type = None
    if "ALUSUS_CC_CXX_BIN_TYPE" in os.environ and os.environ["ALUSUS_CC_CXX_BIN_TYPE"] == "CXX":
        bin_type = "CXX"
    else:
        bin_type = "CC"

    bin_arg = None
    if bin_type == "CC":
        bin_arg = os.environ["ALUSUS_CC_BIN"] if "ALUSUS_CC_BIN" in os.environ else "clang"
    else:
        bin_arg = os.environ["ALUSUS_CXX_BIN"] if "ALUSUS_CXX_BIN" in os.environ else "clang++"

    bin_args = sys.argv[1:]
    lib_paths_args = list()
    if "LIBRARY_PATH" in os.environ:
        lib_paths = parse_path_envvar(os.environ["LIBRARY_PATH"])
        for lib_path in lib_paths:
            if len(lib_path) > 0:
                lib_paths_args += [
                    "-L", lib_path
                ]
        os.environ.pop("LIBRARY_PATH")
    include_paths_args = list()
    if "C_INCLUDE_PATH" in os.environ:
        include_paths = parse_path_envvar(os.environ["C_INCLUDE_PATH"])
        for include_path in include_paths:
            if len(include_path) > 0:
                include_paths_args += [
                    "-I", include_path
                ]
        os.environ.pop("C_INCLUDE_PATH")
    if "CPLUS_INCLUDE_PATH" in os.environ:
        include_paths = parse_path_envvar(os.environ["CPLUS_INCLUDE_PATH"])
        for include_path in include_paths:
            if len(include_path) > 0:
                include_paths_args += [
                    "-I", include_path
                ]
        os.environ.pop("CPLUS_INCLUDE_PATH")
    if "CPATH" in os.environ:
        include_paths = parse_path_envvar(os.environ["CPATH"])
        for include_path in include_paths:
            if len(include_path) > 0:
                include_paths_args += [
                    "-I", include_path
                ]
        os.environ.pop("CPATH")
    if "ALUSUS_CC_CXX_USE_FPIC" in os.environ and os.environ["ALUSUS_CC_CXX_USE_FPIC"] == "1":
        bin_args.append("-fPIC")
    bin_args.append("-Wno-unused-command-line-argument")
    os._exit(subprocess.call([bin_arg] + bin_args +
                             include_paths_args + lib_paths_args))
