import os
import sys
import multiprocessing
import wget
import shutil
import tarfile
import subprocess
import platform
from whichcraft import which
SOURCE_LOCATION = os.path.abspath(__file__)
sys.path.insert(0, os.path.dirname(os.path.dirname(SOURCE_LOCATION)))
from build_deps import template_build  # noqa
from msg import info_msg, success_msg, fail_msg  # noqa
from utils import get_host_cxx_arch  # noqa
from custom_cc_cxx import create_new_environ_with_custom_cc_cxx  # noqa


_DLFCN_WIN32_URL = "https://github.com/dlfcn-win32/dlfcn-win32/archive/v1.2.0.tar.gz"


class build_dlfcn_win32(template_build.template_build):
    def _check_built(install_path, target_system=None):
        if target_system == "windows" or platform.system() == "Windows" and not target_system:
            return os.path.exists(os.path.join(install_path["root"], install_path["lib"], "libdl.dll.a")) and \
                os.path.exists(os.path.join(
                    install_path["root"], install_path["bin"], "libdl.dll"))
        return False

    def build(deps_path, install_path, num_threads=multiprocessing.cpu_count(), target_system=None):
        global _DLFCN_WIN32_URL

        info_msg("Building dlfcn-win32 1.2.0...")

        if build_dlfcn_win32._check_built(install_path, target_system=target_system):
            success_msg("Building dlfcn-win32 1.2.0.")
            return True

        os.makedirs(deps_path, exist_ok=True)
        os.makedirs(install_path["root"], exist_ok=True)

        original_dir = os.getcwd()
        os.chdir(deps_path)

        # Download dlfcn-win32.
        if not os.path.exists(os.path.join("dlfcn-win32-1.2.0.src", "EXTRACTED")):
            info_msg("Getting dlfcn-win32 1.2.0 sources...")
            try:
                os.remove("dlfcn-win32-1.2.0.tar.gz")
            except FileNotFoundError:
                pass
            try:
                shutil.rmtree("dlfcn-win32-1.2.0.src")
            except FileNotFoundError:
                pass
            try:
                shutil.rmtree("dlfcn-win32-1.2.0")
            except FileNotFoundError:
                pass
            wget.download(_DLFCN_WIN32_URL)
            # Print new line after "wget.download", as the latter method will not do so.
            print("")
            with tarfile.open("dlfcn-win32-1.2.0.tar.gz") as fd:
                fd.extractall(".")
            os.rename("dlfcn-win32-1.2.0", "dlfcn-win32-1.2.0.src")
            try:
                os.remove("dlfcn-win32-1.2.0.tar.gz")
            except FileNotFoundError:
                pass
            with open(os.path.join("dlfcn-win32-1.2.0.src", "EXTRACTED"), "w") as fd:
                fd.write("EXTRACTED CHECKER")
            success_msg("dlfcn-win32 1.2.0 sources have been downloaded.")
        else:
            success_msg("dlfcn-win32 1.2.0 sources are already downloaded.")

        os.makedirs("dlfcn-win32-1.2.0.build", exist_ok=True)
        os.makedirs("dlfcn-win32-1.2.0.install", exist_ok=True)

        os.chdir("dlfcn-win32-1.2.0.build")

        new_environ = os.environ.copy()
        new_environ = create_new_environ_with_custom_cc_cxx(
            new_environ, target_system=target_system)
        cmake_cmd = ["cmake",
                     os.path.join(deps_path, "dlfcn-win32-1.2.0.src"),
                     "-DCMAKE_BUILD_TYPE=Release",
                     "-DPYTHON_EXECUTABLE={}".format(sys.executable),
                     "-DCMAKE_INSTALL_PREFIX={}".format(
                         os.path.join(deps_path, "dlfcn-win32-1.2.0.install")),
                     "-DCMAKE_RANLIB={}".format(
                         which(new_environ["RANLIB"] if "RANLIB" in new_environ else "ranlib")),
                     "-DCMAKE_AR={}".format(
                         which(new_environ["AR"] if "AR" in new_environ else "ar")),
                     "-DCMAKE_LINKER={}".format(
                         which(new_environ["LD"] if "LD" in new_environ else "ld")),
                     "-DCMAKE_STRIP={}".format(which(new_environ["STRIP"] if "STRIP" in new_environ else "strip"))]
        if target_system == "windows":
            cmake_system_name = "Windows" if (target_system == "windows") else (
                "Darwin" if (target_system == "macos") else "Linux")
            cmake_cmd += [
                "-DCMAKE_CROSSCOMPILING=TRUE",
                "-DCMAKE_SYSTEM_NAME={}".format(cmake_system_name),
                "-DCMAKE_SYSTEM_PROCESSOR={}".format(get_host_cxx_arch())
            ]
        if platform.system() == "Windows":
            cmake_cmd += [
                "-G", "MinGW Makefiles",
                "-DCMAKE_SH=CMAKE_SH-NOTFOUND"
            ]
        p = subprocess.Popen(cmake_cmd, env=new_environ)
        ret = p.wait()
        if ret:
            fail_msg("Building dlfcn-win32 1.2.0.")
            os.chdir(original_dir)
            return False
        p = subprocess.Popen([("mingw32-make" if (platform.system() == "Windows")
                               else "make"), "-j{}".format(num_threads), "install"], env=new_environ)
        ret = p.wait()
        if ret:
            fail_msg("Building dlfcn-win32 1.2.0.")
            os.chdir(original_dir)
            return False

        os.makedirs(os.path.join(
            install_path["root"], install_path["lib"]), exist_ok=True)
        os.makedirs(os.path.join(
            install_path["root"], install_path["bin"]), exist_ok=True)
        if platform.system() == "Windows" and not target_system or target_system == "windows":
            shutil.copy2(
                os.path.join(deps_path, "dlfcn-win32-1.2.0.install",
                             "lib", "libdl.dll.a"),
                os.path.join(install_path["root"],
                             install_path["lib"], "libdl.dll.a")
            )
            shutil.copy2(
                os.path.join(deps_path, "dlfcn-win32-1.2.0.install",
                             "bin", "libdl.dll"),
                os.path.join(install_path["root"],
                             install_path["bin"], "libdl.dll")
            )

        success_msg("Building dlfcn-win32 1.2.0.")
        os.chdir(original_dir)
        return True

    def get_dep_environ(deps_path, target_system=None):
        host_sep = ":" if platform.system() != "Windows" else ";"
        new_environ = os.environ.copy()
        new_environ["CPATH"] = os.path.join(deps_path, "dlfcn-win32-1.2.0.install", "include") +\
            ("" if ("CPATH" not in new_environ) else
                (host_sep + new_environ["CPATH"]))
        new_environ["LIBRARY_PATH"] = os.path.join(deps_path, "dlfcn-win32-1.2.0.install", "lib") +\
            ("" if ("LIBRARY_PATH" not in new_environ) else
                (host_sep + new_environ["LIBRARY_PATH"]))
        return new_environ
