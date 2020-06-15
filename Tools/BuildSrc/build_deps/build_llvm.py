import os
import sys
import shutil
import wget
import tarfile
import platform
import subprocess
import multiprocessing
import distutils.dir_util
SOURCE_LOCATION = os.path.abspath(__file__)
sys.path.insert(0, os.path.dirname(os.path.dirname(SOURCE_LOCATION)))
from build_deps import template_build, build_zlib  # noqa
from msg import info_msg, success_msg, fail_msg  # noqa
from create_shared_lib import create_dll, create_so, create_dylib  # noqa
from utils import get_host_cxx_arch, get_host_cxx_triple, unix_copy2, shell_split, shell_join, which  # noqa
from custom_cc_cxx import create_new_environ_with_custom_cc_cxx  # noqa


_LLVM_SRC_URL = "https://github.com/llvm/llvm-project/releases/download/llvmorg-10.0.0/llvm-10.0.0.src.tar.xz"


class build_llvm(template_build.template_build):
    def _check_built(install_path, target_system=None):
        # Check for unfinished RPATHs updating process exists.
        if os.path.exists(os.path.join(os.environ["ALUSUS_TMP_DIR"], "UPDATING_RPATHS")):
            return False
        if target_system == "windows" or platform.system() == "Windows" and not target_system:
            return os.path.exists(os.path.join(install_path["root"], install_path["lib"], "libLLVM-10.dll.a")) and\
                os.path.exists(os.path.join(install_path["root"], install_path["lib"], "libLLVM-10.0.0.dll.a")) and\
                os.path.exists(os.path.join(install_path["root"], install_path["lib"], "libLLVM.dll.a")) and\
                os.path.exists(os.path.join(
                    install_path["root"], install_path["bin"], "libLLVM-10.dll"))
        elif target_system == "linux" or platform.system() == "Linux" and not target_system:
            return os.path.exists(os.path.join(install_path["root"], install_path["lib"], "libLLVM-10.so")) and\
                os.path.exists(os.path.join(install_path["root"], install_path["lib"], "libLLVM-10.0.0.so")) and\
                os.path.exists(os.path.join(
                    install_path["root"], install_path["lib"], "libLLVM.so"))
        elif target_system == "macos" or platform.system() == "Darwin" and not target_system:
            return os.path.exists(os.path.join(install_path["root"], install_path["lib"], "libLLVM-10.dylib")) and\
                os.path.exists(os.path.join(install_path["root"], install_path["lib"], "libLLVM-10.0.0.dylib")) and\
                os.path.exists(os.path.join(
                    install_path["root"], install_path["lib"], "libLLVM.dylib"))
        return False

    def build(deps_path, install_path, num_threads=multiprocessing.cpu_count(), target_system=None):
        global _LLVM_SRC_URL

        info_msg("Building LLVM 10.0.0...")

        info_msg("Building LLVM 10.0.0 dependencies...")
        ret = build_zlib.build_zlib.build(deps_path, install_path,
                                          num_threads=num_threads, target_system=target_system)
        if not ret:
            fail_msg("Building LLVM 10.0.0.")
            return False
        success_msg("Building LLVM 10.0.0 dependencies.")

        if build_llvm._check_built(install_path, target_system=target_system):
            success_msg("Building LLVM 10.0.0.")
            return True

        os.makedirs(deps_path, exist_ok=True)
        os.makedirs(install_path["root"], exist_ok=True)

        original_dir = os.getcwd()
        os.chdir(deps_path)
        # Download LLVM.
        if not os.path.exists(os.path.join("llvm-10.0.0.src", "EXTRACTED")):
            info_msg("Getting LLVM 10.0.0 sources...")
            try:
                os.remove("llvm-10.0.0.src.tar.xz")
            except FileNotFoundError:
                pass
            try:
                shutil.rmtree("llvm-10.0.0.src")
            except FileNotFoundError:
                pass
            wget.download(_LLVM_SRC_URL)
            # Print new line after "wget.download", as the latter method will not do so.
            print("")
            with tarfile.open("llvm-10.0.0.src.tar.xz") as fd:
                fd.extractall(".")
            try:
                os.remove("llvm-10.0.0.src.tar.xz")
            except FileNotFoundError:
                pass

            # Apply the patch(es).
            patches_path = os.path.join(os.path.dirname(
                os.path.dirname(os.path.dirname(SOURCE_LOCATION))), "Patches")
            distutils.dir_util.copy_tree(
                os.path.join(
                    patches_path, "llvm-10.0.0.src"),
                "llvm-10.0.0.src"
            )

            with open(os.path.join("llvm-10.0.0.src", "EXTRACTED"), "w") as fd:
                fd.write("EXTRACTED CHECKER")
            success_msg("LLVM 10.0.0 sources have been downloaded.")
        else:
            success_msg("LLVM 10.0.0 sources are already downloaded.")

        # Build LLVM.
        zlib_environ = build_zlib.build_zlib.get_dep_environ(
            deps_path, target_system=target_system)
        host_sep = ":" if platform.system() != "Windows" else ";"
        new_environ = None
        if target_system != None:
            # Cross compile.

            # Build LLVM for the host first.
            info_msg("Building LLVM 10.0.0 for the host...")
            os.makedirs("llvm-10.0.0.host.build", exist_ok=True)
            os.makedirs("llvm-10.0.0.host.install", exist_ok=True)
            os.chdir("llvm-10.0.0.host.build")
            new_environ = os.environ.copy()
            new_environ["CC"] = which(
                new_environ["ALUSUS_HOST_CC"] if "ALUSUS_HOST_CC" in new_environ else "clang")
            new_environ["CXX"] = which(
                new_environ["ALUSUS_HOST_CXX"] if "ALUSUS_HOST_CXX" in new_environ else "clang++")
            new_environ["AR"] = which(
                new_environ["ALUSUS_HOST_AR"] if "ALUSUS_HOST_AR" in new_environ else "ar")
            new_environ["AS"] = which(
                new_environ["ALUSUS_HOST_AS"] if "ALUSUS_HOST_AS" in new_environ else "as")
            new_environ["RANLIB"] = which(
                new_environ["ALUSUS_HOST_RANLIB"] if "ALUSUS_HOST_RANLIB" in new_environ else "ranlib")
            new_environ["NM"] = which(
                new_environ["ALUSUS_HOST_NM"] if "ALUSUS_HOST_NM" in new_environ else "nm")
            new_environ["STRIP"] = which(
                new_environ["ALUSUS_HOST_STRIP"] if "ALUSUS_HOST_STRIP" in new_environ else "strip")
            new_environ["LD"] = which(
                new_environ["ALUSUS_HOST_LD"] if "ALUSUS_HOST_LD" in new_environ else "ld")
            if platform.system() == "Windows":
                new_environ["RC"] = which(
                    new_environ["ALUSUS_HOST_RC"] if "ALUSUS_HOST_RC" in new_environ else "windres")
            if platform.system() == "Darwin":
                new_environ["INSTALL_NAME_TOOL"] = which(
                    new_environ["ALUSUS_HOST_INSTALL_NAME_TOOL"] if "ALUSUS_HOST_INSTALL_NAME_TOOL" in new_environ else "install_name_tool")
                new_environ["OTOOL"] = which(
                    new_environ["ALUSUS_HOST_OTOOL"] if "ALUSUS_HOST_OTOOL" in new_environ else "otool")
            new_environ["LIBRARY_PATH"] = zlib_environ["ALUSUS_HOST_LIBRARY_PATH"] +\
                ("" if ("ALUSUS_HOST_LIBRARY_PATH" not in new_environ) else
                 (host_sep + new_environ["ALUSUS_HOST_LIBRARY_PATH"]))
            new_environ["CPATH"] = zlib_environ["ALUSUS_HOST_CPATH"] +\
                ("" if ("ALUSUS_HOST_CPATH" not in new_environ) else
                 (host_sep + new_environ["ALUSUS_HOST_CPATH"]))
            if "ALUSUS_HOST_C_INCLUDE_PATH" in new_environ:
                new_environ["C_INCLUDE_PATH"] = new_environ["ALUSUS_HOST_C_INCLUDE_PATH"]
            else:
                try:
                    del new_environ["C_INCLUDE_PATH"]
                except KeyError:
                    pass
            if "ALUSUS_HOST_CPLUS_INCLUDE_PATH" in new_environ:
                new_environ["CPLUS_INCLUDE_PATH"] = new_environ["ALUSUS_HOST_CPLUS_INCLUDE_PATH"]
            else:
                try:
                    del new_environ["CPLUS_INCLUDE_PATH"]
                except KeyError:
                    pass
            new_environ["PKG_CONFIG_PATH"] = zlib_environ["ALUSUS_HOST_PKG_CONFIG_PATH"] +\
                ("" if ("PKG_CONFIG_PATH" not in new_environ) else
                 (host_sep + new_environ["PKG_CONFIG_PATH"]))

            host_target_system = "windows" if (platform.system() == "Windows") else (
                "linux" if (platform.system() == "Linux") else "macos")
            new_environ = create_new_environ_with_custom_cc_cxx(
                new_environ, target_system=host_target_system)
            cmake_cmd = ["cmake",
                         os.path.join(deps_path, "llvm-10.0.0.src"),
                         "-DCMAKE_BUILD_TYPE=Release",
                         "-DLLVM_ENABLE_LIBXAR=OFF",
                         "-DPYTHON_EXECUTABLE={}".format(sys.executable),
                         "-DCMAKE_INSTALL_PREFIX={}".format(
                             os.path.join(deps_path, "llvm-10.0.0.host.install")),
                         "-DCMAKE_RANLIB={}".format(which(
                             new_environ["RANLIB"] if "RANLIB" in new_environ else "ranlib")),
                         "-DCMAKE_AR={}".format(which(
                             new_environ["AR"] if "AR" in new_environ else "ar")),
                         "-DCMAKE_LINKER={}".format(which(
                             new_environ["LD"] if "LD" in new_environ else "ld")),
                         "-DCMAKE_STRIP={}".format(which(new_environ["STRIP"] if "STRIP" in new_environ else "strip"))]
            if platform.system() == "Darwin":
                cmake_cmd += [
                    "-DCMAKE_INSTALL_NAME_TOOL={}".format(which(
                        new_environ["INSTALL_NAME_TOOL"] if "INSTALL_NAME_TOOL" in new_environ else "install_name_tool")),
                    "-DCMAKE_OTOOL={}".format(which(
                        new_environ["OTOOL"] if "OTOOL" in new_environ else "otool"))
                ]
            if platform.system() == "Windows":
                cmake_cmd += [
                    "-DCMAKE_RC_COMPILER={}".format(which(
                        new_environ["RC"] if "RC" in new_environ else "windres"))
                ]
            if platform.system() == "Windows":
                cmake_cmd += [
                    "-G", "MinGW Makefiles",
                    "-DCMAKE_SH=CMAKE_SH-NOTFOUND"
                ]
            p = subprocess.Popen(cmake_cmd, env=new_environ)
            ret = p.wait()
            if ret:
                fail_msg("Building LLVM 10.0.0.")
                os.chdir(original_dir)
                return False
            p = subprocess.Popen(
                [("mingw32-make" if (platform.system() == "Windows") else "make"), "-j{}".format(num_threads), "install"], env=new_environ)
            ret = p.wait()
            if ret:
                fail_msg("Building LLVM 10.0.0.")
                os.chdir(original_dir)
                return False

            # Build LLVM for the target now.
            info_msg("Building LLVM 10.0.0 for the target...")
            os.chdir("..")
            os.makedirs("llvm-10.0.0.target.build", exist_ok=True)
            os.makedirs("llvm-10.0.0.target.install", exist_ok=True)
            os.chdir("llvm-10.0.0.target.build")
            new_environ = os.environ.copy()
            new_environ["LIBRARY_PATH"] = zlib_environ["LIBRARY_PATH"] +\
                ("" if ("LIBRARY_PATH" not in new_environ) else
                 (host_sep + new_environ["LIBRARY_PATH"]))
            new_environ["CPATH"] = zlib_environ["CPATH"] +\
                ("" if ("CPATH" not in new_environ) else
                 (host_sep + new_environ["CPATH"]))
            new_environ["PKG_CONFIG_PATH"] = zlib_environ["PKG_CONFIG_PATH"] +\
                ("" if ("PKG_CONFIG_PATH" not in new_environ) else
                 (host_sep + new_environ["PKG_CONFIG_PATH"]))
            new_environ["PATH"] = zlib_environ["ALUSUS_HOST_PATH"] +\
                ("" if ("PATH" not in new_environ) else
                    (host_sep + new_environ["PATH"]))
            new_environ["LD_LIBRARY_PATH"] = zlib_environ["ALUSUS_HOST_LD_LIBRARY_PATH"] +\
                ("" if ("LD_LIBRARY_PATH" not in new_environ) else
                    (host_sep + new_environ["LD_LIBRARY_PATH"]))
            new_environ = create_new_environ_with_custom_cc_cxx(
                new_environ, target_system=target_system)
            host_cxx_arch = get_host_cxx_arch(new_environ=new_environ)
            llvm_target_arch = None
            if host_cxx_arch == "aarch64":
                llvm_target_arch = "AArch64"
            elif host_cxx_arch.startswith("arm"):
                llvm_target_arch = "ARM"
            elif host_cxx_arch == "i686" or host_cxx_arch == "i386":
                llvm_target_arch = "x86"
            elif host_cxx_arch == "x86_64":
                llvm_target_arch = "x86_64"
            else:
                fail_msg("Unsupported target architecture!")
                fail_msg("Building LLVM 10.0.0.")
                os.chdir(original_dir)
                return False
            cxx_host_triple = get_host_cxx_triple(new_environ=new_environ)
            cmake_system_name = "Windows" if (target_system == "windows") else (
                "Darwin" if (target_system == "macos") else "Linux")
            cmake_cmd += [
                "-DCMAKE_CROSSCOMPILING=TRUE",
                "-DLLVM_ENABLE_LIBXAR=ON",
                "-DLLVM_ENABLE_LIBXML2=OFF",
                "-DLLVM_ENABLE_LIBEDIT=OFF",
                "-DLLVM_ENABLE_TERMINFO=OFF",
                "-DCMAKE_SYSTEM_NAME={}".format(cmake_system_name),
                "-DLLVM_TABLEGEN={}".format(os.path.join(deps_path,
                                                         "llvm-10.0.0.host.install", "bin", "llvm-tblgen")),
                "-DLLVM_TARGET_ARCH={}".format(llvm_target_arch),
                "-DCMAKE_SYSTEM_PROCESSOR={}".format(host_cxx_arch),
                "-DLLVM_BUILD_TOOLS=OFF",
                "-DLLVM_INCLUDE_TOOLS=OFF",
                "-DLLVM_BUILD_EXAMPLES=OFF",
                "-DLLVM_INCLUDE_EXAMPLES=OFF",
                "-DLLVM_BUILD_TESTS=OFF",
                "-DLLVM_INCLUDE_TESTS=OFF",
                "-DLLVM_BUILD_BENCHMARKS=OFF",
                "-DLLVM_INCLUDE_BENCHMARKS=OFF",
                "-DLLVM_INCLUDE_UTILS=OFF",
                "-DLLVM_HOST_TRIPLE={}".format(cxx_host_triple),
                "-DLLVM_DEFAULT_TARGET_TRIPLE={}".format(cxx_host_triple)
                # "-DLLVM_ENABLE_PIC={}".format("FALSE" if (
                #     (llvm_target_arch == "AArch64") or (llvm_target_arch == "ARM")) else "TRUE")  # Disabling PIC on ARM based backends, based on hack "1." from https://llvm.org/docs/HowToCrossCompileLLVM.html#hacks.
            ]
            cmake_cmd[[cmake_cmd.index(item) for item in cmake_cmd if item.startswith("-DCMAKE_INSTALL_PREFIX")][0]] =\
                "-DCMAKE_INSTALL_PREFIX={}".format(
                    os.path.join(deps_path, "llvm-10.0.0.target.install"))
            cmake_cmd[[cmake_cmd.index(item) for item in cmake_cmd if item.startswith("-DCMAKE_RANLIB")][0]] =\
                "-DCMAKE_RANLIB={}".format(
                    which(new_environ["RANLIB"] if "RANLIB" in new_environ else "ranlib"))
            cmake_cmd[[cmake_cmd.index(item) for item in cmake_cmd if item.startswith("-DCMAKE_AR")][0]] =\
                "-DCMAKE_AR={}".format(
                    which(new_environ["AR"] if "AR" in new_environ else "ar"))
            cmake_cmd[[cmake_cmd.index(item) for item in cmake_cmd if item.startswith("-DCMAKE_LINKER")][0]] =\
                "-DCMAKE_LINKER={}".format(
                    which(new_environ["LD"] if "LD" in new_environ else "ld"))
            cmake_cmd[[cmake_cmd.index(item) for item in cmake_cmd if item.startswith("-DCMAKE_STRIP")][0]] =\
                "-DCMAKE_STRIP={}".format(
                    which(new_environ["STRIP"] if "STRIP" in new_environ else "strip"))
            if target_system == "macos":
                if platform.system() == "Darwin":
                    cmake_cmd[[cmake_cmd.index(item) for item in cmake_cmd if item.startswith("-DCMAKE_INSTALL_NAME_TOOL")][0]] =\
                        "-DCMAKE_INSTALL_NAME_TOOL={}".format(
                            which(new_environ["INSTALL_NAME_TOOL"] if "INSTALL_NAME_TOOL" in new_environ else "install_name_tool"))
                    cmake_cmd[[cmake_cmd.index(item) for item in cmake_cmd if item.startswith("-DCMAKE_OTOOL")][0]] =\
                        "-DCMAKE_OTOOL={}".format(
                            which(new_environ["OTOOL"] if "OTOOL" in new_environ else "otool"))
                else:
                    cmake_cmd += [
                        "-DCMAKE_INSTALL_NAME_TOOL={}".format(
                            which(new_environ["INSTALL_NAME_TOOL"] if "INSTALL_NAME_TOOL" in new_environ else "install_name_tool")),
                        "-DCMAKE_OTOOL={}".format(
                            which(new_environ["OTOOL"] if "OTOOL" in new_environ else "otool"))
                    ]
            if target_system == "windows":
                if platform.system() == "Windows":
                    cmake_cmd[[cmake_cmd.index(item) for item in cmake_cmd if item.startswith("-DCMAKE_RC_COMPILER")][0]] =\
                        "-DCMAKE_RC_COMPILER={}".format(
                            which(new_environ["RC"] if "RC" in new_environ else "windres"))
                else:
                    cmake_cmd += [
                        "-DCMAKE_RC_COMPILER={}".format(
                            which(new_environ["RC"] if "RC" in new_environ else "windres"))
                    ]
            p = subprocess.Popen(cmake_cmd, env=new_environ)
            ret = p.wait()
            if ret:
                fail_msg("Building LLVM 10.0.0.")
                os.chdir(original_dir)
                return False
            # There is a bug in cross-compiling LLVM, which makes it fail the first time at around "15%".
            # Restarting the CMake part once more should fix it.
            p = subprocess.Popen(cmake_cmd, env=new_environ)
            ret = p.wait()
            if ret:
                fail_msg("Building LLVM 10.0.0.")
                os.chdir(original_dir)
                return False
            p = subprocess.Popen(
                [("mingw32-make" if (platform.system() == "Windows") else "make"), "-j{}".format(num_threads), "install"], env=new_environ)
            ret = p.wait()
            if ret:
                fail_msg("Building LLVM 10.0.0.")
                os.chdir(original_dir)
                return False

        else:
            # Build LLVM for the host only.
            info_msg("Building LLVM 10.0.0 for the host...")
            os.makedirs("llvm-10.0.0.build", exist_ok=True)
            os.makedirs("llvm-10.0.0.install", exist_ok=True)
            os.chdir("llvm-10.0.0.build")
            new_environ = os.environ.copy()
            new_environ["LIBRARY_PATH"] = zlib_environ["LIBRARY_PATH"] +\
                ("" if ("LIBRARY_PATH" not in new_environ) else
                 (host_sep + new_environ["LIBRARY_PATH"]))
            new_environ["CPATH"] = zlib_environ["CPATH"] +\
                ("" if ("CPATH" not in new_environ) else
                 (host_sep + new_environ["CPATH"]))
            new_environ["PKG_CONFIG_PATH"] = zlib_environ["PKG_CONFIG_PATH"] +\
                ("" if ("PKG_CONFIG_PATH" not in new_environ) else
                 (host_sep + new_environ["PKG_CONFIG_PATH"]))
            new_environ = create_new_environ_with_custom_cc_cxx(
                new_environ, target_system=target_system)
            host_cxx_arch = get_host_cxx_arch(new_environ=new_environ)
            llvm_target_arch = None
            if host_cxx_arch == "aarch64":
                llvm_target_arch = "AArch64"
            elif host_cxx_arch.startswith("arm"):
                llvm_target_arch = "ARM"
            elif host_cxx_arch == "i686" or host_cxx_arch == "i386":
                llvm_target_arch = "x86"
            elif host_cxx_arch == "x86_64":
                llvm_target_arch = "x86_64"
            cmake_cmd = [
                "cmake",
                os.path.join(deps_path, "llvm-10.0.0.src"),
                "-DCMAKE_BUILD_TYPE=Release",
                "-DLLVM_ENABLE_LIBXAR=ON",
                "-DLLVM_ENABLE_LIBXML2=OFF",
                "-DLLVM_ENABLE_LIBEDIT=OFF",
                "-DLLVM_ENABLE_TERMINFO=OFF",
                "-DPYTHON_EXECUTABLE={}".format(sys.executable),
                "-DLLVM_TARGET_ARCH={}".format(llvm_target_arch),
                "-DCMAKE_SYSTEM_PROCESSOR={}".format(host_cxx_arch),
                "-DCMAKE_INSTALL_PREFIX={}".format(
                    os.path.join(deps_path, "llvm-10.0.0.install")),
                "-DLLVM_BUILD_TOOLS=OFF",
                "-DLLVM_INCLUDE_TOOLS=OFF",
                "-DLLVM_BUILD_EXAMPLES=OFF",
                "-DLLVM_INCLUDE_EXAMPLES=OFF",
                "-DLLVM_BUILD_TESTS=OFF",
                "-DLLVM_INCLUDE_TESTS=OFF",
                "-DLLVM_BUILD_BENCHMARKS=OFF",
                "-DLLVM_INCLUDE_BENCHMARKS=OFF",
                "-DLLVM_INCLUDE_UTILS=ON",
                "-DCMAKE_RANLIB={}".format(
                    which(new_environ["RANLIB"] if "RANLIB" in new_environ else "ranlib")),
                "-DCMAKE_AR={}".format(
                    which(new_environ["AR"] if "AR" in new_environ else "ar")),
                "-DCMAKE_LINKER={}".format(
                    which(new_environ["LD"] if "LD" in new_environ else "ld")),
                "-DCMAKE_STRIP={}".format(
                    which(new_environ["STRIP"] if "STRIP" in new_environ else "strip"))
                # # Disabling PIC on ARM based backends, based on hack "1." from https://llvm.org/docs/HowToCrossCompileLLVM.html#hacks.
                # "-DLLVM_ENABLE_PIC={}".format("FALSE" if (
                #     (llvm_target_arch == "AArch64") or (llvm_target_arch == "ARM")) else "TRUE")
            ]
            if platform.system() == "Darwin":
                cmake_cmd += [
                    "-DCMAKE_INSTALL_NAME_TOOL={}".format(which(
                        new_environ["INSTALL_NAME_TOOL"] if "INSTALL_NAME_TOOL" in new_environ else "install_name_tool")),
                    "-DCMAKE_OTOOL={}".format(which(
                        new_environ["OTOOL"] if "OTOOL" in new_environ else "otool"))
                ]
            if platform.system() == "Windows":
                cmake_cmd += [
                    "-DCMAKE_RC_COMPILER={}".format(which(
                        new_environ["RC"] if "RC" in new_environ else "windres")),
                    "-G", "MinGW Makefiles",
                    "-DCMAKE_SH=CMAKE_SH-NOTFOUND"
                ]
            p = subprocess.Popen(cmake_cmd, env=new_environ)
            ret = p.wait()
            if ret:
                fail_msg("Building LLVM 10.0.0.")
                os.chdir(original_dir)
                return False
            p = subprocess.Popen(
                [("mingw32-make" if (platform.system() == "Windows") else "make"), "-j{}".format(num_threads), "install"], env=new_environ)
            ret = p.wait()
            if ret:
                fail_msg("Building LLVM 10.0.0.")
                os.chdir(original_dir)
                return False

        # Manually create the DLL and the import libraries for libLLVM-10 for the Windows target or platform.
        if target_system == "windows" or platform.system() == "Windows" and not target_system:
            os.chdir(os.path.join("..", ("llvm-10.0.0.target.install" if (
                target_system == "windows") else "llvm-10.0.0.install"), "lib"))
            try:
                os.remove("libLLVM-10.dll.a")
            except OSError:
                pass
            try:
                os.remove("libLLVM-10.0.0.dll.a")
            except OSError:
                pass
            try:
                os.remove("libLLVM.dll.a")
            except OSError:
                pass
            try:
                os.remove("libLLVM-10.dll")
            except OSError:
                pass
            static_libs = [static_lib for static_lib in os.listdir(".") if not (
                static_lib.endswith(".dll.a") or static_lib.endswith(".lib"))]
            create_dll_out = create_dll(arg_file=static_libs, arg_output_dir=".", arg_output_name="LLVM-10",
                                        arg_link_lib=[
                                            "ole32", "uuid", "z", "pthread"],
                                        new_environ=new_environ)
            if create_dll_out[0]:
                fail_msg("Building LLVM 10.0.0.")
                os.chdir(original_dir)
                return False
            os.makedirs(os.path.join("..", "bin"), exist_ok=True)
            shutil.move(
                "libLLVM-10.dll",
                os.path.join("..", "bin", "libLLVM-10.dll")
            )
            shutil.copy2("libLLVM-10.dll.a", "libLLVM-10.0.0.dll.a")
            shutil.copy2("libLLVM-10.dll.a", "libLLVM.dll.a")
        # Manually create the SO for libLLVM-10 for the Linux target or platform.
        elif target_system == "linux" or platform.system() == "Linux" and not target_system:
            os.chdir(os.path.join("..", ("llvm-10.0.0.target.install" if (
                target_system == "macos") else "llvm-10.0.0.install"), "lib"))
            try:
                os.remove("libLLVM-10.so")
            except OSError:
                pass
            try:
                os.remove("libLLVM-10.0.0.so")
            except OSError:
                pass
            try:
                os.remove("libLLVM-10.0.0.so")
            except OSError:
                pass
            create_so_out = create_so(arg_file=os.listdir("."), arg_output_dir=".", arg_output_name="LLVM-10",
                                      arg_link_lib=["z", "pthread"],
                                      new_environ=new_environ)
            if create_so_out[0]:
                fail_msg("Building LLVM 10.0.0.")
                os.chdir(original_dir)
                return False
            try:
                os.remove("libLLVM-10.0.0.so")
            except FileNotFoundError:
                pass
            try:
                os.remove("libLLVM.so")
            except FileNotFoundError:
                pass
            os.symlink("libLLVM-10.so", "libLLVM-10.0.0.so")
            os.symlink("libLLVM-10.so", "libLLVM.so")
        # Manually create the DYLIB for libLLVM-10 for the macOS target or platform.
        elif target_system == "macos" or platform.system() == "Darwin" and not target_system:
            os.chdir(os.path.join("..", ("llvm-10.0.0.target.install" if (
                target_system == "macos") else "llvm-10.0.0.install"), "lib"))
            try:
                os.remove("libLLVM-10.dylib")
            except OSError:
                pass
            try:
                os.remove("libLLVM-10.0.0.dylib")
            except OSError:
                pass
            try:
                os.remove("libLLVM-10.0.0.dylib")
            except OSError:
                pass
            create_dylib_out = create_dylib(arg_file=os.listdir("."), arg_output_dir=".", arg_output_name="LLVM-10",
                                            arg_link_lib=["z", "pthread"],
                                            new_environ=new_environ)
            if create_dylib_out[0]:
                fail_msg("Building LLVM 10.0.0.")
                os.chdir(original_dir)
                return False
            try:
                os.remove("libLLVM-10.0.0.dylib")
            except FileNotFoundError:
                pass
            try:
                os.remove("libLLVM.dylib")
            except FileNotFoundError:
                pass
            os.symlink("libLLVM-10.dylib", "libLLVM-10.0.0.dylib")
            os.symlink("libLLVM-10.dylib", "libLLVM.dylib")

        os.makedirs(os.path.join(
            install_path["root"], install_path["lib"]), exist_ok=True)
        os.makedirs(os.path.join(
            install_path["root"], install_path["bin"]), exist_ok=True)
        if target_system == "windows" or platform.system() == "Windows" and not target_system:
            shutil.copy2(
                os.path.join(deps_path, ("llvm-10.0.0.target.install" if (target_system == "windows") else "llvm-10.0.0.install"),
                             "lib", "libLLVM-10.dll.a"),
                os.path.join(install_path["root"],
                             install_path["lib"], "libLLVM-10.dll.a")
            )
            shutil.copy2(
                os.path.join(deps_path, ("llvm-10.0.0.target.install" if (target_system == "windows") else "llvm-10.0.0.install"),
                             "lib", "libLLVM-10.0.0.dll.a"),
                os.path.join(
                    install_path["root"], install_path["lib"], "libLLVM-10.0.0.dll.a")
            )
            shutil.copy2(
                os.path.join(deps_path, ("llvm-10.0.0.target.install" if (target_system == "windows") else "llvm-10.0.0.install"),
                             "lib", "libLLVM.dll.a"),
                os.path.join(install_path["root"],
                             install_path["lib"], "libLLVM.dll.a")
            )
            shutil.copy2(
                os.path.join(deps_path, ("llvm-10.0.0.target.install" if (target_system == "windows") else "llvm-10.0.0.install"),
                             "bin", "libLLVM-10.dll"),
                os.path.join(install_path["root"],
                             install_path["bin"], "libLLVM-10.dll")
            )
        elif target_system == "linux" or platform.system() == "Linux" and not target_system:
            unix_copy2(
                os.path.join(deps_path, ("llvm-10.0.0.target.install" if (target_system == "linux") else "llvm-10.0.0.install"),
                             "lib", "libLLVM-10.so"),
                os.path.join(install_path["root"],
                             install_path["lib"], "libLLVM-10.so")
            )
            unix_copy2(
                os.path.join(deps_path, ("llvm-10.0.0.target.install" if (target_system == "linux") else "llvm-10.0.0.install"),
                             "lib", "libLLVM-10.0.0.so"),
                os.path.join(
                    install_path["root"], install_path["lib"], "libLLVM-10.0.0.so")
            )
            unix_copy2(
                os.path.join(deps_path, ("llvm-10.0.0.target.install" if (target_system == "linux") else "llvm-10.0.0.install"),
                             "lib", "libLLVM.so"),
                os.path.join(install_path["root"],
                             install_path["lib"], "libLLVM.so")
            )
        elif target_system == "macos" or platform.system() == "Darwin" and not target_system:
            unix_copy2(
                os.path.join(deps_path, ("llvm-10.0.0.target.install" if (target_system == "macos") else "llvm-10.0.0.install"),
                             "lib", "libLLVM-10.dylib"),
                os.path.join(install_path["root"],
                             install_path["lib"], "libLLVM-10.dylib")
            )
            unix_copy2(
                os.path.join(deps_path, ("llvm-10.0.0.target.install" if (target_system == "macos") else "llvm-10.0.0.install"),
                             "lib", "libLLVM-10.0.0.dylib"),
                os.path.join(
                    install_path["root"], install_path["lib"], "libLLVM-10.0.0.dylib")
            )
            unix_copy2(
                os.path.join(deps_path, ("llvm-10.0.0.target.install" if (target_system == "macos") else "llvm-10.0.0.install"),
                             "lib", "libLLVM.dylib"),
                os.path.join(install_path["root"],
                             install_path["lib"], "libLLVM.dylib")
            )

        success_msg("Building LLVM 10.0.0.")
        os.chdir(original_dir)
        return True

    def get_dep_environ(deps_path, target_system=None):
        host_sep = ":" if platform.system() != "Windows" else ";"
        new_environ = os.environ.copy()
        if target_system != None:
            new_environ["ALUSUS_HOST_PATH"] = os.path.join(deps_path, "llvm-10.0.0.host.install", "bin") + host_sep +\
                build_zlib.build_zlib.get_dep_environ(deps_path, target_system=target_system)["ALUSUS_HOST_PATH"] +\
                ("" if ("ALUSUS_HOST_PATH" not in new_environ) else
                    (host_sep + new_environ["ALUSUS_HOST_PATH"]))
            new_environ["ALUSUS_HOST_LD_LIBRARY_PATH"] = os.path.join(deps_path, "llvm-10.0.0.host.install", "lib") + host_sep +\
                build_zlib.build_zlib.get_dep_environ(deps_path, target_system=target_system)["ALUSUS_HOST_LD_LIBRARY_PATH"] +\
                ("" if ("ALUSUS_HOST_LD_LIBRARY_PATH" not in new_environ) else
                    (host_sep + new_environ["ALUSUS_HOST_LD_LIBRARY_PATH"]))
            new_environ["ALUSUS_HOST_LIBRARY_PATH"] = os.path.join(deps_path, "llvm-10.0.0.host.install", "lib") +\
                ("" if ("ALUSUS_HOST_LIBRARY_PATH" not in new_environ) else
                 (host_sep + new_environ["ALUSUS_HOST_LIBRARY_PATH"]))
            new_environ["ALUSUS_HOST_CPATH"] = os.path.join(deps_path, "llvm-10.0.0.host.install", "include") +\
                ("" if ("ALUSUS_HOST_CPATH" not in new_environ) else
                 (host_sep + new_environ["ALUSUS_HOST_CPATH"]))
            new_environ["CPATH"] = os.path.join(deps_path, "llvm-10.0.0.target.install", "include") +\
                ("" if ("CPATH" not in new_environ) else
                 (host_sep + new_environ["CPATH"]))
            new_environ["LIBRARY_PATH"] = os.path.join(deps_path, "llvm-10.0.0.target.install", "lib") +\
                ("" if ("LIBRARY_PATH" not in new_environ) else
                 (host_sep + new_environ["LIBRARY_PATH"]))
        else:
            new_environ["CPATH"] = os.path.join(deps_path, "llvm-10.0.0.install", "include") +\
                ("" if ("CPATH" not in new_environ) else
                 (host_sep + new_environ["CPATH"]))
            new_environ["LIBRARY_PATH"] = os.path.join(deps_path, "llvm-10.0.0.install", "lib") +\
                ("" if ("LIBRARY_PATH" not in new_environ) else
                 (host_sep + new_environ["LIBRARY_PATH"]))
        return new_environ
