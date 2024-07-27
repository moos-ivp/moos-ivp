#this builds an application which can be used
#to audit MOOS comms

add_executable(atm atm.cpp)
target_link_libraries(atm MOOS)

install(TARGETS atm
  RUNTIME DESTINATION bin
)
