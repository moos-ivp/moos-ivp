#--------------------------------------------------------
# The CMakeLists.txt for:                      lib_survey
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  RectHullGenerator.cpp
  SurveyGenerator.cpp
)

SET(HEADERS
  RectHullGenerator.h
  SurveyGenerator.cpp
)

# Build Library
ADD_LIBRARY(survey ${SRC})

