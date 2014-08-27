#--------------------------------------------------------
# The CMakeLists.txt for:                    lib_geometry
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

SET(HEADERS
  AngleUtils.h
  ArtifactUtils.h
  ConvexHullGenerator.h
  CircularUtils.h
  CPAEngine.h
  CPA_Utils.h
  GeomUtils.h
  PathUtils.h
  CurrentField.h
  XYRangePulse.h
  XYCommsPulse.h
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
  XYFormatUtilsRangePulse.h
  XYFormatUtilsCommsPulse.h
  XYFormatUtilsHazard.h
  XYFormatUtilsHazardSet.h
  XYGrid.h
  XYConvexGrid.h
  XYHazard.h
  XYHazardSet.h
  XYHexagon.h
  XYHexGrid.h
  XYObject.h
  XYPatternBlock.h
  XYFieldGenerator.h
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

