import os
import sys
import multiprocessing
import wget
import shutil
import subprocess
import tarfile
import platform
import distutils.dir_util
from whichcraft import which
SOURCE_LOCATION = os.path.abspath(__file__)
sys.path.insert(0, os.path.dirname(os.path.dirname(SOURCE_LOCATION)))
from build_deps import template_build, build_zlib, build_openssl  # noqa
from msg import info_msg, success_msg, fail_msg  # noqa
from utils import get_host_cxx_arch, unix_copy2  # noqa
from custom_cc_cxx import create_new_environ_with_custom_cc_cxx  # noqa


_LIBZIP_SRC_URL = "https://libzip.org/download/libzip-1.6.1.tar.gz"


class build_libzip(template_build.template_build):
    def _check_built(install_path, target_system=None):
        if target_system == "windows" or platform.system() == "Windows" and not target_system:
            return os.path.exists(os.path.join(install_path, "Bin", "libzip.dll")) and\
                os.path.exists(os.path.join(
                    install_path, "Lib", "libzip.dll.a"))
        elif target_system == "linux" or platform.system() == "Linux" and not target_system:
            return os.path.exists(os.path.join(install_path, "Lib", "libzip.so.5.1")) and\
                os.path.exists(os.path.join(install_path, "Lib", "libzip.so.5")) and\
                os.path.exists(os.path.join(install_path, "Lib", "libzip.so"))
        return False

    def build(deps_path, install_path, num_threads=multiprocessing.cpu_count(), target_system=None):
        global _LIBZIP_SRC_URL

        info_msg("Building libzip 1.6.1...")

        # Build dependecies.
        info_msg("Building libzip 1.6.1 dependencies...")
        ret = build_zlib.build_zlib.build(deps_path, install_path,
                                          num_threads=num_threads, target_system=target_system)
        if not ret:
            fail_msg("Building libzip 1.6.1.")
            return False
        ret = build_openssl.build_openssl.build(deps_path, install_path,
                                                num_threads=num_threads, target_system=target_system)
        if not ret:
            fail_msg("Building libzip 1.6.1.")
            return False
        success_msg("Building libzip 1.6.1 dependencies.")

        # Check built.
        if build_libzip._check_built(install_path, target_system=target_system):
            success_msg("Building libzip 1.6.1.")
            return True

        os.makedirs(deps_path, exist_ok=True)
        os.makedirs(install_path, exist_ok=True)

        original_dir = os.getcwd()
        os.chdir(deps_path)
        # Download libzip.
        if not os.path.exists(os.path.join("libzip-1.6.1.src", "EXTRACTED")):
            info_msg("Getting libzip 1.6.1 sources...")
            try:
                os.remove("libzip-1.6.1.tar.gz")
            except FileNotFoundError:
                pass
            try:
                shutil.rmtree("libzip-1.6.1.src")
            except FileNotFoundError:
                pass
            try:
                shutil.rmtree("libzip-1.6.1")
            except FileNotFoundError:
                pass
            wget.download(_LIBZIP_SRC_URL)
            # Print new line after "wget.download", as the latter method will not do so.
            print("")
            with tarfile.open("libzip-1.6.1.tar.gz") as fd:
                fd.extractall(".")
            os.rename("libzip-1.6.1", "libzip-1.6.1.src")
            try:
                os.remove("libzip-1.6.1.tar.gz")
            except FileNotFoundError:
                pass

            # Apply the patch(es).
            patches_path = os.path.join(os.path.dirname(
                os.path.dirname(os.path.dirname(SOURCE_LOCATION))), "Patches")
            distutils.dir_util.copy_tree(
                os.path.join(
                    patches_path, "libzip-1.6.1.src"),
                "libzip-1.6.1.src"
            )

            with open(os.path.join("libzip-1.6.1.src", "EXTRACTED"), "w") as fd:
                fd.write("EXTRACTED CHECKER")
            success_msg("libzip 1.6.1 sources have been downloaded.")
        else:
            success_msg("libzip 1.6.1 sources are already downloaded.")

        # Build libzip.
        os.makedirs("libzip-1.6.1.build", exist_ok=True)
        os.makedirs("libzip-1.6.1.install", exist_ok=True)
        os.chdir("libzip-1.6.1.build")

        host_sep = ";" if platform.system() == "Windows" else ":"
        new_environ = os.environ.copy()
        zlib_environ = build_zlib.build_zlib.get_dep_environ(
            deps_path, target_system=target_system)
        new_environ["PKG_CONFIG_PATH"] = zlib_environ["PKG_CONFIG_PATH"] +\
            ("" if ("PKG_CONFIG_PATH" not in new_environ)
                else (host_sep + new_environ["PKG_CONFIG_PATH"]))
        new_environ["CPATH"] = zlib_environ["CPATH"] +\
            ("" if ("CPATH" not in new_environ)
                else (host_sep + new_environ["CPATH"]))
        new_environ["LIBRARY_PATH"] = zlib_environ["LIBRARY_PATH"] +\
            ("" if ("LIBRARY_PATH" not in new_environ)
                else (host_sep + new_environ["LIBRARY_PATH"]))
        openssl_environ = build_openssl.build_openssl.get_dep_environ(
            deps_path, target_system=target_system)
        new_environ["PKG_CONFIG_PATH"] = openssl_environ["PKG_CONFIG_PATH"] +\
            ("" if ("PKG_CONFIG_PATH" not in new_environ)
                else (host_sep + new_environ["PKG_CONFIG_PATH"]))
        new_environ["CPATH"] = openssl_environ["CPATH"] +\
            ("" if ("CPATH" not in new_environ)
                else (host_sep + new_environ["CPATH"]))
        new_environ["LIBRARY_PATH"] = openssl_environ["LIBRARY_PATH"] +\
            ("" if ("LIBRARY_PATH" not in new_environ)
                else (host_sep + new_environ["LIBRARY_PATH"]))
        new_environ = create_new_environ_with_custom_cc_cxx(
            new_environ, target_system=target_system)

        cmake_cmd = [
            "cmake",
            os.path.join(deps_path, "libzip-1.6.1.src"),
            "-DCMAKE_INSTALL_PREFIX={}".format(
                os.path.join(deps_path, "libzip-1.6.1.install")),
            "-DCMAKE_BUILD_TYPE=Release",
            "-DCMAKE_SYSTEM_PROCESSOR={}".format(
                get_host_cxx_arch(new_environ=new_environ)),
            "-DCMAKE_CROSSCOMPILING=TRUE",
            "-DOPENSSL_ROOT_DIR={}".format(
                openssl_environ["OPENSSL_ROOT_DIR"]),
            "-DCMAKE_RANLIB={}".format(
                which(new_environ["RANLIB"] if "RANLIB" in new_environ else "ranlib")),
            "-DCMAKE_AR={}".format(which(new_environ["AR"]
                                         if "AR" in new_environ else "ar")),
            "-DCMAKE_LINKER={}".format(
                which(new_environ["LD"] if "LD" in new_environ else "ld")),
            "-DCMAKE_STRIP={}".format(
                which(new_environ["STRIP"] if "STRIP" in new_environ else "strip"))
        ]
        if target_system != None:
            cmake_system_name = "Windows" if (target_system == "windows") else (
                "Darwin" if (target_system == "macos") else "Linux")
            cmake_cmd.append(
                "-DCMAKE_SYSTEM_NAME={}".format(cmake_system_name)
            )
        p = subprocess.Popen(cmake_cmd, env=new_environ)
        ret = p.wait()
        if ret:
            fail_msg("Building libzip 1.6.1.")
            os.chdir(original_dir)
            return False
        p = subprocess.Popen(
            [("mingw32-make" if (platform.system() == "Windows") else "make"), "-j{}".format(num_threads), "install"], env=new_environ)
        ret = p.wait()
        if ret:
            fail_msg("Building libzip 1.6.1.")
            os.chdir(original_dir)
            return False

        if target_system == "windows" or platform.system() == "Windows" and not target_system:
            shutil.copy2(
                os.path.join(deps_path, "libzip-1.6.1.install",
                             "bin", "libzip.dll"),
                os.path.join(install_path, "Bin", "libzip.dll")
            )
            shutil.copy2(
                os.path.join(deps_path, "libzip-1.6.1.install",
                             "lib", "libzip.dll.a"),
                os.path.join(install_path, "Lib", "libzip.dll.a")
            )
        elif target_system == "linux" or platform.system() == "Linux" and not target_system:
            unix_copy2(
                os.path.join(deps_path, "libzip-1.6.1.install",
                             "lib", "libzip.so.5.1"),
                os.path.join(install_path, "Lib", "libzip.so.5.1")
            )
            unix_copy2(
                os.path.join(deps_path, "libzip-1.6.1.install",
                             "lib", "libzip.so.5"),
                os.path.join(install_path, "Lib", "libzip.so.5")
            )
            unix_copy2(
                os.path.join(deps_path, "libzip-1.6.1.install",
                             "lib", "libzip.so"),
                os.path.join(install_path, "Lib", "libzip.so")
            )

        success_msg("Building libzip 1.6.1.")
        os.chdir(original_dir)
        return True

    def get_dep_environ(deps_path, target_system=None):
        new_environ = os.environ.copy()
        host_sep = ";" if platform.system() == "Windows" else ":"
        new_environ["LIBRARY_PATH"] = os.path.join(deps_path, "libzip-1.6.1.install", "lib") +\
            ("" if ("LIBRARY_PATH" not in new_environ)
             else (host_sep + new_environ["LIBRARY_PATH"]))
        new_environ["PKG_CONFIG_PATH"] = os.path.join(deps_path, "libzip-1.6.1.install", "lib", "pkgconfig") +\
            ("" if ("PKG_CONFIG_PATH" not in new_environ)
             else (host_sep + new_environ["PKG_CONFIG_PATH"]))
        new_environ["CPATH"] = os.path.join(deps_path, "libzip-1.6.1.install", "include") +\
            ("" if ("CPATH" not in new_environ)
             else (host_sep + new_environ["CPATH"]))
        return new_environ
