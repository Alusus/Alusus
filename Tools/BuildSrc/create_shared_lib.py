import os
import sys
import subprocess
import shutil
SOURCE_LOCATION = os.path.abspath(__file__)
sys.path.insert(0, os.path.dirname(os.path.dirname(SOURCE_LOCATION)))
from utils import get_host_cxx_arch, shell_split, shell_join  # noqa


def _get_static_libs(arg_file=None, arg_dir=None, arg_dir_recurs=None):
    static_libs = set()

    # Add all static files in `--file` argument.
    if arg_file:
        for static_lib in arg_file:
            if os.path.splitext(static_lib)[1] == ".a":
                static_libs.add(os.path.abspath(static_lib))

    # Add all static files in `--dir` argument.
    if arg_dir:
        for static_lib_dir in arg_dir:
            for static_lib_name in os.listdir(static_lib_dir):
                if os.path.splitext(static_lib_name)[1] == ".a":
                    static_lib = os.path.abspath(
                        os.path.join(static_lib_dir, static_lib_name))
                    static_libs.add(static_lib)

    # Add all static files in `--dir-recurs` argument.
    if arg_dir_recurs:
        for static_lib_dir_recurs in arg_dir_recurs:
            tmp_static_libs = [os.path.abspath(os.path.join(dp, f)) for dp, dn, filenames in os.walk(
                static_lib_dir_recurs) for f in filenames if os.path.splitext(f)[1] == ".a"]
            static_libs.update(tmp_static_libs)

    return static_libs


def create_dll(arg_file=None, arg_dir=None, arg_dir_recurs=None, arg_output_dir=os.getcwd(),
               arg_output_name="tmp", arg_link_lib=None, arg_link_lib_path=None, new_environ=os.environ):
    try:
        os.remove(os.path.join(arg_output_dir,
                               "lib" + arg_output_name + ".dll.a"))
    except FileNotFoundError:
        pass

    try:
        os.remove(os.path.join(arg_output_dir,
                               "lib" + arg_output_name + ".dll"))
    except FileNotFoundError:
        pass

    static_libs = _get_static_libs(
        arg_file=arg_file, arg_dir=arg_dir, arg_dir_recurs=arg_dir_recurs)

    if len(static_libs) == 0:
        return (1, "You need to specify static library files to be added to the import library. " +
                   "Use \"--file\", \"--dir\", or \"--dir-recurs\" with approperiate \".a\" files " +
                   "(must not include import libraries).")

    # Create the DLL main file.
    host_cxx_arch = get_host_cxx_arch(new_environ=new_environ)
    dllmain_path = os.path.join(
        arg_output_dir, "dllmain_{output_name}.c".format(output_name=arg_output_name))
    dllmain_object_file_path = os.path.join(
        os.path.dirname(dllmain_path), os.path.splitext(os.path.basename(dllmain_path))[0] + ".o")
    with open(dllmain_path, "w") as fd:
        fd.write("#include <windows.h>\n\n" +
                 "BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {\n" +
                 "    return TRUE;\n" +
                 "}\n")
    cc = new_environ["CC"] if "CC" in new_environ else "{}-w64-mingw32-cc".format(
        host_cxx_arch)
    cc = shell_split(cc)
    p = subprocess.Popen(
        cc + [dllmain_path, "-c", "-o", dllmain_object_file_path], env=new_environ)
    ret = p.wait()
    if ret:
        return (1, "Failed to execute \"{cc}\".".format(cc=cc))

    # Create the DLL.
    cxx = new_environ["CXX"] if "CXX" in new_environ else "{}-w64-mingw32-c++".format(
        host_cxx_arch)
    cxx = shell_split(cxx)
    link_libs = []
    if arg_link_lib:
        for lib in arg_link_lib:
            link_libs += ["-l{lib}".format(lib=lib)]
    link_lib_paths = []
    if arg_link_lib_path:
        for lib_path in arg_link_lib_path:
            link_lib_paths += ["-L{lib_path}".format(lib_path=lib_path)]
    output_dll_path = os.path.join(
        arg_output_dir, "lib" + arg_output_name + ".dll")
    p = subprocess.Popen(
        cxx + [dllmain_object_file_path, "-shared", "-o", output_dll_path] + link_libs +
        link_lib_paths + ["-Wl,--out-implib,{import_lib_path}".format(
            import_lib_path=os.path.join(
                arg_output_dir, "lib" + arg_output_name + ".lib")
        ), "-fvisibility=default", "-Wl,--whole-archive"] + list(static_libs) + ["-Wl,--no-whole-archive"], env=new_environ)
    ret = p.wait()
    if ret:
        return (1, "Failed to execute {cxx}.".format(cxx=shell_join(cxx)))

    # Create both ".lib" and ".dll.a" files.
    shutil.copy2(
        os.path.join(arg_output_dir, "lib" + arg_output_name + ".lib"),
        os.path.join(arg_output_dir, "lib" + arg_output_name + ".dll.a")
    )

    # Remove temporary files.
    os.remove(dllmain_path)
    os.remove(dllmain_object_file_path)
    return (0, )


def create_dylib(arg_file=None, arg_dir=None, arg_dir_recurs=None, arg_output_dir=os.getcwd(),
                 arg_output_name="tmp", arg_link_lib=None, arg_link_lib_path=None, new_environ=os.environ):

    try:
        os.remove(os.path.join(arg_output_dir,
                               "lib" + arg_output_name + ".dylib"))
    except FileNotFoundError:
        pass

    static_libs = _get_static_libs(
        arg_file=arg_file, arg_dir=arg_dir, arg_dir_recurs=arg_dir_recurs)

    if len(static_libs) == 0:
        return (1, "You need to specify static library files to be added to the import library. " +
                   "Use \"--file\", \"--dir\", or \"--dir-recurs\" with approperiate \".a\" files.")

    # Create the DYLIB.
    host_cxx_arch = get_host_cxx_arch(new_environ=new_environ)
    cxx = new_environ["CXX"] if "CXX" in new_environ else "{}-apple-darwin19-c++".format(
        host_cxx_arch)
    cxx = shell_split(cxx)
    link_libs = []
    if arg_link_lib:
        for lib in arg_link_lib:
            link_libs += ["-l{lib}".format(lib=lib)]
    link_lib_paths = []
    if arg_link_lib_path:
        for lib_path in arg_link_lib_path:
            link_lib_paths += ["-L{lib_path}".format(lib_path=lib_path)]
    output_dylib_path = os.path.join(
        arg_output_dir, "lib" + arg_output_name + ".dylib")
    p = subprocess.Popen(
        cxx + ["-shared", "-fpic", "-o",
               output_dylib_path] + link_libs +
        link_lib_paths + ["-fPIC", "-fvisibility=default", "-Wl,-all_load"] + list(static_libs) + ["-Wl,-noall_load"], env=new_environ)
    ret = p.wait()
    if ret:
        return (1, "Failed to execute {cxx}.".format(cxx=shell_join(cxx)))
    return (0, )


def create_so(arg_file=None, arg_dir=None, arg_dir_recurs=None, arg_output_dir=os.getcwd(),
              arg_output_name="tmp", arg_link_lib=None, arg_link_lib_path=None, new_environ=os.environ):

    try:
        os.remove(os.path.join(arg_output_dir,
                               "lib" + arg_output_name + ".so"))
    except FileNotFoundError:
        pass

    static_libs = _get_static_libs(
        arg_file=arg_file, arg_dir=arg_dir, arg_dir_recurs=arg_dir_recurs)

    if len(static_libs) == 0:
        return (1, "You need to specify static library files to be added to the import library. " +
                   "Use \"--file\", \"--dir\", or \"--dir-recurs\" with approperiate \".a\" files.")

    # Create the SO.
    host_cxx_arch = get_host_cxx_arch(new_environ=new_environ)
    cxx = new_environ["CXX"] if "CXX" in new_environ else "{}-unknown-linux-gnu-c++".format(
        host_cxx_arch)
    cxx = shell_split(cxx)
    link_libs = []
    if arg_link_lib:
        for lib in arg_link_lib:
            link_libs += ["-l{lib}".format(lib=lib)]
    link_lib_paths = []
    if arg_link_lib_path:
        for lib_path in arg_link_lib_path:
            link_lib_paths += ["-L{lib_path}".format(lib_path=lib_path)]
    output_so_path = os.path.join(
        arg_output_dir, "lib" + arg_output_name + ".so")
    p = subprocess.Popen(
        cxx + link_libs +
        link_lib_paths + ["-fPIC", "-fvisibility=default", "-Wl,--whole-archive"] + list(static_libs) + ["-Wl,--no-whole-archive"] +
        ["-shared", "-o", output_so_path], env=new_environ)
    ret = p.wait()
    if ret:
        return (1, "Failed to execute {cxx}.".format(cxx=shell_join(cxx)))
    return (0, )
