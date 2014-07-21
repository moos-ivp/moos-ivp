#--------------------------------------------------------
# The CMakeLists.txt for:                            iSay
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  Sayer.cpp
  Sayer_Info.cpp
  Utterance.cpp
  VoiceUtils.cpp
  UtterQueue.cpp
  main.cpp
)

ADD_EXECUTABLE(iSay ${SRC})

TARGET_LINK_LIBRARIES(iSay
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   genutil
   m
   pthread)

