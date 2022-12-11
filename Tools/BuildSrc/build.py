import argparse
from enum import Enum
import sys
from colorama import init as init_colorama
import alusus_common
import msg
import subprocess
import os
import shutil
import platform
import multiprocessing
import pygit2
import json
import checksumdir
import filecmp
import tarfile
import hashlib
import sanitize_filename
from version_info import AlususVersionInfo


class ExtendedEnum(Enum):
    @classmethod
    def list(cls):
        for item in cls:
            return list(map(lambda c: c, cls))


class _AlususCombinedTargetTriplet:
    def __init__(self,
                 alusus_target_triplet: str,
                 vcpkg_target_triplet: str,
                 platform: str,
                 machine: str,
                 create_packages_func,
                 abi=None,
                 cmake_generator=None) -> None:
        self._alusus_target_triplet = alusus_target_triplet
        self._vcpkg_target_triplet = vcpkg_target_triplet
        self._platform = platform
        self._machine = machine
        self._create_packages_func = create_packages_func
        self._abi = abi
        self._cmake_generator = cmake_generator

    @property
    def alusus_target_triplet(self) -> str:
        return self._alusus_target_triplet

    @property
    def vcpkg_target_triplet(self) -> str:
        return self._vcpkg_target_triplet

    @property
    def platform(self) -> str:
        return self._platform

    @property
    def machine(self) -> str:
        return self._machine

    @property
    def abi(self):
        return self._abi

    @property
    def cmake_generator(self):
        return self._cmake_generator


class AlususTargetTriplet(ExtendedEnum):
    @staticmethod
    def host_default_target_triplet():
        for item in AlususTargetTriplet:
            if item.value.platform == sys.platform and item.value.machine == platform.machine():
                return item

        raise NotImplementedError("Current host has no matching build target.")

    @staticmethod
    def from_alusus_target_triplet_str(alusus_target_triplet_str: str):
        for item in AlususTargetTriplet:
            if item.value.alusus_target_triplet == alusus_target_triplet_str:
                return item
        raise NotImplementedError("Unsupported target triplet.")

    @staticmethod
    def _create_to_be_packaged_dir(alusus_install_location, alusus_packages_location):
        msg.info_msg("Creating to-be-packaged directory...")
        # Create the to-be-installed directory.
        to_be_packaged_dir = os.path.join(
            alusus_packages_location, ".AlususToBePackaged")
        if sys.platform == "win32":
            to_be_packaged_dir = os.path.join(
                alusus_packages_location, "AlususToBePackaged")
        if os.path.isdir(to_be_packaged_dir):
            shutil.rmtree(to_be_packaged_dir, ignore_errors=True)
        elif os.path.isfile(to_be_packaged_dir):
            os.remove(to_be_packaged_dir)
        shutil.copytree(alusus_install_location, to_be_packaged_dir)
        if sys.platform == "win32":
            import ctypes
            FILE_ATTRIBUTE_HIDDEN = 0x02
            ret = ctypes.windll.kernel32.SetFileAttributesW(
                to_be_packaged_dir, FILE_ATTRIBUTE_HIDDEN)
            if not ret:  # There was an error.
                raise ctypes.WinError()

        # Copy other installation files.
        shutil.rmtree(os.path.join(
            to_be_packaged_dir, "Doc"), ignore_errors=True)
        shutil.copytree(
            os.path.join(alusus_common.ALUSUS_REPO_PATH, "Doc"),
            os.path.join(to_be_packaged_dir, "Doc")
        )
        shutil.rmtree(os.path.join(to_be_packaged_dir, "Notices_L18n"),
                      ignore_errors=True)
        shutil.copytree(
            os.path.join(alusus_common.ALUSUS_REPO_PATH, "Notices_L18n"),
            os.path.join(to_be_packaged_dir, "Notices_L18n")
        )
        os.makedirs(os.path.join(to_be_packaged_dir, "Tools",
                    "Gtk_Syntax_Highlighting"), exist_ok=True)
        shutil.copy2(
            os.path.join(alusus_common.ALUSUS_REPO_PATH, "Tools",
                         "Gtk_Syntax_Highlighting", "alusus.lang"),
            os.path.join(to_be_packaged_dir, "Tools",
                         "Gtk_Syntax_Highlighting", "alusus.lang")
        )
        shutil.copy2(
            os.path.join(alusus_common.ALUSUS_REPO_PATH, "Tools",
                         "Gtk_Syntax_Highlighting", "alusus_dark_style.xml"),
            os.path.join(to_be_packaged_dir, "Tools",
                         "Gtk_Syntax_Highlighting", "alusus_dark_style.xml")
        )
        shutil.copy2(
            os.path.join(alusus_common.ALUSUS_REPO_PATH, "Tools",
                         "Gtk_Syntax_Highlighting", "alusus_light_style.xml"),
            os.path.join(to_be_packaged_dir, "Tools",
                         "Gtk_Syntax_Highlighting", "alusus_light_style.xml")
        )
        shutil.copy2(
            os.path.join(alusus_common.ALUSUS_REPO_PATH, "changelog.en.md"),
            os.path.join(to_be_packaged_dir, "changelog.en.md")
        )
        shutil.copy2(
            os.path.join(alusus_common.ALUSUS_REPO_PATH, "changelog.ar.md"),
            os.path.join(to_be_packaged_dir, "changelog.ar.md")
        )
        shutil.copy2(
            os.path.join(alusus_common.ALUSUS_REPO_PATH, "license.pdf"),
            os.path.join(to_be_packaged_dir, "license.pdf")
        )
        shutil.copy2(
            os.path.join(alusus_common.ALUSUS_REPO_PATH, "license.txt"),
            os.path.join(to_be_packaged_dir, "license.txt")
        )

        return to_be_packaged_dir

    @staticmethod
    def _create_packages_common(alusus_packages_location, to_be_packaged_dir, package_basename):
        # Build TAR GZ file.
        msg.info_msg("Building TAR GZ package...")
        with tarfile.open(os.path.join(alusus_packages_location, "{package_basename}.tar.gz".format(package_basename=package_basename)), "w:gz") as tar:
            tar.add(to_be_packaged_dir, arcname=package_basename)

    @staticmethod
    def _generate_fpm_cmd(target_system, version_info, alusus_packages_location, to_be_packaged_dir, package_basename, machine, custom_extention=None) -> str:
        return [
            "fpm", "-s", "dir", "-t", target_system, "-a", machine,
            "-n", alusus_common.FPM_PACKAGE_NAME, "-v", version_info.version_lossy,
            "--description", alusus_common.FPM_PACKAGE_DESCRIPTION
        ] +\
            ([] if version_info.revision_lossy else ["--iteration", version_info.revision_lossy]) +\
            [
            "--url", alusus_common.FPM_PACKAGE_URL, "-m", alusus_common.FPM_PACKAGE_MAINTAINER,
            "--after-install", os.path.join(alusus_common.ALUSUS_REPO_PATH,
                                            "Tools", "Package_Scripts", "post_install.sh"),
            "--after-remove", os.path.join(alusus_common.ALUSUS_REPO_PATH,
                                           "Tools", "Package_Scripts", "post_remove.sh"),
            "-f", "--prefix", os.path.join('/', 'opt', 'Alusus'),
            "-C", to_be_packaged_dir,
            "-p", os.path.join(
                alusus_packages_location,
                "{package_basename}.{ext}".format(
                    package_basename=package_basename,
                    ext=(custom_extention if custom_extention else target_system)
                )
            )
        ]

    @staticmethod
    def _create_packages_linux(alusus_packages_location, to_be_packaged_dir, package_basename, version_info, machine):
        AlususTargetTriplet._create_packages_common(
            alusus_packages_location, to_be_packaged_dir, package_basename)

        err = False

        # Build DEB file.
        msg.info_msg("Building DEB package...")
        ret = subprocess.run(
            AlususTargetTriplet._generate_fpm_cmd(
                "deb", version_info, alusus_packages_location, to_be_packaged_dir, package_basename, machine),
            cwd=alusus_packages_location
        )
        if ret.returncode:
            # Log error but continue.
            msg.err_msg("Building DEB package.")
            err = True

        # Build RPM file.
        msg.info_msg("Building RPM package...")
        ret = subprocess.run(
            AlususTargetTriplet._generate_fpm_cmd(
                "rpm", version_info, alusus_packages_location, to_be_packaged_dir, package_basename, machine),
            cwd=alusus_packages_location
        )
        if ret.returncode:
            # Log error but continue.
            msg.err_msg("Building RPM package.")
            err = True

        # Build PACMAN file.
        msg.info_msg("Building PACMAN package...")
        ret = subprocess.run(
            AlususTargetTriplet._generate_fpm_cmd(
                "pacman", version_info, alusus_packages_location, to_be_packaged_dir, package_basename, machine, custom_extention="pkg"),
            cwd=alusus_packages_location
        )
        if ret.returncode:
            # Log error but continue.
            msg.err_msg("Building PACMAN package.")
            err = True

        return err

    @staticmethod
    def _create_packages_osx(alusus_packages_location, to_be_packaged_dir, package_basename, version_info, machine):
        AlususTargetTriplet._create_packages_common(
            alusus_packages_location, to_be_packaged_dir, package_basename)

        err = False

        # Build OSXPKG file.
        msg.info_msg("Building OSXPKG package...")
        ret = subprocess.run(
            AlususTargetTriplet._generate_fpm_cmd(
                "osxpkg", version_info, alusus_packages_location, to_be_packaged_dir, package_basename, machine, custom_extention="pkg"),
            cwd=alusus_packages_location
        )
        if ret.returncode:
            # Log error but continue.
            msg.err_msg("Building OSXPKG package.")
            err = True

        return err

    def create_packages(self, alusus_install_location, alusus_packages_location):
        to_be_packaged_dir = AlususTargetTriplet._create_to_be_packaged_dir(
            alusus_install_location, alusus_packages_location)
        alusus_packages_location = os.path.join(
            alusus_packages_location, "AlususPackages")
        if os.path.isdir(alusus_packages_location):
            shutil.rmtree(alusus_packages_location, ignore_errors=True)
        elif os.path.isfile(alusus_packages_location):
            os.remove(alusus_packages_location)
        os.makedirs(alusus_packages_location, exist_ok=True)

        version_info = AlususVersionInfo(
            pygit2.Repository(path=alusus_common.ALUSUS_REPO_PATH))
        package_basename = "Alusus-{version}{revision}-{platform}{abi}-{machine}".format(
            version=version_info.version_lossy,
            revision="-" + version_info.revision_lossy if version_info.revision_lossy else "",
            platform=self.value.platform,
            abi="-" + self.value.abi if self.value.abi else "",
            machine=self.value.machine
        )
        package_basename = "{sanitized_package_basename}-{package_basename_short_hash}".format(
            sanitized_package_basename=sanitize_filename.sanitize(
                package_basename),
            package_basename_short_hash=hashlib.sha256(
                package_basename.encode()).hexdigest()[-8:]
        )

        err = self.value._create_packages_func(
            alusus_packages_location, to_be_packaged_dir, package_basename, version_info, self.value.machine)

        msg.info_msg("Cleaning up to-be-packaged directory...")
        shutil.rmtree(to_be_packaged_dir, ignore_errors=True)

        if err:
            msg.err_msg("Building packages step has errors.")
        else:
            msg.success_msg("Building packages.")

    def _get_unix_cmake_generator():
        # Prefer using "ninja" over "make" if available.
        if shutil.which("ninja"):
            return "Ninja"
        else:
            return "Unix Makefiles"

    # Target triplets we support building for.
    X64_LINUX = _AlususCombinedTargetTriplet(
        "x64-linux", "x64-linux-release", "linux", "x86_64", _create_packages_linux, abi="gnu", cmake_generator=_get_unix_cmake_generator())
    X64_OSX = _AlususCombinedTargetTriplet(
        "x64-osx", "x64-osx-release", "darwin", "x86_64", _create_packages_osx, cmake_generator=_get_unix_cmake_generator())
    # TODO: Add more targets here when we add support building for.


class _AlususToCMakeBuildType:
    def __init__(self, alusus_build_type: str, cmake_build_type: str) -> None:
        self._alusus_build_type = alusus_build_type
        self._cmake_build_type = cmake_build_type

    @property
    def alusus_build_type(self) -> str:
        return self._alusus_build_type

    @property
    def cmake_build_type(self) -> str:
        return self._cmake_build_type


class AlususBuildType(ExtendedEnum):
    @staticmethod
    def from_alusus_build_type_str(alusus_build_type_str: str):
        for item in AlususBuildType:
            if item.value.alusus_build_type == alusus_build_type_str:
                return item
        raise NotImplementedError("Unknown build type.")

    # Alusus build types.
    DEBUG = _AlususToCMakeBuildType("debug", "Debug")
    RELEASE = _AlususToCMakeBuildType("release", "Release")


def build_alusus(alusus_build_location: str,
                 alusus_install_location: str,
                 alusus_build_type: AlususBuildType,
                 alusus_target_triplet: AlususTargetTriplet,
                 alusus_clean_and_build: bool,
                 alusus_bin_dirname: str,
                 alusus_lib_dirname: str,
                 alusus_include_dirname: str,
                 skip_installing_std_deps: bool):
    def _get_vcpkg_repo_path(environ):
        ALUSUS_VCPKG_REPO_PATH_ENV_VAR = "ALUSUS_VCPKG_REPO_PATH"
        vcpkg_repo_path = None
        if ALUSUS_VCPKG_REPO_PATH_ENV_VAR in environ:
            vcpkg_repo_path = environ[ALUSUS_VCPKG_REPO_PATH_ENV_VAR]
        else:
            vcpkg_bin_path = shutil.which("vcpkg")
            if not vcpkg_bin_path:
                raise OSError(
                    "Unable to find \"vcpkg\" binary. Make sure it exists in your PATH variable.")
            vcpkg_bin_path = os.path.realpath(vcpkg_bin_path)
            vcpkg_repo_path = os.path.dirname(vcpkg_bin_path)

        return vcpkg_repo_path

    def _restore_ports_overlay(alusus_vcpkg_ports_overlays_location, vcpkg_repo_path):
        manifest_file_path = os.path.join(
            alusus_common.VCPKG_ALUSUS_MANIFEST_DIR, "vcpkg.json")
        manifest_file_data = None
        repo = None
        for package_name in os.listdir(alusus_common.VCPKG_ALUSUS_PORTS_OVERLAY_CHANGES_DIR):
            overlay_port_location = os.path.join(
                alusus_vcpkg_ports_overlays_location, package_name)

            # Check for port overlay hash change.
            hash_data = {}
            with open(
                os.path.join(alusus_common.VCPKG_ALUSUS_PORTS_OVERLAY_HASHES_DIR, "{package_name}.json".format(
                    package_name=package_name)), "r"
            ) as fd:
                hash_data = json.load(fd)
            hash_algorithm = hash_data["Algorithm"] if "Algorithm" in hash_data else "sha256"
            hash = hash_data["Hash"]
            if os.path.isdir(overlay_port_location) and\
                    checksumdir.dirhash(overlay_port_location, hashfunc=hash_algorithm) == hash:
                msg.info_msg("Dependency {package_name}'s vcpkg port overlay is up to date.".format(
                    package_name=json.dumps(package_name)))
                continue

            msg.info_msg("Updating dependency {package_name}'s port overlay...".format(
                package_name=json.dumps(package_name)))

            # Get the package version and port number from the "vcpkg.json" manifest file.
            if not manifest_file_data:
                with open(manifest_file_path, "r") as fd:
                    manifest_file_data = json.load(fd)
            package_version = None
            package_port_num = None
            for package_obj in manifest_file_data["overrides"]:
                if package_obj["name"] == package_name:
                    if "version" in package_obj:
                        package_version = package_obj["version"]
                    elif "version-semver" in package_obj:
                        package_version = package_obj["version-semver"]
                    elif "version-date" in package_obj:
                        package_version = package_obj["version-date"]
                    elif "version-string" in package_obj:
                        package_version = package_obj["version-string"]
                    else:
                        raise NotImplementedError(
                            "Unsupported \"vcpkg\" versioning scheme.")
                    package_port_num = 0
                    if "port-version" in package_obj:
                        package_port_num = package_obj["port-version"]
                    break

            # Find the Git tree that matches the version and port number in "vcpkg.json" manifest file.
            versions_data = {}
            with open(
                os.path.join(vcpkg_repo_path, "versions", "{package_name_first_letter}-".format(
                    package_name_first_letter=package_name[0]), "{package_name}.json".format(package_name=package_name)), "r"
            ) as fd:
                versions_data = json.load(fd)

            git_tree_hash = None
            for version_obj in versions_data["versions"]:

                to_compare_package_version = None
                if "version" in version_obj:
                    to_compare_package_version = version_obj["version"]
                elif "version-semver" in version_obj:
                    to_compare_package_version = version_obj["version-semver"]
                elif "version-date" in version_obj:
                    to_compare_package_version = version_obj["version-date"]
                elif "version-string" in version_obj:
                    to_compare_package_version = version_obj["version-string"]
                else:
                    raise NotImplementedError(
                        "Unsupported \"vcpkg\" versioning scheme.")

                to_compare_package_port_num = 0
                if "port-version" in version_obj:
                    to_compare_package_port_num = version_obj["port-version"]

                if to_compare_package_version == package_version and to_compare_package_port_num == package_port_num:
                    git_tree_hash = version_obj["git-tree"]
                    break

            # Empty current overlay port folder.
            if os.path.isdir(overlay_port_location):
                shutil.rmtree(overlay_port_location, ignore_errors=True)
            elif os.path.isfile(overlay_port_location):
                os.remove(overlay_port_location)

            # Restore the upstream port files inside Alusus build directory.
            def _restore_upstream_port(git_tree, current_path):
                os.makedirs(current_path, exist_ok=True)
                for item in git_tree:
                    if item.type == pygit2.GIT_OBJ_TREE:
                        _restore_upstream_port(item, os.path.join(item.name))
                    elif item.type == pygit2.GIT_OBJ_BLOB:
                        with open(os.path.join(current_path, item.name), "wb") as fd:
                            fd.write(item.data)
            if not repo:
                repo = pygit2.Repository(vcpkg_repo_path)
            _restore_upstream_port(
                repo.get(git_tree_hash), overlay_port_location)

            # Apply Alusus port overlay changes to the restored port files.
            def _apply_alusus_overlay_port_changes(current_src_path, current_dst_path):
                os.makedirs(current_dst_path, exist_ok=True)
                for item in os.listdir(current_src_path):
                    item_src_full_path = os.path.join(current_src_path, item)
                    item_dst_full_path = os.path.join(current_dst_path, item)

                    if os.path.isdir(item_src_full_path):
                        _apply_alusus_overlay_port_changes(
                            item_src_full_path, item_dst_full_path)
                    elif os.path.isfile(item_src_full_path):
                        if os.path.isdir(item_dst_full_path):
                            shutil.rmtree(item_dst_full_path,
                                          ignore_errors=True)
                        elif os.path.isfile(item_dst_full_path):
                            os.remove(item_dst_full_path)
                        shutil.copyfile(
                            item_src_full_path, item_dst_full_path, follow_symlinks=False)
            _apply_alusus_overlay_port_changes(os.path.join(
                alusus_common.VCPKG_ALUSUS_PORTS_OVERLAY_CHANGES_DIR, package_name), overlay_port_location)

            msg.success_msg("Updating dependency {package_name}'s port overlay.".format(
                package_name=json.dumps(package_name)))

    if alusus_clean_and_build:
        shutil.rmtree(alusus_build_location, ignore_errors=True)
    os.makedirs(alusus_build_location, exist_ok=True)

    # Add the "vcpkg" path in the PATH/Path variables.
    environ = os.environ.copy()
    vcpkg_repo_path = _get_vcpkg_repo_path(environ)
    msg.info_msg("Using vcpkg repo path={}".format(
        json.dumps(vcpkg_repo_path)))
    vcpkg_toolchain_file_path = os.path.join(
        vcpkg_repo_path, "scripts", "buildsystems", "vcpkg.cmake")
    path_env_var = "Path" if sys.platform == "win32" else "PATH"
    path_sep = ";" if sys.platform == "win32" else ":"
    environ[path_env_var] = (vcpkg_repo_path + path_sep + environ[path_env_var]
                             ) if path_env_var in environ else vcpkg_repo_path

    # Set the path to the updated vcpkg ports overlay.
    alusus_vcpkg_ports_overlays_location = os.path.join(
        alusus_build_location, "AlususVcpkgPortsOverlay")
    _restore_ports_overlay(
        alusus_vcpkg_ports_overlays_location, vcpkg_repo_path)

    # Configure the build directory.
    msg.info_msg("Configuring Alusus...")
    cmake_cmd = [
        "cmake", alusus_common.ALUSUS_SOURCES_DIR,
        "-DCMAKE_INSTALL_PREFIX={alusus_install_location}".format(
            alusus_install_location=alusus_install_location),
        "-DCMAKE_BUILD_TYPE={alusus_build_type}".format(
            alusus_build_type=alusus_build_type.value.cmake_build_type),
        "-DCMAKE_TOOLCHAIN_FILE={vcpkg_toolchain_file_path}".format(
            vcpkg_toolchain_file_path=vcpkg_toolchain_file_path),
        "-DVCPKG_MANIFEST_DIR={vcpkg_alusus_manifest_dir}".format(
            vcpkg_alusus_manifest_dir=alusus_common.VCPKG_ALUSUS_MANIFEST_DIR),
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
        cmake_cmd += ["-G", alusus_target_triplet.value.cmake_generator]
    ret = subprocess.run(cmake_cmd, cwd=alusus_build_location, env=environ)
    if ret.returncode:
        raise OSError(ret)

    # Build Alusus.
    msg.info_msg("Building Alusus...")
    cmake_cmd = ["cmake", "--build", alusus_build_location,
                 "--parallel", str(multiprocessing.cpu_count())]
    ret = subprocess.run(cmake_cmd, cwd=alusus_build_location, env=environ)
    if ret.returncode:
        raise OSError(ret)

    # Install Alusus.
    msg.info_msg("Install Alusus...")
    cmake_cmd = ["cmake", "--install", alusus_build_location]
    ret = subprocess.run(cmake_cmd, cwd=alusus_build_location, env=environ)
    if ret.returncode:
        raise OSError(ret)

    # Install STD dependencies.
    if not skip_installing_std_deps:
        msg.info_msg("Install Alusus STD dependencies...")
        libs_to_install = []
        if alusus_target_triplet.value.platform == "linux":
            libs_to_install = [
                ("libcurl.so", "libcurl.so"),
                ("libzip.so", "libzip.so")
            ]
            # TODO: Add more libs to install as needed here.
        elif alusus_target_triplet.value.platform == "darwin":
            libs_to_install = [
                ("libcurl.dylib", "libcurl.dylib"),
                ("libzip.dylib", "libzip.dylib")
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
            if not os.path.isfile(shlib_install_location) or \
                    not (os.path.isfile(shlib_install_location) and filecmp.cmp(shlib_build_location, shlib_install_location)):
                msg.info_msg("Installing STD libname={libname}...".format(
                    libname=json.dumps(dst_libname)))
                if os.path.isdir(shlib_install_location):
                    shutil.rmtree(shlib_install_location, ignore_errors=True)
                elif os.path.isfile(shlib_install_location):
                    os.remove(shlib_install_location)
                shutil.copyfile(shlib_build_location, shlib_install_location)
                msg.success_msg("Installing STD libname={libname}.".format(
                    libname=json.dumps(dst_libname)))
            else:
                msg.info_msg("STD libname={libname} is up to date.".format(
                    libname=json.dumps(dst_libname)))

    msg.success_msg("Building and installing Alusus.")


def parse_cmd_args(args):
    parser = argparse.ArgumentParser(description="Alusus build script")
    parser.add_argument("--build-location", type=str,
                        default=None, help="Alusus build location")
    parser.add_argument("--install-location", type=str,
                        default=None, help="Alusus install location")
    parser.add_argument("--build-type",
                        type=AlususBuildType.from_alusus_build_type_str, default=AlususBuildType.DEBUG, help="Alusus build type")
    parser.add_argument("--target-triplet",
                        type=AlususTargetTriplet.from_alusus_target_triplet_str,
                        default=AlususTargetTriplet.host_default_target_triplet(), help="Alusus target triplet")
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
    parser.add_argument("--print-build-location", action="store_true",
                        help="Prints the build location and exits")
    parser.add_argument("--print-install-location", action="store_true",
                        help="Prints the install location and exits")
    parser.add_argument("--print-packages-location", action="store_true",
                        help="Prints the packages location and exits")
    parser.add_argument("--print-supported-build-types", action="store_true",
                        help="Prints the supported build types list and exits")
    parser.add_argument("--print-supported-target-triplets", action="store_true",
                        help="Prints the supported target triplets list and exits")
    processed_args = parser.parse_args(args)

    # Set the default build, install, and packages paths if no custom paths provided.
    alusus_local_build_path = os.path.join(
        os.path.abspath(os.getcwd()), "AlususBuild")
    target_dirname = "{target_triplet}-{build_type}".format(
        target_triplet=processed_args.target_triplet.value.alusus_target_triplet, build_type=processed_args.build_type.value.alusus_build_type)
    target_dirname = "{sanitized_target_dirname}-{name_hash}".format(
        sanitized_target_dirname=sanitize_filename.sanitize(target_dirname), name_hash=hashlib.sha256(
            target_dirname.encode()).hexdigest()[-8:]
    )
    if not processed_args.build_location:
        processed_args.build_location = os.path.join(
            alusus_local_build_path, "Intermediate", target_dirname)
    if not processed_args.install_location:
        processed_args.install_location = os.path.join(
            alusus_local_build_path, "Install", target_dirname)
    if not processed_args.packages_location:
        processed_args.packages_location = os.path.join(
            alusus_local_build_path, "Packages", target_dirname)

    return processed_args


if __name__ == "__main__":
    init_colorama()
    args = parse_cmd_args(sys.argv[1:])

    # Process print arguments.
    if args.print_build_location:
        print(args.build_location)
        exit(0)
    if args.print_install_location:
        print(args.install_location)
        exit(0)
    if args.print_packages_location:
        print(args.packages_location)
        exit(0)
    if args.print_supported_build_types:
        for target in AlususBuildType.list():
            print(target.value.alusus_build_type)
        exit(0)
    if args.print_supported_target_triplets:
        for target in AlususTargetTriplet.list():
            print(target.value.alusus_target_triplet)
        exit(0)

    build_alusus(args.build_location,
                 args.install_location,
                 args.build_type,
                 args.target_triplet,
                 args.clean_and_build,
                 args.bin_dirname,
                 args.lib_dirname,
                 args.include_dirname,
                 args.skip_installing_std_deps)

    if args.build_packages:
        args.target_triplet.create_packages(
            args.install_location, args.packages_location)
