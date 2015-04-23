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

cd "$( dirname "${BASH_SOURCE[0]}" )"

RELEASE_STRING=`git describe --tags 2> /dev/null` || {
  echo "VERSION: 0.0.0"
  echo "REVISION: DEV"
  DATE=`date +%Y-%m-%d`
  HIJRI_DATE=`$PWD/Helpers/greg_to_hijri.php $DATE`
  echo "DATE: $DATE"
  echo "HIJRI_DATE: $HIJRI_DATE"
  exit;
}
CURRENT_BRANCH=`git rev-parse --abbrev-ref HEAD`

VERSION=`echo $RELEASE_STRING | awk 'match($0, /v([0-9.]+)/, a) { print a[1] }'`
DISTANCE=`echo $RELEASE_STRING | awk 'match($0, /-([0-9]+)-g/, a) { print a[1]+1 }'`
COMMIT=`echo $RELEASE_STRING | awk 'match($0, /-g([0-9a-z]+)$/, a) { print a[1] }'`

DATE=`date -d "$(TZ=UTC git show -s --date=local --format=%cd $COMMIT)" +%Y-%m-%d`
HIJRI_DATE=`Helpers/greg_to_hijri.php $DATE`

echo "VERSION: $VERSION"
if [[ $DISTANCE != "" ]]; then
  if [[ $CURRENT_BRANCH == "rb$VERSION" ]]; then
    echo "REVISION: $DISTANCE"
  else
    echo "REVISION: GIT$COMMIT"
  fi
fi

echo "DATE: $DATE"
echo "HIJRI_DATE: $HIJRI_DATE"

