#--------------------------------------------------------
# The CMakeLists.txt for:                         lib_avk
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

ADD_LIBRARY(avk
  AVD_Table.cpp   
  AVD_Entry.cpp
  PopulatorAvdTable.cpp
  PopulatorAvkTable.cpp
  AVK_Table.cpp
  CurveFitting.cpp
  TurnPoly.cpp
  TurnSequence.cpp
)
