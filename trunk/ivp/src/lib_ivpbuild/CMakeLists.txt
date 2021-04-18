#--------------------------------------------------------
# The CMakeLists.txt for:                    lib_ivpbuild
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  AOF.cpp
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
  PDMapBuilder.cpp
  OF_Coupler.cpp
  OF_Rater.cpp
  OF_Reflector.cpp
  PQueue.cpp
  RT_Directed.cpp
  RT_Smart.cpp
  RT_Uniform.cpp
  RT_UniformX.cpp
  RT_Evaluator.cpp
  RT_AutoPeak.cpp
  Regressor.cpp
  ZAIC_SPD.cpp
  ZAIC_HEQ.cpp
  ZAIC_HLEQ.cpp
  ZAIC_LEQ.cpp
  ZAIC_PEAK.cpp
  ZAIC_HDG.cpp
  ZAIC_Vector.cpp
  )

SET(HEADERS
  AOF_Gaussian.h
  AOF.h
  AOF_Linear.h
  AOF_MGaussian.h
  AOF_Quadratic.h
  AOF_Ring.h
  AOF_Rings.h
  AOF_SCALAR.h
  BuildUtils.h
#  Demuxer.h
#  DemuxUnit.h
  FunctionEncoder.h
  FunctionEncoderMK.h
  IO_Utilities.h
  PDMapBuilder.h
  OF_Coupler.h
  OF_Rater.h
  OF_Reflector.h
  PQueue.h
  Regressor.h
  RT_AutoPeak.h
  RT_Directed.h
  RT_Evaluator.h
  RT_Smart.h
  RT_UniformX.h
  ZAIC_SPD.h
  ZAIC_HEQ.h
  ZAIC_HLEQ.h
  ZAIC_LEQ.h
  ZAIC_PEAK.h
  ZAIC_SPD.h
  ZAIC_Vector.h
)

# Build Library
ADD_LIBRARY(ivpbuild ${SRC})
TARGET_LINK_LIBRARIES(ivpbuild ivpcore)


