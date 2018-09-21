#--------------------------------------------------------
# The CMakeLists.txt for:                    lib_ucommand
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  CommandFolio.cpp
  CommandItem.cpp
  CommandSummary.cpp
  UCMD_GUI.cpp
)

SET(HEADERS
  CommandFolio.h
  CommandItem.h
  UCMD_GUI.h
)

# Build Library
ADD_LIBRARY(ucommand ${SRC})

