#--------------------------------------------------------
# The CMakeLists.txt for:                    lib_geometry
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC  
   AngleUtils.cpp         AngleUtils.h         
   ArtifactUtils.cpp      ArtifactUtils.h      
   CPAEngine.cpp          CPAEngine.h          
   CPA_Utils.cpp          CPA_Utils.h          
   CircularUtils.cpp      CircularUtils.h      
   GeomUtils.cpp          GeomUtils.h          
   IO_GeomUtils.cpp       IO_GeomUtils.h       
   LinearExtrapolator.cpp LinearExtrapolator.h 
   XYArc.cpp              XYArc.h              
   XYBuildUtils.cpp       XYBuildUtils.h       
   XYCircle.cpp           XYCircle.h           
   XYEncoders.cpp         XYEncoders.h         
   XYGrid.cpp             XYGrid.h             
   XYHexGrid.cpp          XYHexGrid.h          
   XYHexagon.cpp          XYHexagon.h          
   XYPolygon.cpp          XYPolygon.h          
   XYSegList.cpp          XYSegList.h          
   XYSquare.cpp           XYSquare.h           
)

ADD_LIBRARY(geometry ${SRC})
