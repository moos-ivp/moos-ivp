INCLUDE(FindFLTK)    # This is distributed with CMake.

OPTION(IVP_BUILD_zaicview "Build program zaicview" ON)

IF("${IVP_BUILD_zaicview}" STREQUAL "ON")
   IF(${FLTK_FOUND})
      FILE(GLOB SRC *.cpp)
   
      ADD_EXECUTABLE(zaicview ${SRC})
      
      TARGET_LINK_LIBRARIES(zaicview 
         
   mbutil
   ivpbuild
   ivpcore
   m
   
         # Standard libraries used by this project's FLTK apps...
         fltk 
         fltk_gl 
         X11
         GL
         tiff      
         )

      IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
         MESSAGE(STATUS "zaicview: IVP_PACKAGE_VERSION not set, so not creating install component.")
      ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
         INSTALL(
            TARGETS zaicview
            DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
            COMPONENT )
      ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

   ELSE(${FLTK_FOUND})
      MESSAGE(STATUS
         "Won't build zaicview because can't find needed FLTK stuff.  "
         "You can maybe fix this by setting the FLTK_DIR and FLTK_INCLUDE_DIR CMake variables "
         "because CMake's FindFLTK implementation is pretty stupid."
         )
   ENDIF(${FLTK_FOUND})
ENDIF("${IVP_BUILD_zaicview}" STREQUAL "ON")
