#--------------------------------------------------------
# The CMakeLists.txt for:                    app_domquery
# Author(s):                   Benjamin, Schmidt, Leonard
#--------------------------------------------------------

# Set System Specific Libraries
if (${WIN32})
  SET(SYSTEM_LIBS
    wsock32)
else (${WIN32})
  SET(SYSTEM_LIBS
    m)
endif (${WIN32})

SET(SRC
   DomQuery.cpp
   main.cpp
)
 
ADD_EXECUTABLE(domquery ${SRC})
     
TARGET_LINK_LIBRARIES(domquery
  mbutil 
  ivpbuild
  ivpcore
  ${SYSTEM_LIBS})

#Install Targets
INSTALL(TARGETS domquery
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)

