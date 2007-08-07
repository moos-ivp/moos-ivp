PROJECT(IVP_UFUNCTIONVIS)

INCLUDE(FindFLTK)    # This is distributed with CMake.
IF(FLTK_FOUND)
   FILE(GLOB SRC *.cpp)
   
   ADD_EXECUTABLE(uFunctionVis ${SRC})
   
   TARGET_LINK_LIBRARIES(uFunctionVis
      MOOS
      MOOSGen
      ipfview
      geometry
      mbutil
      ivpbuild
      ivpcore
      m
      pthread
      fltk 
      fltk_gl 
      X11
      GL
      )
ELSE(FLTK_FOUND)
   MESSAGE(STATUS 
      "Won't build uFunctionVis because can't find needed FLTK stuff.  "
      "You can maybe fix this by setting the FLTK_DIR CMake variable "
      "to a value of "/usr/lib", because CMake's FindFLTK implementation "
      "is pretty stupid."
      )
ENDIF(FLTK_FOUND)

