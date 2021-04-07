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
  MacroUtils.cpp
  TermUtils.cpp
  StringTree.cpp
  StringNode.cpp
  VarDataPair.cpp
  VarDataPairUtils.cpp
  Odometer.cpp
  Figlog.cpp
  TStamp.cpp
  ReleaseInfo.cpp
  MailFlagSet.cpp
  LatLonFormatUtils.cpp
  )

SET(HEADERS
  ColorPack.h
  ColorParse.h
  FColorMap.h
  FileBuffer.h
  MBTimer.h
  MBUtils.h
  MacroUtils.h
  StringNode.h
  StringTree.h
  TermUtils.h
  VarDataPair.h
  VarDataPairUtils.h
  Odometer.h
  TStamp.h
  ReleaseInfo.h
  ScopeEntry.h
  MailFlagSet.h
  LatLonFormatUtils.h
)

# Build Library
ADD_LIBRARY(mbutil ${SRC})

