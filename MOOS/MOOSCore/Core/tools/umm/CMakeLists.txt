#this builds an application which can be used
#to probe and play with MOOS comms

add_executable(umm umm.cpp)
target_link_libraries(umm MOOS)

install(TARGETS umm
  RUNTIME DESTINATION bin
)
