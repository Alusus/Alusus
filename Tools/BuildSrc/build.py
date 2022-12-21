import argparse
import sys
from colorama import init as init_colorama
import os
import multiprocessing
import json
import sanitize_filename

# fmt: off
# Alusus import(s).
sys.path.insert(0, os.path.dirname(__file__))
import common
import msg
import vcpkg
from build_type import BuildType
from target_triplet import TargetTriplet
from build_packages import create_packages, BuildPackages
# fmt: on


def build_alusus(alusus_build_location: str,
                 alusus_install_location: str,
                 alusus_build_type: BuildType,
                 alusus_target_triplet: TargetTriplet,
                 alusus_clean_and_build: bool,
                 alusus_bin_dirname: str,
                 alusus_lib_dirname: str,
                 alusus_include_dirname: str,
                 skip_installing_std_deps: bool,
                 verbose_output: bool = False):

    os.makedirs(alusus_build_location, exist_ok=True)
    os.makedirs(alusus_install_location, exist_ok=True)

    # Add the "vcpkg" path in the PATH/Path variable.
    environ = os.environ.copy()
    vcpkg_repo_path = None
    try:
        msg.info_msg("Finding vcpkg Git repository...")
        vcpkg_repo_path = vcpkg.get_vcpkg_repo_path(environ)
    except Exception as e:
        common.eprint(e)
        msg.fail_msg("Finding vcpkg Git repository.")
        return False
    msg.info_msg("Using vcpkg repo path={vcpkg_repo_path}.".format(
        vcpkg_repo_path=json.dumps(vcpkg_repo_path)))
    vcpkg_toolchain_file_path = os.path.join(
        vcpkg_repo_path, "scripts", "buildsystems", "vcpkg.cmake")
    path_env_var = "Path" if sys.platform == "win32" else "PATH"
    path_sep = ";" if sys.platform == "win32" else ":"
    environ[path_env_var] = (vcpkg_repo_path + path_sep + environ[path_env_var]
                             ) if path_env_var in environ else vcpkg_repo_path

    # Set the path to the updated vcpkg ports overlays and restore the ports overlays if they are changed.
    msg.info_msg("Restoring vcpkg dependency ports overlays...")
    alusus_vcpkg_ports_overlays_location = os.path.join(
        alusus_build_location, "AlususVcpkgPortsOverlay")
    try:
        vcpkg.restore_ports_overlays(
            alusus_vcpkg_ports_overlays_location, vcpkg_repo_path)
    except Exception as e:
        common.eprint(e)
        msg.fail_msg("Restoring vcpkg dependency ports overlays.")
        return False

    # Configure the build directory.
    msg.info_msg("Configuring Alusus with CMake...")
    cmake_cmd = [
        "cmake", common.ALUSUS_SOURCES_DIR,
        "-DCMAKE_INSTALL_PREFIX={alusus_install_location}".format(
            alusus_install_location=alusus_install_location),
        "-DCMAKE_BUILD_TYPE={alusus_build_type}".format(
            alusus_build_type=alusus_build_type.value.cmake_build_type),
        "-DCMAKE_TOOLCHAIN_FILE={vcpkg_toolchain_file_path}".format(
            vcpkg_toolchain_file_path=vcpkg_toolchain_file_path),
        "-DVCPKG_MANIFEST_DIR={vcpkg_alusus_manifest_dir}".format(
            vcpkg_alusus_manifest_dir=common.VCPKG_ALUSUS_MANIFEST_DIR),
        "-DVCPKG_TARGET_TRIPLET={vcpkg_target_triplet}".format(
            vcpkg_target_triplet=alusus_target_triplet.value.vcpkg_target_triplet),
        "-DVCPKG_OVERLAY_PORTS={vcpkg_alusus_overlay_ports}".format(
            vcpkg_alusus_overlay_ports=alusus_vcpkg_ports_overlays_location
        ),
        "-DALUSUS_BIN_DIR_NAME={alusus_bin_dirname}".format(
            alusus_bin_dirname=alusus_bin_dirname),
        "-DALUSUS_LIB_DIR_NAME={alusus_lib_dirname}".format(
            alusus_lib_dirname=alusus_lib_dirname),
        "-DALUSUS_INCLUDE_DIR_NAME={alusus_include_dirname}".format(
            alusus_include_dirname=alusus_include_dirname)
    ]
    if alusus_target_triplet.value.cmake_generator:
        msg.info_msg("Using preferred {cmake_generator} CMake generator with the CMake configuration.".format(
            cmake_generator=json.dumps(
                alusus_target_triplet.value.cmake_generator
            )
        ))
        cmake_cmd += ["-G", alusus_target_triplet.value.cmake_generator]
    ret = common.subprocess_run_hidden_except_on_error(
        cmake_cmd, cwd=alusus_build_location, env=environ, verbose_output=verbose_output)
    if ret.returncode:
        msg.fail_msg("Configuring Alusus with CMake.")
        return False

    # Build Alusus.
    msg.info_msg("Building Alusus...")
    cmake_cmd = ["cmake", "--build", alusus_build_location,
                 "--parallel", str(multiprocessing.cpu_count())]
    if alusus_clean_and_build:
        cmake_cmd += ["--clean-first"]
    ret = common.subprocess_run_hidden_except_on_error(
        cmake_cmd, cwd=alusus_build_location, env=environ, verbose_output=verbose_output)
    if ret.returncode:
        msg.fail_msg("Building Alusus.")
        return False

    # Install Alusus.
    msg.info_msg("Installing Alusus...")
    cmake_cmd = ["cmake", "--install", alusus_build_location]
    ret = common.subprocess_run_hidden_except_on_error(
        cmake_cmd, cwd=alusus_build_location, env=environ, verbose_output=verbose_output)
    if ret.returncode:
        msg.fail_msg("Installing Alusus.")
        return False

    # Install STD dependencies.
    if not skip_installing_std_deps:
        msg.info_msg("Installing Alusus STD dependencies...")

        # Install STD libraries.
        msg.info_msg("Installing Alusus STD library dependencies...")
        libs_to_install = []
        if alusus_target_triplet.value.platform == "linux":
            libs_to_install = [
                ("libcurl.so", "libcurl.so"),
                ("libkubazip.so", "libkubazip.so")
            ]
            # TODO: Add more libs to install as needed here.
        elif alusus_target_triplet.value.platform == "darwin":
            libs_to_install = [
                ("libcurl.dylib", "libcurl.dylib"),
                ("libkubazip.dylib", "libkubazip.dylib")
            ]
            # TODO: Add more libs to install as needed here.
        # TODO: Add more logic for other targets here.

        std_libs_location = os.path.join(
            alusus_build_location, "vcpkg_installed", alusus_target_triplet.value.vcpkg_target_triplet, "lib")
        alusus_libs_install_location = os.path.join(
            alusus_install_location, alusus_lib_dirname)
        if alusus_target_triplet.value.platform == "win32":
            # DLLs are found in the binary folder in Windows STD builds.
            std_libs_location = os.path.join(
                alusus_build_location, "vcpkg_installed", alusus_target_triplet.value.vcpkg_target_triplet, "bin")

            # Install the DLL in the binaries folder in Windows.
            alusus_libs_install_location = os.path.join(
                alusus_install_location, alusus_bin_dirname)

        for source_libname, dst_libname in libs_to_install:
            shlib_build_location = os.path.join(
                std_libs_location, source_libname)
            shlib_install_location = os.path.join(
                alusus_libs_install_location, dst_libname)

            # Only copy the shared library if different.
            ret = common.copy_if_different(
                shlib_build_location, shlib_install_location, follow_dst_symlinks=False)
            if ret == common.CopyIfDifferentReturnType.DstUpdated:
                msg.success_msg("Installing STD libname={libname}.".format(
                    libname=json.dumps(dst_libname)))
            elif ret == common.CopyIfDifferentReturnType.DstUpToDate:
                msg.info_msg("STD libname={libname} is up to date.".format(
                    libname=json.dumps(dst_libname)))
            elif ret == common.CopyIfDifferentReturnType.NoSrc:
                msg.err_msg("STD libname={libname} doesn't exist.".format(
                    libname=json.dumps(dst_libname)))

        # Install STD binaries.
        msg.info_msg("Installing Alusus STD binary dependencies...")
        bins_to_install = []
        if alusus_target_triplet.value.platform == "linux":
            bins_to_install = [
                ("wasm-ld", "wasm-ld")
            ]
            # TODO: Add more bins to install as needed here.
        elif alusus_target_triplet.value.platform == "darwin":
            bins_to_install = [
                ("wasm-ld", "wasm-ld")
            ]
            # TODO: Add more bins to install as needed here.
        # TODO: Add more logic for other targets here.

        std_bins_location = os.path.join(
            alusus_build_location, "vcpkg_installed", alusus_target_triplet.value.vcpkg_target_triplet, "bin")
        alusus_bins_install_location = os.path.join(
            alusus_install_location, alusus_bin_dirname)

        for source_binname, dst_binname in bins_to_install:
            bin_build_location = os.path.join(
                std_bins_location, source_binname)
            bin_install_location = os.path.join(
                alusus_bins_install_location, dst_binname)

            # Only copy the shared library if different.
            ret = common.copy_if_different(
                bin_build_location, bin_install_location, follow_dst_symlinks=False)
            if ret == common.CopyIfDifferentReturnType.DstUpdated:
                msg.success_msg("Installing STD binname={binname}.".format(
                    binname=json.dumps(dst_binname)))
            elif ret == common.CopyIfDifferentReturnType.DstUpToDate:
                msg.info_msg("STD binname={binname} is up to date.".format(
                    binname=json.dumps(dst_binname)))
            elif ret == common.CopyIfDifferentReturnType.NoSrc:
                msg.err_msg("STD binname={binname} doesn't exist.".format(
                    binname=json.dumps(dst_binname)))

    msg.success_msg("Building and installing Alusus.")
    return True


def parse_cmd_args(args):
    parser = argparse.ArgumentParser(description="Alusus build script")
    parser.add_argument("--build-location", type=str,
                        default=None, help="Alusus build location")
    parser.add_argument("--install-location", type=str,
                        default=None, help="Alusus install location")
    parser.add_argument("--build-type",
                        type=BuildType.from_alusus_build_type_str, default=BuildType.DEBUG, help="Alusus build type")
    parser.add_argument("--target-triplet",
                        type=TargetTriplet.from_alusus_target_triplet_str,
                        default=TargetTriplet.host_default_target_triplet(), help="Alusus target triplet")
    parser.add_argument("--clean-and-build", action="store_true",
                        help="Whether or not to clean Alusus before building")
    parser.add_argument("--build-packages", action="store_true",
                        help="Whether or not to build Alusus packages")
    parser.add_argument("--packages-location", type=str,
                        default=None, help="Alusus built packages location")
    parser.add_argument("--bin-dirname", type=str, default="Bin",
                        help="Alusus binary folder name inside Alusus install location")
    parser.add_argument("--lib-dirname", type=str, default="Lib",
                        help="Alusus library folder name inside Alusus install location")
    parser.add_argument("--include-dirname",
                        type=str, default="Include", help="Alusus include folder name inside Alusus install location")
    parser.add_argument("--skip-installing-std-deps",
                        help="Whether or not to skip installing the standard libraries dependencies", action="store_true")
    parser.add_argument("--print-supported-build-types", action="store_true",
                        help="Prints the supported build types list and exits")
    parser.add_argument("--print-supported-target-triplets", action="store_true",
                        help="Prints the supported target triplets list and exits")
    parser.add_argument("--verbose", action="store_true",
                        help="Verbose output when calling sub commands")
    processed_args = parser.parse_args(args)

    # Set the default build, install, and packages paths if no custom paths provided.
    alusus_local_build_path = os.path.join(
        os.path.abspath(os.getcwd()), "AlususBuild")
    target_dirname = sanitize_filename.sanitize(
        "{target_triplet}-{build_type}".format(
            target_triplet=processed_args.target_triplet.value.alusus_target_triplet,
            build_type=processed_args.build_type.value.alusus_build_type
        )
    )
    if processed_args.build_location == None:
        processed_args.build_location = os.path.join(
            alusus_local_build_path, "Intermediate", target_dirname)
    else:
        processed_args.build_location = os.path.abspath(processed_args.build_location)
    if processed_args.install_location == None:
        processed_args.install_location = os.path.join(
            alusus_local_build_path, "Install", target_dirname)
    else:
        processed_args.install_location = os.path.abspath(processed_args.install_location)
    if processed_args.packages_location == None:
        processed_args.packages_location = os.path.join(
            alusus_local_build_path, "Packages", target_dirname)
    else:
        processed_args.packages_location = os.path.abspath(processed_args.packages_location)

    return processed_args


if __name__ == "__main__":
    init_colorama()
    args = parse_cmd_args(sys.argv[1:])

    # Process print arguments first.
    if args.print_supported_build_types:
        for target in BuildType.list():
            print(target.value.alusus_build_type)
        exit(0)
    if args.print_supported_target_triplets:
        for target in TargetTriplet.list():
            print(target.value.alusus_target_triplet)
        exit(0)

    ret = build_alusus(args.build_location,
                       args.install_location,
                       args.build_type,
                       args.target_triplet,
                       args.clean_and_build,
                       args.bin_dirname,
                       args.lib_dirname,
                       args.include_dirname,
                       args.skip_installing_std_deps,
                       verbose_output=args.verbose)
    if not ret:
        exit(1)

    if args.build_packages:
        try:
            msg.info_msg("Building packages...")
            create_packages(
                BuildPackages.from_alusus_target_triplet(args.target_triplet),
                args.install_location,
                args.packages_location,
                verbose_output=args.verbose
            )
        except Exception as e:
            common.eprint(e)
            msg.fail_msg("Building required packages.")
            exit(1)
    exit(0)
