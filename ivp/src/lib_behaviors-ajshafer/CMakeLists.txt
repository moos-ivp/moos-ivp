#--------------------------------------------------------
# The CMakeLists.txt for:          lib_behaviors-ajshafer
# Author(s):                                Andrew Shafer
#--------------------------------------------------------

FILE(GLOB SRC
	BHV_Lawnmower.cpp  
	AOF_SearchArtifact.cpp BHV_SearchArtifact.cpp)

ADD_LIBRARY(behaviors-ajshafer ${SRC})

