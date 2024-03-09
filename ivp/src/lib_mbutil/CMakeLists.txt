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
  VQuals.cpp
  HashUtils.cpp
  MacroUtils.cpp
  TermUtils.cpp
  StringTree.cpp
  StringNode.cpp
  VarDataPair.cpp
  VarDataPairUtils.cpp
  AckMessage.cpp
  NodeMessage.cpp
  NodeMessageUtils.cpp
  Odometer.cpp
  Figlog.cpp
  TStamp.cpp
  ReleaseInfo.cpp
  MailFlagSet.cpp
  LatLonFormatUtils.cpp
  OpenURL.cpp
  BundleOut.cpp
  )

SET(HEADERS
  ColorPack.h
  ColorParse.h
  FColorMap.h
  FileBuffer.h
  MBTimer.h
  MBUtils.h
  VQuals.h
  HashUtils.h
  MacroUtils.h
  StringNode.h
  StringTree.h
  TermUtils.h
  VarDataPair.h
  VarDataPairUtils.h
  NodeMessage.h
  NodeMessageUtils.h
  Odometer.h
  TStamp.h
  ReleaseInfo.h
  ScopeEntry.h
  MailFlagSet.h
  LatLonFormatUtils.h
  OpenURL.h
  BundleOut.h
)

# Build Library
ADD_LIBRARY(mbutil ${SRC})

