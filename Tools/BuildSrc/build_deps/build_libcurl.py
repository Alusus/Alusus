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
from build_deps import template_build, build_zlib, build_openssl, build_nghttp2, build_libssh2, build_brotli  # noqa
from msg import info_msg, success_msg, fail_msg  # noqa
from utils import shell_join, shell_split, get_host_cxx_arch, get_host_cxx_triple, unix_copy2  # noqa
from custom_cc_cxx import create_new_environ_with_custom_cc_cxx  # noqa
from create_shared_lib import create_dll, create_dylib  # noqa


_CURL_SRC_URL = "https://curl.haxx.se/download/curl-7.70.0.tar.gz"


class build_libcurl(template_build.template_build):
    def _check_built(install_path, target_system=None):
        # Check for unfinished RPATHs updating process exists.
        if os.path.exists(os.path.join(os.environ["ALUSUS_TMP_DIR"], "UPDATING_RPATHS")):
            return False
        if target_system == "windows" or platform.system() == "Windows" and not target_system:
            return os.path.exists(os.path.join(install_path["root"], install_path["bin"], "libcurl.dll")) and\
                os.path.exists(os.path.join(
                    install_path["root"], install_path["lib"], "libcurl.dll.a"))
        elif target_system == "linux" or platform.system() == "Linux" and not target_system:
            return os.path.exists(os.path.join(install_path["root"], install_path["lib"], "libcurl.so.4.6.0")) and\
                os.path.exists(os.path.join(install_path["root"], install_path["lib"], "libcurl.so.4")) and\
                os.path.exists(os.path.join(
                    install_path["root"], install_path["lib"], "libcurl.so"))
        elif target_system == "macos" or platform.system() == "Darwin" and not target_system:
            return os.path.exists(os.path.join(install_path["root"], install_path["lib"], "libcurl.4.dylib")) and\
                os.path.exists(os.path.join(
                    install_path["root"], install_path["lib"], "libcurl.dylib"))
        return False

    def build(deps_path, install_path, num_threads=multiprocessing.cpu_count(), target_system=None):
        global _CURL_SRC_URL

        info_msg("Building libcurl 7.70.0...")

        # Build dependecies.
        info_msg("Building libcurl 7.70.0 dependencies...")
        ret = build_zlib.build_zlib.build(deps_path, install_path,
                                          num_threads=num_threads, target_system=target_system)
        if not ret:
            fail_msg("Building libcurl 7.70.0.")
            return False
        ret = build_openssl.build_openssl.build(deps_path, install_path,
                                                num_threads=num_threads, target_system=target_system)
        if not ret:
            fail_msg("Building libcurl 7.70.0.")
            return False
        ret = build_nghttp2.build_nghttp2.build(deps_path, install_path,
                                                num_threads=num_threads, target_system=target_system)
        if not ret:
            fail_msg("Building libcurl 7.70.0.")
            return False
        ret = build_libssh2.build_libssh2.build(deps_path, install_path,
                                                num_threads=num_threads, target_system=target_system)
        if not ret:
            fail_msg("Building libcurl 7.70.0.")
            return False
        ret = build_brotli.build_brotli.build(deps_path, install_path,
                                              num_threads=num_threads, target_system=target_system)
        if not ret:
            fail_msg("Building libcurl 7.70.0.")
            return False
        success_msg("Building libcurl 7.70.0 dependencies.")

        # Check built.
        if build_libcurl._check_built(install_path, target_system=target_system):
            success_msg("Building libcurl 7.70.0.")
            return True

        os.makedirs(deps_path, exist_ok=True)
        os.makedirs(install_path["root"], exist_ok=True)

        original_dir = os.getcwd()
        os.chdir(deps_path)
        # Download libcurl.
        if not os.path.exists(os.path.join("curl-7.70.0.src", "EXTRACTED")):
            info_msg("Getting libcurl 7.70.0 sources...")
            try:
                os.remove("curl-7.70.0.tar.gz")
            except FileNotFoundError:
                pass
            try:
                shutil.rmtree("curl-7.70.0.src")
            except FileNotFoundError:
                pass
            try:
                shutil.rmtree("curl-7.70.0")
            except FileNotFoundError:
                pass
            wget.download(_CURL_SRC_URL)
            # Print new line after "wget.download", as the latter method will not do so.
            print("")
            with tarfile.open("curl-7.70.0.tar.gz") as fd:
                fd.extractall(".")
            os.rename("curl-7.70.0", "curl-7.70.0.src")
            try:
                os.remove("curl-7.70.0.tar.gz")
            except FileNotFoundError:
                pass

            # Apply the patch(es).
            patches_path = os.path.join(os.path.dirname(
                os.path.dirname(os.path.dirname(SOURCE_LOCATION))), "Patches")
            distutils.dir_util.copy_tree(
                os.path.join(
                    patches_path, "curl-7.70.0.src"),
                "curl-7.70.0.src"
            )

            with open(os.path.join("curl-7.70.0.src", "EXTRACTED"), "w") as fd:
                fd.write("EXTRACTED CHECKER")
            success_msg("libcurl 7.70.0 sources have been downloaded.")
        else:
            success_msg("libcurl 7.70.0 sources are already downloaded.")

        # Build libcurl.
        os.makedirs("curl-7.70.0.build", exist_ok=True)
        os.makedirs("curl-7.70.0.install", exist_ok=True)
        os.chdir("curl-7.70.0.build")

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
        nghttp2_environ = build_nghttp2.build_nghttp2.get_dep_environ(
            deps_path, target_system=target_system)
        new_environ["PKG_CONFIG_PATH"] = nghttp2_environ["PKG_CONFIG_PATH"] +\
            ("" if ("PKG_CONFIG_PATH" not in new_environ)
                else (host_sep + new_environ["PKG_CONFIG_PATH"]))
        new_environ["CPATH"] = nghttp2_environ["CPATH"] +\
            ("" if ("CPATH" not in new_environ)
                else (host_sep + new_environ["CPATH"]))
        new_environ["LIBRARY_PATH"] = nghttp2_environ["LIBRARY_PATH"] +\
            ("" if ("LIBRARY_PATH" not in new_environ)
                else (host_sep + new_environ["LIBRARY_PATH"]))
        libssh2_environ = build_libssh2.build_libssh2.get_dep_environ(
            deps_path, target_system=target_system)
        new_environ["PKG_CONFIG_PATH"] = libssh2_environ["PKG_CONFIG_PATH"] +\
            ("" if ("PKG_CONFIG_PATH" not in new_environ)
                else (host_sep + new_environ["PKG_CONFIG_PATH"]))
        new_environ["CPATH"] = libssh2_environ["CPATH"] +\
            ("" if ("CPATH" not in new_environ)
                else (host_sep + new_environ["CPATH"]))
        new_environ["LIBRARY_PATH"] = libssh2_environ["LIBRARY_PATH"] +\
            ("" if ("LIBRARY_PATH" not in new_environ)
                else (host_sep + new_environ["LIBRARY_PATH"]))
        brotli_environ = build_brotli.build_brotli.get_dep_environ(
            deps_path, target_system=target_system)
        new_environ["PKG_CONFIG_PATH"] = brotli_environ["PKG_CONFIG_PATH"] +\
            ("" if ("PKG_CONFIG_PATH" not in new_environ)
                else (host_sep + new_environ["PKG_CONFIG_PATH"]))
        new_environ["CPATH"] = brotli_environ["CPATH"] +\
            ("" if ("CPATH" not in new_environ)
                else (host_sep + new_environ["CPATH"]))
        new_environ["LIBRARY_PATH"] = brotli_environ["LIBRARY_PATH"] +\
            ("" if ("LIBRARY_PATH" not in new_environ)
                else (host_sep + new_environ["LIBRARY_PATH"]))
        new_environ = create_new_environ_with_custom_cc_cxx(
            new_environ, target_system=target_system)

        autoconf_cmd = shell_join(
            [
                os.path.join(
                    deps_path, "curl-7.70.0.src", "configure"),
                "--prefix={}".format(os.path.join(deps_path,
                                                  "curl-7.70.0.install")),
                "--with-libssh2",
                "--disable-ldap"
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
        # Will build the shared library later. There's an issue with shared library building for Windows and macOS targets.
        if (target_system == "windows" or platform.system() == "Windows" and not target_system) or\
            (target_system == "macos" or platform.system() == "Darwin" and not target_system):
            autoconf_cmd = shell_join(shell_split(
                autoconf_cmd) + ["--disable-shared"])
        bash_cmd = ["bash", "-c", autoconf_cmd]
        p = subprocess.Popen(bash_cmd, env=new_environ)
        ret = p.wait()
        if ret:
            fail_msg("Building libcurl 7.70.0.")
            os.chdir(original_dir)
            return False
        p = subprocess.Popen(
            ["make", "-j{}".format(num_threads)], env=new_environ)
        ret = p.wait()
        if ret:
            fail_msg("Building libcurl 7.70.0.")
            os.chdir(original_dir)
            return False
        p = subprocess.Popen(["make", "install"], env=new_environ)
        ret = p.wait()
        if ret:
            fail_msg("Building libcurl 7.70.0.")
            os.chdir(original_dir)
            return False

        # We will create libcurl shared library for Windows and macOS targets now.
        if target_system == "windows" or platform.system() == "Windows" and not target_system:
            try:
                os.remove(os.path.join(
                    deps_path, "curl-7.70.0.install", "lib", "libcurl.dll"))
            except OSError:
                pass
            try:
                os.remove(os.path.join(
                    deps_path, "curl-7.70.0.install", "lib", "libcurl.dll.a"))
            except OSError:
                pass
            ret = create_dll(
                arg_dir=[os.path.join(
                    deps_path, "curl-7.70.0.install", "lib")],
                arg_output_dir=os.path.join(
                    deps_path, "curl-7.70.0.install", "lib"),
                arg_output_name="curl",
                arg_link_lib=["z", "crypto", "ssl",
                                "ws2_32", "pthread",
                                "nghttp2", "ssh2", "brotlidec"],
                new_environ=new_environ
            )
            if ret[0]:
                fail_msg(ret[1])
                fail_msg("Building libcurl 7.70.0.")
                os.chdir(original_dir)
                return False
            shutil.move(
                os.path.join(deps_path, "curl-7.70.0.install",
                             "lib", "libcurl.dll"),
                os.path.join(deps_path, "curl-7.70.0.install",
                             "bin", "libcurl.dll")
            )
        elif target_system == "macos" or platform.system() == "Darwin" and not target_system:
            try:
                os.remove(os.path.join(
                    deps_path, "curl-7.70.0.install", "lib", "libcurl.4.dylib"))
            except OSError:
                pass
            try:
                os.remove(os.path.join(
                    deps_path, "curl-7.70.0.install", "lib", "libcurl.dylib"))
            except OSError:
                pass
            ret = create_dylib(
                arg_dir=[os.path.join(
                    deps_path, "curl-7.70.0.install", "lib")],
                arg_output_dir=os.path.join(
                    deps_path, "curl-7.70.0.install", "lib"),
                arg_output_name="curl.4",
                arg_link_lib=["z", "crypto", "ssl", "pthread",
                                "nghttp2", "ssh2", "brotlidec",
                                "clang_rt.osx"],
                new_environ=new_environ
            )
            if ret[0]:
                fail_msg(ret[1])
                fail_msg("Building libcurl 7.70.0.")
                os.chdir(original_dir)
                return False
            os.symlink(
                os.path.join(deps_path, "curl-7.70.0.install",
                             "lib", "libcurl.4.dylib"),
                os.path.join(deps_path, "curl-7.70.0.install",
                             "lib", "libcurl.dylib")
            )

        os.makedirs(os.path.join(
            install_path["root"], install_path["lib"]), exist_ok=True)
        os.makedirs(os.path.join(
            install_path["root"], install_path["bin"]), exist_ok=True)
        if target_system == "windows" or platform.system() == "Windows" and not target_system:
            shutil.copy2(
                os.path.join(deps_path, "curl-7.70.0.install",
                             "bin", "libcurl.dll"),
                os.path.join(install_path["root"],
                             install_path["bin"], "libcurl.dll")
            )
            shutil.copy2(
                os.path.join(deps_path, "curl-7.70.0.install",
                             "lib", "libcurl.dll.a"),
                os.path.join(install_path["root"],
                             install_path["lib"], "libcurl.dll.a")
            )
        elif target_system == "linux" or platform.system() == "Linux" and not target_system:
            unix_copy2(
                os.path.join(deps_path, "curl-7.70.0.install",
                             "lib", "libcurl.so.4.6.0"),
                os.path.join(install_path["root"],
                             install_path["lib"], "libcurl.so.4.6.0")
            )
            unix_copy2(
                os.path.join(deps_path, "curl-7.70.0.install",
                             "lib", "libcurl.so.4"),
                os.path.join(install_path["root"],
                             install_path["lib"], "libcurl.so.4")
            )
            unix_copy2(
                os.path.join(deps_path, "curl-7.70.0.install",
                             "lib", "libcurl.so"),
                os.path.join(install_path["root"],
                             install_path["lib"], "libcurl.so")
            )
        elif target_system == "macos" or platform.system() == "Darwin" and not target_system:
            unix_copy2(
                os.path.join(deps_path, "curl-7.70.0.install",
                             "lib", "libcurl.4.dylib"),
                os.path.join(install_path["root"],
                             install_path["lib"], "libcurl.4.dylib")
            )
            unix_copy2(
                os.path.join(deps_path, "curl-7.70.0.install",
                             "lib", "libcurl.dylib"),
                os.path.join(install_path["root"],
                             install_path["lib"], "libcurl.dylib")
            )

        success_msg("Building libcurl 7.70.0.")
        os.chdir(original_dir)
        return True

    def get_dep_environ(deps_path, target_system=None):
        new_environ = os.environ.copy()
        host_sep = ";" if platform.system() == "Windows" else ":"
        new_environ["LIBRARY_PATH"] = os.path.join(deps_path, "curl-7.70.0.install", "lib") +\
            ("" if ("LIBRARY_PATH" not in new_environ)
             else (host_sep + new_environ["LIBRARY_PATH"]))
        new_environ["PKG_CONFIG_PATH"] = os.path.join(deps_path, "curl-7.70.0.install", "lib", "pkgconfig") +\
            ("" if ("PKG_CONFIG_PATH" not in new_environ)
             else (host_sep + new_environ["PKG_CONFIG_PATH"]))
        new_environ["CPATH"] = os.path.join(deps_path, "curl-7.70.0.install", "include") +\
            ("" if ("CPATH" not in new_environ)
             else (host_sep + new_environ["CPATH"]))
        return new_environ
