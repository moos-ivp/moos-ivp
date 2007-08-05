PROJECT(uMS)


# next few lines are simply to insert icons into
# the console application on windows
IF(WIN32)
    SET(RESOURCES icon1.ico uMS.rc)
ENDIF(WIN32)


#build a list of files to compile
SET(UMS_SRCS ScopeGrid.cpp uMS.cpp ScopeTabPane.cpp DBImage.cpp ScopeGrid.h ScopeTabPane.h DBImage.h ${RESOURCES})

# Add executable
ADD_EXECUTABLE( uMS ${UMS_SRCS})

# Link the executable
TARGET_LINK_LIBRARIES(uMS ${MOOSLIBS} ${PLATFORM_LIBS}  fltkvw ${FLTK_LIBS} ${OPENGL_LIBS} )
