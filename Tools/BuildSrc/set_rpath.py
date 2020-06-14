import argparse
import os
import sys
import platform
import subprocess
SOURCE_LOCATION = os.path.abspath(__file__)
sys.path.insert(0, os.path.dirname(os.path.dirname(SOURCE_LOCATION)))
from utils import eprint  # noqa


# TODO: Add optional dependencies detection such as "machotools" and "elftools" to set RPATHs as
# an alternative to needing "install_name_tool" and "otool" for macOS or "patchelf" for Linux.


def _is_elf(test_file, patchelf=None):
    patchelf = "patchelf" if not patchelf else patchelf
    with open(os.devnull, "w") as fd_null:
        p = subprocess.Popen([patchelf, test_file],
                             stdout=fd_null, stderr=fd_null)
        return p.wait() == 0


def _get_executable_files(arg_file=None, arg_dir=None, arg_dir_recurs=None, target_system=None, otool=None, patchelf=None):
    executable_files_paths = set()
    executable_files_basenames = set()

    otool = "otool" if not otool else otool
    patchelf = "patchelf" if not patchelf else patchelf

    # Add all files in `--file` argument.
    if arg_file:
        for current_file in arg_file:
            if not os.path.isfile(os.path.abspath(current_file)):
                continue
            if target_system == "macos" or platform.system() == "Darwin" and not target_system:
                if subprocess.check_output([otool, "-L", os.path.abspath(current_file)]).decode().strip().endswith("is not an object file"):
                    continue
            elif target_system == "linux" or platform.system() == "Linux" and not target_system:
                if not _is_elf(os.path.abspath(current_file), patchelf=patchelf):
                    continue
            if (target_system == "macos" or platform.system() == "Darwin" and not target_system) or\
                    (target_system == "linux" or platform.system() == "Linux" and not target_system):
                executable_files_paths.add(os.path.abspath(current_file))
                executable_files_basenames.add(
                    os.path.basename(os.path.abspath(current_file)))

    # Add all files in `--dir` argument.
    if arg_dir:
        for current_file_dir in arg_dir:
            for current_file_name in os.listdir(current_file_dir):
                current_file = os.path.abspath(
                    os.path.join(current_file_dir, current_file_name))
                if not os.path.isfile(os.path.abspath(current_file)):
                    continue
                if target_system == "macos" or platform.system() == "Darwin" and not target_system:
                    if subprocess.check_output([otool, "-L", os.path.abspath(current_file)]).decode().strip().endswith("is not an object file"):
                        continue
                elif target_system == "linux" or platform.system() == "Linux" and not target_system:
                    if not _is_elf(os.path.abspath(current_file), patchelf=patchelf):
                        continue

                if (target_system == "macos" or platform.system() == "Darwin" and not target_system) or\
                        (target_system == "linux" or platform.system() == "Linux" and not target_system):
                    executable_files_paths.add(current_file)
                    executable_files_basenames.add(
                        os.path.basename(os.path.abspath(current_file)))

    # Add all files in `--dir-recurs` argument.
    if arg_dir_recurs:
        for executable_files_dir in arg_dir_recurs:
            if target_system == "macos" or platform.system() == "Darwin" and not target_system:
                tmp_executable_files_paths = [os.path.abspath(os.path.join(dp, f)) for dp, dn, filenames in os.walk(
                    executable_files_dir) for f in filenames if not (
                        subprocess.check_output([otool, "-L", os.path.abspath(os.path.abspath(
                            os.path.join(dp, f)))]).decode().strip().endswith("is not an object file")
                )]
                tmp_executable_files_basenames = [os.path.basename(
                    path) for path in tmp_executable_files_paths]
                executable_files_paths.update(tmp_executable_files_paths)
                executable_files_basenames.update(
                    tmp_executable_files_basenames)
            elif target_system == "linux" or platform.system() == "Linux" and not target_system:
                tmp_executable_files_paths = [os.path.abspath(os.path.join(dp, f)) for dp, dn, filenames in os.walk(
                    executable_files_dir) for f in filenames if _is_elf(os.path.join(dp, f), patchelf=patchelf)]
                tmp_executable_files_basenames = [os.path.basename(
                    path) for path in tmp_executable_files_paths]
                executable_files_paths.update(tmp_executable_files_paths)
                executable_files_basenames.update(
                    tmp_executable_files_basenames)
    return executable_files_paths, executable_files_basenames


def _get_current_rpaths(dylib_path, otool=None):
    otool = "otool" if not otool else otool
    cmd_output = subprocess.check_output([otool, "-l", dylib_path]).decode()
    cmd_output_lines = cmd_output.split(os.linesep)
    final_output = list()
    i = 0
    while i < len(cmd_output_lines):
        current_line = cmd_output_lines[i]
        if current_line.strip() == "cmd LC_RPATH":
            rpath_line = cmd_output_lines[i + 2]
            rpath = rpath_line[rpath_line.find("path") + len("path"):]
            rpath = rpath[:rpath.rfind("(offset")]
            rpath = rpath.strip()
            final_output.append(rpath)
        i += 1
    return final_output


def _get_dylib_deps(dylib_path, otool=None):
    otool = "otool" if not otool else otool
    cmd_output = subprocess.check_output([otool, "-L", dylib_path]).decode()
    cmd_output_lines = cmd_output.split(os.linesep)
    final_output = list()
    for line in cmd_output_lines:
        if line.strip() and not line.strip().endswith(":"):
            to_add = line.strip()[:line.strip().rfind(
                "(compatibility")].strip()
            if to_add.endswith(".dylib"):
                final_output.append(to_add)
    return final_output


def set_rpath(arg_rpath=None, arg_file=None, arg_dir=None, arg_dir_recurs=None, environ=os.environ, target_system=None):
    install_name_tool = environ["INSTALL_NAME_TOOL"] if "INSTALL_NAME_TOOL" in environ else "install_name_tool"
    otool = environ["OTOOL"] if "OTOOL" in environ else "otool"
    patchelf = environ["PATCHELF"] if "PATCHELF" in environ else "patchelf"
    executable_files_paths, executable_files_basenames = _get_executable_files(
        arg_file=arg_file, arg_dir=arg_dir, arg_dir_recurs=arg_dir_recurs, target_system=target_system, otool=otool, patchelf=patchelf)

    for executable_file_path in executable_files_paths:
        if target_system == "macos" or platform.system() == "Darwin" and not target_system:
            # Set the ID.
            otool_out_lines = subprocess.check_output(
                [otool, "-D", executable_file_path]).decode().split(os.pathsep)
            current_id = otool_out_lines[1].strip()
            current_id_parts = current_id.rsplit("/", 1)
            current_id_basename = current_id_parts[len(
                current_id_parts) - 1].strip()
            new_id = "@rpath/{}".format(current_id_basename)
            with open(os.devnull, "w") as fd_null:
                p = subprocess.Popen([install_name_tool, "-id",
                                      new_id, executable_file_path], stdout=fd_null, stderr=fd_null)
                p.wait()

            # Delete the current RPATHs.
            current_rpaths = _get_current_rpaths(
                executable_file_path, otool=otool)
            for current_rpath in current_rpaths:
                with open(os.devnull, "w") as fd_null:
                    p = subprocess.Popen([install_name_tool, "-delete_rpath",
                                          current_rpath, executable_file_path], stdout=fd_null, stderr=fd_null)
                    p.wait()

            # Update the dependencies with "@rpath" tag.
            rpaths = arg_rpath
            dylib_deps = _get_dylib_deps(executable_file_path, otool=otool)
            for dylib_dep in dylib_deps:
                dylib_dep_parts = dylib_dep.rsplit("/", 1)
                dylib_dep_basename = dylib_dep_parts[len(
                    dylib_dep_parts) - 1].strip()

                # Leave non-local ones intact.
                if dylib_dep_basename in executable_files_basenames:
                    new_dylib_dep = "@rpath/{}".format(dylib_dep_basename)
                    with open(os.devnull, "w") as fd_null:
                        p = subprocess.Popen([install_name_tool, "-change",
                                              dylib_dep, new_dylib_dep, executable_file_path], stdout=fd_null, stderr=fd_null)
                        p.wait()

            # Add the new RPATHs.
            for rpath in rpaths:
                if rpath != "@rpath":
                    with open(os.devnull, "w") as fd_null:
                        p = subprocess.Popen(
                            [install_name_tool, "-add_rpath", rpath, executable_file_path], stdout=fd_null, stderr=fd_null)
                        p.wait()
        if target_system == "linux" or platform.system() == "Linux" and not target_system:
            rpaths = arg_rpath
            rpaths_str = ":".join(rpaths)
            with open(os.devnull, "w") as fd_null:
                p = subprocess.Popen(
                    [patchelf, "--set-rpath", rpaths_str, executable_file_path], stdout=fd_null, stderr=fd_null)
                p.wait()
    return (0, )


# if __name__ == "__main__":
#     # Parse args.
#     parser = argparse.ArgumentParser()
#     parser.add_argument("--rpath", action="append")
#     parser.add_argument("--file", action="append")
#     parser.add_argument("--dir", action="append")
#     parser.add_argument("--dir-recurs", action="append")
#     parser.add_argument("--target-system", default=None)
#     args = parser.parse_args()

#     ret = set_rpath(arg_rpath=args.rpath, arg_file=args.file, arg_dir=args.dir,
#                     arg_dir_recurs=args.dir_recurs, target_system=args.target_system)
#     if ret[0]:
#         eprint(ret[1])
#     os._exit(ret[0])
