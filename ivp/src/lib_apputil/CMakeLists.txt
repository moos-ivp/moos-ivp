#--------------------------------------------------------
# The CMakeLists.txt for:                     lib_apputil
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  AppCastRepo.cpp
  AppCastSet.cpp
  AppCastTree.cpp
  ACTable.cpp
  ACBlock.cpp
  InfoCastSettings.cpp
)

SET(HEADERS
  AppCastRepo.h
  AppCastSet.h
  AppCastTree.h
  ACBlock.h
  ACTable.h
  InfoCastSettings.h
)

# Build Library
IVP_ADD_LIBRARY(apputil ${SRC})
TARGET_LINK_LIBRARIES(apputil
  PUBLIC mbutil MOOS::MOOS)
