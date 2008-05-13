#--------------------------------------------------------
# The CMakeLists.txt for:                      app_ffview
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

IF(BUILD_GUIS)

   SET(SRC
      AOF_Cache.cpp
      GUI.cpp
      PopulatorAOF.cpp
      Viewer.cpp
      main.cpp
   )	  

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

ENDIF(BUILD_GUIS)
