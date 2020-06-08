import os
import sys
import multiprocessing
import wget
import shutil
import tarfile
import platform
SOURCE_LOCATION = os.path.abspath(__file__)
sys.path.insert(0, os.path.dirname(os.path.dirname(SOURCE_LOCATION)))
from build_deps import template_build  # noqa
from msg import info_msg, success_msg, fail_msg  # noqa


_GHC_FILESYSTEM_URL = "https://github.com/gulrak/filesystem/archive/v1.3.2.tar.gz"


class build_ghc_filesystem(template_build.template_build):
    def build(deps_path, install_path, num_threads=multiprocessing.cpu_count(), target_system=None):
        global _GHC_FILESYSTEM_URL

        os.makedirs(deps_path, exist_ok=True)
        os.makedirs(install_path["root"], exist_ok=True)

        original_dir = os.getcwd()
        os.chdir(deps_path)

        # Download dlfcn-win32.
        if not os.path.exists(os.path.join("filesystem-1.3.2.src", "EXTRACTED")):
            info_msg("Getting GHC Filesystem 1.3.2 sources...")
            try:
                os.remove("filesystem-1.3.2.tar.gz")
            except FileNotFoundError:
                pass
            try:
                shutil.rmtree("filesystem-1.3.2.src")
            except FileNotFoundError:
                pass
            try:
                shutil.rmtree("filesystem-1.3.2")
            except FileNotFoundError:
                pass
            wget.download(_GHC_FILESYSTEM_URL)
            # Print new line after "wget.download", as the latter method will not do so.
            print("")
            with tarfile.open("filesystem-1.3.2.tar.gz") as fd:
                fd.extractall(".")
            os.rename("filesystem-1.3.2", "filesystem-1.3.2.src")
            try:
                os.remove("filesystem-1.3.2.tar.gz")
            except FileNotFoundError:
                pass
            with open(os.path.join("filesystem-1.3.2.src", "EXTRACTED"), "w") as fd:
                fd.write("EXTRACTED CHECKER")
            success_msg("GHC Filesystem 1.3.2 sources have been downloaded.")
        else:
            success_msg("GHC Filesystem 1.3.2 sources are already downloaded.")

        os.chdir(original_dir)
        return True

    def get_dep_environ(deps_path, target_system=None):
        host_sep = ":" if platform.system() != "Windows" else ";"
        new_environ = os.environ.copy()
        new_environ["CPATH"] = os.path.join(deps_path, "filesystem-1.3.2.src", "include") +\
            ("" if ("CPATH" not in new_environ) else
                (host_sep + new_environ["CPATH"]))
        return new_environ
