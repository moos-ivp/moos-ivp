set(LIBNAME alogTools)

set( SOURCES
    indexedAlogReader.cpp
    alogLineReader.cpp
    indexReader.cpp
    indexWriter.cpp
    recordTypes.cpp
    utils.cpp
    FileNotFoundException.cpp
    VersionException.cpp
)


##########################
# Packages / Dependencies
set(INCLUDE_ROOTS  
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
    
set(${LIBNAME}_DEPEND_INCLUDE_DIRS
    ${INCLUDE_ROOTS}
    CACHE INTERNAL
    "List of include dirs for the dependencies of ${LIBNAME}"    
)
list(REMOVE_DUPLICATES ${LIBNAME}_DEPEND_INCLUDE_DIRS)

if(UNIX)
    set(DEPENDENCIES  )
endif(UNIX)

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



######################################
#export the library
cache_internal_append_unique(PROJECT_EXPORT_LIBS 
    ${LIBNAME}
)



#######################################
# Create the library
# This is boilerplate.  Any extra libs you want to link should be in the '..._DEPEND_...'
# variables
include_directories(${${LIBNAME}_INCLUDE_DIRS} ${${LIBNAME}_DEPEND_INCLUDE_DIRS})
add_library(${LIBNAME} ${SOURCES} ${PUBLIC_HEADERS})
target_link_libraries(${LIBNAME} ${${LIBNAME}_DEPEND_LIBRARIES})

#######################################
# install headers
install(DIRECTORY ${INCLUDE_ROOTS} DESTINATION ${CMAKE_INSTALL_PREFIX} FILES_MATCHING PATTERN "*.h")

# install libraries
install(TARGETS ${LIBNAME} DESTINATION lib)

#oh and build some  helper applications
add_subdirectory(bin)

