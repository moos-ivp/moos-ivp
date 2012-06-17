#--------------------------------------------------------
# The CMakeLists.txt for:                      lib_mbutil
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  ColorParse.cpp
  ColorPack.cpp
  FColorMap.cpp
  FileBuffer.cpp
  MBTimer.cpp
  MBUtils.cpp
  TermUtils.cpp
  StringTree.cpp
  StringNode.cpp
  VarDataPair.cpp
  Figlog.cpp
  ReleaseInfo.cpp
)

SET(HEADERS
  ColorPack.h
  ColorParse.h
  FColorMap.h
  FileBuffer.h
  MBTimer.h
  MBUtils.h
  StringNode.h
  StringTree.h
  TermUtils.h
  VarDataPair.h
  ReleaseInfo.h
)

# SET_SOURCE_FILES_PROPERTIES(
#    MBUtils.cpp 
#    PROPERTIES OBJECT_DEPENDS ${IVP_SRC_DIR_ABSOLUTE}/svn_revision_number.h
#    )


# Build Library
ADD_LIBRARY(mbutil ${SRC})

