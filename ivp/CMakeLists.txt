CMAKE_MINIMUM_REQUIRED(VERSION 2.0)

PROJECT(IVP)


SET(IVP_PACKAGE_VERSION "" CACHE STRING "The version of IvP that you're building.  I.e., 0.1.0")
IF ("${IVP_PACKAGE_VERSION}" STREQUAL "")
   MESSAGE("If you don't set IVP_PACKAGE_VERSION, no INSTALL directives will "
      "be called.  This is because they use IVP_PACKAGE_VERSION as part of their "
      "naming scheme.  This means that 'make install' won't put the project's "
      "files in the ideal locations, and our scripts for creating packags "
      "(such as Debian .deb files) won't work either."
      )
ENDIF ("${IVP_PACKAGE_VERSION}" STREQUAL "")

# This is used to supply a preprocessor definition that gets compiled into some
# code, such as lib_marineview.  I really dislike this approach because it's 
# very brittle w.r.t. anything getting moved around, or if you use the code on
# a different system than that on which it was compiled. -CJC
GET_FILENAME_COMPONENT(IVP_DATA_DIR ./data ABSOLUTE)

SUBDIRS(
   docs
   src
)
