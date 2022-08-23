#this builds an application which can be used
#to monitor the QoS in moos

add_executable(mqos mqos.cpp)
target_link_libraries(mqos MOOS)

install(TARGETS mqos
  RUNTIME DESTINATION bin
)
