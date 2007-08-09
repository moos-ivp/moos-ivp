PROJECT(IVP_UVOICE)

INCLUDE(FindSWIG)
INCLUDE(FindPythonLibs)

IF(NOT SWIG_FOUND)
  MESSAGE(STATUS "Won't be able to buil uVoice's _MOOS library, because "
    "CMake can't find the 'swig' program that's used to my Python wrappers "
    "for C++ libraries.")
ENDIF(NOT SWIG_FOUND)

IF("${PYTHON_INCLUDE_PATH}" STREQUAL "")
  SET(PYTHON_INCLUDE_PATH_FOUND 0)
  MESSAGE(STATUS "Won't be able to buil uVoice's _MOOS library, because "
    "CMake can't find the Python.h.  If you know where it is, you should be "
    "able to fix this by setting the CMake variable PYTHON_INCLUDE_PATH "
    "to the directory containing the right version of Python.h")
ELSE("${PYTHON_INCLUDE_PATH}" STREQUAL "")
  SET(PYTHON_INCLUDE_PATH_FOUND 1)
ENDIF("${PYTHON_INCLUDE_PATH}" STREQUAL "")

IF(${SWIG_FOUND} AND ${PYTHON_INCLUDE_PATH_FOUND})
  INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})

  ADD_CUSTOM_COMMAND(
    OUTPUT MOOS_wrap.cxx
    COMMAND ${SWIG_EXECUTABLE} -python -c++ -shadow -dirprot MOOS.i
    DEPENDS MOOS.i
    )

  ADD_LIBRARY(_MOOS SHARED
    MOOS_wrap.cxx
    )
ENDIF(${SWIG_FOUND} AND ${PYTHON_INCLUDE_PATH_FOUND})

