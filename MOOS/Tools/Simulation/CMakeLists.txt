#do we want to build ocean simulator?
SET(BUILD_OCEAN_SIM OFF CACHE BOOL "Build Marine Simulators?")
IF(BUILD_OCEAN_SIM)
    SET(SIM_DIRS ${SIM_DIRS} Ocean)
ENDIF(BUILD_OCEAN_SIM)

#add here for land sims..

IF (SIM_DIRS)
    SUBDIRS(${SIM_DIRS})
ENDIF (SIM_DIRS)

