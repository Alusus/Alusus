import argparse
import os
import sys
import subprocess
import json

_arg_output_dir = os.getcwd()

def _eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

def create_dll(arg_file=None, arg_dir=None, arg_dir_recurs=None, arg_output_dir=_arg_output_dir, arg_output_name='tmp', arg_with_dlltool='dlltool', arg_with_ar='ar', arg_link_lib=None, arg_link_lib_path=None):
    static_libs = set()

    # Add all static files in `--file` argument.
    if arg_file:
        for static_lib in arg_file:
            if os.path.splitext(static_lib)[1] == '.a':
                static_libs.add(static_lib)

    # Add all static files in `--dir` argument.
    if arg_dir:
        for static_lib_dir in arg_dir:
            for static_lib_name in os.listdir(static_lib_dir):
                if os.path.splitext(static_lib_name)[1] == '.a':
                    static_lib = os.path.join(static_lib_dir, static_lib_name)
                    static_libs.add(static_lib)

    # Add all static files in `--dir-recurs` argument.
    if arg_dir_recurs:
        for static_lib_dir_recurs in arg_dir_recurs:
            tmp_static_libs = [os.path.join(dp, f) for dp, dn, filenames in os.walk(
                static_lib_dir_recurs) for f in filenames if os.path.splitext(f)[1] == '.a']
            static_libs.update(tmp_static_libs)

    if len(static_libs) == 0:
        return (1, 'You need to specify static library files to be added to the import library. ' + \
                   'Use \"--file\", \"--dir\", or \"--dir-recurs\" with approperiate \".a\" files.')
    
    # Create export string.
    import_lib_path = os.path.join(arg_output_dir, 'lib{output_name}.dll.a'.format(output_name=arg_output_name))
    export_str = 'create {import_lib_path}\n'.format(
        import_lib_path=import_lib_path)
    for static_lib in static_libs:
        export_str += 'addlib {static_lib}\n'.format(static_lib=static_lib)
    export_str += 'save\n'
    export_str += 'end\n'

    # Use "ar" and "dlltool" to create the import library and "def" file.
    p = subprocess.Popen([arg_with_ar, '-M'], stdin=subprocess.PIPE)
    p.communicate(input=export_str.encode())
    ret = p.wait()
    if ret:
        return (1, 'Failed to execute \"{ar}\".'.format(ar=arg_with_ar))
    import_def_path = os.path.join(arg_output_dir, 'lib{output_name}.dll.def'.format(output_name=arg_output_name))
    ret = subprocess.call([arg_with_dlltool, '-z', import_def_path, '--export-all-symbols', import_lib_path])
    if ret:
        return (1, 'Failed to execute \"{dlltool}\".'.format(dlltool=arg_with_dlltool))
    # Create the DLL main file.
    dllmain_path = os.path.join(arg_output_dir, 'dllmain_{output_name}.c'.format(output_name=arg_output_name))
    dllmain_object_file_path = os.path.join(
        os.path.dirname(dllmain_path), os.path.splitext(os.path.basename(dllmain_path))[0] + '.o')
    with open(dllmain_path, 'w') as fd:
        fd.write('#include <windows.h>\n\n' + \
                 'BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {\n' + \
                 '    return TRUE;\n' + \
                 '}\n')
    cc = os.environ['CC'] if 'CC' in os.environ else 'gcc'
    cc = cc.split()
    ret = subprocess.call(cc + [dllmain_path, '-c', '-o', dllmain_object_file_path])
    if ret:
        return (1, 'Failed to execute \"{cc}\".'.format(cc=cc))


    # Create the DLL.
    cxx = os.environ['CXX'] if 'CXX' in os.environ else 'g++'
    cxx = cxx.split()
    output_dll_path = os.path.join(arg_output_dir, 'lib{output_name}.dll'.format(output_name=arg_output_name))
    link_libs = []
    if arg_link_lib:
        for lib in arg_link_lib:
            link_libs += ['-l{lib}'.format(lib=lib)]
    link_lib_paths = []
    if arg_link_lib_path:
        for lib_path in arg_link_lib_path:
            link_lib_paths += ['-L{lib_path}'.format(lib_path=lib_path)]
    ret = subprocess.call(
        cxx + [dllmain_object_file_path, import_def_path, '-shared', '-o',
         output_dll_path, '-L{libdir}'.format(libdir=arg_output_dir),
         '-l{libname}'.format(libname=arg_output_name)] + link_libs + link_lib_paths)
    if ret:
        return (1, 'Failed to execute \"{cxx}\".'.format(cxx=cxx))

    # Remove temporary files.
    os.remove(import_def_path)
    os.remove(dllmain_path)
    os.remove(dllmain_object_file_path)
    return (0, )

if __name__ == "__main__":
    # Parse args.
    parser = argparse.ArgumentParser()
    parser.add_argument('--file', action='append')
    parser.add_argument('--dir', action='append')
    parser.add_argument('--dir-recurs', action='append')
    parser.add_argument('--output-dir', default=_arg_output_dir)
    parser.add_argument('--output-name', default='tmp')
    parser.add_argument('--with-dlltool', default='dlltool')
    parser.add_argument('--with-ar', default='ar')
    parser.add_argument('--link-lib', action='append')
    parser.add_argument('--link-lib-path', action='append')
    args = parser.parse_args()

    ret = create_dll(arg_file=args.file, arg_dir=args.dir, arg_dir_recurs=args.dir_recurs,
               arg_output_dir=args.output_dir, arg_output_name=args.output_name,
               arg_with_dlltool=args.with_dlltool, arg_with_ar=args.with_ar,
               arg_link_lib=args.link_lib, arg_link_lib_path=args.link_lib_path)
    if ret[0]:
        _eprint(ret[1])
    os._exit(ret[0])
