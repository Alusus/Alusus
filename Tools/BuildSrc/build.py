#!/usr/bin/env python3
from __future__ import print_function
import colorama
import wget
import termcolor
import argparse
import site
import os
import multiprocessing
import shutil
import tarfile
from zipfile import ZipFile
import subprocess
import platform
from urllib.request import urlretrieve
from version_info import get_version_info
from msg import errMsg, failMsg, infoMsg, successMsg, warnMsg
import lzma

# Build Args
MAKE_THREAD_COUNT = multiprocessing.cpu_count()
BUILD_TYPE = "debug"

# Paths.
ALUSUS_ROOT = os.path.dirname(os.path.dirname(
    os.path.dirname(os.path.realpath(__file__))))
ORIGINAL_PATH = os.path.realpath(os.getcwd())
os.chdir(ALUSUS_ROOT)
PRODUCT_PATH = ORIGINAL_PATH
INSTALL_PATH = PRODUCT_PATH
RELEASE_INSTALL_PATH = os.path.join(
    '/', 'opt', 'Alusus') if os.name == "posix" else os.path.join('/', 'Alusus')
BUILDS_PATH = os.path.join(PRODUCT_PATH, "Builds")
BUILD_PATH = os.path.join(BUILDS_PATH, BUILD_TYPE[0].upper() + BUILD_TYPE[1:])
DEPS_PATH = os.path.join(BUILDS_PATH, 'Dependencies')
PACKAGES_PATH = os.path.join(BUILDS_PATH, 'Packages')
os.chdir(ORIGINAL_PATH)

# Package Creation Args.
CREATE_PACKAGES = "no"
ARCHITECTURE = "native"
PACKAGE_NAME = "alusus"
PACKAGE_DESCRIPTION = "Alusus Programming Language's core compilation system and standard libraries."
PACKAGE_MAINTAINER = "Sarmad Khalid Abdullah <sarmad@alusus.org>"
PACKAGE_URL = "https://alusus.org"

# Current system.
THIS_SYSTEM = platform.system()


def is_windows():
    return THIS_SYSTEM == "Windows"


def is_macos():
    return THIS_SYSTEM == "Darwin"


def is_linux():
    return THIS_SYSTEM == "Linux"


def get_lib_filename(name):
    if is_linux():
        return "lib{}.so".format(name)
    elif is_macos():
        return "lib{}.dylib".format(name)
    elif is_windows():
        return "{}.dll".format(name)
    else:
        raise NotImplementedError("Unsupported system.")


def process_args():
    global BUILD_TYPE
    global BUILDS_PATH
    global BUILD_PATH
    global CREATE_PACKAGES
    global INSTALL_PATH
    global RELEASE_INSTALL_PATH
    global DEPS_PATH
    global PACKAGES_PATH
    global PRODUCT_PATH

    parser = argparse.ArgumentParser(add_help=False,
                                     formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument('-h', '--help', action='help', default=argparse.SUPPRESS,
                        help='Show this help message and exit.')
    parser.add_argument("--btype", metavar="BUILD_TYPE", help="Set the build type of Alusus:\n" +
                        "    d [default]: debug build.\n" +
                        "    r: release build.\n" +
                        "    p: release and package build.",
                        choices=["d", "r", "p"], required=False, default="d")
    parser.add_argument("--bloc", metavar="BUILD_LOCATION", help="Set the build location (default is the sources root directory).",
                        required=True, default=None)
    parser.add_argument("--iloc", metavar="INSTALL_LOCATION", help="Set the install location (default is the value of \"--bloc\").",
                        required=False, default=None)
    parser.add_argument("-g", action="store_true", help="Set the install location globally on the current system"
                                                        " (located in \"{}\" - MAY REQUIRE ROOT PRIVILEGE)."
                                                        " This will be overridden by the value of \"--iloc\" option.".format(
                                                            RELEASE_INSTALL_PATH),
                        required=False, default=False)
    args = parser.parse_args()

    if args.btype == "r":
        BUILD_TYPE = "release"
    elif args.btype == "p":
        BUILD_TYPE = "release"
        CREATE_PACKAGES = "yes"

    if args.bloc:
        PRODUCT_PATH = os.path.realpath(args.bloc)
        if not os.path.isdir(PRODUCT_PATH):
            os.makedirs(PRODUCT_PATH)
        BUILDS_PATH = os.path.join(PRODUCT_PATH, "Builds")
        BUILD_PATH = os.path.join(
            BUILDS_PATH, BUILD_TYPE[0].upper() + BUILD_TYPE[1:])
        DEPS_PATH = os.path.join(BUILDS_PATH, "Dependencies")
        PACKAGES_PATH = os.path.join(BUILDS_PATH, "Packages")
        INSTALL_PATH = os.path.join(PRODUCT_PATH, "LocalInstall")

    if args.g:
        INSTALL_PATH = RELEASE_INSTALL_PATH
        shutil.rmtree(INSTALL_PATH, ignore_errors=True)
        os.makedirs(INSTALL_PATH)

    if args.iloc:
        INSTALL_PATH = os.path.realpath(args.iloc)


def create_dirs():
    """
    Create the directories required during the build process. These are:
    - the build directory: where Alusus is built.
    - the dependencies directory: where Alusus's dependencies are built.
    """
    global BUILD_PATH
    global DEPS_PATH
    os.makedirs(BUILD_PATH, exist_ok=True)
    os.makedirs(DEPS_PATH, exist_ok=True)
    os.makedirs(os.path.join(os.path.realpath(
        INSTALL_PATH), "Lib"), exist_ok=True)


def fetch_dep(dep_name, url, dir_name, filename):
    if not os.path.exists(os.path.join(dir_name, "EXTRACTED")):
        if not os.path.exists(filename):
            infoMsg(f"Downloading {dep_name} from {url}...")
            wget.download(url)
        else:
            infoMsg(f"{dep_name} already available at {filename}.")
        if filename.endswith(".zip"):
            with ZipFile(filename,"r") as zip_ref:
                zip_ref.extractall(".")
        else:
            with lzma.open(filename) as fd:
                with tarfile.open(fileobj=fd) as tar:
                    infoMsg(f"Extracting {dep_name} sources from {filename}...")
                    tar.extractall()
        os.remove(filename)
        with open(os.path.join(dir_name, "EXTRACTED"), "w") as fd:
            fd.write(f"{dep_name} EXTRACTED CHECKER")
        infoMsg(f"Finished extracting {dep_name} sources.")
    else:
        infoMsg(f"{dep_name} sources are already available.")


def build_llvm():
    global DEPS_PATH
    global MAKE_THREAD_COUNT
    url = "https://github.com/llvm/llvm-project/releases/download/llvmorg-10.0.0/llvm-10.0.0.src.tar.xz"
    filename = "llvm-10.0.0.src.tar.xz"
    src_dir, build_dir, install_dir = "llvm-10.0.0.src", "llvm-10.0.0.build", "llvm-10.0.0.install"

    old_path = os.path.realpath(os.getcwd())
    os.chdir(DEPS_PATH)

    fetch_dep("LLVM", url, src_dir, filename)

    if os.path.exists(os.path.join(install_dir, "INSTALLED")):
        infoMsg("LLVM is already built and installed.")
        successMsg("Building LLVM.")
        return os.path.realpath(install_dir)
    try:
        os.makedirs(install_dir)
    except OSError:
        pass
    if not os.path.exists(build_dir):
        try:
            os.makedirs(build_dir)
        except OSError:
            pass
    os.chdir(build_dir)

    try:
        ret = subprocess.call([
            "cmake",
            os.path.join("..", src_dir),
            f"-DCMAKE_INSTALL_PREFIX={os.path.join(DEPS_PATH, install_dir)}",
            "-DCMAKE_BUILD_TYPE=MinSizeRel"])
        if ret != 0:
            failMsg("Building LLVM.")
            exit(1)

        if is_windows():
            ret = subprocess.call(
                "msbuild INSTALL.vcxproj /p:Configuration=MinSizeRel /m".split())
            if ret != 0:
                failMsg("Building LLVM.")
                exit(1)
        else:
            ret = subprocess.call(
                "make install -j{}".format(MAKE_THREAD_COUNT).split())
            if ret != 0:
                failMsg("Building LLVM.")
                exit(1)

        os.chdir(DEPS_PATH)
        with open(os.path.join(install_dir, "INSTALLED"), "w") as fd:
            fd.write("LLVM INSTALLED CHECKER")

    except (OSError, subprocess.CalledProcessError):
        failMsg("Building LLVM.")
        exit(1)
    successMsg("Building LLVM.")
    os.chdir(old_path)
    return os.path.realpath(install_dir)


def build_libcurl():
    global DEPS_PATH
    global MAKE_THREAD_COUNT
    global INSTALL_PATH
    url = "https://github.com/curl/curl/releases/download/curl-7_70_0/curl-7.70.0.tar.xz"
    src_dir = "curl-7.70.0"
    filename = "curl-7.70.0.tar.xz"
    output = get_lib_filename("curl")

    old_path = os.path.realpath(os.getcwd())
    os.chdir(DEPS_PATH)

    fetch_dep("libcurl", url, src_dir, filename)

    if os.path.exists(os.path.join(os.path.realpath(INSTALL_PATH), "Lib", output)):
        infoMsg("libcurl is already built and installed.")
        successMsg("Building libcurl.")
        return

    os.chdir(src_dir)

    try:
        if is_windows():
            # TODO: Build libcurl for windows.
            raise NotImplementedError(
                "Building libcurl for Windows OS is not implemented yet!")
        else:
            ret = subprocess.call("./configure")
            if ret != 0:
                failMsg("Building libcurl (./configure).")
                exit(1)
            ret = subprocess.call(
                "make -j{}".format(MAKE_THREAD_COUNT).split())
            if ret != 0:
                failMsg("Building libcurl (make).")
                exit(1)

            shutil.copy2(
                os.path.join(DEPS_PATH, src_dir, "lib", ".libs", output),
                os.path.join(os.path.realpath(INSTALL_PATH), "Lib", output)
            )
            if ret != 0:
                failMsg(f"Building libcurl. Couldn't copy {output}.")
                exit(1)

    except (OSError, subprocess.CalledProcessError) as e:
        failMsg(f"Building libcurl. Exception: {e}")
        exit(1)
    successMsg("Building libcurl.")
    os.chdir(old_path)


def build_libzip():
    global DEPS_PATH
    global MAKE_THREAD_COUNT
    global INSTALL_PATH

    url = "https://github.com/kuba--/zip/archive/v0.1.19.zip"
    src_dir = "zip-0.1.19"
    filename = "zip-0.1.19.zip"
    output = get_lib_filename("zip")

    old_path = os.path.realpath(os.getcwd())
    os.chdir(DEPS_PATH)

    fetch_dep("libzip", url, src_dir, filename)

    if os.path.exists(os.path.join(os.path.realpath(INSTALL_PATH), "Lib", output)):
        infoMsg("libzip is already built and installed.")
        successMsg("Building libzip.")
        return

    os.chdir(src_dir)

    try:
        if is_windows():
            # TODO: Build libzip for windows.
            raise NotImplementedError(
                "Building libzip for Windows OS is not implemented yet!")
        else:
            build_dir = os.path.join(DEPS_PATH, src_dir, "build")
            if not os.path.exists(build_dir):
                try:
                    os.mkdir(build_dir)
                except:
                    failMsg(f'Cannot make "{build_dir}" directory.')
                    exit(1)

            os.chdir("build")

            ret = subprocess.call(["cmake", "-DBUILD_SHARED_LIBS=true", ".."])
            if ret != 0:
                failMsg("Building libzip.")
                exit(1)
            ret = subprocess.call(
                "make -j{}".format(MAKE_THREAD_COUNT).split())
            if ret != 0:
                failMsg("Building libzip.")
                exit(1)

            shutil.copy2(
                os.path.join(DEPS_PATH, build_dir, output),
                os.path.join(os.path.realpath(INSTALL_PATH), "Lib", output)
            )
            if ret != 0:
                failMsg("Building libzip.")
                exit(1)
    except (OSError, subprocess.CalledProcessError):
        failMsg("Building libzip.")
        exit(1)
    successMsg("Building libzip.")
    os.chdir(old_path)


def copy_other_installation_files():
    global ALUSUS_ROOT
    global INSTALL_PATH
    infoMsg("Copying other installation files...")
    shutil.rmtree(os.path.join(INSTALL_PATH, "Doc"), ignore_errors=True)
    shutil.copytree(
        os.path.join(ALUSUS_ROOT, "Doc"),
        os.path.join(INSTALL_PATH, "Doc")
    )
    shutil.rmtree(os.path.join(INSTALL_PATH, "Notices_L18n"),
                  ignore_errors=True)
    shutil.copytree(
        os.path.join(ALUSUS_ROOT, "Notices_L18n"),
        os.path.join(INSTALL_PATH, "Notices_L18n")
    )
    try:
        os.makedirs(os.path.join(INSTALL_PATH, "Tools",
                                 "Gtk_Syntax_Highlighting"))
    except OSError:
        pass
    shutil.copy2(
        os.path.join(ALUSUS_ROOT, "Tools",
                     "Gtk_Syntax_Highlighting", "alusus.lang"),
        os.path.join(INSTALL_PATH, "Tools",
                     "Gtk_Syntax_Highlighting", "alusus.lang")
    )
    shutil.copy2(
        os.path.join(ALUSUS_ROOT, "changelog.en.md"),
        os.path.join(INSTALL_PATH, "changelog.en.md")
    )
    shutil.copy2(
        os.path.join(ALUSUS_ROOT, "changelog.ar.md"),
        os.path.join(INSTALL_PATH, "changelog.ar.md")
    )
    shutil.copy2(
        os.path.join(ALUSUS_ROOT, "license.pdf"),
        os.path.join(INSTALL_PATH, "license.pdf")
    )
    shutil.copy2(
        os.path.join(ALUSUS_ROOT, "license.txt"),
        os.path.join(INSTALL_PATH, "license.txt")
    )
    successMsg("Copying other installation files.")


def build_alusus(llvm_path):
    global BUILD_PATH
    global ALUSUS_ROOT
    global BUILD_TYPE
    global INSTALL_PATH
    global DEPS_PATH
    global MAKE_THREAD_COUNT
    global CREATE_PACKAGES
    global DLFCN_WIN32_NAME

    infoMsg("Building Alusus...")
    old_path = os.path.realpath(os.getcwd())
    os.chdir(BUILD_PATH)

    try:
        cmake_cmd = ["cmake",
                     os.path.join(ALUSUS_ROOT, "Sources"),
                     f"-DCMAKE_BUILD_TYPE={BUILD_TYPE}".format(),
                     f"-DCMAKE_INSTALL_PREFIX={INSTALL_PATH}".format(),
                     f"-DLLVM_PATH={llvm_path}".format()]

        ret = subprocess.call(cmake_cmd)
        if ret != 0:
            failMsg("Building Alusus.")
            exit(1)
        if is_windows():
            ret = subprocess.call("msbuild INSTALL.vcxproj /m".split())
            if ret != 0:
                failMsg("Building Alusus.")
                exit(1)
        else:
            ret = subprocess.call(
                "make install -j{}".format(MAKE_THREAD_COUNT).split())
            if ret != 0:
                failMsg("Building Alusus.")
                exit(1)

    except (OSError, subprocess.CalledProcessError):
        failMsg("Building Alusus.")
        exit(1)
    os.chdir(old_path)
    successMsg("Building Alusus.")


def create_packages_windows():
    # TODO: Implement Windows packaging.
    raise NotImplementedError("Windows OS packaging is not implemented yet!")


def create_packages_unix():
    global ALUSUS_ROOT
    global PACKAGES_PATH
    global ARCHITECTURE
    global PACKAGE_NAME
    global PACKAGE_DESCRIPTION
    global PACKAGE_URL
    global PACKAGE_MAINTAINER
    global RELEASE_INSTALL_PATH
    global INSTALL_PATH

    old_path = os.path.realpath(os.getcwd())
    os.chdir(PACKAGES_PATH)

    input_type = "dir"
    version, revision, _, _ = get_version_info()
    after_install_script = os.path.join(
        ALUSUS_ROOT, "Tools", "BuildSrc", "Package_Scripts", "post_install.sh")
    after_remove_script = os.path.join(
        ALUSUS_ROOT, "Tools", "BuildSrc", "Package_Scripts", "post_remove.sh")

    current_cmd = [
        "fpm", "-s", input_type, "-t", "deb" if is_linux() else "osxpkg", "-a", ARCHITECTURE,
        "-n", PACKAGE_NAME, "-v", version, "--description", PACKAGE_DESCRIPTION,
        "--url", PACKAGE_URL, "-m", PACKAGE_MAINTAINER,
        "--after-install", after_install_script,
        "--after-remove", after_remove_script, "-f",
        "--prefix", RELEASE_INSTALL_PATH,
        "-C",
        INSTALL_PATH
    ]

    if revision != "":
        # Insert just before "--description".
        idx = current_cmd.index("--description")
        current_cmd.insert(idx, revision)
        current_cmd.insert(idx, "--iteration")

    ret = subprocess.call(current_cmd)
    if ret != 0:
        failMsg("Creating {} Package.".format(
            "DEB" if is_linux() else "OSXPKG"))
        exit(1)

    # Create additional package of type RPM for Linux systems.
    if is_linux():
        # Replace "deb" with "rpm".
        current_cmd[current_cmd.index("-t") + 1] = "rpm"

        ret = subprocess.call(current_cmd)
        if ret != 0:
            failMsg("Creating RPM Package.")
            exit(1)

    os.chdir(old_path)


def create_packages():
    global PACKAGES_PATH
    global THIS_SYSTEM

    infoMsg("Creating installation packages...")
    shutil.rmtree(PACKAGES_PATH, ignore_errors=True)
    os.makedirs(PACKAGES_PATH)
    copy_other_installation_files()
    if is_windows():
        create_packages_windows()
    elif is_linux() or is_macos():
        create_packages_unix()
    else:
        raise NotImplementedError(
            "Packaging is not supported on \"{}\"!".format(THIS_SYSTEM))
    successMsg("Creating installation packages.")


if __name__ == "__main__":
    colorama.init()
    process_args()

    infoMsg("Building dependencies...")
    create_dirs()
    llvm_path = build_llvm()
    build_libcurl()
    build_libzip()
    successMsg("Building dependencies.")
    build_alusus(llvm_path)
    if CREATE_PACKAGES == "yes":
        create_packages()
