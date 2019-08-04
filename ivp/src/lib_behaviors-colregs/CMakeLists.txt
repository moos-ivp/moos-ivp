#--------------------------------------------------------
# The CMakeLists.txt for:           lib_behaviors-colregs
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  BHV_AvdColregsV17.cpp
  BHV_AvdColregsV19.cpp
  AOF_R13.cpp
  AOF_R14.cpp
  AOF_R16.cpp
  AOF_R17.cpp
  AOF_CPA.cpp
)

SET(HEADERS 
  BHV_AvdColregsV17.h
  BHV_AvdColregsV19.h
  AOF_R13.h
  AOF_R14.h
  AOF_R16.h
  AOF_R17.h
  AOF_CPA.h
) 

ADD_LIBRARY(behaviors-colregs ${SRC})

TARGET_LINK_LIBRARIES(behaviors-colregs
   mbutil
   helmivp
   behaviors
   geometry
   )

