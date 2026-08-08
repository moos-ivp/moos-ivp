#--------------------------------------------------------
# The CMakeLists.txt for:                    lib_gen_poly
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  XYGenPolygon.cpp
  GPUtils.cpp
)

SET(HEADERS
  XYGenPolygon.h
  GPUtils.h
)

# Build Library
ADD_LIBRARY(gen_poly ${SRC})

TARGET_LINK_LIBRARIES(gen_poly
   mbutil
)

