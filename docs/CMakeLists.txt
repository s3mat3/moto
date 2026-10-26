cmake_minimum_required(VERSION 3.24)

if (NOT (DEFINED DOC_BASE_NAME))
  set(DOC_BASE_NAME "moto-doc")
endif()

if (NOT (DEFINED MOTO_VERSION_STRING))
  set(MOTO_VERSION_STRING "0.0.0")
endif()

project(
  ${DOC_BASE_NAME}_project
  VERSION ${MOTO_VERSION_STRING}
  LANGUAGES NONE
)

## if compile to motoi document 
set(PROJ_DOC_MOTOI 1) 

## if use subdirectory under correct

if (NOT (DEFINED MOTO_BASE_DIR))
  set(MOTO_BASE_DIR     ${PROJECT_SOURCE_DIR})
  set(MOTO_BUILD_DIR    ${PROJECT_BINARY_DIR})
  set(MOTO_DOC_OUT_DIR  ${MOTO_BUILD_DIR}/docs)
  set(MOTO_3RD_BASE     ${MOTO_BASE_DIR}/3rd)
  set(MOTO_INCLUDE_BASE ${MOTO_BASE_DIR}/include)
  set(MOTO_DOC_BASE     ${MOTO_BASE_DIR}/docs)
endif()

if(NOT EXISTS ${MOTO_DOC_OUT_DIR})
  file(MAKE_DIRECTORY ${MOTO_DOC_OUT_DIR})
endif()

find_package(Doxygen)
if(DOXYGEN_FOUND)
  set(DOXYGEN_CONF_FILE "Doxyfile")
  set(DOXYGEN_CONF_PATH   ${MOTO_DOC_BASE}/${DOXYGEN_CONF_FILE})
  set(DOXYGEN_TARGET ${DOC_BASE_NAME})
  CONFIGURE_FILE(${DOXYGEN_CONF_PATH}.in ${DOXYGEN_CONF_PATH})
  ADD_CUSTOM_TARGET(${DOXYGEN_TARGET}
    ${DOXYGEN_EXECUTABLE} ${DOXYGEN_CONF_PATH}
	WORKING_DIRECTORY ${MOTO_DOC_OUT_DIR})
else(DOXYGEN_FOUND)
  message (WARNING "doxygen binary couldn't be found")
endif(DOXYGEN_FOUND)
