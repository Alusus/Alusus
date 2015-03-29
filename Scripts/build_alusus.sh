BLU_CLR='\033[0;34m'
GRN_CLR='\033[0;32m'
RED_CLR='\033[0;31m'
YEL_CLR='\033[1;33m'
NO_CLR='\033[0m'

# INFORMATION MESSAGE
InformationMessage () {
   echo -e "${BLU_CLR}INFO:${1}${NO_CLR}"
}

# SUCCESS MESSAGE
SuccessMessage () {
   echo -e "${GRN_CLR}DONE:${1}${NO_CLR}"
}

# WARNING MESSAGE
WarnMessage () {
   echo -e "${YEL_CLR}WARN:${1}${NO_CLR}"
}

# ERROR MESSAGE
ErrorMessage () {
   echo -e "${RED_CLR}ERRO:${1}${NO_CLR}"
}


PAR_PROC_NBR=8

LLVM_SRC_NAME="llvm-3.3.src"
LLVM_SRC_URL="http://llvm.org/releases/3.3/llvm-3.3.src.tar.gz"
BOOST_SRC_NAME="boost_1_55_0"
BOOST_SRC_URL="http://sourceforge.net/projects/boost/files/boost/1.55.0/boost_1_55_0.tar.gz"
CATCH_SRC_NAME="CATCH"
CATCH_SRC_URL="http://alusus.net/Downloads/CATCH.tar.gz"

LLVM_BUILD="llvm-build"

mkdir -p ../Builds
mkdir -p ../tmp

cd ../tmp

if [[ ! -e ${LLVM_SRC_NAME} ]]; then
   wget ${LLVM_SRC_URL}
   tar -xvf ${LLVM_SRC_NAME}.tar.gz
else
   InformationMessage "LLVM Sources have been already brought back"
fi

if [[ ! -e ${BOOST_SRC_NAME} ]]; then
   wget ${BOOST_SRC_URL}
   tar -xvf ${BOOST_SRC_NAME}.tar.gz
else
   InformationMessage "BOOST Sources have been already brought back"
fi

if [[ ! -e ${CATCH_SRC_NAME} ]]; then
   wget ${CATCH_SRC_URL}
   tar -xvf ${CATCH_SRC_NAME}.tar.gz
else
   InformationMessage "CATCH have been already brought back"
fi

if [[ ! -e ${LLVM_BUILD} ]]; then
   mkdir ${LLVM_BUILD}
fi

cd ${LLVM_BUILD}
cmake ../${LLVM_SRC_NAME}
make -j${PAR_PROC_NBR}

ALUSUS_ROOT_PATH="/opt/alusus/"
ALUSUS_BUILD_TYPE=debug

pwd

cd ../../Builds

cmake ../Sources -DBOOST_PATH=../tmp/${BOOST_SRC_NAME} -DCATCH_PATH=../tmp/${CATCH_SRC_NAME} -DCMAKE_BUILD_TYPE=${ALUSUS_BUILD_TYPE} -DCMAKE_INSTALL_PREFIX=${ALUSUS_ROOT_PATH} -DLLVM_PATH=../tmp/${LLVM_BUILD}

make -j${PAR_PROC_NBR}

#sudo rm -r ${ALUSUS_ROOT_PATH}

sudo make install

cd ..

sudo cp -r Doc ${ALUSUS_ROOT_PATH}

ALUSUS_PACKAGE="Packages"

if [[ ! -e ${ALUSUS_PACKAGE} ]]; then
   mkdir ${ALUSUS_PACKAGE}
fi

cd ${ALUSUS_PACKAGE}

rm -rf *

DEB_CHANGE_LOG="deb-changelog.en.txt"
RPM_CHANGE_LOG="rpm-changelog.en.txt"
INPUT_TYPE="dir"
ARCHITECTURE="native"

PACKAGE_NAME="alusus"

PACKAGE_VERSION=`cat ../${DEB_CHANGE_LOG} | head -1 | awk '{split($0,a," "); print a[2]}'`
PACKAGE_VERSION=${PACKAGE_VERSION:1:-1}

PACKAGE_DESCRIPTION="alusus is a programming language that is flexible, comprehensive, and decentralized. It s open to all programming methodologies by allowing anyone to upgrade and enhance it without limitations, without permission or coordination with a central team, without needing to rebuild or redistribute the compiler, and most importantly without needing to persuade the users to update their development environments or settings."
PACKAGE_MAINTAINER="hicham OUALI ALAMI<hicham@oualialami.com>"
PACKAGE_URL="http://alusus.net/?lang=en"
AFTER_INSTALL_SCRIPT="after_install.sh"
AFTER_REMOVAL_SCRIPT="after_removal.sh"

fpm -s ${INPUT_TYPE} -t deb -a "${ARCHITECTURE}" -n "${PACKAGE_NAME}" -v "${PACKAGE_VERSION}" --description "${PACKAGE_DESCRIPTION}" --url "${PACKAGE_URL}" -m "${PACKAGE_MAINTAINER}" --deb-changelog "../${DEB_CHANGE_LOG}" --after-install "../Scripts/${AFTER_INSTALL_SCRIPT}" --after-remove "../Scripts/${AFTER_REMOVAL_SCRIPT}" --directories "${ALUSUS_ROOT_PATH}" "${ALUSUS_ROOT_PATH}" 


fpm -s ${INPUT_TYPE} -t rpm -a "${ARCHITECTURE}" -n "${PACKAGE_NAME}" -v "${PACKAGE_VERSION}" --description "${PACKAGE_DESCRIPTION}" --url "${PACKAGE_URL}" -m "${PACKAGE_MAINTAINER}" --after-install "../Scripts/${AFTER_INSTALL_SCRIPT}" --after-remove "../Scripts/${AFTER_REMOVAL_SCRIPT}" --directories "${ALUSUS_ROOT_PATH}" "${ALUSUS_ROOT_PATH}" 


