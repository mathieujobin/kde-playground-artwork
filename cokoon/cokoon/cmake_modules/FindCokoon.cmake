if (COKOON_INCLUDE_DIR AND COKOON_LIBS AND COKOON_SPEC_EXECUTABLE)
    # Already in cache, be silent
    set(Cokoon_FIND_QUIETLY TRUE)
endif (COKOON_INCLUDE_DIR AND COKOON_LIBS AND COKOON_SPEC_EXECUTABLE)

set(COKOON_INCLUDE_DIR)
set(COKOON_LIBS)
set(COKOON_SPEC_EXECUTABLE)

# when building cokoon, make the cokoon_spec rules depend on the binary...
set( _COKOONSPECIFICATION_COMPILER_DEP cokoonspec_compiler)

if (BOOTSTRAP_COKOON)
   set(COKOON_INCLUDE_DIR ${CMAKE_SOURCE_DIR})
   set(COKOON_LIBS cokoon)
   set(COKOON_SPEC_EXECUTABLE "${CMAKE_BINARY_DIR}/cokoon/cokoonspec_compiler")
else (BOOTSTRAP_COKOON)
   find_path(COKOON_INCLUDE_DIR NAMES cokoon/document.h cokoon/bridges.h
             PATHS
             ${CMAKE_SOURCE_DIR} ${INCLUDE_INSTALL_DIR} )


   find_library(COKOON_LIBS NAMES cokoon
                PATHS
                ${LIB_INSTALL_DIR})

   find_program(COKOON_SPEC_EXECUTABLE NAME cokoonspec_compiler
          PATHS "${CMAKE_BINARY_DIR}/cokoon" ${BIN_INSTALL_DIR} ) # disabled NO_DEFAULT_PATH
endif (BOOTSTRAP_COKOON)

message( STATUS "inc/lib/exec ${COKOON_LIBS} ${COKOON_INCLUDE_DIR} ${COKOON_SPEC_EXECUTABLE}")
if (COKOON_LIBS AND COKOON_INCLUDE_DIR AND COKOON_SPEC_EXECUTABLE)
    set(COKOON_FOUND TRUE)
endif (COKOON_LIBS AND COKOON_INCLUDE_DIR AND COKOON_SPEC_EXECUTABLE)

if (COKOON_FOUND)
#    if (NOT Cokoon_FIND_QUIETLY)
    message(STATUS "Found Cokoon: ${COKOON_LIBS}")
#    endif (NOT Cokoon_FIND_QUIETLY)
else (COKOON_FOUND)
    if (Cokoon_FIND_REQUIRED)
    message(FATAL_ERROR "Could NOT find Cokoon")
    endif (Cokoon_FIND_REQUIRED)
endif (COKOON_FOUND)

mark_as_advanced(COKOON_INCLUDE_DIR COKOON_LIBS COKOON_SPEC_EXECUTABLE)


macro (COKOON_ADD_SPEC_FILES _sources )

   foreach (_current_FILE ${ARGN})
       get_filename_component(_tmp_FILE ${_current_FILE} ABSOLUTE)
       get_filename_component(_basename ${_tmp_FILE} NAME_WE)

       set(_header_FILE ${CMAKE_CURRENT_BINARY_DIR}/${_basename}.h)

       add_custom_command(OUTPUT ${_header_FILE}
          COMMAND ${COKOON_SPEC_EXECUTABLE}
          ARGS ${_tmp_FILE} ${_header_FILE}
          MAIN_DEPENDENCY ${_tmp_FILE}
          DEPENDS ${_COKOONSPECIFICATION_COMPILER_DEP} )

       list(APPEND ${_sources} ${_header_FILE})
   endforeach (_current_FILE)

endmacro (COKOON_ADD_SPEC_FILES)

