#this builds an application which can be used
#to monitor the moos

include_directories( ${MOOS_INCLUDE_DIRS} ${MOOS_DEPEND_INCLUDE_DIRS})
add_executable(mtm mtm )
target_link_libraries(mtm ${MOOS_LIBRARIES} ${MOOS_DEPEND_LIBRARIES})

INSTALL(TARGETS mtm
  RUNTIME DESTINATION bin
)
