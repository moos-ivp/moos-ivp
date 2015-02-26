#--------------------------------------------------------
# The CMakeLists.txt for:                   uFldPathCheck
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)
ADD_EXECUTABLE(uFldPathCheck ${SRC})

TARGET_LINK_LIBRARIES(uFldPathCheck
   ${MOOS_LIBRARIES}
   mbutil
   apputil
   contacts
   geometry
   m
   pthread)

