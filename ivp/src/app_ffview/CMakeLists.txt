#--------------------------------------------------------
# The CMakeLists.txt for:                      app_ffview
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)
  
ADD_EXECUTABLE(ffview ${SRC})
     
TARGET_LINK_LIBRARIES(ffview
  bhvutil
  ipfview
  behaviors
  geometry
  mbutil
  ivpbuild
  ivpcore
  m
   
  # Standard libraries used by this project's FLTK apps...
  fltk 
  fltk_gl 
  tiff)

