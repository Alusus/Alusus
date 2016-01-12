#!/bin/bash

# Dependencies
LLVM_SRC_URL="http://llvm.org/releases/3.3/llvm-3.3.src.tar.gz"
LLVM_NAME="llvm-3.3"
BOOST_SRC_URL="http://sourceforge.net/projects/boost/files/boost/1.55.0/boost_1_55_0.tar.gz"
BOOST_SRC_NAME="boost_1_55_0"
CATCH_SRC_URL="https://github.com/philsquared/Catch/archive/v1.3.0.tar.gz"
CATCH_SRC_NAME="Catch-1.3.0"

# Paths
ALUSUS_ROOT="$(pwd)/.."
INSTALL_PATH="$(pwd)/.."
RELEASE_INSTALL_PATH="/opt/Alusus"
BUILDS_PATH="$(pwd)/../Builds"
DEPS_PATH="$(pwd)/../Builds/Dependencies"
PACKAGES_PATH="$(pwd)/../Builds/Packages"

# Build Args
MAKE_THREAD_COUNT=8
BUILD_TYPE="debug"

# Package Creation Args.
VERSION_FILE="version_info.sh"
CREATE_PACKAGES="no"
ARCHITECTURE="native"
PACKAGE_NAME="alusus"
PACKAGE_DESCRIPTION="Alusus Programming Language's core compilation system and standard libraries."
PACKAGE_MAINTAINER="hicham OUALI ALAMI<hicham@oualialami.com>"
PACKAGE_URL="http://alusus.net"
AFTER_INSTALL_SCRIPT="post_install.sh"
AFTER_REMOVAL_SCRIPT="post_remove.sh"


#------------------------
# Command Line Processing

for var in "$@"
do
  if [[ $var == "--help" ]]; then
    echo "Options:"
    echo "-d: Use 'debug' build type. This is the default."
    echo "-r: Use 'release' build type."
    echo "-p: Create release packages after the build."
    exit
  elif [[ $var == "-d" ]]; then
    BUILD_TYPE="debug"
  elif [[ $var == "-r" ]]; then
    BUILD_TYPE="release"
  elif [[ $var == "-p" ]]; then
    BUILD_TYPE="release"
    CREATE_PACKAGES="yes"
    INSTALL_PATH="$RELEASE_INSTALL_PATH"
    sudo rm -rf $INSTALL_PATH/*
  fi
done

BUILD_PATH=$BUILDS_PATH/$BUILD_TYPE


#---------------------------
# Message Printing Functions

# Color Constants
BLU_CLR='\033[0;34m'
GRN_CLR='\033[0;32m'
RED_CLR='\033[0;31m'
YEL_CLR='\033[1;33m'
NO_CLR='\033[0m'

# INFORMATION MESSAGE
InformationMessage () {
   echo -e "${BLU_CLR}INFO: ${1}${NO_CLR}"
}

# SUCCESS MESSAGE
SuccessMessage () {
   echo -e "${GRN_CLR}DONE: ${1}${NO_CLR}"
}

# WARNING MESSAGE
WarnMessage () {
   echo -e "${YEL_CLR}WARNING: ${1}${NO_CLR}"
}

# ERROR MESSAGE
ErrorMessage () {
   echo -e "${RED_CLR}ERROR: ${1}${NO_CLR}"
}

# FAIL MESSAGE
FailMessage () {
   echo -e "${RED_CLR}FAILED: ${1}${NO_CLR}"
}


#-------------------------
# Dependencies Preparation

InformationMessage "Preparing dependencies..."

mkdir -p $BUILD_PATH
mkdir -p $DEPS_PATH

cd $DEPS_PATH

if [[ ! -e "${LLVM_NAME}.src" ]]; then
  wget ${LLVM_SRC_URL}
  tar -xvf ${LLVM_NAME}.src.tar.gz
else
  InformationMessage "LLVM sources is already available."
fi

if [[ ! -e ${BOOST_SRC_NAME} ]]; then
  wget ${BOOST_SRC_URL}
  tar -xvf ${BOOST_SRC_NAME}.tar.gz
else
  InformationMessage "BOOST sources is already available."
fi

if [[ ! -e ${CATCH_SRC_NAME} ]]; then
  wget ${CATCH_SRC_URL}
  tar -xvf $(basename ${CATCH_SRC_URL})
else
  InformationMessage "CATCH is already available."
fi

if [[ ! -e ${LLVM_NAME}.install ]]; then
  mkdir ${LLVM_NAME}.install
  if [[ ! -e ${LLVM_NAME}.build ]]; then
    mkdir ${LLVM_NAME}.build
  fi
  cd ${LLVM_NAME}.build
  cmake ../${LLVM_NAME}.src -DCMAKE_INSTALL_PREFIX=$DEPS_PATH/${LLVM_NAME}.install
  make -j${MAKE_THREAD_COUNT} || { FailMessage "Building LLVM."; exit 1; }
  make install
else
  InformationMessage "LLVM is already built."
fi

SuccessMessage "Preparing dependencies."


#----------------
# Alusus Building

InformationMessage "Building Alusus..."

cd $BUILD_PATH

cmake $ALUSUS_ROOT/Sources -DBOOST_PATH=$DEPS_PATH/$BOOST_SRC_NAME -DCATCH_PATH=$DEPS_PATH/$CATCH_SRC_NAME/single_include -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DCMAKE_INSTALL_PREFIX=${INSTALL_PATH} -DLLVM_PATH=$DEPS_PATH/${LLVM_NAME}.install

make -j${MAKE_THREAD_COUNT} || { FailMessage "Building Alusus."; exit 1; }

if [[ $CREATE_PACKAGES == "no" ]]; then
  make install
fi

SuccessMessage "Building Alusus."

if [[ $CREATE_PACKAGES == "no" ]]; then
  cd $ALUSUS_ROOT/Tools
  exit
fi


#-------------------------------
# Installation Packages Creation

InformationMessage "Creating installation packages..."

sudo make install

cd $ALUSUS_ROOT

sudo cp -r Doc ${INSTALL_PATH}/
sudo mkdir -p ${INSTALL_PATH}/Tools/Gtk_Syntax_Highlighting
sudo cp Tools/Gtk_Syntax_Highlighting/alusus.lang ${INSTALL_PATH}/Tools/Gtk_Syntax_Highlighting/
sudo cp changelog.en.txt ${INSTALL_PATH}/
sudo cp changelog.ar.txt ${INSTALL_PATH}/
sudo cp license.pdf ${INSTALL_PATH}/
sudo cp license.txt ${INSTALL_PATH}/

if [[ ! -e ${PACKAGES_PATH} ]]; then
   mkdir ${PACKAGES_PATH}
fi

cd ${PACKAGES_PATH}

rm -rf *

INPUT_TYPE="dir"

VERSION_INFO=`$ALUSUS_ROOT/Tools/$VERSION_FILE`
PACKAGE_VERSION=`echo $VERSION_INFO | awk 'match($0, /VERSION: ([0-9.]+)/, a) { print a[1] }'`
PACKAGE_REVISION=`echo $VERSION_INFO | awk 'match($0, /REVISION: ([a-zA-Z0-9]+)/, a) { print a[1] }'`
if [[ $PACKAGE_REVISION != "" ]]; then
  PACKAGE_REVISION="--iteration ${PACKAGE_REVISION}"
fi

fpm -s ${INPUT_TYPE} -t deb -a "${ARCHITECTURE}" -n "${PACKAGE_NAME}" -v "${PACKAGE_VERSION}" ${PACKAGE_REVISION} --description "${PACKAGE_DESCRIPTION}" --url "${PACKAGE_URL}" -m "${PACKAGE_MAINTAINER}" --after-install "$ALUSUS_ROOT/Tools/Package_Scripts/${AFTER_INSTALL_SCRIPT}" --after-remove "$ALUSUS_ROOT/Tools/Package_Scripts/${AFTER_REMOVAL_SCRIPT}" --directories "${INSTALL_PATH}" "${INSTALL_PATH}" || { FailMessage "Creating DEB Package."; exit 1; }

fpm -s ${INPUT_TYPE} -t rpm -a "${ARCHITECTURE}" -n "${PACKAGE_NAME}" -v "${PACKAGE_VERSION}" ${PACKAGE_REVISION} --description "${PACKAGE_DESCRIPTION}" --url "${PACKAGE_URL}" -m "${PACKAGE_MAINTAINER}" --after-install "$ALUSUS_ROOT/Tools/Package_Scripts/${AFTER_INSTALL_SCRIPT}" --after-remove "$ALUSUS_ROOT/Tools/Package_Scripts/${AFTER_REMOVAL_SCRIPT}" --directories "${INSTALL_PATH}" "${INSTALL_PATH}" || { FailMessage "Creating RPM Package."; exit 1; }

SuccessMessage "Creating installation packages."

