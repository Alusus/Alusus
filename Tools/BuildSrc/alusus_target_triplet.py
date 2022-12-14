import platform
import shutil
import sys

# Alusus imports.
from alusus_common import ExtendedEnum


class _AlususCombinedTargetTriplet:
    def __init__(self,
                 alusus_target_triplet: str,
                 vcpkg_target_triplet: str,
                 platform: str,
                 machine: str,
                 abi=None,
                 cmake_generator=None) -> None:
        self._alusus_target_triplet = alusus_target_triplet
        self._vcpkg_target_triplet = vcpkg_target_triplet
        self._platform = platform
        self._machine = machine
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


def _get_unix_cmake_generator():
    # Prefer using "ninja" over "make" if available in Unix operating systems.
    if shutil.which("ninja"):
        return "Ninja"
    else:
        return "Unix Makefiles"


class AlususTargetTriplet(ExtendedEnum):
    @staticmethod
    def host_default_target_triplet():
        for item in AlususTargetTriplet.list():
            if item.value.platform == sys.platform and item.value.machine == platform.machine():
                return item
        raise NotImplementedError("Current host has no matching build target.")

    @staticmethod
    def from_alusus_target_triplet_str(alusus_target_triplet_str: str):
        for item in AlususTargetTriplet.list():
            if item.value.alusus_target_triplet == alusus_target_triplet_str:
                return item
        raise NotImplementedError("Unsupported target triplet.")

    # Target triplets we support building for.
    X64_LINUX = _AlususCombinedTargetTriplet(
        "x64-linux", "x64-linux-release", "linux", "x86_64", abi="gnu", cmake_generator=_get_unix_cmake_generator())
    X64_OSX = _AlususCombinedTargetTriplet(
        "x64-osx", "x64-osx-release", "darwin", "x86_64", cmake_generator=_get_unix_cmake_generator())
    # TODO: Add more targets here when we add support building for.
