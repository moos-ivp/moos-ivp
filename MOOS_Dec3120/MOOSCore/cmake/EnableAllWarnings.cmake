# This script turns on all compiler warnings

if(NOT CMAKE_CXX_COMPILER_ID)
  # CMAKE_CXX_COMPILER_ID doesn't get set until after the
  # 'project' function has been called.
  message(AUTHOR_WARNING
    "You must not include the 'EnableAllWarnings' module until "
    "*AFTER* the 'project' function has been invoked."
  )
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
    CMAKE_CXX_COMPILER_ID MATCHES "Clang")

  if( NOT CMAKE_CXX_FLAGS MATCHES "-Wall" )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall" )
  endif()
  if( NOT CMAKE_C_FLAGS MATCHES "-Wall" )
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
  endif()

  if( NOT CMAKE_CXX_FLAGS MATCHES "-Wextra" )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra" )
  endif()
  if( NOT CMAKE_C_FLAGS MATCHES "-Wextra" )
    set( CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -Wextra" )
  endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "(MSVC|Intel)")
  # Use the highest warning level for visual studio.
  if( CMAKE_CXX_FLAGS MATCHES "/W[0-4]" )
    string( REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" )
  else()
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4" )
  endif()

  if( CMAKE_C_FLAGS MATCHES "/W[0-4]" )
    string( REGEX REPLACE "/W[0-4]" "/W4" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" )
  else()
    set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4" )
  endif()

  # Disable deprecation warnings for standard C functions in VS2005 and later
  if( CMAKE_COMPILER_2005 )
    add_definitions( -D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE )
  endif()
endif()
