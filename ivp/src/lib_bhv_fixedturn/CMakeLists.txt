#--------------------------------------------------------
# The CMakeLists.txt for:               lib_bhv_fixedturn
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  BHV_FixedTurn.cpp	
  FixedTurn.cpp
  FixedTurnSet.cpp
)

SET(HEADERS
  BHV_FixedTurn.h
  FixedTurn.h
  FixedTurnSet.h
)

# Build Library
ADD_LIBRARY(bhv_fixedturn ${SRC})

