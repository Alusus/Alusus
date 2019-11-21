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
import colorama
import sys
from version_info import get_version_info
from create_dll import create_dll

# Current system variables.
THIS_SYSTEM = platform.system()
SHARED_LIBS_EXT = "dll" if THIS_SYSTEM == "Windows" else "so"
if THIS_SYSTEM == "Darwin":
    SHARED_LIBS_EXT = "dylib"
LIB_DIR = "Bin" if THIS_SYSTEM == "Windows" else "Lib"

# Dependencies.
LLVM_SRC_URL = "http://releases.llvm.org/7.0.1/llvm-7.0.1.src.tar.xz"
LLVM_NAME = "llvm-7.0.1"
LLVM_SHARED_LIB_BUILD_NAME = "libLLVM-7" if THIS_SYSTEM != "Darwin" else "libLLVM"
LLVM_SHARED_LIB_INSTALL_NAME = "libLLVM"
LIBCURL_SRC_URL = "https://github.com/curl/curl/releases/download/curl-7_64_1/curl-7.64.1.tar.xz"
LIBCURL_NAME = "curl-7.64.1"
LIBZIP_SRC_URL = "https://github.com/kuba--/zip/archive/v0.1.14.zip"
LIBZIP_NAME = "zip-0.1.14"
DLFCN_WIN32_URL = "https://github.com/dlfcn-win32/dlfcn-win32/archive/v1.1.2.zip"
DLFCN_WIN32_VERSION = "1.1.2"
DLFCN_WIN32_NAME = "dlfcn-win32"

# Build args
MAKE_CMD = "mingw32-make" if platform.system() == "Windows" else "make"
MAKE_THREAD_COUNT = global_args['numThreads']
BUILD_TYPE = global_args['buildType']

# Paths.
ALUSUS_ROOT = os.path.dirname(os.path.dirname(
    os.path.dirname(os.path.abspath(__file__))))
ORIGINAL_PATH = os.path.abspath(os.getcwd())
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

# Package creation args.
CREATE_PACKAGES = global_args['createPackages']
ARCHITECTURE = "native"
PACKAGE_NAME = "alusus"
PACKAGE_DESCRIPTION = "Alusus Programming Language's core compilation system and standard libraries."
PACKAGE_MAINTAINER = "Sarmad Khalid Abdullah <sarmad@alusus.org>"
PACKAGE_URL = "http://alusus.net"

# Version and date args.
ALUSUS_VERSION = global_args['version']
ALUSUS_REVERSION = global_args['revision']
ALUSUS_DATE = global_args['date']
ALUSUS_HIJRI_DATE = global_args['hijri_date']


def build_llvm():
    global BUILD_PATH
    global DEPS_PATH
    global LLVM_NAME
    global LLVM_SRC_URL
    global LLVM_SHARED_LIB_BUILD_NAME
    global LLVM_SHARED_LIB_INSTALL_NAME
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

    old_path = os.path.abspath(os.getcwd())
    os.chdir(DEPS_PATH)

    if not os.path.exists(os.path.join(LLVM_NAME + ".src", "EXTRACTED")):
        if not os.path.exists(LLVM_NAME + ".src.tar.xz"):
            subprocess.call(['wget', LLVM_SRC_URL])
            print('')  # Printing new line after 'wget'.
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

    if os.path.exists(os.path.join(os.path.abspath(INSTALL_PATH), LIB_DIR, "{0}.{1}".format(LLVM_SHARED_LIB_INSTALL_NAME, SHARED_LIBS_EXT))):
        infoMsg("{} is already built and installed.".format(
            LLVM_SHARED_LIB_INSTALL_NAME))
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
            os._exit(1)

        ret = subprocess.call(
            "{0} install -j{1}".format(MAKE_CMD, MAKE_THREAD_COUNT).split())
        if ret != 0:
            failMsg("Building LLVM.")
            os._exit(1)

        if not os.path.exists(os.path.join(os.path.abspath(INSTALL_PATH), LIB_DIR)):
            os.makedirs(os.path.join(os.path.abspath(INSTALL_PATH), LIB_DIR))

        if THIS_SYSTEM == "Windows":
            # Force build shared library in Windows.
            infoMsg("Building {llvmDylibName}.{dylibExt} on Windows MinGW...".format(
                    llvmDylibName=LLVM_SHARED_LIB_INSTALL_NAME, dylibExt=SHARED_LIBS_EXT))
            if THIS_SYSTEM == "Windows":
                try:
                    os.remove(os.path.join(DEPS_PATH, LLVM_NAME + ".install", "lib", "libLLVM.dll.a"))
                except OSError:
                    pass
                ret = create_dll(arg_dir=[os.path.join(DEPS_PATH, LLVM_NAME + ".install", "lib")],
                        arg_output_dir=os.path.join(os.path.abspath(INSTALL_PATH), LIB_DIR),
                        arg_output_name='LLVM', arg_link_lib=[
                            'ole32', 'uuid', 'LLVMSupport', 'z'])
                if ret[0]:
                    failMsg("Building LLVM.")
                    os._exit(1)
        else:
            shutil.copy2(
                os.path.join(DEPS_PATH, LLVM_NAME + ".install", "lib",
                            "{0}.{1}".format(LLVM_SHARED_LIB_BUILD_NAME, SHARED_LIBS_EXT)),
                os.path.join(os.path.abspath(INSTALL_PATH), LIB_DIR, "{0}.{1}".format(
                    LLVM_SHARED_LIB_INSTALL_NAME, SHARED_LIBS_EXT))
            )
    except (IOError, OSError, subprocess.CalledProcessError, TypeError) as e:
        failMsg(str(e))
        failMsg("Building LLVM.")
        os._exit(1)
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
    global THIS_SYSTEM

    try:
        os.makedirs(BUILD_PATH)
    except OSError:
        pass

    try:
        os.makedirs(DEPS_PATH)
    except OSError:
        pass

    old_path = os.path.abspath(os.getcwd())
    os.chdir(DEPS_PATH)

    if not os.path.exists(os.path.join(LIBCURL_NAME + ".src", "EXTRACTED")):
        if not os.path.exists(LIBCURL_NAME + ".tar.xz"):
            subprocess.call(['wget', LIBCURL_SRC_URL])
            print('')  # Printing new line after 'wget'.
        with lzma.open(LIBCURL_NAME + ".tar.xz") as fd:
            with tarfile.open(fileobj=fd) as tar:
                infoMsg("Extracting libcurl sources...")
                tar.extractall()
        os.remove(LIBCURL_NAME + ".tar.xz")
        os.rename(LIBCURL_NAME, LIBCURL_NAME + ".src")
        with open(os.path.join(LIBCURL_NAME + ".src", "EXTRACTED"), "w") as fd:
            fd.write("LIBCURL EXTRACTED CHECKER")
        infoMsg("Finished extracting libcurl sources.")
    else:
        infoMsg("libcurl sources are already available.")

    if os.path.exists(os.path.join(os.path.abspath(INSTALL_PATH), LIB_DIR, "libcurl.{}".format(SHARED_LIBS_EXT))):
        infoMsg("libcurl is already built and installed.")
        successMsg("Building libcurl.")
        return

    if not os.path.exists(LIBCURL_NAME + ".build"):
        os.makedirs(LIBCURL_NAME + ".build")
    if not os.path.exists(LIBCURL_NAME + ".install"):
        os.makedirs(LIBCURL_NAME + ".install")
    os.chdir(LIBCURL_NAME + ".build")

    try:
        configure_path = os.path.abspath(os.path.join(
            '..', LIBCURL_NAME + ".src", "configure"))
        install_prefix_path = os.path.abspath(
            os.path.join('..', LIBCURL_NAME + ".install"))
        # Convert the paths to UNIX based on Windows to run under MSYS2 shell.
        if THIS_SYSTEM == "Windows":
            configure_path = subprocess.check_output(
                ['cygpath', configure_path]).decode('utf8')
            configure_path = configure_path[:len(configure_path) - 1]
            install_prefix_path = subprocess.check_output(
                ['cygpath', install_prefix_path]).decode('utf8')
            install_prefix_path = install_prefix_path[:len(
                install_prefix_path) - 1]
        bash_cmd = '{0} --prefix={1} --enable-shared=yes'.format(
            configure_path, install_prefix_path)
        if THIS_SYSTEM == "Windows":
            bash_cmd += ' --disable-dependency-tracking --host={}'.format(
                os.environ['MINGW_HOST'])
        ret = subprocess.call(['bash', '-c', bash_cmd])
        if ret != 0:
            failMsg("Building libcurl.")
            os._exit(1)

        # On Windows, we will be using MSYS2's "make" (not "mingw32-make") command, as we are going to build under MSYS2's UNIX shell.
        ret = subprocess.call(
            ['make', '-j{0}'.format(MAKE_THREAD_COUNT), 'install'])
        if ret != 0:
            failMsg("Building libcurl.")
            os._exit(1)


        if not os.path.exists(os.path.join(os.path.abspath(INSTALL_PATH), LIB_DIR)):
            try:
                os.makedirs(os.path.join(
                    os.path.abspath(INSTALL_PATH), LIB_DIR))
            except:
                failMsg(
                    "Cannot create \"" + os.path.join(os.path.abspath(INSTALL_PATH), LIB_DIR) + "\" directory.")
        if not os.path.exists(os.path.join(os.path.abspath(INSTALL_PATH), LIB_DIR)):
            os.makedirs(os.path.join(os.path.abspath(INSTALL_PATH), LIB_DIR))

        if THIS_SYSTEM == "Windows":
            # Force build shared library in Windows.
            infoMsg("Building libcurl.{dylibExt} on Windows MinGW...".format(dylibExt=SHARED_LIBS_EXT))
            if THIS_SYSTEM == "Windows":
                try:
                    os.remove(os.path.join(DEPS_PATH, LIBCURL_NAME + ".install", "lib", "libcurl.dll.a"))
                except OSError:
                    pass
                ret = create_dll(arg_dir=[os.path.join(DEPS_PATH, LIBCURL_NAME + ".install", "lib")],
                        arg_output_dir=os.path.join(os.path.abspath(INSTALL_PATH), LIB_DIR),
                        arg_output_name='curl', arg_link_lib=[
                            'ws2_32', 'ssl', 'psl', 'ole32', 'idn2', 'unistring', 'crypto', 'z',
                            'intl', 'brotlidec-static', 'brotlicommon-static', 'nghttp2', 'iconv',
                            'wldap32'])
                if ret[0]:
                    failMsg("Building libcurl.")
                    os._exit(1)
        else:
            shutil.copy2(
                os.path.join(DEPS_PATH, LIBCURL_NAME + ".install", LIB_DIR.lower(),
                            "libcurl.{0}".format(SHARED_LIBS_EXT)),
                os.path.join(os.path.abspath(INSTALL_PATH), LIB_DIR,
                            "libcurl.{}".format(SHARED_LIBS_EXT))
            )

    except (IOError, OSError, subprocess.CalledProcessError, TypeError) as e:
        failMsg(str(e))
        failMsg("Building libcurl.")
        os._exit(1)
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

    old_path = os.path.abspath(os.getcwd())
    os.chdir(DEPS_PATH)

    if not os.path.exists(os.path.join(LIBZIP_NAME + ".src", "EXTRACTED")):
        if not os.path.exists('v0.1.14.zip'):
            subprocess.call(['wget', LIBZIP_SRC_URL])
            print('')  # Printing new line after 'wget'.
        with ZipFile(file='v0.1.14.zip', mode="r") as zip:
            infoMsg("Extracting libzip sources...")
            zip.extractall()
            os.rename(LIBZIP_NAME, LIBZIP_NAME + ".src")
        os.remove('v0.1.14.zip')
        with open(os.path.join(LIBZIP_NAME + ".src", "EXTRACTED"), "w") as fd:
            fd.write("LIBZIP EXTRACTED CHECKER")
        infoMsg("Finished extracting libzip sources.")
    else:
        infoMsg("libzip sources are already available.")

    if os.path.exists(os.path.join(os.path.abspath(INSTALL_PATH), LIB_DIR, "libzip.{}".format(SHARED_LIBS_EXT))):
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
                     "-DCMAKE_INSTALL_PREFIX={}".format(
                         os.path.join(DEPS_PATH, LIBZIP_NAME + ".install")),
                     os.path.join(DEPS_PATH, LIBZIP_NAME + ".src")]
        if THIS_SYSTEM == "Windows":
            cmake_cmd += [
                "-G", "MinGW Makefiles",
                "-DCMAKE_SH=CMAKE_SH-NOTFOUND"
            ]
        ret = subprocess.call(cmake_cmd)
        if ret != 0:
            failMsg("Building libzip.")
            os._exit(1)
        ret = subprocess.call(
            "{0} install -j{1}".format(MAKE_CMD, MAKE_THREAD_COUNT).split())
        if ret != 0:
            failMsg("Building libzip.")
            os._exit(1)
        if not os.path.exists(os.path.join(os.path.abspath(INSTALL_PATH), LIB_DIR)):
            os.makedirs(os.path.join(os.path.abspath(INSTALL_PATH), LIB_DIR))
        shared_lib_dir = "bin" if THIS_SYSTEM == "Windows" else "lib"
        shutil.copy2(
            os.path.join(DEPS_PATH, LIBZIP_NAME + ".install",
                         shared_lib_dir, "libzip.{}".format(SHARED_LIBS_EXT)),
            os.path.join(os.path.abspath(INSTALL_PATH), LIB_DIR)
        )
        # Copy import library to the install dirctory.
        if THIS_SYSTEM == "Windows":
            shutil.copy2(
                os.path.join(DEPS_PATH, LIBZIP_NAME + ".install",
                             "lib", "libzip.{}.a".format(SHARED_LIBS_EXT)),
                os.path.join(os.path.abspath(INSTALL_PATH), LIB_DIR)
            )
    except (IOError, OSError, subprocess.CalledProcessError) as e:
        failMsg(str(e))
        failMsg("Building libzip.")
        os._exit(1)
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

    old_path = os.path.abspath(os.getcwd())
    os.chdir(DEPS_PATH)

    dlfcn_folder_name = DLFCN_WIN32_NAME + '-' + DLFCN_WIN32_VERSION
    if not os.path.exists(os.path.join(dlfcn_folder_name + ".src", "EXTRACTED")):
        if not os.path.exists("v1.1.2.zip"):
            subprocess.call(['wget', DLFCN_WIN32_URL])
            print('')  # Printing new line after 'wget'.
        with ZipFile(file=("v1.1.2.zip"), mode="r") as zip:
            infoMsg("Extracting dlfcn-win32 sources...")
            zip.extractall()
        os.rename(dlfcn_folder_name, dlfcn_folder_name + ".src")
        os.remove("v1.1.2.zip")
        with open(os.path.join(dlfcn_folder_name + ".src", "EXTRACTED"), "w") as fd:
            fd.write("DLFCN-WIN32 EXTRACTED CHECKER")
        infoMsg("Finished extracting dlfcn-win32 sources.")
    else:
        infoMsg("dlfcn-win32 sources are already available.")

    if os.path.exists(os.path.join(os.path.abspath(INSTALL_PATH), LIB_DIR, "libdl.{}".format(SHARED_LIBS_EXT))):
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
                     "-DCMAKE_INSTALL_PREFIX={}".format(os.path.join(
                         DEPS_PATH, dlfcn_folder_name + ".install")),
                     os.path.join(DEPS_PATH, dlfcn_folder_name + ".src"), ]
        if THIS_SYSTEM == "Windows":
            cmake_cmd += [
                "-G", "MinGW Makefiles",
                "-DCMAKE_SH=CMAKE_SH-NOTFOUND"
            ]
        ret = subprocess.call(cmake_cmd)
        if ret != 0:
            failMsg("Building libdl.")
            os._exit(1)
        ret = subprocess.call(
            "{0} install -j{1}".format(MAKE_CMD, MAKE_THREAD_COUNT).split())
        if ret != 0:
            failMsg("Building libdl.")
            os._exit(1)

        if not os.path.exists(os.path.join(os.path.abspath(INSTALL_PATH), LIB_DIR)):
            try:
                os.makedirs(os.path.join(
                    os.path.abspath(INSTALL_PATH), LIB_DIR))
            except:
                failMsg(
                    "Cannot make \"" + os.path.join(os.path.abspath(INSTALL_PATH), LIB_DIR) + "\" directory.")
                os._exit(1)
        if not os.path.exists(os.path.join(os.path.abspath(INSTALL_PATH), LIB_DIR)):
            os.makedirs(os.path.join(os.path.abspath(INSTALL_PATH), LIB_DIR))
        shutil.copy2(
            os.path.join(DEPS_PATH, dlfcn_folder_name + ".install",
                         "bin", "libdl.{}".format(SHARED_LIBS_EXT)),
            os.path.join(os.path.abspath(INSTALL_PATH), LIB_DIR)
        )
        # Copy import library to the install dirctory.
        if THIS_SYSTEM == "Windows":
            shutil.copy2(
                os.path.join(DEPS_PATH, dlfcn_folder_name + ".install",
                             "lib", "libdl.{}.a".format(SHARED_LIBS_EXT)),
                os.path.join(os.path.abspath(INSTALL_PATH), LIB_DIR)
            )
    except (IOError, OSError, subprocess.CalledProcessError) as e:
        failMsg(str(e))
        failMsg("Building libdl.")
        os._exit(1)
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


def get_exe_and_dll_list():
    global INSTALL_PATH

    to_return = list()
    for exec_file in os.listdir(os.path.join(INSTALL_PATH, "Bin")):
        if exec_file.endswith(".exe") or exec_file.endswith(".dll"):
            to_return.append(os.path.join(
                os.path.join(INSTALL_PATH, "Bin", exec_file)))
    return to_return


# Copying MinGW DLL's to make a portable Alusus that does not rely on a MinGW install.
def copy_mingw_dlls():
    global INSTALL_PATH
    global THIS_SYSTEM

    if THIS_SYSTEM != "Windows":
        return

    infoMsg("Copying MinGW DLL's...")

    ignored_dirs = [
        subprocess.check_output(['cygpath', 'C:\\Windows\\System32']).decode(
            'utf8').strip().lower(),  # Default Windows DLL's.
        subprocess.check_output(['cygpath', os.path.join(INSTALL_PATH, 'Bin')]).decode(
            'utf8').strip().lower()  # Installation directory DLL's.
    ]

    exec_list = get_exe_and_dll_list()
    i = 0
    while i < len(exec_list):
        current_executable = exec_list[i]

        ldd_output = subprocess.check_output(
            ['ldd', current_executable]).decode('utf8').strip().split('\n')

        updated = False
        current_try = 1
        max_tries = 5
        j = 0
        while j < len(ldd_output):
            dependency = ldd_output[j]
            try:
                unix_dll_path = dependency.split()[2]
                windows_dll_path = subprocess.check_output(
                    ['cygpath', '-w', unix_dll_path]).decode('utf8').strip()
                unix_dll_path_dir = subprocess.check_output(
                    ['cygpath', os.path.dirname(windows_dll_path)]).decode('utf8').strip()
                if unix_dll_path_dir.lower() not in ignored_dirs:
                    shutil.copy2(windows_dll_path,
                                 os.path.join(INSTALL_PATH, 'Bin'))
                    i = 0
                    exec_list = get_exe_and_dll_list()
                    updated = True
                j += 1
            except subprocess.CalledProcessError:
                if current_try <= max_tries:
                    print('Error: running LDD again on \"{0}\" (try {1}/{2})...'.format(
                        current_executable, current_try, max_tries))
                    ldd_output = subprocess.check_output(
                        ['ldd', current_executable]).decode('utf8').strip().split('\n')
                    j = 0
                    current_try += 1
                else:
                    j += 1
        if not updated:
            i += 1

    infoMsg("Finished Copying MinGW DLL's.")


def copy_other_installation_files():
    global ALUSUS_ROOT
    global INSTALL_PATH
    global THIS_SYSTEM

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


def build_alusus():
    global BUILD_PATH
    global ALUSUS_ROOT
    global BUILD_TYPE
    global INSTALL_PATH
    global DEPS_PATH
    global PYTHON_DEPS_PATH
    global LLVM_NAME
    global DLFCN_WIN32_NAME
    global DLFCN_WIN32_VERSION
    global MAKE_THREAD_COUNT
    global CREATE_PACKAGES
    global THIS_SYSTEM

    infoMsg("Building Alusus...")
    if not os.path.isdir(BUILD_PATH):
        os.makedirs(BUILD_PATH)
    old_path = os.path.abspath(os.getcwd())
    os.chdir(BUILD_PATH)

    try:
        # CMake will not run everytime by default to reduce build time.
        if (not os.path.exists('CMAKE_CHECKER')) or global_args['rerunCMake']:
            cmake_cmd = ["cmake",
                         "{}".format(os.path.join(ALUSUS_ROOT, "Sources")),
                         "-DCMAKE_BUILD_TYPE={}".format(BUILD_TYPE),
                         "-DCMAKE_INSTALL_PREFIX={}".format(INSTALL_PATH if THIS_SYSTEM != "Windows" else INSTALL_PATH.replace('\\', '/')),
                         "-DLLVM_PATH={}".format(
                             (os.path.join(DEPS_PATH, LLVM_NAME + ".install").replace('\\', '/') if THIS_SYSTEM == "Windows" else
                              os.path.join(DEPS_PATH, LLVM_NAME + ".install"))
                         ),
                         "-DPYTHON_EXECUTABLE={}".format(sys.executable),
                         "-DPYTHON_PREFIX={}".format(PYTHON_DEPS_PATH)]

            if THIS_SYSTEM == "Windows":
                dlfcn_folder_name = DLFCN_WIN32_NAME + '-' + DLFCN_WIN32_VERSION
                cmake_cmd += [
                    "-G", "MinGW Makefiles",
                    "-DCMAKE_SH=CMAKE_SH-NOTFOUND",
                    "-DDLFCN_WIN32_PATH={}".format(os.path.join(
                        DEPS_PATH, dlfcn_folder_name + ".install").replace('\\', '/'))
                ]

            ret = subprocess.call(cmake_cmd)
            if ret != 0:
                failMsg("Building Alusus.")
                os._exit(1)

            with open('CMAKE_CHECKER', 'w') as fd:
                fd.write("CMAKE GENERATOR CHECKER")

        ret = subprocess.call(
            "{0} install -j{1}".format(MAKE_CMD, MAKE_THREAD_COUNT).split())
        if ret != 0:
            failMsg("Building Alusus.")
            os._exit(1)
        if THIS_SYSTEM == "Windows":
            copy_mingw_dlls()

    except (IOError, OSError, subprocess.CalledProcessError) as e:
        failMsg(str(e))
        failMsg("Building Alusus.")
        os._exit(1)
    os.chdir(old_path)
    successMsg("Building Alusus.")


def generate_iscc_script():
    global BUILD_PATH
    global INSTALL_PATH
    global PACKAGES_PATH
    global BUILD_TYPE
    global ALUSUS_VERSION
    global ALUSUS_ROOT

    packages_path = os.path.join(
        PACKAGES_PATH, BUILD_TYPE[0].upper() + BUILD_TYPE[1:])
    script = "[Setup]\n" + \
             "AppName=Alusus\n" + \
             "AppVersion={}\n".format(ALUSUS_VERSION) + \
             "AppCopyright=\"Copyright © 2014 Alusus Software Ltd.\"\n" + \
             "WizardStyle=modern\n" + \
             "WizardSmallImageFile=\"{}\"\n".format(os.path.join(ALUSUS_ROOT, 'Tools', 'Res', 'logo.en.bmp')) + \
             "WizardImageFile=\"{}\"\n".format(os.path.join(ALUSUS_ROOT, 'Tools', 'Res', 'banner.bmp')) + \
             "SetupIconFile=\"{}\"\n".format(os.path.join(ALUSUS_ROOT, 'Tools', 'Res', 'icon.en.ico')) + \
             "UninstallDisplayIcon=\"{}\"\n".format(os.path.join(ALUSUS_ROOT, 'Tools', 'Res', 'icon.en.ico')) + \
             "DefaultDirName=\"{}\\Alusus\"\n".format('C:\\Program Files' if os.environ['MINGW_HOST'] == 'x86_64-w64-mingw32' else 'C:\\Program Files (x86)') + \
             "DefaultGroupName=Alusus\n" + \
             "Compression=lzma2\n" + \
             "SolidCompression=yes\n" + \
             "OutputDir=\"{}\"\n".format(packages_path) + \
             "OutputBaseFilename=alusus_{0}_{1}\n\n".format(ALUSUS_VERSION, os.environ['MINGW_HOST']) + \
             "[Files]\n" + \
             "Source: \"{}\"; DestDir: \"{{app}}\"; Flags: ignoreversion recursesubdirs\n\n".format(os.path.join(INSTALL_PATH, '*')) + \
             "[Tasks]\n" + \
             "Name: \"desktopicon\"; Description: \"{cm:CreateDesktopIcon}\"; GroupDescription: \"{cm:AdditionalIcons}\"; Flags: unchecked\n\n" + \
             "[Icons]\n" + \
             "Name: \"{{group}}\\Alusus (Interactive Mode)\"; Filename: \"{{app}}\\Bin\\alusus.exe\"; WorkingDir: \"{{app}}\"; Parameters: \"-i\"; IconFilename: \"{}\"\n".format(os.path.join(ALUSUS_ROOT, 'Tools', 'Res', 'icon.en.ico')) + \
             "Name: \"{{userdesktop}}\\Alusus (Interactive Mode)\"; Filename: \"{{app}}\\Bin\\alusus.exe\"; WorkingDir: \"{{app}}\"; Parameters: \"-i\"; IconFilename: \"{}\"; Tasks: desktopicon\n".format(
                 os.path.join(ALUSUS_ROOT, 'Tools', 'Res', 'icon.en.ico'))
    with open(os.path.join(packages_path, 'alusus_{0}_{1}.iss'.format(ALUSUS_VERSION, os.environ['MINGW_HOST'])), 'w') as fd:
        fd.write(script)


def create_packages_windows():
    global PACKAGES_PATH
    global BUILD_TYPE
    global ALUSUS_VERSION

    generate_iscc_script()
    packages_path = os.path.join(
        PACKAGES_PATH, BUILD_TYPE[0].upper() + BUILD_TYPE[1:])
    ret = subprocess.call(['ISCC', os.path.join(
        packages_path, 'alusus_{0}_{1}.iss'.format(ALUSUS_VERSION, os.environ['MINGW_HOST']))])
    if ret != 0:
        failMsg("Creating EXE Package.")
        os._exit(1)


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

    old_path = os.path.abspath(os.getcwd())
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
        os._exit(1)

    # Create additional package of type RPM for Linux systems.
    if THIS_SYSTEM == "Linux":
        # Replace "deb" with "rpm".
        current_cmd[current_cmd.index("-t") + 1] = "rpm"

        ret = subprocess.call(current_cmd)
        if ret != 0:
            failMsg("Creating RPM Package.")
            os._exit(1)

    os.chdir(old_path)


def create_packages():
    global PACKAGES_PATH
    global BUILD_TYPE
    global THIS_SYSTEM

    infoMsg("Creating installation packages...")
    packages_path = os.path.join(
        PACKAGES_PATH, BUILD_TYPE[0].upper() + BUILD_TYPE[1:])
    shutil.rmtree(packages_path, ignore_errors=True)
    os.makedirs(packages_path)
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
    if THIS_SYSTEM == "Windows" and 'MINGW_HOST' not in os.environ:
        errMsg('\"MINGW_HOST\" environment variable is not defined. Possible values are either \"i686-w64-mingw32\" or \"x86_64-w64-mingw32\".')
        os._exit(1)
    elif THIS_SYSTEM == "Windows" and os.environ['MINGW_HOST'] not in ['i686-w64-mingw32', 'x86_64-w64-mingw32']:
        errMsg('Wrong value for \"MINGW_HOST\" environment variable is set. Possible values are either \"i686-w64-mingw32\" or \"x86_64-w64-mingw32\".')
        os._exit(1)
    prep_debs()
    build_alusus()
    if CREATE_PACKAGES == "yes":
        create_packages()
