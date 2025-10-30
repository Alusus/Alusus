import json
import os
import shutil
import sys
import tarfile
import sanitize_filename

# fmt: off
# Alusus import(s).
sys.path.insert(0, os.path.dirname(__file__))
from target_triplet import TargetTriplet
import msg
import common
from version_info import VersionInfo
# fmt: on


def _create_to_be_packaged_dir(alusus_install_location, alusus_packages_location):
    msg.info_msg("Creating to-be-packaged directory...")
    # Create the to-be-installed directory.
    to_be_packaged_dir = os.path.join(
        alusus_packages_location, ".AlususToBePackaged")
    if sys.platform == "win32":
        to_be_packaged_dir = os.path.join(
            alusus_packages_location, "AlususToBePackaged")
    common.remove_path(to_be_packaged_dir, follow_symlinks=False)
    shutil.copytree(alusus_install_location, to_be_packaged_dir,
                    symlinks=True, ignore_dangling_symlinks=True)
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
        os.path.join(common.ALUSUS_REPO_PATH, "Doc"),
        os.path.join(to_be_packaged_dir, "Doc")
    )
    shutil.rmtree(os.path.join(to_be_packaged_dir, "Notices_L18n"),
                  ignore_errors=True)
    shutil.copytree(
        os.path.join(common.ALUSUS_REPO_PATH, "Notices_L18n"),
        os.path.join(to_be_packaged_dir, "Notices_L18n")
    )
    os.makedirs(os.path.join(to_be_packaged_dir, "Tools",
                "Gtk_Syntax_Highlighting"), exist_ok=True)
    shutil.copy2(
        os.path.join(common.ALUSUS_REPO_PATH, "Tools",
                     "Gtk_Syntax_Highlighting", "alusus.lang"),
        os.path.join(to_be_packaged_dir, "Tools",
                     "Gtk_Syntax_Highlighting", "alusus.lang")
    )
    shutil.copy2(
        os.path.join(common.ALUSUS_REPO_PATH, "Tools",
                     "Gtk_Syntax_Highlighting", "alusus_dark_style.xml"),
        os.path.join(to_be_packaged_dir, "Tools",
                     "Gtk_Syntax_Highlighting", "alusus_dark_style.xml")
    )
    shutil.copy2(
        os.path.join(common.ALUSUS_REPO_PATH, "Tools",
                     "Gtk_Syntax_Highlighting", "alusus_light_style.xml"),
        os.path.join(to_be_packaged_dir, "Tools",
                     "Gtk_Syntax_Highlighting", "alusus_light_style.xml")
    )
    shutil.copy2(
        os.path.join(common.ALUSUS_REPO_PATH, "changelog.en.md"),
        os.path.join(to_be_packaged_dir, "changelog.en.md")
    )
    shutil.copy2(
        os.path.join(common.ALUSUS_REPO_PATH, "changelog.ar.md"),
        os.path.join(to_be_packaged_dir, "changelog.ar.md")
    )
    shutil.copy2(
        os.path.join(common.ALUSUS_REPO_PATH, "license.pdf"),
        os.path.join(to_be_packaged_dir, "license.pdf")
    )
    shutil.copy2(
        os.path.join(common.ALUSUS_REPO_PATH, "license.txt"),
        os.path.join(to_be_packaged_dir, "license.txt")
    )

    return to_be_packaged_dir


def _create_packages_common(alusus_packages_location, to_be_packaged_dir, package_basename, verbose_output):
    err = False

    # Build TAR GZ archive file.
    # TAR GZ archive files are not optional.
    msg.info_msg("Building TAR GZ archive file...")
    with tarfile.open(
        os.path.join(alusus_packages_location, "{package_basename}.tar.gz".format(
            package_basename=package_basename)), "w:gz"
    ) as tar:
        tar.add(to_be_packaged_dir, arcname=package_basename)

    # Build ZIP archive file using "7z".
    msg.info_msg("Building ZIP archive file...")
    zip_package_path = os.path.join(alusus_packages_location, "{package_basename}.zip".format(
        package_basename=package_basename))
    to_be_packaged_dir_basename = os.path.basename(to_be_packaged_dir)
    to_be_packaged_dir_dirname = os.path.dirname(to_be_packaged_dir)
    # Build the ZIP archive.
    ret = common.subprocess_run_hidden_except_on_error([
        "7z", "a", "-snl", zip_package_path, to_be_packaged_dir_basename
    ], verbose_output=verbose_output, cwd=to_be_packaged_dir_dirname)
    if ret.returncode:
        # Log error but continue.
        msg.warn_msg("Failed building ZIP archive file.")
        err = True
    if not ret.returncode:
        # Rename the archive name.
        ret = common.subprocess_run_hidden_except_on_error([
            "7z", "rn", "-snl", zip_package_path, to_be_packaged_dir_basename, package_basename
        ], verbose_output=verbose_output,)
        if ret.returncode:
            # Log error but continue.
            msg.warn_msg("Failed building ZIP archive file.")
            err = True

            # Remove the ZIP package.
            try:
                os.remove(zip_package_path)
            except:
                pass

    return err


def _generate_fpm_cmd(target_system, version_info, alusus_packages_location, to_be_packaged_dir, machine) -> str:
    return [
        "fpm", "-s", "dir", "-t", target_system, "-a", machine,
        "-n", common.FPM_PACKAGE_NAME, "-v", version_info.version_lossy,
        "--description", common.FPM_PACKAGE_DESCRIPTION
    ] +\
        ([] if (not version_info.revision_lossy) else ["--iteration", version_info.revision_lossy]) +\
        [
        "--url", common.FPM_PACKAGE_URL, "-m", common.FPM_PACKAGE_MAINTAINER,
        "--after-install", os.path.join(common.ALUSUS_REPO_PATH,
                                        "Tools", "Package_Scripts", "post_install.sh"),
        "--after-remove", os.path.join(common.ALUSUS_REPO_PATH,
                                       "Tools", "Package_Scripts", "post_remove.sh"),
        "-f", "--prefix", os.path.join('/', 'opt', 'Alusus'),
        "-C", to_be_packaged_dir
    ]


def _create_packages_linux(alusus_packages_location, to_be_packaged_dir, package_basename, version_info, machine, verbose_output):
    err = _create_packages_common(
        alusus_packages_location, to_be_packaged_dir, package_basename, verbose_output)

    # Build DEB file.
    msg.info_msg("Building DEB package...")
    ret = common.subprocess_run_hidden_except_on_error(
        _generate_fpm_cmd(
            "deb", version_info, alusus_packages_location, to_be_packaged_dir, machine),
        cwd=alusus_packages_location, verbose_output=verbose_output,
    )
    if ret.returncode:
        # Log error but continue.
        msg.warn_msg("Failed building DEB package.")
        err = True

    # Build RPM file.
    msg.info_msg("Building RPM package...")
    ret = common.subprocess_run_hidden_except_on_error(
        _generate_fpm_cmd(
            "rpm", version_info, alusus_packages_location, to_be_packaged_dir, machine),
        cwd=alusus_packages_location, verbose_output=verbose_output,
    )
    if ret.returncode:
        # Log error but continue.
        msg.warn_msg("Failed building RPM package.")
        err = True

    # Build PACMAN file.
    msg.info_msg("Building PACMAN package...")
    ret = common.subprocess_run_hidden_except_on_error(
        _generate_fpm_cmd(
            "pacman", version_info, alusus_packages_location, to_be_packaged_dir, machine),
        cwd=alusus_packages_location, verbose_output=verbose_output,
    )
    if ret.returncode:
        # Log error but continue.
        msg.warn_msg("Failed building PACMAN package.")
        err = True

    return err


def _create_packages_osx(alusus_packages_location, to_be_packaged_dir, package_basename, version_info, machine, verbose_output):
    err = _create_packages_common(
        alusus_packages_location, to_be_packaged_dir, package_basename, verbose_output)

    # Build OSXPKG file.
    msg.info_msg("Building OSXPKG package...")
    ret = common.subprocess_run_hidden_except_on_error(
        _generate_fpm_cmd("osxpkg", version_info,
                          alusus_packages_location, to_be_packaged_dir, machine),
        cwd=alusus_packages_location, verbose_output=verbose_output,
    )
    if ret.returncode:
        # Log error but continue.
        msg.warn_msg("Failed building OSXPKG package.")
        err = True

    return err


def create_packages(alusus_build_packages, alusus_install_location, alusus_packages_location, verbose_output=False):
    to_be_packaged_dir = _create_to_be_packaged_dir(
        alusus_install_location, alusus_packages_location)

    version_info = VersionInfo(
        common.ALUSUS_REPO_PATH, verbose_output=verbose_output)
    package_basename = sanitize_filename.sanitize("alusus-{version}{revision}-{machine}-{platform}{abi}".format(
        version=version_info.version_lossy,
        revision="-" + version_info.revision_lossy if version_info.revision_lossy else "",
        machine=alusus_build_packages.value.alusus_target_triplet.value.machine,
        platform=alusus_build_packages.value.alusus_target_triplet.value.platform,
        abi="-" + alusus_build_packages.value.alusus_target_triplet.value.abi if alusus_build_packages.value.alusus_target_triplet.value.abi else ""
    ))

    err = alusus_build_packages.value.create_packages_func(
        alusus_packages_location, to_be_packaged_dir, package_basename, version_info, alusus_build_packages.value.alusus_target_triplet.value.machine,
        verbose_output)

    msg.info_msg("Cleaning up to-be-packaged directory...")
    shutil.rmtree(to_be_packaged_dir, ignore_errors=True)

    if err:
        msg.warn_msg("Some optional packages failed to build.")
    else:
        msg.success_msg("Building packages.")


class _BuildPackagesWithPackageBuildingFunc:
    def __init__(self, alusus_target_triplet, create_packages_func) -> None:
        self._alusus_target_triplet = alusus_target_triplet
        self._create_packages_func = create_packages_func

    @property
    def alusus_target_triplet(self):
        return self._alusus_target_triplet

    @property
    def create_packages_func(self):
        return self._create_packages_func


class BuildPackages(common.ExtendedEnum):
    @staticmethod
    def from_alusus_target_triplet(alusus_target_triplet: TargetTriplet):
        for item in BuildPackages.list():
            if item.value.alusus_target_triplet == alusus_target_triplet:
                return item
        raise NotImplementedError(
            "{alusus_target_triplet_class_name} has no matching package building function.".format(
                alusus_target_triplet_class_name=json.dumps(
                    alusus_target_triplet.__class__.__name__)
            ))

    X64_LINUX = _BuildPackagesWithPackageBuildingFunc(
        TargetTriplet.X64_LINUX, _create_packages_linux)
    ARM64_OSX = _BuildPackagesWithPackageBuildingFunc(
        TargetTriplet.ARM64_OSX, _create_packages_osx)
