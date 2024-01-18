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
  PlatModelGenerator.cpp
  PMGen.cpp
  PMGen_Dubins.cpp
  PMGen_Holonomic.cpp
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
  PlatModelGenerator.h
  PMGen.h
  PMGen_Dubins.h
  PMGen_Holonomic.h
)

# Build Library
ADD_LIBRARY(helmivp ${SRC})

