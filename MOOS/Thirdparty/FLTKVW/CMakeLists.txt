PROJECT(FLTKVW)

SET(FLTKVW_SRCS
   Flv_List.cxx
   Flv_Style.cxx
   Flv_Table.cxx
   Flv_List.H
   Flv_Style.H
   Flv_Table.H
   MOOSFLTKUI.h
   FLTKCheckList.h)

INCLUDE(${MOOS_SOURCE_DIR}/MOOSFindFLTK.cmake)

IF(FLTK_FOUND)
  INCLUDE_DIRECTORIES(${FLTK_INCLUDE_DIR})
  ADD_LIBRARY(fltkvw ${FLTKVW_SRCS})
ELSE(FLTK_FOUND)
  MESSAGE("FLTKVW: FLTK not found. FLTKVW won't be built.")
ENDIF(FLTK_FOUND)


#===============================================================================
# For installation and Debian packaging...
#===============================================================================

IF(FLTK_FOUND)
   IF ("${MOOS_VERSION}" STREQUAL "")
      MESSAGE(STATUS "MOOS_VERSION not set, so not creating install component.")
   ELSE("${MOOS_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS fltkvw
         DESTINATION lib/moos-${MOOS_VERSION}
         COMPONENT Moos_Thirdparty_Dev)
      
      # Note: If there are any header files that are truly unneeded by users of this
      # library, it should be OK to remove them from the PUBLIC_HEADERS variable,
      # which in turn will prevent them from being installed. -CJC
      SET(PUBLIC_HEADERS 
         FLTKCheckList.h
         MOOSFLTKUI.h
      )
      
      INSTALL(
         FILES ${PUBLIC_HEADERS}
         DESTINATION include/moos-${MOOS_VERSION}/Thirdparty/FLTKVW
         COMPONENT Moos_Thirdparty_Dev)
   ENDIF("${MOOS_VERSION}" STREQUAL "")
ELSEIF(FLTK_FOUND)
   MESSAGE(STATUS 
      "Warning: Not building fltkvw, because the FLTK library couldn't be "
      "found.  So the 'Moos_Thirdparty_Dev' install component (and associated "
      "Debian package file) won't contain the fltkvw library.")
ENDIF(FLTK_FOUND)

#===============================================================================
