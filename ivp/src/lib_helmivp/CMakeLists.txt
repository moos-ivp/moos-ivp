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

ADD_LIBRARY(helmivp ${SRC})
