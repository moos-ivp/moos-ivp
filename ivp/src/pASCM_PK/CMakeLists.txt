#--------------------------------------------------------
# The CMakeLists.txt for:                      pASCM_PK
# Author(s):                                   Phong TRan
#--------------------------------------------------------

SET(SRC
   AscmContact.cpp
   MOOS_ASCM_PK.cpp
   PkContacts.cpp
   mainASCM_PK.cpp
)

ADD_EXECUTABLE(pASCM_PK ${SRC})

TARGET_LINK_LIBRARIES(pASCM_PK 
MOOS
MOOSGen
MOOSUtility
mbutil
ais
m
pthread)
