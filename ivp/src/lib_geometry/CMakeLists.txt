#--------------------------------------------------------
# The CMakeLists.txt for:                    lib_geometry
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC  
   AngleUtils.cpp        ArtifactUtils.cpp      
   CPAEngine.cpp         CPA_Utils.cpp          
   CircularUtils.cpp     GeomUtils.cpp          
   IO_GeomUtils.cpp      LinearExtrapolator.cpp 
   XYArc.cpp             XYBuildUtils.cpp       
   XYCircle.cpp          XYEncoders.cpp         
   XYGrid.cpp            XYHexGrid.cpp          
   XYHexagon.cpp         XYPolygon.cpp          
   XYSegList.cpp         XYSquare.cpp         
   XYArtifactGrid.cpp  
)

ADD_LIBRARY(geometry ${SRC})
