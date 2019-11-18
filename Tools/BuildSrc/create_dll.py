import argparse
import os
import sys
import subprocess
import json


def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)


if __name__ == "__main__":
    # Parse args.
    parser = argparse.ArgumentParser()
    parser.add_argument('--file', action='append')
    parser.add_argument('--dir', action='append')
    parser.add_argument('--dir-recurs', action='append')
    parser.add_argument('--output-dir', default=os.getcwd())
    parser.add_argument('--output-name', default='tmp')
    parser.add_argument('--with-dlltool', default='dlltool')
    parser.add_argument('--with-ar', default='ar')
    parser.add_argument('--link-lib', action='append')
    parser.add_argument('--link-lib-path', action='append')
    args = parser.parse_args()

    static_libs = set()

    # Add all static files in `--file` argument.
    if args.file:
        for static_lib in args.file:
            if os.path.splitext(static_lib)[1] == '.a':
                static_libs.add(static_lib)

    # Add all static files in `--dir` argument.
    if args.dir:
        for static_lib_dir in args.dir:
            for static_lib_name in os.listdir(static_lib_dir):
                if os.path.splitext(static_lib_name)[1] == '.a':
                    static_lib = os.path.join(static_lib_dir, static_lib_name)
                    static_libs.add(static_lib)

    # Add all static files in `--dir-recurs` argument.
    if args.dir_recurs:
        for static_lib_dir_recurs in args.dir_recurs:
            tmp_static_libs = [os.path.join(dp, f) for dp, dn, filenames in os.walk(
                static_lib_dir_recurs) for f in filenames if os.path.splitext(f)[1] == '.a']
            static_libs.update(tmp_static_libs)

    if len(static_libs) == 0:
        eprint('You need to specify static library files to be added to the import library. ' + \
               'Use \"--file\", \"--dir\", or \"--dir-recurs\" with approperiate \".a\" files.')
        os._exit(1)
    
    # Create export string.
    import_lib_path = os.path.join(args.output_dir, 'lib{output_name}.dll.a'.format(output_name=args.output_name))
    export_str = 'create {import_lib_path}\n'.format(
        import_lib_path=import_lib_path)
    for static_lib in static_libs:
        export_str += 'addlib {static_lib}\n'.format(static_lib=static_lib)
    export_str += 'save\n'
    export_str += 'end\n'

    # Use "ar" and "dlltool" to create the import library and "def" file.
    p = subprocess.Popen([args.with_ar, '-M'], stdin=subprocess.PIPE)
    p.communicate(input=export_str.encode())
    ret = p.wait()
    if ret:
        eprint('Failed to execute \"{ar}\".'.format(ar=args.with_ar))
        os._exit(1)
    import_def_path = os.path.join(args.output_dir, 'lib{output_name}.dll.def'.format(output_name=args.output_name))
    ret = subprocess.call([args.with_dlltool, '-z', import_def_path, '--export-all-symbols', import_lib_path])
    if ret:
        eprint('Failed to execute \"{dlltool}\".'.format(dlltool=args.with_dlltool))
        os._exit(1)

    # Create the DLL main file.
    dllmain_path = os.path.join(args.output_dir, 'dllmain_{output_name}.c'.format(output_name=args.output_name))
    dllmain_object_file_path = os.path.join(
        os.path.dirname(dllmain_path), os.path.splitext(os.path.basename(dllmain_path))[0] + '.o')
    with open(dllmain_path, 'w') as fd:
        fd.write('#include <windows.h>\n\n' + \
                 'BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {\n' + \
                 '    return TRUE;\n' + \
                 '}\n')
    cc = os.environ['CC'] if 'CC' in os.environ else 'gcc'
    ret = subprocess.call([cc, dllmain_path, '-c', '-o', dllmain_object_file_path])
    if ret:
        eprint('Failed to execute \"{cc}\".'.format(cc=cc))
        os._exit(1)

    # Create the DLL.
    cxx = os.environ['CXX'] if 'CXX' in os.environ else 'g++'
    output_dll_path = os.path.join(args.output_dir, 'lib{output_name}.dll'.format(output_name=args.output_name))
    link_libs = []
    if args.link_lib:
        for lib in args.link_lib:
            link_libs += ['-l{lib}'.format(lib=lib)]
    link_lib_paths = []
    if args.link_lib_path:
        for lib_path in args.link_lib_path:
            link_lib_paths += ['-L{lib_path}'.format(lib_path=lib_path)]
    ret = subprocess.call(
        [cxx, dllmain_object_file_path, import_def_path, '-shared', '-o',
         output_dll_path, '-L{libdir}'.format(libdir=args.output_dir),
         '-l{libname}'.format(libname=args.output_name)] + link_libs + link_lib_paths)
    if ret:
        eprint('Failed to execute \"{cxx}\".'.format(cxx=cxx))
        os._exit(1)

    # Remove temporary files.
    os.remove(import_def_path)
    os.remove(dllmain_path)
    os.remove(dllmain_object_file_path)
