from enum import Enum
import os
import shutil
import sys

ALUSUS_REPO_PATH = os.path.dirname(os.path.dirname(
    os.path.dirname(os.path.realpath(__file__))))
ALUSUS_SOURCES_DIR = os.path.join(ALUSUS_REPO_PATH, "Sources")
ALUSUS_TOOLS_DIR = os.path.join(ALUSUS_REPO_PATH, "Tools")
VCPKG_ALUSUS_DIR = os.path.join(ALUSUS_REPO_PATH, "Tools", "vcpkg")
VCPKG_ALUSUS_MANIFEST_DIR = os.path.join(VCPKG_ALUSUS_DIR, "Manifest")
VCPKG_ALUSUS_PORTS_OVERLAY_DIR = os.path.join(VCPKG_ALUSUS_DIR, "PortsOverlay")
VCPKG_ALUSUS_PORTS_OVERLAY_CHANGES_DIR = os.path.join(
    VCPKG_ALUSUS_PORTS_OVERLAY_DIR, "Changes")
VCPKG_ALUSUS_PORTS_OVERLAY_HASHES_DIR = os.path.join(
    VCPKG_ALUSUS_PORTS_OVERLAY_DIR, "Hashes")
FPM_PACKAGE_NAME = "alusus"
FPM_PACKAGE_DESCRIPTION = "Alusus Programming Language's core compilation system and standard libraries."
FPM_PACKAGE_MAINTAINER = "Sarmad Khalid Abdullah <sarmad@alusus.org>"
FPM_PACKAGE_URL = "https://alusus.org"
ALUSUS_VCPKG_REPO_PATH_ENV_VAR = "ALUSUS_VCPKG_REPO_PATH"


class ExtendedEnum(Enum):
    @classmethod
    def list(cls):
        for _ in cls:
            return list(map(lambda c: c, cls))


def remove_path(path, follow_symlinks=True):
    if not follow_symlinks and os.path.islink(path):
        os.unlink(path)
    elif os.path.isdir(path):
        shutil.rmtree(path, ignore_errors=True)
    elif os.path.isfile(path):
        os.remove(path)


def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)


class CopyIfDifferentReturnType(ExtendedEnum):
    NoSrc = 1
    DstUpToDate = 2
    DstUpdated = 3


def copy_if_different(src, dst, follow_src_symlinks=True, follow_dst_symlinks=True, hash_mode="sha256") -> CopyIfDifferentReturnType:
    import checksum

    class PathType(ExtendedEnum):
        FILE = 1
        DIR = 2
        LINK = 3

    src_type = None
    if os.path.islink(src) and not follow_src_symlinks:
        src_type = PathType.LINK
    elif os.path.isdir(src):
        src_type = PathType.DIR
    elif os.path.isfile(src):
        src_type = PathType.FILE

    # Source doesn't exist.
    if src_type == None:
        return CopyIfDifferentReturnType.NoSrc

    dst_type = None
    if os.path.islink(dst) and not follow_dst_symlinks:
        dst_type = PathType.LINK
    elif os.path.isdir(dst):
        dst_type = PathType.DIR
    elif os.path.isfile(dst):
        dst_type = PathType.FILE

    different = False
    if (src_type != dst_type) or\
       (src_type == PathType.LINK) or\
       (src_type == PathType.FILE and
        checksum.get_for_file(src, hash_mode=hash_mode) != checksum.get_for_file(dst, hash_mode=hash_mode)) or\
       (src_type == PathType.DIR and
            checksum.get_for_directory(src, hash_mode=hash_mode) != checksum.get_for_directory(dst, hash_mode=hash_mode)):
        different = True
    # Source is no different from the destination. Do nothing.
    if not different:
        return CopyIfDifferentReturnType.DstUpToDate

    remove_path(dst, follow_symlinks=follow_dst_symlinks)
    if not follow_src_symlinks and src_type == PathType.LINK:
        os.link(os.readlink(src), dst)
    elif src_type == PathType.DIR:
        shutil.copytree(src, dst)
    elif src_type == PathType.FILE:
        shutil.copy(src, dst)

    return CopyIfDifferentReturnType.DstUpdated
