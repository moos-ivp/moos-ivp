#--------------------------------------------------------
# The CMakeLists.txt for:                     lib_helmivp
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
   BehaviorFactory.cpp
   HelmEngine.cpp
   HelmEngineBeta.cpp
   HelmReport.cpp
   ModeSet.cpp
   ModeEntry.cpp
   Populator_BehaviorSet.cpp
   Populator_BehaviorSet2.cpp
   AVD_Table.cpp
)

SET(HEADERS
  AVD_Table.h
  BehaviorFactory.h
  HelmEngineBeta.h
  HelmEngine.h
  HelmReport.h
  ModeEntry.h
  ModeSet.h
  Populator_BehaviorSet2.h
  Populator_BehaviorSet.h
)

# Build Library
ADD_LIBRARY(helmivp ${SRC})

# Install Headers
INSTALL( FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/lib_helmivp)

# Install Libararies
INSTALL( TARGETS helmivp
   RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
   LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
   ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)
