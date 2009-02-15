#--------------------------------------------------------
# The CMakeLists.txt for:                         lib_avd
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
   AVD_Table.cpp
   AVD_Regressor.cpp
)

ADD_LIBRARY(avd ${SRC})
