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
IVP_ADD_LIBRARY(ucommand ${SRC})
TARGET_LINK_LIBRARIES(ucommand
  PUBLIC "$<BUILD_INTERFACE:ivp_fltk>$<INSTALL_INTERFACE:IVP::FLTK>"
  PRIVATE apputil mbutil)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-cast-function-type ")
