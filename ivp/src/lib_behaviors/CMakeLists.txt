#--------------------------------------------------------
# The CMakeLists.txt for:                   lib_behaviors
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  IvPBehavior.cpp
  IvPContactBehavior.cpp
  LifeEventHistory.cpp
)

SET(HEADERS
  BehaviorReport.h
  BehaviorSet.h
  IvPBehavior.h
)

# Build Library
ADD_LIBRARY(behaviors ${SRC})

# Install Headers
INSTALL( FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/lib_behaviors)

# Install Libararies
INSTALL( TARGETS behaviors
   RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
   LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
   ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)

