# A set of functions for extracting system information to nicely formattted
# strings. Useful for submitting meaning build names to CDash.

###########################################################################
# Returns the current git branch of the source directory
function( moos_get_current_git_branch CURRENT_GIT_BRANCH_NAME )

  set( GIT_QUERY_COMMAND "git" "rev-parse" "--abbrev-ref" "HEAD" )

  execute_process(COMMAND ${GIT_QUERY_COMMAND}
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                  OUTPUT_VARIABLE result
                  OUTPUT_STRIP_TRAILING_WHITESPACE)

  set( ${CURRENT_GIT_BRANCH_NAME} ${result} PARENT_SCOPE )

endfunction()

###########################################################################
# Returns the result of "uname -m", if uname exists.
function( moos_get_uname_output UNAME_OUTPUT )

  find_program(UNAME_PROGRAM NAMES uname)
  mark_as_advanced(UNAME_PROGRAM)

  if(UNAME_PROGRAM)
    execute_process(COMMAND ${UNAME_PROGRAM} "-m"
                    OUTPUT_VARIABLE result
                    OUTPUT_STRIP_TRAILING_WHITESPACE)

    set( ${UNAME_OUTPUT} ${result} PARENT_SCOPE )
  endif()

endfunction()

###########################################################################
# Returns the system name to the user. Under OS X uses sw_vers, and Linux
# attemps to use lsb_release -d
function( moos_get_operating_system_name OPERATING_SYSTEM_NAME )

  # Mac OS X - call sw_vers, put together multiple queries
  if( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin" )
    find_program(SW_VERS_PROGRAM sw_vers)
    mark_as_advanced(SW_VERS_PROGRAM)

    # get the product name e.g "Mac OS X", version e.g "10.9" and build version
    execute_process(COMMAND ${SW_VERS_PROGRAM} "-productName"
                    OUTPUT_VARIABLE product_name
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND ${SW_VERS_PROGRAM} "-productVersion"
                    OUTPUT_VARIABLE product_version
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND ${SW_VERS_PROGRAM} "-buildVersion"
                    OUTPUT_VARIABLE build_version
                    OUTPUT_STRIP_TRAILING_WHITESPACE)

    set(${OPERATING_SYSTEM_NAME}
        "${product_name} ${product_version} ${build_version}"
        PARENT_SCOPE)
  endif()

  # Linux - attempt to use lsb_release, this works on Ubuntu
  if( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )
    find_program(LSB_RELEASE_PROGRAM lsb_release)
    mark_as_advanced(LSB_RELEASE_PROGRAM)

    if( LSB_RELEASE_PROGRAM )
      execute_process(COMMAND ${LSB_RELEASE_PROGRAM} "-d"
                      OUTPUT_VARIABLE result
                      OUTPUT_STRIP_TRAILING_WHITESPACE)

      # strip the beginning "Description:\t" text, otherwise when using the
      # string to submit to CDash, multiple build names seem to get created.
      string(REGEX REPLACE "Description:\t" "" result ${result})
      set( ${OPERATING_SYSTEM_NAME} "${result}" PARENT_SCOPE )

    endif()
  endif()

endfunction()

###########################################################################
# Returns the first line of the compiler --version result.
# Need to know if using gcc or clang.
function( moos_get_compiler_version COMPILER_VERSION )
  execute_process(COMMAND "${CMAKE_CXX_COMPILER}" --version
                  COMMAND head -n1
                  OUTPUT_VARIABLE compiler_info
                  OUTPUT_STRIP_TRAILING_WHITESPACE)

  # replace any forward slashes with a -, the submission name changes at
  # different stages otherwise, resulting in apparent multiple commits
  string(REGEX REPLACE "/" "-" compiler_info "${compiler_info}")
  set( ${COMPILER_VERSION} "${compiler_info}" PARENT_SCOPE )
endfunction()

###########################################################################
# Constructs a formatted string detailing build information, including
# git branch, cmake build type, system information and compiler information
# Useful for CDash submissions
function( moos_make_informative_build_name BUILD_NAME_STRING )

  moos_get_current_git_branch( git_branch )
  moos_get_operating_system_name( system_name )
  moos_get_uname_output( uname_result )
  moos_get_compiler_version( compiler_info )

  set(FULL_NAME "${system_name} - ${uname_result} - ${compiler_info} - ${git_branch} - ${CMAKE_BUILD_TYPE}")

  set( ${BUILD_NAME_STRING} ${FULL_NAME} PARENT_SCOPE )

endfunction()
