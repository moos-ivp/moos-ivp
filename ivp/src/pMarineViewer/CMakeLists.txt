#--------------------------------------------------------
# The CMakeLists.txt for:                   pMarineViewer
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)
  
ADD_EXECUTABLE(pMarineViewer ${SRC})
     
TARGET_LINK_LIBRARIES(pMarineViewer         
  MOOS
  MOOSGen
  MOOSUtility
  marineview
  mbutil
  genutil
  geometry
  m
   
  # Standard libraries used by this project's FLTK apps...
  fltk 
  fltk_gl 
  tiff)
