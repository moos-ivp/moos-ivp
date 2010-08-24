#--------------------------------------------------------
# The CMakeLists.txt for:                    lib_geometry
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC  
   AngleUtils.cpp        ArtifactUtils.cpp      
   CPAEngine.cpp         CPA_Utils.cpp          
   CircularUtils.cpp     GeomUtils.cpp          
   IO_GeomUtils.cpp      LinearExtrapolator.cpp 
   XYArc.cpp             XYFormatUtilsPoint.cpp
   XYCircle.cpp          XYEncoders.cpp         
   XYGrid.cpp            XYHexGrid.cpp          
   XYHexagon.cpp         XYPolygon.cpp          
   XYSegList.cpp         XYSquare.cpp         
   XYArtifactGrid.cpp    XYPoint.cpp
   XYFormatUtilsSegl.cpp XYFormatUtilsPoly.cpp
   XYSegment.cpp         XYPatternBlock.cpp
   XYObject.cpp          XYFormatUtilsCircle.cpp
)

SET(HEADERS
  AngleUtils.h
  ArtifactUtils.h
  CircularUtils.h
  CPAEngine.h
  CPA_Utils.h
  GeomUtils.h
  IO_GeomUtils.h
  LinearExtrapolator.h
  XYArc.h
  XYArtifactGrid.h
  XYBuildUtils.h
  XYCircle.h
  XYEncoders.h
  XYFormatUtilsCircle.h
  XYFormatUtilsPoint.h
  XYFormatUtilsPoly.h
  XYFormatUtilsSegl.h
  XYGrid.h
  XYHexagon.h
  XYHexGrid.h
  XYObject.h
  XYPatternBlock.h
  XYPoint.h
  XYPolygon.h
  XYSegList.h
  XYSegment.h
  XYSquare.h
)

# Build Library
ADD_LIBRARY(geometry ${SRC})

# Install Headers
INSTALL( FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/lib_geometry)

# Install Libararies
INSTALL( TARGETS geometry
   RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
   LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
   ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)
