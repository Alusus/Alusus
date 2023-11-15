import json
import os
import shutil
import checksum
import sys
import distutils.dir_util

# fmt: off
# Alusus import(s).
sys.path.insert(0, os.path.dirname(__file__))
from git import restore_git_tree_to_path
import common
import msg
# fmt: on


def get_vcpkg_repo_path(environ=os.environ.copy()):
    vcpkg_repo_path = None
    if common.ALUSUS_VCPKG_REPO_PATH_ENV_VAR in environ:
        vcpkg_repo_path = environ[common.ALUSUS_VCPKG_REPO_PATH_ENV_VAR]
    else:
        vcpkg_bin_path = shutil.which("vcpkg")
        if vcpkg_bin_path == None:
            raise OSError(
                "Unable to find vcpkg binary. Make sure it exists in your PATH variable.")
        vcpkg_bin_path = os.path.realpath(vcpkg_bin_path)
        vcpkg_repo_path = os.path.dirname(vcpkg_bin_path)

    return vcpkg_repo_path


def _get_vcpkg_version_from_json_object(json_obj) -> str:
    if "version" in json_obj:
        return json_obj["version"]
    elif "version-semver" in json_obj:
        return json_obj["version-semver"]
    elif "version-date" in json_obj:
        return json_obj["version-date"]
    elif "version-string" in json_obj:
        return json_obj["version-string"]
    else:
        raise NotImplementedError(
            "Unsupported vcpkg versioning scheme.")


def restore_ports_overlays(alusus_vcpkg_ports_overlays_location, vcpkg_repo_path=None, verbose_output=False):
    if vcpkg_repo_path == None:
        vcpkg_repo_path = get_vcpkg_repo_path()
    manifest_file_data = None

    for package_name in os.listdir(common.VCPKG_ALUSUS_PORTS_OVERLAY_DIR):
        overlay_port_location = os.path.join(
            alusus_vcpkg_ports_overlays_location, package_name)

        # Check if the port overlay exists.
        if os.path.exists(os.path.join(overlay_port_location, "DONE")):
            continue

        # Get the package version and port number from the "vcpkg.json" manifest file.
        if manifest_file_data == None:
            with open(os.path.join(common.VCPKG_ALUSUS_MANIFEST_DIR, "vcpkg.json"), "r") as fd:
                manifest_file_data = json.load(fd)
        package_version = None
        package_port_num = None
        for package_obj in manifest_file_data["overrides"]:
            if package_obj["name"] == package_name:
                package_version = _get_vcpkg_version_from_json_object(
                    package_obj)
                package_port_num = 0
                if "port-version" in package_obj:
                    package_port_num = package_obj["port-version"]
                break
        if package_version == None or package_port_num == None:
            raise KeyError("Did not find the version and port number information for package {package_name}.".format(
                package_name=json.dumps(package_name)
            ))

        # Find the Git tree that matches the version and port number in "vcpkg.json" manifest file.
        versions_data = None
        with open(
            os.path.join(
                vcpkg_repo_path,
                "versions",
                "{package_name_first_letter}-".format(
                    package_name_first_letter=package_name[0]),
                "{package_name}.json".format(package_name=package_name)
            ), "r"
        ) as fd:
            versions_data = json.load(fd)
        git_tree_hash = None
        for version_obj in versions_data["versions"]:
            to_compare_package_version = _get_vcpkg_version_from_json_object(
                version_obj)
            to_compare_package_port_num = 0
            if "port-version" in version_obj:
                to_compare_package_port_num = version_obj["port-version"]

            if to_compare_package_version == package_version and to_compare_package_port_num == package_port_num:
                git_tree_hash = version_obj["git-tree"]
                break
        if git_tree_hash == None:
            raise KeyError("Did not find the git tree hash for the specified version and port number for {package_name}.".format(
                package_name=json.dumps(package_name)
            ))

        # Empty current overlay port folder.
        common.remove_path(overlay_port_location, follow_symlinks=False)

        # Restore the upstream port files inside Alusus build directory.
        restore_git_tree_to_path(vcpkg_repo_path, git_tree_hash, overlay_port_location)

        # Apply Alusus port overlay changes to the restored port files.
        distutils.dir_util.copy_tree(
            os.path.join(
                common.VCPKG_ALUSUS_PORTS_OVERLAY_DIR, package_name),
            overlay_port_location, preserve_symlinks=1
        )

        # Add check that the overlay changes are fully written to disk.
        with open(os.path.join(overlay_port_location, "DONE"), "w") as fd:
            fd.write("")

        msg.success_msg("Updating dependency {package_name}'s port overlay.".format(
            package_name=json.dumps(package_name)))
