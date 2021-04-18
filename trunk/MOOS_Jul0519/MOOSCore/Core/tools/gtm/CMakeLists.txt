#this builds an application which can be used
#to graph MOOS connectivity

add_executable(gtm gtm.cpp)
target_link_libraries(gtm MOOS)

install(TARGETS gtm
  RUNTIME DESTINATION bin
)
