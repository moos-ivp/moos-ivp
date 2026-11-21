# Use only version tags; unrelated repository tags are not releases.
execute_process(
  COMMAND "${GIT_EXECUTABLE}" describe --tags --long
    --match "v[0-9]*" --match "[0-9]*" HEAD
  WORKING_DIRECTORY "${IVP_VERSION_SOURCE_DIR}"
  RESULT_VARIABLE git_result
  OUTPUT_VARIABLE git_description
  ERROR_VARIABLE git_error
  OUTPUT_STRIP_TRAILING_WHITESPACE)
if(NOT git_result EQUAL 0 OR
   NOT git_description MATCHES "^v?([0-9].*)-([0-9]+)-g([0-9a-f]+)$")
  message(FATAL_ERROR
    "Cannot determine MOOS-IvP version from Git. Ensure release tags and history are available. ${git_error}")
endif()
if(CMAKE_MATCH_2 STREQUAL "0")
  set(IVP_VERSION "${CMAKE_MATCH_1}")
else()
  set(IVP_VERSION "${CMAKE_MATCH_1}+git${CMAKE_MATCH_2}.g${CMAKE_MATCH_3}")
endif()
# configure_file changes the timestamp only when the version changes.
configure_file("${CMAKE_CURRENT_LIST_DIR}/IVPVersion.h.in"
  "${IVP_VERSION_HEADER}" @ONLY)
