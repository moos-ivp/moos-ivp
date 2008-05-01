#--------------------------------------------------------
# The CMakeLists.txt for:                     app_logview
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

IF(BUILD_GUIS)

	FILE(GLOB SRC *.cpp)
	  
	ADD_EXECUTABLE(logview ${SRC})
	     
	TARGET_LINK_LIBRARIES(logview
	  marineview
	  navplot
	  geometry
	  mbutil
	  m

	  # Standard libraries used by this project's FLTK apps...
	  fltk 
	  fltk_gl 
	  tiff)
	  
ENDIF(BUILD_GUIS)
