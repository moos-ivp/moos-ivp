#--------------------------------------------------------
# The CMakeLists.txt for:                     lib_helmivp
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  HelmReport.cpp
  HelmReportUtils.cpp
  ModeSet.cpp
  ModeEntry.cpp
  Populator_BehaviorSet.cpp
  BehaviorSet.cpp
  BFactoryStatic.cpp
  BFactoryDynamic.cpp
  BehaviorSpec.cpp
  BehaviorReport.cpp
  SpecBuild.cpp
#  KCache.cpp
#  Populator_KCache.cpp
)

SET(HEADERS
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
#  KCache.h
#  Populator_KCache.h
)

# Build Library
ADD_LIBRARY(helmivp ${SRC})

