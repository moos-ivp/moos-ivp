# Provide an 'uninstall' target so that we can use 'make uninstall'
#
# Configures the "cmake_uninstall.cmake.in" script and places it in the requested
# location, before adding it as an 'uninstall' target to the project.
#
# Usage example:
# configure_uninstall_target(${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
#

macro(configure_uninstall_target outfile)

    # First configure the script
    configure_file(${CMAKE_MODULE_PATH}/cmake_uninstall.cmake.in ${outfile} IMMEDIATE @ONLY)

    # Now set it as the 'uninstall' target
    add_custom_target(uninstall
        "${CMAKE_COMMAND}" -P "${outfile}")

endmacro()
