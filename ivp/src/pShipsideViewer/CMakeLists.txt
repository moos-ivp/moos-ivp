#--------------------------------------------------------
# The CMakeLists.txt for:                   pMarineViewer
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)
  
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
