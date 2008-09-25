PROJECT(uMS)

# next few lines are simply to insert icons into
# the console application on windows
IF(WIN32)
    SET(RESOURCES icon1.ico uMS.rc)
ENDIF(WIN32)


#build a list of files to compile
SET(UMS_SRCS ScopeGrid.cpp uMS.cpp ScopeTabPane.cpp DBImage.cpp ScopeGrid.h ScopeTabPane.h DBImage.h ${RESOURCES})

#look for FLTK ourselves
INCLUDE(${MOOS_SOURCE_DIR}/MOOSFindFLTK.cmake)
#FIND_PACKAGE( FLTK REQUIRED )

IF(FLTK_FOUND)

   INCLUDE_DIRECTORIES(${FLTK_INCLUDE_DIR})


   # Add executable
   #(note the WIN32 bit says under windows use /subsystem:windows not console
   #this prevents fltk app from producing an annoying console aswell
   ADD_EXECUTABLE( uMS WIN32 ${UMS_SRCS})

   # Link the executable
   TARGET_LINK_LIBRARIES(uMS ${MOOSLIBS} ${PLATFORM_LIBS} fltkvw ${FLTK_LIBRARIES} ${OPENGL_LIBS} )

ELSE(FLTK_FOUND)
    
   MESSAGE("uMS won't be built as FLTK was not found.")

ENDIF(FLTK_FOUND)
