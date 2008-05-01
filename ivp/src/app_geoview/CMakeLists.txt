#--------------------------------------------------------
# The CMakeLists.txt for:                         geoview
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

IF(BUILD_GUIS)

	FILE(GLOB SRC *.cpp)
	  
	ADD_EXECUTABLE(geoview ${SRC})
	     
	TARGET_LINK_LIBRARIES(geoview
	  marineview
	  mbutil
	  geometry
	  m
	   
	  # Standard libraries used by this project's FLTK apps...
	  fltk 
	  fltk_gl 
	  tiff)
  
ENDIF(BUILD_GUIS)
