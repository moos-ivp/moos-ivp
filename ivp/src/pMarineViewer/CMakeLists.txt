PROJECT(IVP_PMARINEVIEWER)

INCLUDE(FindFLTK)    # This is distributed with CMake.
IF(FLTK_FOUND)
   FILE(GLOB SRC *.cpp)
   
   ADD_EXECUTABLE(pMarineViewer ${SRC})
   
   
   TARGET_LINK_LIBRARIES(pMarineViewer
      MOOS
      MOOSGen
      marineview
      mbutil
      geometry
      m
      pthread
      fltk 
      fltk_gl 
      X11
      GL
      tiff
      )
ELSE(FLTK_FOUND)
   MESSAGE(STATUS 
      "Won't build pMarineViewer because can't find needed FLTK stuff.  "
      "You can maybe fix this by setting the FLTK_DIR CMake variable "
      "to a value of "/usr/lib", because CMake's FindFLTK implementation "
      "is pretty stupid."
      )
ENDIF(FLTK_FOUND)

