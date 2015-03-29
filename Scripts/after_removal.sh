LINK_DIR=/usr/local/bin
LINK_NAME=alusus_d

if [ -L "${LINK_DIR}/${LINK_NAME}" ]
then
   unlink ${LINK_DIR}/${LINK_NAME}
fi

