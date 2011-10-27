PROJECT(uMS)

# next few lines are simply to insert icons into
# the console application on windows
IF(WIN32)
    SET(RESOURCES icon1.ico uMS.rc)
ENDIF(WIN32)

#build a list of files to compile
SET(UMS_SRCS ScopeGrid.cpp uMS.cpp ScopeTabPane.cpp DBImage.cpp ScopeGrid.h ScopeTabPane.h DBImage.h ${RESOURCES})

#look for FLTK ourselves
FIND_PACKAGE( FLTK REQUIRED )

#fltk has OpenGL dependancy
FIND_PACKAGE( OpenGL REQUIRED )

IF(FLTK_FOUND)

   INCLUDE_DIRECTORIES(${FLTK_INCLUDE_DIR})

   MESSAGE(STATUS "FLTK libraries  ${FLTK_LIBRARIES}")

   LINK_DIRECTORIES(${CMAKE_LIBRARY_PATH})


   # Add executable
   #(note the WIN32 bit says under windows use /subsystem:windows not console
   #this prevents fltk app from producing an annoying console as well

   
   ADD_EXECUTABLE( uMS WIN32  MACOSX_BUNDLE ${UMS_SRCS})

   IF( APPLE )
      SET( PROGNAME uMS )
#      SET( MACOSX_BUNDLE_ICON_FILE xanthos.icns)
      FILE(MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/${PROGNAME}.app/Contents)


   #   ADD_CUSTOM_COMMAND( TARGET ${PROGNAME} POST_BUILD
   #   COMMAND cp ARGS Mac/${MACOSX_BUNDLE_ICON_FILE} ${EXECUTABLE_OUTPUT_PATH}/${PROGNAME}.app/Contents/Resources
   #   COMMENT Setting up translations & resources ) 
   ENDIF( APPLE )


   # Link the executable
   TARGET_LINK_LIBRARIES( uMS ${MOOSLIBS} ${PLATFORM_LIBS} fltkvw ${FLTK_LIBRARIES} ${OPENGL_LIBRARIES} )

ELSE(FLTK_FOUND)
    
   MESSAGE("uMS won't be built as FLTK was not found.")

ENDIF(FLTK_FOUND)
