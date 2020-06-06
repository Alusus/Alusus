import subprocess
import os
import shutil
import platform
import sys
if platform.system() == "Windows":
    from mslex import quote, split
else:
    from shlex import quote, split


def get_host_cxx_triple(new_environ=os.environ):
    return subprocess.check_output(
        [new_environ["CXX"] if "CXX" in new_environ else "clang++", "-dumpmachine"], env=new_environ).decode().strip()


def get_host_cxx_arch(new_environ=os.environ):
    return get_host_cxx_triple().split("-")[0]


def shell_split(cmd):
    return split(cmd)


def shell_join(cmd_parts):
    return " ".join(
        [quote(cmd_part) for cmd_part in cmd_parts]
    )


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


def get_local_rpaths(target_system=None):
    rpaths = list()
    if not (target_system == "windows" or platform.system() == "Windows" and not target_system):
        exec_path_var = "@executable_path"
        if target_system == "linux" or platform.system() == "Linux" and not target_system:
            exec_path_var = "$ORIGIN"
        rpaths += [
            exec_path_var,
            "{exec_path_var}/../Lib".format(exec_path_var=exec_path_var)
        ]
    return rpaths
