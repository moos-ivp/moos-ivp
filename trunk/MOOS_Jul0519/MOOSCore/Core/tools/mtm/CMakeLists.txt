#this builds an application which can be used
#to monitor the moos

add_executable(mtm mtm)
target_link_libraries(mtm MOOS)

install(TARGETS mtm
  RUNTIME DESTINATION bin
)
