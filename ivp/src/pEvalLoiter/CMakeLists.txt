#--------------------------------------------------------
# The CMakeLists.txt for:                     pEvalLoiter
# Author(s):                                Mike Benjamin            
#--------------------------------------------------------

SET(SRC
  EvalLoiter.cpp
  EvalLoiter_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pEvalLoiter ${SRC})

TARGET_LINK_LIBRARIES(pEvalLoiter
   ${MOOS_LIBRARIES}
   apputil
   geometry
   mbutil
   logic
   contacts
   m
   pthread)

