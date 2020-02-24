from __future__ import print_function
import site
import os
import multiprocessing
import shutil
import tarfile
from zipfile import ZipFile
import subprocess
import platform
from version_info import get_version_info
from msg import errMsg, failMsg, infoMsg, successMsg, warnMsg
try:
    import install_dep
except ImportError:
    import sys
    if os.path.dirname(os.path.realpath(__file__)) not in sys.path:
        sys.path.append(os.path.dirname(os.path.realpath(__file__)))
    import install_dep
try:
    install_dep.install_package('lzma')
    import lzma
except ImportError:
    install_dep.install_package('backports.lzma')
    from backports import lzma
install_dep.install_package('argparse'); import argparse
install_dep.install_package('termcolor'); import termcolor
install_dep.install_package('wget'); import wget
install_dep.install_package('colorama'); import colorama

# Dependencies.
LLVM_SRC_URL = "http://releases.llvm.org/7.0.1/llvm-7.0.1.src.tar.xz"
LLVM_NAME = "llvm-7.0.1"
LIBCURL_SRC_URL="https://github.com/curl/curl/releases/download/curl-7_64_1/curl-7.64.1.tar.xz"
LIBCURL_NAME="curl-7.64.1"
LIBZIP_SRC_URL="https://github.com/kuba--/zip/archive/v0.1.14.zip"
LIBZIP_NAME="zip-0.1.14"

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

def get_lib_filename(name):
  global THIS_SYSTEM
  if THIS_SYSTEM == "Linux":
    return "lib{}.so".format(name)
  elif THIS_SYSTEM == "Darwin":
    return "lib{}.dylib".format(name)
  elif THIS_SYSTEM == "Windows":
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


def build_llvm():
    global BUILD_PATH
    global DEPS_PATH
    global LLVM_NAME
    global LLVM_SRC_URL
    global MAKE_THREAD_COUNT

    try:
        os.makedirs(BUILD_PATH)
    except OSError:
        pass

    try:
        os.makedirs(DEPS_PATH)
    except OSError:
        pass

    old_path = os.path.realpath(os.getcwd())
    os.chdir(DEPS_PATH)

    if not os.path.exists(os.path.join(LLVM_NAME + ".src", "EXTRACTED")):
        if not os.path.exists(LLVM_NAME + ".src.tar.xz"):
            wget.download(LLVM_SRC_URL)
        with lzma.open(LLVM_NAME + ".src.tar.xz") as fd:
            with tarfile.open(fileobj=fd) as tar:
                infoMsg("Extracting LLVM sources...")
                tar.extractall()
        os.remove(LLVM_NAME + ".src.tar.xz")
        with open(os.path.join(LLVM_NAME + ".src", "EXTRACTED"), "w") as fd:
            fd.write("LLVM EXTRACTED CHECKER")
        infoMsg("Finished extracting LLVM sources.")
    else:
        infoMsg("LLVM sources are already available.")

    if os.path.exists(os.path.join(LLVM_NAME + ".install", "INSTALLED")):
        infoMsg("LLVM is already built and installed.")
        successMsg("Building LLVM.")
        return
    try:
        os.makedirs(LLVM_NAME + ".install")
    except OSError:
        pass
    if not os.path.exists(LLVM_NAME + ".build"):
        try:
            os.makedirs(LLVM_NAME + ".build")
        except OSError:
            pass
    os.chdir(LLVM_NAME + ".build")

    try:
        cmake_cmd = ["cmake", os.path.join("..", LLVM_NAME + ".src"),
                     "-DCMAKE_INSTALL_PREFIX={}".format(
                         os.path.join(DEPS_PATH, LLVM_NAME + ".install")),
                     "-DCMAKE_BUILD_TYPE=MinSizeRel"]

        ret = subprocess.call(cmake_cmd)
        if ret != 0:
            failMsg("Building LLVM.")
            exit(1)

        if THIS_SYSTEM == "Windows":
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
        with open(os.path.join(LLVM_NAME + ".install", "INSTALLED"), "w") as fd:
            fd.write("LLVM INSTALLED CHECKER")

    except (OSError, subprocess.CalledProcessError):
        failMsg("Building LLVM.")
        exit(1)
    successMsg("Building LLVM.")
    os.chdir(old_path)


def build_libcurl():
    global BUILD_PATH
    global DEPS_PATH
    global LIBCURL_NAME
    global LIBCURL_SRC_URL
    global MAKE_THREAD_COUNT
    global INSTALL_PATH

    try:
        os.makedirs(BUILD_PATH)
    except OSError:
        pass

    try:
        os.makedirs(DEPS_PATH)
    except OSError:
        pass

    old_path = os.path.realpath(os.getcwd())
    os.chdir(DEPS_PATH)

    if not os.path.exists(os.path.join(LIBCURL_NAME, "EXTRACTED")):
        if not os.path.exists(LIBCURL_NAME + ".tar.xz"):
            wget.download(LIBCURL_SRC_URL)
        with lzma.open(LIBCURL_NAME + ".tar.xz") as fd:
            with tarfile.open(fileobj=fd) as tar:
                infoMsg("Extracting libcurl sources...")
                tar.extractall()
        os.remove(LIBCURL_NAME + ".tar.xz")
        with open(os.path.join(LIBCURL_NAME, "EXTRACTED"), "w") as fd:
            fd.write("LIBCURL EXTRACTED CHECKER")
        infoMsg("Finished extracting libcurl sources.")
    else:
        infoMsg("libcurl sources are already available.")

    if os.path.exists(os.path.join(os.path.realpath(INSTALL_PATH), "Lib", get_lib_filename("curl"))):
        infoMsg("libcurl is already built and installed.")
        successMsg("Building libcurl.")
        return

    os.chdir(LIBCURL_NAME)

    try:
        if THIS_SYSTEM == "Windows":
            # TODO: Build libcurl for windows.
            raise NotImplementedError("Building libcurl for Windows OS is not implemented yet!")
        else:
            ret = subprocess.call("./configure")
            if ret != 0:
                failMsg("Building libcurl.")
                exit(1)
            ret = subprocess.call("make -j{}".format(MAKE_THREAD_COUNT).split())
            if ret != 0:
                failMsg("Building libcurl.")
                exit(1)

            if not os.path.exists(os.path.join(os.path.realpath(INSTALL_PATH), "Lib")):
                try:
                    os.mkdir(os.path.join(os.path.realpath(INSTALL_PATH), "Lib"))
                except:
                    failMsg("Cannot make \"" + os.path.join(os.path.realpath(INSTALL_PATH), "Lib") + "\" directory.")

            shutil.copy2(
                os.path.join(DEPS_PATH, LIBCURL_NAME, "lib", ".libs", get_lib_filename("curl")),
                os.path.join(os.path.realpath(INSTALL_PATH), "Lib", get_lib_filename("curl"))
            )
            if ret != 0:
                failMsg("Building libcurl.")
                exit(1)

    except (OSError, subprocess.CalledProcessError):
        failMsg("Building libcurl.")
        exit(1)
    successMsg("Building libcurl.")
    os.chdir(old_path)


def build_libzip():
    global BUILD_PATH
    global DEPS_PATH
    global LIBZIP_NAME
    global LIBZIP_SRC_URL
    global MAKE_THREAD_COUNT
    global INSTALL_PATH

    try:
        os.makedirs(BUILD_PATH)
    except OSError:
        pass

    try:
        os.makedirs(DEPS_PATH)
    except OSError:
        pass

    old_path = os.path.realpath(os.getcwd())
    os.chdir(DEPS_PATH)

    if not os.path.exists(os.path.join(LIBZIP_NAME, "EXTRACTED")):
        if not os.path.exists(LIBZIP_NAME + ".zip"):
            wget.download(LIBZIP_SRC_URL)
        with ZipFile(file=LIBZIP_NAME + ".zip", mode="r") as zip:
            infoMsg("Extracting libzip sources...")
            zip.extractall()
        os.remove(LIBZIP_NAME + ".zip")
        with open(os.path.join(LIBZIP_NAME, "EXTRACTED"), "w") as fd:
            fd.write("LIBZIP EXTRACTED CHECKER")
        infoMsg("Finished extracting libzip sources.")
    else:
        infoMsg("libzip sources are already available.")

    if os.path.exists(os.path.join(os.path.realpath(INSTALL_PATH), "Lib", get_lib_filename("zip"))):
        infoMsg("libzip is already built and installed.")
        successMsg("Building libzip.")
        return

    os.chdir(LIBZIP_NAME)

    try:
        if THIS_SYSTEM == "Windows":
            # TODO: Build libzip for windows.
            raise NotImplementedError("Building libzip for Windows OS is not implemented yet!")
        else:
            if not os.path.exists(os.path.join(DEPS_PATH, LIBZIP_NAME, "build")):
                try:
                    os.mkdir(os.path.join(DEPS_PATH, LIBZIP_NAME, "build"))
                except:
                    failMsg("Cannot make \"" + os.path.join(DEPS_PATH, LIBZIP_NAME, "build") + "\" directory.")
                    exit(1)

            os.chdir("build")

            ret = subprocess.call(["cmake", "-DBUILD_SHARED_LIBS=true", ".."])
            if ret != 0:
                failMsg("Building libzip.")
                exit(1)
            ret = subprocess.call("make -j{}".format(MAKE_THREAD_COUNT).split())
            if ret != 0:
                failMsg("Building libzip.")
                exit(1)

            if not os.path.exists(os.path.join(os.path.realpath(INSTALL_PATH), "Lib")):
                try:
                    os.mkdir(os.path.join(os.path.realpath(INSTALL_PATH), "Lib"))
                except:
                    failMsg("Cannot make \"" + os.path.join(os.path.realpath(INSTALL_PATH), "Lib") + "\" directory.")
                    exit(1)

            shutil.copy2(
                os.path.join(DEPS_PATH, LIBZIP_NAME, "build", get_lib_filename("zip")),
                os.path.join(os.path.realpath(INSTALL_PATH), "Lib", get_lib_filename("zip"))
            )
            if ret != 0:
                failMsg("Building libzip.")
                exit(1)
    except (OSError, subprocess.CalledProcessError):
        failMsg("Building libzip.")
        exit(1)
    successMsg("Building libzip.")
    os.chdir(old_path)


def prep_debs():
    infoMsg("Preparing dependencies...")
    build_llvm()
    build_libcurl()
    build_libzip()
    successMsg("Building dependencies.")


def copy_other_installation_files():
    global ALUSUS_ROOT
    global INSTALL_PATH
    infoMsg("Copying other installation files...")
    shutil.rmtree(os.path.join(INSTALL_PATH, "Doc"), ignore_errors=True)
    shutil.copytree(
        os.path.join(ALUSUS_ROOT, "Doc"),
        os.path.join(INSTALL_PATH, "Doc")
    )
    shutil.rmtree(os.path.join(INSTALL_PATH, "Notices_L18n"), ignore_errors=True)
    shutil.copytree(
        os.path.join(ALUSUS_ROOT, "Notices_L18n"),
        os.path.join(INSTALL_PATH, "Notices_L18n")
    )
    try:
        os.makedirs(os.path.join(INSTALL_PATH, "Tools", "Gtk_Syntax_Highlighting"))
    except OSError:
        pass
    shutil.copy2(
        os.path.join(ALUSUS_ROOT, "Tools", "Gtk_Syntax_Highlighting", "alusus.lang"),
        os.path.join(INSTALL_PATH, "Tools", "Gtk_Syntax_Highlighting", "alusus.lang")
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


def build_alusus():
    global BUILD_PATH
    global ALUSUS_ROOT
    global BUILD_TYPE
    global INSTALL_PATH
    global DEPS_PATH
    global LLVM_NAME
    global MAKE_THREAD_COUNT
    global CREATE_PACKAGES
    global DLFCN_WIN32_NAME
    global THIS_SYSTEM

    infoMsg("Building Alusus...")
    if not os.path.isdir(BUILD_PATH):
        os.makedirs(BUILD_PATH)
    old_path = os.path.realpath(os.getcwd())
    os.chdir(BUILD_PATH)

    try:
        cmake_cmd = ["cmake",
                     "{}".format(os.path.join(ALUSUS_ROOT, "Sources")),
                     "-DCMAKE_BUILD_TYPE={}".format(BUILD_TYPE),
                     "-DCMAKE_INSTALL_PREFIX={}".format(INSTALL_PATH),
                     "-DLLVM_PATH={}".format(os.path.join(DEPS_PATH, LLVM_NAME + ".install"))]

        ret = subprocess.call(cmake_cmd)
        if ret != 0:
            failMsg("Building Alusus.")
            exit(1)
        if THIS_SYSTEM == "Windows":
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
    global THIS_SYSTEM
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
        "fpm", "-s", input_type, "-t", "deb" if THIS_SYSTEM == "Linux" else "osxpkg", "-a", ARCHITECTURE,
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
            "DEB" if THIS_SYSTEM == "Linux" else "OSXPKG"))
        exit(1)

    # Create additional package of type RPM for Linux systems.
    if THIS_SYSTEM == "Linux":
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
    if THIS_SYSTEM == "Windows":
        create_packages_windows()
    elif THIS_SYSTEM == "Linux" or THIS_SYSTEM == "Darwin":
        create_packages_unix()
    else:
        raise NotImplementedError(
            "Packaging is not supported on \"{}\"!".format(THIS_SYSTEM))
    successMsg("Creating installation packages.")

if __name__ == "__main__":
    colorama.init()
    process_args()
    prep_debs()
    build_alusus()
    if CREATE_PACKAGES == "yes":
        create_packages()
