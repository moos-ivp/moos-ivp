#--------------------------------------------------------
# The CMakeLists.txt for:                   pMarineViewer
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC
  SSV_GUI.cpp
  SSV_MOOSApp.cpp
  SSV_Viewer.cpp
  main_ssviewer.cpp)
  
ADD_EXECUTABLE(pShipsideViewer ${SRC})
     
TARGET_LINK_LIBRARIES(pShipsideViewer
   MOOS
   MOOSGen
   MOOSUtility
   marineview
   mbutil
   genutil
   geometry
   m
   pthread
   X11
   GL
   fltk 
   fltk_gl 
   tiff
   )
