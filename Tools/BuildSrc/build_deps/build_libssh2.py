import os
import sys
import multiprocessing
import wget
import shutil
import subprocess
import tarfile
import platform
import distutils.dir_util
SOURCE_LOCATION = os.path.abspath(__file__)
sys.path.insert(0, os.path.dirname(os.path.dirname(SOURCE_LOCATION)))
from build_deps import template_build, build_openssl, build_zlib  # noqa
from msg import info_msg, success_msg, fail_msg  # noqa
from utils import shell_join, shell_split, get_host_cxx_arch, get_host_cxx_triple, unix_copy2, get_local_rpaths  # noqa
from create_shared_lib import create_dll  # noqa
from custom_cc_cxx import create_new_environ_with_custom_cc_cxx  # noqa


_LIBSSH2_SRC_URL = "https://www.libssh2.org/download/libssh2-1.9.0.tar.gz"


class build_libssh2(template_build.template_build):
    def _check_built(install_path, target_system=None):
        if target_system == "windows" or platform.system() == "Windows" and not target_system:
            return os.path.exists(os.path.join(install_path, "Bin", "libssh2.dll")) and\
                os.path.exists(os.path.join(
                    install_path, "Lib", "libssh2.dll.a"))
        elif target_system == "linux" or platform.system() == "Linux" and not target_system:
            return os.path.exists(os.path.join(install_path, "Lib", "libssh2.so.1.0.1")) and\
                os.path.exists(os.path.join(install_path, "Lib", "libssh2.so.1")) and\
                os.path.exists(os.path.join(install_path, "Lib", "libssh2.so"))
        elif target_system == "macos" or platform.system() == "Darwin" and not target_system:
            return os.path.exists(os.path.join(install_path, "Lib", "libssh2.1.dylib")) and\
                os.path.exists(os.path.join(
                    install_path, "Lib", "libssh2.dylib"))
        return False

    def build(deps_path, install_path, num_threads=multiprocessing.cpu_count(), target_system=None):
        global _LIBSSH2_SRC_URL

        info_msg("Building libssh2 1.9.0...")

        # Build dependecies.
        info_msg("Building libssh2 1.9.0 dependencies...")
        ret = build_zlib.build_zlib.build(deps_path, install_path,
                                          num_threads=num_threads, target_system=target_system)
        if not ret:
            fail_msg("Building libcurl 7.70.0.")
            return False
        ret = build_openssl.build_openssl.build(deps_path, install_path,
                                                num_threads=num_threads, target_system=target_system)
        if not ret:
            fail_msg("Building libssh2 1.9.0.")
            return False
        success_msg("Building libssh2 1.9.0 dependencies.")

        # Check built.
        if build_libssh2._check_built(install_path, target_system=target_system):
            success_msg("Building libssh2 1.9.0.")
            return True

        os.makedirs(deps_path, exist_ok=True)
        os.makedirs(install_path, exist_ok=True)

        original_dir = os.getcwd()
        os.chdir(deps_path)
        # Download libssh2.
        if not os.path.exists(os.path.join("libssh2-1.9.0.src", "EXTRACTED")):
            info_msg("Getting libssh2 1.9.0 sources...")
            try:
                os.remove("libssh2-1.9.0.tar.gz")
            except FileNotFoundError:
                pass
            try:
                shutil.rmtree("libssh2-1.9.0.src")
            except FileNotFoundError:
                pass
            try:
                shutil.rmtree("libssh2-1.9.0")
            except FileNotFoundError:
                pass
            wget.download(_LIBSSH2_SRC_URL)
            # Print new line after "wget.download", as the latter method will not do so.
            print("")
            with tarfile.open("libssh2-1.9.0.tar.gz") as fd:
                fd.extractall(".")
            os.rename("libssh2-1.9.0", "libssh2-1.9.0.src")
            try:
                os.remove("libssh2-1.9.0.tar.gz")
            except FileNotFoundError:
                pass

            # Apply the patch(es).
            patches_path = os.path.join(os.path.dirname(
                os.path.dirname(os.path.dirname(SOURCE_LOCATION))), "Patches")
            distutils.dir_util.copy_tree(
                os.path.join(
                    patches_path, "libssh2-1.9.0.src"),
                "libssh2-1.9.0.src"
            )

            with open(os.path.join("libssh2-1.9.0.src", "EXTRACTED"), "w") as fd:
                fd.write("EXTRACTED CHECKER")
            success_msg("libssh2 1.9.0 sources have been downloaded.")
        else:
            success_msg("libssh2 1.9.0 sources are already downloaded.")

        # Build libssh2.
        os.makedirs("libssh2-1.9.0.build", exist_ok=True)
        os.makedirs("libssh2-1.9.0.install", exist_ok=True)
        os.chdir("libssh2-1.9.0.build")

        host_sep = ";" if platform.system() == "Windows" else ":"
        new_environ = os.environ.copy()
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
            new_environ, target_system=target_system, rpaths=get_local_rpaths(target_system=target_system))

        autoconf_cmd = shell_join(
            [
                os.path.join(
                    deps_path, "libssh2-1.9.0.src", "configure"),
                "--prefix={}".format(os.path.join(deps_path,
                                                  "libssh2-1.9.0.install")),
            ]
        )
        if target_system != None:
            autoconf_cmd = shell_join(shell_split(autoconf_cmd) + [
                "--host={arch}-{system}".format(
                    arch=get_host_cxx_arch(new_environ=new_environ),
                    system=("w64-mingw32" if target_system ==
                            "windows" else "-".join(get_host_cxx_triple(new_environ=new_environ).split("-")[1:]))
                )
            ])
        bash_cmd = ["bash", "-c", autoconf_cmd]
        p = subprocess.Popen(bash_cmd, env=new_environ)
        ret = p.wait()
        if ret:
            fail_msg("Building libssh2 1.9.0.")
            os.chdir(original_dir)
            return False
        p = subprocess.Popen(
            ["make", "-j{}".format(num_threads)], env=new_environ)
        ret = p.wait()
        if ret:
            fail_msg("Building libssh2 1.9.0.")
            os.chdir(original_dir)
            return False
        p = subprocess.Popen(["make", "install"], env=new_environ)
        ret = p.wait()
        if ret:
            fail_msg("Building libssh2 1.9.0.")
            os.chdir(original_dir)
            return False

        # We will create libssh2 shared library for Windows now (for some reason, the build script
        # fails to create shared library for Windows).
        if target_system == "windows" or platform.system() == "Windows" and not target_system:
            shutil.move(
                os.path.join(deps_path, "libssh2-1.9.0.install",
                             "lib", "libssh2.a"),
                os.path.join(deps_path, "libssh2-1.9.0.install",
                             "lib", "libssh2_static.a")
            )
            ret = create_dll(
                arg_dir=[os.path.join(
                    deps_path, "libssh2-1.9.0.install", "lib")],
                arg_output_dir=os.path.join(
                    deps_path, "libssh2-1.9.0.install", "lib"),
                arg_output_name="ssh2",
                arg_link_lib=["z", "crypto", "ssl", "ws2_32"],
                new_environ=new_environ
            )
            if ret[0]:
                fail_msg(ret[1])
                fail_msg("Building libssh2 1.9.0.")
                os.chdir(original_dir)
                return False
            os.makedirs(os.path.join(deps_path, "libssh2-1.9.0.install",
                                     "bin"), exist_ok=True)
            shutil.move(
                os.path.join(deps_path, "libssh2-1.9.0.install",
                             "lib", "libssh2.dll"),
                os.path.join(deps_path, "libssh2-1.9.0.install",
                             "bin", "libssh2.dll")
            )

            # This is a hacky way, but the intent is to force libcurl to link to the
            # dynamic libssh2 version, since libcurl is only looking at libssh2.a,
            # rather than libssh2.dll.a.
            shutil.copy2(
                os.path.join(deps_path, "libssh2-1.9.0.install",
                             "lib", "libssh2.dll.a"),
                os.path.join(deps_path, "libssh2-1.9.0.install",
                             "lib", "libssh2.a")
            )

        if target_system == "windows" or platform.system() == "Windows" and not target_system:
            shutil.copy2(
                os.path.join(deps_path, "libssh2-1.9.0.install",
                             "bin", "libssh2.dll"),
                os.path.join(install_path, "Bin", "libssh2.dll")
            )
            shutil.copy2(
                os.path.join(deps_path, "libssh2-1.9.0.install",
                             "lib", "libssh2.dll.a"),
                os.path.join(install_path, "Lib", "libssh2.dll.a")
            )
        elif target_system == "linux" or platform.system() == "Linux" and not target_system:
            unix_copy2(
                os.path.join(deps_path, "libssh2-1.9.0.install",
                             "lib", "libssh2.so.1.0.1"),
                os.path.join(install_path, "Lib", "libssh2.so.1.0.1")
            )
            unix_copy2(
                os.path.join(deps_path, "libssh2-1.9.0.install",
                             "lib", "libssh2.so.1"),
                os.path.join(install_path, "Lib", "libssh2.so.1")
            )
            unix_copy2(
                os.path.join(deps_path, "libssh2-1.9.0.install",
                             "lib", "libssh2.so"),
                os.path.join(install_path, "Lib", "libssh2.so")
            )
        elif target_system == "macos" or platform.system() == "Darwin" and not target_system:
            unix_copy2(
                os.path.join(deps_path, "libssh2-1.9.0.install",
                             "lib", "libssh2.1.dylib"),
                os.path.join(install_path, "Lib", "libssh2.1.dylib")
            )
            unix_copy2(
                os.path.join(deps_path, "libssh2-1.9.0.install",
                             "lib", "libssh2.dylib"),
                os.path.join(install_path, "Lib", "libssh2.dylib")
            )
        success_msg("Building libssh2 1.9.0.")
        os.chdir(original_dir)
        return True

    def get_dep_environ(deps_path, target_system=None):
        new_environ = os.environ.copy()
        host_sep = ";" if platform.system() == "Windows" else ":"
        new_environ["LIBRARY_PATH"] = os.path.join(deps_path, "libssh2-1.9.0.install", "lib") +\
            ("" if ("LIBRARY_PATH" not in new_environ)
             else (host_sep + new_environ["LIBRARY_PATH"]))
        new_environ["PKG_CONFIG_PATH"] = os.path.join(deps_path, "libssh2-1.9.0.install", "lib", "pkgconfig") +\
            ("" if ("PKG_CONFIG_PATH" not in new_environ)
             else (host_sep + new_environ["PKG_CONFIG_PATH"]))
        new_environ["CPATH"] = os.path.join(deps_path, "libssh2-1.9.0.install", "include") +\
            ("" if ("CPATH" not in new_environ)
             else (host_sep + new_environ["CPATH"]))
        return new_environ
