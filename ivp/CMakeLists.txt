CMAKE_MINIMUM_REQUIRED(VERSION 2.0)

SET(IVP_PACKAGE_VERSION "" CACHE STRING "The version of IvP that you're building.  I.e., 0.1.0")
IF ("${IVP_PACKAGE_VERSION}" STREQUAL "")
   MESSAGE("If you don't set IVP_PACKAGE_VERSION, no INSTALL directives will "
      "be called.  This is because they use IVP_PACKAGE_VERSION as part of their "
      "naming scheme.  This means that 'make install' won't put the project's "
      "files in the ideal locations, and our scripts for creating packags "
      "(such as Debian .deb files) won't work either."
      )
ENDIF ("${IVP_PACKAGE_VERSION}" STREQUAL "")

SUBDIRS(
   docs
   src
)
