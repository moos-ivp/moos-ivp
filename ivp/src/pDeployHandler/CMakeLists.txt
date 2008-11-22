#--------------------------------------------------------
# The CMakeLists.txt for:                  pDeployHandler
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
   DeployHandler.cpp
   DeployHandlerMain.cpp
)

ADD_EXECUTABLE(pDeployHandler ${SRC})

TARGET_LINK_LIBRARIES(pDeployHandler
   MOOS
   MOOSGen
   mbutil
   m
   pthread
   )