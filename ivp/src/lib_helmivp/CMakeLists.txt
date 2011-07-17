#--------------------------------------------------------
# The CMakeLists.txt for:                     lib_helmivp
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
   HelmEngine.cpp
   HelmReport.cpp
   ModeSet.cpp
   ModeEntry.cpp
   Populator_BehaviorSet.cpp
   BehaviorSet.cpp
   BFactoryStatic.cpp
   BFactoryDynamic.cpp
   BehaviorSpec.cpp
   BehaviorReport.cpp
   SpecBuild.cpp
   AVD_Table.cpp
)

SET(HEADERS
  HelmEngine.h
  HelmReport.h
  ModeSet.h
  ModeEntry.h
  Populator_BehaviorSet.h
  BehaviorSet.h
  BFactoryStatic.h
  BFactoryDynamic.h
  BehaviorSpec.h
  BehaviorReport.h
  SpecBuild.h
  AVD_Table.h
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
