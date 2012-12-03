#--------------------------------------------------------
# The CMakeLists.txt for:                     pSearchGrid
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pSearchGrid ${SRC})

TARGET_LINK_LIBRARIES(pSearchGrid
   ${MOOS_LIBRARIES}
   geometry
   mbutil
   contacts
   ${SYSTEM_LIBS}
)

