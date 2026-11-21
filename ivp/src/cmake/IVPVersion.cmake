# Example without Git or release tags: unknown
set(IVP_VERSION "unknown")
if(GIT_EXECUTABLE)
  # Use only version tags; unrelated repository tags are not releases.
  execute_process(
    COMMAND "${GIT_EXECUTABLE}" describe --tags --long
      --match "v[0-9]*" --match "[0-9]*" HEAD
    WORKING_DIRECTORY "${IVP_VERSION_SOURCE_DIR}"
    RESULT_VARIABLE git_result
    OUTPUT_VARIABLE git_description
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  if(git_result STREQUAL "0" AND
     git_description MATCHES "^v?([0-9].*)-([0-9]+)-g([0-9a-f]+)$")
    if(CMAKE_MATCH_2 STREQUAL "0")
      # Example at tag v26.11.0: 26.11.0
      set(IVP_VERSION "${CMAKE_MATCH_1}")
    else()
      # Example seven commits after v26.11.0: 26.11.0+git7.g05280d86a
      set(IVP_VERSION "${CMAKE_MATCH_1}+git${CMAKE_MATCH_2}.g${CMAKE_MATCH_3}")
    endif()
  endif()
endif()
if(IVP_VERSION STREQUAL "unknown")
  message(WARNING
    "Cannot determine MOOS-IvP version from Git; using 'unknown'. Git and reachable release tags are needed for version information.")
endif()
# configure_file changes the timestamp only when the version changes.
configure_file("${CMAKE_CURRENT_LIST_DIR}/IVPVersion.h.in"
  "${IVP_VERSION_HEADER}" @ONLY)
