#--------------------------------------------------------
# The CMakeLists.txt for:                     lib_genutil
# Author(s):                             Christian Convey
#--------------------------------------------------------

SET(SRC
   MOOSAppRunnerThread.cpp
   fileutil.cpp
   stringutil.cpp
)

ADD_LIBRARY(genutil ${SRC})

