if(NOT DEFINED PATCH_FILE)
    message(FATAL_ERROR "PATCH_FILE is required")
endif()

if(NOT DEFINED PATCH_WORKING_DIRECTORY)
    message(FATAL_ERROR "PATCH_WORKING_DIRECTORY is required")
endif()

find_program(PATCH_EXECUTABLE patch)
if(NOT PATCH_EXECUTABLE)
    message(FATAL_ERROR "patch is required to apply ${PATCH_FILE}")
endif()

execute_process(
    COMMAND "${PATCH_EXECUTABLE}"
        --dry-run
        --forward
        --strip 1
        --input "${PATCH_FILE}"
    WORKING_DIRECTORY "${PATCH_WORKING_DIRECTORY}"
    RESULT_VARIABLE FORWARD_RESULT
    OUTPUT_VARIABLE FORWARD_OUTPUT
    ERROR_VARIABLE FORWARD_ERROR
)

if("${FORWARD_RESULT}" STREQUAL "0")
    execute_process(
        COMMAND "${PATCH_EXECUTABLE}"
            --forward
            --strip 1
            --input "${PATCH_FILE}"
        WORKING_DIRECTORY "${PATCH_WORKING_DIRECTORY}"
        RESULT_VARIABLE APPLY_RESULT
        OUTPUT_VARIABLE APPLY_OUTPUT
        ERROR_VARIABLE APPLY_ERROR
    )
    if(NOT "${APPLY_RESULT}" STREQUAL "0")
        message(FATAL_ERROR
            "Failed to apply ${PATCH_FILE}\n"
            "${APPLY_OUTPUT}${APPLY_ERROR}")
    endif()
else()
    execute_process(
        COMMAND "${PATCH_EXECUTABLE}"
            --dry-run
            --reverse
            --strip 1
            --input "${PATCH_FILE}"
        WORKING_DIRECTORY "${PATCH_WORKING_DIRECTORY}"
        RESULT_VARIABLE REVERSE_RESULT
        OUTPUT_VARIABLE REVERSE_OUTPUT
        ERROR_VARIABLE REVERSE_ERROR
    )
    if(NOT "${REVERSE_RESULT}" STREQUAL "0")
        message(FATAL_ERROR
            "Patch cannot be applied and is not already applied: ${PATCH_FILE}\n"
            "Forward check:\n${FORWARD_OUTPUT}${FORWARD_ERROR}\n"
            "Reverse check:\n${REVERSE_OUTPUT}${REVERSE_ERROR}")
    endif()
endif()
