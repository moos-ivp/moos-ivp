#--------------------------------------------------------
# The CMakeLists.txt for:                    lib_geometry
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

SET(HEADERS
  AngleUtils.h
  ArtifactUtils.h
  CircularUtils.h
  CPAEngine.h
  CPA_Utils.h
  GeomUtils.h
  CurrentField.h
  XYRangePulse.h
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
  XYFormatUtilsVector.h
  XYFormatUtilsMarker.h
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
  XYVector.h
)

# Build Library
ADD_LIBRARY(geometry ${SRC})

TARGET_LINK_LIBRARIES( geometry
   mbutil
   )


# Install Headers
INSTALL( FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/lib_geometry)

# Install Libararies
INSTALL( TARGETS geometry
   RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
   LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
   ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)
