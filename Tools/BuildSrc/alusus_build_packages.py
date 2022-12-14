import hashlib
import json
import os
import shutil
import subprocess
import sys
import tarfile
import sanitize_filename

# Alusus imports.
from alusus_git import AlususGitFromRepoPathWithGitBinary
from alusus_target_triplet import AlususTargetTriplet
import alusus_msg
import alusus_common
from alusus_version_info import AlususVersionInfo


def _create_to_be_packaged_dir(alusus_install_location, alusus_packages_location):
    alusus_msg.info_msg("Creating to-be-packaged directory...")
    # Create the to-be-installed directory.
    to_be_packaged_dir = os.path.join(
        alusus_packages_location, ".AlususToBePackaged")
    if sys.platform == "win32":
        to_be_packaged_dir = os.path.join(
            alusus_packages_location, "AlususToBePackaged")
    alusus_common.remove_path(to_be_packaged_dir, follow_symlinks=False)
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


def _create_packages_common(alusus_packages_location, to_be_packaged_dir, package_basename):
    # Build TAR GZ file.
    alusus_msg.info_msg("Building TAR GZ package...")
    with tarfile.open(os.path.join(alusus_packages_location, "{package_basename}.tar.gz".format(package_basename=package_basename)), "w:gz") as tar:
        tar.add(to_be_packaged_dir, arcname=package_basename)


def _generate_fpm_cmd(target_system, version_info, alusus_packages_location, to_be_packaged_dir, machine) -> str:
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
        "-C", to_be_packaged_dir
    ]


def _create_packages_linux(alusus_packages_location, to_be_packaged_dir, package_basename, version_info, machine):
    _create_packages_common(
        alusus_packages_location, to_be_packaged_dir, package_basename)

    err = False

    # Build DEB file.
    alusus_msg.info_msg("Building DEB package...")
    ret = subprocess.run(
        _generate_fpm_cmd(
            "deb", version_info, alusus_packages_location, to_be_packaged_dir, machine),
        cwd=alusus_packages_location
    )
    if ret.returncode:
        # Log error but continue.
        alusus_msg.err_msg("Building DEB package.")
        err = True

    # Build RPM file.
    alusus_msg.info_msg("Building RPM package...")
    ret = subprocess.run(
        _generate_fpm_cmd(
            "rpm", version_info, alusus_packages_location, to_be_packaged_dir, machine),
        cwd=alusus_packages_location
    )
    if ret.returncode:
        # Log error but continue.
        alusus_msg.err_msg("Building RPM package.")
        err = True

    # Build PACMAN file.
    alusus_msg.info_msg("Building PACMAN package...")
    ret = subprocess.run(
        _generate_fpm_cmd(
            "pacman", version_info, alusus_packages_location, to_be_packaged_dir, machine),
        cwd=alusus_packages_location
    )
    if ret.returncode:
        # Log error but continue.
        alusus_msg.err_msg("Building PACMAN package.")
        err = True

    return err


def _create_packages_osx(alusus_packages_location, to_be_packaged_dir, package_basename, version_info, machine):
    _create_packages_common(
        alusus_packages_location, to_be_packaged_dir, package_basename)

    err = False

    # Build OSXPKG file.
    alusus_msg.info_msg("Building OSXPKG package...")
    ret = subprocess.run(
        _generate_fpm_cmd("osxpkg", version_info,
                          alusus_packages_location, to_be_packaged_dir, machine),
        cwd=alusus_packages_location
    )
    if ret.returncode:
        # Log error but continue.
        alusus_msg.err_msg("Building OSXPKG package.")
        err = True

    return err


def _create_packages(alusus_build_packages, alusus_install_location, alusus_packages_location):
    to_be_packaged_dir = _create_to_be_packaged_dir(
        alusus_install_location, alusus_packages_location)
    alusus_packages_location = os.path.join(
        alusus_packages_location, "AlususPackages")
    alusus_common.remove_path(alusus_packages_location, follow_symlinks=False)
    os.makedirs(alusus_packages_location, exist_ok=True)

    version_info = AlususVersionInfo(
        AlususGitFromRepoPathWithGitBinary(alusus_common.ALUSUS_REPO_PATH))
    package_basename = "Alusus-{version}{revision}-{platform}{abi}-{machine}".format(
        version=version_info.version_lossy,
        revision="-" + version_info.revision_lossy if version_info.revision_lossy else "",
        platform=alusus_build_packages.value.alusus_target_triplet.value.platform,
        abi="-" + alusus_build_packages.value.alusus_target_triplet.value.abi if alusus_build_packages.value.alusus_target_triplet.value.abi else "",
        machine=alusus_build_packages.value.alusus_target_triplet.value.machine
    )
    package_basename = "{sanitized_package_basename}-{package_basename_short_hash}".format(
        sanitized_package_basename=sanitize_filename.sanitize(
            package_basename),
        package_basename_short_hash=hashlib.sha256(
            package_basename.encode()).hexdigest()[-8:]
    )

    err = alusus_build_packages.value.create_packages_func(
        alusus_packages_location, to_be_packaged_dir, package_basename, version_info, alusus_build_packages.value.alusus_target_triplet.value.machine)

    alusus_msg.info_msg("Cleaning up to-be-packaged directory...")
    shutil.rmtree(to_be_packaged_dir, ignore_errors=True)

    if err:
        alusus_msg.err_msg("Building packages step has errors.")
    else:
        alusus_msg.success_msg("Building packages.")


class _AlususBuildPackagesWithPackageBuildingFunc:
    def __init__(self, alusus_target_triplet, create_packages_func) -> None:
        self._alusus_target_triplet = alusus_target_triplet
        self._create_packages_func = create_packages_func

    @property
    def alusus_target_triplet(self):
        return self._alusus_target_triplet

    @property
    def create_packages_func(self):
        return self._create_packages_func


class AlususBuildPackages(alusus_common.ExtendedEnum):
    @staticmethod
    def from_alusus_target_triplet(alusus_target_triplet: AlususTargetTriplet):
        for item in AlususBuildPackages.list():
            if item.value.alusus_target_triplet == alusus_target_triplet:
                return item
        raise NotImplementedError(
            "{alusus_target_triplet_class_name} has no matching package building function.".format(
                alusus_target_triplet_class_name=json.dumps(
                    alusus_target_triplet.__class__.__name__)
            ))

    create_packages = _create_packages

    X64_LINUX = _AlususBuildPackagesWithPackageBuildingFunc(
        AlususTargetTriplet.X64_LINUX, _create_packages_linux)
    X64_OSX = _AlususBuildPackagesWithPackageBuildingFunc(
        AlususTargetTriplet.X64_OSX, _create_packages_osx)
