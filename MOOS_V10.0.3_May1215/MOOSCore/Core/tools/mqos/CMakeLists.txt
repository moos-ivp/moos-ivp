#this builds an application which can be used
#to monitor the QoS in moos

include_directories( ${MOOS_INCLUDE_DIRS} ${MOOS_DEPEND_INCLUDE_DIRS})
add_executable(mqos mqos.cpp )
target_link_libraries(mqos ${MOOS_LIBRARIES} ${MOOS_DEPEND_LIBRARIES})

INSTALL(TARGETS mqos
  RUNTIME DESTINATION bin
)
