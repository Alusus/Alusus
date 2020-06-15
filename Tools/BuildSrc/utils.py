import subprocess
import os
import shutil
import platform
import sys
SOURCE_LOCATION = os.path.abspath(__file__)
sys.path.insert(0, os.path.dirname(os.path.dirname(SOURCE_LOCATION)))
import whichcraft  # noqa
if platform.system() == "Windows":  # noqa
    from mslex import quote, split  # noqa
else:  # noqa
    from shlex import quote, split  # noqa
from parse_path_envvar import parse_path_envvar  # noqa


def shell_split(cmd):
    return split(cmd)


def shell_join(cmd_parts):
    return " ".join(
        [quote(cmd_part) for cmd_part in cmd_parts]
    )


def which(cmd):
    cmd = shell_split(cmd)
    cmd = [whichcraft.which(cmd[0])] + cmd[1:]
    cmd = shell_join(cmd)
    return cmd


def get_host_cxx_triple(new_environ=os.environ):
    return subprocess.check_output(
        shell_split(which(new_environ["CXX"] if "CXX" in new_environ else "clang++")) + ["-dumpmachine"], env=new_environ).decode().strip()


def get_host_cxx_arch(new_environ=os.environ):
    return get_host_cxx_triple().split("-")[0]


def create_tmp_dir():
    os.makedirs(os.environ["ALUSUS_TMP_DIR"], exist_ok=True)


def delete_tmp_dir():
    shutil.rmtree(os.environ["ALUSUS_TMP_DIR"], ignore_errors=True)


def unix_copy(src, dst):
    if os.path.islink(src):
        if os.path.lexists(dst):
            os.unlink(dst)
        linkto = os.readlink(src)
        os.symlink(linkto, dst)
    else:
        shutil.copy(src, dst)


def unix_copy2(src, dst):
    if os.path.islink(src):
        if os.path.lexists(dst):
            os.unlink(dst)
        linkto = os.readlink(src)
        os.symlink(linkto, dst)
    else:
        shutil.copy2(src, dst)


def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)


def get_lib_path(libname, target_system=None, environ=os.environ):
    cc = which(environ["CC"] if "CC" in environ else "cc")
    cc_output_lines = subprocess.check_output(
        [cc, "-print-search-dirs"], env=environ).decode().split(os.linesep)
    libraries_line_prefix = "libraries: ="
    libraries_line = [line for line in cc_output_lines if len(
        line) > 0 and line.startswith(libraries_line_prefix)][0]
    libraries_line = libraries_line[len(libraries_line_prefix):]
    libraries_paths = parse_path_envvar(libraries_line)
    lib_exts = [".so"] if (target_system == "linux" or platform.system() == "Linux" and not target_system) else (
        [".dylib"] if (target_system == "macos" or platform.system(
        ) == "Darwin" and not target_system) else [".lib", ".dll.a"]
    )
    to_return = None
    for libraries_path in libraries_paths:
        for lib_ext in lib_exts:
            lib_path = os.path.abspath(os.path.join(
                libraries_path, "lib{libname}{lib_ext}".format(libname=libname, lib_ext=lib_ext)))
            if os.path.exists(lib_path):
                to_return = {
                    "lib_path": lib_path
                }

                # Non Windows OSes only need the library file's path.
                if not (target_system == "windows" or platform.system() == "Windows" and not target_system):
                    return to_return

                # Add DLLs pointed by the the import library and found in the "PATH" environment variable.
                path_env_var = environ["PATH"] if "PATH" in environ else ""
                paths = parse_path_envvar(path_env_var)
                ar = which(environ["AR"] if "AR" in environ else "ar")
                dlls = set(
                    [item.strip() for item in subprocess.check_output(
                        [ar, "-t", lib_path], env=environ).decode().split(os.linesep) if len(item.strip()) > 0]
                )
                dll_paths = set()
                for dll in dlls:
                    if len(dll) > 0 and dll.endswith(".dll"):
                        for path in paths:
                            dll_path = os.path.abspath(os.path.join(path, dll))
                            if os.path.exists(dll_path):
                                dll_paths.add(dll_path)
                                break
                if len(dll_paths) > 0:
                    to_return["dll_paths"] = list(dll_paths)
                else:
                    # No matching DLL found.
                    to_return = None
                return to_return
    return to_return


# Parse "ALUSUS_TO_COPY_LIBS". These are ","-seperated libraries to be copied into the final installation directory,
# and you can escape the "," and "\" characters by prefixing them with "\"".
def get_to_copy_libs(environ=os.environ):
    to_return = set()
    if "ALUSUS_TO_COPY_LIBS" not in environ:
        return to_return
    curr_str = ""
    i = 0
    while i < len(environ["ALUSUS_TO_COPY_LIBS"]):
        char = environ["ALUSUS_TO_COPY_LIBS"][i]
        if char == "\\" and i < (len(environ["ALUSUS_TO_COPY_LIBS"]) - 1) and environ["ALUSUS_TO_COPY_LIBS"][i + 1] == ",":
            char = ","
            i += 1
            curr_str += char
        elif char == "\\" and i < (len(environ["ALUSUS_TO_COPY_LIBS"]) - 1) and environ["ALUSUS_TO_COPY_LIBS"][i + 1] == "\\":
            char = "\\"
            i += 1
            curr_str += char
        elif char == ",":
            if curr_str:
                to_return.add(curr_str)
                curr_str = ""
        else:
            curr_str += char
        i += 1
    if curr_str:
        to_return.add(curr_str)
        curr_str = ""
    return to_return
