#!/bin/bash
# This file extracts version information from GIT using tag and branch names.
# It looks for the latest tag reachable from the current commit and uses the
# version number in that tag. The tag name must be in this format: v1.0.0
# If the current commit is on the tag then no revision will be printed,
# otherwise the following criteria is followed in determining the revision:
# * If the current branch's name starts with 'rb' followed by the version
#   number the revision will be a number equivalent to the number of commits
#   on top of that tag.
# * Otherwise the revision is 'GIT' followed by the first 6 letters of the
#   current commit's SHA.
# The date of the release will be equivalent to the date of the current GIT
# commit in UTC.


import os
import subprocess
import datetime
try:
    import install_dep
except ImportError:
    import sys
    sys.path.append(os.path.dirname(os.path.realpath(__file__)))
    import install_dep
install_dep.install_package('ummalqura'); from ummalqura.hijri_date import HijriDate


def get_version_info():
    original_path = os.path.realpath(os.getcwd())
    os.chdir(os.path.dirname(os.path.realpath(__file__)))
    release_string = None
    version = None
    revision = None
    date = None
    hijri_date = None

    try:
        release_string = subprocess.check_output("git describe --tags".split()).decode("utf-8").rstrip()
        version = release_string.split("-")[0][1:]
        distance = str(int(release_string.split("-")[1]) + 1)
        commit = release_string.split("-")[2][1:]
        if distance:
            current_branch = subprocess.check_output("git rev-parse --abbrev-ref HEAD".split()).decode("utf-8").rstrip()
            if current_branch == "vb{}".format(version):
                revision = distance
            else:
                revision = "GIT{}".format(commit)

    except (FileNotFoundError, subprocess.CalledProcessError):
        version = "0.0.0"
        revision = "DEV"
        
    now = datetime.datetime.now()
    date = "{0}-{1}-{2}".format(now.year, str(now.month).rjust(2, '0'), str(now.day).rjust(2, '0'))
    islamic_date_now = HijriDate(now.year, now.month, now.day, gr=True)
    hijri_date = "{0}-{1}-{2}".format(int(islamic_date_now.year), str(int(islamic_date_now.month)).rjust(2, '0'), str(int(islamic_date_now.day)).rjust(2, '0'))
    os.chdir(original_path)
    return version, revision, date, hijri_date


def print_version_info():
    version, revision, date, hijri_date = get_version_info()
    print("VERSION: {}".format(version))
    print("REVISION: {}".format(revision))
    print("DATE: {}".format(date))
    print("HIJRI_DATE: {}".format(hijri_date))


if __name__ == "__main__":
    print_version_info()
