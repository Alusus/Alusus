import os
import platform
import sys
import argparse
import shlex
import multiprocessing
import hashlib
import subprocess
import shutil
SOURCE_LOCATION = os.path.abspath(__file__)
sys.path.insert(0, os.path.dirname(SOURCE_LOCATION))
from build_deps import build_ghc_filesystem, build_dlfcn_win32, build_libcurl, build_libzip, build_llvm  # noqa
from version_info import get_version_info  # noqa
from msg import info_msg, success_msg, fail_msg, warn_msg  # noqa
from custom_cc_cxx import create_new_environ_with_custom_cc_cxx, copy_cc_cxx_wrapppers_to_tmp_dir  # noqa
from utils import get_host_cxx_arch, delete_tmp_dir, get_lib_path, get_to_copy_libs, which  # noqa
from set_rpath import set_rpath


def parse_args():
    parser = argparse.ArgumentParser(add_help=False,
                                     formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument("build-path", metavar="BUILD_PATH", help="Set the build path.",
                        default=None)
    parser.add_argument("-h", "--help", action="help", default=argparse.SUPPRESS,
                        help="Show this help message and exit.")
    parser.add_argument("--build-type", metavar="BUILD_TYPE", help="Set the build type of Alusus. Possible values:\n" +
                        "    d (default): debug build.\n" +
                        "    r: release build.",
                        choices=["d", "r"], required=False, default="d")
    parser.add_argument("--build-packages", help="Create installable package(s) and packaged archives (default: False).",
                        action="store_true", required=False, default=False)
    parser.add_argument("--install-path", metavar="INSTALL_PATH", help="Set the install path (default: {}).".format(
        shlex.quote(os.path.join("[build-path]",
                                 "LocalInstall", "[Debug|Release]"))
    ),
        required=False, default=None)
    parser.add_argument("--install-include-name", metavar="INSTALL_INCLUDE_NAME", help="Set the install include directory name (default: Include).",
                        required=False, default="Include")
    parser.add_argument("--install-lib-name", metavar="INSTALL_LIB_NAME", help="Set the install library directory name (default: Lib).",
                        required=False, default="Lib")
    parser.add_argument("--install-bin-name", metavar="INSTALL_BIN_NAME", help="Set the install binary directory name (default: Bin).",
                        required=False, default="Bin")
    parser.add_argument("--num-threads", metavar="NUM_THREADS",
                        help="Specify a custom number of threads to use when building dependencies " +
                             "and Alusus (default: {}).".format(
                                 multiprocessing.cpu_count()),
                        type=int, required=False, default=multiprocessing.cpu_count())

    parser.add_argument("--target", metavar="TARGET", help="Cross compile target host name. Choose one of the following:\n" +
                        "    1. linux: Build for Linux.\n" +
                        "    2. windows: Build for Windows.\n" +
                        "    3. macos: Build for macOS.",
                        required=False, default=None, choices=["linux", "windows", "macos"])
    args = parser.parse_args()
    if "target" not in args:
        args.target = None

    # No need to cross-compile to the same architecture we are currently running the build script in. We can natively build
    # instead to speedup build time.
    if args.target and\
       ((args.target == "windows" and platform.system() == "Windows") or
        (args.target == "linux" and platform.system() == "Linux") or
            (args.target == "macos" and platform.system() == "Darwin")):
        args.target = None
    args.build_path = getattr(args, "build-path")
    del(args.__dict__["build-path"])
    args.build_path = os.path.abspath(args.build_path)
    if not args.install_path:
        args.install_path = {
            "root": os.path.join(args.build_path, "LocalInstall", "Debug" if args.build_type == "d" else "Release"),
            "include": args.install_include_name,
            "lib": args.install_lib_name,
            "bin": args.install_bin_name
        }
    else:
        args.install_path = {
            "root": args.install_path,
            "include": args.install_include_name,
            "lib": args.install_lib_name,
            "bin": args.install_bin_name
        }
    del args.install_include_name
    del args.install_lib_name
    del args.install_bin_name
    return args


def generate_paths(build_path, install_path, build_type):
    global SOURCE_LOCATION

    build_type_folder = "Debug" if build_type == "d" else "Release"
    to_return = {}
    to_return["alusus_root_path"] = os.path.dirname(
        os.path.dirname(os.path.dirname(SOURCE_LOCATION)))
    to_return["builds_path"] = os.path.join(
        build_path, "Builds", build_type_folder)
    to_return["install_path"] = install_path
    to_return["deps_path"] = os.path.join(build_path, "Builds", "Dependencies")
    to_return["pkgs_path"] = os.path.join(
        build_path, "Builds", "Packages", build_type_folder)
    return to_return


def build_deps(deps_path, install_path, num_threads=multiprocessing.cpu_count(), target_system=None):
    ret = build_llvm.build_llvm.build(
        deps_path, install_path, num_threads=num_threads, target_system=target_system)
    if not ret:
        return ret
    ret = build_libcurl.build_libcurl.build(
        deps_path, install_path, num_threads=num_threads, target_system=target_system)
    if not ret:
        return ret
    ret = build_libzip.build_libzip.build(
        deps_path, install_path, num_threads=num_threads, target_system=target_system)
    if not ret:
        return ret

    # In case a toolchain doesn't support std::filesystem yet (e.g. LLVM MinGW toolchain).
    ret = build_ghc_filesystem.build_ghc_filesystem.build(
        deps_path, install_path, num_threads=num_threads, target_system=target_system)
    if not ret:
        return ret

    if platform.system() == "Windows" and not target_system or target_system == "windows":
        ret = build_dlfcn_win32.build_dlfcn_win32.build(
            deps_path, install_path, num_threads=num_threads, target_system=target_system)
        if not ret:
            return ret
    return True


def post_build_alusus(install_path, target_system=None, environ=os.environ):
    # Copy the libraries pointed by "ALUSUS_TO_COPY_LIBS".
    libs = get_to_copy_libs(environ=environ)
    for lib in libs:
        paths = get_lib_path(
            lib, target_system=target_system, environ=environ)
        if paths:
            shutil.copy2(
                os.path.abspath(paths["lib_path"]),
                os.path.abspath(os.path.join(
                    install_path["root"], install_path["lib"]))
            )
            if "dll_paths" in paths:
                for dll_path in paths["dll_paths"]:
                    shutil.copy2(
                        os.path.abspath(dll_path),
                        os.path.abspath(os.path.join(
                            install_path["root"], install_path["bin"]))
                    )
    if not (platform.system() == "Windows" and not target_system or target_system == "windows"):
        # To indicate it is currently updating the RPATHs.
        warn_msg("Updating the RPATHs of the shared libraries (DO NOT interrupt the build process)...")
        with open(os.path.join(environ["ALUSUS_TMP_DIR"], "UPDATING_RPATHS"), "w") as fd:
            fd.write("SETTING RPATH INDICATOR")
        if platform.system() == "Darwin" and not target_system or target_system == "macos":
            set_rpath(
                arg_dir=[
                    os.path.join(os.path.join(install_path["root"], install_path["lib"])),
                    os.path.join(os.path.join(install_path["root"], install_path["bin"])),
                ],
                arg_rpath=[
                    "@executable_path/",
                    "@executable_path/../{}/".format(install_path["lib"])
                ],
                target_system=target_system,
                environ=environ
            )
        elif platform.system() == "Linux" and not target_system or target_system == "linux":
            set_rpath(
                arg_dir=[
                    os.path.join(os.path.join(install_path["root"], install_path["lib"])),
                    os.path.join(os.path.join(install_path["root"], install_path["bin"])),
                ],
                arg_rpath=[
                    "$ORIGIN/",
                    "$ORIGIN/../{}/".format(install_path["lib"])
                ],
                target_system=target_system,
                environ=environ
            )

        # To indicate it has done updating the RPATHs.
        os.remove(os.path.join(environ["ALUSUS_TMP_DIR"], "UPDATING_RPATHS"))
        success_msg("Updating the RPATHs of the shared libraries.")


def build_alusus(deps_path, builds_path, alusus_root_path, install_path, build_type, num_threads=multiprocessing.cpu_count(), target_system=None):
    copy_cc_cxx_wrapppers_to_tmp_dir()

    current_version_info = get_version_info()
    info_msg("Building Alusus {}...".format(current_version_info[0]))

    info_msg("Building dependencies for Alusus {}...".format(
        current_version_info[0]))
    ret = build_deps(deps_path, install_path,
                     num_threads=num_threads, target_system=target_system)
    if not ret:
        delete_tmp_dir()
        return ret
    success_msg("Building dependencies for Alusus {}.".format(
        current_version_info[0]))

    os.makedirs(deps_path, exist_ok=True)
    os.makedirs(builds_path, exist_ok=True)
    os.makedirs(install_path["root"], exist_ok=True)

    original_dir = os.getcwd()
    os.chdir(builds_path)

    # Set the required build environment variables.
    new_environ = os.environ.copy()
    host_sep = ":" if platform.system() != "Windows" else ";"
    ghc_filesystem_environ = build_ghc_filesystem.build_ghc_filesystem.get_dep_environ(
        deps_path, target_system=target_system)
    llvm_environ = build_llvm.build_llvm.get_dep_environ(
        deps_path, target_system=target_system)
    new_cpath_value = llvm_environ["CPATH"] + \
        host_sep + ghc_filesystem_environ["CPATH"]
    new_library_path_value = llvm_environ["LIBRARY_PATH"]
    if target_system == "windows" or platform.system() == "Windows" and not target_system:
        dlfcn_win32_environ = build_dlfcn_win32.build_dlfcn_win32.get_dep_environ(
            deps_path, target_system=target_system)
        new_cpath_value += host_sep + \
            dlfcn_win32_environ["CPATH"]
        new_library_path_value += host_sep + \
            dlfcn_win32_environ["LIBRARY_PATH"]
    new_environ["CPATH"] = new_cpath_value + \
        ("" if ("CPATH" not in new_environ)
         else (host_sep + new_environ["CPATH"]))
    new_environ["LIBRARY_PATH"] = new_library_path_value + \
        ("" if ("LIBRARY_PATH" not in new_environ)
         else (host_sep + new_environ["LIBRARY_PATH"]))
    new_environ = create_new_environ_with_custom_cc_cxx(
        new_environ, target_system=target_system)

    os.makedirs(os.path.join(
        install_path["root"], install_path["lib"]), exist_ok=True)
    os.makedirs(os.path.join(
        install_path["root"], install_path["bin"]), exist_ok=True)
    os.makedirs(os.path.join(
        install_path["root"], install_path["include"]), exist_ok=True)
    cmake_system_name = "Windows" if (target_system == "windows" or platform.system() == "Windows" and not target_system) else (
        "Darwin" if (target_system == "macos" or platform.system() == "Darwin" and not target_system) else "Linux")
    cmake_cmd = ["cmake",
                 os.path.join(alusus_root_path, "Sources"),
                 "-Wno-dev",
                 "-DCMAKE_CROSSCOMPILING=TRUE",
                 "-DCMAKE_SYSTEM_NAME={}".format(cmake_system_name),
                 "-DCMAKE_SYSTEM_PROCESSOR={}".format(
                     get_host_cxx_arch(new_environ=new_environ)),
                 "-DCMAKE_BUILD_TYPE={}".format(
                     "Release" if build_type == "r" else "Debug"),
                 "-DPYTHON_EXECUTABLE={}".format(sys.executable),
                 "-DCMAKE_INSTALL_PREFIX={}".format(install_path["root"]),
                 "-DALUSUS_INSTALL_BIN_DIR={}".format(install_path["bin"]),
                 "-DALUSUS_INSTALL_LIB_DIR={}".format(install_path["lib"]),
                 "-DALUSUS_INSTALL_INCLUDE_DIR={}".format(
                     install_path["include"]),
                 "-DALUSUS_BUILD_STATIC_LIBS=TRUE",
                 "-DALUSUS_LINK_SHARED_LIBS=TRUE",
                 "-DALUSUS_SET_RPATH=TRUE",
                 "-DALUSUS_PRINT_VERSION_INFO=TRUE",
                 "-DCMAKE_RANLIB={}".format(
                     which(new_environ["RANLIB"] if "RANLIB" in new_environ else "ranlib")),
                 "-DCMAKE_AR={}".format(
                     which(new_environ["AR"] if "AR" in new_environ else "ar")),
                 "-DCMAKE_LINKER={}".format(
                     which(new_environ["LD"] if "LD" in new_environ else "ld")),
                 "-DCMAKE_STRIP={}".format(which(new_environ["STRIP"] if "STRIP" in new_environ else "strip"))]
    if target_system == "macos" or platform.system() == "Darwin" and not target_system:
        cmake_cmd += [
            "-DCMAKE_INSTALL_NAME_TOOL={}".format(which(
                new_environ["INSTALL_NAME_TOOL"] if "INSTALL_NAME_TOOL" in new_environ else "install_name_tool")),
            "-DCMAKE_OTOOL={}".format(which(
                new_environ["OTOOL"] if "OTOOL" in new_environ else "otool"))
        ]
    if target_system == "windows" or platform.system() == "Windows" and not target_system:
        cmake_cmd += [
            "-DCMAKE_RC_COMPILER={}".format(which(
                new_environ["RC"] if "RC" in new_environ else "windres"))
        ]
        if platform.system() == "Windows" and not target_system:
            cmake_cmd += [
                "-G", "MinGW Makefiles",
                "-DCMAKE_SH=CMAKE_SH-NOTFOUND"
            ]
    p = subprocess.Popen(cmake_cmd, env=new_environ)
    ret = p.wait()
    if ret:
        fail_msg("Building Alusus {}.".format(current_version_info[0]))
        os.chdir(original_dir)
        delete_tmp_dir()
        return False
    p = subprocess.Popen(
        [("mingw32-make" if (platform.system() == "Windows") else "make"), "-j{}".format(num_threads), "install"], env=new_environ)
    ret = p.wait()
    if ret:
        fail_msg("Building Alusus {}.".format(current_version_info[0]))
        os.chdir(original_dir)
        delete_tmp_dir()
        return False

    post_build_alusus(
        install_path, target_system=target_system, environ=new_environ)

    success_msg("Building Alusus {}.".format(current_version_info[0]))
    os.chdir(original_dir)
    delete_tmp_dir()
    return True


def build_packages(pkgs_path, install_path):
    return True


def main():
    args = parse_args()
    target_system = None
    if "target" in args and args.target != "linux":
        target_system = args.target

    paths = generate_paths(args.build_path, args.install_path, args.build_type)
    ret = build_alusus(paths["deps_path"], paths["builds_path"],
                       paths["alusus_root_path"], paths["install_path"],
                       args.build_type, num_threads=args.num_threads,
                       target_system=target_system)
    if not ret:
        return ret
    if args.build_packages:
        ret = build_packages(paths["pkgs_paths"], paths["install_path"])
        if not ret:
            return ret


if __name__ == "__main__":
    if not main():
        sys.exit(1)
    else:
        sys.exit(0)
