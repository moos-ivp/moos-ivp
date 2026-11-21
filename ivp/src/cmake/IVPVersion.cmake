# An explicit version supports packagers and builds without Git history.
if(NOT DEFINED IVP_VERSION)
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
        set(IVP_VERSION "${git_sha}")
        execute_process(
          COMMAND "${GIT_EXECUTABLE}" symbolic-ref --quiet --short HEAD
          WORKING_DIRECTORY "${IVP_VERSION_SOURCE_DIR}"
          RESULT_VARIABLE branch_result
          OUTPUT_VARIABLE git_branch
          ERROR_QUIET
          OUTPUT_STRIP_TRAILING_WHITESPACE)
        if(branch_result STREQUAL "0")
          set(IVP_VERSION "${git_branch}-${git_sha}")
        endif()
        # Like git describe --dirty, consider tracked changes only.
        execute_process(
          COMMAND "${GIT_EXECUTABLE}" status --porcelain --untracked-files=no
          WORKING_DIRECTORY "${IVP_VERSION_SOURCE_DIR}"
          RESULT_VARIABLE status_result
          OUTPUT_VARIABLE git_status
          ERROR_QUIET
          OUTPUT_STRIP_TRAILING_WHITESPACE)
        if(status_result STREQUAL "0" AND NOT git_status STREQUAL "")
          string(APPEND IVP_VERSION "-dirty")
        endif()
      endif()
    endif()
  endif()
  if(IVP_VERSION STREQUAL "unknown")
    message(WARNING
      "Cannot determine MOOS-IvP version from Git; using 'unknown'. Set IVP_VERSION explicitly or build from a Git checkout.")
  endif()
endif()
if(NOT DEFINED IVP_PACKAGER)
  set(IVP_PACKAGER "MIT")
endif()
# configure_file changes the timestamp only when the version or packager changes.
configure_file("${CMAKE_CURRENT_LIST_DIR}/IVPVersion.h.in"
  "${IVP_VERSION_HEADER}" @ONLY ESCAPE_QUOTES)
