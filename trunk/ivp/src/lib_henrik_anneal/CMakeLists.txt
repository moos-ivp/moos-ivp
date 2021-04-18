#--------------------------------------------------------
# The CMakeLists.txt for:                 lib_henrik_anneal
# Author(s):                               Henrik Schmidt
#--------------------------------------------------------

FILE(GLOB SRC *.cpp *.c)

ADD_LIBRARY(henrik_anneal SHARED ${SRC})

TARGET_LINK_LIBRARIES(henrik_anneal ${MOOS_LIBRARIES} mbutil)
