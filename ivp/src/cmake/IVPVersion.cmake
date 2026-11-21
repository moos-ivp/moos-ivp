# An explicit version supports packagers and builds without Git history.
if(NOT DEFINED IVP_VERSION)
  # Example without Git or release tags: unknown
  set(IVP_VERSION "unknown")
  if(GIT_EXECUTABLE)
    execute_process(
      COMMAND "${GIT_EXECUTABLE}" symbolic-ref --quiet --short HEAD
      WORKING_DIRECTORY "${IVP_VERSION_SOURCE_DIR}"
      RESULT_VARIABLE branch_result
      OUTPUT_VARIABLE git_branch
      ERROR_QUIET
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(branch_result STREQUAL "0" AND NOT git_branch STREQUAL "main")
      # Example on a clean feature branch: feature/navigation
      set(IVP_VERSION "${git_branch}")
      # Like git describe --dirty, consider tracked changes only.
      execute_process(
        COMMAND "${GIT_EXECUTABLE}" status --porcelain --untracked-files=no
        WORKING_DIRECTORY "${IVP_VERSION_SOURCE_DIR}"
        RESULT_VARIABLE status_result
        OUTPUT_VARIABLE git_status
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE)
      if(status_result STREQUAL "0" AND NOT git_status STREQUAL "")
        # Example with tracked changes: feature/navigation-dirty
        string(APPEND IVP_VERSION "-dirty")
      endif()
    else()
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
         git_description MATCHES "^(v?[0-9].*)-([0-9]+)-g([0-9a-f]+)$")
        if(CMAKE_MATCH_2 STREQUAL "0")
          # Example at tag v26.11.0: v26.11.0
          set(IVP_VERSION "${CMAKE_MATCH_1}")
        else()
          # Example seven commits after v26.11.0: v26.11.0+git7.g05280d86a
          set(IVP_VERSION "${CMAKE_MATCH_1}+git${CMAKE_MATCH_2}.g${CMAKE_MATCH_3}")
        endif()
      endif()
    endif()
  endif()
  if(IVP_VERSION STREQUAL "unknown")
    message(WARNING
      "Cannot determine MOOS-IvP version from Git; using 'unknown'. Git and reachable release tags are needed for version information.")
  endif()
endif()
if(NOT DEFINED IVP_PACKAGER)
  set(IVP_PACKAGER "MIT")
endif()
# configure_file changes the timestamp only when the version or packager changes.
configure_file("${CMAKE_CURRENT_LIST_DIR}/IVPVersion.h.in"
  "${IVP_VERSION_HEADER}" @ONLY ESCAPE_QUOTES)
