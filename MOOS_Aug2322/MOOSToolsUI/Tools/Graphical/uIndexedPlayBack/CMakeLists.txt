CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# --------------------------------------------------------------
# Indicate CMake 2.7 and above that we don't want to mix relative
#  and absolute paths in linker lib lists.
# Run "cmake --help-policy CMP0003" for more information.
# --------------------------------------------------------------
if(COMMAND cmake_policy)
      cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

set (EXEC uIndexedPlayback)

SET(SOURCES 
    uPlayBackMain.cpp
    PlaybackWindow.cpp
    PlaybackWindow.h
    MOOSPlayBackIndex.h
    MOOSPlayBackIndex.cpp
    ALogIndexWrapper.cpp
)

#look for MOOS ourselves
FIND_PACKAGE( MOOS REQUIRED )

#look for FLTK ourselves
FIND_PACKAGE( FLTK REQUIRED )

#fltk has OpenGL dependancy
FIND_PACKAGE( OpenGL REQUIRED )

IF(FLTK_FOUND)


    INCLUDE_DIRECTORIES(${fltkvw_DEPEND_INCLUDE_DIRS} ${fltkvw_INCLUDE_DIRS} ${alogTools_DEPEND_INCLUDE_DIRS} ${alogTools_INCLUDE_DIRS} ${MOOS_INCLUDE_DIRS} ${MOOS_DEPEND_INCLUDE_DIRS})
    
    #ADD_EXECUTABLE( ${EXEC} WIN32 MACOSX_BUNDLE ${SOURCES})   
    ADD_EXECUTABLE( ${EXEC} WIN32  ${SOURCES})   
   
    TARGET_LINK_LIBRARIES(${EXEC} ${MOOS_LIBRARIES} ${MOOS_DEPEND_LIBRARIES} ${fltkvw_LIBRARIES} ${alogTools_LIBRARIES} ${alogTools_DEPEND_LIBRARIES} ${fltkvw_DEPEND_LIBRARIES}  )

 
ELSE(FLTK_FOUND)
   MESSAGE("${EXEC} won't be built as FLTK was not found.")
ENDIF(FLTK_FOUND)
