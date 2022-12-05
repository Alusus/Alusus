# This Python script extracts version information from Git using tag and branch names.
# It looks for the latest tag reachable from the current commit and uses the
# version number in that tag. The tag name must be in this format: v1.0.0
# If the current commit is on the tag then no revision will be printed,
# otherwise the following criteria is followed in determining the revision:
# * If the current branch's name starts with 'vb' followed by the version
#   number the revision will be a number equivalent to the number of commits
#   on top of that tag.
# * Otherwise the revision is 'GIT' followed by the first 6 letters of the
#   current commit's SHA.
# The date of the release will be equivalent to the date of the current Git
# commit in UTC.

# Note:
#
# This script requires "pygit2" Pip package for Git operations and "ummalqura" Pip
# package for the Islamic date.


import os
import datetime
try:
    import ummalqura.hijri_date as ummalqura_hijri_date
except ImportError:
    ummalqura_hijri_date = None
try:
    import pygit2
except ImportError:
    pygit2 = None


_ALUSUS_REPO_PATH = os.path.dirname(
    os.path.dirname(os.path.realpath(__file__)))


class AlususVersionInfo:
    def __init__(self, alusus_repo) -> None:
        self.update_version_info(alusus_repo)

    def update_version_info(self, alusus_repo):
        self._version = None
        self._revision = None
        now = datetime.datetime.now()
        self._date = "{0}-{1}-{2}".format(now.year, str(
            now.month).rjust(2, '0'), str(now.day).rjust(2, '0'))
        self._hijri_date = None

        if alusus_repo and pygit2 and isinstance(alusus_repo, pygit2.Repository):
            try:
                # Use "pygit2" to get the Git information.
                release_string = alusus_repo.describe(
                    describe_strategy=pygit2.GIT_DESCRIBE_TAGS)
                current_repo = alusus_repo.head.shorthand

                # Parse the version and the revision.
                self._version = release_string.split("-")[0][1:]
                if release_string.find("-") != -1:
                    if current_repo == "vb{}".format(self._version):
                        self._revision = str(
                            int(release_string.split("-")[1]) + 1)
                    else:
                        self._revision = "GIT{}".format(
                            release_string.split("-")[2][1:])
            except pygit2.GitError:
                pass

        # Set the Islamic date.
        if ummalqura_hijri_date:
            islamic_date_now = ummalqura_hijri_date.HijriDate(
                now.year, now.month, now.day, gr=True)
            self._hijri_date = "{0}-{1}-{2}".format(int(islamic_date_now.year), str(int(
                islamic_date_now.month)).rjust(2, '0'), str(int(islamic_date_now.day)).rjust(2, '0'))

    @property
    def version(self):
        return self._version

    @property
    def revision(self):
        return self._revision

    @property
    def date(self):
        return self._date

    @property
    def hijri_date(self):
        return self._hijri_date


def print_version_info(alusus_repo):
    version_info = AlususVersionInfo(alusus_repo)
    print("VERSION: {}".format(
        version_info.version if version_info.version else "0.0.0"))
    print("REVISION: {}".format(
        version_info.revision if version_info.revision else "DEV"))
    print("DATE: {}".format(version_info.date))
    print("HIJRI_DATE: {}".format(
        version_info.hijri_date if version_info.hijri_date else "N/A"))


if __name__ == "__main__":
    alusus_repo = None
    try:
        alusus_repo = pygit2.Repository(
            path=_ALUSUS_REPO_PATH) if pygit2 else None
    except pygit2.GitError:
        pass
    print_version_info(alusus_repo)
