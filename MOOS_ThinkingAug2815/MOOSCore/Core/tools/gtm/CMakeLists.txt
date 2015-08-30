#this builds an application which can be used
#to graph MOOS connectivity

include_directories( ${MOOS_INCLUDE_DIRS} ${MOOS_DEPEND_INCLUDE_DIRS})
add_executable(gtm gtm.cpp )
target_link_libraries(gtm ${MOOS_LIBRARIES} ${MOOS_DEPEND_LIBRARIES})

INSTALL(TARGETS gtm
  RUNTIME DESTINATION bin
)
