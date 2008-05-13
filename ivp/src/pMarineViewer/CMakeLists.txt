#--------------------------------------------------------
# The CMakeLists.txt for:                   pMarineViewer
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

IF(BUILD_GUIS)

   SET(SRC
      PMV_GUI.cpp
      PMV_MOOSApp.cpp
      PMV_Viewer.cpp
      main_mviewer.cpp
   )

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
  
ENDIF(BUILD_GUIS)
