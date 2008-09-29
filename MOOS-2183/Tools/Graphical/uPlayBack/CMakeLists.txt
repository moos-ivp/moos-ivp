PROJECT(uPlayBack)

#build a list of files to compile
SET(UPLAYBACK_SRCS 
uPlayBackMain.cpp
PlaybackWindow.cpp
PlaybackWindow.h
MOOSPlayBackV2.h
MOOSPlayBackV2.cpp)


#look for FLTK ourselves
FIND_PACKAGE( FLTK REQUIRED )


IF(FLTK_FOUND)

   INCLUDE_DIRECTORIES(${FLTK_INCLUDE_DIR})

   # Add executable
   #(note the WIN32 bit says under windows use /subsystem:windows not console
   #this prevents fltk app from producing an annoying console aswell
   #ADD_EXECUTABLE( uPlayBack WIN32 ${UPLAYBACK_SRCS})   
   # However, this does not work under windows in release mode
 ADD_EXECUTABLE( uPlayBack WIN32  MACOSX_BUNDLE ${UPLAYBACK_SRCS})   

   # Link the executable
   TARGET_LINK_LIBRARIES(uPlayBack ${MOOSLIBS} ${PLATFORM_LIBS}  fltkvw ${FLTK_LIBRARIES} ${OPENGL_LIBS} )

ELSE(FLTK_FOUND)
    
   MESSAGE("uPB won't be built as FLTK was not found.")

ENDIF(FLTK_FOUND)
