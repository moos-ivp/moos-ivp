if(NOT DEFINED PATCH_FILE)
    message(FATAL_ERROR "PATCH_FILE is required")
endif()

if(NOT DEFINED PATCH_WORKING_DIRECTORY)
    message(FATAL_ERROR "PATCH_WORKING_DIRECTORY is required")
endif()

find_program(GIT_EXECUTABLE git)
if(NOT GIT_EXECUTABLE)
    message(FATAL_ERROR "Git is required to apply ${PATCH_FILE}")
endif()

execute_process(
    COMMAND "${GIT_EXECUTABLE}" apply --check "${PATCH_FILE}"
    WORKING_DIRECTORY "${PATCH_WORKING_DIRECTORY}"
    RESULT_VARIABLE FORWARD_RESULT
    ERROR_VARIABLE FORWARD_ERROR
)

if("${FORWARD_RESULT}" STREQUAL "0")
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" apply "${PATCH_FILE}"
        WORKING_DIRECTORY "${PATCH_WORKING_DIRECTORY}"
        RESULT_VARIABLE APPLY_RESULT
        ERROR_VARIABLE APPLY_ERROR
    )
    if(NOT "${APPLY_RESULT}" STREQUAL "0")
        message(FATAL_ERROR "Failed to apply ${PATCH_FILE}: ${APPLY_ERROR}")
    endif()
else()
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" apply --reverse --check "${PATCH_FILE}"
        WORKING_DIRECTORY "${PATCH_WORKING_DIRECTORY}"
        RESULT_VARIABLE REVERSE_RESULT
        ERROR_VARIABLE REVERSE_ERROR
    )
    if(NOT "${REVERSE_RESULT}" STREQUAL "0")
        message(FATAL_ERROR
            "Patch cannot be applied and is not already applied: ${PATCH_FILE}\n"
            "Forward check: ${FORWARD_ERROR}\n"
            "Reverse check: ${REVERSE_ERROR}")
    endif()
endif()
