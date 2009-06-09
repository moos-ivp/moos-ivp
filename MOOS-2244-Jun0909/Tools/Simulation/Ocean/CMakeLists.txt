
#do we want to build multivehicle AUV simulator?
SET(BUILD_UMVS OFF CACHE BOOL "Build Marine Sim (uMVS)?")
IF(BUILD_UMVS)
    SET(OCEAN_SIM_DIRS ${OCEAN_SIM_DIRS} uMVS  )
ENDIF(BUILD_UMVS)

IF (OCEAN_SIM_DIRS)
    SUBDIRS(${OCEAN_SIM_DIRS})
ENDIF (OCEAN_SIM_DIRS)

