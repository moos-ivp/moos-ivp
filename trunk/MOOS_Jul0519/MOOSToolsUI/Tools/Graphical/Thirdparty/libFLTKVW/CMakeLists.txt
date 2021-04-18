set(LIBNAME fltkvw)

SET(SOURCES
   Flv_List.cxx
   Flv_Style.cxx
   Flv_Table.cxx)

FIND_PACKAGE( FLTK REQUIRED)

# commented out by mikerb Oct 8th 2020
#FIND_LIBRARY(GLUT  REQUIRED )
#FIND_LIBRARY(OpenGL REQUIRED )


IF(APPLE)
   INCLUDE_DIRECTORIES ( /System/Library/Frameworks )
   FIND_LIBRARY(COCOA_LIBRARY Cocoa)
   SET(EXTRA_MAC_LIBS ${COCOA_LIBRARY} ${GLUT_LIBRARY} ${OpenGL_LIBRARY})
ENDIF (APPLE)



##########################
# Packages / Dependencies
set(INCLUDE_ROOTS  
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${FLTK_INCLUDE_DIR}  #mikerb
)
    
set(${LIBNAME}_DEPEND_INCLUDE_DIRS
    ${INCLUDE_ROOTS}
    CACHE INTERNAL
    "List of include dirs for the dependencies of ${LIBNAME}"    
)
list(REMOVE_DUPLICATES ${LIBNAME}_DEPEND_INCLUDE_DIRS)

#if(UNIX)
#    set(DEPENDENCIES fltk ${COCOA_LIBRARY} ${GLUT_LIBRARY} ${OpenGL_LIBRARY} )
#endif(UNIX)

set(DEPENDENCIES ${FLTK_LIBRARIES} ) #mikerb

set(${LIBNAME}_DEPEND_LIBRARIES
    ${DEPENDENCIES}
    CACHE INTERNAL
    "List of libraries which are dependencies of ${LIBNAME}"    
)
list(REMOVE_DUPLICATES ${LIBNAME}_DEPEND_LIBRARIES)


#######################################
# Export include and library locations
# This is essentially boilerplate.  Shouldn't need to change it.

set(${LIBNAME}_INCLUDE_DIRS
    ${INCLUDE_ROOTS}
    CACHE INTERNAL
    "Where to find ${LIBNAME} include directories"
)
list(REMOVE_DUPLICATES ${LIBNAME}_INCLUDE_DIRS)

set(${LIBNAME}_LIBRARIES
    ${LIBNAME}
    CACHE INTERNAL
    "List of libraries needed to link ${LIBNAME} library"
)
list(REMOVE_DUPLICATES ${LIBNAME}_LIBRARIES)


#######################################
# Create the library
# This is boilerplate.  Any extra libs you want to link should be in the '..._DEPEND_...'
# variables
include_directories(${${LIBNAME}_INCLUDE_DIRS} ${${LIBNAME}_DEPEND_INCLUDE_DIRS})
add_library(${LIBNAME} ${SOURCES} ${PUBLIC_HEADERS})
target_link_libraries(${LIBNAME} ${${LIBNAME}_DEPEND_LIBRARIES})


