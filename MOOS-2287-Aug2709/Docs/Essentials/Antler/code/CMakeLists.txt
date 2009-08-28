PROJECT(AntlerTestApps)


# Add executable
ADD_EXECUTABLE( pAntlerTestAppA A.cpp)
ADD_EXECUTABLE( pAntlerTestAppB B.cpp)
ADD_EXECUTABLE( pAntlerTestAppC C.cpp)

# Link the executable
TARGET_LINK_LIBRARIES(pAntlerTestAppA ${MOOSLIBS} ${PLATFORM_LIBS} )
TARGET_LINK_LIBRARIES(pAntlerTestAppB ${MOOSLIBS} ${PLATFORM_LIBS} )
TARGET_LINK_LIBRARIES(pAntlerTestAppC ${MOOSLIBS} ${PLATFORM_LIBS} )



INSTALL(TARGETS
pAntlerTestAppA pAntlerTestAppB pAntlerTestAppC
RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)
