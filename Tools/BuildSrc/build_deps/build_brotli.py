import os
import sys
import multiprocessing
import wget
import shutil
import subprocess
import tarfile
import platform
SOURCE_LOCATION = os.path.abspath(__file__)
sys.path.insert(0, os.path.dirname(os.path.dirname(SOURCE_LOCATION)))
from build_deps import template_build  # noqa
from msg import info_msg, success_msg, fail_msg  # noqa
from utils import get_host_cxx_arch, unix_copy2, which  # noqa
from custom_cc_cxx import create_new_environ_with_custom_cc_cxx  # noqa


_BROTLI_SRC_URL = "https://github.com/google/brotli/archive/v1.0.7.tar.gz"


class build_brotli(template_build.template_build):
    def _check_built(install_path, target_system=None):
        # Check for unfinished RPATHs updating process exists.
        if os.path.exists(os.path.join(os.environ["ALUSUS_TMP_DIR"], "UPDATING_RPATHS")):
            return False
        if target_system == "windows" or platform.system() == "Windows" and not target_system:
            return os.path.exists(os.path.join(install_path["root"], install_path["bin"], "libbrotlicommon.dll")) and\
                os.path.exists(os.path.join(install_path["root"], install_path["bin"], "libbrotlidec.dll")) and\
                os.path.exists(os.path.join(install_path["root"], install_path["bin"], "libbrotlienc.dll")) and\
                os.path.exists(os.path.join(install_path["root"], install_path["lib"], "libbrotlicommon.dll.a")) and\
                os.path.exists(os.path.join(install_path["root"], install_path["lib"], "libbrotlidec.dll.a")) and\
                os.path.exists(os.path.join(
                    install_path["root"], install_path["lib"], "libbrotlienc.dll.a"))
        elif target_system == "linux" or platform.system() == "Linux" and not target_system:
            return os.path.exists(os.path.join(install_path["root"], install_path["lib"], "libbrotlicommon.so.1.0.7")) and\
                os.path.exists(os.path.join(install_path["root"], install_path["lib"], "libbrotlidec.so.1.0.7")) and\
                os.path.exists(os.path.join(install_path["root"], install_path["lib"], "libbrotlienc.so.1.0.7")) and\
                os.path.exists(os.path.join(install_path["root"], install_path["lib"], "libbrotlicommon.so.1")) and\
                os.path.exists(os.path.join(install_path["root"], install_path["lib"], "libbrotlidec.so.1")) and\
                os.path.exists(os.path.join(install_path["root"], install_path["lib"], "libbrotlienc.so.1")) and\
                os.path.exists(os.path.join(install_path["root"], install_path["lib"], "libbrotlicommon.so")) and\
                os.path.exists(os.path.join(install_path["root"], install_path["lib"], "libbrotlidec.so")) and\
                os.path.exists(os.path.join(
                    install_path["root"], install_path["lib"], "libbrotlienc.so"))
        elif target_system == "macos" or platform.system() == "Darwin" and not target_system:
            return os.path.exists(os.path.join(install_path["root"], install_path["lib"], "libbrotlicommon.1.0.7.dylib")) and\
                os.path.exists(os.path.join(install_path["root"], install_path["lib"], "libbrotlidec.1.0.7.dylib")) and\
                os.path.exists(os.path.join(install_path["root"], install_path["lib"], "libbrotlienc.1.0.7.dylib")) and\
                os.path.exists(os.path.join(install_path["root"], install_path["lib"], "libbrotlicommon.1.dylib")) and\
                os.path.exists(os.path.join(install_path["root"], install_path["lib"], "libbrotlidec.1.dylib")) and\
                os.path.exists(os.path.join(install_path["root"], install_path["lib"], "libbrotlienc.1.dylib")) and\
                os.path.exists(os.path.join(install_path["root"], install_path["lib"], "libbrotlicommon.dylib")) and\
                os.path.exists(os.path.join(install_path["root"], install_path["lib"], "libbrotlidec.dylib")) and\
                os.path.exists(os.path.join(
                    install_path["root"], install_path["lib"], "libbrotlienc.dylib"))
        return False

    def build(deps_path, install_path, num_threads=multiprocessing.cpu_count(), target_system=None):
        global _BROTLI_SRC_URL

        info_msg("Building Brotli 1.0.7...")

        # Check built.
        if build_brotli._check_built(install_path, target_system=target_system):
            success_msg("Building Brotli 1.0.7.")
            return True

        os.makedirs(deps_path, exist_ok=True)
        os.makedirs(install_path["root"], exist_ok=True)

        original_dir = os.getcwd()
        os.chdir(deps_path)
        # Download brotli.
        if not os.path.exists(os.path.join("brotli-1.0.7.src", "EXTRACTED")):
            info_msg("Getting Brotli 1.0.7 sources...")
            try:
                os.remove("brotli-1.0.7.tar.gz")
            except FileNotFoundError:
                pass
            try:
                shutil.rmtree("brotli-1.0.7.src")
            except FileNotFoundError:
                pass
            try:
                shutil.rmtree("brotli-1.0.7")
            except FileNotFoundError:
                pass
            wget.download(_BROTLI_SRC_URL)
            # Print new line after "wget.download", as the latter method will not do so.
            print("")
            with tarfile.open("brotli-1.0.7.tar.gz") as fd:
                fd.extractall(".")
            os.rename("brotli-1.0.7", "brotli-1.0.7.src")
            try:
                os.remove("brotli-1.0.7.tar.gz")
            except FileNotFoundError:
                pass
            with open(os.path.join("brotli-1.0.7.src", "EXTRACTED"), "w") as fd:
                fd.write("EXTRACTED CHECKER")
            success_msg("Brotli 1.0.7 sources have been downloaded.")
        else:
            success_msg("Brotli 1.0.7 sources are already downloaded.")

        # Build brotli.
        os.makedirs("brotli-1.0.7.build", exist_ok=True)
        os.makedirs("brotli-1.0.7.install", exist_ok=True)
        os.chdir("brotli-1.0.7.build")

        new_environ = os.environ.copy()
        new_environ = create_new_environ_with_custom_cc_cxx(
            new_environ, target_system=target_system)
        cmake_system_name = "Windows" if (target_system == "windows" or platform.system() == "Windows" and not target_system) else (
            "Darwin" if (target_system == "macos" or platform.system() == "Darwin" and not target_system) else "Linux")
        cmake_cmd = [
            "cmake",
            os.path.join(deps_path, "brotli-1.0.7.src"),
            "-DCMAKE_INSTALL_PREFIX={}".format(
                os.path.join(deps_path, "brotli-1.0.7.install")),
            "-DCMAKE_BUILD_TYPE=Release",
            "-DCMAKE_SYSTEM_NAME={}".format(cmake_system_name),
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
            fail_msg("Building Brotli 1.0.7.")
            os.chdir(original_dir)
            return False

        p = subprocess.Popen(
            [("mingw32-make" if (platform.system() == "Windows") else "make"), "-j{}".format(num_threads), "install"], env=new_environ)
        ret = p.wait()
        if ret:
            fail_msg("Building Brotli 1.0.7.")
            os.chdir(original_dir)
            return False

        os.makedirs(os.path.join(
            install_path["root"], install_path["lib"]), exist_ok=True)
        os.makedirs(os.path.join(
            install_path["root"], install_path["bin"]), exist_ok=True)
        if target_system == "windows" or platform.system() == "Windows" and not target_system:
            shutil.copy2(
                os.path.join(deps_path, "brotli-1.0.7.install",
                             "bin", "libbrotlicommon.dll"),
                os.path.join(
                    install_path["root"], install_path["bin"], "libbrotlicommon.dll")
            )
            shutil.copy2(
                os.path.join(deps_path, "brotli-1.0.7.install",
                             "bin", "libbrotlidec.dll"),
                os.path.join(install_path["root"],
                             install_path["bin"], "libbrotlidec.dll")
            )
            shutil.copy2(
                os.path.join(deps_path, "brotli-1.0.7.install",
                             "bin", "libbrotlienc.dll"),
                os.path.join(install_path["root"],
                             install_path["bin"], "libbrotlienc.dll")
            )
            shutil.copy2(
                os.path.join(deps_path, "brotli-1.0.7.install",
                             "lib", "libbrotlicommon.dll.a"),
                os.path.join(
                    install_path["root"], install_path["lib"], "libbrotlicommon.dll.a")
            )
            shutil.copy2(
                os.path.join(deps_path, "brotli-1.0.7.install",
                             "lib", "libbrotlidec.dll.a"),
                os.path.join(
                    install_path["root"], install_path["lib"], "libbrotlidec.dll.a")
            )
            shutil.copy2(
                os.path.join(deps_path, "brotli-1.0.7.install",
                             "lib", "libbrotlienc.dll.a"),
                os.path.join(
                    install_path["root"], install_path["lib"], "libbrotlienc.dll.a")
            )
        elif target_system == "linux" or platform.system() == "Linux" and not target_system:
            unix_copy2(
                os.path.join(deps_path, "brotli-1.0.7.install",
                             "lib", "libbrotlicommon.so.1.0.7"),
                os.path.join(
                    install_path["root"], install_path["lib"], "libbrotlicommon.so.1.0.7")
            )
            unix_copy2(
                os.path.join(deps_path, "brotli-1.0.7.install",
                             "lib", "libbrotlidec.so.1.0.7"),
                os.path.join(
                    install_path["root"], install_path["lib"], "libbrotlidec.so.1.0.7")
            )
            unix_copy2(
                os.path.join(deps_path, "brotli-1.0.7.install",
                             "lib", "libbrotlienc.so.1.0.7"),
                os.path.join(
                    install_path["root"], install_path["lib"], "libbrotlienc.so.1.0.7")
            )
            unix_copy2(
                os.path.join(deps_path, "brotli-1.0.7.install",
                             "lib", "libbrotlicommon.so.1"),
                os.path.join(
                    install_path["root"], install_path["lib"], "libbrotlicommon.so.1")
            )
            unix_copy2(
                os.path.join(deps_path, "brotli-1.0.7.install",
                             "lib", "libbrotlidec.so.1"),
                os.path.join(
                    install_path["root"], install_path["lib"], "libbrotlidec.so.1")
            )
            unix_copy2(
                os.path.join(deps_path, "brotli-1.0.7.install",
                             "lib", "libbrotlienc.so.1"),
                os.path.join(
                    install_path["root"], install_path["lib"], "libbrotlienc.so.1")
            )
            unix_copy2(
                os.path.join(deps_path, "brotli-1.0.7.install",
                             "lib", "libbrotlicommon.so"),
                os.path.join(
                    install_path["root"], install_path["lib"], "libbrotlicommon.so")
            )
            unix_copy2(
                os.path.join(deps_path, "brotli-1.0.7.install",
                             "lib", "libbrotlidec.so"),
                os.path.join(install_path["root"],
                             install_path["lib"], "libbrotlidec.so")
            )
            unix_copy2(
                os.path.join(deps_path, "brotli-1.0.7.install",
                             "lib", "libbrotlienc.so"),
                os.path.join(install_path["root"],
                             install_path["lib"], "libbrotlienc.so")
            )
        elif target_system == "macos" or platform.system() == "Darwin" and not target_system:
            unix_copy2(
                os.path.join(deps_path, "brotli-1.0.7.install",
                             "lib", "libbrotlicommon.1.0.7.dylib"),
                os.path.join(install_path["root"], install_path["lib"],
                             "libbrotlicommon.1.0.7.dylib")
            )
            unix_copy2(
                os.path.join(deps_path, "brotli-1.0.7.install",
                             "lib", "libbrotlidec.1.0.7.dylib"),
                os.path.join(
                    install_path["root"], install_path["lib"], "libbrotlidec.1.0.7.dylib")
            )
            unix_copy2(
                os.path.join(deps_path, "brotli-1.0.7.install",
                             "lib", "libbrotlienc.1.0.7.dylib"),
                os.path.join(
                    install_path["root"], install_path["lib"], "libbrotlienc.1.0.7.dylib")
            )
            unix_copy2(
                os.path.join(deps_path, "brotli-1.0.7.install",
                             "lib", "libbrotlicommon.1.dylib"),
                os.path.join(
                    install_path["root"], install_path["lib"], "libbrotlicommon.1.dylib")
            )
            unix_copy2(
                os.path.join(deps_path, "brotli-1.0.7.install",
                             "lib", "libbrotlidec.1.dylib"),
                os.path.join(
                    install_path["root"], install_path["lib"], "libbrotlidec.1.dylib")
            )
            unix_copy2(
                os.path.join(deps_path, "brotli-1.0.7.install",
                             "lib", "libbrotlienc.1.dylib"),
                os.path.join(
                    install_path["root"], install_path["lib"], "libbrotlienc.1.dylib")
            )
            unix_copy2(
                os.path.join(deps_path, "brotli-1.0.7.install",
                             "lib", "libbrotlicommon.dylib"),
                os.path.join(
                    install_path["root"], install_path["lib"], "libbrotlicommon.dylib")
            )
            unix_copy2(
                os.path.join(deps_path, "brotli-1.0.7.install",
                             "lib", "libbrotlidec.dylib"),
                os.path.join(
                    install_path["root"], install_path["lib"], "libbrotlidec.dylib")
            )
            unix_copy2(
                os.path.join(deps_path, "brotli-1.0.7.install",
                             "lib", "libbrotlienc.dylib"),
                os.path.join(
                    install_path["root"], install_path["lib"], "libbrotlienc.dylib")
            )

        success_msg("Building Brotli 1.0.7.")
        os.chdir(original_dir)
        return True

    def get_dep_environ(deps_path, target_system=None):
        new_environ = os.environ.copy()
        host_sep = ";" if platform.system() == "Windows" else ":"
        new_environ["LIBRARY_PATH"] = os.path.join(deps_path, "brotli-1.0.7.install", "lib") +\
            ("" if ("LIBRARY_PATH" not in new_environ)
             else (host_sep + new_environ["LIBRARY_PATH"]))
        new_environ["PKG_CONFIG_PATH"] = os.path.join(deps_path, "brotli-1.0.7.install", "lib", "pkgconfig") +\
            ("" if ("PKG_CONFIG_PATH" not in new_environ)
             else (host_sep + new_environ["PKG_CONFIG_PATH"]))
        new_environ["CPATH"] = os.path.join(deps_path, "brotli-1.0.7.install", "include") +\
            ("" if ("CPATH" not in new_environ)
             else (host_sep + new_environ["CPATH"]))
        return new_environ
