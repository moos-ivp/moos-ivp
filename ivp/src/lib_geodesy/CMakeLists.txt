#--------------------------------------------------------
# The CMakeLists.txt for:                     lib_geodesy
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

set(LIBNAME MOOSGeodesy)
set(SRC MOOSGeodesy.cpp)

#include_directories(${PROJ4_INCLUDE_DIRS})
#link_directories(${PROJ4_LIB_PATH})

# Build Library
ADD_LIBRARY(${LIBNAME} ${SRC})

TARGET_LINK_LIBRARIES( ${LIBNAME} )

