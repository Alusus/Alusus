from __future__ import print_function
from global_args import global_args
import os
import multiprocessing
import shutil
import tarfile
from zipfile import ZipFile
import subprocess
import platform
import lzma
from msg import errMsg, failMsg, infoMsg, successMsg, warnMsg
import wget
import sys
from version_info import get_version_info

# Current system variables.
THIS_SYSTEM = platform.system()
SHARED_LIBS_EXT = "dll" if THIS_SYSTEM == "Windows" else "so"
LIB_DIR = "Bin" if THIS_SYSTEM == "Windows" else "Lib"

# Dependencies.
LLVM_SRC_URL = "http://releases.llvm.org/7.0.1/llvm-7.0.1.src.tar.xz"
LLVM_NAME = "llvm-7.0.1"
LLVM_SHARED_LIB_NAME = "libLLVM-7"
LIBCURL_SRC_URL="https://github.com/curl/curl/releases/download/curl-7_64_1/curl-7.64.1.tar.xz"
LIBCURL_NAME="curl-7.64.1"
LIBZIP_SRC_URL="https://github.com/kuba--/zip/archive/v0.1.14.zip"
LIBZIP_NAME="zip-0.1.14"
DLFCN_WIN32_URL="https://github.com/dlfcn-win32/dlfcn-win32/archive/v1.1.2.zip"
DLFCN_WIN32_VERSION="1.1.2"
DLFCN_WIN32_NAME="dlfcn-win32"

# Build Args
MAKE_CMD = "mingw32-make" if platform.system() == "Windows" else "make"
MAKE_THREAD_COUNT = multiprocessing.cpu_count()
BUILD_TYPE = global_args['buildType']

# Paths.
ALUSUS_ROOT = os.path.dirname(os.path.dirname(
    os.path.dirname(os.path.realpath(__file__))))
ORIGINAL_PATH = os.path.realpath(os.getcwd())
os.chdir(ALUSUS_ROOT)
PRODUCT_PATH = global_args['productPath']
INSTALL_PATH = global_args['installPath']
RELEASE_INSTALL_PATH = global_args['releaseInstallPath']
BUILDS_PATH = global_args['buildsPath']
BUILD_PATH = global_args['buildPath']
DEPS_PATH = global_args['depsPath']
PYTHON_DEPS_PATH = global_args['pythonDepsPath']
PACKAGES_PATH = global_args['packagesPath']
os.chdir(ORIGINAL_PATH)

# Package Creation Args.
CREATE_PACKAGES = global_args['createPackages']
ARCHITECTURE = "native"
PACKAGE_NAME = "alusus"
PACKAGE_DESCRIPTION = "Alusus Programming Language's core compilation system and standard libraries."
PACKAGE_MAINTAINER = "Sarmad Khalid Abdullah <sarmad@alusus.org>"
PACKAGE_URL = "http://alusus.net"


def build_llvm():
    global BUILD_PATH
    global DEPS_PATH
    global LLVM_NAME
    global LLVM_SRC_URL
    global LLVM_SHARED_LIB_NAME
    global MAKE_THREAD_COUNT
    global MAKE_CMD
    global LIB_DIR
    global SHARED_LIBS_EXT
    global THIS_SYSTEM

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
            print('') # Printing new line after 'wget'.
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

    if os.path.exists(os.path.join(os.path.realpath(INSTALL_PATH), LIB_DIR, "{0}.{1}".format(LLVM_SHARED_LIB_NAME, SHARED_LIBS_EXT))):
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
                     "-DCMAKE_BUILD_TYPE=MinSizeRel",
                     "-DPYTHON_EXECUTABLE={}".format(sys.executable)]
        
        if THIS_SYSTEM == "Windows":
            cmake_cmd += [
                "-G", "MinGW Makefiles",
                "-DCMAKE_SH=CMAKE_SH-NOTFOUND"
            ]
        else:
            # We will create libLLVM later on Windows MinGW after building it statically.
            cmake_cmd += ["-DLLVM_LINK_LLVM_DYLIB=ON"]

        ret = subprocess.call(cmake_cmd)
        if ret != 0:
            failMsg("Building LLVM.")
            exit(1)

        ret = subprocess.call(
            "{0} install -j{1}".format(MAKE_CMD, MAKE_THREAD_COUNT).split())
        if ret != 0:
            failMsg("Building LLVM.")
            exit(1)
        
        # Now we create the "libLLVM" DLL for Windows MinGW.
        if THIS_SYSTEM == "Windows":
            infoMsg("Building {llvmDylibName}.{dylibExt} on Windows MinGW...".format(llvmDylibName=LLVM_SHARED_LIB_NAME, dylibExt=SHARED_LIBS_EXT))
            temp_path = os.getcwd()
            script_dir = os.path.dirname(os.path.realpath(__file__))
            os.chdir(os.path.join(DEPS_PATH, LLVM_NAME + ".install", "lib"))
            shutil.copy2(
                os.path.join(script_dir, 'create_libLLVM_MinGW.sh'),
                'create_libLLVM_MinGW.sh'
            )
            ret = subprocess.call(['bash', '-c', '{script} {arg}'.format(script='./create_libLLVM_MinGW.sh', arg=LLVM_SHARED_LIB_NAME)])
            if ret != 0:
                failMsg("Building LLVM.")
                exit(1)
            os.remove('create_libLLVM_MinGW.sh')
            os.chdir(temp_path)
            infoMsg("Finished building {llvmDylibName}.{dylibExt} on Windows MinGW.".format(llvmDylibName=LLVM_SHARED_LIB_NAME, dylibExt=SHARED_LIBS_EXT))

        if not os.path.exists(os.path.join(os.path.realpath(INSTALL_PATH), LIB_DIR)):
            os.makedirs(os.path.join(os.path.realpath(INSTALL_PATH), LIB_DIR))
        shutil.copy2(
            os.path.join(DEPS_PATH, LLVM_NAME + ".install", "lib", "{0}.{1}".format(LLVM_SHARED_LIB_NAME, SHARED_LIBS_EXT)),
            os.path.join(os.path.realpath(INSTALL_PATH), LIB_DIR)
        )
    except (IOError, OSError, subprocess.CalledProcessError) as e:
        failMsg(str(e))
        failMsg("Building LLVM.")
        exit(1)
    os.chdir(old_path)
    successMsg("Building LLVM.")


def build_libcurl():
    global BUILD_PATH
    global DEPS_PATH
    global LIBCURL_NAME
    global LIBCURL_SRC_URL
    global MAKE_THREAD_COUNT
    global INSTALL_PATH
    global LIB_DIR
    global SHARED_LIBS_EXT
    global THIS_SYSTEM

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
            print('') # Printing new line after 'wget'.
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

    if os.path.exists(os.path.join(os.path.realpath(INSTALL_PATH), LIB_DIR, "libcurl.{}".format(SHARED_LIBS_EXT))):
        infoMsg("libcurl is already built and installed.")
        successMsg("Building libcurl.")
        return

    os.chdir(LIBCURL_NAME)

    try:
        ret = subprocess.call("bash -c ./configure".split())
        if ret != 0:
            failMsg("Building libcurl.")
            exit(1)
        ret = subprocess.call("{0} -j{1}".format(MAKE_CMD, MAKE_THREAD_COUNT).split())
        if ret != 0:
            failMsg("Building libcurl.")
            exit(1)

        # Now we create the "libcurl.dll" for Windows MinGW.
        if THIS_SYSTEM == "Windows":
            subprocess.call(['g++', '-shared', '-o', os.path.join('lib', '.libs', 'libcurl.dll'), os.path.join('lib', '.libs', 'libcurl.a')])

        if not os.path.exists(os.path.join(os.path.realpath(INSTALL_PATH), LIB_DIR)):
            try:
                os.makedirs(os.path.join(os.path.realpath(INSTALL_PATH), LIB_DIR))
            except:
                failMsg("Cannot make \"" + os.path.join(os.path.realpath(INSTALL_PATH), LIB_DIR) + "\" directory.")
        if not os.path.exists(os.path.join(os.path.realpath(INSTALL_PATH), LIB_DIR)):
            os.makedirs(os.path.join(os.path.realpath(INSTALL_PATH), LIB_DIR))
        shutil.copy2(
            os.path.join(DEPS_PATH, LIBCURL_NAME, "lib", ".libs", "libcurl.{}".format(SHARED_LIBS_EXT)),
            os.path.join(os.path.realpath(INSTALL_PATH), LIB_DIR)
        )
    except (IOError, OSError, subprocess.CalledProcessError) as e:
        failMsg(str(e))
        failMsg("Building libcurl.")
        exit(1)
    os.chdir(old_path)
    successMsg("Building libcurl.")


def build_libzip():
    global BUILD_PATH
    global DEPS_PATH
    global LIBZIP_NAME
    global LIBZIP_SRC_URL
    global MAKE_THREAD_COUNT
    global INSTALL_PATH
    global LIB_DIR
    global SHARED_LIBS_EXT
    global THIS_SYSTEM

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

    if not os.path.exists(os.path.join(LIBZIP_NAME + ".src", "EXTRACTED")):
        if not os.path.exists(LIBZIP_NAME + ".zip"):
            wget.download(LIBZIP_SRC_URL)
            print('') # Printing new line after 'wget'.
        with ZipFile(file=LIBZIP_NAME + ".zip", mode="r") as zip:
            infoMsg("Extracting libzip sources...")
            zip.extractall()
            os.rename(LIBZIP_NAME, LIBZIP_NAME + ".src")
        os.remove(LIBZIP_NAME + ".zip")
        with open(os.path.join(LIBZIP_NAME + ".src", "EXTRACTED"), "w") as fd:
            fd.write("LIBZIP EXTRACTED CHECKER")
        infoMsg("Finished extracting libzip sources.")
    else:
        infoMsg("libzip sources are already available.")

    if os.path.exists(os.path.join(os.path.realpath(INSTALL_PATH), LIB_DIR, "libzip.{}".format(SHARED_LIBS_EXT))):
        infoMsg("libzip is already built and installed.")
        successMsg("Building libzip.")
        return

    try:
        if not os.path.exists(os.path.join(DEPS_PATH, LIBZIP_NAME + ".build")):
            os.makedirs(os.path.join(DEPS_PATH, LIBZIP_NAME + ".build"))
        if not os.path.exists(os.path.join(DEPS_PATH, LIBZIP_NAME + ".install")):
            os.makedirs(os.path.join(DEPS_PATH, LIBZIP_NAME + ".install"))
        os.chdir(os.path.join(DEPS_PATH, LIBZIP_NAME + ".build"))

        cmake_cmd = ["cmake",
                     "-DBUILD_SHARED_LIBS=ON",
                     "-DCMAKE_BUILD_TYPE=MinSizeRel",
                     "-DCMAKE_DISABLE_TESTING=\"\"",
                     "-DPYTHON_EXECUTABLE={}".format(sys.executable),
                     "-DCMAKE_INSTALL_PREFIX={}".format(os.path.join(DEPS_PATH, LIBZIP_NAME + ".install")),
                     os.path.join(DEPS_PATH, LIBZIP_NAME + ".src")]
        if THIS_SYSTEM == "Windows":
            cmake_cmd += [
                "-G", "MinGW Makefiles",
                "-DCMAKE_SH=CMAKE_SH-NOTFOUND"
            ]
        ret = subprocess.call(cmake_cmd)
        if ret != 0:
            failMsg("Building libzip.")
            exit(1)
        ret = subprocess.call("{0} install -j{1}".format(MAKE_CMD, MAKE_THREAD_COUNT).split())
        if ret != 0:
            failMsg("Building libzip.")
            exit(1)
        if not os.path.exists(os.path.join(os.path.realpath(INSTALL_PATH), LIB_DIR)):
            os.makedirs(os.path.join(os.path.realpath(INSTALL_PATH), LIB_DIR))
        shutil.copy2(
            os.path.join(DEPS_PATH, LIBZIP_NAME + ".install", "bin", "libzip.{}".format(SHARED_LIBS_EXT)),
            os.path.join(os.path.realpath(INSTALL_PATH), LIB_DIR)
        )
    except (IOError, OSError, subprocess.CalledProcessError) as e:
        failMsg(str(e))
        failMsg("Building libzip.")
        exit(1)
    os.chdir(old_path)
    successMsg("Building libzip.")

def build_dlfcn_win32():
    global BUILD_PATH
    global DEPS_PATH
    global DLFCN_WIN32_NAME
    global DLFCN_WIN32_URL
    global DLFCN_WIN32_VERSION
    global MAKE_THREAD_COUNT
    global INSTALL_PATH
    global LIB_DIR
    global SHARED_LIBS_EXT
    global THIS_SYSTEM

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

    dlfcn_folder_name = DLFCN_WIN32_NAME + '-' + DLFCN_WIN32_VERSION
    if not os.path.exists(os.path.join(dlfcn_folder_name + ".src", "EXTRACTED")):
        if not os.path.exists((dlfcn_folder_name + ".zip")):
            wget.download(DLFCN_WIN32_URL)
            print('') # Printing new line after 'wget'.
        with ZipFile(file=(dlfcn_folder_name + ".zip"), mode="r") as zip:
            infoMsg("Extracting dlfcn-win32 sources...")
            zip.extractall()
        os.rename(dlfcn_folder_name, dlfcn_folder_name + ".src")
        os.remove((dlfcn_folder_name + ".zip"))
        with open(os.path.join(dlfcn_folder_name + ".src", "EXTRACTED"), "w") as fd:
            fd.write("DLFCN-WIN32 EXTRACTED CHECKER")
        infoMsg("Finished extracting dlfcn-win32 sources.")
    else:
        infoMsg("dlfcn-win32 sources are already available.")

    if os.path.exists(os.path.join(os.path.realpath(INSTALL_PATH), LIB_DIR, "libdl.{}".format(SHARED_LIBS_EXT))):
        infoMsg("libdl is already built and installed.")
        successMsg("Building libdl.")
        return

    try:
        if not os.path.exists(dlfcn_folder_name + ".build"):
            os.makedirs(dlfcn_folder_name + ".build")
        if not os.path.exists(dlfcn_folder_name + ".install"):
            os.makedirs(dlfcn_folder_name + ".install")
        os.chdir(dlfcn_folder_name + ".build")

        cmake_cmd = ["cmake",
                     "-DBUILD_SHARED_LIBS=ON",
                     "-DCMAKE_BUILD_TYPE=MinSizeRel",
                     "-DPYTHON_EXECUTABLE={}".format(sys.executable),
                     "-DCMAKE_INSTALL_PREFIX={}".format(os.path.join(DEPS_PATH, dlfcn_folder_name + ".install")),
                     os.path.join(DEPS_PATH, dlfcn_folder_name + ".src"),]
        if THIS_SYSTEM == "Windows":
            cmake_cmd += [
                "-G", "MinGW Makefiles",
                "-DCMAKE_SH=CMAKE_SH-NOTFOUND"
            ]
        ret = subprocess.call(cmake_cmd)
        if ret != 0:
            failMsg("Building libdl.")
            exit(1)
        ret = subprocess.call("{0} install -j{1}".format(MAKE_CMD, MAKE_THREAD_COUNT).split())
        if ret != 0:
            failMsg("Building libdl.")
            exit(1)

        if not os.path.exists(os.path.join(os.path.realpath(INSTALL_PATH), LIB_DIR)):
            try:
                os.makedirs(os.path.join(os.path.realpath(INSTALL_PATH), LIB_DIR))
            except:
                failMsg("Cannot make \"" + os.path.join(os.path.realpath(INSTALL_PATH), LIB_DIR) + "\" directory.")
                exit(1)
        if not os.path.exists(os.path.join(os.path.realpath(INSTALL_PATH), LIB_DIR)):
            os.makedirs(os.path.join(os.path.realpath(INSTALL_PATH), LIB_DIR))
        shutil.copy2(
            os.path.join(DEPS_PATH, dlfcn_folder_name + ".install", "bin", "libdl.{}".format(SHARED_LIBS_EXT)),
            os.path.join(os.path.realpath(INSTALL_PATH), LIB_DIR)
        )
    except (IOError, OSError, subprocess.CalledProcessError) as e:
        failMsg(str(e))
        failMsg("Building libdl.")
        exit(1)
    os.chdir(old_path)
    successMsg("Building libdl.")


def prep_debs():
    global THIS_SYSTEM

    infoMsg("Preparing dependencies...")
    build_llvm()
    build_libcurl()
    build_libzip()
    if THIS_SYSTEM == "Windows":
        build_dlfcn_win32()
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
        os.path.join(ALUSUS_ROOT, "changelog.en.txt"),
        os.path.join(INSTALL_PATH, "changelog.en.txt")
    )
    shutil.copy2(
        os.path.join(ALUSUS_ROOT, "changelog.ar.txt"),
        os.path.join(INSTALL_PATH, "changelog.ar.txt")
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
    global PYTHON_DEPS_PATH
    global LLVM_NAME
    global MAKE_THREAD_COUNT
    global CREATE_PACKAGES
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
                     "-DLLVM_PATH={}".format(os.path.join(DEPS_PATH, LLVM_NAME + ".install")),
                     "-DPYTHON_EXECUTABLE={}".format(sys.executable),
                     "-DPYTHON_PREFIX={}".format(PYTHON_DEPS_PATH)]
        
        if THIS_SYSTEM == "Windows":
            cmake_cmd += [
                "-G", "MinGW Makefiles",
                "-DCMAKE_SH=CMAKE_SH-NOTFOUND"
            ]

        ret = subprocess.call(cmake_cmd)
        if ret != 0:
            failMsg("Building Alusus.")
            exit(1)
        ret = subprocess.call(
            "{0} install -j{1}".format(MAKE_CMD, MAKE_THREAD_COUNT).split())
        if ret != 0:
            failMsg("Building Alusus.")
            exit(1)

    except (IOError, OSError, subprocess.CalledProcessError) as e:
        failMsg(str(e))
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
    global PYTHON_DEPS_PATH

    old_path = os.path.realpath(os.getcwd())
    os.chdir(PACKAGES_PATH)

    input_type = "dir"
    version, revision, _, _ = get_version_info(PYTHON_DEPS_PATH)
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
    prep_debs()
    build_alusus()
    if CREATE_PACKAGES == "yes":
       create_packages()
