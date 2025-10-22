#--------------------------------------------------------
# The CMakeLists.txt for:                         nspatch
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

# Set System Specific Libraries
SET(SYSTEM_LIBS m)

SET(SRC
  BHVFile.cpp
  MOOSFile.cpp
  Populator_BHVFile.cpp
  Populator_MOOSFile.cpp
  main.cpp
  PatchApplicator.cpp)

ADD_EXECUTABLE(nspatch ${SRC})
   
TARGET_LINK_LIBRARIES(nspatch
  mbutil
  ${SYSTEM_LIBS})

