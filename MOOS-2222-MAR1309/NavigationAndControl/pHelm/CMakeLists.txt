PROJECT(pHelm)

# next few lines are simply to insert icons into
# the console application on windows
IF(WIN32)
    SET(RESOURCES icon1.ico pHelm.rc)
ENDIF(WIN32)

#what files are needed?
SET(PHELM_SRCS HelmApp.cpp HelmApp.h HelmMain.cpp  ${RESOURCES})

SET( BUILD_NEWMAT ON CACHE BOOL "Build Newmat" FORCE)

INCLUDE_DIRECTORIES(${MOOS_SOURCE_DIR}/NavigationAndControl)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR} ${THIRDPARTY_DIR}/newmat)

# Add executable called pLogger
ADD_EXECUTABLE( pHelm ${PHELM_SRCS})

# indicate how to link
TARGET_LINK_LIBRARIES(pHelm MOOSTask ${MOOSLIBS} ${PLATFORM_LIBS}   newmat )


INSTALL(TARGETS
pHelm
RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)
