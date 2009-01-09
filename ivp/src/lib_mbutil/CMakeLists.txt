#--------------------------------------------------------
# The CMakeLists.txt for:                      lib_mbutil
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  ColorParse.cpp
  FColorMap.cpp
  FileBuffer.cpp
  MBTimer.cpp
  MBUtils.cpp
  TermUtils.cpp
  StringTree.cpp
  StringNode.cpp
)

# SET_SOURCE_FILES_PROPERTIES(
#    MBUtils.cpp 
#    PROPERTIES OBJECT_DEPENDS ${IVP_SRC_DIR_ABSOLUTE}/svn_revision_number.h
#    )

ADD_LIBRARY(mbutil ${SRC})
