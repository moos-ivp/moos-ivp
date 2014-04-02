#this builds an application which can be used
#to probe and play with MOOS comms

include_directories( ${MOOS_INCLUDE_DIRS} ${MOOS_DEPEND_INCLUDE_DIRS})
add_executable(ktm ktm.cpp )
target_link_libraries(ktm ${MOOS_LIBRARIES} ${MOOS_DEPEND_LIBRARIES})

INSTALL(TARGETS ktm
  RUNTIME DESTINATION bin
)
