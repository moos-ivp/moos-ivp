#--------------------------------------------------------
# The CMakeLists.txt for:                  pFrontEstimate
# Author(s):                               Henrik Schmidt
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pFrontEstimate ${SRC})
   
TARGET_LINK_LIBRARIES(pFrontEstimate
   ${MOOS_LIBRARIES}
   mbutil
   ufield
   m
   pthread
   henrik_anneal
)
