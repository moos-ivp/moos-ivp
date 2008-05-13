#--------------------------------------------------------
# The CMakeLists.txt for:                     lib_helmivp
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
   BehaviorFactory.cpp
   HelmEngine.cpp
   HelmReport.cpp
   Populator_BehaviorSet.cpp
   Populator_BehaviorSet2.cpp
)

ADD_LIBRARY(helmivp ${SRC})
