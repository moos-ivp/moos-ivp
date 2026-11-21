# An explicit version supports packagers and builds without Git history.
# Example: -DIVP_VERSION_OVERRIDE=v26.11.0-debian uses v26.11.0-debian unchanged.
if(DEFINED IVP_VERSION_OVERRIDE AND NOT IVP_VERSION_OVERRIDE STREQUAL "")
  set(IVP_VERSION "${IVP_VERSION_OVERRIDE}")
else()
  # Example without Git or a valid HEAD: unknown.
  set(IVP_VERSION "unknown")
  if(GIT_EXECUTABLE)
    execute_process(
      COMMAND "${GIT_EXECUTABLE}" describe --tags --exact-match HEAD
      WORKING_DIRECTORY "${IVP_VERSION_SOURCE_DIR}"
      RESULT_VARIABLE tag_result
      OUTPUT_VARIABLE git_tag
      ERROR_QUIET
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(tag_result STREQUAL "0")
      # Example at a clean tag v26.11.0: v26.11.0.
      set(IVP_VERSION "${git_tag}")
    else()
      execute_process(
        COMMAND "${GIT_EXECUTABLE}" rev-parse --short HEAD
        WORKING_DIRECTORY "${IVP_VERSION_SOURCE_DIR}"
        RESULT_VARIABLE sha_result
        OUTPUT_VARIABLE git_sha
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE)
      if(sha_result STREQUAL "0")
        # Example at an untagged, detached HEAD: 9430ddd26.
        set(IVP_VERSION "${git_sha}")
        execute_process(
          COMMAND "${GIT_EXECUTABLE}" symbolic-ref --quiet --short HEAD
          WORKING_DIRECTORY "${IVP_VERSION_SOURCE_DIR}"
          RESULT_VARIABLE branch_result
          OUTPUT_VARIABLE git_branch
          ERROR_QUIET
          OUTPUT_STRIP_TRAILING_WHITESPACE)
        if(branch_result STREQUAL "0")
          # Example on a clean branch: feature/navigation-9430ddd26.
          set(IVP_VERSION "${git_branch}-${git_sha}")
        endif()
      endif()
    endif()
    if(NOT IVP_VERSION STREQUAL "unknown")
      # Like git describe --dirty, consider tracked changes only.
      execute_process(
        COMMAND "${GIT_EXECUTABLE}" status --porcelain --untracked-files=no
        WORKING_DIRECTORY "${IVP_VERSION_SOURCE_DIR}"
        RESULT_VARIABLE status_result
        OUTPUT_VARIABLE git_status
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE)
      if(status_result STREQUAL "0" AND NOT git_status STREQUAL "")
        # Examples: v26.11.0-dirty, feature/navigation-9430ddd26-dirty,
        # or 9430ddd26-dirty.
        string(APPEND IVP_VERSION "-dirty")
      endif()
    endif()
  endif()
  if(IVP_VERSION STREQUAL "unknown")
    message(WARNING
      "Cannot determine MOOS-IvP version from Git; using 'unknown'. Set IVP_VERSION_OVERRIDE explicitly or build from a Git checkout.")
  endif()
endif()
if(NOT DEFINED IVP_PACKAGER)
  set(IVP_PACKAGER "MIT")
endif()
# configure_file changes the timestamp only when the version or packager changes.
configure_file("${CMAKE_CURRENT_LIST_DIR}/IVPVersion.h.in"
  "${IVP_VERSION_HEADER}" @ONLY ESCAPE_QUOTES)
