

# next few lines are simply to insert icons into
# the console application on windows
IF(WIN32)
    SET(RESOURCES icon1.ico uMS.rc)
ENDIF(WIN32)

#build a list of files to compile
SET(UMS_SRCS ScopeGrid.cpp uMS.cpp ScopeTabPane.cpp DBImage.cpp ScopeGrid.h ScopeTabPane.h DBImage.h ${RESOURCES})

FIND_PACKAGE(MOOS REQUIRED)

#look for FLTK ourselves
FIND_PACKAGE( FLTK REQUIRED )

#fltk has OpenGL dependancy
FIND_PACKAGE( OpenGL REQUIRED )

IF(FLTK_FOUND)

   INCLUDE_DIRECTORIES(${fltkvw_DEPEND_INCLUDE_DIRS} ${fltkvw_INCLUDE_DIRS} ${MOOS_INCLUDE_DIRS} ${MOOS_DEPEND_INCLUDE_DIRS})

  
   # Add executable
   #(note the WIN32 bit says under windows use /subsystem:windows not console
   #this prevents fltk app from producing an annoying console as well
   
   #ADD_EXECUTABLE( uMS WIN32  MACOSX_BUNDLE ${UMS_SRCS}  # mikerb comment out MACOSX_BUNDLE
   ADD_EXECUTABLE( uMS WIN32  ${UMS_SRCS})

   # Link the executable
   TARGET_LINK_LIBRARIES( uMS ${MOOS_LIBRARIES} ${MOOS_DEPEND_LIBRARIES} ${fltkvw_LIBRARIES} ${fltkvw_DEPEND_LIBRARIES}  )

ELSE(FLTK_FOUND)
    
   MESSAGE("uMS won't be built as FLTK was not found.")

ENDIF(FLTK_FOUND)
