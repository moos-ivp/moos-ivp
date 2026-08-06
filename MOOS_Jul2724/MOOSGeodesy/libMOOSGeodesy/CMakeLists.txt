set(LIBNAME MOOSGeodesy)

SET(SOURCES
    MOOSGeodesy.cpp
)

include(PlatformDefines)

if(MOOSGEODESY_USE_PROJ)
    list(APPEND SOURCES MOOSGeodesyProj.cpp)
else()
    list(APPEND SOURCES MOOSGeodesyOriginal.cpp)
endif()

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
    set(DEPENDENCIES m)
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


#######################################
# Create the library
# This is boilerplate.  Any extra libs you want to link should be in the '..._DEPEND_...'
# variables
add_library(${LIBNAME} ${SOURCES} ${PUBLIC_HEADERS})
add_library(MOOSGeodesy::MOOSGeodesy ALIAS ${LIBNAME})
target_include_directories(${LIBNAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
if(MOOSGEODESY_USE_PROJ)
    target_compile_definitions(${LIBNAME} PUBLIC MOOSGEODESY_IMPL=MOOSGEODESY_IMPL_PROJ)
    target_include_directories(${LIBNAME} PRIVATE ${PROJ4_INCLUDE_DIR})
    target_link_libraries(${LIBNAME} PRIVATE ${PROJ4_LIBRARY})
else()
    target_compile_definitions(${LIBNAME} PUBLIC MOOSGEODESY_IMPL=MOOSGEODESY_IMPL_PLAIN)
endif()
target_link_libraries(${LIBNAME} PUBLIC ${${LIBNAME}_DEPEND_LIBRARIES})

#######################################
# install headers
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    FILES_MATCHING PATTERN "*.h")


# install libraries
INSTALL(TARGETS ${LIBNAME}
  EXPORT MOOSGeodesyTargets
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)
