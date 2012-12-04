#--------------------------------------------------------
# The CMakeLists.txt for:                     lib_apputil
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  AppCastRepo.cpp
  AppCastSet.cpp
  AppCastTree.cpp
  AppMsg.cpp
  ACTable.cpp
  ACBlock.cpp
)

SET(HEADERS
  AppCastRepo.h
  AppCastSet.h
  AppCastTree.h
  AppMsg.h
  ACTable.h
  ACBlock.h
  ACTable.h
)

# Build Library
ADD_LIBRARY(apputil ${SRC})

