#--------------------------------------------------------
# The CMakeLists.txt for:                       lib_realm
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  RealmCast.cpp
  RealmCastSet.cpp
  RealmCastTree.cpp
  RealmRepo.cpp
  RealmSummary.cpp
  WatchCast.cpp
  WatchCluster.cpp
)

SET(HEADERS
  RealmCast.h
  RealmCastSet.h
  RealmCastTree.h
  RealmRepo.h
  RealmSummary.h
  WatchCast.h
  WatchCluster.h
)

# Build Library
ADD_LIBRARY(realm ${SRC})

