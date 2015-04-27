#--------------------------------------------------------
# The CMakeLists.txt for:                 lib_henrik_util
# Author(s):                               Henrik Schmidt
#--------------------------------------------------------

FILE(GLOB SRC *.cpp *.c)

ADD_LIBRARY(henrik_util SHARED ${SRC})

TARGET_LINK_LIBRARIES(henrik_util ${MOOS_LIBRARIES} mbutil)
