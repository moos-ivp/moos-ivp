PROJECT(IVP_APP_GZAICVIEW)

INCLUDE(FindFLTK)    # This is distributed with CMake.
IF(FLTK_FOUND)
   FILE(GLOB SRC *.cpp)
   
   ADD_EXECUTABLE(gzaicview ${SRC})
   
   TARGET_LINK_LIBRARIES(gzaicview
      ivpbuild
      mbutil
      ivpcore
      m
      fltk 
      fltk_gl 
      X11
      GL
      tiff
      )
ELSE(FLTK_FOUND)
   MESSAGE(STATUS 
      "Won't build gzaicview because can't find needed FLTK stuff.  "
      "You can maybe fix this by setting the FLTK_DIR and FLTK_INCLUDE_DIR CMake variables "
      "because CMake's FindFLTK implementation is pretty stupid."
      )
ENDIF(FLTK_FOUND)

