
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

FIND_PROGRAM(SVN_CMD "svn")
FIND_PROGRAM(MAKE_CMD "make")
FIND_PROGRAM(CMAKE_CMD "cmake")
FIND_PROGRAM(CTEST_CMD "ctest")

#=============================================================================
# FUNCTION:    CLEAN_SVN_CHECKOUT
# DESCRIPTION: Performs a clean svn checkout, which removes the LOCAL_DIR if
#              it exists.
#=============================================================================
FUNCTION( SVN_CLEAN_CHECKOUT REPO_LOCATION LOCAL_DIR )

   IF("${LOCAL_DIR}" STREQUAL "")
      MESSAGE(FATAL_ERROR 
         "Error! SVN_CLEAN_CHECKOUT must specify a local directory!")
   ENDIF("${LOCAL_DIR}" STREQUAL "")

   MESSAGE( "Checking out ${REPO_LOCATION}")
   IF( EXISTS ${LOCAL_DIR} )
      FILE( REMOVE_RECURSE ${LOCAL_DIR} )
      MESSAGE( "Cleaning ${LOCAL_DIR}")
   ENDIF( EXISTS ${LOCAL_DIR} )

   EXECUTE_PROCESS(COMMAND ${SVN_CMD} "checkout" ${REPO_LOCATION} ${LOCAL_DIR}
                   RESULT_VARIABLE M_STATUS
                   OUTPUT_VARIABLE M_OUTPUT
                   ERROR_VARIABLE  M_ERROR )

   IF( ${ERROR} )
      MESSAGE(FATAL_ERROR "Error: ${M_ERROR}")
   ENDIF( ${ERROR} )

ENDFUNCTION( SVN_CLEAN_CHECKOUT )

#=============================================================================
# FUNCTION:    BUILD_MOOS
# DESCRIPTION: Builds the MOOS project located inside the MOOSIVP_DIR
#=============================================================================
FUNCTION( BUILD_MOOS MOOS_DIR )
   IF(${WIN32})
      # Set Windows Flags
      SET(MOOS_C_FLAGS "")
      SET(MOOS_CXX_FLAGS "")
   ELSEIF(${APPLE})
      # Set Mac OS-X Flags
      SET(MOOS_C_FLAGS "-m32")
      SET(MOOS_CXX_FLAGS "-fPIC -m32")
   ELSE(${WIN32})
      # Set Linux Flags
      SET(MOOS_C_FLAGS "")
      SET(MOOS_CXX_FLAGS "-fPIC")
   ENDIF(${WIN32})

   SET(CLEANED_MOOS_BUILD_GRAPHICAL_TOOLS "OFF")
   SET(CLEANED_MOOS_BUILD_MATLAB_TOOLS "OFF")
   SET(MOOS_MATLAB_DIR "/usr/bin")

   #------------------------------------
   # Generate MOOS Makefiles using CMake
   #------------------------------------
   EXECUTE_PROCESS(COMMAND ${CMAKE_CMD}  "."
      "-DFLTK_DIR=/usr/lib " 
      "-DFLTK_INCLUDE_DIR=/usr/include " 
      "-DBUILD_FLTKVW=OFF "
      "-DPYTHON_INCLUDE_PATH=/usr/include/python2.5 " 
      "-DBUILD_CONTROL_AND_NAV=ON " 
      "-DBUILD_INSTRUMENTS=ON " 
      "-DDB_HAS_HTTP=ON " 
      "-DBUILD_TOOLS=ON " 
      "-DBUILD_OCEAN_SIM=ON " 
      "-DBUILD_NAVIGATION=OFF " 
      "-DBUILD_HELM=OFF " 
      "-DBUILD_NEWMAT=OFF " 
      "-DBUILD_GRAPHICAL_TOOLS=${CLEANED_MOOS_BUILD_GRAPHICAL_TOOLS} " 
      "-DBUILD_MATLAB_TOOLS=${CLEANED_MOOS_BUILD_MATLAB_TOOLS} " 
      "-DMATLAB_DIR=${MOOS_MATLAB_DIR} " 
      "-DBUILD_LAND_INSTRUMENTS=OFF " 
      "-DBUILD_OCEAN_INSTRUMENTS=OFF " 
      "-DBUILD_UMVS=ON " 
      "-DBUILD_UPB=ON " 
      "-DCMAKE_CXX_FLAGS='${MOOS_CXX_FLAGS}' " 
      "-DCMAKE_C_FLAGS='${MOOS_C_FLAGS}'"  
      WORKING_DIRECTORY "${MOOS_DIR}"
      RESULT_VARIABLE M_STATUS
      OUTPUT_VARIABLE M_OUTPUT
      ERROR_VARIABLE  M_ERROR )

   IF( ${M_ERROR} )
      MESSAGE(FATAL_ERROR "Error: ${M_ERROR}")
   ENDIF( ${M_ERROR} )

   SET(RETURN_OUTPUT "${RETURN_OUTPUT} ${M_OUTPUT}")

   #-------------------------------------------------------------------------
   # Build MOOS: Handle CMake 2.6 for Linux users. Windows users must upgrade
   #-------------------------------------------------------------------------
   IF(${CMAKE_VERSION} VERSION_LESS 2.8 )
      # Shame on you! You should really upgrade to 2.8.
      IF( ${WIN32} )
         # Windows users have no excuse! Upgrade to 2.8! It's easy. Just
         # download the binary package from cmake.org
         MESSAGE(FATAL 
            "ERROR! Windows users must upgrade to CMake version 2.8.")
      ELSE( ${WIN32} )
         # Linux and Mac users can just use make to build it.
         EXECUTE_PROCESS( COMMAND ${MAKE_CMD}
                       WORKING_DIRECTORY "${MOOS_DIR}"
                       RESULT_VARIABLE M_STATUS
                       OUTPUT_VARIABLE M_OUTPUT
                       ERROR_VARIABLE  M_ERROR )
         IF( NOT "${M_ERROR}" STREQUAL "" )
            MESSAGE(FATAL_ERROR "Error: ${M_ERROR}")
         ENDIF( NOT "${M_ERROR}" STREQUAL "" )
      ENDIF( ${WIN32} )
   ELSE( ${CMAKE_VERSION} VERSION_LESS 2.8 )
      # You are using CMake 2.8. Good for you. We can use the --build flag.
      EXECUTE_PROCESS( COMMAND ${CMAKE_CMD} "--build" "."
                       WORKING_DIRECTORY "${MOOS_DIR}"
                       RESULT_VARIABLE M_STATUS
                       OUTPUT_VARIABLE M_OUTPUT
                       ERROR_VARIABLE  M_ERROR )
      IF( NOT "${M_ERROR}" STREQUAL "" )
         MESSAGE(FATAL_ERROR "Error: ${M_ERROR}")
      ENDIF( NOT "${M_ERROR}" STREQUAL "" )
   ENDIF( ${CMAKE_VERSION} VERSION_LESS 2.8 )



   SET(RETURN_OUTPUT "${RETURN_OUTPUT} ${M_OUTPUT}")

   SET(${ARGV1} "${RETURN_OUTPUT}" PARENT_SCOPE)

ENDFUNCTION( BUILD_MOOS MOOSIVP_DIR )

#=============================================================================
# FUNCTION:   RESOLVE_SYMLINK
# DESCRIPTIN: Resolves the specified symlink and returns its full path in the
#             specified variable.
#=============================================================================
FUNCTION(RESOLVE_SYMLINK VARIABLE LINK)
   IF( ${WIN32} )
      FILE(READ ${LINK} LOCATION OFFSET 5)
      GET_FILENAME_COMPONENT(RPATH ${LINK} PATH)
      GET_FILENAME_COMPONENT(RPATH "${RPATH}" ABSOLUTE)
      GET_FILENAME_COMPONENT(FULL_PATH "${RPATH}/${LOCATION}" ABSOLUTE)
   ELSE( ${WIN32} )
      IF(${CMAKE_VERSION} VERSION_GREATER 2.8)
         GET_FILENAME_COMPONENT(FULL_PATH ${LINK} REALPATH)
      ELSE(${CMAKE_VERSION} VERSION_GREATER 2.8)
         GET_FILENAME_COMPONENT(FULL_PATH ${LINK} ABSOLUTE)
      ENDIF(${CMAKE_VERSION} VERSION_GREATER 2.8)
   ENDIF( ${WIN32} )
   # Set the return variable to the full path of the link
   SET(${VARIABLE} "${FULL_PATH}" PARENT_SCOPE)
ENDFUNCTION(RESOLVE_SYMLINK)

