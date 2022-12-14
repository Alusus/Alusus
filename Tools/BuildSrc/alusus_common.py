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


def remove_path(path):
    if os.path.isdir(path):
        shutil.rmtree(path, ignore_errors=True)
    elif os.path.isfile(path):
        os.remove(path)


def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)
