from alusus_common import ExtendedEnum


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
        for item in AlususBuildType.list():
            if item.value.alusus_build_type == alusus_build_type_str:
                return item
        raise NotImplementedError("Unknown build type.")

    # Alusus build types.
    DEBUG = _AlususToCMakeBuildType("debug", "Debug")
    RELEASE = _AlususToCMakeBuildType("release", "Release")
