import os
import sys
import multiprocessing
import wget
import shutil
import subprocess
import tarfile
import platform
from whichcraft import which
SOURCE_LOCATION = os.path.abspath(__file__)
sys.path.insert(0, os.path.dirname(os.path.dirname(SOURCE_LOCATION)))
from build_deps import template_build, build_zlib, build_openssl  # noqa
from msg import info_msg, success_msg, fail_msg  # noqa
from utils import get_host_cxx_arch, unix_copy2  # noqa
from custom_cc_cxx import create_new_environ_with_custom_cc_cxx  # noqa


_NGHTTP2_SRC_URL = "https://github.com/nghttp2/nghttp2/archive/v1.40.0.tar.gz"


class build_nghttp2(template_build.template_build):
    def _check_built(install_path, target_system=None):
        if target_system == "windows" or platform.system() == "Windows" and not target_system:
            return os.path.exists(os.path.join(install_path, "Bin", "libnghttp2.dll")) and\
                os.path.exists(os.path.join(
                    install_path, "Lib", "libnghttp2.dll.a"))
        elif target_system == "linux" or platform.system() == "Linux" and not target_system:
            return os.path.exists(os.path.join(install_path, "Lib", "libnghttp2.so.14.19.0")) and\
                os.path.exists(os.path.join(install_path, "Lib", "libnghttp2.so.14")) and\
                os.path.exists(os.path.join(
                    install_path, "Lib", "libnghttp2.so"))
        elif target_system == "macos" or platform.system() == "Darwin" and not target_system:
            return os.path.exists(os.path.join(install_path, "Lib", "libnghttp2.14.19.0.dylib")) and\
                os.path.exists(os.path.join(install_path, "Lib", "libnghttp2.14.dylib")) and\
                os.path.exists(os.path.join(
                    install_path, "Lib", "libnghttp2.dylib"))
        return False

    def build(deps_path, install_path, num_threads=multiprocessing.cpu_count(), target_system=None):
        global _NGHTTP2_SRC_URL

        info_msg("Building nghttp2 1.40.0...")

        # Build dependecies.
        info_msg("Building nghttp2 1.40.0 dependencies...")
        ret = build_zlib.build_zlib.build(deps_path, install_path,
                                          num_threads=num_threads, target_system=target_system)
        if not ret:
            fail_msg("Building nghttp2 1.40.0.")
            return False
        ret = build_openssl.build_openssl.build(deps_path, install_path,
                                                num_threads=num_threads, target_system=target_system)
        if not ret:
            fail_msg("Building nghttp2 1.40.0.")
            return False
        success_msg("Building nghttp2 1.40.0 dependencies.")

        # Check built.
        if build_nghttp2._check_built(install_path, target_system=target_system):
            success_msg("Building nghttp2 1.40.0.")
            return True

        os.makedirs(deps_path, exist_ok=True)
        os.makedirs(install_path, exist_ok=True)

        original_dir = os.getcwd()
        os.chdir(deps_path)

        # Download nghttp2.
        if not os.path.exists(os.path.join("nghttp2-1.40.0.src", "EXTRACTED")):
            info_msg("Getting nghttp2 1.40.0 sources...")
            try:
                os.remove("nghttp2-1.40.0.tar.gz")
            except FileNotFoundError:
                pass
            try:
                shutil.rmtree("nghttp2-1.40.0.src")
            except FileNotFoundError:
                pass
            try:
                shutil.rmtree("nghttp2-1.40.0")
            except FileNotFoundError:
                pass
            wget.download(_NGHTTP2_SRC_URL)
            # Print new line after "wget.download", as the latter method will not do so.
            print("")
            with tarfile.open("nghttp2-1.40.0.tar.gz") as fd:
                fd.extractall(".")
            os.rename("nghttp2-1.40.0", "nghttp2-1.40.0.src")
            try:
                os.remove("nghttp2-1.40.0.tar.gz")
            except FileNotFoundError:
                pass
            with open(os.path.join("nghttp2-1.40.0.src", "EXTRACTED"), "w") as fd:
                fd.write("EXTRACTED CHECKER")
            success_msg("nghttp2 1.40.0 sources have been downloaded.")
        else:
            success_msg("nghttp2 1.40.0 sources are already downloaded.")

        # Build nghttp2.
        os.makedirs("nghttp2-1.40.0.build", exist_ok=True)
        os.makedirs("nghttp2-1.40.0.install", exist_ok=True)
        os.chdir("nghttp2-1.40.0.build")

        new_environ = os.environ.copy()
        host_sep = ";" if platform.system() == "Windows" else ":"
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
        new_environ = create_new_environ_with_custom_cc_cxx(
            new_environ, target_system=target_system)

        cmake_cmd = [
            "cmake",
            os.path.join(deps_path, "nghttp2-1.40.0.src"),
            "-DCMAKE_INSTALL_PREFIX={}".format(
                os.path.join(deps_path, "nghttp2-1.40.0.install")),
            "-DCMAKE_BUILD_TYPE=Release",
            "-DCMAKE_SYSTEM_PROCESSOR={}".format(
                get_host_cxx_arch(new_environ=new_environ)),
            "-DCMAKE_CROSSCOMPILING=TRUE",
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
        if target_system == "windows" or platform.system() == "Windows" and not target_system:
            cmake_cmd.append("-DCMAKE_RC_COMPILER={}".format(
                which(new_environ["RC"] if "RC" in new_environ else "windres")))
        p = subprocess.Popen(cmake_cmd, env=new_environ)
        ret = p.wait()
        if ret:
            fail_msg("Building nghttp2 1.40.0.")
            os.chdir(original_dir)
            return False
        p = subprocess.Popen(
            [("mingw32-make" if (platform.system() == "Windows") else "make"), "-j{}".format(num_threads), "install"], env=new_environ)
        ret = p.wait()
        if ret:
            fail_msg("Building nghttp2 1.40.0.")
            os.chdir(original_dir)
            return False

        if target_system == "windows" or platform.system() == "Windows" and not target_system:
            shutil.copy2(
                os.path.join(deps_path, "nghttp2-1.40.0.install",
                             "bin", "libnghttp2.dll"),
                os.path.join(install_path, "Bin", "libnghttp2.dll")
            )
            shutil.copy2(
                os.path.join(deps_path, "nghttp2-1.40.0.install",
                             "lib", "libnghttp2.dll.a"),
                os.path.join(install_path, "Lib", "libnghttp2.dll.a")
            )
        elif target_system == "linux" or platform.system() == "Linux" and not target_system:
            unix_copy2(
                os.path.join(deps_path, "nghttp2-1.40.0.install",
                             "lib", "libnghttp2.so.14.19.0"),
                os.path.join(install_path, "Lib", "libnghttp2.so.14.19.0")
            )
            unix_copy2(
                os.path.join(deps_path, "nghttp2-1.40.0.install",
                             "lib", "libnghttp2.so.14"),
                os.path.join(install_path, "Lib", "libnghttp2.so.14")
            )
            unix_copy2(
                os.path.join(deps_path, "nghttp2-1.40.0.install",
                             "lib", "libnghttp2.so"),
                os.path.join(install_path, "Lib", "libnghttp2.so")
            )
        elif target_system == "macos" or platform.system() == "Darwin" and not target_system:
            unix_copy2(
                os.path.join(deps_path, "nghttp2-1.40.0.install",
                             "lib", "libnghttp2.14.19.0.dylib"),
                os.path.join(install_path, "Lib", "libnghttp2.14.19.0.dylib")
            )
            unix_copy2(
                os.path.join(deps_path, "nghttp2-1.40.0.install",
                             "lib", "libnghttp2.14.dylib"),
                os.path.join(install_path, "Lib", "libnghttp2.14.dylib")
            )
            unix_copy2(
                os.path.join(deps_path, "nghttp2-1.40.0.install",
                             "lib", "libnghttp2.dylib"),
                os.path.join(install_path, "Lib", "libnghttp2.dylib")
            )

        success_msg("Building nghttp2 1.40.0.")
        os.chdir(original_dir)
        return True

    def get_dep_environ(deps_path, target_system=None):
        new_environ = os.environ.copy()
        host_sep = ";" if platform.system() == "Windows" else ":"
        new_environ["LIBRARY_PATH"] = os.path.join(deps_path, "nghttp2-1.40.0.install", "lib") +\
            ("" if ("LIBRARY_PATH" not in new_environ)
             else (host_sep + new_environ["LIBRARY_PATH"]))
        new_environ["PKG_CONFIG_PATH"] = os.path.join(deps_path, "nghttp2-1.40.0.install", "lib", "pkgconfig") +\
            ("" if ("PKG_CONFIG_PATH" not in new_environ)
             else (host_sep + new_environ["PKG_CONFIG_PATH"]))
        new_environ["CPATH"] = os.path.join(deps_path, "nghttp2-1.40.0.install", "include") +\
            ("" if ("CPATH" not in new_environ)
             else (host_sep + new_environ["CPATH"]))
        return new_environ
