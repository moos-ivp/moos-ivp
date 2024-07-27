#this builds an application which can be used
#to probe and play with MOOS comms

add_executable(ktm ktm.cpp)
target_link_libraries(ktm MOOS)

install(TARGETS ktm
  RUNTIME DESTINATION bin
)
