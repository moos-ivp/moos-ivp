#this builds the pMOOSBridge application
set(EXECNAME pMOOSBridge)

find_package(MOOS 10.0 )

#what files are needed?
SET(SRCS MOOSBridgeMain.cpp MOOSCommunity.cpp MOOSBridge.cpp  MOOSCommunity.h MOOSBridge.h MOOSUDPLink.cpp MOOSUDPLink.h )

include_directories( ${${EXECNAME}_INCLUDE_DIRS} ${MOOS_INCLUDE_DIRS} ${MOOS_DEPEND_INCLUDE_DIRS})
add_executable(${EXECNAME} ${SRCS} )
target_link_libraries(${EXECNAME} ${MOOS_LIBRARIES} ${MOOS_DEPEND_LIBRARIES})


INSTALL(TARGETS ${EXECNAME}
  RUNTIME DESTINATION bin
)
