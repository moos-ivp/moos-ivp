#--------------------------------------------------------
# The CMakeLists.txt for:                    lib_ivpbuild
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
   AOF_Linear.cpp
   AOF_Quadratic.cpp
   AOF_Ring.cpp
   AOF_Rings.cpp
   AOF_Gaussian.cpp
   AOF_MGaussian.cpp
   BuildUtils.cpp
   DemuxUnit.cpp
   Demuxer.cpp
   FunctionEncoder.cpp
   FunctionEncoderMK.cpp
   IO_Utilities.cpp
   OF_Coupler.cpp
   OF_Rater.cpp
   OF_Reflector.cpp
   PQueue.cpp
   RT_Focus.cpp
   RT_Priority.cpp
   RT_Uniform.cpp
   Regressor.cpp
   ZAIC_HEQ.cpp
   ZAIC_HLEQ.cpp
   ZAIC_LEQ.cpp
   ZAIC_PEAK.cpp
)

ADD_LIBRARY(ivpbuild ${SRC})

