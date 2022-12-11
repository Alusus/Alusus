import os

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
