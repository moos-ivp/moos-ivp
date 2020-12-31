#--------------------------------------------------------
# The CMakeLists.txt for:                          pRealm
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  Realm.cpp
  Realm_Info.cpp
  PipeWay.cpp
  main.cpp
)

ADD_EXECUTABLE(pRealm ${SRC})

TARGET_LINK_LIBRARIES(pRealm
   ${MOOS_LIBRARIES}
   realm
   apputil
   mbutil
   m
   pthread)

