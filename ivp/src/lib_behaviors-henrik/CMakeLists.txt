#--------------------------------------------------------
# The CMakeLists.txt for:            lib_behaviors-henrik
# Author(s):                               Henrik Schmidt
#--------------------------------------------------------

SET(SRC
   AOF_HArrayTurn.cpp
   AOF_HCutRangeCPA.cpp
   AOF_HShadow.cpp
   BHV_ClassifyYoyo.cpp
   BHV_ConstantAltitude.cpp
   BHV_HArrayAngle.cpp
   BHV_HArrayTurn.cpp
   BHV_HCloseRange.cpp
   BHV_HCutRange.cpp
   BHV_HPeriodicSurface.cpp
   BHV_HShadow.cpp
   BHV_HTrail.cpp
   BHV_MinAltitude.cpp
   BHV_RaceTrack.cpp
   BHV_ZigZag.cpp
)

ADD_LIBRARY(behaviors-henrik ${SRC})
