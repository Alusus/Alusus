EXEC_NAME=alusus_d-0.1.1
EXEC_DIR=/opt/alusus
LINK_DIR=/usr/local/bin
LINK_NAME=alusus_d

if [ -L "${LINK_DIR}/${LINK_NAME}" ]
then
   unlink ${LINK_DIR}/${LINK_NAME}
fi

ln -s ${EXEC_DIR}/${EXEC_NAME} ${LINK_DIR}/${LINK_NAME}
