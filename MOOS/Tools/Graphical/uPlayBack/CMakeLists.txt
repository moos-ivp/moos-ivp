PROJECT(uPlayBack)

#build a list of files to compile
SET(UPLAYBACK_SRCS 
uPlayBackMain.cpp
PlaybackWindow.cpp
PlaybackWindow.h
MOOSPlayBackV2.h
MOOSPlayBackV2.cpp)


# Add executable
#(note the WIN32 bit says under windows use /subsystem:windows not console
#this prevents fltk app from producing an annoying console aswell



#look for FLTK ourselves
INCLUDE(${MOOS_SOURCE_DIR}/MOOSFindFLTK.cmake)

IF(FLTK_FOUND)

   INCLUDE_DIRECTORIES(${FLTK_INCLUDE_DIR})

   # Add executable
   #(note the WIN32 bit says under windows use /subsystem:windows not console
   #this prevents fltk app from producing an annoying console aswell
   
   ADD_EXECUTABLE( uPlayBack WIN32 ${UPLAYBACK_SRCS})   
   # Link the executable
   TARGET_LINK_LIBRARIES(uPlayBack ${MOOSLIBS} ${PLATFORM_LIBS}  fltkvw ${FLTK_LIBRARIES} ${OPENGL_LIBS} )

ELSE(FLTK_FOUND)
    
   MESSAGE("uPB won't be built as FLTK was not found.")

ENDIF(FLTK_FOUND)

#===============================================================================
# For installation and Debian packaging...
#===============================================================================

IF(FLTK_FOUND)
   IF ("${MOOS_VERSION}" STREQUAL "")
      MESSAGE(STATUS "MOOS_VERSION not set, so not creating install component.")
   ELSE("${MOOS_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS uPlayBack
         DESTINATION bin/moos-${MOOS_VERSION}
         COMPONENT Moos_Tools)
   ENDIF("${MOOS_VERSION}" STREQUAL "")
ENDIF(FLTK_FOUND)

#===============================================================================
