# - Try to find OpenBabel2
# Once done this will define
#
#  OPENBABEL2_FOUND - system has OpenBabel2
#  OPENBABEL2_INCLUDE_DIR - the OpenBabel2 include directory
#  OPENBABEL2_LIBRARIES - Link these to use OpenBabel2

if (OPENBABEL2_INCLUDE_DIR AND OPENBABEL2_LIBRARIES)

  # in cache already
  set(OPENBABEL2_FOUND TRUE)

else (OPENBABEL2_INCLUDE_DIR AND OPENBABEL2_LIBRARIES)

  include(UsePkgConfig)

  pkgconfig(openbabel-2.0 _obIncDir _obLinkDir _obLinkFlags _obCflags)

  # query pkg-config asking for a openbabel >= 2.1.0
  exec_program(${PKGCONFIG_EXECUTABLE} ARGS --atleast-version=2.1.0 openbabel-2.0 RETURN_VALUE _return_VALUE OUTPUT_VARIABLE _pkgconfigDevNull )
  if(_return_VALUE STREQUAL "0")
	set(OPENBABEL_MINI_FOUND TRUE)
  endif(_return_VALUE STREQUAL "0")
  message(STATUS "OPENBABEL_MINI_FOUND <${OPENBABEL_MINI_FOUND}>")

  exec_program(${PKGCONFIG_EXECUTABLE} ARGS --variable=pkgincludedir openbabel-2.0 RETURN_VALUE _return_VALUE OUTPUT_VARIABLE _obPkgIncDir )
  if (_obPkgIncDir)
    set(_obIncDir "${_obPkgIncDir}")
  endif (_obPkgIncDir)

  find_path(OPENBABEL2_INCLUDE_DIR openbabel/obconversion.h
    ${_obIncDir}
    /usr/local/include
    /usr/include
    ${GNUWIN32_DIR}/include
  )

  find_library(OPENBABEL2_LIBRARIES NAMES openbabel
    PATHS
    ${_obLinkDir}
    /usr/lib
    /usr/local/lib
    ${GNUWIN32_DIR}/lib
  )

  if(OPENBABEL2_INCLUDE_DIR AND OPENBABEL2_LIBRARIES AND OPENBABEL_MINI_FOUND)
    set(OPENBABEL2_FOUND TRUE)
  endif(OPENBABEL2_INCLUDE_DIR AND OPENBABEL2_LIBRARIES AND OPENBABEL_MINI_FOUND)

  if (OPENBABEL2_FOUND)
    if (NOT OpenBabel2_FIND_QUIETLY)
      message(STATUS "Found OpenBabel2: ${OPENBABEL2_LIBRARIES}")
    endif (NOT OpenBabel2_FIND_QUIETLY)
  else (OPENBABEL2_FOUND)
	if( NOT OPENBABEL_MINI_FOUND)
	  message(STATUS "openBabel >=0.21 requires")
	endif(NOT OPENBABEL_MINI_FOUND)
    if (OpenBabel2_FIND_REQUIRED)
      message(FATAL_ERROR "Could NOT find OpenBabel2")
    endif (OpenBabel2_FIND_REQUIRED)
  endif (OPENBABEL2_FOUND)

  mark_as_advanced(OPENBABEL2_INCLUDE_DIR OPENBABEL2_LIBRARIES)

endif (OPENBABEL2_INCLUDE_DIR AND OPENBABEL2_LIBRARIES)
