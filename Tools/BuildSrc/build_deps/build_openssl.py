import os
import sys
import multiprocessing
import wget
import shutil
import distutils.dir_util
import subprocess
import tarfile
import platform
SOURCE_LOCATION = os.path.abspath(__file__)
sys.path.insert(0, os.path.dirname(os.path.dirname(SOURCE_LOCATION)))
from build_deps import template_build  # noqa
from msg import info_msg, success_msg, fail_msg  # noqa
from utils import shell_join, get_host_cxx_arch, unix_copy2  # noqa
from custom_cc_cxx import create_new_environ_with_custom_cc_cxx  # noqa


_SSL_SRC_URL = "https://codeload.github.com/tatsuhiro-t/openssl/tar.gz/5332ff475df45a123e654aded3406737c6232335"


class build_openssl(template_build.template_build):
    def _check_built(install_path, target_system=None):
        # Check for unfinished RPATHs updating process exists.
        if os.path.exists(os.path.join(os.environ["ALUSUS_TMP_DIR"], "UPDATING_RPATHS")):
            return False
        if target_system == "windows" or platform.system() == "Windows" and not target_system:
            return os.path.exists(os.path.join(install_path["root"], install_path["bin"], "libcrypto-1_1.dll")) and\
                os.path.exists(os.path.join(install_path["root"], install_path["bin"], "libssl-1_1.dll")) and\
                os.path.exists(os.path.join(install_path["root"], install_path["lib"], "libcrypto.dll.a")) and\
                os.path.exists(os.path.join(
                    install_path["root"], install_path["lib"], "libssl.dll.a"))
        elif target_system == "linux" or platform.system() == "Linux" and not target_system:
            return os.path.exists(os.path.join(install_path["root"], install_path["lib"], "libcrypto.so.1.1")) and\
                os.path.exists(os.path.join(install_path["root"], install_path["lib"], "libssl.so.1.1")) and\
                os.path.exists(os.path.join(install_path["root"], install_path["lib"], "libcrypto.so")) and\
                os.path.exists(os.path.join(
                    install_path["root"], install_path["lib"], "libssl.so"))
        elif target_system == "macos" or platform.system() == "Darwin" and not target_system:
            return os.path.exists(os.path.join(install_path["root"], install_path["lib"], "libcrypto.1.1.dylib")) and\
                os.path.exists(os.path.join(install_path["root"], install_path["lib"], "libssl.1.1.dylib")) and\
                os.path.exists(os.path.join(install_path["root"], install_path["lib"], "libcrypto.dylib")) and\
                os.path.exists(os.path.join(
                    install_path["root"], install_path["lib"], "libssl.dylib"))
        return False

    def build(deps_path, install_path, num_threads=multiprocessing.cpu_count(), target_system=None):
        global _SSL_SRC_URL
        global SOURCE_LOCATION

        info_msg(
            "Building OpenSSL commit hash 5332ff475df45a123e654aded3406737c6232335...")

        # Check built.
        if build_openssl._check_built(install_path, target_system=target_system):
            success_msg(
                "Building OpenSSL commit hash 5332ff475df45a123e654aded3406737c6232335.")
            return True

        os.makedirs(deps_path, exist_ok=True)
        os.makedirs(install_path["root"], exist_ok=True)

        original_dir = os.getcwd()
        os.chdir(deps_path)
        # Download OpenSSL.
        if not os.path.exists(os.path.join("openssl-5332ff475df45a123e654aded3406737c6232335.src", "EXTRACTED")):
            info_msg(
                "Getting OpenSSL commit hash 5332ff475df45a123e654aded3406737c6232335 sources...")
            try:
                os.remove(
                    "openssl-5332ff475df45a123e654aded3406737c6232335.tar.gz")
            except FileNotFoundError:
                pass
            try:
                shutil.rmtree(
                    "openssl-5332ff475df45a123e654aded3406737c6232335.src")
            except FileNotFoundError:
                pass
            try:
                shutil.rmtree(
                    "openssl-5332ff475df45a123e654aded3406737c6232335")
            except FileNotFoundError:
                pass
            wget.download(_SSL_SRC_URL)
            # Print new line after "wget.download", as the latter method will not do so.
            print("")
            with tarfile.open("openssl-5332ff475df45a123e654aded3406737c6232335.tar.gz") as fd:
                fd.extractall(".")
            os.rename("openssl-5332ff475df45a123e654aded3406737c6232335",
                      "openssl-5332ff475df45a123e654aded3406737c6232335.src")
            try:
                os.remove(
                    "openssl-5332ff475df45a123e654aded3406737c6232335.tar.gz")
            except FileNotFoundError:
                pass

            # Apply the patch(es).
            patches_path = os.path.join(os.path.dirname(
                os.path.dirname(os.path.dirname(SOURCE_LOCATION))), "Patches")
            distutils.dir_util.copy_tree(
                os.path.join(
                    patches_path, "openssl-5332ff475df45a123e654aded3406737c6232335.src"),
                "openssl-5332ff475df45a123e654aded3406737c6232335.src"
            )

            with open(os.path.join("openssl-5332ff475df45a123e654aded3406737c6232335.src", "EXTRACTED"), "w") as fd:
                fd.write("EXTRACTED CHECKER")
            success_msg(
                "OpenSSL commit hash 5332ff475df45a123e654aded3406737c6232335 sources have been downloaded.")
        else:
            success_msg(
                "OpenSSL commit hash 5332ff475df45a123e654aded3406737c6232335 sources are already downloaded.")

        # Build OpenSSL.
        os.makedirs(
            "openssl-5332ff475df45a123e654aded3406737c6232335.build", exist_ok=True)
        os.makedirs(
            "openssl-5332ff475df45a123e654aded3406737c6232335.install", exist_ok=True)
        os.chdir("openssl-5332ff475df45a123e654aded3406737c6232335.build")

        openssl_target = None
        host_cxx_arch = get_host_cxx_arch()
        if target_system == "windows" or platform.system() == "Windows" and not target_system:
            openssl_target = "mingw-{}".format(host_cxx_arch)
        elif target_system == "linux" or platform.system() == "Linux" and not target_system:
            openssl_target = "linux-{}".format(host_cxx_arch)
        elif target_system == "macos" or platform.system() == "Darwin" and not target_system:
            openssl_target = "darwin-{}".format(host_cxx_arch)
        new_environ = os.environ.copy()
        if "CC" not in new_environ:
            new_environ["CC"] = "clang"
        if "CXX" not in new_environ:
            new_environ["CXX"] = "clang++"
        if "AR" not in new_environ:
            new_environ["AR"] = "ar"
        if "AS" not in new_environ:
            new_environ["AS"] = "as"
        if "RANLIB" not in new_environ:
            new_environ["RANLIB"] = "ranlib"
        if "NM" not in new_environ:
            new_environ["NM"] = "nm"
        if "STRIP" not in new_environ:
            new_environ["STRIP"] = "strip"
        if "LD" not in new_environ:
            new_environ["LD"] = "ld"
        if target_system == "windows" or platform.system() == "Windows" and not target_system:
            if "RC" not in new_environ:
                new_environ["RC"] = "windres"
        new_environ = create_new_environ_with_custom_cc_cxx(
            new_environ, target_system=target_system)
        p = subprocess.Popen(
            ["bash", "-c", shell_join(
                [
                    os.path.join(
                        deps_path, "openssl-5332ff475df45a123e654aded3406737c6232335.src", "Configure"),
                    "--prefix={}".format(os.path.join(deps_path,
                                                      "openssl-5332ff475df45a123e654aded3406737c6232335.install")),
                    openssl_target
                ]
            )
            ], env=new_environ)
        ret = p.wait()
        if ret:
            fail_msg(
                "Building OpenSSL commit hash 5332ff475df45a123e654aded3406737c6232335.")
            os.chdir(original_dir)
            return False
        p = subprocess.Popen(
            ["make", "-j{}".format(num_threads)], env=new_environ)
        ret = p.wait()
        if ret:
            fail_msg(
                "Building OpenSSL commit hash 5332ff475df45a123e654aded3406737c6232335.")
            os.chdir(original_dir)
            return False
        p = subprocess.Popen(["make", "install"], env=new_environ)
        ret = p.wait()
        if ret:
            fail_msg(
                "Building OpenSSL commit hash 5332ff475df45a123e654aded3406737c6232335.")
            os.chdir(original_dir)
            return False

        os.makedirs(os.path.join(
            install_path["root"], install_path["lib"]), exist_ok=True)
        os.makedirs(os.path.join(
            install_path["root"], install_path["bin"]), exist_ok=True)
        if target_system == "windows" or platform.system() == "Windows" and not target_system:
            shutil.copy(
                os.path.join(deps_path, "openssl-5332ff475df45a123e654aded3406737c6232335.install",
                             "bin", "libcrypto-1_1.dll"),
                os.path.join(
                    install_path["root"], install_path["bin"], "libcrypto-1_1.dll")
            )
            shutil.copy(
                os.path.join(deps_path, "openssl-5332ff475df45a123e654aded3406737c6232335.install",
                             "bin", "libssl-1_1.dll"),
                os.path.join(install_path["root"],
                             install_path["bin"], "libssl-1_1.dll")
            )
            shutil.copy(
                os.path.join(deps_path, "openssl-5332ff475df45a123e654aded3406737c6232335.install",
                             "lib", "libcrypto.dll.a"),
                os.path.join(install_path["root"],
                             install_path["lib"], "libcrypto.dll.a")
            )
            shutil.copy(
                os.path.join(deps_path, "openssl-5332ff475df45a123e654aded3406737c6232335.install",
                             "lib", "libssl.dll.a"),
                os.path.join(install_path["root"],
                             install_path["lib"], "libssl.dll.a")
            )
        elif target_system == "linux" or platform.system() == "Linux" and not target_system:
            unix_copy2(
                os.path.join(deps_path, "openssl-5332ff475df45a123e654aded3406737c6232335.install",
                             "lib", "libcrypto.so.1.1"),
                os.path.join(install_path["root"],
                             install_path["lib"], "libcrypto.so.1.1")
            )
            unix_copy2(
                os.path.join(deps_path, "openssl-5332ff475df45a123e654aded3406737c6232335.install",
                             "lib", "libssl.so.1.1"),
                os.path.join(install_path["root"],
                             install_path["lib"], "libssl.so.1.1")
            )
            unix_copy2(
                os.path.join(deps_path, "openssl-5332ff475df45a123e654aded3406737c6232335.install",
                             "lib", "libcrypto.so"),
                os.path.join(install_path["root"],
                             install_path["lib"], "libcrypto.so")
            )
            unix_copy2(
                os.path.join(deps_path, "openssl-5332ff475df45a123e654aded3406737c6232335.install",
                             "lib", "libssl.so"),
                os.path.join(install_path["root"],
                             install_path["lib"], "libssl.so")
            )
        elif target_system == "macos" or platform.system() == "Darwin" and not target_system:
            unix_copy2(
                os.path.join(deps_path, "openssl-5332ff475df45a123e654aded3406737c6232335.install",
                             "lib", "libcrypto.1.1.dylib"),
                os.path.join(
                    install_path["root"], install_path["lib"], "libcrypto.1.1.dylib")
            )
            unix_copy2(
                os.path.join(deps_path, "openssl-5332ff475df45a123e654aded3406737c6232335.install",
                             "lib", "libssl.1.1.dylib"),
                os.path.join(install_path["root"],
                             install_path["lib"], "libssl.1.1.dylib")
            )
            unix_copy2(
                os.path.join(deps_path, "openssl-5332ff475df45a123e654aded3406737c6232335.install",
                             "lib", "libcrypto.dylib"),
                os.path.join(install_path["root"],
                             install_path["lib"], "libcrypto.dylib")
            )
            unix_copy2(
                os.path.join(deps_path, "openssl-5332ff475df45a123e654aded3406737c6232335.install",
                             "lib", "libssl.dylib"),
                os.path.join(install_path["root"],
                             install_path["lib"], "libssl.dylib")
            )

        success_msg(
            "Building OpenSSL commit hash 5332ff475df45a123e654aded3406737c6232335.")
        os.chdir(original_dir)
        return True

    def get_dep_environ(deps_path, target_system=None):
        new_environ = os.environ.copy()
        host_sep = ";" if platform.system() == "Windows" else ":"
        new_environ["LIBRARY_PATH"] = os.path.join(deps_path, "openssl-5332ff475df45a123e654aded3406737c6232335.install", "lib") +\
            ("" if ("LIBRARY_PATH" not in new_environ)
             else (host_sep + new_environ["LIBRARY_PATH"]))
        new_environ["PKG_CONFIG_PATH"] = os.path.join(deps_path, "openssl-5332ff475df45a123e654aded3406737c6232335.install", "lib", "pkgconfig") +\
            ("" if ("PKG_CONFIG_PATH" not in new_environ)
             else (host_sep + new_environ["PKG_CONFIG_PATH"]))
        new_environ["CPATH"] = os.path.join(deps_path, "openssl-5332ff475df45a123e654aded3406737c6232335.install", "include") +\
            ("" if ("CPATH" not in new_environ)
             else (host_sep + new_environ["CPATH"]))
        new_environ["OPENSSL_ROOT_DIR"] = os.path.join(
            deps_path, "openssl-5332ff475df45a123e654aded3406737c6232335.install")
        return new_environ
